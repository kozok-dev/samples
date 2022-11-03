#pragma once
#include "sprite.cpp"
#include "soundmanager.cpp"

// �摜�X�v���C�g�N���X�B
class SpriteImg : public Sprite {
public:
	// �F�X�v���C�g�R���X�g���N�^�B
	// �����Fpd3dd Direct3D�f�o�C�X�I�u�W�F�N�g�B
	//       color �F�B
	//       pos_x X�ʒu�B
	//       pos_y Y�ʒu�B
	//       width ���B
	//       height �����B
	SpriteImg(LPDIRECT3DDEVICE9 pd3dd, D3DCOLOR color, short pos_x, short pos_y, WORD width, WORD height)
		: Sprite(pd3dd) {

		DWORD i, j, *pbuf;
		D3DLOCKED_RECT d3dlr;

		m_pd3dt_sel = m_pd3dt_push = NULL;
		m_spr_idx = 0;
		m_psnd_mng = NULL;
		m_snd_filename_sel[0] = m_snd_filename_push[0] = '\0';

		// ��X�v���C�g�쐬�B
		for (m_scr_width = 1; m_scr_width < width; m_scr_width <<= 1);
		for (m_scr_height = 1; m_scr_height < height; m_scr_height <<= 1);
		if (FAILED(m_pd3dd->CreateTexture(m_scr_width, m_scr_height, 1, 0
			, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pd3dt, NULL))) return;

		m_pos_x = pos_x;
		m_pos_y = pos_y;
		m_width = width;
		m_height = height;

		if (FAILED(m_pd3dt->LockRect(0, &d3dlr, NULL, D3DLOCK_DISCARD))) return;

		// �h��Ԃ��B
		pbuf = (DWORD *)d3dlr.pBits;
		for (i = 0; i < m_height; i++) {
			for (j = 0; j < m_width; j++) *pbuf++ = color;
			pbuf += m_scr_width - m_width;
		}

		m_pd3dt->UnlockRect(0);
	}

	// �摜�X�v���C�g�R���X�g���N�^�B
	// �����Fpd3dd Direct3D�f�o�C�X�I�u�W�F�N�g�B
	//       pfilename �t�@�C�����B
	//       pfilename_sel �}�E�X�I�[�o�[���̃t�@�C�����B
	//       pfilename_push �}�E�X�{�^���������̃t�@�C�����B
	//       pos_x X�ʒu�B
	//       pos_y Y�ʒu�B
	//       id �X�v���C�gID�B
	//       psnd_mng �����Ǘ��I�u�W�F�N�g�B
	//       psnd_filename_sel �}�E�X�I�[�o�[���̉����t�@�C�����B
	//       psnd_filename_push �}�E�X�{�^���������̉����t�@�C�����B
	SpriteImg(LPDIRECT3DDEVICE9 pd3dd, LPCSTR pfilename, LPCSTR pfilename_sel
		, LPCSTR pfilename_push, short pos_x, short pos_y, BYTE id, SoundManager *psnd_mng
		, LPCSTR psnd_filename_sel, LPCSTR psnd_filename_push) : Sprite(pd3dd) {

		BITMAPINFOHEADER bih;

		m_id = id;
		m_pd3dt_sel = m_pd3dt_push = NULL;
		m_spr_idx = 0;
		m_psnd_mng = psnd_mng;

		if (!createSpriteFromBitmap(pfilename, &bih, &m_pd3dt)) return;

		m_pos_x = pos_x;
		m_pos_y = pos_y;
		m_width = bih.biWidth;
		m_height = bih.biHeight;

		if (pfilename_sel != NULL) {
			// �}�E�X�I�[�o�[���̉摜�X�v���C�g�쐬�B
			if (!createSpriteFromBitmap(pfilename_sel, &bih, &m_pd3dt_sel)) m_width = 0;
		}

		if (pfilename_push != NULL) {
			// �}�E�X�N���b�N���̉摜�X�v���C�g�쐬�B
			if (!createSpriteFromBitmap(pfilename_push, &bih, &m_pd3dt_push)) m_width = 0;
		}

		if (psnd_filename_sel != NULL && lstrlenA(psnd_filename_sel) < sizeof(m_snd_filename_sel)) {
			lstrcpyA(m_snd_filename_sel, psnd_filename_sel);
		} else {
			m_snd_filename_sel[0] = '\0';
		}

		if (psnd_filename_push != NULL && lstrlenA(psnd_filename_push) < sizeof(m_snd_filename_push)) {
			lstrcpyA(m_snd_filename_push, psnd_filename_push);
		} else {
			m_snd_filename_push[0] = '\0';
		}
	}

	// �f�X�g���N�^�B
	~SpriteImg() {
		if (m_pd3dt_sel != NULL) m_pd3dt_sel->Release();
		if (m_pd3dt_push != NULL) m_pd3dt_push->Release();
	}

	LPDIRECT3DTEXTURE9 getSprite() {
		switch (m_spr_idx) {
		case 1:
			return m_pd3dt_sel;
		case 2:
			return m_pd3dt_push;
		default:
			return m_pd3dt;
		}
	}

	void onLButtonDown() {
		if (m_pd3dt_push != NULL) m_spr_idx = 2;
		else if (m_spr_idx == 0 && m_pd3dt_sel != NULL) m_spr_idx = 1;
		if (m_psnd_mng != NULL && m_snd_filename_push[0] != '\0') m_psnd_mng->Play(9, m_snd_filename_push);
	}

	void onLButtonUp() {
		if (m_pd3dt_sel != NULL) m_spr_idx = 1;
	}

	void onMouseOver() {
		if (m_pd3dt_sel != NULL) m_spr_idx = 1;
		if (m_psnd_mng != NULL && m_snd_filename_sel[0] != '\0') m_psnd_mng->Play(9, m_snd_filename_sel);
	}

	void onMouseOut() {
		m_spr_idx = 0;
	}

protected:
	LPDIRECT3DTEXTURE9 m_pd3dt_sel, m_pd3dt_push;
	BYTE m_spr_idx;

	SoundManager *m_psnd_mng;
	char m_snd_filename_sel[MAX_PATH], m_snd_filename_push[MAX_PATH];
};
