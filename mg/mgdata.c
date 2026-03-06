#include <windows.h>
#define CHARCOPYBUTTON 100
#define STAGECOPYBUTTON 101
#define GETBUTTON 102
#define CLOSEBUTTON 103
HWND hcharedit, hstageedit, hlogedit;

void copyClipboard(HWND hwnd) {
	int size;
	LPTSTR data;
	HGLOBAL hglobal;

	if (!OpenClipboard(NULL)) return;

	size = GetWindowTextLength(hwnd) + 1;
	if (hglobal = GlobalAlloc(GMEM_MOVEABLE, size)) {
		EmptyClipboard();
		data = GlobalLock(hglobal);
		GetWindowText(hwnd, data, size);
		data[size] = '\0';
		GlobalUnlock(hglobal);
		SetClipboardData(CF_TEXT, hglobal);
		GlobalFree(hglobal);
	}
	CloseClipboard();
}

void getData()
{
	LPTSTR data;
	TCHAR fn[MAX_PATH], buf1[2000] = {'\0'}, buf2[1000];
	HANDLE hfd, hfddef;
	WIN32_FIND_DATA fd, fddef;
	int cnt, size;

	SetWindowText(hcharedit, NULL);
	SetWindowText(hstageedit, NULL);
	SetWindowText(hlogedit, NULL);

	if ((hfd = FindFirstFile(TEXT("..\\chars\\*"), &fd)) == INVALID_HANDLE_VALUE) {
		lstrcat(buf1, TEXT("Fatal error: \"chars\" directory is not found."));
	} else {
		if (data = (LPTSTR)GlobalAlloc(GMEM_FIXED, 0)) {
			cnt = 0;
			do {
				if (!lstrcmp(fd.cFileName, TEXT(".")) || !lstrcmp(fd.cFileName, TEXT("..")) ||
				!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;

				wsprintf(fn, TEXT("..\\chars\\%s\\%s.def"), fd.cFileName, fd.cFileName);
				if ((hfddef = FindFirstFile(fn, &fddef)) != INVALID_HANDLE_VALUE) {
					lstrcpy(fn, fddef.cFileName);
					fn[lstrlen(fn) - 4] = '\0';
					if (lstrcmp(fd.cFileName, fn)) {
						lstrcat(buf1, TEXT("Wrong file name: <dir:"));
						lstrcat(buf1, fd.cFileName);
						lstrcat(buf1, TEXT("> <def:"));
						lstrcat(buf1, fn);
						lstrcat(buf1, TEXT(">\r\n"));
					} else {
						wsprintf(buf2, TEXT("%s, , order=0\r\n"), fn);

						size = GlobalSize(data);

						if ((data = (LPTSTR)GlobalReAlloc(data,
						size + lstrlen(buf2) + sizeof(TCHAR), GMEM_MOVEABLE)) == NULL) {
							lstrcpy(buf1, TEXT("Fatal error: GlobalReAlloc is failed.\r\n"));
							break;
						}

						data[size - 1] = '\0';
						lstrcat(data, buf2);
						cnt++;
					}
					FindClose(hfddef);
				} else {
					lstrcat(buf1, TEXT("Not exist file name: <dir:"));
					lstrcat(buf1, fd.cFileName);
					lstrcat(buf1, TEXT("> <def:>\r\n"));
				}
			} while (FindNextFile(hfd, &fd));

			wsprintf(buf2, TEXT("%d\r\n"), cnt);
			lstrcat(buf1, TEXT("Char count: "));
			lstrcat(buf1, buf2);

			SetWindowText(hcharedit, data);
			GlobalFree(data);
		}
		FindClose(hfd);
	}

	if ((hfd = FindFirstFile(TEXT("..\\stages\\*.def"), &fd)) == INVALID_HANDLE_VALUE) {
		lstrcat(buf1, TEXT("Fatal error: \"stages\" directory is not found."));
	} else {
		if (data = (LPTSTR)GlobalAlloc(GMEM_FIXED, 0)) {
			cnt = 0;
			do {
				wsprintf(buf2, TEXT("stages/%s\r\n"), fd.cFileName);

				size = GlobalSize(data);

				if ((data = (LPTSTR)GlobalReAlloc(data,
				size + lstrlen(buf2) + sizeof(TCHAR), GMEM_MOVEABLE)) == NULL) {
					lstrcpy(buf1, TEXT("Fatal error: GlobalReAlloc is failed.\r\n"));
					break;
				}

				data[size - 1] = '\0';
				lstrcat(data, buf2);
				cnt++;
			} while (FindNextFile(hfd, &fd));

			wsprintf(buf2, TEXT("%d"), cnt);
			lstrcat(buf1, TEXT("Stage count: "));
			lstrcat(buf1, buf2);

			SetWindowText(hstageedit, data);
			GlobalFree(data);
		}
		FindClose(hfd);
	}

	SetWindowText(hlogedit, buf1);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case CHARCOPYBUTTON:
			copyClipboard(hcharedit);
			break;
		case STAGECOPYBUTTON:
			copyClipboard(hstageedit);
			break;
		case GETBUTTON:
			getData();
			break;
		case CLOSEBUTTON:
			DestroyWindow(hwnd);
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
	HWND hwnd_main, hwnd;
	WNDCLASS wc;
	MSG msg;
	HFONT hfont;
	HANDLE hmutex;

	hmutex = CreateMutex(NULL, FALSE, TEXT("mgdata"));
	if (GetLastError() == ERROR_ALREADY_EXISTS) return 0;

	wc.style         = 0;
	wc.lpfnWndProc   = WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = TEXT("mgdata");
	if (!RegisterClass(&wc)) return 0;

	if ((hwnd_main = CreateWindow(TEXT("mgdata"), TEXT("Mgdata"), WS_MINIMIZEBOX | WS_SYSMENU,
	200, 200, 600, 500, NULL, NULL, hInstance, NULL)) == NULL) return 0;

	if ((hwnd = CreateWindow(TEXT("static"), TEXT("Char"), WS_CHILD | WS_VISIBLE,
	130, 0, 40, 20, hwnd_main, NULL, hInstance, NULL)) == NULL) return 0;
	hfont = CreateFont(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("ÇlÇr ÉSÉVÉbÉN"));
	SendMessage(hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);

	if ((hwnd = CreateWindow(TEXT("static"), TEXT("Stage"), WS_CHILD | WS_VISIBLE,
	420, 0, 40, 20, hwnd_main, NULL, hInstance, NULL)) == NULL) return 0;
	SendMessage(hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);

	if ((hcharedit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("edit"), NULL, WS_CHILD | WS_HSCROLL | WS_VISIBLE | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE,
	0, 15, 296, 308, hwnd_main, NULL, hInstance, NULL)) == NULL) return 0;
	SendMessage(hcharedit, WM_SETFONT, (WPARAM)hfont, TRUE);

	if ((hstageedit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("edit"), NULL, WS_CHILD | WS_HSCROLL | WS_VISIBLE | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE,
	298, 15, 296, 308, hwnd_main, NULL, hInstance, NULL)) == NULL) return 0;
	SendMessage(hstageedit, WM_SETFONT, (WPARAM)hfont, TRUE);

	if ((hwnd = CreateWindowEx(0, TEXT("button"), TEXT("CharCopy"), WS_CHILD | WS_VISIBLE,
	90, 324, 100, 30, hwnd_main, (HMENU)CHARCOPYBUTTON, hInstance, NULL)) == NULL) return 0;
	SendMessage(hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);

	if ((hwnd = CreateWindowEx(0, TEXT("button"), TEXT("StageCopy"), WS_CHILD | WS_VISIBLE,
	410, 324, 100, 30, hwnd_main, (HMENU)STAGECOPYBUTTON, hInstance, NULL)) == NULL) return 0;
	SendMessage(hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);

	if ((hwnd = CreateWindowEx(0, TEXT("button"), TEXT("Get"), WS_CHILD | WS_VISIBLE,
	245, 326, 100, 25, hwnd_main, (HMENU)GETBUTTON, hInstance, NULL)) == NULL) return 0;
	SendMessage(hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);

	if ((hlogedit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
	0, 355, 594, 97, hwnd_main, NULL, hInstance, NULL)) == NULL) return 0;
	SendMessage(hlogedit, WM_SETFONT, (WPARAM)hfont, TRUE);

	if ((hwnd = CreateWindowEx(0, TEXT("button"), TEXT("Close"), WS_CHILD | WS_VISIBLE,
	245, 454, 100, 20, hwnd_main, (HMENU)CLOSEBUTTON, hInstance, NULL)) == NULL) return 0;
	SendMessage(hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);

	ShowWindow(hwnd_main, SW_SHOWNORMAL);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DeleteObject(hfont);
	CloseHandle(hmutex);
	return msg.wParam;
}