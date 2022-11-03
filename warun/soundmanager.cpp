#pragma once
#include "sound.cpp"

// 音声管理クラス。
class SoundManager {
public:
	static const BYTE SOUND_CNT = 10;	// 音声の数。

	// オブジェクト取得。
	static SoundManager *getObject() {
		static SoundManager snd_mng;
		return &snd_mng;
	}

	// デストラクタ。
	~SoundManager() {
		BYTE i;

		for (i = 0; i < SOUND_CNT; i++) {
			if (m_psound[i] != NULL) delete m_psound[i];
		}
		if (m_pds != NULL) m_pds->Release();
	}

	// 作成。
	// 引数：hwnd ウィンドウハンドル。
	// 返値：成功ならtrue、それ以外はfalse。
	bool Create(HWND hwnd) {
		if (DirectSoundCreate8(&DSDEVID_DefaultPlayback, &m_pds, NULL) != DS_OK) return false;
		if (m_pds->SetCooperativeLevel(hwnd, DSSCL_PRIORITY) != DS_OK) return false;
		return true;
	}

	// 再生。
	// 引数：snd_idx 音声バッファ番号。
	//       pfilename ファイル名。
	//       loop_flag ループ再生する場合はtrue、それ以外はfalseを指定する。
	//       time フェードインが完了するまでのフレーム単位の時間。
	//       pfilename_intro イントロファイル名。
	void Play(BYTE snd_idx, LPCSTR pfilename, bool loop_flag = false, WORD time = 0, LPSTR pfilename_intro = NULL) {
		Sound **ppsound;

		if (snd_idx >= SOUND_CNT || m_close) return;
		ppsound = &m_psound[snd_idx];

		if (*ppsound != NULL) delete *ppsound;	// 作成済みの場合はまず削除。

		*ppsound = new Sound(m_pds);
		if (*ppsound == NULL || !(*ppsound)->Play(pfilename, loop_flag, time, pfilename_intro)) m_error = true;
	}

	// 停止。
	// 引数：snd_idx 音声バッファ番号。
	//       time フェードアウトが完了するまでのフレーム単位の時間。
	void Stop(BYTE snd_idx, WORD time = 0) {
		Sound *psound;

		if (snd_idx >= SOUND_CNT) return;
		psound = m_psound[snd_idx];
		if (psound != NULL) psound->Fade(time, true);
	}

	// 終了設定。
	// 引数：time フェードアウトが完了するまでのフレーム単位の時間。
	//       flag trueで終了、falseで解除(timeは無視される)。
	void Close(WORD time, bool flag) {
		BYTE i;

		m_close = flag;
		if (!m_close) return;

		for (i = 0; i < SOUND_CNT; i++) {
			if (i == 9) continue;	// ボタン系の動作による音声は対象外とする。
			if (m_psound[i] != NULL) m_psound[i]->Close(time);
		}
	}

	// 毎フレーム実行される処理。
	// 返値：成功ならtrue、それ以外はfalse。
	bool Process() {
		BYTE i;

		if (m_error) return false;

		for (i = 0; i < SOUND_CNT; i++) {
			if (m_psound[i] != NULL) m_psound[i]->Process();
		}
		return true;
	}

protected:
	LPDIRECTSOUND8 m_pds;
	Sound *m_psound[SOUND_CNT];
	bool m_close, m_error;

	// コンストラクタ。
	SoundManager() {
		BYTE i;

		m_pds = NULL;
		for (i = 0; i < SOUND_CNT; i++) m_psound[i] = NULL;
		m_close = m_error = false;
	}
};
