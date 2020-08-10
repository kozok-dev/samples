#include <windows.h>
#include <commctrl.h>

HWND g_hcwnd;

LRESULT CALLBACK WindowProc1(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR buf[10];
	RECT rect;
	HMENU hmenu;

	switch (uMsg) {
	case WM_COMMAND:
		if (LOWORD(wParam) == 1 && g_hcwnd == NULL)
			g_hcwnd = CreateWindow(TEXT("test2"), TEXT("ChildWindow"), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
				250, 300, 200, 100, hwnd, NULL, NULL, NULL);
		else {
			wsprintf(buf, TEXT("%d"), LOWORD(wParam));
			MessageBox(hwnd, buf, NULL, MB_OK);
		}
		return 0;
	case WM_NOTIFY:
		#define pnmtb ((LPNMTOOLBAR)lParam)
		switch (((LPNMHDR)lParam)->code) {
		case TBN_DROPDOWN:
			SendMessage(pnmtb->hdr.hwndFrom, TB_GETRECT, (WPARAM)pnmtb->iItem, (LPARAM)&rect);
			MapWindowPoints(pnmtb->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rect, 2);

			hmenu = CreatePopupMenu();
			InsertMenu(hmenu, -1, MF_BYPOSITION | MF_STRING, 2, TEXT("tab1"));
			InsertMenu(hmenu, -1, MF_BYPOSITION | MF_STRING, 3, TEXT("tab2"));
			InsertMenu(hmenu, -1, MF_BYPOSITION | MF_STRING, 4, TEXT("tab3"));
			InsertMenu(hmenu, -1, MF_BYPOSITION | MF_STRING, 5, TEXT("tab4"));
			TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN, rect.left, rect.bottom, 0, hwnd, NULL);
			DestroyMenu(hmenu);
			break;
		}

		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WindowProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_DESTROY:
		g_hcwnd = NULL;
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;
	HWND hwnd, htb;
	MSG msg;
	TBBUTTON tbb[2];

	wc.style = 0;
	wc.lpfnWndProc = WindowProc1;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("test1");
	if (!RegisterClass(&wc)) return 0;
	wc.lpfnWndProc = WindowProc2;
	wc.lpszClassName = TEXT("test2");
	if (!RegisterClass(&wc)) return 0;

	if ((hwnd = CreateWindow(TEXT("test1"), TEXT("ToolbarTest"), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		200, 250, 300, 200, NULL, NULL, hInstance, NULL)) == NULL) return 0;

	if ((htb = CreateWindow(TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | TBSTYLE_LIST,
		0, 0, 0, 0, hwnd, NULL, hInstance, NULL)) == NULL) return 0;
	SendMessage(htb, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

	tbb[0].iBitmap = -1;
	tbb[0].idCommand = 1;
	tbb[0].fsState = TBSTATE_ENABLED;
	tbb[0].fsStyle = TBSTYLE_BUTTON;
	tbb[0].dwData = 0;
	tbb[0].iString = (INT_PTR)TEXT("Btn1");

	tbb[1].iBitmap = -1;
	tbb[1].idCommand = 0;
	tbb[1].fsState = TBSTATE_ENABLED;
	tbb[1].fsStyle = TBSTYLE_DROPDOWN;
	tbb[1].dwData = 0;
	tbb[1].iString = (INT_PTR)TEXT("Btn2");

	SendMessage(htb, TB_ADDBUTTONS, 2, (LPARAM)&tbb);
	SendMessage(htb, TB_SETIMAGELIST, 0, 0);
	SendMessage(htb, TB_SETBUTTONSIZE, 0, 40);

	SendMessage(htb, TB_AUTOSIZE, 0, 0);

	while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);
	return msg.wParam;
}
