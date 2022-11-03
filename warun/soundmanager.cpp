#pragma once
#include "sound.cpp"

// �����Ǘ��N���X�B
class SoundManager {
public:
	static const BYTE SOUND_CNT = 10;	// �����̐��B

	// �I�u�W�F�N�g�擾�B
	static SoundManager *getObject() {
		static SoundManager snd_mng;
		return &snd_mng;
	}

	// �f�X�g���N�^�B
	~SoundManager() {
		BYTE i;

		for (i = 0; i < SOUND_CNT; i++) {
			if (m_psound[i] != NULL) delete m_psound[i];
		}
		if (m_pds != NULL) m_pds->Release();
	}

	// �쐬�B
	// �����Fhwnd �E�B���h�E�n���h���B
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
	bool Create(HWND hwnd) {
		if (DirectSoundCreate8(&DSDEVID_DefaultPlayback, &m_pds, NULL) != DS_OK) return false;
		if (m_pds->SetCooperativeLevel(hwnd, DSSCL_PRIORITY) != DS_OK) return false;
		return true;
	}

	// �Đ��B
	// �����Fsnd_idx �����o�b�t�@�ԍ��B
	//       pfilename �t�@�C�����B
	//       loop_flag ���[�v�Đ�����ꍇ��true�A����ȊO��false���w�肷��B
	//       time �t�F�[�h�C������������܂ł̃t���[���P�ʂ̎��ԁB
	//       pfilename_intro �C���g���t�@�C�����B
	void Play(BYTE snd_idx, LPCSTR pfilename, bool loop_flag = false, WORD time = 0, LPSTR pfilename_intro = NULL) {
		Sound **ppsound;

		if (snd_idx >= SOUND_CNT || m_close) return;
		ppsound = &m_psound[snd_idx];

		if (*ppsound != NULL) delete *ppsound;	// �쐬�ς݂̏ꍇ�͂܂��폜�B

		*ppsound = new Sound(m_pds);
		if (*ppsound == NULL || !(*ppsound)->Play(pfilename, loop_flag, time, pfilename_intro)) m_error = true;
	}

	// ��~�B
	// �����Fsnd_idx �����o�b�t�@�ԍ��B
	//       time �t�F�[�h�A�E�g����������܂ł̃t���[���P�ʂ̎��ԁB
	void Stop(BYTE snd_idx, WORD time = 0) {
		Sound *psound;

		if (snd_idx >= SOUND_CNT) return;
		psound = m_psound[snd_idx];
		if (psound != NULL) psound->Fade(time, true);
	}

	// �I���ݒ�B
	// �����Ftime �t�F�[�h�A�E�g����������܂ł̃t���[���P�ʂ̎��ԁB
	//       flag true�ŏI���Afalse�ŉ���(time�͖��������)�B
	void Close(WORD time, bool flag) {
		BYTE i;

		m_close = flag;
		if (!m_close) return;

		for (i = 0; i < SOUND_CNT; i++) {
			if (i == 9) continue;	// �{�^���n�̓���ɂ�鉹���͑ΏۊO�Ƃ���B
			if (m_psound[i] != NULL) m_psound[i]->Close(time);
		}
	}

	// ���t���[�����s����鏈���B
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
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

	// �R���X�g���N�^�B
	SoundManager() {
		BYTE i;

		m_pds = NULL;
		for (i = 0; i < SOUND_CNT; i++) m_psound[i] = NULL;
		m_close = m_error = false;
	}
};
