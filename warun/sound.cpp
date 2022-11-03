#pragma once
#include <dsound.h>
#include "packfile.cpp"

// �����Đ��N���X�B
class Sound {
public:
	static const WORD VOLUME_MAX = 5000;	// �ő剹�ʁB

	// �R���X�g���N�^�B
	// �����Fpds DirectSound�I�u�W�F�N�g�B
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

	// �f�X�g���N�^�B
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

	// �Đ��B
	// �����Fpfilename �t�@�C�����B
	//       loop_flag ���[�v�Đ�����ꍇ��true�A����ȊO��false���w�肷��B
	//       time �t�F�[�h�C������������܂ł̃t���[���P�ʂ̎��ԁB
	//       pfilename_intro �C���g���t�@�C�����B�w�肵���ꍇ�Apfilename���ʏ�Đ����AWAVE�t�H�[�}�b�g��pfilename�ƈ�v���Ȃ��Ǝ��s����B
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
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

		// �����o�b�t�@�쐬�B
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

		// ����̉����f�[�^�ǂݍ��݂͉����o�b�t�@�S�́B
		if (m_pdsb->Lock(0, 0, &pbuf, &m_buf_size[0], NULL, 0, DSBLOCK_ENTIREBUFFER) != DS_OK) return false;
		succ_flag = readWave((HPSTR)pbuf);
		if (m_pdsb->Unlock(pbuf, m_buf_size[0], NULL, 0) != DS_OK) succ_flag = false;
		if (!succ_flag) return false;

		// �����ʒu�ʒm�ݒ�B
		if (FAILED(m_pdsb->QueryInterface(IID_IDirectSoundNotify8, (LPVOID *)&pdsn))) return false;
		succ_flag = false;
		m_event = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (m_event != NULL) {
			if (isNormalPlay()) {
				dsbpn[0].dwOffset = DSBPN_OFFSETSTOP;
				dsbpn[0].hEventNotify = m_event;
				notify_cnt = 1;
			} else {
				// 2��ڈȍ~�̓ǂݍ��݃T�C�Y���ő�o�b�t�@�T�C�Y�̔����ɐݒ�B
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
		m_pdsb->Play(0, 0, m_loop_flag || !isNormalPlay() ? DSBPLAY_LOOPING : 0);	// �����Đ��B
		return true;
	}

	// ��~�B
	// �����Fevent_flag �C�x���g�ʒm���Ȃ��Ȃ�true���w�肷��B
	void Stop(bool event_flag = false) {
		m_end_flag = true;
		if (!event_flag && m_event != NULL) SetEvent(m_event);
		if (m_pdsb != NULL) m_pdsb->Stop();
	}

	// �����̃t�F�[�h�C��/�A�E�g�����B
	// �t�F�[�h�A�E�g�̏ꍇ�A�t�F�[�h�A�E�g��ɒ�~����B
	// �����Ftime �t�F�[�h�C��/�A�E�g����������܂ł̃t���[���P�ʂ̎��ԁB
	//       out_flag �t�F�[�h�C���Ȃ�false�A�t�F�[�h�A�E�g�Ȃ�true���w�肷��B
	void Fade(WORD time, bool out_flag) {
		m_fade_time_cnt = time;
		m_fade_time = 0;
		m_fade_flag = out_flag;

		if (m_fade_time_cnt > 0) {
			// �t�F�[�h�C��/�A�E�g��������ꍇ�͊J�n�l��ݒ肷��B
			m_pdsb->SetVolume(m_fade_flag ? DSBVOLUME_MAX : DSBVOLUME_MIN);
		} else {
			// �t�F�[�h�C���������Ȃ��ꍇ�͏I���l��ݒ肷��B�t�F�[�h�A�E�g�������Ȃ��ꍇ�͒�~����B
			if (m_fade_flag) Stop();
			else m_pdsb->SetVolume(DSBVOLUME_MAX);
		}
	}

	// �I���ݒ�B
	// �����Ftime �t�F�[�h�A�E�g����������܂ł̃t���[���P�ʂ̎��ԁB
	void Close(WORD time) {
		LONG volume;

		if (m_pdsb->GetVolume(&volume) != DS_OK) return;

		m_volume = VOLUME_MAX + volume;

		m_fade_time_cnt = time;
		m_fade_time = 0;
		m_fade_flag = true;
		if (m_fade_time_cnt == 0) Stop();
	}

	// ���t���[�����s����鏈���B
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

	// �����f�[�^���X�g���[�~���O�����o�b�t�@�ɑO�������ƌ㔼�����Ō��݂ɉ����f�[�^��ǂݍ��ށB
	// 2��ڈȍ~�̉����f�[�^�ǂݍ��ݏ����Ƃ��Ď��s���邱�ƁB�ʏ퉹���o�b�t�@�̏ꍇ�͎��s���Ȃ����ƁB
	// �Ԓl�F�Đ��ł���Ȃ�true�A����ȊO��false�B
	bool setStreamingBuffer() {
		bool succ_flag;
		DWORD pos, buf_size;
		LPVOID pbuf;

		if (m_end_flag) return false;
		if (!m_next_flag) {	// ����H
			m_next_flag = true;
			return true;
		}

		// ���S�ɉ����o�b�t�@�ɏ������ނ��߁A�Đ��ʒu���ړ�����܂ő҂B
		for (;;) {
			if (m_end_flag) return false;	// ���̃��[�v�������ɕʃX���b�h�ōĐ����~�����\��������̂Œ�~���Ă��Ȃ����m�F����B
			if (m_pdsb->GetCurrentPosition(&pos, NULL) != DS_OK) return false;
			if (!m_pos_flag && pos >= m_buf_size[0]) break;	// �O�����������S�ɉ����o�b�t�@�ɏ������ނ��߂ɍĐ��ʒu���㔼�����ɂȂ��Ă���B
			if (m_pos_flag && pos <= m_buf_size[0]) break;	// �㔼���������S�ɉ����o�b�t�@�ɏ������ނ��߂ɍĐ��ʒu���O�������ɂȂ��Ă���B
			Sleep(1);
		}

		// ���[�v�Đ������A�Ō�܂œǂݍ��񂾏ꍇ�͍Đ����镔�����܂��c���Ă��邽�߁A����̒ʒm�ŏI������悤�ɂ���B
		if (!m_loop_flag && !readWave(NULL)) m_end_flag = true;

		buf_size = m_buf_size[0];

		// �����f�[�^�ǂݍ��݁B
		if (m_pdsb->Lock(m_pos_flag ? m_buf_size[0] : 0, m_buf_size[0]
			, &pbuf, &m_buf_size[0], NULL, 0, 0) != DS_OK) return false;
		succ_flag = readWave((char *)pbuf);
		if (m_pdsb->Unlock(pbuf, m_buf_size[0], NULL, 0) != DS_OK) return false;
		if (!succ_flag) return false;

		m_buf_size[0] = buf_size;	// �T�C�Y���ύX����Ă���\��������̂Ŗ߂��B
		m_pos_flag = !m_pos_flag;
		return true;
	}

	// �C�x���g�n���h�����擾�B
	// �Ԓl�F�C�x���g�n���h���B
	const HANDLE getEvent() {
		return m_event;
	}

	// �ʏ�Đ����X�g���[�~���O�Đ����Ԃ��B
	// �Ԓl�F�ʏ�Đ�����true�A�X�g���[�~���O�Đ��Ȃ�false�B
	bool isNormalPlay() {
		return m_buf_size[0] >= m_data_size[0];
	}

	// �����ʒu�ʒm�ɂ�鏈���B
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
	WAVEFORMATEX m_wf;	// WAVE�t�H�[�}�b�g�B

	PackFile *m_pfile[2];	// 0:�����t�@�C���A1:�C���g�������t�@�C���B
	BYTE m_snd_idx;	// �Đ����鉹���t�@�C���C���f�b�N�X�B

	DWORD m_data_size[2];	// �����f�[�^�T�C�Y�B�����o�b�t�@�T�C�Y���傫���ꍇ�̓X�g���[�~���O�Đ����K�v�B
	DWORD m_buf_size[2];	// �����o�b�t�@�T�C�Y�B�����f�[�^�T�C�Y��菬�����ꍇ�̓X�g���[�~���O�Đ����K�v�B
	DWORD m_data_pos[2];	// �����f�[�^�̍ŏ��̈ʒu�B
	DWORD m_data_offset;	// �����f�[�^�̌��݂̈ʒu�B

	bool m_loop_flag;	// ���[�v�Đ��t���O�B
	bool m_pos_flag;	// �����o�b�t�@�̑O���������㔼�������̃t���O�B
	bool m_end_flag;	// �Đ��I���t���O�B
	bool m_next_flag;	// �X�g���[�~���O�Đ��̂��߂̃t���O�B
	HANDLE m_thread, m_event;	// �X�g���[�~���O�Đ��̂��߂̃n���h���B

	WORD m_fade_time_cnt, m_fade_time;	// �t�F�[�h�C��/�A�E�g�p�J�E���g�B
	bool m_fade_flag;	// �t�F�[�h�C��/�A�E�g�p�t���O�B
	int m_volume;	// ���ʁB

	// WAVE�t�@�C�����J���AWAVE�t�H�[�}�b�g�Ɖ����f�[�^�T�C�Y�Ɖ����o�b�t�@�T�C�Y��
	// �����f�[�^�̍ŏ��̈ʒu��ݒ肷��B�ǂݍ��񂾃t�@�C���͕��Ȃ��B
	// �����Fpfilename �t�@�C�����B
	//       snd_idx �����t�@�C���C���f�b�N�X�B0�Ȃ�WAVE�t�H�[�}�b�g��ݒ肵�A����ȊO�Ȃ�WAVE�t�H�[�}�b�g���`�F�b�N�B
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
	bool openWave(LPCSTR pfilename, BYTE snd_idx) {
		char buf[20];
		DWORD size, offset;
		WAVEFORMATEX wf;

		// WAVE�t�@�C�����J���B
		if (snd_idx > 1 || m_pfile[snd_idx] == NULL) return false;
		if (!m_pfile[snd_idx]->Open(pfilename)) return false;

		// WAVE�����擾�B
		if (m_pfile[snd_idx]->Read(buf, 20) != 20) return false;
		if (memcmp(buf, "RIFF", 4) != 0) return false;
		if (memcmp(buf + 8, "WAVEfmt ", 8) != 0) return false;
		size = *(DWORD *)(buf + 16);
		size = size < sizeof(WAVEFORMATEX) ? size : sizeof(WAVEFORMATEX);
		offset = 20;

		// WAVE�t�H�[�}�b�g���擾���A�K�؂��`�F�b�N����B
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

		// WAVE�f�[�^���擾�B
		if (m_pfile[snd_idx]->Read(buf, 8) != 8) return false;
		if (memcmp(buf, "data", 4) != 0) return false;
		size = *(DWORD *)(buf + 4);
		if (size < DSBSIZE_MIN) return false;	// �����f�[�^�T�C�Y������������̂Ŏ��s�Ƃ���B
		m_data_size[snd_idx] = size;	// �����f�[�^�T�C�Y�ݒ�B
		offset += 8;
		m_data_pos[snd_idx] = offset;	// �����f�[�^�̍ŏ��̈ʒu��ݒ�B

		// �ő�o�b�t�@�T�C�Y�ݒ�B
		m_buf_size[snd_idx] = m_wf.nChannels * m_wf.nSamplesPerSec * (m_wf.wBitsPerSample / 8) * 4;

		// WAVE�f�[�^�T�C�Y���ő�o�b�t�@�T�C�Y��菬�����Ȃ�A�o�b�t�@�T�C�Y��WAVE�f�[�^�T�C�Y�ɂ���B
		if (m_data_size[snd_idx] < m_buf_size[snd_idx]) m_buf_size[snd_idx] = m_data_size[snd_idx];

		return true;
	}

	// �J���Ă���t�@�C���̃f�[�^�̈ʒu����ǂݍ��ށB�܂��́A�ǂݍ��߂�WAVE�f�[�^�����邩�`�F�b�N�B
	// �Ō�܂œǂݍ��񂾂Ƃ��A���[�v�Đ��łȂ��ꍇ�͈ȍ~������ǂݍ��݁A���[�v�Đ��̏ꍇ�͏z����B
	// �����Fpdata �f�[�^�̓ǂݍ��ݐ�B�܂��́ANULL�B
	// �Ԓl�Fpdata��NULL�łȂ��ꍇ�A�����Ȃ�true�A����ȊO��false�B
	//       pdata��NULL�̏ꍇ�A�ǂݍ��߂�WAVE�f�[�^������Ȃ�true�A�Ō�܂œǂݍ��񂾂Ȃ�false�B
	bool readWave(char *pdata) {
		DWORD size_diff1, size_diff2;

		if (pdata == NULL) return m_data_offset < m_data_size[m_snd_idx] || m_snd_idx != 0;

		// WAVE�f�[�^��ǂݍ��ށB
		if (m_data_offset + m_buf_size[m_snd_idx] > m_data_size[m_snd_idx]) {	// �z���K�v���H
			if (m_data_offset < m_data_size[m_snd_idx]) {
				// ���݈ʒu����Ō�܂�WAVE�f�[�^��ǂݍ��ށB
				size_diff1 = m_data_size[m_snd_idx] - m_data_offset;
				if (m_pfile[m_snd_idx]->Read(pdata, size_diff1) != size_diff1) return false;
			} else {	// ���ɍŌ�܂œǂ݂���ł����B
				size_diff1 = 0;
			}

			size_diff2 = m_buf_size[m_snd_idx] - size_diff1;
			if (m_loop_flag || m_snd_idx != 0) {
				if (m_snd_idx != 0) m_snd_idx = 0;	// �C���g�����I�������̂ŁB

				// WAVE�f�[�^�̍ŏ��̈ʒu�ֈړ��B
				if (!m_pfile[m_snd_idx]->Seek(m_data_pos[m_snd_idx], SEEK_SET)) return false;

				// �c���WAVE�f�[�^��ǂݍ��ށB
				if (m_pfile[m_snd_idx]->Read(pdata + size_diff1, size_diff2) != size_diff2) return false;

				m_data_offset = size_diff2;
			} else {
				// ���[�v�Đ����Ȃ��ꍇ�A�Ō�ɖ����f�[�^��ݒ肵�Ē�~���ɋ͂��ɉ�����̂�h�~����B
				FillMemory(pdata + size_diff1, size_diff2, m_wf.wBitsPerSample == 8 ? 128 : 0);
				m_data_offset = m_data_size[m_snd_idx];
			}
		} else {
			// ���݈ʒu���特���o�b�t�@�T�C�Y�̕�����WAVE�f�[�^��ǂݍ��ށB
			if (m_pfile[m_snd_idx]->Read(pdata, m_buf_size[m_snd_idx]) != m_buf_size[m_snd_idx]) return false;

			m_data_offset += m_buf_size[m_snd_idx];

			if (m_snd_idx == 1) {
				size_diff1 = m_buf_size[0] - m_buf_size[1];
				if (size_diff1 > 0) {
					// �C���g���͑S�ēǂݍ��񂾂��A�܂��o�b�t�@���]���Ă���̂ŁB
					m_snd_idx = 0;
					if (m_pfile[0]->Read(pdata + m_buf_size[1], size_diff1) != size_diff1) return false;
					m_data_offset = size_diff1;
				}
			}
		}

		return true;
	}
};
