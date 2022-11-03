#pragma once
#include <d3d9.h>
#include "packfile.cpp"

// スプライトクラス。
class Sprite {
public:
	static const BYTE ACTION_CNT = 200;	// アクションの数。
	static const DWORD D3DFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;	// 2D用のDirect3DのFVF。

	// コンストラクタ。
	// 引数：pd3dd Direct3Dデバイスオブジェクト。
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

	// デストラクタ。
	virtual ~Sprite() {
		free(m_paction);
		if (m_pd3dt != NULL) m_pd3dt->Release();
	}

	// 成功したかどうか。
	// 引数：成功ならtrue、それ以外はfalse。
	bool isSuccess() {
		return m_width > 0;
	}

	// 停止アクションを追加する。
	// 引数：time 停止させるフレーム単位の時間。
	// 返値：成功ならtrue、それ以外はfalse。
	bool addPauseAction(short time) {
		Action action;

		action.type = PAUSE;
		action.param1 = time;	// 停止時間。
		action.param2 = 0;	// 未使用。
		action.param3 = 0;	// 未使用。
		return addAction(&action);
	}

	// フェードインアクションを追加する。
	// 引数：time フェードインが完了するまでのフレーム単位の時間。
	// 返値：成功ならtrue、それ以外はfalse。
	bool addFadeInAction(short time) {
		Action action;

		action.type = FADE_IN;
		action.param1 = time;	// 処理時間。
		action.param2 = 0;	// 未使用。
		action.param3 = 0;	// 未使用。
		return addAction(&action);
	}

	// フェードアウトアクションを追加する。
	// 引数：time フェードアウトが完了するまでのフレーム単位の時間。
	// 返値：成功ならtrue、それ以外はfalse。
	bool addFadeOutAction(short time) {
		Action action;

		action.type = FADE_OUT;
		action.param1 = time;	// 処理時間。
		action.param2 = 0;	// 未使用。
		action.param3 = 0;	// 未使用。
		return addAction(&action);
	}

	// スプライトがY方向に移動するアクションを追加する。
	// 引数：start_pos 開始位置。
	//       end_pos 終了位置。
	//       move 移動量。
	// 返値：成功ならtrue、それ以外はfalse。
	bool addMoveYAction(short start_pos, short end_pos, short move) {
		Action action;

		action.type = MOVE_Y;
		action.param1 = start_pos;	// 開始位置。
		action.param2 = end_pos;	// 終了位置。
		action.param3 = move;	// 移動量。
		return addAction(&action);
	}

	// アクションのループを設定する。
	// 引数：loop ループを開始するアクションの0からの位置。負の値を指定するとループしない。
	void setActionLoop(short loop) {
		m_action_loop = loop;
	}

	// アクションをリセットする。アクションによって変更された状態はそのまま残る。ループはしないように設定される。
	void resetAction() {
		m_action_cnt = m_action = 0;
		m_action_frame = 0;
		m_action_loop = -1;
		m_action_proc = false;
	}

	// アクションを再開する。
	void restartAction() {
		m_action_stop_flag = false;
	}

	// アクションを停止する。
	void stopAction() {
		m_action_stop_flag = true;
	}

	// スプライトオブジェクト取得。
	// 返値：スプライトオブジェクト。
	virtual LPDIRECT3DTEXTURE9 getSprite() {
		return m_pd3dt;
	}

	// スプライトのX位置取得。
	// 返値：スプライトのX位置。
	short getPosX() {
		return m_pos_x;
	}

	// スプライトのY位置取得。
	// 返値：スプライトのY位置。
	short getPosY() {
		return m_pos_y;
	}

	// スプライトの幅取得。
	// 返値：スプライトの幅。
	WORD getWidth() {
		return m_width;
	}

	// スプライトの高さ取得。
	// 返値：スプライトの高さ。
	WORD getHeight() {
		return m_height;
	}

	// スプライトID取得。
	// 返値：スプライトID。
	BYTE getID() {
		return m_id;
	}

	// スプライトが有効かどうかを返す。
	// 返値：enable 有効ならtrue、それ以外はfalse。
	bool isEnable() {
		return m_enable && m_alpha == 255;
	}

	// スプライトが有効かどうか設定する。
	// 引数：enble 有効ならtrue、それ以外はfalse。
	void setEnable(bool enable) {
		m_enable = enable;
	}

	// 毎フレーム実行される処理。
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

					// 終了位置より移動しないように調整。
					if (paction->param3 > 0 && m_pos_y > paction->param2
						|| paction->param3 < 0 && m_pos_y < paction->param2) m_pos_y = paction->param2;
				}
				m_action_frame++;
				m_action_proc = true;
				return;
			}

			m_action_frame = 0;	// 1つのアクションが終了したのでフレームカウントを0に戻す。

			if (m_action_loop < 0 && m_action == 0) {	// ループしていない、且つ、ループによるアクション位置が移動していない？
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
							// ループ開始位置がアクションの最大位置より大きいのでループしないものとする。
							m_action_cnt = 0;
							break;
						}
						// ループ開始位置をアクションの先頭にする。
						m_action_cnt -= m_action_loop;
						MoveMemory(m_paction, m_paction + m_action_loop, m_action_cnt * sizeof(Action));
						m_action_loop = 0;
					} else if (!m_action_proc) {	// 無限ループ条件を満たしたか？
						break;
					}
				}
			}
		}
	}

	// 描画処理。
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

	// マウスをスプライト上で左ボタンを押下時の処理。
	virtual void onLButtonDown() {
	}

	// マウスをスプライト上で左ボタンを離した時の処理。
	virtual void onLButtonUp() {
	}

	// マウスがスプライト上に合った時の処理。
	virtual void onMouseOver() {
	}

	// マウスがスプライト上を離れた時の処理。
	virtual void onMouseOut() {
	}

	// SpriteStringクラスで使用。
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

	short m_pos_x, m_pos_y;	// スプライト位置。
	WORD m_width, m_height, m_scr_width, m_scr_height;	// スプライトサイズ。
	BYTE m_alpha;	// スプライトの透明度。

	BYTE m_id;	// スプライトID。
	bool m_enable;

	enum ACTION_TYPE {
		PAUSE = 1, FADE_IN, FADE_OUT, MOVE_Y
	};
	struct Action {	// スプライトのアクションクラス。
		ACTION_TYPE type;	// アクションタイプ。
		short param1, param2, param3;	// アクションごとに用途が異なるパラメータ。
	} *m_paction;
	BYTE m_action_cnt, m_action;
	UINT m_action_frame;	// アクションごとのフレームカウント。
	short m_action_loop;
	bool m_action_stop_flag;
	bool m_action_proc;	// アクションの無限ループチェック用。

	// スプライトにアクションを追加する。
	// 引数：paction_add 追加するアクション。
	// 返値：成功ならtrue、それ以外はfalse。
	bool addAction(Action *paction_add) {
		Action *paction;

		if (m_action_cnt >= ACTION_CNT) return true;

		// アクションを追加する領域を確保。
		paction = (Action *)realloc(m_paction, (m_action_cnt + 1) * sizeof(Action));
		if (paction == NULL) return false;
		m_paction = paction;
		paction = &m_paction[m_action_cnt];	// 領域の最後の要素を設定。
		m_action_cnt++;

		CopyMemory(paction, paction_add, sizeof(Action));	// 最後の要素に追加するアクションを設定。
		return true;
	}

	// ビットマップファイルからスプライトを作成。
	// 引数：pfilename 24か32ビットのビットマップファイル名。
	//       pbih ビットマップ情報格納先。
	//       ppd3dt Direct3Dテクスチャオブジェクト設定先。
	// 返値：成功ならtrue、それ以外はfalse。
	bool createSpriteFromBitmap(LPCSTR pfilename, BITMAPINFOHEADER *pbih, IDirect3DTexture9 **ppd3dt) {
		int i, j;
		bool flag;
		BYTE padding, *pdata;
		DWORD width, height, size, *pbuf, *pdata4;
		BITMAPFILEHEADER bfh;
		D3DLOCKED_RECT d3dlr;
		PackFile pf(0);

		if (!pf.Open(pfilename)) return false;

		// BITMAPファイルヘッダ取得とチェック。
		if (pf.Read(&bfh, sizeof(bfh)) != sizeof(bfh)) return false;
		if (memcmp(&bfh.bfType, "BM", 2) != 0) return false;

		// BITMAP情報ヘッダ取得とチェック。
		if (pf.Read(pbih, sizeof(*pbih)) != sizeof(*pbih)) return false;
		if (pbih->biSize < sizeof(BITMAPINFOHEADER) || pbih->biSize > sizeof(BITMAPV5HEADER)) return false;
		if (pbih->biWidth < 1 || pbih->biHeight < 1) return false;
		if (pbih->biBitCount != 24 && pbih->biBitCount != 32) return false;
		if (pbih->biCompression != BI_RGB && pbih->biCompression != BI_BITFIELDS) return false;

		// BITMAPデータ位置に移動。
		if (!pf.Seek(bfh.bfOffBits, SEEK_SET)) return false;

		// BITMAPデータサイズ設定。
		width = pbih->biWidth;
		height = pbih->biHeight;
		size = width * (pbih->biBitCount / 8);
		padding = (4 - (size & 3)) & 3;
		size = (size + padding) * height;

		// 空スプライト作成。
		for (m_scr_width = 1; m_scr_width < width; m_scr_width <<= 1);
		for (m_scr_height = 1; m_scr_height < height; m_scr_height <<= 1);
		if (FAILED(m_pd3dd->CreateTexture(m_scr_width, m_scr_height, 1, 0
			, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, ppd3dt, NULL))) return false;

		// BITMAPデータ領域確保。
		pdata = new BYTE[size];
		if (pdata == NULL) return false;
		flag = false;

		// BITMAPデータ取得。
		if (pf.Read(pdata, size) != size) goto procend;

		if (FAILED((*ppd3dt)->LockRect(0, &d3dlr, NULL, 0))) goto procend;

		// 1ピクセルずつ設定。
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
