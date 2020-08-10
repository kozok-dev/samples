#include <windows.h>
#include <commctrl.h>

TCHAR testlist[][2][20] = {
	{TEXT("Item0"), TEXT("NextItem9")},
	{TEXT("Item1"), TEXT("NextItem8")},
	{TEXT("Item2"), TEXT("NextItem7")},
	{TEXT("Item3"), TEXT("NextItem6")},
	{TEXT("Item4"), TEXT("NextItem5")},
	{TEXT("Item5"), TEXT("NextItem4")},
	{TEXT("Item6"), TEXT("NextItem3")},
	{TEXT("Item7"), TEXT("NextItem2")},
	{TEXT("Item8"), TEXT("NextItem1")},
	{TEXT("Item9"), TEXT("NextItem000000000")}
};
HWND hwnd, hlistview, hlistbox;
HIMAGELIST hil;
WNDPROC defwndproc;

int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	return 1;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int select = -1, select_sub = -1;
	static bool mode, active;

	switch (uMsg) {
	case WM_ACTIVATEAPP:
		active = wParam;
		return 0;
	case WM_NCPAINT: {
		LRESULT lr = DefWindowProc(hwnd, uMsg, wParam, lParam);
		SendMessage(hwnd, WM_NCACTIVATE, active || IsWindowVisible(hlistbox), 0);
		return lr;
	}
	case WM_NCACTIVATE:
		if (IsWindowVisible(hlistbox)) wParam = true;
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
		case NM_CLICK: {
			LPNMITEMACTIVATE ia = (LPNMITEMACTIVATE)lParam;
			select = ia->iItem;
			select_sub = ia->iSubItem;
			mode = true;
			InvalidateRect(hlistview, NULL, false);
		}
			break;
		}
		break;
	case WM_DRAWITEM: {
		LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
		RECT rect;
		int cols[2];
		ListView_GetColumnOrderArray(hlistview, 2, &cols);
		int w0 = ListView_GetColumnWidth(hlistview, cols[0]);
		int w1 = ListView_GetColumnWidth(hlistview, cols[1]);

		if (select == dis->itemID) {
			SelectObject(dis->hDC, GetSysColorBrush(COLOR_HIGHLIGHT));
			Rectangle(dis->hDC, dis->rcItem.left, dis->rcItem.top, dis->rcItem.right, dis->rcItem.bottom);
			CopyMemory(&rect, &dis->rcItem, sizeof(RECT));
			rect.bottom--;
			DrawFocusRect(dis->hDC, &rect);

			SetTextColor(dis->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
		} else {
			SelectObject(dis->hDC, GetStockObject(WHITE_PEN));
			SelectObject(dis->hDC, GetStockObject(WHITE_BRUSH));
			Rectangle(dis->hDC, dis->rcItem.left, dis->rcItem.top, dis->rcItem.right, dis->rcItem.bottom);

			SetTextColor(dis->hDC, GetSysColor(COLOR_BTNTEXT));
		}

		rect.left = dis->rcItem.left + w0 * (cols[0] != 0);
		rect.top = dis->rcItem.top;
		rect.right = rect.left + (cols[0] != 0 ? w1 : w0);
		rect.bottom = dis->rcItem.bottom;
		DrawText(dis->hDC, testlist[dis->itemID][0], -1, &rect, DT_NOPREFIX | DT_CENTER | DT_VCENTER);

		rect.left = dis->rcItem.left + w0 * (cols[0] == 0);
		rect.right = rect.left + (cols[0] == 0 ? w1 : w0) - 15 * ((cols[0] == 0 ? w1 : w0) >= 15);
		DrawText(dis->hDC, testlist[dis->itemID][1], -1, &rect, DT_NOPREFIX | DT_CENTER | DT_VCENTER);

		if ((cols[0] == 0 ? w1 : w0) >= 15) {
			rect.left = dis->rcItem.left + w0 * (cols[0] == 0) + (cols[0] == 0 ? w1 : w0) - 15;
			rect.top = dis->rcItem.top;
			rect.right = rect.left + 15;
			rect.bottom = rect.top + 15;
			DrawEdge(dis->hDC, &rect, EDGE_RAISED, BF_MIDDLE | BF_RECT);

			if (select == dis->itemID && select_sub == 1 && mode) {
				POINT c_point;
				c_point.x = dis->rcItem.left + w0 * (cols[0] == 0);
				c_point.y = dis->rcItem.top + (rect.bottom - rect.top);
				ClientToScreen(hlistview, &c_point);

				MoveWindow(hlistbox, c_point.x, c_point.y, cols[0] == 0 ? w1 : w0, 100, false);
				ShowWindow(hlistbox, SW_SHOWNORMAL);
				mode = false;
			}

			POINT point[3];
			point[0].x = rect.left + 4;
			point[0].y = rect.top + 6;
			point[1].x = point[0].x + 6;
			point[1].y = point[0].y;
			point[2].x = point[0].x + 3;
			point[2].y = point[0].y + 3;
			SelectObject(dis->hDC, GetStockObject(BLACK_PEN));
			SelectObject(dis->hDC, GetStockObject(BLACK_BRUSH));
			Polygon(dis->hDC, point, 3);
		} else {
			mode = false;
		}
	}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK procListBox(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_KILLFOCUS:
	case WM_LBUTTONDBLCLK:
		ShowWindow(hwnd, SW_HIDE);
		return 0;
	}
	return CallWindowProc(defwndproc, hwnd, uMsg, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int i;
	TCHAR buf[10];
	WNDCLASS wc;
	MSG msg;
	HFONT hfont;
	LVCOLUMN lvc;
	LVITEM lvi;

	wc.style = 0;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("test");
	if (!RegisterClass(&wc)) return 0;

	hfont = CreateFont(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
	CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, NULL);

	InitCommonControls();

	if ((hwnd = CreateWindow(wc.lpszClassName, TEXT("CtlTest"), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		200, 200, 300, 200, NULL, NULL, hInstance, NULL)) == NULL) return 0;

	if ((hlistview = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL, WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDRAWFIXED,
		50, 50, 200, 100, hwnd, NULL, hInstance, NULL)) == NULL) return 0;

	if ((hlistbox = CreateWindow(TEXT("LISTBOX"), NULL, WS_POPUPWINDOW | WS_VSCROLL,
		0, 0, 0, 100, hwnd, NULL, hInstance, NULL)) == NULL) return 0;
	defwndproc = (WNDPROC)GetWindowLong(hlistbox, GWL_WNDPROC);
	SetWindowLong(hlistbox, GWL_WNDPROC, (LONG)procListBox);

	hil = ImageList_Create(1, 15, ILC_COLOR, 1, 0);
	ListView_SetImageList(hlistview, hil, LVSIL_SMALL);

	for (i = 0; i < 10; i++) SendMessage(hlistbox, LB_ADDSTRING, 0, (LPARAM)testlist[i][1]);
	SendMessage(hlistbox, WM_SETFONT, (WPARAM)hfont, false);

	SendMessage(hlistview, WM_SETFONT, (WPARAM)hfont, false);
	ListView_SetExtendedListViewStyle(hlistview, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_HEADERDRAGDROP);

	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvc.cx = 70;
	lvc.pszText = TEXT("Column1");
	ListView_InsertColumn(hlistview, 0, &lvc);
	lvc.fmt = LVCFMT_CENTER;
	lvc.cx = 100;
	lvc.pszText = TEXT("Column2");
	ListView_InsertColumn(hlistview, 1, &lvc);

	lvi.mask = LVIF_IMAGE;
	lvi.iSubItem = 0;
	for (i = 0; i < 10; i++) {
		lvi.iImage = 0;
		lvi.iItem = i;
		ListView_InsertItem(hlistview, &lvi);
	}

	while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);
	DeleteObject(hfont);
	return msg.wParam;
}
