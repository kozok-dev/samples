#define UNICODE
#include <windows.h>

BOOL flag;
HBITMAP hbmp[5];

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	int i;
	HDC hdc, hmdc;

	hdc = GetDC((HWND)lpParameter);
	hmdc = CreateCompatibleDC(hdc);
	for (i = 0; i < 40; i++) {
		SelectObject(hmdc, hbmp[i % 5]);
		BitBlt(hdc, 0, 0, 40, 80, hmdc, 0, 0, SRCCOPY);
		Sleep(17 * 4);
	}
	DeleteDC(hmdc);
	ReleaseDC((HWND)lpParameter, hdc);
	flag = 0;
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		hbmp[0] = (HBITMAP)LoadImage(NULL, TEXT("01.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbmp[1] = (HBITMAP)LoadImage(NULL, TEXT("02.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbmp[2] = (HBITMAP)LoadImage(NULL, TEXT("03.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbmp[3] = (HBITMAP)LoadImage(NULL, TEXT("04.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbmp[4] = (HBITMAP)LoadImage(NULL, TEXT("05.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		return 0;
	case WM_LBUTTONDOWN:
		if (!flag) {
			flag = 1;
			CreateThread(NULL, 0, ThreadProc, hwnd, 0, NULL);
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
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("animtest");
	if (!RegisterClass(&wc)) return 0;

	if (CreateWindow(wc.lpszClassName, TEXT("AnimTest"), WS_SYSMENU | WS_VISIBLE,
	300, 300, 200, 200, NULL, NULL, hInstance, NULL) == NULL) return 0;

	while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);
	return msg.wParam;
}
