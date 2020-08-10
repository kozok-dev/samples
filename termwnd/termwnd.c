#define _WIN32_IE 0x500
#include <stdio.h>
#include <windows.h>
#include <psapi.h>

struct wndpid {
	HWND hwnd;
	DWORD pid;
} wndpid[100];

struct pidwnd {
	DWORD pid;
	char filename[256];
} pidwnd[100];

int n_wndpid, n_pidwnd;
HWND hedit;
HMENU hmenu;
NOTIFYICONDATA nid;

BOOL CALLBACK EnumWindowProc(HWND hwnd, LPARAM lParam)
{
	DWORD pid;
	int i;

	GetWindowThreadProcessId(hwnd, &pid);

	for (i = 0; i < n_pidwnd; i++) {
		if (pid == pidwnd[i].pid) break;
	}
	if (i < n_pidwnd) {
		wndpid[n_wndpid].hwnd = hwnd;
		wndpid[n_wndpid].pid  = pid;
		n_wndpid++;
	}

	return TRUE;
}

void termWnd(HWND hwnd)
{
	DWORD pid[100];
	HMODULE module[100];
	char filename[256];
	DWORD n_pid;
	DWORD n_module;
	HANDLE hproc;
	int i, j, k, v, t = 0;
	MENUITEMINFO mii;

	/* Init counter */
	n_wndpid = n_pidwnd = 0;

	/* Get running processes */
	EnumProcesses(pid, sizeof(pid), &n_pid);
	n_pid /= sizeof(DWORD);
	for (i = 0; i < n_pid; i++) {
		if ((hproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid[i])) != NULL) {
			if (EnumProcessModules(hproc, module, sizeof(module), &n_module)) {
				GetModuleBaseName(hproc, module[0], filename, sizeof(filename));
				if (!lstrcmp(filename, "IEXPLORE.EXE")) {
					pidwnd[n_pidwnd].pid = pid[i];
					lstrcpy(pidwnd[n_pidwnd].filename, filename);
					n_pidwnd++;
				}
			}
		}
	}

	/* Get started windows */
	EnumWindows(EnumWindowProc, 0);

	/* Check disable windows */
	for (i = 0; i < n_pidwnd; i++) {
		/* Count same process-id */
		k = 0;
		for (j = 0; j < n_wndpid; j++) {
			if (pidwnd[i].pid == wndpid[j].pid) k++;
		}

		/* Check */
		v = 0;
		for (j = 0; j < n_wndpid; j++) {
			if (pidwnd[i].pid == wndpid[j].pid) {
				/*if (IsWindowVisible(wndpid[j].hwnd)) break;*/
				v++;
			}
		}
		if (v == k) {	/* Is all disable window? */
			hproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pidwnd[i].pid);
			TerminateProcess(hproc, 0);
			t++;
		}
	}

	CloseHandle(hproc);

	/* Result window */
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	GetMenuItemInfo(hmenu, 101, FALSE, &mii);
	if (mii.fState == MFS_CHECKED) {
		nid.uFlags |= 0x10;	/* Add balloon tip flag */
		wsprintf(nid.szInfo, "%d %s performed", t, t == 1 ? "was" : "were");
		Shell_NotifyIcon(NIM_MODIFY, &nid);
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char strtwtime[10];
	static int twtime;
	POINT pt;
	static MENUITEMINFO mii;

	switch (uMsg) {
	case WM_CREATE:
		/* Menu */
		hmenu = CreatePopupMenu();
		InsertMenu(hmenu, -1, MF_BYPOSITION, 100, "Run");
		InsertMenu(hmenu, -1, MF_BYPOSITION | MF_CHECKED, 101, "Show result window");
		InsertMenu(hmenu, -1, MF_BYPOSITION, 102, "Setting");
		InsertMenu(hmenu, -1, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
		InsertMenu(hmenu, -1, MF_BYPOSITION, 103, "Exit");
		SetMenuDefaultItem(hmenu, 100, FALSE);
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_STATE;
		mii.fState = MFS_CHECKED;

		/* Task tray */
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hwnd;
		nid.uID = 100;
		nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		nid.uCallbackMessage = WM_USER;
		nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		lstrcpy(nid.szTip, "Terminate windows");
		lstrcpy(nid.szInfoTitle, "Terminate windows");
		nid.dwInfoFlags = 1;	/* Icon flag */
		nid.uTimeout = 10000;	/* Minimum value */
		Shell_NotifyIcon(NIM_ADD, &nid);

		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 100:
			termWnd(hwnd);
			break;
		case 101:
			mii.fState = mii.fState == MFS_CHECKED ? MFS_UNCHECKED : MFS_CHECKED;
			SetMenuItemInfo(hmenu, 101, FALSE, &mii);
			break;
		case 102:
			wsprintf(strtwtime, "%d", twtime);
			SetWindowText(hedit, strtwtime);
			ShowWindow(hwnd, SW_SHOW);
			break;
		case 103:
			DestroyWindow(hwnd);
			break;
		case 104:
			/* Reset timer */
			GetWindowText(hedit, strtwtime, sizeof(strtwtime));
			sscanf(strtwtime, "%d", &twtime);
			KillTimer(hwnd, 100);
			if (twtime > 0) SetTimer(hwnd, 100, twtime, NULL);
			ShowWindow(hwnd, SW_HIDE);
			break;
		case 105:
			ShowWindow(hwnd, SW_HIDE);
			break;
		}
		return 0;
	case WM_TIMER:
		termWnd(hwnd);
		return 0;
	case WM_USER:
		/* From tasktray */
		switch (lParam) {
		case WM_RBUTTONDOWN:
			SetForegroundWindow(hwnd);	/* If popup by tasktray, important */
			GetCursorPos(&pt);
			TrackPopupMenu(hmenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hwnd, NULL);
			break;
		case WM_LBUTTONDBLCLK:
			termWnd(hwnd);
			break;
		}
		return 0;
	case WM_CLOSE:
		ShowWindow(hwnd, SW_HIDE);
		return 0;
	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE, &nid);
		DestroyMenu(hmenu);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstanace, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wc;
	HANDLE hmutex;
	HFONT hfont;
	RECT rect1, rect2;
	HWND hwnd, hbtn1, hbtn2, hbtn3;

	/* Double startup safety */
	hmutex = CreateMutex(NULL, FALSE, "termwnd");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(hmutex);
		return 0;
	}

	/* Window class */
	wc.style         = CS_DBLCLKS;
	wc.lpfnWndProc   = WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "termwnd";
	if (!RegisterClass(&wc)) return 0;

	/* Create windows */
	if (!(hwnd = CreateWindow(
		"termwnd", "Terminate windows",
		WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 200, 150,
		NULL, NULL, hInstance, NULL
	))) return 0;
	if (!(hedit = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"edit", "0",
		WS_CHILD | WS_VISIBLE | ES_NUMBER,
		48, 20, 100, 18,
		hwnd, NULL, hInstance, NULL
	))) return 0;
	if (!(hbtn1 = CreateWindow(
		"button", "OK",
		WS_CHILD | WS_VISIBLE,
		18, 57, 70, 20,
		hwnd, (HMENU)104, hInstance, NULL
	))) return 0;
	if (!(hbtn2 = CreateWindow(
		"button", "Cancel",
		WS_CHILD | WS_VISIBLE,
		106, 57, 70, 20,
		hwnd, (HMENU)105, hInstance, NULL
	))) return 0;
	if (!(hbtn3 = CreateWindow(
		"button", "Exit",
		WS_CHILD | WS_VISIBLE,
		63, 87, 70, 20,
		hwnd, (HMENU)103, hInstance, NULL
	))) return 0;

	/* Main window move center position */
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect1, 0);	/* Display size (without taskbar) */
	GetWindowRect(hwnd, &rect2);
	MoveWindow(
		hwnd,
		rect1.right / 2 - ((rect2.right - rect2.left) / 2), rect1.bottom / 2 - ((rect2.bottom - rect2.top) / 2),
		rect2.right - rect2.left, rect2.bottom - rect2.top,
		0
	);

	/* Set window's font */
	hfont = CreateFont(
		12, 0, 0, 0, FW_NORMAL,
		FALSE, FALSE, FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "‚l‚r ‚oƒSƒVƒbƒN"
	);
	SendMessage(hedit, WM_SETFONT, (WPARAM)hfont, 0);
	SendMessage(hbtn1, WM_SETFONT, (WPARAM)hfont, 0);
	SendMessage(hbtn2, WM_SETFONT, (WPARAM)hfont, 0);
	SendMessage(hbtn3, WM_SETFONT, (WPARAM)hfont, 0);

	/* Message loop */
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DeleteObject(hfont);
	CloseHandle(hmutex);
	return msg.wParam;
}
