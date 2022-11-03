#pragma once
#include "sprite.cpp"

// 文字列スプライトクラス。
class SpriteString : public Sprite {
public:
	// 画像スプライトコンストラクタ。
	// 引数：pd3dd Direct3Dデバイスオブジェクト。
	//       pos_x X位置。
	//       pos_y Y位置。
	//       width 幅。
	//       height 高さ。
	//       size フォントサイズ。
	//       color フォントの色。
	//       color_mo カーソルを合わせた時のフォントの色。変えたくないなら0、黒にしたいなら0xff000000。
	//       id スプライトID。
	SpriteString(LPDIRECT3DDEVICE9 pd3dd, short pos_x, short pos_y, WORD width, WORD height
		, BYTE size, D3DCOLOR color, D3DCOLOR color_mo, BYTE id) : Sprite(pd3dd) {

		m_id = id;
		m_color = color;
		m_color_mo = color_mo;
		m_color_flag = false;

		// フォント作成。
		m_hfont = CreateFont(size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET
			, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("ＭＳ Ｐ明朝"));
		if (m_hfont == NULL) return;

		// 空スプライト作成。
		for (m_scr_width = 1; m_scr_width < width; m_scr_width <<= 1);
		for (m_scr_height = 1; m_scr_height < height; m_scr_height <<= 1);
		if (FAILED(m_pd3dd->CreateTexture(m_scr_width, m_scr_height, 1, 0
			, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pd3dt, NULL))) return;

		m_pos_x = pos_x;
		m_pos_y = pos_y;
		m_width = width;
		m_height = height;
	}

	~SpriteString() {
		if (m_hfont != NULL) DeleteObject(m_hfont);
	}

	// 透明に設定後、文字列を描画領域に書き込む(実行後は透明になったままなので表示処理を実行すること)。
	// 引数：ptext 文章。
	//       pname 名前。
	//       bracket 文章を囲む括弧の種類。
	//       noclr_flag trueにすると描画領域を初期化しない。
	//       pos_x 書き込むX位置。
	//       pos_y 書き込むY位置。
	// 返値：成功ならtrue、それ以外はfalse。
	bool writeString(LPCWSTR ptext, LPCWSTR pname, BYTE bracket, bool noclr_flag, WORD pos_x, WORD pos_y) {
		static LPCWSTR pbracket_start[] = {L"「", L"（", L"『", L" "};
		static LPCWSTR pbracket_end[] = {L"」", L"）", L"』", L""};
		bool succ_flag = false, lock_flag = false;
		LPWSTR pstr = NULL;
		HDC hdc = CreateCompatibleDC(NULL);
		TEXTMETRIC tm;
		D3DLOCKED_RECT d3dlr;

		m_alpha = 0;
		m_color_flag = false;
		if (hdc == NULL) return false;

		SelectObject(hdc, m_hfont);
		if (!GetTextMetrics(hdc, &tm)) goto procend;

		if (FAILED(m_pd3dt->LockRect(0, &d3dlr, NULL, 0))) goto procend;
		lock_flag = true;
		if (!noclr_flag) ZeroMemory(d3dlr.pBits, m_scr_width * m_height * 4);

		if (pname == NULL) {
			if (!writeStringDetail(hdc, &tm, &d3dlr, ptext, &pos_x, &pos_y, 0)) goto procend;
		} else {
			if (bracket >= sizeof(pbracket_start) / sizeof(pbracket_start[0])) bracket = 0;

			if (!writeStringDetail(hdc, &tm, &d3dlr, pname, &pos_x, &pos_y, 0)) goto procend;
			if (!writeStringDetail(hdc, &tm, &d3dlr, pbracket_start[bracket], &pos_x, &pos_y, 0)) goto procend;

			pstr = new WCHAR[lstrlenW(ptext) + 2];
			if (pstr == NULL) goto procend;
			if (lstrcpyW(pstr, ptext) == NULL) goto procend;
			if (lstrcatW(pstr, pbracket_end[bracket]) == NULL) goto procend;

			writeStringDetail(hdc, &tm, &d3dlr, pstr, &pos_x, &pos_y, pos_x);
		}

		succ_flag = true;
procend:
		if (lock_flag) m_pd3dt->UnlockRect(0);
		if (pstr != NULL) delete[] pstr;
		DeleteDC(hdc);
		return succ_flag;
	}

	void onLButtonDown() {
		if (!m_color_flag && m_color_mo > 0) changeColor(m_color_mo);
	}

	void onMouseOver() {
		if (m_color_mo > 0) changeColor(m_color_mo);
		m_color_flag = true;
	}

	void onMouseOut() {
		if (m_color_mo > 0) changeColor(m_color);
		m_color_flag = false;
	}

protected:
	HFONT m_hfont;
	D3DCOLOR m_color, m_color_mo;
	bool m_color_flag;

	// 書き込み対象領域に文字情報を取得しながら1文字ずつ適切な位置に書き込んでいく。
	// 引数：hdc デバイスコンテキスト。
	//       ptm フォントメトリック。
	//       pd3dlr 書き込み対象領域。
	//       pstr 書き込む文字列。改行文字で改行する。横にはみ出る場合や行頭と行末の禁則文字で自動改行する。
	//       ppos_x 書き込みを開始するX位置。処理後、最後に書き込んだ文字の位置に変更される。
	//       ppos_y 書き込みを開始するY位置。処理後、最後に書き込んだ文字の位置に変更される。
	//       nl_pos 改行後に書き込みを開始するX位置。
	// 返値：文字列を正しく書き込めた場合はtrue、それ以外はfalse。
	bool writeStringDetail(HDC hdc, const LPTEXTMETRIC ptm, const D3DLOCKED_RECT *pd3dlr
		, LPCWSTR pstr, WORD *ppos_x, WORD *ppos_y, WORD nl_pos) {

		static MAT2 mat = {{0, 1}, {0, 0}, {0, 0}, {0, 1}};
		LPCWSTR plen = pstr + lstrlenW(pstr);
		BYTE nl_type, *pfont_buf, *pfb;
		WORD i, j;
		DWORD pos, font_buf_size;
		GLYPHMETRICS gm, gm_next;

		for (; pstr < plen; pstr++) {
			// 文字情報を格納するのに必要なバッファサイズを取得する。
			font_buf_size = GetGlyphOutlineW(hdc, *pstr, GGO_GRAY8_BITMAP, &gm, 0, NULL, &mat);
			if (font_buf_size == GDI_ERROR) break;

			// 改行すべきかどうか判定。
			nl_type = 0;
			if (*pstr == L'\n') {
				nl_type = 2;	// 強制改行。
			} else if (*ppos_x > nl_pos) {	// 位置が行の始めの場合は自動改行しない。
				if (*ppos_x + gm.gmCellIncX >= m_width) {
					nl_type = 1;	// 横にはみ出るので改行。
				} else if (pstr + 1 < plen) {
					if (getNormalStrPos(pstr, false) == 0) {	// 今の文字が行頭禁則文字ではない？
						pos = getNormalStrPos(pstr + 1, false);

						for (i = j = 0; i < pos; j += gm_next.gmCellIncX, i++) {
							if (GetGlyphOutlineW(hdc, pstr[1 + i], GGO_GRAY8_BITMAP, &gm_next, 0, NULL, &mat) == GDI_ERROR) break;
							if (*ppos_x + gm.gmCellIncX + j + gm_next.gmCellIncX < m_width) continue;
							nl_type = 1;	// 行頭禁則文字のための改行。
							break;
						}

						if (i < pos && nl_type == 0) break;	// 上記処理の文字情報取得関数が失敗した場合。
					}
					if (nl_type == 0 && getNormalStrPos(pstr, true) > 0) {	// 今の文字が行末禁則文字か？
						if (GetGlyphOutlineW(hdc, pstr[1], GGO_GRAY8_BITMAP, &gm_next, 0, NULL, &mat) == GDI_ERROR) continue;
						if (*ppos_x + gm.gmCellIncX + gm_next.gmCellIncX >= m_width) nl_type = 1;	// 行末禁則文字のための改行。
					}
				}
			}

			if (nl_type > 0) {	// 改行？
				*ppos_x = nl_pos;
				*ppos_y += ptm->tmHeight;
				if (nl_type == 2 || *pstr == L' ' || *pstr == L'　') continue;	// 改行文字か、行頭が空白なら書き込まない。
			}

			if (*ppos_y + ptm->tmHeight > m_height) return true;	// 縦にはみ出る場合はこれ以上の文字の書き込みは不可とする。

			// 文字情報格納バッファを確保する。
			pfont_buf = new BYTE[font_buf_size];
			if (pfont_buf == NULL) break;

			font_buf_size = GetGlyphOutlineW(hdc, *pstr, GGO_GRAY8_BITMAP, &gm, font_buf_size, pfont_buf, &mat);
			if (font_buf_size != GDI_ERROR) {
				// バッファに4の倍数のサイズに調整された文字情報が格納された。

				if (*pstr != L' ' && *pstr != L'　') {	// 空白を描画領域に書き込む必要はない。
					pfb = pfont_buf;
					for (i = 0; i < gm.gmBlackBoxY; i++) {
						for (j = 0; j < gm.gmBlackBoxX; pfb++, j++) {
							if (*pfb <= 0) continue;
							pos = (i + (ptm->tmAscent - gm.gmptGlyphOrigin.y) + *ppos_y) * m_scr_width
								+ (j + gm.gmptGlyphOrigin.x + *ppos_x);
							if (pos >= m_scr_width * m_height) continue;

							// 描画領域に書き込めば後は描画処理で描画される。
							((DWORD *)pd3dlr->pBits)[pos] = ((DWORD)(255 * ((float)*pfb / 64) + 0.5) << 24) + (m_color & 0x00ffffff);
						}
						pfb += (4 - (j & 3)) & 3;	// 4の倍数になるように調整。
					}
				}

				*ppos_x += gm.gmCellIncX;
			}

			delete[] pfont_buf;
			if (font_buf_size == GDI_ERROR) break;
		}

		return pstr == plen;
	}

	// 文字列が禁則文字でなくなった位置を返す。
	// 引数：pstr 調べる文字列。
	//       chchr_end_flag falseなら行頭禁則文字、それ以外は行末禁則文字をチェックする。
	// 返値：文字列が禁則文字でなくなった位置。
	UINT getNormalStrPos(LPCWSTR pstr, bool chchr_end_flag) {
		static LPCWSTR pchchr_start = L"、。，．・！？％」』）｝〕］】ー～…ぁぃぅぇぉゃゅょっァィゥェォャュョッ､｡,.･!?%｣)}]ｰｧｨｩｪｫｬｭｮｯ";
		static LPCWSTR pchchr_end = L"「『（｛〔［【｢({[";

		return wcsspn(pstr, chchr_end_flag ? pchchr_end : pchchr_start);
	}

	// 色を変更する。
	// 引数：color 色。
	void changeColor(D3DCOLOR color) {
		DWORD i, j, *pbuf;
		D3DLOCKED_RECT d3dlr;

		if (FAILED(m_pd3dt->LockRect(0, &d3dlr, NULL, 0))) return;

		pbuf = (DWORD *)d3dlr.pBits;
		for (i = 0; i < m_height; i++) {
			for (j = 0; j < m_width; j++) {
				*pbuf = (*pbuf & 0xff000000) + (color & 0x00ffffff);
				pbuf++;
			}
			pbuf += m_scr_width - m_width;
		}

		m_pd3dt->UnlockRect(0);
	}
};
