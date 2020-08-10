#define UNICODE
#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

HINSTANCE g_hinst;

class SimpleGpImg {
	HWND m_hwnd;

public:
	WCHAR m_fn[256];

	SimpleGpImg(HWND hwnd) {
		WNDCLASS wc;

		m_fn[0] = L'\0';

		if (!GetClassInfo(g_hinst, TEXT("sgi"), &wc)) {
			wc.style = 0;
			wc.lpfnWndProc = procWnd;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = g_hinst;
			wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
			wc.lpszMenuName = NULL;
			wc.lpszClassName = TEXT("sgi");
			if (!RegisterClass(&wc)) return;
		}

		m_hwnd = CreateWindowEx(WS_EX_STATICEDGE, wc.lpszClassName, NULL, WS_CHILD | WS_VISIBLE,
		180, 0, 100, 100, hwnd, NULL, g_hinst, this);
	}

	void changeImg(HWND hwnd) {
		CommDlg_OpenSave_GetFilePathW(hwnd, m_fn, sizeof(m_fn) / sizeof(m_fn[0]));
		InvalidateRect(m_hwnd, NULL, TRUE);
	}

	LRESULT Paint() {
		RECT rect;
		HDC hdc;
		PAINTSTRUCT ps;

		hdc = BeginPaint(m_hwnd, &ps);
		Graphics g(hdc);
		Image img(m_fn);
		GetClientRect(m_hwnd, &rect);
		if (img.GetWidth() < rect.right) rect.right = img.GetWidth();
		if (img.GetHeight() < rect.bottom) rect.bottom = img.GetHeight();
		g.DrawImage(&img, 0, 0, rect.right, rect.bottom);
		EndPaint(m_hwnd, &ps);
		return 0;
	}

	static LRESULT CALLBACK procWnd(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		static SimpleGpImg *sgi;

		switch (uMsg) {
		case WM_CREATE:
			sgi = (SimpleGpImg *)((LPCREATESTRUCT)lParam)->lpCreateParams;
			return 0;
		case WM_PAINT:
			return sgi->Paint();
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
};

UINT_PTR CALLBACK OFNHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	static SimpleGpImg *sgi;

	switch (uiMsg) {
	case WM_INITDIALOG:
		if (sgi != NULL) delete sgi;
		sgi = new SimpleGpImg(hdlg);
		return TRUE;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
		case CDN_SELCHANGE:
			sgi->changeImg(GetParent(hdlg));
			break;
		}
		return TRUE;
	}
	return FALSE;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WCHAR fn[256] = {L'\0'};
	OPENFILENAMEW ofn;

	switch (uMsg) {
	case WM_LBUTTONDOWN:
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;
		ofn.hInstance = g_hinst;
		ofn.lpstrFilter = L"All files (*.*)\0*.*\0\0";
		ofn.lpstrFile = fn;
		ofn.nMaxFile = sizeof(fn) / sizeof(fn[0]);
		ofn.Flags = OFN_ENABLEHOOK | OFN_ENABLESIZING | OFN_ENABLETEMPLATE | OFN_EXPLORER;
		ofn.lpfnHook = OFNHookProc;
		ofn.lpTemplateName = MAKEINTRESOURCEW(1);
		GetOpenFileNameW(&ofn);
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
	GdiplusStartupInput gsi;
	ULONG_PTR gt;

	GdiplusStartup(&gt, &gsi, NULL);

	wc.style = 0;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("oftest");
	if (!RegisterClass(&wc)) return 0;

	g_hinst = hInstance;

	if (CreateWindow(wc.lpszClassName, TEXT("OpenFile Test"), WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE,
	250, 250, 200, 150, NULL, NULL, hInstance, NULL) == NULL) return 0;

	while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);
	GdiplusShutdown(gt);
	return msg.wParam;
}
