#define UNICODE
#include <windows.h>
#include <commctrl.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hsts;
	static int parts[] = {100, 150, -1};

	switch (uMsg) {
	case WM_CREATE:
		hsts = CreateStatusWindow(WS_CHILD | WS_VISIBLE, TEXT("Test text"), hwnd, 1);
		SendMessage(hsts, SB_SETMINHEIGHT, 37, 0);
		SendMessage(hsts, SB_SETPARTS, sizeof(parts) / sizeof(parts[0]), (LPARAM)parts);
		SendMessage(hsts, SB_SETICON, 0, (LPARAM)LoadIcon(NULL, IDI_INFORMATION));
		SendMessage(hsts, SB_SETTEXT, 1, (LPARAM)TEXT("text2"));
		SendMessage(hsts, SB_SETTEXT, 2, (LPARAM)TEXT("just test"));
		return 0;
	case WM_SIZE:
		SendMessage(hsts, WM_SIZE, wParam, lParam);
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
	wc.lpszClassName = TEXT("ststest");
	if (!RegisterClass(&wc)) return 0;

	InitCommonControls();

	if (CreateWindow(
		wc.lpszClassName, TEXT("StatusBar Test"), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		250, 250, 300, 100, NULL, NULL, hInstance, NULL
	) == NULL) return 0;

	while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);
	return msg.wParam;
}
