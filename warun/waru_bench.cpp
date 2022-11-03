#include "spritemanager.cpp"

class AppWindow {
public:
	static const BYTE FPS = 60;
	static const WORD WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
	static const WORD WIDTH_MIN = 640, HEIGHT_MIN = 480;
	static const BYTE WINDOW_SIZE_RATIO = 75;

	static AppWindow *getObject() {
		static AppWindow aw;
		return &aw;
	}

	~AppWindow() {
		if (m_pd3dd != NULL) m_pd3dd->Release();
		if (m_pd3d != NULL) m_pd3d->Release();
		if (m_ppackfile != NULL) fclose(m_ppackfile);

		timeEndPeriod(1);
	}

	bool Create(HINSTANCE hinst) {
		if (m_hwnd != NULL) return false;
#if PACKFILE_TYPE < 2
		if (m_ppackfile == NULL) return false;
#endif
		if (!createWindow(hinst)) return false;

		m_pd3d = Direct3DCreate9(D3D_SDK_VERSION);
		if (m_pd3d == NULL) return false;

		if (FAILED(m_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd_draw
			, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_d3dpp, &m_pd3dd))) return false;

		if (!setDraw()) return false;

		m_pspr_mng = SpriteManager::getObject(m_hwnd, m_pd3dd);

		m_pspr_mng->createSpriteImg(0, "img/bg_01.bmp");
		m_pspr_mng->createSpriteColor(1, D3DCOLOR_ARGB(0xee, 0xff, 0xff, 0xff));
		m_pspr_mng->createSpriteString(2, 10, 530, 300, 60, 30, D3DCOLOR_XRGB(0x00, 0x00, 0x00));
		m_pspr_mng->createSpriteString(3, 10, 10, 200, 30, 30, D3DCOLOR_XRGB(0x00, 0x00, 0x00));
		m_pspr_mng->createSpriteString(4, 10, 50, 300, 20, 20, D3DCOLOR_XRGB(0x00, 0x00, 0x00));
		m_pspr_mng->createSpriteString(5, 10, 80, 330, 20, 20, D3DCOLOR_XRGB(0x00, 0x00, 0x00));
		m_pspr_mng->createSpriteString(6, 10, 110, 330, 20, 20, D3DCOLOR_XRGB(0x00, 0x00, 0x00));

		m_pspr_mng->writeString(2, L"WaruN bench mark\nCopyright 2009, kozok");
		m_pspr_mng->writeString(3, L"? fps");
		m_pspr_mng->writeString(4, L"bg read time = ? msec");
		m_pspr_mng->writeString(5, L"script seq-scan time = ? msec");
		m_pspr_mng->writeString(6, L"draw time per frame = ? msec");

		m_pspr_mng->addFadeInAction(0, 0);
		m_pspr_mng->addFadeInAction(1, 0);
		m_pspr_mng->addFadeInAction(2, 0);
		m_pspr_mng->addFadeInAction(3, 0);
		m_pspr_mng->addFadeInAction(4, 0);
		m_pspr_mng->addFadeInAction(5, 0);
		m_pspr_mng->addFadeInAction(6, 0);

		ShowWindow(m_hwnd, SW_SHOWNORMAL);
		return true;
	}

	int Run() {
		char buf[1024];
		UINT frame = 0, sum1 = 0, cnt1 = 0, sum2 = 0, cnt2 = 0, time_before = timeGetTime(), time_frame;
		BOOL msg_flag;
		WCHAR wbuf[256];
		MSG msg;
		LARGE_INTEGER freq, before, after;
		PackFile pf(1024);

		QueryPerformanceFrequency(&freq);

		for (;;) {
			msg_flag = IsIconic(m_hwnd) ? GetMessage(&msg, NULL, 0, 0) : PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
			if (msg_flag == -1) return 0;
			if (msg.message == WM_QUIT) break;
			if (msg_flag) {
				DispatchMessage(&msg);
				continue;
			}

			if (timeGetTime() - time_before >= 1000) {
				swprintf(wbuf, L"%.2f fps", (float)frame * 1000 / (float)(timeGetTime() - time_before));
				m_pspr_mng->writeString(3, wbuf);
				m_pspr_mng->addFadeInAction(3, 0);

				swprintf(wbuf, L"bg read time = %f msec", (double)sum1 * 1000 / (double)freq.QuadPart / (double)cnt1);
				m_pspr_mng->writeString(4, wbuf);
				m_pspr_mng->addFadeInAction(4, 0);

				QueryPerformanceFrequency(&freq);
				QueryPerformanceCounter(&before);
				if (pf.Open("scr/waru.txt")) {
					while (pf.readLine(buf, 1024));
				}
				QueryPerformanceCounter(&after);
				swprintf(wbuf, L"script seq-scan time = %f msec"
					, (double)(after.QuadPart - before.QuadPart) * 1000 / (double)freq.QuadPart);
				m_pspr_mng->writeString(5, wbuf);
				m_pspr_mng->addFadeInAction(5, 0);

				swprintf(wbuf, L"draw time per frame = %f msec", (double)sum2 * 1000 / (double)freq.QuadPart / (double)cnt2);
				m_pspr_mng->writeString(6, wbuf);
				m_pspr_mng->addFadeInAction(6, 0);

				m_pspr_mng->createSpriteImg(7, "img/bg_01.bmp");
				m_pspr_mng->createSpriteImg(8, "img/bg_01.bmp");

				frame = sum1 = cnt1 = sum2 = cnt2 = 0;
				time_before = timeGetTime();
			}

			if (frame < 5) {
				QueryPerformanceCounter(&before);
				m_pspr_mng->createSpriteImg(9, "img/bg_01.bmp");
				QueryPerformanceCounter(&after);
				sum1 += after.QuadPart - before.QuadPart;
				cnt1++;
			}

			if (!m_pspr_mng->Process()) {
				MessageBox(m_hwnd, TEXT("エラーが発生しました。"), TEXT("warun"), MB_OK | MB_ICONEXCLAMATION);
				DestroyWindow(m_hwnd);
				continue;
			}

			frame++;
			time_frame = 1000 / (float)FPS * (float)frame;
			if (frame == 1) {
				QueryPerformanceCounter(&before);
				Draw();
				QueryPerformanceCounter(&after);
				sum2 += after.QuadPart - before.QuadPart;
				cnt2++;
			} else {
				Draw();
			}
			while (timeGetTime() - time_before < time_frame) Sleep(1);
		}

		return msg.wParam;
	}

	void changeSize(WORD width, WORD height) {
		WORD width_draw, height_draw;

		height_draw = width * ((float)WINDOW_SIZE_RATIO / 100);
		if (height_draw <= height) {
			width_draw = width;
		} else {
			width_draw = height / ((float)WINDOW_SIZE_RATIO / 100);
			height_draw = height;
		}
		if (width_draw < WIDTH_MIN) width_draw = WIDTH_MIN;
		if (height_draw < HEIGHT_MIN) height_draw = HEIGHT_MIN;

		SetWindowPos(
			m_hwnd_draw, NULL
			, width > width_draw ? width / 2 - width_draw / 2 : 0
			, height > height_draw ? height / 2 - height_draw / 2 : 0
			, width_draw, height_draw, SWP_NOZORDER
		);
	}

	void Draw() {
		if (m_d3d_enable > 0) {
			switch (m_pd3dd->TestCooperativeLevel()) {
			case D3D_OK:
				if (setDraw()) m_d3d_enable = 0;
				break;
			case D3DERR_DEVICENOTRESET:
				if (SUCCEEDED(m_pd3dd->Reset(&m_d3dpp))) m_d3d_enable = 1;
				return;
			default:
				return;
			}
		}

		m_pd3dd->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 1, 0);
		if (SUCCEEDED(m_pd3dd->BeginScene())) {
			m_pspr_mng->Draw();
			m_pd3dd->EndScene();
		}
		if (m_pd3dd->Present(NULL, NULL, NULL, NULL) == D3DERR_DEVICELOST) m_d3d_enable = 2;
	}

	static LRESULT CALLBACK procWnd(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		POINT point;
		AppWindow *paw = (AppWindow *)GetWindowLong(hwnd, GWL_USERDATA);

		switch (uMsg) {
		case WM_CREATE:
			paw = (AppWindow *)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			SetWindowLong(hwnd, GWL_USERDATA, (LONG)paw);
			return 0;
		case WM_PAINT:
			paw->Draw();
			break;
		case WM_SIZE:
			paw->changeSize(LOWORD(lParam), HIWORD(lParam));
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

protected:
	HWND m_hwnd, m_hwnd_draw;

	LPDIRECT3D9 m_pd3d;
	LPDIRECT3DDEVICE9 m_pd3dd;
	D3DPRESENT_PARAMETERS m_d3dpp;
	BYTE m_d3d_enable;	// Direct3D有効状態(0:有効、1:復元していない状態、2:D3DDevice消失状態)。

	SpriteManager *m_pspr_mng;
	FILE *m_ppackfile;

	AppWindow() {
		WCHAR wbuf[MAX_PATH];

		m_hwnd = m_hwnd_draw = NULL;

		m_pd3d = NULL;
		m_pd3dd = NULL;
		m_d3dpp.BackBufferWidth = 0;
		m_d3dpp.BackBufferHeight = 0;
		m_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		m_d3dpp.BackBufferCount = 1;
		m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
		m_d3dpp.MultiSampleQuality = 0;
		m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_d3dpp.hDeviceWindow = NULL;
		m_d3dpp.Windowed = TRUE;
		m_d3dpp.EnableAutoDepthStencil = FALSE;
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
		m_d3dpp.Flags = 0;
		m_d3dpp.FullScreen_RefreshRateInHz = 0;
		m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		m_d3d_enable = 0;

		m_pspr_mng = NULL;

		timeBeginPeriod(1);
		GetModuleFileNameW(NULL, wbuf, sizeof(wbuf) / sizeof(wbuf[0]));
		*wcsrchr(wbuf, L'\\') = L'\0';
		SetCurrentDirectoryW(wbuf);

		m_ppackfile = fopen(PACKFILE, "rb+");
	}

	bool createWindow(HINSTANCE hinst) {
		RECT rect;
		WNDCLASS wc;

		wc.style = 0;
		wc.lpfnWndProc = procWnd;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hinst;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = TEXT("waru_bench1");
		if (RegisterClass(&wc) == 0) return false;
		wc.lpfnWndProc = DefWindowProc;
		wc.lpszClassName = TEXT("waru_bench2");
		if (RegisterClass(&wc) == 0) return false;

		if (SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0) == 0) {
			rect.left = rect.top = 0;
		} else {
			rect.left = rect.right - rect.left;
			rect.top = rect.bottom - rect.top;
		}
		rect.right = WINDOW_WIDTH + GetSystemMetrics(SM_CXSIZEFRAME) * 2;
		rect.bottom = WINDOW_HEIGHT + GetSystemMetrics(SM_CYSIZEFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);

		m_hwnd = CreateWindow(TEXT("waru_bench1"), TEXT("WaruN bench mark"), WS_OVERLAPPEDWINDOW
			, rect.left > rect.right ? rect.left / 2 - rect.right / 2 : CW_USEDEFAULT
			, rect.top > rect.bottom ? rect.top / 2 - rect.bottom / 2 : CW_USEDEFAULT
			, rect.right, rect.bottom, NULL, NULL, hinst, this);
		if (m_hwnd == NULL) return false;

		m_hwnd_draw = CreateWindow(TEXT("waru_bench2"), NULL, WS_CHILD | WS_VISIBLE, 0, 0
			, WINDOW_WIDTH, WINDOW_HEIGHT, m_hwnd, NULL, hinst, this);
		if (m_hwnd_draw == NULL) return false;

		SendMessage(m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hinst, MAKEINTRESOURCE(1)));
		return true;
	}

	bool setDraw() {
		if (FAILED(m_pd3dd->SetFVF(Sprite::D3DFVF))) return false;

		if (FAILED(m_pd3dd->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE))) return false;
		if (FAILED(m_pd3dd->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA))) return false;
		if (FAILED(m_pd3dd->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA))) return false;

		if (FAILED(m_pd3dd->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE))) return false;

		return true;
	}
};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int ret;
	HANDLE hmutex;
	AppWindow *paw;

	hmutex = CreateMutex(NULL, FALSE, TEXT("waru_bench"));
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(hmutex);
		MessageBox(NULL, TEXT("既に起動しています。"), TEXT("WaruN"), MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	paw = AppWindow::getObject();
	if (!paw->Create(hInstance)) {
		CloseHandle(hmutex);
		MessageBox(NULL, TEXT("起動に失敗しました。"), TEXT("WaruN"), MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	ret = paw->Run();
	CloseHandle(hmutex);
	return ret;
}
