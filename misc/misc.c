#define _WIN32_IE 0x500
#include <windows.h>
#include <commctrl.h>
#include <olectl.h>
#include <shlwapi.h>

#define testbmp 1000

HWND hwnd, hsta, htxt, hbtn, hlistbox, hdirlist, htree;
HIMAGELIST himglist;
char drives[26][3];

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char drive[4];
	HMENU hmenu1, hmenu2;
	HBRUSH hbrush;
	HDC hdc, hmem;
	HANDLE hfind;
	POINT pt;
	PAINTSTRUCT ps;
	LPDRAWITEMSTRUCT lpDraw;
	LPMEASUREITEMSTRUCT lpMeasure;
	TVINSERTSTRUCT tvi;
	HTREEITEM hti;
	SHFILEINFO fi;
	WIN32_FIND_DATA fd;
	WCHAR fbuf[MAX_PATH];
	static pix[10];
	static HWND htool;
	static HFONT hfont;
	static HBITMAP hbitmap;
	static TOOLINFO ti;
	static LPPICTURE lpPict;
	static OLE_XSIZE_HIMETRIC width;
	static OLE_YSIZE_HIMETRIC height;

	switch (uMsg) {
	case WM_CREATE:
		hfont = CreateFont(
			17, 0, 0, 0, FW_BOLD,
			FALSE, FALSE, FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH, "Times New Roman"
		);
		hbitmap = LoadImage(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(testbmp), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);

		InitCommonControls();

		/* Tooltip */
		htool = CreateWindowEx(
			0,
			TOOLTIPS_CLASS, NULL,
			TTS_ALWAYSTIP | TTS_BALLOON,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL
		);
		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
		ti.hwnd = hwnd;
		ti.uId = (UINT)hwnd;
		ti.lpszText = "TooltipTest";
		SendMessage(htool, TTM_SETTITLEA, (WPARAM)1, (LPARAM)"Tooltips");
		SendMessage(htool, TTM_ADDTOOL, 0, (LPARAM)&ti);

		/* Image list */
		himglist = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR16 | ILC_MASK, 3, 0);
		SHGetFileInfo("c:\\", 0, &fi, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON);
		ImageList_AddIcon(himglist, fi.hIcon);
		DestroyIcon(fi.hIcon);
		SHGetFileInfo("c:\\WINNT", 0, &fi, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON);
		ImageList_AddIcon(himglist, fi.hIcon);
		DestroyIcon(fi.hIcon);
		SHGetFileInfo("c:\\WINNT", 0, &fi, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_OPENICON);
		ImageList_AddIcon(himglist, fi.hIcon);
		DestroyIcon(fi.hIcon);

		/* Tree view */
		htree = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			WC_TREEVIEW, NULL,
			WS_CHILD | WS_HSCROLL | WS_VSCROLL | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT,
			20, 170, 150, 140,
			hwnd, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL
		);
		TreeView_SetImageList(htree, himglist, TVSIL_NORMAL);
		tvi.hParent = TVI_ROOT;
		tvi.hInsertAfter = TVI_LAST;
		tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvi.item.iImage = 0;
		tvi.item.iSelectedImage = 0;
		tvi.item.pszText = "C:";
		hti = TreeView_InsertItem(htree, &tvi);

		tvi.hParent = hti;
		tvi.item.iImage = 1;
		tvi.item.iSelectedImage = 2;

		hfind = FindFirstFile("c:\\*", &fd);
		if (hfind != INVALID_HANDLE_VALUE) {
			do {
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					tvi.item.pszText = fd.cFileName;
					TreeView_InsertItem(htree, &tvi);
				}
			} while(FindNextFile(hfind, &fd));
		}
		FindClose(hfind);

		TreeView_Expand(htree, hti, TVE_EXPAND);

		/* Image by OLE */
		GetModuleFileNameW(NULL, fbuf, MAX_PATH);
		*(StrRChrW(fbuf, NULL, '\\') + 1) = (WCHAR)NULL;
		lstrcatW(fbuf, L"misc.jpg");
		OleLoadPicturePath(fbuf, NULL, 0, 0, &IID_IPicture, (void **)&lpPict);
		if (lpPict) {
			lpPict->lpVtbl->get_Width(lpPict, &width);
			lpPict->lpVtbl->get_Height(lpPict, &height);
		}

		return 0;
	case WM_RBUTTONDOWN:
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		ClientToScreen(hwnd, &pt);

		hmenu1 = CreatePopupMenu();
		hmenu2 = CreatePopupMenu();
		InsertMenu(hmenu1, -1, MF_BYPOSITION, 100, "TestPopup1");
		InsertMenu(hmenu1, -1, MF_BYPOSITION | MF_CHECKED | MF_GRAYED, 101, "TestPopup2");
		InsertMenu(hmenu1, -1, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
		InsertMenu(hmenu1, -1, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hmenu2, "TestPopup3");
		InsertMenu(hmenu2, 0, MF_BYPOSITION, 103, "SubPopup1");
		InsertMenu(hmenu2, 1, MF_BYPOSITION | MF_OWNERDRAW, 104, 0);
		TrackPopupMenu(hmenu1, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
		DestroyMenu(hmenu1);
		DestroyMenu(hmenu2);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case 100:
				MessageBox(hwnd, (LPCSTR)pix, "GetPixel test", MB_OK | MB_ICONINFORMATION);
				break;
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		hmem = CreateCompatibleDC(hdc);
		SelectObject(hmem, hbitmap);
		BitBlt(hdc, 0, 80, 70, 50, hmem, 0, 0, SRCCOPY);
		DeleteDC(hmem);
		wsprintf((LPSTR)pix, "#%06x", GetPixel(hdc, 0, 80));
		SetBkMode(hdc, TRANSPARENT);
		SelectObject(hdc, hfont);
		TextOut(hdc, 10, 105, "Text by TextOut", strlen("Text by TextOut"));
		if (lpPict) lpPict->lpVtbl->Render(lpPict, hdc, 180, 140, MulDiv(width, GetDeviceCaps(hdc, LOGPIXELSX), 2540), MulDiv(height, GetDeviceCaps(hdc, LOGPIXELSY), 2540), 0, height, width, -height, NULL);
		EndPaint(hwnd, &ps);

		break;
	case WM_MEASUREITEM:
		lpMeasure = (LPMEASUREITEMSTRUCT)lParam;
		if (lpMeasure->itemID == 104) lpMeasure->itemHeight = 30;
		return 0;
	case WM_DRAWITEM:
		lpDraw = (LPDRAWITEMSTRUCT)lParam;

		if (lpDraw->CtlID == 102) {	/* ComboBox */
			if (lpDraw->itemState & ODS_FOCUS) {
				hbrush = GetSysColorBrush(COLOR_HIGHLIGHT);
				SetBkColor(lpDraw->hDC, GetSysColor(COLOR_HIGHLIGHT));
				SetTextColor(lpDraw->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
			} else {
				hbrush = CreateSolidBrush(GetBkColor(lpDraw->hDC));
			}
			FillRect(lpDraw->hDC, &lpDraw->rcItem, hbrush);
			DeleteObject(hbrush);

			wsprintf(drive, "%s\\", drives[lpDraw->itemData]);
			SHGetFileInfo(drive, 0, &fi, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON);
			DrawIconEx(lpDraw->hDC, lpDraw->rcItem.left, lpDraw->rcItem.top, fi.hIcon, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0, NULL, DI_NORMAL);
			DestroyIcon(fi.hIcon);
			SetTextAlign(lpDraw->hDC, TA_BOTTOM);
			TextOut(lpDraw->hDC, lpDraw->rcItem.left + GetSystemMetrics(SM_CXSMICON) + 3, lpDraw->rcItem.bottom - 2, drives[lpDraw->itemData], lstrlen(drives[lpDraw->itemData]));
		} else if (lpDraw->itemID == 104) {	/* Popup */
			if (lpDraw->itemState & ODS_SELECTED) {
				hbrush = GetSysColorBrush(COLOR_DESKTOP);
				SetBkColor(lpDraw->hDC, GetSysColor(COLOR_DESKTOP));
				SetTextColor(lpDraw->hDC, RGB(255, 192, 0));
			} else {
				hbrush = CreateSolidBrush(GetBkColor(lpDraw->hDC));
			}
			FillRect(lpDraw->hDC, &lpDraw->rcItem, hbrush);
			DeleteObject(hbrush);

			SetTextAlign(lpDraw->hDC, TA_BOTTOM);
			SelectObject(lpDraw->hDC, hfont);
			TextOut(lpDraw->hDC, lpDraw->rcItem.left + GetSystemMetrics(SM_CXMENUCHECK), lpDraw->rcItem.bottom - 4, "SubPopup2", lstrlen("SubPopup2"));
		}

		return TRUE;
	case WM_DESTROY:
		DeleteObject(hfont);
		DeleteObject(hbitmap);
		ImageList_Destroy(himglist);
		if (lpPict) lpPict->lpVtbl->Release(lpPict);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HFONT hfont;
	MSG msg;
	WNDCLASS wc;
	HANDLE hmutex;
	DWORD drive;
	int d = 0, i;

	hmutex = CreateMutex(NULL, FALSE, "test");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		hwnd = FindWindow("test", "TestWindow");
		SetForegroundWindow(hwnd);
		ShowWindow(hwnd, SW_SHOWNORMAL);
		CloseHandle(hmutex);
		return 0;
	}

	wc.style         = 0;
	wc.lpfnWndProc   = WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "test";
	if (!RegisterClass(&wc)) return 0;

	if (!(hwnd = CreateWindow(
		"test", "TestWindow",
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
		200, 200, 250, 350,
		NULL, NULL, hInstance, NULL
	))) return 0;

	if (!(hsta = CreateWindowEx(
		WS_EX_STATICEDGE,
		"static", "StaticText",
		WS_CHILD | WS_VISIBLE,
		0, 3, 55, 14,
		hwnd, NULL, hInstance, NULL
	))) return 0;

	if (!(htxt = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"edit", "EditTest",
		WS_CHILD | WS_VISIBLE,
		60, 0, 100, 20,
		hwnd, NULL, hInstance, NULL
	))) return 0;

	if (!(hbtn = CreateWindow(
		"button", "Button",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		0, 30, 100, 40,
		hwnd, (HMENU)100, hInstance, NULL
	))) return 0;

	if (!(hlistbox = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"listbox", NULL,
		WS_CHILD | WS_VISIBLE,
		120, 30, 100, 100,
		hwnd, (HMENU)101, hInstance, NULL
	))) return 0;

	if (!(hdirlist = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"combobox", NULL,
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED,
		20, 140, 150, 130,
		hwnd, (HMENU)102, hInstance, NULL
	))) return 0;

	hfont = CreateFont(
		12, 0, 0, 0, FW_NORMAL,
		FALSE, FALSE, FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, "‚l‚r ‚oƒSƒVƒbƒN"
	);
	SendMessage(htxt, WM_SETFONT, (WPARAM)hfont, 0);
	SendMessage(hsta, WM_SETFONT, (WPARAM)hfont, 0);
	SendMessage(hbtn, WM_SETFONT, (WPARAM)hfont, 0);
	SendMessage(hlistbox, WM_SETFONT, (WPARAM)hfont, 0);
	SendMessage(hdirlist, WM_SETFONT, (WPARAM)hfont, 0);

	SendMessage(hlistbox, LB_INSERTSTRING, (WPARAM)0, (LPARAM)"Test");
	SendMessage(hlistbox, LB_INSERTSTRING, (WPARAM)1, (LPARAM)"ListBox");
	SendMessage(hlistbox, LB_INSERTSTRING, (WPARAM)2, (LPARAM)"Windows");
	SendMessage(hlistbox, LB_INSERTSTRING, (WPARAM)3, (LPARAM)"TestWindow");
	SendMessage(hlistbox, LB_INSERTSTRING, (WPARAM)4, (LPARAM)"Application");

	drive = GetLogicalDrives();
	for (i = 0; i < 26; i++) {
		if (drive & (1 << i)) {
			wsprintf(drives[d], "%c:", i + 65);
			SendMessage(hdirlist, CB_INSERTSTRING, (WPARAM)d, (LPARAM)d);
			d++;
		}
	}

	SendMessage(hlistbox, LB_SETCURSEL, (WPARAM)2, 0);
	SendMessage(hdirlist, CB_SETCURSEL, 0, 0);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DeleteObject(hfont);
	CloseHandle(hmutex);
	return msg.wParam;
}
