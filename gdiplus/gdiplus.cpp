#define _WIN32_WINNT 0x400
#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
using namespace Gdiplus;

HWND g_hwnd, g_hwnd_canvas;

LRESULT CALLBACK procWindow1(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_SIZE: {
		RECT rect;
		if (!GetClientRect(hwnd, &rect)) return 0;
		SetWindowPos(g_hwnd_canvas, NULL, 0, 0, rect.right - 20, rect.bottom - 20, SWP_NOMOVE);

		if (!GetClientRect(g_hwnd_canvas, &rect)) return 0;
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE;

		si.nPage = rect.right - rect.left;
		if (si.nPage == 0) return 0;
		SetScrollInfo(g_hwnd_canvas, SB_HORZ, &si, true);

		si.nPage = rect.bottom - rect.top;
		if (si.nPage == 0) return 0;
		SetScrollInfo(g_hwnd_canvas, SB_VERT, &si, true);

		InvalidateRect(g_hwnd_canvas, NULL, false);
	}
		return 0;

	case WM_MOUSEWHEEL: {
		SendMessage(g_hwnd_canvas, WM_VSCROLL, MAKEWPARAM(GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? SB_PAGEUP : SB_PAGEDOWN, 0), (LPARAM)NULL);
	}
		return 0;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK procWindow2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static bool drag_mode = false;
	static int mouse_x, mouse_y;

	switch (uMsg) {
	case WM_PAINT: {

		RECT rect;
		if (!GetClientRect(hwnd, &rect)) return 0;
		Bitmap offscreen(rect.right - rect.left, rect.bottom - rect.top);

		Graphics graphics(&offscreen);
		graphics.Clear(Color(255, 255, 255, 255));
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);

		SCROLLINFO si_h;
		SCROLLINFO si_v;
		si_h.cbSize = sizeof(SCROLLINFO);
		si_h.fMask = SIF_POS | SIF_RANGE;
		si_v.cbSize = sizeof(SCROLLINFO);
		si_v.fMask = SIF_POS | SIF_RANGE;
		if (GetScrollInfo(hwnd, SB_HORZ, &si_h) && GetScrollInfo(hwnd, SB_VERT, &si_v)) graphics.TranslateTransform((REAL)-si_h.nPos, (REAL)-si_v.nPos);

		Pen pen(Color(255, 0, 0, 0), 2);
		SolidBrush brush(Color(255, 128, 128, 255));
		Point points[4];
		points[0].X = 100; points[0].Y = 100;
		points[1].X = 300; points[1].Y = 100;
		points[2].X = 350; points[2].Y = 300;
		points[3].X = 50; points[3].Y = 300;
		graphics.FillPolygon(&brush, points, sizeof(points) / sizeof(points[0]));
		graphics.DrawPolygon(&pen, points, sizeof(points) / sizeof(points[0]));

		if (drag_mode) {
			Pen pen_mouse(Color(128, 0, 0, 0), 2);
			Point points_mouse[3];
			points_mouse[0].X = 100; points_mouse[0].Y = 100;
			points_mouse[1].X = mouse_x + si_h.nPos; points_mouse[1].Y = mouse_y + si_v.nPos;
			points_mouse[2].X = 350; points_mouse[2].Y = 300;

			graphics.DrawLines(&pen_mouse, points_mouse, sizeof(points_mouse) / sizeof(points_mouse[0]));
		}

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		if (hdc == NULL) return 0;
		Graphics onscreen(hdc);
		onscreen.DrawImage(&offscreen, 0, 0);
		EndPaint(hwnd, &ps);
	}
		return 0;

	case WM_ERASEBKGND:
		return 0;

	case WM_LBUTTONDOWN:
		drag_mode = true;
		SetCapture(hwnd);
	case WM_MOUSEMOVE:
		mouse_x = GET_X_LPARAM(lParam);
		mouse_y = GET_Y_LPARAM(lParam);

		InvalidateRect(g_hwnd_canvas, NULL, false);
		return 0;

	case WM_LBUTTONUP:
		drag_mode = false;
		ReleaseCapture();
		InvalidateRect(g_hwnd_canvas, NULL, false);
		return 0;

	case WM_HSCROLL: {
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS | SIF_RANGE | SIF_TRACKPOS;
		if (!GetScrollInfo(hwnd, SB_HORZ, &si)) return 0;

		switch (LOWORD(wParam)) {
		case SB_LEFT:
			si.nPos = si.nMin;
			break;
		case SB_RIGHT:
			si.nPos = si.nMax;
			break;
		case SB_LINELEFT:
			si.nPos -= 10;
			break;
		case SB_LINERIGHT:
			si.nPos += 10;
			break;
		case SB_PAGELEFT:
			si.nPos -= 100;
			break;
		case SB_PAGERIGHT:
			si.nPos += 100;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;
		}
		SetScrollPos(hwnd, SB_HORZ, si.nPos, true);
		InvalidateRect(hwnd, NULL, false);
	}
		return 0;

	case WM_VSCROLL: {
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS | SIF_RANGE | SIF_TRACKPOS;
		if (!GetScrollInfo(hwnd, SB_VERT, &si)) return 0;

		switch (LOWORD(wParam)) {
		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;
		case SB_LINEDOWN:
			si.nPos += 10;
			break;
		case SB_LINEUP:
			si.nPos -= 10;
			break;
		case SB_PAGEDOWN:
			si.nPos += 100;
			break;
		case SB_PAGEUP:
			si.nPos -= 100;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;
		case SB_TOP:
			si.nPos = si.nMax;
			break;
		}
		SetScrollPos(hwnd, SB_VERT, si.nPos, true);
		InvalidateRect(hwnd, NULL, false);
	}
		return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

INT_PTR CALLBACK procDialog(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return false;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	GdiplusStartupInput gsi;
	ULONG_PTR gdiplus_token;
	if (GdiplusStartup(&gdiplus_token, &gsi, NULL) != Ok) return 0;

	WNDCLASS wc;
	wc.style = 0;
	wc.lpfnWndProc = procWindow1;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("test1");
	if (RegisterClass(&wc) == 0) return 0;

	wc.lpfnWndProc = procWindow2;
	wc.lpszClassName = TEXT("test2");
	if (RegisterClass(&wc) == 0) return 0;

	g_hwnd = CreateWindow(TEXT("test1"), TEXT("Test Window"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, NULL);
	if (g_hwnd == NULL) return 0;

	g_hwnd_canvas = CreateWindow(TEXT("test2"), NULL, WS_CHILD | WS_HSCROLL | WS_VSCROLL | WS_VISIBLE, 10, 10, CW_USEDEFAULT, CW_USEDEFAULT, g_hwnd, NULL, hInstance, NULL);
	if (g_hwnd_canvas == NULL) return 0;

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE | SIF_DISABLENOSCROLL;
	si.nMin = 0;
	si.nMax = 1999;
	SetScrollInfo(g_hwnd_canvas, SB_HORZ, &si, false);
	SetScrollInfo(g_hwnd_canvas, SB_VERT, &si, false);

	ShowWindow(g_hwnd, SW_SHOWNORMAL);

	MSG msg;
	BOOL ret;
	for (;;) {
		ret = GetMessage(&msg, NULL, 0, 0);
		if (ret == 0 || ret == -1) break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	GdiplusShutdown(gdiplus_token);
	return msg.wParam;
}
