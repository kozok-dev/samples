#define UNICODE
#include <windows.h>
#include <commctrl.h>

/* ���p�\�ȃc�[���o�[�{�^���B*/
TBBUTTON tbb[] = {
	{0, 0, TBSTATE_ENABLED, TBSTYLE_DROPDOWN, {0, 0}, (DWORD_PTR)TEXT("New")},
	{1, 1, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, (DWORD_PTR)TEXT("Open")},
	{2, 2, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, (DWORD_PTR)TEXT("Save")},
	{3, 3, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, (DWORD_PTR)TEXT("Cut")},
	{4, 4, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, (DWORD_PTR)TEXT("Copy")},
	{5, 5, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, (DWORD_PTR)TEXT("Paste")},
	{6, 6, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, (DWORD_PTR)TEXT("Undo")},
	{7, 7, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, (DWORD_PTR)TEXT("Redo")}
};
TBBUTTON tbs = {0, 0, 0, TBSTYLE_SEP};	/* �c�[���o�[�̃Z�p���[�^�B*/

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int i;
	RECT rect;
	HMENU hmenu;
	TPMPARAMS tpmp;
	LPNMTTDISPINFO pnttdi;
	LPNMTOOLBAR pntb;
	static HWND htb;
	static HCURSOR hc;

	switch (uMsg) {
	case WM_CREATE:
		htb = CreateToolbarEx(hwnd, WS_VISIBLE | CCS_ADJUSTABLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS, 1, 8,
		((LPCREATESTRUCT)lParam)->hInstance, 3, tbb, sizeof(tbb) / sizeof(tbb[0]), 0, 0, 16, 16, sizeof(TBBUTTON));
		SendMessage(htb, TB_INSERTBUTTON, 3, (LPARAM)&tbs);
		SendMessage(htb, TB_INSERTBUTTON, 7, (LPARAM)&tbs);

		hc = LoadCursor(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(2));
		SetClassLong(htb, GCL_HCURSOR, (LONG)hc);
		return 0;
	case WM_SIZE:
		SendMessage(htb, WM_SIZE, wParam, lParam);
		return 0;
	case WM_NOTIFY:
		SetCursor(hc);
		switch (((LPNMHDR)lParam)->code) {
		case TBN_INITCUSTOMIZE:
			return TBNRF_HIDEHELP;	/* �w���v�{�^���͕K�v�����B*/
		case TBN_QUERYINSERT:
		case TBN_QUERYDELETE:
			/* �c�[���o�[�{�^���̒ǉ�/�폜�����B*/
			return TRUE;
		case TBN_GETBUTTONINFO:
			/* ���p�\�ȃc�[���o�[�{�^����ݒ肷��B*/
			pntb = (LPNMTOOLBAR)lParam;
			if (pntb->iItem < sizeof(tbb) / sizeof(tbb[0])) {
				pntb->tbButton = tbb[pntb->iItem];
				pntb->tbButton.iString = tbb[pntb->iItem].dwData;
				return TRUE;
			}
			break;
		case TBN_RESET:
			/* ��x�S�č폜���ď�����Ԃɐݒ肷��B*/
			i = SendMessage(htb, TB_BUTTONCOUNT, 0, 0);
			while (i--) SendMessage(htb, TB_DELETEBUTTON, 0, 0);
			SendMessage(htb, TB_ADDBUTTONS, 8, (LPARAM)tbb);
			SendMessage(htb, TB_INSERTBUTTON, 3, (LPARAM)&tbs);
			SendMessage(htb, TB_INSERTBUTTON, 7, (LPARAM)&tbs);
			break;
		case TTN_GETDISPINFO:	/* �c�[���o�[�̃c�[���`�b�v�\���B*/
			pnttdi = (LPNMTTDISPINFO)lParam;
			pnttdi->lpszText = (LPTSTR)tbb[pnttdi->hdr.idFrom].dwData;
			pnttdi->hinst = NULL;
			break;
		case TBN_DROPDOWN:	/* �c�[���o�[�̃h���b�v�_�E���B*/
			pntb = (LPNMTOOLBAR)lParam;

			SendMessage(pntb->hdr.hwndFrom, TB_GETRECT, (WPARAM)pntb->iItem, (LPARAM)&rect);
			MapWindowPoints(pntb->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rect, 2);

			hmenu = CreatePopupMenu();
			AppendMenu(hmenu, MF_STRING, 1, TEXT("Option 1"));
			AppendMenu(hmenu, MF_STRING, 2, TEXT("Option 2"));
			AppendMenu(hmenu, MF_STRING, 3, TEXT("Option 3"));
			tpmp.cbSize = sizeof(TPMPARAMS);
			tpmp.rcExclude = rect;
			TrackPopupMenuEx(hmenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_VERTICAL, rect.left, rect.bottom, hwnd, &tpmp);
			DestroyMenu(hmenu);
			break;
		case NM_RDBLCLK:
			SendMessage(htb, TB_CUSTOMIZE, 0, 0);	/* �c�[���o�[�J�X�^�}�C�Y�_�C�A���O��\������B*/
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
	MSG msg;

	wc.style = 0;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
	wc.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(2));
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("tbtest");
	if (!RegisterClass(&wc)) return 0;

	InitCommonControls();

	if (CreateWindow(
		wc.lpszClassName, TEXT("ToolBar Test"), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		300, 300, 250, 51, NULL, NULL, hInstance, NULL
	) == NULL) return 0;

	while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);
	return msg.wParam;
}
