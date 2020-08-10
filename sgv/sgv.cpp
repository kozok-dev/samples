#include "sgv.h"

SimpleGridView::SimpleGridView() {
	m_rowcnt = 0;
	m_colcnt = 0;
	m_pitem = NULL;
	m_pppdata = NULL;
}

SimpleGridView::~SimpleGridView() {
	char j;
	short i;

	for (i = 0; i < m_rowcnt; i++) {
		for (j = 0; j < m_colcnt; j++)
			if (m_pitem[j].type == ITEMTYPE_STRING) free(m_pppdata[i][j]);
		free(m_pppdata[i]);
	}
	free(m_pppdata);
	free(m_pitem);
	DeleteObject(m_hfont);
}

bool SimpleGridView::Create(HWND hwnd, HINSTANCE hinst, int posx, int posy, int width, int height) {
	WNDCLASS wc;

	if (!GetClassInfo(hinst, TEXT("sgv"), &wc)) {
		wc.style = 0;
		wc.lpfnWndProc = procWnd;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hinst;
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = TEXT("sgv");
		if (!RegisterClass(&wc)) return false;
	}

	if ((m_hfont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, NULL)) == NULL) return false;

	return (m_hwnd = CreateWindow(wc.lpszClassName, NULL, WS_CHILD | WS_HSCROLL | WS_VSCROLL | WS_VISIBLE,
		posx, posy, width, height, hwnd, NULL, hinst, this)) != NULL;
}

bool SimpleGridView::addItem(TCHAR *name, ITEMTYPE type, TCHAR *format, BYTE width, bool setscrflag) {
	Item *pitem;

	// 項目の制限によるチェック。
	if (m_colcnt > ITEM_MAX || lstrlen(name) > ITEMNAME_MAX || lstrlen(format) > ITEMFORMAT_MAX) return false;

	// 格納領域確保。
	if (m_pitem == NULL) {
		if ((m_pitem = (Item *)malloc(sizeof(Item))) == NULL) return false;
	} else {
		if ((pitem = (Item *)realloc(m_pitem, (m_colcnt + 1) * sizeof(Item))) == NULL) return false;
		m_pitem = pitem;
	}

	// 項目設定。
	pitem = &m_pitem[m_colcnt];
	strcpy(pitem->name, name);
	pitem->type = type;
	strcpy(pitem->format, format);
	pitem->width = width;

	m_colcnt++;
	if (setscrflag) setHScrollInfo();
	return true;
}

void **SimpleGridView::addData(bool setscrflag) {
	char i;
	void ***pppdata;

	// データの制限によるチェック。
	if (m_colcnt <= 0 || m_rowcnt > DATA_MAX) return NULL;

	// 格納領域確保。
	if (m_pppdata == NULL) {
		if ((m_pppdata = (void ***)malloc(4)) == NULL) return NULL;
	} else {
		if ((pppdata = (void ***)realloc(m_pppdata, (m_rowcnt + 1) * 4)) == NULL) return NULL;
		m_pppdata = pppdata;
	}
	if ((m_pppdata[m_rowcnt] = (void **)malloc(m_colcnt * 4)) == NULL) return NULL;

	if (setscrflag) setVScrollInfo();
	return m_pppdata[m_rowcnt++];
}

void SimpleGridView::setWndSize(int width, int height) {
	SetWindowPos(m_hwnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
	setHScrollInfo();
	setVScrollInfo();
}

void *SimpleGridView::floatValue(float f) {
	return (void *)*(int *)&f;
}

void *SimpleGridView::stringValue(TCHAR *str) {
	char len;
	TCHAR *cstr;

	if ((len = lstrlen(str)) > STRING_MAX || (cstr = (TCHAR *)malloc((len + 1) * sizeof(TCHAR))) == NULL) return NULL;
	strcpy(cstr, str);
	return cstr;
}

LRESULT CALLBACK SimpleGridView::procWnd(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;
	static SimpleGridView *sgv;

	switch (uMsg) {
	case WM_CREATE:
		sgv = (SimpleGridView *)((LPCREATESTRUCT)lParam)->lpCreateParams;
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		sgv->Paint(hdc);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_LBUTTONDOWN:
		SetFocus(hwnd);
		return 0;
	case WM_MOUSEWHEEL:
		sgv->mouseWheel(HIWORD(wParam));
		return 0;
	case WM_HSCROLL:
		sgv->hScroll(HIWORD(wParam), LOWORD(wParam));
		return 0;
	case WM_VSCROLL:
		sgv->vScroll(HIWORD(wParam), LOWORD(wParam));
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void SimpleGridView::Paint(HDC hdc) {
	char i;
	short disprowcnt, width = 0, j, k;
	TCHAR data[STRING_MAX + 1], format[ITEMFORMAT_MAX + 2];
	RECT cltrect, rect;
	HPEN hpen;

	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdc, m_hfont);

	// クライアント領域の高さ/幅を取得して表示する項目/データ数を設定できるようにする。
	GetClientRect(m_hwnd, &cltrect);
	disprowcnt = cltrect.bottom / 14;

	// 項目/データの描画。
	hpen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_APPWORKSPACE));
	SelectObject(hdc, hpen);
	for (i = GetScrollPos(m_hwnd, SB_HORZ); i < m_colcnt && width < cltrect.right; i++) {
		if (m_pitem[i].width <= 0) continue;

		rect.left = width + 1;
		rect.right = m_pitem[i].width + width - 1;

		// 項目。
		SelectObject(hdc, GetSysColorBrush(COLOR_3DFACE));
		Rectangle(hdc, width, 0, m_pitem[i].width + width, 15);
		rect.top = 2;
		rect.bottom = 14;
		DrawText(hdc, m_pitem[i].name, -1, &rect, DT_CENTER | DT_VCENTER);

		// データ。
		for (j = GetScrollPos(m_hwnd, SB_VERT), k = 0; j < m_rowcnt && k < disprowcnt; j++, k++) {
			SelectObject(hdc, GetSysColorBrush(COLOR_WINDOW));
			Rectangle(hdc, width, 14 + k * 14, m_pitem[i].width + width, 29 + k * 14);
			rect.top = 16 + k * 14;
			rect.bottom = 28 + k * 14;

			lstrcpy(format, TEXT("%"));
			lstrcat(format, m_pitem[i].format);
			if (m_pitem[i].type == ITEMTYPE_DECIMAL) _stprintf(data, format, *(float *)&m_pppdata[j][i]);
			else _stprintf(data, format, m_pppdata[j][i]);
			DrawText(hdc, data, -1, &rect, DT_CENTER | DT_VCENTER);
		}

		width += m_pitem[i].width - 1;
	}
	DeleteObject(hpen);

	// 必要に応じて空描画する。
	hpen = CreatePen(PS_NULL, 0, 0);
	SelectObject(hdc, hpen);
	SelectObject(hdc, GetSysColorBrush(COLOR_WINDOW));
	if (width < cltrect.right) Rectangle(hdc, width + 1, 0, cltrect.right + 1, 16 + k * 14);	// 列。
	if (k < disprowcnt) Rectangle(hdc, 0, 15 + k * 14, width + 2, cltrect.bottom + 1);	// 行。
	DeleteObject(hpen);
}

void SimpleGridView::mouseWheel(short dir) {
	SetScrollPos(m_hwnd, SB_VERT, GetScrollPos(m_hwnd, SB_VERT) + (dir <= 0 ? 3 : -3), TRUE);
	RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
}

void SimpleGridView::hScroll(WORD pos, WORD type) {
	int posmin, posmax;

	switch (type) {
	case SB_LEFT:
		if (GetScrollRange(m_hwnd, SB_HORZ, &posmin, &posmax)) SetScrollPos(m_hwnd, SB_HORZ, posmin, TRUE);
		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
		break;
	case SB_RIGHT:
		if (GetScrollRange(m_hwnd, SB_HORZ, &posmin, &posmax)) SetScrollPos(m_hwnd, SB_HORZ, posmax, TRUE);
		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
		break;
	case SB_LINELEFT:
		SetScrollPos(m_hwnd, SB_HORZ, GetScrollPos(m_hwnd, SB_HORZ) - 1, TRUE);
		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
		break;
	case SB_LINERIGHT:
		SetScrollPos(m_hwnd, SB_HORZ, GetScrollPos(m_hwnd, SB_HORZ) + 1, TRUE);
		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
		break;
	case SB_PAGELEFT:
		SetScrollPos(m_hwnd, SB_HORZ, GetScrollPos(m_hwnd, SB_HORZ) - 10, TRUE);
		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
		break;
	case SB_PAGERIGHT:
		SetScrollPos(m_hwnd, SB_HORZ, GetScrollPos(m_hwnd, SB_HORZ) + 10, TRUE);
		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
		break;
	case SB_THUMBTRACK:
		SetScrollPos(m_hwnd, SB_HORZ, pos, TRUE);
		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
		break;
	}
}

void SimpleGridView::vScroll(WORD pos, WORD type) {
	int posmin, posmax;

	switch (type) {
	case SB_BOTTOM:
		if (GetScrollRange(m_hwnd, SB_VERT, &posmin, &posmax)) SetScrollPos(m_hwnd, SB_VERT, posmax, TRUE);
		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
		break;
	case SB_LINEDOWN:
		SetScrollPos(m_hwnd, SB_VERT, GetScrollPos(m_hwnd, SB_VERT) + 1, TRUE);
		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
		break;
	case SB_LINEUP:
		SetScrollPos(m_hwnd, SB_VERT, GetScrollPos(m_hwnd, SB_VERT) - 1, TRUE);
		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
		break;
	case SB_PAGEDOWN:
		SetScrollPos(m_hwnd, SB_VERT, GetScrollPos(m_hwnd, SB_VERT) + 10, TRUE);
		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
		break;
	case SB_PAGEUP:
		SetScrollPos(m_hwnd, SB_VERT, GetScrollPos(m_hwnd, SB_VERT) - 10, TRUE);
		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
		break;
	case SB_THUMBTRACK:
		SetScrollPos(m_hwnd, SB_VERT, pos, TRUE);
		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
		break;
	case SB_TOP:
		if (GetScrollRange(m_hwnd, SB_VERT, &posmin, &posmax)) SetScrollPos(m_hwnd, SB_VERT, posmin, TRUE);
		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
		break;
	}
}

void SimpleGridView::setHScrollInfo() {
	RECT rect;
	SCROLLINFO si;

	GetClientRect(m_hwnd, &rect);
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_RANGE;
	si.nPage = rect.right / 14;
	si.nMin = 0;
	si.nMax = si.nPage + m_colcnt - 2;
	SetScrollInfo(m_hwnd, SB_HORZ, &si, TRUE);
}

void SimpleGridView::setVScrollInfo() {
	RECT rect;
	SCROLLINFO si;

	GetClientRect(m_hwnd, &rect);
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_RANGE;
	si.nPage = rect.bottom / 14;
	si.nMin = 0;
	si.nMax = si.nPage + m_rowcnt - 2;
	SetScrollInfo(m_hwnd, SB_VERT, &si, TRUE);
}
