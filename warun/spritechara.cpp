#pragma once
#include "sprite.cpp"

// キャラスプライトクラス。
class SpriteChara : public Sprite {
public:
	static const WORD WINDOW_HEIGHT = 600;	// ウィンドウの高さ。

	// コンストラクタ。
	// 引数：pd3dd Direct3Dデバイスオブジェクト。
	//       pfilename ファイル名。
	//       pos_x X位置。
	SpriteChara(LPDIRECT3DDEVICE9 pd3dd, LPCSTR pfilename, short pos_x) : Sprite(pd3dd) {
		BITMAPINFOHEADER bih;

		if (!createSpriteFromBitmap(pfilename, &bih, &m_pd3dt)) return;

		m_width = bih.biWidth;
		m_height = bih.biHeight;
		m_pos_x = pos_x;
		m_pos_y = WINDOW_HEIGHT - m_height;
	}
};
