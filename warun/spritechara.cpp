#pragma once
#include "sprite.cpp"

// �L�����X�v���C�g�N���X�B
class SpriteChara : public Sprite {
public:
	static const WORD WINDOW_HEIGHT = 600;	// �E�B���h�E�̍����B

	// �R���X�g���N�^�B
	// �����Fpd3dd Direct3D�f�o�C�X�I�u�W�F�N�g�B
	//       pfilename �t�@�C�����B
	//       pos_x X�ʒu�B
	SpriteChara(LPDIRECT3DDEVICE9 pd3dd, LPCSTR pfilename, short pos_x) : Sprite(pd3dd) {
		BITMAPINFOHEADER bih;

		if (!createSpriteFromBitmap(pfilename, &bih, &m_pd3dt)) return;

		m_width = bih.biWidth;
		m_height = bih.biHeight;
		m_pos_x = pos_x;
		m_pos_y = WINDOW_HEIGHT - m_height;
	}
};
