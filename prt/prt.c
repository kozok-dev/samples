#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PRINTDLG pd;
	DOCINFO di;

	switch (uMsg) {
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 1:
			pd.lStructSize = sizeof(PRINTDLG);
			pd.hwndOwner = hwnd;
			pd.hDevMode = NULL;
			pd.hDevNames = NULL;
			pd.hDC = NULL;
			pd.Flags = PD_NOSELECTION | PD_RETURNDC;
			pd.nFromPage = 1;
			pd.nToPage = 1;
			pd.nMinPage = 1;
			pd.nMaxPage = 1;
			pd.nCopies = 1;
			pd.hInstance = NULL;
			pd.lCustData = 0;
			if (!PrintDlg(&pd)) break;

			di.cbSize = sizeof(DOCINFO);
			di.lpszDocName = NULL;
			di.lpszOutput = NULL;
			di.lpszDatatype = NULL;
			di.fwType = 0;
			if (StartDoc(pd.hDC, &di) > 0)
				if (StartPage(pd.hDC) > 0) {
					TextOut(pd.hDC, 0, 0, TEXT("Test print"), sizeof(TEXT("Test print")));
					if (EndPage(pd.hDC) > 0) EndDoc(pd.hDC);
				}
			DeleteDC(pd.hDC);

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
	WNDCLASS wc;
	HWND hwnd;
	MSG msg;
	HMENU hmenu;

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

	/* MEMO: ALTÇâüÇµÇΩå„Ç…ì™ï∂éöÇ‡ÇµÇ≠ÇÕ&ÇÃå„ÇÃï∂éöÇâüÇµÇƒÇ‡é¿çsÇ≥ÇÍÇÈÅB*/
	hmenu = CreateMenu();
	InsertMenu(hmenu, -1, MF_BYPOSITION | MF_STRING, 1, TEXT("&Print"));

	if ((hwnd = CreateWindow(wc.lpszClassName, TEXT("PrtTest"), WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE,
		200, 250, 150, 100, NULL, hmenu, hInstance, NULL)) == NULL) return 0;

	while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);
	return msg.wParam;
}
