#include "sgv.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	void **ppdata;
	static SimpleGridView *sgw;

	switch (uMsg) {
		case WM_CREATE:
		if ((sgw = new SimpleGridView()) == NULL ||
			!sgw->addItem(TEXT("test項目"), SimpleGridView::ITEMTYPE_DECIMAL, TEXT("f"), 70) ||
			!sgw->addItem(TEXT("Item"), SimpleGridView::ITEMTYPE_INTEGER, TEXT("d"), 50) ||
			!sgw->addItem(TEXT("項目テスト"), SimpleGridView::ITEMTYPE_STRING, TEXT("s"), 100)) return 0;

		if ((ppdata = sgw->addData()) == NULL) return 0;
		ppdata[0] = SimpleGridView::floatValue(2.554352);
		ppdata[1] = (void *)243;
		ppdata[2] = SimpleGridView::stringValue(TEXT("ABCCDF"));
		if ((ppdata = sgw->addData()) == NULL) return 0;
		ppdata[0] = SimpleGridView::floatValue(0.164);
		ppdata[1] = (void *)80;
		ppdata[2] = SimpleGridView::stringValue(TEXT("テストテスト"));
		if ((ppdata = sgw->addData()) == NULL) return 0;
		ppdata[0] = SimpleGridView::floatValue(0.0000164);
		ppdata[1] = (void *)132;
		ppdata[2] = SimpleGridView::stringValue(TEXT("0100120101"));
		if ((ppdata = sgw->addData()) == NULL) return 0;
		ppdata[0] = SimpleGridView::floatValue(5);
		ppdata[1] = (void *)6;
		ppdata[2] = SimpleGridView::stringValue(TEXT("glgllg"));
		if ((ppdata = sgw->addData()) == NULL) return 0;
		ppdata[0] = SimpleGridView::floatValue(13.5);
		ppdata[1] = (void *)145;
		ppdata[2] = SimpleGridView::stringValue(TEXT("5:1"));

		if (!sgw->Create(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, 10, 10, 0, 0)) return 0;
		return 0;
	case WM_SIZE:
		sgw->setWndSize(LOWORD(lParam) - 20, HIWORD(lParam) - 20);
		return 0;
	case WM_DESTROY:
		delete sgw;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;
	HWND hwnd;
	MSG msg;

	wc.style = 0;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_MENU + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("test");
	if (!RegisterClass(&wc)) return 0;

	if ((hwnd = CreateWindow(wc.lpszClassName, TEXT("SimpleGridView Test"), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		200, 200, 375, 395, NULL, NULL, hInstance, NULL)) == NULL) return 0;

	while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);
	return msg.wParam;
}
