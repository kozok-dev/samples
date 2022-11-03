#pragma once
#include <d3d9.h>
#include "packfile.cpp"

// �X�v���C�g�N���X�B
class Sprite {
public:
	static const BYTE ACTION_CNT = 200;	// �A�N�V�����̐��B
	static const DWORD D3DFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;	// 2D�p��Direct3D��FVF�B

	// �R���X�g���N�^�B
	// �����Fpd3dd Direct3D�f�o�C�X�I�u�W�F�N�g�B
	Sprite(LPDIRECT3DDEVICE9 pd3dd) {
		m_pd3dd = pd3dd;
		m_pd3dt = NULL;

		m_pos_x = m_pos_y = 0;
		m_width = m_height = m_scr_width = m_scr_height = 0;
		m_alpha = 0;

		m_id = 0;
		m_enable = true;

		m_paction = NULL;
		m_action_cnt = m_action = 0;
		m_action_frame = 0;
		m_action_loop = -1;
		m_action_stop_flag = m_action_proc = false;
	}

	// �f�X�g���N�^�B
	virtual ~Sprite() {
		free(m_paction);
		if (m_pd3dt != NULL) m_pd3dt->Release();
	}

	// �����������ǂ����B
	// �����F�����Ȃ�true�A����ȊO��false�B
	bool isSuccess() {
		return m_width > 0;
	}

	// ��~�A�N�V������ǉ�����B
	// �����Ftime ��~������t���[���P�ʂ̎��ԁB
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
	bool addPauseAction(short time) {
		Action action;

		action.type = PAUSE;
		action.param1 = time;	// ��~���ԁB
		action.param2 = 0;	// ���g�p�B
		action.param3 = 0;	// ���g�p�B
		return addAction(&action);
	}

	// �t�F�[�h�C���A�N�V������ǉ�����B
	// �����Ftime �t�F�[�h�C������������܂ł̃t���[���P�ʂ̎��ԁB
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
	bool addFadeInAction(short time) {
		Action action;

		action.type = FADE_IN;
		action.param1 = time;	// �������ԁB
		action.param2 = 0;	// ���g�p�B
		action.param3 = 0;	// ���g�p�B
		return addAction(&action);
	}

	// �t�F�[�h�A�E�g�A�N�V������ǉ�����B
	// �����Ftime �t�F�[�h�A�E�g����������܂ł̃t���[���P�ʂ̎��ԁB
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
	bool addFadeOutAction(short time) {
		Action action;

		action.type = FADE_OUT;
		action.param1 = time;	// �������ԁB
		action.param2 = 0;	// ���g�p�B
		action.param3 = 0;	// ���g�p�B
		return addAction(&action);
	}

	// �X�v���C�g��Y�����Ɉړ�����A�N�V������ǉ�����B
	// �����Fstart_pos �J�n�ʒu�B
	//       end_pos �I���ʒu�B
	//       move �ړ��ʁB
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
	bool addMoveYAction(short start_pos, short end_pos, short move) {
		Action action;

		action.type = MOVE_Y;
		action.param1 = start_pos;	// �J�n�ʒu�B
		action.param2 = end_pos;	// �I���ʒu�B
		action.param3 = move;	// �ړ��ʁB
		return addAction(&action);
	}

	// �A�N�V�����̃��[�v��ݒ肷��B
	// �����Floop ���[�v���J�n����A�N�V������0����̈ʒu�B���̒l���w�肷��ƃ��[�v���Ȃ��B
	void setActionLoop(short loop) {
		m_action_loop = loop;
	}

	// �A�N�V���������Z�b�g����B�A�N�V�����ɂ���ĕύX���ꂽ��Ԃ͂��̂܂܎c��B���[�v�͂��Ȃ��悤�ɐݒ肳���B
	void resetAction() {
		m_action_cnt = m_action = 0;
		m_action_frame = 0;
		m_action_loop = -1;
		m_action_proc = false;
	}

	// �A�N�V�������ĊJ����B
	void restartAction() {
		m_action_stop_flag = false;
	}

	// �A�N�V�������~����B
	void stopAction() {
		m_action_stop_flag = true;
	}

	// �X�v���C�g�I�u�W�F�N�g�擾�B
	// �Ԓl�F�X�v���C�g�I�u�W�F�N�g�B
	virtual LPDIRECT3DTEXTURE9 getSprite() {
		return m_pd3dt;
	}

	// �X�v���C�g��X�ʒu�擾�B
	// �Ԓl�F�X�v���C�g��X�ʒu�B
	short getPosX() {
		return m_pos_x;
	}

	// �X�v���C�g��Y�ʒu�擾�B
	// �Ԓl�F�X�v���C�g��Y�ʒu�B
	short getPosY() {
		return m_pos_y;
	}

	// �X�v���C�g�̕��擾�B
	// �Ԓl�F�X�v���C�g�̕��B
	WORD getWidth() {
		return m_width;
	}

	// �X�v���C�g�̍����擾�B
	// �Ԓl�F�X�v���C�g�̍����B
	WORD getHeight() {
		return m_height;
	}

	// �X�v���C�gID�擾�B
	// �Ԓl�F�X�v���C�gID�B
	BYTE getID() {
		return m_id;
	}

	// �X�v���C�g���L�����ǂ�����Ԃ��B
	// �Ԓl�Fenable �L���Ȃ�true�A����ȊO��false�B
	bool isEnable() {
		return m_enable && m_alpha == 255;
	}

	// �X�v���C�g���L�����ǂ����ݒ肷��B
	// �����Fenble �L���Ȃ�true�A����ȊO��false�B
	void setEnable(bool enable) {
		m_enable = enable;
	}

	// ���t���[�����s����鏈���B
	void Process() {
		Action *paction;

		while (m_action < m_action_cnt && !m_action_stop_flag) {
			paction = &m_paction[m_action];

			switch (paction->type) {
			case PAUSE:
				if (m_action_frame >= paction->param1) break;
				m_action_frame++;
				m_action_proc = true;
				return;

			case FADE_IN:
				if (m_action_frame == 0) m_alpha = paction->param1 > 0 ? 0 : 255;
				if (m_action_frame >= paction->param1) break;
				m_action_frame++;
				m_alpha = (float)m_action_frame / (float)paction->param1 * 255;
				m_action_proc = true;
				return;

			case FADE_OUT:
				if (m_action_frame == 0) m_alpha = paction->param1 > 0 ? 255 : 0;
				if (m_action_frame >= paction->param1) break;
				m_action_frame++;
				m_alpha = 255 - (float)m_action_frame / (float)paction->param1 * 255;
				m_action_proc = true;
				return;

			case MOVE_Y:
				if (m_action_frame == 0) {
					m_pos_y = paction->param1;
					if (paction->param3 == 0) break;
				} else {
					if (m_pos_y == paction->param2) break;

					m_pos_y += paction->param3;

					// �I���ʒu���ړ����Ȃ��悤�ɒ����B
					if (paction->param3 > 0 && m_pos_y > paction->param2
						|| paction->param3 < 0 && m_pos_y < paction->param2) m_pos_y = paction->param2;
				}
				m_action_frame++;
				m_action_proc = true;
				return;
			}

			m_action_frame = 0;	// 1�̃A�N�V�������I�������̂Ńt���[���J�E���g��0�ɖ߂��B

			if (m_action_loop < 0 && m_action == 0) {	// ���[�v���Ă��Ȃ��A���A���[�v�ɂ��A�N�V�����ʒu���ړ����Ă��Ȃ��H
				m_action_proc = false;
				m_action_cnt--;
				if (m_action_cnt == 0) break;
				MoveMemory(m_paction, m_paction + 1, m_action_cnt * sizeof(Action));
			} else {
				m_action++;
				if (m_action >= m_action_cnt) {
					m_action = 0;

					if (m_action_loop > 0) {
						m_action_proc = false;
						if (m_action_loop >= m_action_cnt) {
							// ���[�v�J�n�ʒu���A�N�V�����̍ő�ʒu���傫���̂Ń��[�v���Ȃ����̂Ƃ���B
							m_action_cnt = 0;
							break;
						}
						// ���[�v�J�n�ʒu���A�N�V�����̐擪�ɂ���B
						m_action_cnt -= m_action_loop;
						MoveMemory(m_paction, m_paction + m_action_loop, m_action_cnt * sizeof(Action));
						m_action_loop = 0;
					} else if (!m_action_proc) {	// �������[�v�����𖞂��������H
						break;
					}
				}
			}
		}
	}

	// �`�揈���B
	void Draw() {
		VERTEX_INFO vi[4];

		vi[0].z = vi[1].z = vi[2].z = vi[3].z = vi[0].u = vi[0].v = vi[1].v = vi[2].u = 0;
		vi[0].rhw = vi[1].rhw = vi[2].rhw = vi[3].rhw = vi[1].u = vi[2].v = vi[3].u = 1; vi[3].v = 1;
		vi[0].color = vi[1].color = vi[2].color = vi[3].color = (m_alpha << 24) + 0xffffff;

		vi[0].x = vi[2].x = m_pos_x;
		vi[0].y = vi[1].y = m_pos_y;
		vi[1].x = vi[3].x = m_scr_width + m_pos_x;
		vi[2].y = vi[3].y = m_scr_height + m_pos_y;

		if (FAILED(m_pd3dd->SetTexture(0, getSprite()))) return;
		m_pd3dd->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vi, sizeof(vi[0]));
	}

	// �}�E�X���X�v���C�g��ō��{�^�����������̏����B
	virtual void onLButtonDown() {
	}

	// �}�E�X���X�v���C�g��ō��{�^���𗣂������̏����B
	virtual void onLButtonUp() {
	}

	// �}�E�X���X�v���C�g��ɍ��������̏����B
	virtual void onMouseOver() {
	}

	// �}�E�X���X�v���C�g��𗣂ꂽ���̏����B
	virtual void onMouseOut() {
	}

	// SpriteString�N���X�Ŏg�p�B
	virtual bool writeString(LPCWSTR, LPCWSTR, BYTE, bool, WORD, WORD) {
		return true;
	}

protected:
	LPDIRECT3DDEVICE9 m_pd3dd;
	LPDIRECT3DTEXTURE9 m_pd3dt;
	struct VERTEX_INFO {
		float x, y, z, rhw;
		D3DCOLOR color;
		float u, v;
	};

	short m_pos_x, m_pos_y;	// �X�v���C�g�ʒu�B
	WORD m_width, m_height, m_scr_width, m_scr_height;	// �X�v���C�g�T�C�Y�B
	BYTE m_alpha;	// �X�v���C�g�̓����x�B

	BYTE m_id;	// �X�v���C�gID�B
	bool m_enable;

	enum ACTION_TYPE {
		PAUSE = 1, FADE_IN, FADE_OUT, MOVE_Y
	};
	struct Action {	// �X�v���C�g�̃A�N�V�����N���X�B
		ACTION_TYPE type;	// �A�N�V�����^�C�v�B
		short param1, param2, param3;	// �A�N�V�������Ƃɗp�r���قȂ�p�����[�^�B
	} *m_paction;
	BYTE m_action_cnt, m_action;
	UINT m_action_frame;	// �A�N�V�������Ƃ̃t���[���J�E���g�B
	short m_action_loop;
	bool m_action_stop_flag;
	bool m_action_proc;	// �A�N�V�����̖������[�v�`�F�b�N�p�B

	// �X�v���C�g�ɃA�N�V������ǉ�����B
	// �����Fpaction_add �ǉ�����A�N�V�����B
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
	bool addAction(Action *paction_add) {
		Action *paction;

		if (m_action_cnt >= ACTION_CNT) return true;

		// �A�N�V������ǉ�����̈���m�ہB
		paction = (Action *)realloc(m_paction, (m_action_cnt + 1) * sizeof(Action));
		if (paction == NULL) return false;
		m_paction = paction;
		paction = &m_paction[m_action_cnt];	// �̈�̍Ō�̗v�f��ݒ�B
		m_action_cnt++;

		CopyMemory(paction, paction_add, sizeof(Action));	// �Ō�̗v�f�ɒǉ�����A�N�V������ݒ�B
		return true;
	}

	// �r�b�g�}�b�v�t�@�C������X�v���C�g���쐬�B
	// �����Fpfilename 24��32�r�b�g�̃r�b�g�}�b�v�t�@�C�����B
	//       pbih �r�b�g�}�b�v���i�[��B
	//       ppd3dt Direct3D�e�N�X�`���I�u�W�F�N�g�ݒ��B
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
	bool createSpriteFromBitmap(LPCSTR pfilename, BITMAPINFOHEADER *pbih, IDirect3DTexture9 **ppd3dt) {
		int i, j;
		bool flag;
		BYTE padding, *pdata;
		DWORD width, height, size, *pbuf, *pdata4;
		BITMAPFILEHEADER bfh;
		D3DLOCKED_RECT d3dlr;
		PackFile pf(0);

		if (!pf.Open(pfilename)) return false;

		// BITMAP�t�@�C���w�b�_�擾�ƃ`�F�b�N�B
		if (pf.Read(&bfh, sizeof(bfh)) != sizeof(bfh)) return false;
		if (memcmp(&bfh.bfType, "BM", 2) != 0) return false;

		// BITMAP���w�b�_�擾�ƃ`�F�b�N�B
		if (pf.Read(pbih, sizeof(*pbih)) != sizeof(*pbih)) return false;
		if (pbih->biSize < sizeof(BITMAPINFOHEADER) || pbih->biSize > sizeof(BITMAPV5HEADER)) return false;
		if (pbih->biWidth < 1 || pbih->biHeight < 1) return false;
		if (pbih->biBitCount != 24 && pbih->biBitCount != 32) return false;
		if (pbih->biCompression != BI_RGB && pbih->biCompression != BI_BITFIELDS) return false;

		// BITMAP�f�[�^�ʒu�Ɉړ��B
		if (!pf.Seek(bfh.bfOffBits, SEEK_SET)) return false;

		// BITMAP�f�[�^�T�C�Y�ݒ�B
		width = pbih->biWidth;
		height = pbih->biHeight;
		size = width * (pbih->biBitCount / 8);
		padding = (4 - (size & 3)) & 3;
		size = (size + padding) * height;

		// ��X�v���C�g�쐬�B
		for (m_scr_width = 1; m_scr_width < width; m_scr_width <<= 1);
		for (m_scr_height = 1; m_scr_height < height; m_scr_height <<= 1);
		if (FAILED(m_pd3dd->CreateTexture(m_scr_width, m_scr_height, 1, 0
			, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, ppd3dt, NULL))) return false;

		// BITMAP�f�[�^�̈�m�ہB
		pdata = new BYTE[size];
		if (pdata == NULL) return false;
		flag = false;

		// BITMAP�f�[�^�擾�B
		if (pf.Read(pdata, size) != size) goto procend;

		if (FAILED((*ppd3dt)->LockRect(0, &d3dlr, NULL, 0))) goto procend;

		// 1�s�N�Z�����ݒ�B
		pbuf = (DWORD *)d3dlr.pBits;
		if (pbih->biBitCount == 24) {
			for (i = height - 1; i >= 0; i--) {
				for (j = 0; j < width; j++) {
					size = (i * width + j) * 3 + (i * padding);
					*pbuf++ = 0xff000000 + (pdata[size + 2] << 16) + (pdata[size + 1] << 8) + pdata[size];
				}
				pbuf += m_scr_width - width;
			}
		} else {
			pdata4 = (DWORD *)pdata;
			for (i = height - 1; i >= 0; i--) {
				for (j = 0; j < width; j++) *pbuf++ = pdata4[i * width + j];
				pbuf += m_scr_width - width;
			}
		}

		(*ppd3dt)->UnlockRect(0);
		flag = true;
procend:
		delete[] pdata;
		return flag;
	}
};
