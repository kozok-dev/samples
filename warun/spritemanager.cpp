#pragma once
#include "spriteimg.cpp"
#include "spritechara.cpp"
#include "spritestring.cpp"

// �X�v���C�g�Ǘ��N���X�B
class SpriteManager {
public:
	static const BYTE SPRITE_CNT = 50;	// �X�v���C�g�̐��B
	static const WORD WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;	// �E�B���h�E�T�C�Y�B

	// �I�u�W�F�N�g�擾�B
	// �����Fhwnd �`��E�B���h�E�n���h���B
	//       pd3dd Direct3D�f�o�C�X�I�u�W�F�N�g�B
	static SpriteManager *getObject(HWND hwnd, LPDIRECT3DDEVICE9 pd3dd) {
		static SpriteManager spr_mng(hwnd, pd3dd);
		return &spr_mng;
	}

	// �f�X�g���N�^�B
	~SpriteManager() {
		BYTE i;

		for (i = 0; i < SPRITE_CNT; i++) {
			if (m_psprite[i] != NULL) delete m_psprite[i];
		}
	}

	// �X�v���C�g�F�I�u�W�F�N�g�쐬�B
	// �����Fsprite_idx �X�v���C�g�ԍ��B
	//       color �F�B
	//       pos_x �X�v���C�g��X�ʒu�B
	//       pos_y �X�v���C�g��Y�ʒu�B
	//       width �X�v���C�g�̕��B0�܂��͏ȗ�����ƒ[�܂łɂȂ�B
	//       height �X�v���C�g�̍����B0�܂��͏ȗ�����ƒ[�܂łɂȂ�B
	void createSpriteColor(BYTE sprite_idx, D3DCOLOR color, short pos_x = 0, short pos_y = 0
		, WORD width = 0, WORD height = 0) {

		Sprite *psprite;

		if (width == 0 && WINDOW_WIDTH > pos_x) width = WINDOW_WIDTH - pos_x;
		if (height == 0 && WINDOW_HEIGHT > pos_y) height = WINDOW_HEIGHT - pos_y;

		psprite = new SpriteImg(m_pd3dd, color, pos_x, pos_y, width, height);
		setSprite(psprite, sprite_idx);
	}

	// �X�v���C�g�摜�I�u�W�F�N�g�쐬�B
	// �����Fsprite_idx �X�v���C�g�ԍ��B
	//       pfilename �X�v���C�g�t�@�C�����B
	//       pos_x �X�v���C�g��X�ʒu�B
	//       pos_y �X�v���C�g��Y�ʒu�B
	void createSpriteImg(BYTE sprite_idx, LPCSTR pfilename, short pos_x = 0, short pos_y = 0) {
		Sprite *psprite = new SpriteImg(m_pd3dd, pfilename, NULL, NULL, pos_x, pos_y, 0, NULL, NULL, NULL);
		setSprite(psprite, sprite_idx);
	}

	// �X�v���C�g�摜�{�^���I�u�W�F�N�g�쐬�B
	// �����Fsprite_idx �X�v���C�g�ԍ��B
	//       pfilename �X�v���C�g�t�@�C�����B
	//       pfilename_sel �}�E�X�I�[�o�[���̃X�v���C�g�t�@�C�����B
	//       pfilename_push �}�E�X�{�^���������̃X�v���C�g�t�@�C�����B
	//       pos_x �X�v���C�g��X�ʒu�B
	//       pos_y �X�v���C�g��Y�ʒu�B
	//       id �X�v���C�gID�B
	//       psnd_mng �����Ǘ��I�u�W�F�N�g�B
	//       psnd_filename_sel �}�E�X�I�[�o�[���̉����t�@�C�����B
	//       psnd_filename_push �}�E�X�{�^���������̉����t�@�C�����B
	void createSpriteImgButton(BYTE sprite_idx, LPCSTR pfilename
		, LPCSTR pfilename_sel, LPCSTR pfilename_push, short pos_x, short pos_y, BYTE id = 0
		, SoundManager *psnd_mng = NULL, LPCSTR psnd_filename_sel = NULL, LPCSTR psnd_filename_push = NULL) {

		Sprite *psprite = new SpriteImg(m_pd3dd, pfilename, pfilename_sel, pfilename_push
			, pos_x, pos_y, id, psnd_mng, psnd_filename_sel, psnd_filename_push);
		setSprite(psprite, sprite_idx);
	}

	// �X�v���C�g�L�����I�u�W�F�N�g�쐬�B
	// �����Fsprite_idx �X�v���C�g�ԍ��B
	//       pfilename �X�v���C�g�t�@�C�����B
	//       pos_x �X�v���C�g��X�ʒu�B
	void createSpriteChara(BYTE sprite_idx, LPCSTR pfilename, short pos_x) {
		Sprite *psprite = new SpriteChara(m_pd3dd, pfilename, pos_x);
		setSprite(psprite, sprite_idx);
	}

	// �X�v���C�g������I�u�W�F�N�g�쐬�B
	// �����Fsprite_idx �X�v���C�g�ԍ��B
	//       pos_x �X�v���C�g��X�ʒu�B
	//       pos_y �X�v���C�g��Y�ʒu�B
	//       width �X�v���C�g�̕��B
	//       height �X�v���C�g�̍����B
	//       size �t�H���g�T�C�Y�B
	//       color �t�H���g�̐F�B
	//       color_mo �J�[�\�������킹�����̃t�H���g�̐F�B�ς������Ȃ��Ȃ�0�A���ɂ������Ȃ�0xff000000�B
	//       id �X�v���C�gID�B
	void createSpriteString(BYTE sprite_idx, short pos_x, short pos_y, WORD width, WORD height
		, BYTE size, D3DCOLOR color, D3DCOLOR color_mo = 0, BYTE id = 0) {

		Sprite *psprite = new SpriteString(m_pd3dd, pos_x, pos_y, width, height, size, color, color_mo, id);
		setSprite(psprite, sprite_idx);
	}

	// �X�v���C�g���폜����B
	// �����Fsprite_idx �X�v���C�g�ԍ��B
	void deleteSprite(BYTE sprite_idx) {
		Sprite **ppsprite;

		if (sprite_idx >= SPRITE_CNT) return;
		ppsprite = &m_psprite[sprite_idx];
		if (*ppsprite == NULL) return;

		delete *ppsprite;
		*ppsprite = NULL;
	}

	// ��~�A�N�V������ǉ�����B
	// �����Fsprite_idx �X�v���C�g�ԍ��B
	//       time ��~������t���[���P�ʂ̎��ԁB
	void addPauseAction(BYTE sprite_idx, short time) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		if (!psprite->addPauseAction(time)) m_error = true;
	}

	// �t�F�[�h�C���A�N�V������ǉ�����B
	// �����Fsprite_idx �X�v���C�g�ԍ��B
	//       time �t�F�[�h�C������������܂ł̃t���[���P�ʂ̎��ԁB
	void addFadeInAction(BYTE sprite_idx, short time) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		if (!psprite->addFadeInAction(time)) m_error = true;
	}

	// �t�F�[�h�A�E�g�A�N�V������ǉ�����B
	// �����Fsprite_idx �X�v���C�g�ԍ��B
	//       time �t�F�[�h�A�E�g����������܂ł̃t���[���P�ʂ̎��ԁB
	//       noproc_flag true���w�肷��Ə����ς݂Ȃ�ēx�������Ȃ��悤�ɂ���B
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
	bool addFadeOutAction(BYTE sprite_idx, short time, bool noproc_flag = false) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return false;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return false;

		if (!noproc_flag || psprite->isEnable()) {
			if (!psprite->addFadeOutAction(time)) {
				m_error = true;
				return false;
			}
		}
		return true;
	}

	// �X�v���C�g��Y�����Ɉړ�����A�N�V������ǉ�����B
	// �����Fsprite_idx �X�v���C�g�ԍ��B
	//       start_pos �J�n�ʒu�B
	//       end_pos �I���ʒu�B
	//       move �ړ��ʁB
	void addMoveYAction(BYTE sprite_idx, short start_pos, short end_pos, short move) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		if (!psprite->addMoveYAction(start_pos, end_pos, move)) m_error = true;
	}

	// �����ɐݒ��A�������`��̈�ɏ������ށB
	// �����Fsprite_idx �X�v���C�g�ԍ��B
	//       ptext ���́B
	//       pname ���O�B
	//       bracket ���͂��͂ފ��ʂ̎�ށB
	//       noclr_flag true�ɂ���ƕ`��̈�����������Ȃ��B
	//       pos_x ��������X�ʒu�B
	//       pos_y ��������Y�ʒu�B
	void writeString(BYTE sprite_idx, LPCWSTR ptext, LPCWSTR pname = NULL, BYTE bracket = 0, bool noclr_flag = false, WORD pos_x = 0, WORD pos_y = 0) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		if (!psprite->writeString(ptext, pname, bracket, noclr_flag, pos_x, pos_y)) m_error = true;
	}

	// �A�N�V�����̃��[�v��ݒ肷��B
	// �����Fsprite_idx �X�v���C�g�ԍ��B
	//       loop ���[�v���J�n����A�N�V������0����̈ʒu�B���̒l���w�肷��ƃ��[�v���Ȃ��B
	void setActionLoop(BYTE sprite_idx, short loop) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		psprite->setActionLoop(loop);
	}

	// �A�N�V���������Z�b�g����B�A�N�V�����ɂ���ĕύX���ꂽ��Ԃ͂��̂܂܎c��܂��B
	// �����Fsprite_idx �X�v���C�g�ԍ��B
	void resetAction(BYTE sprite_idx) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		psprite->resetAction();
	}

	// �A�N�V�������ĊJ����B
	// �����Fsprite_idx �X�v���C�g�ԍ��B
	void restartAction(BYTE sprite_idx) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		psprite->restartAction();
	}

	// �A�N�V�������~����B
	// �����Fsprite_idx �X�v���C�g�ԍ��B
	void stopAction(BYTE sprite_idx) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		psprite->stopAction();
	}

	// �X�v���C�g���L�����ǂ����ݒ肷��B
	// �����Fsprite_idx �X�v���C�g�ԍ��B
	//       enable �L���Ȃ�true�A����ȊO��false�B
	void setEnable(BYTE sprite_idx, bool enable) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		psprite->setEnable(enable);
	}

	// ���t���[�����s����鏈���B
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
	bool Process() {
		BYTE i;

		if (m_error) return false;

		for (i = 0; i < SPRITE_CNT; i++) {
			if (m_psprite[i] != NULL) m_psprite[i]->Process();
		}
		return true;
	}

	// �`�揈���B
	void Draw() {
		BYTE i;

		for (i = 0; i < SPRITE_CNT; i++) {
			if (m_psprite[i] != NULL) m_psprite[i]->Draw();
		}
	}

	// �}�E�X�C�x���g���̏����B
	//   �X�v���C�gID��ݒ肵�Ă���X�v���C�g�̏ꍇ�A
	//   �}�E�X�̍��{�^�������������藣�����Ƃ��̈ʒu�ōł���O�ɕ\�����Ă���X�v���C�g�̊e�폈�����Ăяo���A
	//   �}�E�X���E�B���h�E���œ������Ă���Ƃ��̏�����X�EY�ʒu�ɉ������X�v���C�g�̊e�폈�����Ăяo���B
	// �����Ftype ���{�^������������1�A���{�^���𗣂����Ƃ���2�A�}�E�X�𓮂����Ă��铮��Ȃ�3���w�肷��B
	//       pos_x X�ʒu�B
	//       pos_y Y�ʒu�B
	//       noprepare_flag true�ɂ���ƃE�B���h�E�T�C�Y�ɂ����W�̒��������Ȃ��B
	// �Ԓl�F�N���b�N�����ꍇ�A�X�v���C�gID��Ԃ����A�X�v���C�gID��ݒ肵�Ă��Ȃ��X�v���C�g��0��Ԃ��B
	//       ����ȊO��0�B
	BYTE eventMouse(BYTE type, WORD pos_x, WORD pos_y, bool noprepare_flag) {
		short i;
		WORD ret_id = 0, id, spr_pox_x, spr_pox_y;
		RECT rect;
		Sprite *psprite;

		if (!GetClientRect(m_hwnd, &rect)) return 0;
		rect.right -= rect.left;
		rect.bottom -= rect.top;

		if (pos_x >= rect.right || pos_y >= rect.bottom) {	// �}�E�X�̈ʒu���E�B���h�E�̊O�����H
			i = -1;
		} else {
			// �E�B���h�E�T�C�Y�̔䗦�ɍ��킹�č��W�𒲐��B
			if (!noprepare_flag) {
				pos_x = (float)pos_x / ((float)rect.right / WINDOW_WIDTH);
				pos_y = (float)pos_y / ((float)rect.bottom / WINDOW_HEIGHT);
			}

			for (i = SPRITE_CNT - 1; i >= 0; i--) {	// ��O�ɕ\�����Ă���X�v���C�g���珇�Ƀ`�F�b�N�B
				psprite = m_psprite[i];
				if (psprite == NULL) continue;
				id = psprite->getID();

				// �X�v���C�g�����ǂ����`�F�b�N�B
				spr_pox_x = psprite->getPosX();
				spr_pox_y = psprite->getPosY();
				if (pos_x < spr_pox_x || pos_y < spr_pox_y || pos_x >= spr_pox_x + psprite->getWidth()
					|| pos_y >= spr_pox_y + psprite->getHeight()) continue;
				if (id == 0 || !psprite->isEnable()) continue;

				switch (type) {
				case 1:	// �}�E�X�̍��{�^�������B
					ret_id = id;
					psprite->onLButtonDown();
					break;
				case 2:	// �}�E�X�̍��{�^���𗣂����B
					psprite->onLButtonUp();
					break;
				case 3:	// �}�E�X�𓮂������B
					if (m_sprite_mousepos == i) return 0;	// �����X�v���C�g�ɂ���̂ŏ������Ȃ��B
					psprite->onMouseOver();
					break;
				}

				break;
			}
		}

		if (m_sprite_mousepos != i) {
			if (m_sprite_mousepos > 0 && m_sprite_mousepos < SPRITE_CNT) {
				psprite = m_psprite[m_sprite_mousepos];
				if (psprite != NULL && psprite->isEnable()) psprite->onMouseOut();	// �O��̃X�v���C�g��mouseout�����B
			}

			m_sprite_mousepos = i;	// �X�v���C�g�ԍ������݂̃}�E�X�̈ʒu�ɂ���X�v���C�g�ԍ��ɐݒ�B
		}

		return ret_id;
	}

	// �}�E�X���X�v���C�g��𗣂ꂽ���̏���������B
	// �����Fsprite_idx �X�v���C�g�ԍ�(�S�Ăɂ������Ȃ畉�̒l���w�肷��)�B
	//       all_flag false�Ȃ�L���ȃX�v���C�g�̂݁Atrue�Ȃ�S�āB
	//       range_flag false�Ȃ�sprite_idx�̂݁Atrue�Ȃ�0�`sprite_idx��ΏۂƂ���B
	void mouseOut(char sprite_idx, bool all_flag, bool range_flag) {
		short cnt, i;
		Sprite *psprite;

		if (sprite_idx < 0 || sprite_idx > SPRITE_CNT) cnt = SPRITE_CNT - 1;
		else if (range_flag) cnt = sprite_idx;
		else cnt = -1;

		if (cnt < 0) {
			psprite = m_psprite[sprite_idx];
			if (psprite != NULL && psprite->getID() > 0 && (all_flag || psprite->isEnable())) {
				psprite->onMouseOut();
			}
		} else {
			for (i = 0; i <= cnt; i++) {
				psprite = m_psprite[i];
				if (psprite != NULL && psprite->getID() > 0 && (all_flag || psprite->isEnable())) {
					psprite->onMouseOut();
				}
			}
		}
	}

protected:
	HWND m_hwnd;
	LPDIRECT3DDEVICE9 m_pd3dd;
	Sprite *m_psprite[SPRITE_CNT];
	short m_sprite_mousepos;	// �}�E�X�̈ʒu�ɂ���X�v���C�g�ԍ��B
	bool m_error;

	// �R���X�g���N�^�B
	// �����Fhwnd �`��E�B���h�E�n���h���B
	//       pd3dd Direct3D�f�o�C�X�I�u�W�F�N�g�B
	SpriteManager(HWND hwnd, LPDIRECT3DDEVICE9 pd3dd) {
		BYTE i;

		m_hwnd = hwnd;
		m_pd3dd = pd3dd;
		for (i = 0; i < SPRITE_CNT; i++) m_psprite[i] = NULL;
		m_sprite_mousepos = -1;
		m_error = false;
	}

	// �X�v���C�g�쐬�B
	// �����Fpsprite �L���ȃX�v���C�g�I�u�W�F�N�g�B
	//       sprite_idx �X�v���C�g�ԍ��B
	void setSprite(Sprite *psprite, BYTE sprite_idx) {
		Sprite **ppsprite;

		if (sprite_idx >= SPRITE_CNT) {
			delete psprite;
			return;
		}

		ppsprite = &m_psprite[sprite_idx];
		if (*ppsprite != NULL) delete *ppsprite;	// �쐬�ς݂̏ꍇ�͂܂��폜�B
		*ppsprite = psprite;

		if (!(*ppsprite)->isSuccess()) m_error = true;
	}
};
