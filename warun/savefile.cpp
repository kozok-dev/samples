#pragma once
#include <stdio.h>
#include <d3d9.h>
#include "cryptblowfish.cpp"
#include "md5.cpp"
#define SCR_SAVE_FILE "warun.sav"
#define SCR_PLAY_FILE "warun.tmp"
#define SCR_SIGN "WARUNSAVE\0"

// �Z�[�u�f�[�^�N���X�B
class SaveFile {
public:
	static const BYTE SAVE_CNT = 16;	// �Z�[�u�ł��鐔�B

	// �I�u�W�F�N�g�擾�B
	static SaveFile *getObject() {
		static SaveFile sf;
		return &sf;
	}

	// �f�X�g���N�^�B
	~SaveFile() {
		unlink(SCR_PLAY_FILE);
	}

	// �Z�[�u�f�[�^��ۑ��B
	// �����Fidx �Z�[�u�̔ԍ�(�L�����̃Q�[���N���A��Ԃ�����ύX�������Ȃ畉�̒l���w�肷��)�B
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
	bool Save(short idx) {
		BYTE data[16], *pdata;
		FILE *pfile;
		SYSTEMTIME st;
		SaveData sd;
		MD5 md5;

		pfile = fopen(SCR_SAVE_FILE ".tmp", "wb");
		if (pfile == NULL) return false;

		// �Í����f�[�^�̈���m�ہB
		pdata = new BYTE[sizeof(m_ss)];
		if (pdata == NULL) return false;

		if (idx >= 0 && idx < SAVE_CNT) {
			// ���ݎ������擾���Đݒ�B
			GetLocalTime(&st);
			wsprintfW(m_ssp.sd.date, L"%d-%02d-%02d %02d:%02d:%02d"
				, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

			sd = m_ss.sd[idx];
			m_ss.sd[idx] = m_ssp.sd;
		}
		CopyMemory(pdata, &m_ss, sizeof(m_ss));
		Encrypt(pdata, sizeof(m_ss));

		// ���f�[�^��MD5���v�Z�B
		md5.Init();
		md5.Update((BYTE *)&m_ss, sizeof(m_ss));
		md5.Final(data);

		// �V�O�j�`���ƈÍ��������f�[�^�ƌ��f�[�^��MD5���������ށB
		if (fwrite(SCR_SIGN, 10, 1, pfile) != 1 || fwrite(pdata, sizeof(m_ss), 1, pfile) != 1
			|| fwrite(data, sizeof(data), 1, pfile) != 1) {

			delete[] pdata;
			fclose(pfile);
			unlink(SCR_SAVE_FILE ".tmp");
			if (idx >= 0 && idx < SAVE_CNT) m_ss.sd[idx] = sd;
			return false;
		}

		delete[] pdata;
		fclose(pfile);
		unlink(SCR_SAVE_FILE);
		if (rename(SCR_SAVE_FILE ".tmp", SCR_SAVE_FILE) != 0) {
			unlink(SCR_SAVE_FILE ".tmp");
			if (idx >= 0 && idx < SAVE_CNT) m_ss.sd[idx] = sd;
			return false;
		}
		return true;
	}

	// �v���C���̃f�[�^��ۑ�/�폜�B
	// �����Fdel_flag false�Ȃ�ۑ��Atrue�Ȃ�폜�B
	void savePlaying(bool del_flag) {
		BYTE data[16], *pdata;
		FILE *pfile;
		MD5 md5;

		if (del_flag) {
			unlink(SCR_PLAY_FILE);
			return;
		}

		pfile = fopen(SCR_PLAY_FILE, "wb");
		if (pfile == NULL) return;

		// �Í����f�[�^�̈���m�ۂ��Đݒ肵�A�Í�������B
		pdata = new BYTE[sizeof(m_ssp)];
		if (pdata == NULL) return;
		CopyMemory(pdata, &m_ssp, sizeof(m_ssp));
		Encrypt(pdata, sizeof(m_ssp));

		// ���f�[�^��MD5���v�Z�B
		md5.Init();
		md5.Update((BYTE *)&m_ssp, sizeof(m_ssp));
		md5.Final(data);

		fwrite(SCR_SIGN, 10, 1, pfile);	// �V�O�j�`�����������ށB
		fwrite(pdata, sizeof(m_ssp), 1, pfile);	// �Í��������f�[�^���������ށB
		fwrite(data, sizeof(data), 1, pfile);	// ���f�[�^��MD5���������ށB
		delete[] pdata;
		fclose(pfile);
	}

protected:
	struct SaveData {	// �X�N���v�g�̃Z�[�u�f�[�^�̃N���X�B
		BYTE chara;	// ��l����ʁB
		UINT pos;	// �X�N���v�g�̉��Ԗڂ�WAIT��SELECT��(1�ȏ�Ȃ�Z�[�u�f�[�^�Ƃ��ėL���Ƃ���)�B
		WCHAR bg_name[32];	// �w�i���B
		WCHAR day[16];	// ���B
		WCHAR date[32];	// ���t�B

		D3DCOLOR bg_color;	// �w�i�F�B
		char bg_filename[MAX_PATH];	// �w�i�摜�B

		char chara_filename1[MAX_PATH], chara_filename2[MAX_PATH];	// �L�����B
		short chara_pos1, chara_pos2;	// �L�����̕\��X�ʒu�B

		D3DCOLOR color;	// �L��������̔w�i�F�B

		WCHAR text[512];	// ���́B
		WCHAR name[32];	// ���̖͂��O�B
		BYTE bracket;	// ���ʎ�ʁB

		char bgm_filename[MAX_PATH], bgm_intro_filename[MAX_PATH];	// BGM�B

		char scr_var;	// �X�N���v�g�p�ϐ��B
	};

	struct ScriptSave {	// �Z�[�u�f�[�^�̃N���X(�T�C�Y��8�̔{��)�B
		BYTE chara[5];	// �L�����̃Q�[���N���A��ԁB
		SaveData sd[SAVE_CNT];	// �X�N���v�g�̃Z�[�u�f�[�^�B
	};

	struct ScriptSavePlaying {	// �v���C���̃Z�[�u�f�[�^�̃N���X(�T�C�Y��8�̔{��)�B
		SaveData sd;
		int dmy;	// 8�̔{���ɂ��邽�߂̃_�~�[�ϐ��B
	};

	// �R���X�g���N�^�B
	SaveFile() {
		Load(SCR_SAVE_FILE, &m_ss, sizeof(m_ss));	// �Z�[�u�f�[�^��ǂݍ��ށB
		Load(SCR_PLAY_FILE, &m_ssp, sizeof(m_ssp));	// �v���C���������f�[�^��ǂݍ��ށB
	}

	// �Z�[�u�f�[�^��ǂݍ��ށB�ǂݍ��߂Ȃ������珉��������B
	// �����Fpfilename �t�@�C�����B
	//       pdata ScriptSave�^��ScriptSavePlaying�^�̃f�[�^�B
	//       len �f�[�^���B
	void Load(LPCSTR pfilename, void *pdata, UINT len) {
		bool flag = false;
		BYTE data1[16], data2[16];
		DWORD vec1_0, vec1_1, vec2_0, vec2_1, *pbuf, *plen;
		FILE *pfile;
		CryptBlowfish *pcb;
		MD5 md5;

		pfile = fopen(pfilename, "rb");
		if (pfile == NULL) goto procend;

		// �V�O�j�`���m�F�B
		if (fread(data1, 10, 1, pfile) != 1) goto procend;
		if (memcmp(data1, SCR_SIGN, 10) != 0) goto procend;

		if (fread(pdata, len, 1, pfile) != 1) goto procend;
		if (fread(data1, sizeof(data1), 1, pfile) != 1) goto procend;

		// �������B
		pcb = CryptBlowfish::getObject();
		pbuf = (DWORD *)pdata;
		plen = (DWORD *)((char *)pdata + len);
		vec1_0 = pcb->m_vec[0];
		vec1_1 = pcb->m_vec[1];
		for (; pbuf < plen; pbuf += 2) {
			vec2_0 = *pbuf;
			vec2_1 = pbuf[1];

			pcb->Decrypt(pbuf, pbuf + 1);
			*pbuf ^= vec1_0;
			pbuf[1] ^= vec1_1;

			vec1_0 = vec2_0;
			vec1_1 = vec2_1;
		}

		// �����������f�[�^��MD5���v�Z���Ċm�F�B
		md5.Init();
		md5.Update((BYTE *)pdata, len);
		md5.Final(data2);
		if (memcmp(data1, data2, sizeof(data1)) != 0) goto procend;

		flag = true;
procend:
		if (pfile != NULL) fclose(pfile);
		if (!flag) ZeroMemory(pdata, len);
	}

	// �Í����B
	// �����Fpdata �f�[�^�B
	//       len �f�[�^���B
	void Encrypt(void *pdata, UINT len) {
		DWORD vec1, vec2, *pbuf, *plen;
		CryptBlowfish *pcb = CryptBlowfish::getObject();

		pbuf = (DWORD *)pdata;
		plen = (DWORD *)((char *)pdata + len);

		vec1 = pcb->m_vec[0];
		vec2 = pcb->m_vec[1];
		for (; pbuf < plen; pbuf += 2) {
			*pbuf ^= vec1;
			pbuf[1] ^= vec2;

			pcb->Encrypt(pbuf, pbuf + 1);

			vec1 = *pbuf;
			vec2 = pbuf[1];
		}
	}

public:
	ScriptSave m_ss;
	ScriptSavePlaying m_ssp;
};
