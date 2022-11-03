#pragma once
#include "sprite.cpp"
#include "soundmanager.cpp"

// 画像スプライトクラス。
class SpriteImg : public Sprite {
public:
	// 色スプライトコンストラクタ。
	// 引数：pd3dd Direct3Dデバイスオブジェクト。
	//       color 色。
	//       pos_x X位置。
	//       pos_y Y位置。
	//       width 幅。
	//       height 高さ。
	SpriteImg(LPDIRECT3DDEVICE9 pd3dd, D3DCOLOR color, short pos_x, short pos_y, WORD width, WORD height)
		: Sprite(pd3dd) {

		DWORD i, j, *pbuf;
		D3DLOCKED_RECT d3dlr;

		m_pd3dt_sel = m_pd3dt_push = NULL;
		m_spr_idx = 0;
		m_psnd_mng = NULL;
		m_snd_filename_sel[0] = m_snd_filename_push[0] = '\0';

		// 空スプライト作成。
		for (m_scr_width = 1; m_scr_width < width; m_scr_width <<= 1);
		for (m_scr_height = 1; m_scr_height < height; m_scr_height <<= 1);
		if (FAILED(m_pd3dd->CreateTexture(m_scr_width, m_scr_height, 1, 0
			, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pd3dt, NULL))) return;

		m_pos_x = pos_x;
		m_pos_y = pos_y;
		m_width = width;
		m_height = height;

		if (FAILED(m_pd3dt->LockRect(0, &d3dlr, NULL, D3DLOCK_DISCARD))) return;

		// 塗りつぶす。
		pbuf = (DWORD *)d3dlr.pBits;
		for (i = 0; i < m_height; i++) {
			for (j = 0; j < m_width; j++) *pbuf++ = color;
			pbuf += m_scr_width - m_width;
		}

		m_pd3dt->UnlockRect(0);
	}

	// 画像スプライトコンストラクタ。
	// 引数：pd3dd Direct3Dデバイスオブジェクト。
	//       pfilename ファイル名。
	//       pfilename_sel マウスオーバー時のファイル名。
	//       pfilename_push マウスボタン押下時のファイル名。
	//       pos_x X位置。
	//       pos_y Y位置。
	//       id スプライトID。
	//       psnd_mng 音声管理オブジェクト。
	//       psnd_filename_sel マウスオーバー時の音声ファイル名。
	//       psnd_filename_push マウスボタン押下時の音声ファイル名。
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
			// マウスオーバー時の画像スプライト作成。
			if (!createSpriteFromBitmap(pfilename_sel, &bih, &m_pd3dt_sel)) m_width = 0;
		}

		if (pfilename_push != NULL) {
			// マウスクリック時の画像スプライト作成。
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

	// デストラクタ。
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
