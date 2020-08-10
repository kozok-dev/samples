#include <windows.h>
#include <objbase.h>
#define DLGDBG 0
#if DLGDBG
#include <stdio.h>
#endif

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	TCHAR text[50];
	LPOLESTR olestr;
	GUID guid;

	switch (uMsg) {
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 1:
			wsprintf(text, "%u", GetTickCount());
			SetDlgItemText(hwndDlg, 3, text);
			break;
		case 2:
			CoCreateGuid(&guid);
			StringFromCLSID(&guid, &olestr);
			SetDlgItemTextW(hwndDlg, 3, olestr);
			break;
		}
		return TRUE;
	case WM_CLOSE:
		DestroyWindow(hwndDlg);
		return TRUE;
	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;
	}
	return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hdlg;
	MSG msg;
	LPDLGTEMPLATE dlg;
	LPDLGITEMTEMPLATE dlgitem;
	LPWORD w;
	LPVOID dbg;
#if DLGDBG
	FILE *f = fopen("dlgdbg.txt", "w");
	int i;
#endif

	dlg = (LPDLGTEMPLATE)GlobalAlloc(GPTR, 256);
	dlg->style = WS_MINIMIZEBOX | WS_SYSMENU | DS_SETFONT;
	dlg->cdit = 3;
	dlg->x = 100;
	dlg->y = 100;
	dlg->cx = 200;
	dlg->cy = 150;
	w = (LPWORD)(dlg + 1);
	*w++ = 0;
	*w++ = 0;
	w += MultiByteToWideChar(CP_ACP, 0, "DialogTest", -1, (LPWSTR)w, 50);
	*w++ = 9;
	w += MultiByteToWideChar(CP_ACP, 0, "MS Shell Dlg", -1, (LPWSTR)w, 50);

	w = (LPWORD)(((ULONG)w + 3) & ~3);
	dlgitem = (LPDLGITEMTEMPLATE)w;
	dlgitem->style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON;
	dlgitem->dwExtendedStyle = 0;
	dlgitem->x = 10;
	dlgitem->y = 10;
	dlgitem->cx = 50;
	dlgitem->cy = 30;
	dlgitem->id = 1;
	w = (LPWORD)(dlgitem + 1);
	*w++ = 0xffff;
	*w++ = 0x0080;
	w += MultiByteToWideChar(CP_ACP, 0, "Button1", -1, (LPWSTR)w, 50);
	*w++ = 0;

	w = (LPWORD)(((ULONG)w + 3) & ~3);
	dlgitem = (LPDLGITEMTEMPLATE)w;
	dlgitem->style = WS_CHILD | WS_VISIBLE;
	dlgitem->dwExtendedStyle = 0;
	dlgitem->x = 70;
	dlgitem->y = 10;
	dlgitem->cx = 50;
	dlgitem->cy = 30;
	dlgitem->id = 2;
	w = (LPWORD)(dlgitem + 1);
	*w++ = 0xffff;
	*w++ = 0x0080;
	w += MultiByteToWideChar(CP_ACP, 0, "Button2", -1, (LPWSTR)w, 50);
	*w++ = 0;

	w = (LPWORD)(((ULONG)w + 3) & ~3);
	dlgitem = (LPDLGITEMTEMPLATE)w;
	dlgitem->style = WS_CHILD | WS_BORDER | WS_VISIBLE;
	dlgitem->dwExtendedStyle = 0;
	dlgitem->x = 10;
	dlgitem->y = 50;
	dlgitem->cx = 175;
	dlgitem->cy = 30;
	dlgitem->id = 3;
	w = (LPWORD)(dlgitem + 1);
	*w++ = 0xffff;
	*w++ = 0x0082;
	w += MultiByteToWideChar(CP_ACP, 0, "Static", -1, (LPWSTR)w, 50);
	*w++ = 0;

#if DLGDBG
	if (f) {
		fprintf(f, "Memory dump of dialog template (Alignment: WORD)\n");
		for (w = (LPWORD)dlg, i = 0; i < 128; w++, i++) {
			fprintf(f, "%u(0x%x): %u", w, w, *w);
			if (iswprint(*w)) fwprintf(f, L"(%c)", *w);
			fprintf(f, "\n");
		}
		fclose(f);
	}
#endif

	if (hdlg = CreateDialogIndirect(hInstance, dlg, NULL, DialogProc)) {
		GlobalFree(dlg);
		ShowWindow(hdlg, SW_SHOWNORMAL);
		while (GetMessage(&msg, NULL, 0, 0)) {
			if (!IsDialogMessage(hdlg, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	} else {
		GlobalFree(dlg);
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), 0, (LPTSTR)&dbg, 0, NULL
		);
		MessageBox(NULL, (LPCTSTR)dbg, "DialogTest", MB_OK);
		LocalFree(dbg);
	}
	return 0;
}