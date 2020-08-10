#include <windows.h>
#include <commctrl.h>

TCHAR testlist[][2][10] = {
	{TEXT("Item0"), TEXT("NextItem9")},
	{TEXT("Item1"), TEXT("NextItem8")},
	{TEXT("Item2"), TEXT("NextItem7")},
	{TEXT("Item3"), TEXT("NextItem6")},
	{TEXT("Item4"), TEXT("NextItem5")},
	{TEXT("Item5"), TEXT("NextItem4")},
	{TEXT("Item6"), TEXT("NextItem3")},
	{TEXT("Item7"), TEXT("NextItem2")},
	{TEXT("Item8"), TEXT("NextItem1")},
	{TEXT("Item9"), TEXT("NextItem0")}
};
HWND hwnd, hlist, htab;

int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	return 1;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	#define plvdi ((NMLVDISPINFO *)lParam)
	#define pnmlv ((LPNMLISTVIEW)lParam)
	int i, j;
	TCHAR buf1[10], buf2[10];

	switch (uMsg) {
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
		case LVN_GETDISPINFO:
			switch (plvdi->item.iSubItem) {
			case 0:
				plvdi->item.pszText = testlist[plvdi->item.iItem][0];
				break;
			case 1:
				plvdi->item.pszText = testlist[plvdi->item.iItem][1];
				break;
			}
			break;
		case LVN_COLUMNCLICK:
			for (i = 0; i < 9; i++)
				for (j = i + 1; j < 10; j++) {
					lstrcpy(buf1, testlist[i][0]);
					lstrcpy(buf2, testlist[i][1]);
					lstrcpy(testlist[i][0], testlist[j][0]);
					lstrcpy(testlist[i][1], testlist[j][1]);
					lstrcpy(testlist[j][0], buf1);
					lstrcpy(testlist[j][1], buf2);
				}

			ListView_SortItemsEx(hlist, CompareFunc, 0);
			break;
		}

		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int i;
	TCHAR buf[10];
	WNDCLASS wc;
	MSG msg;
	HFONT hfont;
	LVCOLUMN lvc;
	LVITEM lvi;
	TCITEM tci;

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

	if ((hwnd = CreateWindow(wc.lpszClassName, TEXT("CtlTest"), WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE,
		200, 200, 300, 300, NULL, NULL, hInstance, NULL)) == NULL) return 0;

	if ((hlist = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL, WS_CHILD | WS_VISIBLE | LVS_REPORT,
		50, 50, 200, 200, hwnd, NULL, hInstance, NULL)) == NULL) return 0;

	if ((htab = CreateWindow(WC_TABCONTROL, NULL, WS_CHILD | WS_VISIBLE,
		25, 10, 250, 250, hwnd, NULL, hInstance, NULL)) == NULL) return 0;
	SendMessage(htab, WM_SETFONT, (WPARAM)hfont, FALSE);

	ListView_SetExtendedListViewStyle(hlist, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP | LVS_EX_HEADERDRAGDROP);

	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvc.cx = 70;
	lvc.pszText = TEXT("Column1");
	ListView_InsertColumn(hlist, 0, &lvc);
	lvc.fmt = LVCFMT_CENTER;
	lvc.cx = 100;
	lvc.pszText = TEXT("Column2");
	ListView_InsertColumn(hlist, 1, &lvc);

	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 0;
	lvi.pszText = LPSTR_TEXTCALLBACK;
	for (i = 0; i < 10; i++) {
		lvi.iItem = i;
		ListView_InsertItem(hlist, &lvi);
	}

	tci.mask = TCIF_TEXT;
	for (i = 0; i < 10; i++) {
		wsprintf(buf, TEXT("Tab%d"), i);
		tci.pszText = buf;
		TabCtrl_InsertItem(htab, i, &tci);
	}

	while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);
	DeleteObject(hfont);
	return msg.wParam;
}
