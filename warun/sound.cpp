#pragma once
#include <dsound.h>
#include "packfile.cpp"

// 音声再生クラス。
class Sound {
public:
	static const WORD VOLUME_MAX = 5000;	// 最大音量。

	// コンストラクタ。
	// 引数：pds DirectSoundオブジェクト。
	Sound(LPDIRECTSOUND8 pds) {
		m_pds = pds;
		m_pdsb = NULL;
		ZeroMemory(&m_wf, sizeof(m_wf));

		m_pfile[0] = new PackFile(0);
		m_pfile[1] = new PackFile(0);
		m_snd_idx = 0;

		m_data_size[0] = m_data_size[1] = 0;
		m_buf_size[0] = m_buf_size[1] = 0;
		m_data_pos[0] = m_data_pos[1] = 0;
		m_data_offset = 0;

		m_loop_flag = m_pos_flag = m_end_flag = m_next_flag = false;
		m_thread = m_event = NULL;

		m_fade_time_cnt = m_fade_time = 0;
		m_fade_flag = false;
		m_volume = VOLUME_MAX;
	}

	// デストラクタ。
	~Sound() {
		Stop();
		if (m_event != NULL) CloseHandle(m_event);
		if (m_thread != NULL) {
			WaitForSingleObject(m_thread, INFINITE);
			CloseHandle(m_thread);
		}
		if (m_pfile[0] != NULL) delete m_pfile[0];
		if (m_pfile[1] != NULL) delete m_pfile[1];
		if (m_pdsb != NULL) m_pdsb->Release();
	}

	// 再生。
	// 引数：pfilename ファイル名。
	//       loop_flag ループ再生する場合はtrue、それ以外はfalseを指定する。
	//       time フェードインが完了するまでのフレーム単位の時間。
	//       pfilename_intro イントロファイル名。指定した場合、pfilenameが通常再生か、WAVEフォーマットがpfilenameと一致しないと失敗する。
	// 返値：成功ならtrue、それ以外はfalse。
	bool Play(LPCSTR pfilename, bool loop_flag, WORD time, LPCSTR pfilename_intro) {
		LPDIRECTSOUNDBUFFER pdsb_p;
		DSBUFFERDESC dsbd;
		LPDIRECTSOUNDNOTIFY pdsn;
		DSBPOSITIONNOTIFY dsbpn[2];
		LPVOID pbuf;
		BYTE notify_cnt;
		bool succ_flag;

		if (!openWave(pfilename, 0)) return false;
		if (pfilename_intro != NULL) {
			if (isNormalPlay()) return false;
			if (!openWave(pfilename_intro, 1)) return false;
			m_snd_idx = 1;
		}

		// 音声バッファ作成。
		dsbd.dwSize = sizeof(dsbd);
		dsbd.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2;
		dsbd.dwBufferBytes = m_buf_size[0];
		dsbd.dwReserved = 0;
		dsbd.lpwfxFormat = &m_wf;
		dsbd.guid3DAlgorithm = GUID_NULL;
		if (m_pds->CreateSoundBuffer(&dsbd, &pdsb_p, NULL) != DS_OK) return false;
		succ_flag = SUCCEEDED(pdsb_p->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID *)&m_pdsb));
		pdsb_p->Release();
		if (!succ_flag) return false;

		// 初回の音声データ読み込みは音声バッファ全体。
		if (m_pdsb->Lock(0, 0, &pbuf, &m_buf_size[0], NULL, 0, DSBLOCK_ENTIREBUFFER) != DS_OK) return false;
		succ_flag = readWave((HPSTR)pbuf);
		if (m_pdsb->Unlock(pbuf, m_buf_size[0], NULL, 0) != DS_OK) succ_flag = false;
		if (!succ_flag) return false;

		// 音声位置通知設定。
		if (FAILED(m_pdsb->QueryInterface(IID_IDirectSoundNotify8, (LPVOID *)&pdsn))) return false;
		succ_flag = false;
		m_event = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (m_event != NULL) {
			if (isNormalPlay()) {
				dsbpn[0].dwOffset = DSBPN_OFFSETSTOP;
				dsbpn[0].hEventNotify = m_event;
				notify_cnt = 1;
			} else {
				// 2回目以降の読み込みサイズを最大バッファサイズの半分に設定。
				if (m_buf_size[1] >= m_buf_size[0]) m_buf_size[1] = (float)m_buf_size[1] / 2 + 0.5;
				m_buf_size[0] = (float)m_buf_size[0] / 2 + 0.5;

				dsbpn[0].dwOffset = 0;
				dsbpn[0].hEventNotify = m_event;
				dsbpn[1].dwOffset = m_buf_size[0];
				dsbpn[1].hEventNotify = m_event;
				notify_cnt = 2;
			}
			if (pdsn->SetNotificationPositions(notify_cnt, dsbpn) == DS_OK) succ_flag = true;
		}
		pdsn->Release();
		if (!succ_flag) return false;
		m_thread = CreateThread(NULL, 0, procSoundPos, this, 0, NULL);
		if (m_thread == NULL) return false;

		Fade(time, false);
		m_loop_flag = loop_flag;
		m_pdsb->Play(0, 0, m_loop_flag || !isNormalPlay() ? DSBPLAY_LOOPING : 0);	// 音声再生。
		return true;
	}

	// 停止。
	// 引数：event_flag イベント通知しないならtrueを指定する。
	void Stop(bool event_flag = false) {
		m_end_flag = true;
		if (!event_flag && m_event != NULL) SetEvent(m_event);
		if (m_pdsb != NULL) m_pdsb->Stop();
	}

	// 音声のフェードイン/アウト処理。
	// フェードアウトの場合、フェードアウト後に停止する。
	// 引数：time フェードイン/アウトが完了するまでのフレーム単位の時間。
	//       out_flag フェードインならfalse、フェードアウトならtrueを指定する。
	void Fade(WORD time, bool out_flag) {
		m_fade_time_cnt = time;
		m_fade_time = 0;
		m_fade_flag = out_flag;

		if (m_fade_time_cnt > 0) {
			// フェードイン/アウト処理する場合は開始値を設定する。
			m_pdsb->SetVolume(m_fade_flag ? DSBVOLUME_MAX : DSBVOLUME_MIN);
		} else {
			// フェードイン処理しない場合は終了値を設定する。フェードアウト処理しない場合は停止する。
			if (m_fade_flag) Stop();
			else m_pdsb->SetVolume(DSBVOLUME_MAX);
		}
	}

	// 終了設定。
	// 引数：time フェードアウトが完了するまでのフレーム単位の時間。
	void Close(WORD time) {
		LONG volume;

		if (m_pdsb->GetVolume(&volume) != DS_OK) return;

		m_volume = VOLUME_MAX + volume;

		m_fade_time_cnt = time;
		m_fade_time = 0;
		m_fade_flag = true;
		if (m_fade_time_cnt == 0) Stop();
	}

	// 毎フレーム実行される処理。
	void Process() {
		WORD fade_time;

		if (m_fade_time >= m_fade_time_cnt) return;
		m_fade_time++;

		if (m_fade_flag) {
			if (m_fade_time >= m_fade_time_cnt) {
				Stop();
				return;
			}
			fade_time = m_fade_time_cnt - m_fade_time;
		} else {
			fade_time = m_fade_time;
		}

		m_pdsb->SetVolume((float)fade_time / m_fade_time_cnt * m_volume - VOLUME_MAX);
	}

	// 音声データをストリーミング音声バッファに前半部分と後半部分で交互に音声データを読み込む。
	// 2回目以降の音声データ読み込み処理として実行すること。通常音声バッファの場合は実行しないこと。
	// 返値：再生できるならtrue、それ以外はfalse。
	bool setStreamingBuffer() {
		bool succ_flag;
		DWORD pos, buf_size;
		LPVOID pbuf;

		if (m_end_flag) return false;
		if (!m_next_flag) {	// 初回？
			m_next_flag = true;
			return true;
		}

		// 安全に音声バッファに書き込むため、再生位置が移動するまで待つ。
		for (;;) {
			if (m_end_flag) return false;	// このループ処理中に別スレッドで再生を停止される可能性があるので停止していないか確認する。
			if (m_pdsb->GetCurrentPosition(&pos, NULL) != DS_OK) return false;
			if (!m_pos_flag && pos >= m_buf_size[0]) break;	// 前半部分を安全に音声バッファに書き込むために再生位置が後半部分になってから。
			if (m_pos_flag && pos <= m_buf_size[0]) break;	// 後半部分を安全に音声バッファに書き込むために再生位置が前半部分になってから。
			Sleep(1);
		}

		// ループ再生せず、最後まで読み込んだ場合は再生する部分がまだ残っているため、次回の通知で終了するようにする。
		if (!m_loop_flag && !readWave(NULL)) m_end_flag = true;

		buf_size = m_buf_size[0];

		// 音声データ読み込み。
		if (m_pdsb->Lock(m_pos_flag ? m_buf_size[0] : 0, m_buf_size[0]
			, &pbuf, &m_buf_size[0], NULL, 0, 0) != DS_OK) return false;
		succ_flag = readWave((char *)pbuf);
		if (m_pdsb->Unlock(pbuf, m_buf_size[0], NULL, 0) != DS_OK) return false;
		if (!succ_flag) return false;

		m_buf_size[0] = buf_size;	// サイズが変更されている可能性があるので戻す。
		m_pos_flag = !m_pos_flag;
		return true;
	}

	// イベントハンドルを取得。
	// 返値：イベントハンドル。
	const HANDLE getEvent() {
		return m_event;
	}

	// 通常再生かストリーミング再生か返す。
	// 返値：通常再生からtrue、ストリーミング再生ならfalse。
	bool isNormalPlay() {
		return m_buf_size[0] >= m_data_size[0];
	}

	// 音声位置通知による処理。
	static DWORD WINAPI procSoundPos(LPVOID lpParameter) {
		Sound *psound = (Sound *)lpParameter;
		bool normal_play_flag = psound->isNormalPlay(), end_flag = false;
		HANDLE event = psound->getEvent();

		while (!end_flag) {
			switch (WaitForSingleObject(event, INFINITE)) {
			case WAIT_OBJECT_0:
				if (normal_play_flag) {
					end_flag = true;
					break;
				}

				if (!psound->setStreamingBuffer()) end_flag = true;
				break;
			default:
				end_flag = true;
				break;
			}
		}

		psound->Stop(true);
		return 0;
	}

protected:
	LPDIRECTSOUND8 m_pds;
	LPDIRECTSOUNDBUFFER8 m_pdsb;
	WAVEFORMATEX m_wf;	// WAVEフォーマット。

	PackFile *m_pfile[2];	// 0:音声ファイル、1:イントロ音声ファイル。
	BYTE m_snd_idx;	// 再生する音声ファイルインデックス。

	DWORD m_data_size[2];	// 音声データサイズ。音声バッファサイズより大きい場合はストリーミング再生が必要。
	DWORD m_buf_size[2];	// 音声バッファサイズ。音声データサイズより小さい場合はストリーミング再生が必要。
	DWORD m_data_pos[2];	// 音声データの最初の位置。
	DWORD m_data_offset;	// 音声データの現在の位置。

	bool m_loop_flag;	// ループ再生フラグ。
	bool m_pos_flag;	// 音声バッファの前半部分か後半部分かのフラグ。
	bool m_end_flag;	// 再生終了フラグ。
	bool m_next_flag;	// ストリーミング再生のためのフラグ。
	HANDLE m_thread, m_event;	// ストリーミング再生のためのハンドル。

	WORD m_fade_time_cnt, m_fade_time;	// フェードイン/アウト用カウント。
	bool m_fade_flag;	// フェードイン/アウト用フラグ。
	int m_volume;	// 音量。

	// WAVEファイルを開き、WAVEフォーマットと音声データサイズと音声バッファサイズと
	// 音声データの最初の位置を設定する。読み込んだファイルは閉じない。
	// 引数：pfilename ファイル名。
	//       snd_idx 音声ファイルインデックス。0ならWAVEフォーマットを設定し、それ以外ならWAVEフォーマットをチェック。
	// 返値：成功ならtrue、それ以外はfalse。
	bool openWave(LPCSTR pfilename, BYTE snd_idx) {
		char buf[20];
		DWORD size, offset;
		WAVEFORMATEX wf;

		// WAVEファイルを開く。
		if (snd_idx > 1 || m_pfile[snd_idx] == NULL) return false;
		if (!m_pfile[snd_idx]->Open(pfilename)) return false;

		// WAVE情報を取得。
		if (m_pfile[snd_idx]->Read(buf, 20) != 20) return false;
		if (memcmp(buf, "RIFF", 4) != 0) return false;
		if (memcmp(buf + 8, "WAVEfmt ", 8) != 0) return false;
		size = *(DWORD *)(buf + 16);
		size = size < sizeof(WAVEFORMATEX) ? size : sizeof(WAVEFORMATEX);
		offset = 20;

		// WAVEフォーマットを取得し、適切かチェックする。
		if (snd_idx == 0) {
			if (m_pfile[snd_idx]->Read(&m_wf, size) != size) return false;
			if (m_wf.wFormatTag != WAVE_FORMAT_PCM) return false;
			if (m_wf.nChannels != 1 && m_wf.nChannels != 2) return false;
			if (m_wf.nSamplesPerSec < 8000 || m_wf.nSamplesPerSec > 48000) return false;
			if (m_wf.wBitsPerSample != 8 && m_wf.wBitsPerSample != 16) return false;
			m_wf.cbSize = 0;
		} else {
			if (m_pfile[snd_idx]->Read(&wf, size) != size) return false;
			wf.cbSize = 0;
			if (memcmp(&wf, &m_wf, sizeof(m_wf)) != 0) return false;
		}
		offset += size;

		// WAVEデータ情報取得。
		if (m_pfile[snd_idx]->Read(buf, 8) != 8) return false;
		if (memcmp(buf, "data", 4) != 0) return false;
		size = *(DWORD *)(buf + 4);
		if (size < DSBSIZE_MIN) return false;	// 音声データサイズが小さすぎるので失敗とする。
		m_data_size[snd_idx] = size;	// 音声データサイズ設定。
		offset += 8;
		m_data_pos[snd_idx] = offset;	// 音声データの最初の位置を設定。

		// 最大バッファサイズ設定。
		m_buf_size[snd_idx] = m_wf.nChannels * m_wf.nSamplesPerSec * (m_wf.wBitsPerSample / 8) * 4;

		// WAVEデータサイズが最大バッファサイズより小さいなら、バッファサイズはWAVEデータサイズにする。
		if (m_data_size[snd_idx] < m_buf_size[snd_idx]) m_buf_size[snd_idx] = m_data_size[snd_idx];

		return true;
	}

	// 開いているファイルのデータの位置から読み込む。または、読み込めるWAVEデータがあるかチェック。
	// 最後まで読み込んだとき、ループ再生でない場合は以降無音を読み込み、ループ再生の場合は循環する。
	// 引数：pdata データの読み込み先。または、NULL。
	// 返値：pdataがNULLでない場合、成功ならtrue、それ以外はfalse。
	//       pdataがNULLの場合、読み込めるWAVEデータがあるならtrue、最後まで読み込んだならfalse。
	bool readWave(char *pdata) {
		DWORD size_diff1, size_diff2;

		if (pdata == NULL) return m_data_offset < m_data_size[m_snd_idx] || m_snd_idx != 0;

		// WAVEデータを読み込む。
		if (m_data_offset + m_buf_size[m_snd_idx] > m_data_size[m_snd_idx]) {	// 循環が必要か？
			if (m_data_offset < m_data_size[m_snd_idx]) {
				// 現在位置から最後までWAVEデータを読み込む。
				size_diff1 = m_data_size[m_snd_idx] - m_data_offset;
				if (m_pfile[m_snd_idx]->Read(pdata, size_diff1) != size_diff1) return false;
			} else {	// 既に最後まで読みこんでいた。
				size_diff1 = 0;
			}

			size_diff2 = m_buf_size[m_snd_idx] - size_diff1;
			if (m_loop_flag || m_snd_idx != 0) {
				if (m_snd_idx != 0) m_snd_idx = 0;	// イントロが終了したので。

				// WAVEデータの最初の位置へ移動。
				if (!m_pfile[m_snd_idx]->Seek(m_data_pos[m_snd_idx], SEEK_SET)) return false;

				// 残りのWAVEデータを読み込む。
				if (m_pfile[m_snd_idx]->Read(pdata + size_diff1, size_diff2) != size_diff2) return false;

				m_data_offset = size_diff2;
			} else {
				// ループ再生しない場合、最後に無音データを設定して停止時に僅かに音が鳴るのを防止する。
				FillMemory(pdata + size_diff1, size_diff2, m_wf.wBitsPerSample == 8 ? 128 : 0);
				m_data_offset = m_data_size[m_snd_idx];
			}
		} else {
			// 現在位置から音声バッファサイズの分だけWAVEデータを読み込む。
			if (m_pfile[m_snd_idx]->Read(pdata, m_buf_size[m_snd_idx]) != m_buf_size[m_snd_idx]) return false;

			m_data_offset += m_buf_size[m_snd_idx];

			if (m_snd_idx == 1) {
				size_diff1 = m_buf_size[0] - m_buf_size[1];
				if (size_diff1 > 0) {
					// イントロは全て読み込んだが、まだバッファが余っているので。
					m_snd_idx = 0;
					if (m_pfile[0]->Read(pdata + m_buf_size[1], size_diff1) != size_diff1) return false;
					m_data_offset = size_diff1;
				}
			}
		}

		return true;
	}
};
