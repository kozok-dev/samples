#pragma once
#include "spriteimg.cpp"
#include "spritechara.cpp"
#include "spritestring.cpp"

// スプライト管理クラス。
class SpriteManager {
public:
	static const BYTE SPRITE_CNT = 50;	// スプライトの数。
	static const WORD WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;	// ウィンドウサイズ。

	// オブジェクト取得。
	// 引数：hwnd 描画ウィンドウハンドル。
	//       pd3dd Direct3Dデバイスオブジェクト。
	static SpriteManager *getObject(HWND hwnd, LPDIRECT3DDEVICE9 pd3dd) {
		static SpriteManager spr_mng(hwnd, pd3dd);
		return &spr_mng;
	}

	// デストラクタ。
	~SpriteManager() {
		BYTE i;

		for (i = 0; i < SPRITE_CNT; i++) {
			if (m_psprite[i] != NULL) delete m_psprite[i];
		}
	}

	// スプライト色オブジェクト作成。
	// 引数：sprite_idx スプライト番号。
	//       color 色。
	//       pos_x スプライトのX位置。
	//       pos_y スプライトのY位置。
	//       width スプライトの幅。0または省略すると端までになる。
	//       height スプライトの高さ。0または省略すると端までになる。
	void createSpriteColor(BYTE sprite_idx, D3DCOLOR color, short pos_x = 0, short pos_y = 0
		, WORD width = 0, WORD height = 0) {

		Sprite *psprite;

		if (width == 0 && WINDOW_WIDTH > pos_x) width = WINDOW_WIDTH - pos_x;
		if (height == 0 && WINDOW_HEIGHT > pos_y) height = WINDOW_HEIGHT - pos_y;

		psprite = new SpriteImg(m_pd3dd, color, pos_x, pos_y, width, height);
		setSprite(psprite, sprite_idx);
	}

	// スプライト画像オブジェクト作成。
	// 引数：sprite_idx スプライト番号。
	//       pfilename スプライトファイル名。
	//       pos_x スプライトのX位置。
	//       pos_y スプライトのY位置。
	void createSpriteImg(BYTE sprite_idx, LPCSTR pfilename, short pos_x = 0, short pos_y = 0) {
		Sprite *psprite = new SpriteImg(m_pd3dd, pfilename, NULL, NULL, pos_x, pos_y, 0, NULL, NULL, NULL);
		setSprite(psprite, sprite_idx);
	}

	// スプライト画像ボタンオブジェクト作成。
	// 引数：sprite_idx スプライト番号。
	//       pfilename スプライトファイル名。
	//       pfilename_sel マウスオーバー時のスプライトファイル名。
	//       pfilename_push マウスボタン押下時のスプライトファイル名。
	//       pos_x スプライトのX位置。
	//       pos_y スプライトのY位置。
	//       id スプライトID。
	//       psnd_mng 音声管理オブジェクト。
	//       psnd_filename_sel マウスオーバー時の音声ファイル名。
	//       psnd_filename_push マウスボタン押下時の音声ファイル名。
	void createSpriteImgButton(BYTE sprite_idx, LPCSTR pfilename
		, LPCSTR pfilename_sel, LPCSTR pfilename_push, short pos_x, short pos_y, BYTE id = 0
		, SoundManager *psnd_mng = NULL, LPCSTR psnd_filename_sel = NULL, LPCSTR psnd_filename_push = NULL) {

		Sprite *psprite = new SpriteImg(m_pd3dd, pfilename, pfilename_sel, pfilename_push
			, pos_x, pos_y, id, psnd_mng, psnd_filename_sel, psnd_filename_push);
		setSprite(psprite, sprite_idx);
	}

	// スプライトキャラオブジェクト作成。
	// 引数：sprite_idx スプライト番号。
	//       pfilename スプライトファイル名。
	//       pos_x スプライトのX位置。
	void createSpriteChara(BYTE sprite_idx, LPCSTR pfilename, short pos_x) {
		Sprite *psprite = new SpriteChara(m_pd3dd, pfilename, pos_x);
		setSprite(psprite, sprite_idx);
	}

	// スプライト文字列オブジェクト作成。
	// 引数：sprite_idx スプライト番号。
	//       pos_x スプライトのX位置。
	//       pos_y スプライトのY位置。
	//       width スプライトの幅。
	//       height スプライトの高さ。
	//       size フォントサイズ。
	//       color フォントの色。
	//       color_mo カーソルを合わせた時のフォントの色。変えたくないなら0、黒にしたいなら0xff000000。
	//       id スプライトID。
	void createSpriteString(BYTE sprite_idx, short pos_x, short pos_y, WORD width, WORD height
		, BYTE size, D3DCOLOR color, D3DCOLOR color_mo = 0, BYTE id = 0) {

		Sprite *psprite = new SpriteString(m_pd3dd, pos_x, pos_y, width, height, size, color, color_mo, id);
		setSprite(psprite, sprite_idx);
	}

	// スプライトを削除する。
	// 引数：sprite_idx スプライト番号。
	void deleteSprite(BYTE sprite_idx) {
		Sprite **ppsprite;

		if (sprite_idx >= SPRITE_CNT) return;
		ppsprite = &m_psprite[sprite_idx];
		if (*ppsprite == NULL) return;

		delete *ppsprite;
		*ppsprite = NULL;
	}

	// 停止アクションを追加する。
	// 引数：sprite_idx スプライト番号。
	//       time 停止させるフレーム単位の時間。
	void addPauseAction(BYTE sprite_idx, short time) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		if (!psprite->addPauseAction(time)) m_error = true;
	}

	// フェードインアクションを追加する。
	// 引数：sprite_idx スプライト番号。
	//       time フェードインが完了するまでのフレーム単位の時間。
	void addFadeInAction(BYTE sprite_idx, short time) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		if (!psprite->addFadeInAction(time)) m_error = true;
	}

	// フェードアウトアクションを追加する。
	// 引数：sprite_idx スプライト番号。
	//       time フェードアウトが完了するまでのフレーム単位の時間。
	//       noproc_flag trueを指定すると処理済みなら再度処理しないようにする。
	// 返値：成功ならtrue、それ以外はfalse。
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

	// スプライトがY方向に移動するアクションを追加する。
	// 引数：sprite_idx スプライト番号。
	//       start_pos 開始位置。
	//       end_pos 終了位置。
	//       move 移動量。
	void addMoveYAction(BYTE sprite_idx, short start_pos, short end_pos, short move) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		if (!psprite->addMoveYAction(start_pos, end_pos, move)) m_error = true;
	}

	// 透明に設定後、文字列を描画領域に書き込む。
	// 引数：sprite_idx スプライト番号。
	//       ptext 文章。
	//       pname 名前。
	//       bracket 文章を囲む括弧の種類。
	//       noclr_flag trueにすると描画領域を初期化しない。
	//       pos_x 書き込むX位置。
	//       pos_y 書き込むY位置。
	void writeString(BYTE sprite_idx, LPCWSTR ptext, LPCWSTR pname = NULL, BYTE bracket = 0, bool noclr_flag = false, WORD pos_x = 0, WORD pos_y = 0) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		if (!psprite->writeString(ptext, pname, bracket, noclr_flag, pos_x, pos_y)) m_error = true;
	}

	// アクションのループを設定する。
	// 引数：sprite_idx スプライト番号。
	//       loop ループを開始するアクションの0からの位置。負の値を指定するとループしない。
	void setActionLoop(BYTE sprite_idx, short loop) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		psprite->setActionLoop(loop);
	}

	// アクションをリセットする。アクションによって変更された状態はそのまま残ります。
	// 引数：sprite_idx スプライト番号。
	void resetAction(BYTE sprite_idx) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		psprite->resetAction();
	}

	// アクションを再開する。
	// 引数：sprite_idx スプライト番号。
	void restartAction(BYTE sprite_idx) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		psprite->restartAction();
	}

	// アクションを停止する。
	// 引数：sprite_idx スプライト番号。
	void stopAction(BYTE sprite_idx) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		psprite->stopAction();
	}

	// スプライトが有効かどうか設定する。
	// 引数：sprite_idx スプライト番号。
	//       enable 有効ならtrue、それ以外はfalse。
	void setEnable(BYTE sprite_idx, bool enable) {
		Sprite *psprite;

		if (sprite_idx >= SPRITE_CNT) return;
		psprite = m_psprite[sprite_idx];
		if (psprite == NULL) return;

		psprite->setEnable(enable);
	}

	// 毎フレーム実行される処理。
	// 返値：成功ならtrue、それ以外はfalse。
	bool Process() {
		BYTE i;

		if (m_error) return false;

		for (i = 0; i < SPRITE_CNT; i++) {
			if (m_psprite[i] != NULL) m_psprite[i]->Process();
		}
		return true;
	}

	// 描画処理。
	void Draw() {
		BYTE i;

		for (i = 0; i < SPRITE_CNT; i++) {
			if (m_psprite[i] != NULL) m_psprite[i]->Draw();
		}
	}

	// マウスイベント時の処理。
	//   スプライトIDを設定しているスプライトの場合、
	//   マウスの左ボタンを押下したり離したときの位置で最も手前に表示しているスプライトの各種処理を呼び出し、
	//   マウスをウィンドウ内で動かしているときの処理はX・Y位置に応じたスプライトの各種処理を呼び出す。
	// 引数：type 左ボタンを押下時は1、左ボタンを離したときは2、マウスを動かしている動作なら3を指定する。
	//       pos_x X位置。
	//       pos_y Y位置。
	//       noprepare_flag trueにするとウィンドウサイズによる座標の調整をしない。
	// 返値：クリックした場合、スプライトIDを返すが、スプライトIDを設定していないスプライトは0を返す。
	//       それ以外は0。
	BYTE eventMouse(BYTE type, WORD pos_x, WORD pos_y, bool noprepare_flag) {
		short i;
		WORD ret_id = 0, id, spr_pox_x, spr_pox_y;
		RECT rect;
		Sprite *psprite;

		if (!GetClientRect(m_hwnd, &rect)) return 0;
		rect.right -= rect.left;
		rect.bottom -= rect.top;

		if (pos_x >= rect.right || pos_y >= rect.bottom) {	// マウスの位置がウィンドウの外側か？
			i = -1;
		} else {
			// ウィンドウサイズの比率に合わせて座標を調整。
			if (!noprepare_flag) {
				pos_x = (float)pos_x / ((float)rect.right / WINDOW_WIDTH);
				pos_y = (float)pos_y / ((float)rect.bottom / WINDOW_HEIGHT);
			}

			for (i = SPRITE_CNT - 1; i >= 0; i--) {	// 手前に表示しているスプライトから順にチェック。
				psprite = m_psprite[i];
				if (psprite == NULL) continue;
				id = psprite->getID();

				// スプライト内かどうかチェック。
				spr_pox_x = psprite->getPosX();
				spr_pox_y = psprite->getPosY();
				if (pos_x < spr_pox_x || pos_y < spr_pox_y || pos_x >= spr_pox_x + psprite->getWidth()
					|| pos_y >= spr_pox_y + psprite->getHeight()) continue;
				if (id == 0 || !psprite->isEnable()) continue;

				switch (type) {
				case 1:	// マウスの左ボタン押下。
					ret_id = id;
					psprite->onLButtonDown();
					break;
				case 2:	// マウスの左ボタンを離した。
					psprite->onLButtonUp();
					break;
				case 3:	// マウスを動かした。
					if (m_sprite_mousepos == i) return 0;	// 同じスプライトにいるので処理しない。
					psprite->onMouseOver();
					break;
				}

				break;
			}
		}

		if (m_sprite_mousepos != i) {
			if (m_sprite_mousepos > 0 && m_sprite_mousepos < SPRITE_CNT) {
				psprite = m_psprite[m_sprite_mousepos];
				if (psprite != NULL && psprite->isEnable()) psprite->onMouseOut();	// 前回のスプライトのmouseout処理。
			}

			m_sprite_mousepos = i;	// スプライト番号を現在のマウスの位置にあるスプライト番号に設定。
		}

		return ret_id;
	}

	// マウスがスプライト上を離れた時の処理をする。
	// 引数：sprite_idx スプライト番号(全てにしたいなら負の値を指定する)。
	//       all_flag falseなら有効なスプライトのみ、trueなら全て。
	//       range_flag falseならsprite_idxのみ、trueなら0～sprite_idxを対象とする。
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
	short m_sprite_mousepos;	// マウスの位置にあるスプライト番号。
	bool m_error;

	// コンストラクタ。
	// 引数：hwnd 描画ウィンドウハンドル。
	//       pd3dd Direct3Dデバイスオブジェクト。
	SpriteManager(HWND hwnd, LPDIRECT3DDEVICE9 pd3dd) {
		BYTE i;

		m_hwnd = hwnd;
		m_pd3dd = pd3dd;
		for (i = 0; i < SPRITE_CNT; i++) m_psprite[i] = NULL;
		m_sprite_mousepos = -1;
		m_error = false;
	}

	// スプライト作成。
	// 引数：psprite 有効なスプライトオブジェクト。
	//       sprite_idx スプライト番号。
	void setSprite(Sprite *psprite, BYTE sprite_idx) {
		Sprite **ppsprite;

		if (sprite_idx >= SPRITE_CNT) {
			delete psprite;
			return;
		}

		ppsprite = &m_psprite[sprite_idx];
		if (*ppsprite != NULL) delete *ppsprite;	// 作成済みの場合はまず削除。
		*ppsprite = psprite;

		if (!(*ppsprite)->isSuccess()) m_error = true;
	}
};
