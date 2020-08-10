/*
「※」マークコメントにこのサンプルの課題が書かれている。
また、タブ表示、2バイト文字表示にちゃんと対応する等の課題も残っている。
*/

#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <windowsx.h>
#include <imm.h>

/* キャレットの位置を引数x、yに基づいて取得する。
戻り値はxがy行の文字数より多いか、use_xが偽の場合はy行の文字数が返り、それ以外はxがそのまま返る。
ただし、yがstrの改行数(つまり、実際の行数)より多い場合は-2147483648を返す。
y行の文字数がスクロール位置より小さい場合は0を返す。*/
int get_edit_pos(LPTSTR str, int x, int y, HWND hwnd, BOOL use_x)
{
	int size = GlobalSize(str), i, j;

	if (size == 0) return -2147483648;

	y += GetScrollPos(hwnd, SB_VERT);
	for (i = 0; i < size && y > 0; i++)
		if (str[i] == '\n') y--;
	if (y > 0) return -2147483648;	/* 指定した行より実際の行が少ない場合。*/
	if (use_x) {
		x += GetScrollPos(hwnd, SB_HORZ);
		for (j = i; j < size && x > j - i; j++)
			if (str[j] == '\n') break;
	} else
		for (j = i; j < size && str[j] != '\n'; j++);

	return j - i - GetScrollPos(hwnd, SB_HORZ);
}

/* メモリ内の文字列の位置を引数x、yに基づいて取得する。
引数x、yをメモリ内の文字列の位置に対応させ、そのポインタを返す。
yがstrの改行数(つまり、実際の行数)より多い場合はNULLを返す。
xがy行の文字数より多い場合はy行の文字数として処理する。*/
LPTSTR get_edit_str(LPTSTR str, int x, int y, HWND hwnd)
{
	int size = GlobalSize(str), i, j;

	y += GetScrollPos(hwnd, SB_VERT);
	for (i = 0; i < size && y > 0; i++)
		if (str[i] == '\n') y--;
	if (y > 0) return NULL;	/* 指定した行より実際の行が少ない場合。*/
	x += GetScrollPos(hwnd, SB_HORZ);
	for (j = i; j < size && x > j - i; j++)
		if (str[j] == '\n') break;

	return str + j;
}

/* 選択文字列を描画/消去描画する。*/
void draw_selected_text(HWND hwnd, HDC hdc, LPTSTR str, HFONT hfont, LPTEXTMETRIC ptm,
int x, int y, int from_x, int from_y, BOOL clear)
{
	int i, j, k, l, m;

	HideCaret(hwnd);
	SelectObject(hdc, hfont);
	if (clear) {
		SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
	} else {
		SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
	}
	if (y == from_y) {	/* 選択文字列が1行か？ */
		j = y / ptm->tmHeight;
		k = from_y / ptm->tmHeight;
		/* 位置xが大きい方をl、小さいほうをmとする。*/
		if (x < from_x) {
			l = x / ptm->tmAveCharWidth;
			m = from_x / ptm->tmAveCharWidth;
		} else {
			l = from_x / ptm->tmAveCharWidth;
			m = x / ptm->tmAveCharWidth;
		}
		/* 小さい方の指定位置から大きい方の指定位置まで描画する。*/
		TextOut(hdc, l * ptm->tmAveCharWidth, j * ptm->tmHeight, get_edit_str(str, l, j, hwnd), m - l);
	} else {	/* 選択文字列が複数行。*/
		/* 位置yが大きい方をl、小さいほうをmとする。位置xの大小関係は無視できる。*/
		if (y < from_y) {
			j = y / ptm->tmHeight;
			k = from_y / ptm->tmHeight;
			l = x / ptm->tmAveCharWidth;
			m = from_x / ptm->tmAveCharWidth;
		} else {
			j = from_y / ptm->tmHeight;
			k = y / ptm->tmHeight;
			l = from_x / ptm->tmAveCharWidth;
			m = x / ptm->tmAveCharWidth;
		}
		for (i = j; i <= k; i++) {
			/* 指定位置から行の末尾まで描画する。*/
			if (i == j) TextOut(hdc, l * ptm->tmAveCharWidth, j * ptm->tmHeight, get_edit_str(str, l, j, hwnd), get_edit_pos(str, -1, j, hwnd, FALSE) - l);
			/* 行の始めから指定位置まで描画する。*/
			else if (i == k) TextOut(hdc, 0, k * ptm->tmHeight, get_edit_str(str, 0, k, hwnd), m);
			/* 行ごと描画する。*/
			else TextOut(hdc, 0, i * ptm->tmHeight, get_edit_str(str, 0, i, hwnd), get_edit_pos(str, -1, i, hwnd, FALSE));
		}
	}
	ShowCaret(hwnd);
}

typedef struct {
	int min_x;	/* 選択位置の小さい方のx。*/
	int min_y;	/* 選択位置の小さい方のy。*/
	int max_x;	/* 選択位置の大きい方のx。*/
	int max_y;	/* 選択位置の大きい方のy。*/
	int sel_cnt;	/* 選択文字数。*/
	int sel_cnt2;	/* 指定位置が小さい場合に選択文字数を設定します。それ以外は0です。*/
} EDITSELPOS;

/* 選択文字列情報を取得(pespに設定)する。
選択されていない場合は小さい方と大きい方の位置は同じになり、選択文字数は0になります。*/
void get_edit_selection_pos(LPTSTR str, HWND hwnd, LPTEXTMETRIC ptm, BOOL *pselected,
int x, int y, int from_x, int from_y, EDITSELPOS *pesp)
{
	if (*pselected) {	/* 選択されているか？ */
		if (y == from_y) {	/* 選択文字列が1行か？ */
			/* min_yもmax_y同じ値になる。*/
			pesp->min_y = y / ptm->tmHeight;
			pesp->max_y = from_y / ptm->tmHeight;

			if (x < from_x) {
				pesp->min_x = x / ptm->tmAveCharWidth;
				pesp->max_x = from_x / ptm->tmAveCharWidth;
				pesp->sel_cnt2 = 1;
			} else {
				pesp->min_x = from_x / ptm->tmAveCharWidth;
				pesp->max_x = x / ptm->tmAveCharWidth;
				pesp->sel_cnt2 = 0;
			}
		} else {
			if (y < from_y) {
				pesp->min_y = y / ptm->tmHeight;
				pesp->max_y = from_y / ptm->tmHeight;
				pesp->min_x = x / ptm->tmAveCharWidth;
				pesp->max_x = from_x / ptm->tmAveCharWidth;
				pesp->sel_cnt2 = 1;
			} else {
				pesp->min_y = from_y / ptm->tmHeight;
				pesp->max_y = y / ptm->tmHeight;
				pesp->min_x = from_x / ptm->tmAveCharWidth;
				pesp->max_x = x / ptm->tmAveCharWidth;
				pesp->sel_cnt2 = 0;
			}
		}
		pesp->sel_cnt = get_edit_str(str, pesp->max_x, pesp->max_y, hwnd) - get_edit_str(str, pesp->min_x, pesp->min_y, hwnd);
		if (pesp->sel_cnt2) pesp->sel_cnt2 = pesp->sel_cnt;

		*pselected = FALSE;
	} else {
		pesp->sel_cnt = pesp->sel_cnt2 = 0;
		pesp->min_y = pesp->max_y = y / ptm->tmHeight;
		pesp->min_x = pesp->max_x = x / ptm->tmAveCharWidth;
	}
}

/* スクロールバーの設定。vは増減量。-2147483648を指定すると再描画を行う。*/
void set_max_line(HWND hwnd, LPTEXTMETRIC ptm, int h, int v)
{
	int i;
	RECT rect;
	SCROLLINFO si;
	static int hline, vline;

	/* 横 */
	if (h != -2147483648) h += GetScrollPos(hwnd, SB_HORZ);
	if (hline < h || h == -2147483648) {
		if (h != -2147483648) hline = h;
		GetClientRect(hwnd, &rect);
		if ((i = hline - (rect.right - rect.left) / ptm->tmAveCharWidth + 1) > 0 || h == -2147483648) {
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_PAGE | SIF_RANGE;
			si.nPage = (rect.right - rect.left) / ptm->tmAveCharWidth;
			si.nMin = 0;
			si.nMax = i + si.nPage - 1;
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		}
	}
	/* 縦 */
	if (v != 0 || v == -2147483648) {
		if (v != -2147483648) vline += v;
		GetClientRect(hwnd, &rect);
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE;
		si.nPage = (rect.bottom - rect.top) / ptm->tmHeight;
		si.nMin = 0;
		si.nMax = vline + si.nPage - 1;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
	}
}

/* 位置が画面外にあるとき、スクロールバーを移動させる。*/
void set_pos(HWND hwnd, LPTEXTMETRIC ptm, int *x, int *y, int *from_x, int *from_y)
{
	int i, j, k, l;
	RECT rect;

	i = GetScrollPos(hwnd, SB_HORZ);
	if (*x < 0)	/* 位置が画面より左にあるか？ */
		SetScrollPos(hwnd, SB_HORZ, i + *x / ptm->tmAveCharWidth, TRUE);
	else {
		GetClientRect(hwnd, &rect);
		if (*x + ptm->tmAveCharWidth > rect.right - rect.left) /* 位置が画面より右にあるか？ */
			SetScrollPos(hwnd, SB_HORZ, i + *x / ptm->tmAveCharWidth - (rect.right - rect.left) / ptm->tmAveCharWidth + 1, TRUE);
	}
	j = GetScrollPos(hwnd, SB_HORZ);
	if (i != j) {
		/* 左へ進むと正、下へ進むと負にする。*/
		*x += (i - j) * ptm->tmAveCharWidth;
		*from_x += (i - j) * ptm->tmAveCharWidth;
	}

	k = GetScrollPos(hwnd, SB_VERT);
	if (*y < 0)	/* 位置が画面より上にあるか？ */
		SetScrollPos(hwnd, SB_VERT, k + *y / ptm->tmHeight, TRUE);
	else {
		GetClientRect(hwnd, &rect);
		if (*y + ptm->tmHeight > rect.bottom - rect.top) /* 位置が画面より下にあるか？ */
			SetScrollPos(hwnd, SB_VERT, k + *y / ptm->tmHeight - (rect.bottom - rect.top) / ptm->tmHeight + 1, TRUE);
	}
	l = GetScrollPos(hwnd, SB_VERT);
	if (k != l) {
		/* 上へ進むと正、下へ進むと負にする。*/
		*y += (k - l) * ptm->tmHeight;
		*from_y += (k - l) * ptm->tmHeight;
	}

	if (i != j || k != l) InvalidateRect(hwnd, NULL, TRUE);
}

/* キャレットの位置を位置x、yに飽和設定する。*/
void set_edit_pos(LPTSTR str, int *x, int *y, int *from_x, int *from_y, HWND hwnd, LPTEXTMETRIC ptm)
{
	int size = GlobalSize(str), i, j, k;

	k = *y;
	k += GetScrollPos(hwnd, SB_VERT);
	for (i = j = 0; i < size && k > 0; i++)
		if (str[i] == '\n') {
			j = i + 1;
			k--;
		}

	*x += GetScrollPos(hwnd, SB_HORZ);
	for (i = j; j < size && *x > j - i; j++)
		if (str[j] == '\n') break;

	*x = (j - i - GetScrollPos(hwnd, SB_HORZ)) * ptm->tmAveCharWidth;
	*y = (*y - k) * ptm->tmHeight;
	set_pos(hwnd, ptm, x, y, from_x, from_y);
	SetCaretPos(*x, *y);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int i, j, k;
	float l;
	LPTSTR pstr;
	POINT point;
	RECT rect;
	PAINTSTRUCT ps;
	HDC hdc;
	HGLOBAL halc;
	HMENU hmenu;
	HIMC himc;
	COMPOSITIONFORM composform;
	EDITSELPOS esp;
	static int caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y;
	static BOOL selected = FALSE;
	static LPTSTR str = NULL;
	static LOGFONT logfont = {13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
	CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "ＭＳ ゴシック"};
	static TEXTMETRIC tm;
	static HFONT hfont;

	switch (uMsg) {
	case WM_CREATE:
		hdc = GetDC(hwnd);
		hfont = CreateFontIndirect(&logfont);
		SelectObject(hdc, hfont);
		GetTextMetrics(hdc, &tm);
		ReleaseDC(hwnd, hdc);
		caret_pos_x = caret_pos_y = 0;
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
		SelectObject(hdc, hfont);
		/* 行毎に描画。*/
		for (i = 0; (j = get_edit_pos(str, -1, i, hwnd, FALSE)) != -2147483648; i++) TextOut(hdc, 0, i * tm.tmHeight, get_edit_str(str, 0, i, hwnd), j);
		if (selected) draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, FALSE);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_SIZE:
		i = GetScrollPos(hwnd, SB_HORZ);

		set_max_line(hwnd, &tm, -2147483648, -2147483648);

		if (caret_pos_x < 0)	/* 位置が画面より左にあるか？ */
			SetScrollPos(hwnd, SB_HORZ, i + caret_pos_x / tm.tmAveCharWidth, TRUE);
		else {
			GetClientRect(hwnd, &rect);
			if (caret_pos_x + tm.tmAveCharWidth > rect.right - rect.left) /* 位置が画面より右にあるか？ */
				SetScrollPos(hwnd, SB_HORZ, i + caret_pos_x / tm.tmAveCharWidth - (rect.right - rect.left) / tm.tmAveCharWidth + 1, TRUE);
		}
		j = GetScrollPos(hwnd, SB_HORZ);
		if (i != j) {
			/* 左へ進むと正、下へ進むと負にする。*/
			caret_pos_x += (i - j) * tm.tmAveCharWidth;
			caret_from_pos_x += (i - j) * tm.tmAveCharWidth;
			SetCaretPos(caret_pos_x, caret_pos_y);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;
	case WM_SETFOCUS:
		CreateCaret(hwnd, NULL, 1, tm.tmHeight);
		ShowCaret(hwnd);
		return 0;
	case WM_KILLFOCUS:
		DestroyCaret();
		return 0;
	case WM_LBUTTONDOWN:
		if (selected) {
			hdc = GetDC(hwnd);
			draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
			ReleaseDC(hwnd, hdc);
			selected = FALSE;
		}

		/* マウスの位置をテキストの位置に合うようにする。*/
		l = (float)GET_X_LPARAM(lParam) / tm.tmAveCharWidth;
		caret_pos_x = l > 0 ? l + .5 : l - 1;
		l = (float)GET_Y_LPARAM(lParam) / tm.tmHeight;
		caret_pos_y = l > 0 ? l : l - 1;
		set_edit_pos(str, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y, hwnd, &tm);
		/* 選択開始位置も設定しておく。*/
		caret_from_pos_x = caret_pos_x;
		caret_from_pos_y = caret_pos_y;

		SetCapture(hwnd);
		return 0;
	case WM_LBUTTONUP:
		ReleaseCapture();
		return 0;
	case WM_MOUSEMOVE:
		if (!(wParam & MK_LBUTTON)) break;

		hdc = GetDC(hwnd);
		/* 一度、消去描画を行う。※良法を検討すべきである。*/
		draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);

		/* マウスの位置をテキストの位置に合うようにする。*/
		l = (float)GET_X_LPARAM(lParam) / tm.tmAveCharWidth;
		caret_pos_x = l > 0 ? l + .5 : l - 1;
		l = (float)GET_Y_LPARAM(lParam) / tm.tmHeight;
		caret_pos_y = l > 0 ? l : l - 1;
		set_edit_pos(str, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y, hwnd, &tm);

		selected = caret_pos_x != caret_from_pos_x || caret_pos_y != caret_from_pos_y;
		/* 描画。*/
		draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, FALSE);

		ReleaseDC(hwnd, hdc);
		return 0;
	case WM_RBUTTONDOWN:
		hmenu = CreatePopupMenu();
		AppendMenu(hmenu, MF_STRING | (selected > 0 ? 0 : MF_GRAYED), 100, "切り取り(&T)");
		AppendMenu(hmenu, MF_STRING | (selected > 0 ? 0 : MF_GRAYED), 101, "コピー(&C)");

		j = IsClipboardFormatAvailable(CF_TEXT) ? 0 : MF_GRAYED;
		AppendMenu(hmenu, MF_STRING, 102, "貼り付け(&P)");

		AppendMenu(hmenu, MF_STRING | (selected > 0 ? 0 : MF_GRAYED), 103, "削除(&D)");
		AppendMenu(hmenu, MF_SEPARATOR, 0, NULL);

		for (i = 0; get_edit_pos(str, 0, i, hwnd, TRUE) != -2147483648; i++);
		i--;
		/* 文字列があり、すべて選択されていなければ有効にする。*/
		if (i >= 0 && (!selected || caret_from_pos_x != -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth ||
			caret_from_pos_y != -GetScrollPos(hwnd, SB_VERT) * tm.tmHeight ||
			caret_pos_x != get_edit_pos(str, -1, i, hwnd, FALSE) * tm.tmAveCharWidth ||
			caret_pos_y != i * tm.tmHeight)) j = 0;
		else
			j = MF_GRAYED;
		AppendMenu(hmenu, MF_STRING | j, 104, "すべて選択(&A)");

		GetCursorPos(&point);
		TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, point.x, point.y, 0, hwnd, NULL);
		DestroyMenu(hmenu);
		return 0;
	case WM_MOUSEWHEEL:
		i = GetScrollPos(hwnd, SB_VERT);
		SetScrollPos(hwnd, SB_VERT, GetScrollPos(hwnd, SB_VERT) + (GET_WHEEL_DELTA_WPARAM(wParam) < 0 ? 3 : -3), TRUE);
		j = GetScrollPos(hwnd, SB_VERT);
		if (i != j) {
			/* 上へ進むと正、下へ進むと負にする。*/
			caret_pos_y += (i - j) * tm.tmHeight;
			caret_from_pos_y += (i - j) * tm.tmHeight;
			SetCaretPos(caret_pos_x, caret_pos_y);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;
	case WM_KEYDOWN:
		if (GetCapture() != NULL) break;

		switch (wParam) {
		case VK_UP:
			if (GetKeyState(VK_SHIFT) < 0) {
				if (caret_pos_y <= -GetScrollPos(hwnd, SB_VERT) * tm.tmHeight) break;

				/* 選択開始位置を保存する。*/
				if (!selected) {
					caret_from_pos_x = caret_pos_x;
					caret_from_pos_y = caret_pos_y;
					selected = TRUE;
				}

				HideCaret(hwnd);
				hdc = GetDC(hwnd);
				SelectObject(hdc, hfont);

				/* 選択/通常文字列表示の設定。*/
				if (caret_pos_y <= caret_from_pos_y) {
					SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
					SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
				} else {
					SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
					SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
				}

				/* 選択/通常文字列を描画する。*/
				i = get_edit_pos(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd, TRUE);
				TextOut(hdc, 0, caret_pos_y, get_edit_str(str, 0, caret_pos_y / tm.tmHeight, hwnd), i);
				i = get_edit_pos(str, -1, caret_pos_y / tm.tmHeight - 1, hwnd, FALSE) - caret_pos_x / tm.tmAveCharWidth;
				TextOut(hdc, caret_pos_x, caret_pos_y - tm.tmHeight, get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight - 1, hwnd), i);

				/* 位置によっては別の位置の選択/通常文字列も描画する。*/
				if (caret_pos_y - tm.tmHeight == caret_from_pos_y && caret_pos_x < caret_from_pos_x) {
					SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
					SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
					i = caret_from_pos_x / tm.tmAveCharWidth - caret_pos_x / tm.tmAveCharWidth;
					TextOut(hdc, caret_pos_x, caret_pos_y - tm.tmHeight, get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight - 1, hwnd), i);
				} else if (caret_pos_y == caret_from_pos_y && caret_pos_x > caret_from_pos_x) {
					SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
					SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
					i = caret_pos_x / tm.tmAveCharWidth - caret_from_pos_x / tm.tmAveCharWidth;
					TextOut(hdc, caret_from_pos_x, caret_from_pos_y, get_edit_str(str, caret_from_pos_x / tm.tmAveCharWidth, caret_from_pos_y / tm.tmHeight, hwnd), i);
				}

				ReleaseDC(hwnd, hdc);
				ShowCaret(hwnd);
			} else if (selected) {
				hdc = GetDC(hwnd);
				draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
				ReleaseDC(hwnd, hdc);
				selected = FALSE;
			}

			if (caret_pos_y > -GetScrollPos(hwnd, SB_VERT) * tm.tmHeight) {	/* 上に移動できるか？ */
				caret_pos_y -= tm.tmHeight;
				caret_pos_x = get_edit_pos(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd, TRUE) * tm.tmAveCharWidth;
			}
			set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
			break;
		case VK_DOWN:
			if (GetKeyState(VK_SHIFT) < 0) {
				if (get_edit_pos(str, 0, caret_pos_y / tm.tmHeight + 1, hwnd, TRUE) == -2147483648) break;

				/* 選択開始位置を保存する。*/
				if (!selected) {
					caret_from_pos_x = caret_pos_x;
					caret_from_pos_y = caret_pos_y;
					selected = TRUE;
				}

				HideCaret(hwnd);
				hdc = GetDC(hwnd);
				SelectObject(hdc, hfont);

				/* 選択/通常文字列表示の設定。*/
				if (caret_pos_y >= caret_from_pos_y) {
					SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
					SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
				} else {
					SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
					SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
				}

				/* 選択/通常文字列を描画する。*/
				i = get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE) - caret_pos_x / tm.tmAveCharWidth;
				TextOut(hdc, caret_pos_x, caret_pos_y, get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd), i);
				i = get_edit_pos(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight + 1, hwnd, TRUE);
				TextOut(hdc, 0, caret_pos_y + tm.tmHeight, get_edit_str(str, 0, caret_pos_y / tm.tmHeight + 1, hwnd), i);

				/* 位置によっては別の位置の選択/通常文字列も描画する。*/
				if (caret_pos_y == caret_from_pos_y && caret_pos_x < caret_from_pos_x) {
					SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
					SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
					i = caret_from_pos_x / tm.tmAveCharWidth - caret_pos_x / tm.tmAveCharWidth;
					TextOut(hdc, caret_pos_x, caret_pos_y, get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd), i);
				} else if (caret_pos_y + tm.tmHeight == caret_from_pos_y && caret_pos_x > caret_from_pos_x) {
					SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
					SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
					i = caret_pos_x / tm.tmAveCharWidth - caret_from_pos_x / tm.tmAveCharWidth;
					TextOut(hdc, caret_from_pos_x, caret_from_pos_y, get_edit_str(str, caret_from_pos_x / tm.tmAveCharWidth, caret_from_pos_y / tm.tmHeight, hwnd), i);
				}

				ReleaseDC(hwnd, hdc);
				ShowCaret(hwnd);
			} else if (selected) {
				hdc = GetDC(hwnd);
				draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
				ReleaseDC(hwnd, hdc);
				selected = FALSE;
			}

			if ((i = get_edit_pos(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight + 1, hwnd, TRUE)) != -2147483648) {	/* 下に移動できるか？ */
				caret_pos_y += tm.tmHeight;
				caret_pos_x = i * tm.tmAveCharWidth;
			}
			set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
			break;
		case VK_LEFT:
			if (caret_pos_x > -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth) {
				if (GetKeyState(VK_SHIFT) < 0) {
					/* 選択開始位置を保存する。*/
					if (!selected) {
						caret_from_pos_x = caret_pos_x;
						caret_from_pos_y = caret_pos_y;
						selected = TRUE;
					}

					HideCaret(hwnd);
					hdc = GetDC(hwnd);
					SelectObject(hdc, hfont);

					/* 選択/通常文字列表示の設定。*/
					if (caret_pos_y == caret_from_pos_y && caret_pos_x <= caret_from_pos_x ||
					caret_pos_y < caret_from_pos_y) {
						SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
						SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
					} else {
						SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
						SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
					}

					/* 選択/通常文字列を描画する。*/
					TextOut(hdc, caret_pos_x - tm.tmAveCharWidth, caret_pos_y, get_edit_str(str, caret_pos_x / tm.tmAveCharWidth - 1, caret_pos_y / tm.tmHeight, hwnd), 1);

					ReleaseDC(hwnd, hdc);
					ShowCaret(hwnd);
				} else if (selected) {
					hdc = GetDC(hwnd);
					draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
					ReleaseDC(hwnd, hdc);
					selected = FALSE;
				}

				caret_pos_x -= tm.tmAveCharWidth;
			} else if (caret_pos_y > -GetScrollPos(hwnd, SB_VERT) * tm.tmHeight) {
				if (GetKeyState(VK_SHIFT) < 0) {
					/* 選択開始位置を保存する。*/
					if (!selected) {
						caret_from_pos_x = caret_pos_x;
						caret_from_pos_y = caret_pos_y;
						selected = TRUE;
					}
				} else if (selected) {
					hdc = GetDC(hwnd);
					draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
					ReleaseDC(hwnd, hdc);
					selected = FALSE;
				}

				caret_pos_y -= tm.tmHeight;
				caret_pos_x = get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE) * tm.tmAveCharWidth;
			} else if (selected && GetKeyState(VK_SHIFT) >= 0) {
				hdc = GetDC(hwnd);
				draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
				ReleaseDC(hwnd, hdc);
				selected = FALSE;
			}
			set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
			break;
		case VK_RIGHT:
			if (caret_pos_x / tm.tmAveCharWidth < get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE)) {
				if (GetKeyState(VK_SHIFT) < 0) {
					/* 選択開始位置を保存する。*/
					if (!selected) {
						caret_from_pos_x = caret_pos_x;
						caret_from_pos_y = caret_pos_y;
						selected = TRUE;
					}

					HideCaret(hwnd);
					hdc = GetDC(hwnd);
					SelectObject(hdc, hfont);

					/* 選択/通常文字列表示の設定。*/
					if (caret_pos_y == caret_from_pos_y && caret_pos_x >= caret_from_pos_x ||
					caret_pos_y > caret_from_pos_y) {
						SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
						SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
					} else {
						SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
						SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
					}

					/* 選択/通常文字列を描画する。*/
					TextOut(hdc, caret_pos_x, caret_pos_y, get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd), 1);

					ReleaseDC(hwnd, hdc);
					ShowCaret(hwnd);
				} else if (selected) {
					hdc = GetDC(hwnd);
					draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
					ReleaseDC(hwnd, hdc);
					selected = FALSE;
				}

				caret_pos_x += tm.tmAveCharWidth;
			} else if ((i = get_edit_pos(str, 0, caret_pos_y / tm.tmHeight + 1, hwnd, TRUE)) != -2147483648) {
				if (GetKeyState(VK_SHIFT) < 0) {
					/* 選択開始位置を保存する。*/
					if (!selected) {
						caret_from_pos_x = caret_pos_x;
						caret_from_pos_y = caret_pos_y;
						selected = TRUE;
					}
				} else if (selected) {
					hdc = GetDC(hwnd);
					draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
					ReleaseDC(hwnd, hdc);
					selected = FALSE;
				}

				caret_pos_x = -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth;
				caret_pos_y += tm.tmHeight;
			} else if (selected && GetKeyState(VK_SHIFT) >= 0) {
				hdc = GetDC(hwnd);
				draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
				ReleaseDC(hwnd, hdc);
				selected = FALSE;
			}
			set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
			break;
		case VK_HOME:
			caret_pos_x = -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth;
			set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
			break;
		case VK_END:
			caret_pos_x = get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE) * tm.tmAveCharWidth;
			set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
			break;
		case VK_DELETE:
			/* 文字列が選択されていないなら位置を1つ先に移動させてからBack Spaceと同じ処理を行う。
			そうでない場合はBack Spaceと同じ処理とする。*/
			if (caret_pos_x / tm.tmAveCharWidth < get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE)) {
				if (!selected) caret_pos_x += tm.tmAveCharWidth;
				goto backspace;
			} else if ((i = get_edit_pos(str, 0, caret_pos_y / tm.tmHeight + 1, hwnd, TRUE)) != -2147483648) {
				if (!selected) {
					caret_pos_x = -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth;
					caret_pos_y += tm.tmHeight;
				}
				goto backspace;
			} else if (selected)
				goto backspace;
			MessageBeep(MB_OK);
			break;
		}
		SetCaretPos(caret_pos_x, caret_pos_y);
		return 0;
	case WM_CHAR:
		if (GetCapture() != NULL) break;

		/* ※メモリから文字列を削除する前の行等の情報が取得できればRectangleのrect.rightを回避できるだろう。*/
		switch (wParam) {
		case 0x08:	/* Back Space */
			if (caret_pos_x <= -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth &&
			caret_pos_y <= -GetScrollPos(hwnd, SB_VERT) * tm.tmHeight && !selected) {
				MessageBeep(MB_OK);
				return 0;
			}
backspace:
			get_edit_selection_pos(str, hwnd, &tm, &selected, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, &esp);

			/* 削除前の大きい方の位置の行の末尾と削除前の大きい方の指定位置の差の位置を保存する。削除して行が変わる場合に使用する。*/
			j = (get_edit_pos(str, -1, esp.max_y, hwnd, FALSE) - esp.max_x) * tm.tmAveCharWidth;

			/* メモリ内の指定位置の1文字/選択文字列を削除する。
			文字列が選択されていない場合は1文字削除とし、そうでない場合は選択文字列のみ削除とする。*/
			pstr = get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd);
			MoveMemory(pstr - (esp.sel_cnt > 0 ? (esp.sel_cnt2 ? 0 : esp.sel_cnt) : 1), pstr + esp.sel_cnt2, GlobalSize(str) - (pstr + esp.sel_cnt2 - str));
			str = (LPTSTR)GlobalReAlloc(str, GlobalSize(str) - (esp.sel_cnt > 0 ? esp.sel_cnt : 1), GMEM_MOVEABLE);

			/* 位置を選択文字列の小さい方の位置に設定する。*/
			caret_pos_x = esp.min_x * tm.tmAveCharWidth;
			caret_pos_y = esp.min_y * tm.tmHeight;

			hdc = GetDC(hwnd);
			SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
			SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
			SelectObject(hdc, hfont);
			HideCaret(hwnd);
			GetClientRect(hwnd, &rect);
			SelectObject(hdc, CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW)));
			SelectObject(hdc, GetSysColorBrush(COLOR_WINDOW));

			if (caret_pos_x > -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth) {
				if (esp.sel_cnt == 0) caret_pos_x -= tm.tmAveCharWidth;

				k = 0;	/* 選択文字列用。*/
				set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
				set_max_line(hwnd, &tm, get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE), esp.min_y - esp.max_y);
			} else {
				if (esp.sel_cnt == 0) {
					caret_pos_y -= tm.tmHeight;
					if (caret_pos_y < -GetScrollPos(hwnd, SB_VERT) * tm.tmHeight) caret_pos_y = -GetScrollPos(hwnd, SB_VERT) * tm.tmHeight;	/* 選択文字列時、位置yが負になる場合があるため。*/
					/* 上の行に移動の際、位置xを残りの文字分ずらす。*/
					caret_pos_x = get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE) * tm.tmAveCharWidth - j;
				}

				/* 削除された行より下の行を描画する。*/
				for (i = caret_pos_y / tm.tmHeight + 1; get_edit_pos(str, 0, i, hwnd, TRUE) != -2147483648; i++) {
					Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight);
					TextOut(hdc, 0, i * tm.tmHeight, get_edit_str(str, 0, i, hwnd), get_edit_pos(str, -1, i, hwnd, FALSE));
				}
				Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight);

				k = 1;	/* 選択文字列用。*/
				set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
				set_max_line(hwnd, &tm, get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE), (esp.sel_cnt > 0 ? 0 : -1) - (esp.max_y - esp.min_y));
			}
			/* 削除した文字の行を削除した文字の位置から描画する。*/
			i = get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE) - caret_pos_x / tm.tmAveCharWidth;
			TextOut(hdc, caret_pos_x, caret_pos_y, get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd), i);
			/* 行の末尾からその行を消去描画する。*/
			Rectangle(hdc, caret_pos_x + i * tm.tmAveCharWidth, caret_pos_y, rect.right, caret_pos_y + tm.tmHeight);

			/* 選択文字列が複数行の場合、削除された行より下の行を描画する。*/
			if (esp.min_y < esp.max_y) {
				for (i = esp.min_y + 1; get_edit_pos(str, 0, i, hwnd, TRUE) != -2147483648; i++) {
					Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight);
					TextOut(hdc, 0, i * tm.tmHeight, get_edit_str(str, 0, i, hwnd), get_edit_pos(str, -1, i, hwnd, FALSE));
				}
				/* 削除された複数行を消去描画する。*/
				Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight * (esp.max_y - esp.min_y + k));
			}

			DeleteObject(SelectObject(hdc, GetStockObject(BLACK_PEN)));
			ReleaseDC(hwnd, hdc);
			break;
		case 0x0a:	/* Enter */
		case 0x0d:
			get_edit_selection_pos(str, hwnd, &tm, &selected, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, &esp);

			/* 文字列が選択されている/いない状態や、文字列の途中の位置に入力しても
			メモリ内の文字列の適切な位置に改行文字を設定する。*/
			str = str == NULL ? (LPTSTR)GlobalAlloc(GMEM_FIXED, sizeof(TCHAR)) : (LPTSTR)GlobalReAlloc(str, GlobalSize(str) + sizeof(TCHAR), GMEM_MOVEABLE);
			pstr = get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd);
			MoveMemory(pstr + 1 - (esp.sel_cnt2 ? 0 : esp.sel_cnt), pstr + esp.sel_cnt2, GlobalSize(str) - (pstr + esp.sel_cnt2 - str) - 1);
			pstr[esp.sel_cnt2 ? 0 : -esp.sel_cnt] = '\n';
			if (esp.sel_cnt > 0) str = (LPTSTR)GlobalReAlloc(str, GlobalSize(str) - esp.sel_cnt, GMEM_MOVEABLE);

			/* 位置を選択文字列の小さい方の位置に設定する。*/
			caret_pos_x = esp.min_x * tm.tmAveCharWidth;
			caret_pos_y = esp.min_y * tm.tmHeight;

			hdc = GetDC(hwnd);
			SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
			SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
			SelectObject(hdc, hfont);
			HideCaret(hwnd);
			GetClientRect(hwnd, &rect);
			SelectObject(hdc, CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW)));
			SelectObject(hdc, GetSysColorBrush(COLOR_WINDOW));
			/* 追加された行以降を描画する。*/
			for (i = esp.min_y; get_edit_pos(str, 0, i, hwnd, TRUE) != -2147483648; i++) {
				Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight);
				TextOut(hdc, 0, i * tm.tmHeight, get_edit_str(str, 0, i, hwnd), get_edit_pos(str, -1, i, hwnd, FALSE));
			}

			/* 選択文字列が複数行の場合、削除された行より下の行を描画する。*/
			if (esp.min_y < esp.max_y) {
				for (i = esp.min_y + 1; get_edit_pos(str, 0, i, hwnd, TRUE) != -2147483648; i++) {
					Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight);
					TextOut(hdc, 0, i * tm.tmHeight, get_edit_str(str, 0, i, hwnd), get_edit_pos(str, -1, i, hwnd, FALSE));
				}
				/* 削除された複数行を消去描画する。*/
				Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight * (esp.max_y - esp.min_y));
			}

			DeleteObject(SelectObject(hdc, GetStockObject(BLACK_PEN)));
			ReleaseDC(hwnd, hdc);

			caret_pos_x = -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth;
			caret_pos_y += tm.tmHeight;
			set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
			set_max_line(hwnd, &tm, get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE), 1 - (esp.max_y - esp.min_y));
			break;
		case 0x1b:	/* Escape */
			if (!selected) break;
			hdc = GetDC(hwnd);
			draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, TRUE);
			ReleaseDC(hwnd, hdc);
			selected = FALSE;
			break;
		default:
			get_edit_selection_pos(str, hwnd, &tm, &selected, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, &esp);

			/* 文字列が選択されている/いない状態や、文字列の途中の位置に入力しても
			メモリ内の文字列の適切な位置に入力された1文字を設定する。*/
			str = str == NULL ? (LPTSTR)GlobalAlloc(GMEM_FIXED, sizeof(TCHAR)) : (LPTSTR)GlobalReAlloc(str, GlobalSize(str) + sizeof(TCHAR), GMEM_MOVEABLE);
			pstr = get_edit_str(str, caret_pos_x / tm.tmAveCharWidth, caret_pos_y / tm.tmHeight, hwnd);
			MoveMemory(pstr + 1 - (esp.sel_cnt2 ? 0 : esp.sel_cnt), pstr + esp.sel_cnt2, GlobalSize(str) - (pstr + esp.sel_cnt2 - str) - 1);
			pstr[esp.sel_cnt2 ? 0 : -esp.sel_cnt] = (TCHAR)wParam;
			if (esp.sel_cnt > 0) str = (LPTSTR)GlobalReAlloc(str, GlobalSize(str) - esp.sel_cnt, GMEM_MOVEABLE);

			/* 位置を選択文字列の小さい方の位置に設定する。*/
			caret_pos_x = esp.min_x * tm.tmAveCharWidth;
			caret_pos_y = esp.min_y * tm.tmHeight;

			hdc = GetDC(hwnd);
			SelectObject(hdc, hfont);
			HideCaret(hwnd);
			GetClientRect(hwnd, &rect);

			SelectObject(hdc, CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW)));
			SelectObject(hdc, GetSysColorBrush(COLOR_WINDOW));
			/* まず、追加された行以降を消去描画する。*/
			Rectangle(hdc, caret_pos_x, caret_pos_y, rect.right, caret_pos_y + tm.tmHeight);

			/* 挿入した文字の行を挿入した文字の位置から描画する。*/
			SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
			SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
			i = get_edit_pos(str, -1, esp.min_y, hwnd, FALSE) - esp.min_x;
			TextOut(hdc, caret_pos_x, caret_pos_y, get_edit_str(str, esp.min_x, esp.min_y, hwnd), i);

			/* 選択文字列が複数行の場合、削除された行より下の行を描画する。*/
			if (esp.min_y < esp.max_y) {
				for (i = esp.min_y + 1; get_edit_pos(str, 0, i, hwnd, TRUE) != -2147483648; i++) {
					Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight);
					TextOut(hdc, 0, i * tm.tmHeight, get_edit_str(str, 0, i, hwnd), get_edit_pos(str, -1, i, hwnd, FALSE));
				}
				/* 削除された複数行を消去描画する。*/
				Rectangle(hdc, 0, i * tm.tmHeight, rect.right, i * tm.tmHeight + tm.tmHeight * (esp.max_y - esp.min_y));
			}

			DeleteObject(SelectObject(hdc, GetStockObject(BLACK_PEN)));
			ReleaseDC(hwnd, hdc);

			caret_pos_x += tm.tmAveCharWidth;
			set_pos(hwnd, &tm, &caret_pos_x, &caret_pos_y, &caret_from_pos_x, &caret_from_pos_y);
			set_max_line(hwnd, &tm, get_edit_pos(str, -1, caret_pos_y / tm.tmHeight, hwnd, FALSE), esp.min_y - esp.max_y);
		}
		SetCaretPos(caret_pos_x, caret_pos_y);
		ShowCaret(hwnd);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 100:	/* 「切り取り」 */
		case 101:	/* 「コピー」 */
			if (!selected || !OpenClipboard(hwnd) || !EmptyClipboard()) break;

			i = selected;	/* ダミーを使用してselectedは保つ。*/
			get_edit_selection_pos(str, hwnd, &tm, &i, caret_pos_x, caret_pos_y, caret_from_pos_x, caret_from_pos_y, &esp);

			halc = GlobalAlloc(GMEM_MOVEABLE, esp.sel_cnt + 1);
			pstr = GlobalLock(halc);
			CopyMemory(pstr, get_edit_str(str, esp.min_x, esp.min_y, hwnd), esp.sel_cnt);
			pstr[esp.sel_cnt] = '\0';
			GlobalUnlock(halc);
			SetClipboardData(CF_TEXT, halc);
			CloseClipboard();

			if (LOWORD(wParam) == 100) goto backspace;
			break;
		case 102:	/* 「貼り付け」 */
			if (!IsClipboardFormatAvailable(CF_TEXT) || !OpenClipboard(hwnd)) break;

			halc = GetClipboardData(CF_TEXT);
			pstr = GlobalLock(halc);
			/* ※一度に貼り付けできるべきである。*/
			for (i = 0; pstr[i] != '\0'; i++)
				if (pstr[i] != 0x0d) SendMessage(hwnd, WM_CHAR, pstr[i], 0);
			GlobalUnlock(halc);
			CloseClipboard();
			break;
		case 103:	/* 「削除」 */
			if (selected) goto backspace;
			break;
		case 104:	/* 「すべて選択」 */
			for (i = 0; get_edit_pos(str, 0, i, hwnd, TRUE) != -2147483648; i++);
			i--;
			if (i < 0) break;

			caret_from_pos_x = -GetScrollPos(hwnd, SB_HORZ) * tm.tmAveCharWidth;
			caret_from_pos_y = -GetScrollPos(hwnd, SB_VERT) * tm.tmHeight;
			caret_pos_x = get_edit_pos(str, -1, i, hwnd, FALSE) * tm.tmAveCharWidth;
			caret_pos_y = i * tm.tmHeight;
			selected = TRUE;

			hdc = GetDC(hwnd);
			draw_selected_text(hwnd, hdc, str, hfont, &tm, caret_from_pos_x, caret_from_pos_y, caret_pos_x, caret_pos_y, FALSE);
			ReleaseDC(hwnd, hdc);
			break;
		}
		return 0;
	case WM_HSCROLL:	/* 横スクロール */
		i = GetScrollPos(hwnd, SB_HORZ);
		switch (LOWORD(wParam)) {
		case SB_LEFT:
			GetScrollRange(hwnd, SB_HORZ, &j, &k);
			SetScrollPos(hwnd, SB_HORZ, j, TRUE);
			break;
		case SB_RIGHT:
			GetScrollRange(hwnd, SB_HORZ, &j, &k);
			SetScrollPos(hwnd, SB_HORZ, k, TRUE);
			break;
		case SB_LINELEFT:
			SetScrollPos(hwnd, SB_HORZ, GetScrollPos(hwnd, SB_HORZ) - 1, TRUE);
			break;
		case SB_LINERIGHT:
			SetScrollPos(hwnd, SB_HORZ, GetScrollPos(hwnd, SB_HORZ) + 1, TRUE);
			break;
		case SB_PAGELEFT:
			GetClientRect(hwnd, &rect);
			SetScrollPos(hwnd, SB_HORZ, GetScrollPos(hwnd, SB_HORZ) - (rect.right - rect.left) / tm.tmAveCharWidth, TRUE);
			break;
		case SB_PAGERIGHT:
			GetClientRect(hwnd, &rect);
			SetScrollPos(hwnd, SB_HORZ, GetScrollPos(hwnd, SB_HORZ) + (rect.right - rect.left) / tm.tmAveCharWidth, TRUE);
			break;
		case SB_THUMBTRACK:
			SetScrollPos(hwnd, SB_HORZ, HIWORD(wParam), TRUE);
			break;
		}
		j = GetScrollPos(hwnd, SB_HORZ);
		if (i != j) {
			/* 左へ進むと正、右へ進むと負にする。*/
			caret_pos_x += (i - j) * tm.tmAveCharWidth;
			caret_from_pos_x += (i - j) * tm.tmAveCharWidth;
			SetCaretPos(caret_pos_x, caret_pos_y);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;
	case WM_VSCROLL:	/* 縦スクロール */
		i = GetScrollPos(hwnd, SB_VERT);
		switch (LOWORD(wParam)) {
		case SB_BOTTOM:
			GetScrollRange(hwnd, SB_VERT, &j, &k);
			SetScrollPos(hwnd, SB_VERT, k, TRUE);
			break;
		case SB_LINEDOWN:
			SetScrollPos(hwnd, SB_VERT, GetScrollPos(hwnd, SB_VERT) + 1, TRUE);
			break;
		case SB_LINEUP:
			SetScrollPos(hwnd, SB_VERT, GetScrollPos(hwnd, SB_VERT) - 1, TRUE);
			break;
		case SB_PAGEDOWN:
			GetClientRect(hwnd, &rect);
			SetScrollPos(hwnd, SB_VERT, GetScrollPos(hwnd, SB_VERT) + (rect.bottom - rect.top) / tm.tmHeight, TRUE);
			break;
		case SB_PAGEUP:
			GetClientRect(hwnd, &rect);
			SetScrollPos(hwnd, SB_VERT, GetScrollPos(hwnd, SB_VERT) - (rect.bottom - rect.top) / tm.tmHeight, TRUE);
			break;
		case SB_THUMBTRACK:
			SetScrollPos(hwnd, SB_VERT, HIWORD(wParam), TRUE);
			break;
		case SB_TOP:
			GetScrollRange(hwnd, SB_VERT, &j, &k);
			SetScrollPos(hwnd, SB_VERT, j, TRUE);
			break;
		}
		j = GetScrollPos(hwnd, SB_VERT);
		if (i != j) {
			/* 上へ進むと正、下へ進むと負にする。*/
			caret_pos_y += (i - j) * tm.tmHeight;
			caret_from_pos_y += (i - j) * tm.tmHeight;
			SetCaretPos(caret_pos_x, caret_pos_y);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		return 0;
	case WM_IME_STARTCOMPOSITION:
		/* IMEの表示位置をキャレットの位置に設定する。*/
		himc = ImmGetContext(hwnd);
		ImmGetCompositionWindow(himc, &composform);
		GetCaretPos(&composform.ptCurrentPos);
		ImmSetCompositionFont(himc, &logfont);
		ImmSetCompositionWindow(himc, &composform);
		ImmReleaseContext(hwnd, himc);
		return 0;
	case WM_DESTROY:
		GlobalFree(str);
		DeleteObject(hfont);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;
	MSG msg;
	HWND hwnd;
	ACCEL accel[] = {
		{FCONTROL | FVIRTKEY, 0x58, 100},	/* 「切り取り」 */
		{FCONTROL | FVIRTKEY, 0x43, 101},	/* 「コピー」 */
		{FCONTROL | FVIRTKEY, 0x56, 102},	/* 「貼り付け」 */
		{FCONTROL | FVIRTKEY, 0x41, 104}	/* 「すべて選択」 */
	};
	HACCEL haccel;

	wc.style = 0;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_IBEAM);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOWFRAME;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "edittest";
	if (!RegisterClass(&wc)) return 0;

	if ((hwnd = CreateWindow(
		wc.lpszClassName, "EditTest",
		WS_HSCROLL | WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_VSCROLL,
		200, 200, 300, 250, NULL, NULL, hInstance, NULL
	)) == NULL) return 0;
	ShowScrollBar(hwnd, SB_HORZ, FALSE);
	ShowScrollBar(hwnd, SB_VERT, FALSE);

	haccel = CreateAcceleratorTable(accel, sizeof(accel) / sizeof(accel[0]));

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (TranslateAccelerator(hwnd, haccel, &msg)) continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DestroyAcceleratorTable(haccel);
	return msg.wParam;
}
