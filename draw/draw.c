#define UNICODE
#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rerect;
	HDC hdc;
	PAINTSTRUCT ps;
	static RECT rect;

	switch (uMsg) {
	case WM_LBUTTONDOWN:
		rect.left = rect.right = GET_X_LPARAM(lParam);
		rect.top = rect.bottom = GET_Y_LPARAM(lParam);
		SetCapture(hwnd);
	case WM_MOUSEMOVE:
		if (!(wParam & MK_LBUTTON)) return 0;

		rerect.right = GET_X_LPARAM(lParam);
		rerect.bottom = GET_Y_LPARAM(lParam);

		if (rect.left > rerect.right) rerect.left = rect.right + (rect.right < rerect.right ? 0 : 1);
		else rerect.left = rect.right - (rect.right < rerect.right ? 1 : 0);
		rerect.top = rect.top;
		InvalidateRect(hwnd, &rerect, TRUE);

		rerect.left = rect.left;
		if (rect.top > rerect.bottom) rerect.top = rect.bottom + (rect.bottom < rerect.bottom ? 0 : 1);
		else rerect.top = rect.bottom - (rect.bottom < rerect.bottom ? 1 : 0);
		InvalidateRect(hwnd, &rerect, TRUE);

		if (rect.left > rerect.right) rerect.left = rect.right + (rect.right < rerect.right ? 0 : 1);
		else rerect.left = rect.right - (rect.right < rerect.right ? 1 : 0);
		if (rect.top > rerect.bottom) rerect.top = rect.bottom + (rect.bottom < rerect.bottom ? 0 : 1);
		else rerect.top = rect.bottom - (rect.bottom < rerect.bottom ? 1 : 0);
		InvalidateRect(hwnd, &rerect, TRUE);

		rect.right = GET_X_LPARAM(lParam);
		rect.bottom = GET_Y_LPARAM(lParam);

		hdc = GetDC(hwnd);
		SelectObject(hdc, GetStockObject(BLACK_PEN));
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
		ReleaseDC(hwnd, hdc);
		return 0;
	case WM_LBUTTONUP:
		ReleaseCapture();
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SelectObject(hdc, GetStockObject(BLACK_PEN));
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		Rectangle(hdc, 50, 50, 100, 100);
		Rectangle(hdc, 75, 75, 120, 120);
		Rectangle(hdc, 100, 100, 200, 200);
		Rectangle(hdc, 30, 30, 190, 190);
		Rectangle(hdc, 50, 10, 150, 105);
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
		EndPaint(hwnd, &ps);
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
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("drawtest");
	if (!RegisterClass(&wc)) return 0;

	if (CreateWindow(wc.lpszClassName, TEXT("DrawTest"), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
	200, 200, 300, 250, NULL, NULL, hInstance, NULL) == NULL) return 0;

	while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);
	return msg.wParam;
}
