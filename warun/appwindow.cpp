#pragma once
#include "script.cpp"

// メインウィンドウクラス。
class AppWindow {
public:
	static const BYTE FPS = 60;	// 1秒間に行う描画回数(frame per second)。
	static const WORD WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;	// ウィンドウサイズ。
	static const WORD WIDTH_MIN = 640, HEIGHT_MIN = 480;	// 最小サイズ。
	static const BYTE WINDOW_SIZE_RATIO = 75;	// サイズ比率(100倍)。

	// オブジェクト取得。
	static AppWindow *getObject() {
		static AppWindow aw;
		return &aw;
	}

	// デストラクタ。
	~AppWindow() {
		if (m_pd3dd != NULL) m_pd3dd->Release();
		if (m_pd3d != NULL) m_pd3d->Release();

		timeEndPeriod(1);
	}

	// 作成。
	// 引数：hinst アプリケーション開始時に取得したインスタンスハンドル。
	// 返値：成功ならtrue、それ以外はfalse。
	bool Create(HINSTANCE hinst) {
		if (m_hwnd != NULL) return false;
		if (!createWindow(hinst)) return false;

		// Direct3Dオブジェクト作成。
		m_pd3d = Direct3DCreate9(D3D_SDK_VERSION);
		if (m_pd3d == NULL) return false;

		// デバイス作成。
		if (FAILED(m_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd_draw
			, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_d3dpp, &m_pd3dd))) return false;

		if (!setDraw()) return false;

		m_pspr_mng = SpriteManager::getObject(m_hwnd_draw, m_pd3dd);	// スプライト管理オブジェクト取得。
		m_psnd_mng = SoundManager::getObject();	// 音声管理オブジェクト取得。
		m_pscript = Script::getObject(m_hwnd, m_pspr_mng, m_psnd_mng);	// スクリプトオブジェクト取得。

		if (!m_psnd_mng->Create(m_hwnd)) return false;
		if (!m_pscript->setIntro()) return false;
		ShowWindow(m_hwnd, SW_SHOWNORMAL);
		return true;
	}

	// メッセージループ。
	// 返値：メッセージのwParam。
	int Run() {
		UINT frame = 0, time_before = timeGetTime(), time_frame;
		BOOL msg_flag;
		MSG msg;

		// 描画系とイベント処理。
		for (;;) {
			// 描画処理をする必要がない場合はイベントを受け取るまで待機するようにするが、
			// そうでない場合は、イベントがなければ描画処理をする。
			msg_flag = IsIconic(m_hwnd) ? GetMessage(&msg, NULL, 0, 0) : PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
			if (msg_flag == -1) return 0;	// エラー。
			if (msg.message == WM_QUIT) break;
			if (msg_flag) {
				DispatchMessage(&msg);
				continue;
			}

			// PeekMessageが「イベントなし」を返したので描画処理等をする。

			if (timeGetTime() - time_before >= 1000) {
				frame = 0;
				time_before = timeGetTime();
			}

			m_pscript->Process();

			// fpsが固定になるように描画処理をする。描画処理が指定したfpsより遅い場合は描画処理をしない。
			frame++;
			time_frame = 1000 / (float)FPS * (float)frame;
			if (timeGetTime() - time_before < time_frame) {
				Draw();
				while (timeGetTime() - time_before < time_frame) Sleep(1);
			}
		}

		return msg.wParam;
	}

	// 描画。
	void Draw() {
		if (m_d3d_enable > 0) {
			switch (m_pd3dd->TestCooperativeLevel()) {
			case D3D_OK:
				// 以前のループでD3DDeviceのリセットに成功していたので、描画の再設定が成功すれば描画できるようにする。
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

	// マウスイベントに応じた処理。
	// 引数：type マウスイベントタイプ。
	//       pos_x X位置。
	//       pos_y Y位置。
	void eventMouse(BYTE type, WORD pos_x, WORD pos_y) {
		if (m_d3d_enable == 0) m_pscript->eventMouse(type, pos_x, pos_y, false);
	}

	// キーボードイベントに応じた処理。
	// 引数：key 押下したキー。
	void keyDown(BYTE key) {
		if (m_d3d_enable == 0) m_pscript->onKeyDown(key);
	}

	// 描画領域のサイズを比率が一定になるように変更する。
	// 引数：width ウィンドウの幅。
	//       height ウィンドウの高さ。
	void changeSize(WORD width, WORD height) {
		WORD width_draw, height_draw;	// 表示サイズ。

		// まず、幅を基準にして高さを設定し、その高さがウィンドウの高さより大きい場合は
		// 高さを基準にして幅を設定する。それでも、その幅がウィンドウの幅より大きい場合はありえない。
		height_draw = width * ((float)WINDOW_SIZE_RATIO / 100);
		if (height_draw <= height) {
			width_draw = width;	// 幅を基準にして高さを設定し、その高さがウィンドウの高さ以下だったので幅をウィンドウと同じ幅に設定。
		} else {
			// 高さを基準にして幅を設定する。
			width_draw = height / ((float)WINDOW_SIZE_RATIO / 100);
			height_draw = height;
		}
		// サイズが指定した最小サイズより小さくならないようにする。
		if (width_draw < WIDTH_MIN) width_draw = WIDTH_MIN;
		if (height_draw < HEIGHT_MIN) height_draw = HEIGHT_MIN;

		// 表示位置は縦横ともに中央になるように設定する。ウィンドウサイズが表示サイズ以下の場合は、表示位置は0にする。
		SetWindowPos(
			m_hwnd_draw, NULL
			, width > width_draw ? width / 2 - width_draw / 2 : 0
			, height > height_draw ? height / 2 - height_draw / 2 : 0
			, width_draw, height_draw, SWP_NOZORDER
		);
	}

	// 終了設定。
	void Close() {
		m_pscript->Close(false);
	}

	// ウィンドウプロシージャ。
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
		case WM_MOUSEMOVE:
			paw->eventMouse(3, 0xffff, 0xffff);
			return 0;
		case WM_KEYDOWN:
			paw->keyDown(wParam);
			return 0;
		case WM_KILLFOCUS:
			paw->eventMouse(3, 0xffff, 0xffff);
			return 0;
		case WM_CLOSE:
			if (IsIconic(hwnd)) OpenIcon(hwnd);
			paw->Close();
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	// 描画ウィンドウプロシージャ。
	static LRESULT CALLBACK procDrawWnd(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		AppWindow *paw = (AppWindow *)GetWindowLong(hwnd, GWL_USERDATA);

		switch (uMsg) {
		case WM_CREATE:
			paw = (AppWindow *)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			SetWindowLong(hwnd, GWL_USERDATA, (LONG)paw);
			return 0;
		case WM_LBUTTONDOWN:
			paw->eventMouse(1, LOWORD(lParam), HIWORD(lParam));
			return 0;
		case WM_LBUTTONUP:
			paw->eventMouse(2, LOWORD(lParam), HIWORD(lParam));
			return 0;
		case WM_MOUSEMOVE:
			paw->eventMouse(3, LOWORD(lParam), HIWORD(lParam));
			return 0;
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

protected:
	HWND m_hwnd;	// トップウィンドウ。
	HWND m_hwnd_draw;	// 描画ウィンドウ。

	LPDIRECT3D9 m_pd3d;
	LPDIRECT3DDEVICE9 m_pd3dd;
	D3DPRESENT_PARAMETERS m_d3dpp;
	BYTE m_d3d_enable;	// Direct3D有効状態(0:有効、1:復元していない状態、2:D3DDevice消失状態)。

	SpriteManager *m_pspr_mng;
	SoundManager *m_psnd_mng;
	Script *m_pscript;

	// コンストラクタ。
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
		m_psnd_mng = NULL;
		m_pscript = NULL;

		timeBeginPeriod(1);
		GetModuleFileNameW(NULL, wbuf, sizeof(wbuf) / sizeof(wbuf[0]));
		*wcsrchr(wbuf, L'\\') = L'\0';
		SetCurrentDirectoryW(wbuf);
	}

	// ウィンドウ作成。
	// 引数：hinst アプリケーション開始時に取得したHINSTANCE。
	// 返値：成功ならtrue、それ以外はfalse。
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
		wc.lpszClassName = TEXT("warun1");
		if (RegisterClass(&wc) == 0) return false;
		wc.lpfnWndProc = procDrawWnd;
		wc.lpszClassName = TEXT("warun2");
		if (RegisterClass(&wc) == 0) return false;

		if (SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0) == 0) {
			rect.left = rect.top = 0;
		} else {
			// タスクバーを除く画面のサイズを設定。
			rect.left = rect.right - rect.left;
			rect.top = rect.bottom - rect.top;
		}
		// クライアント領域に合わせてウィンドウサイズを設定。
		rect.right = WINDOW_WIDTH + GetSystemMetrics(SM_CXSIZEFRAME) * 2;
		rect.bottom = WINDOW_HEIGHT + GetSystemMetrics(SM_CYSIZEFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);

		// 位置が画面の中央になるようにウィンドウを作成する。
		m_hwnd = CreateWindow(TEXT("warun1"), TEXT("WaruN"), WS_OVERLAPPEDWINDOW
			, rect.left > rect.right ? rect.left / 2 - rect.right / 2 : CW_USEDEFAULT
			, rect.top > rect.bottom ? rect.top / 2 - rect.bottom / 2 : CW_USEDEFAULT
			, rect.right, rect.bottom, NULL, NULL, hinst, this);
		if (m_hwnd == NULL) return false;

		// 描画領域用のウィンドウ作成。
		m_hwnd_draw = CreateWindow(TEXT("warun2"), NULL, WS_CHILD | WS_VISIBLE, 0, 0
			, WINDOW_WIDTH, WINDOW_HEIGHT, m_hwnd, NULL, hinst, this);
		if (m_hwnd_draw == NULL) return false;

		SendMessage(m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hinst, MAKEINTRESOURCE(1)));
		return true;
	}

	// 描画設定。
	// 返値：成功ならtrue、それ以外はfalse。
	bool setDraw() {
		// 2D形式の頂点情報設定。
		if (FAILED(m_pd3dd->SetFVF(Sprite::D3DFVF))) return false;

		// 頂点情報のアルファ設定。
		if (FAILED(m_pd3dd->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE))) return false;
		if (FAILED(m_pd3dd->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA))) return false;
		if (FAILED(m_pd3dd->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA))) return false;

		// スプライトのアルファ設定。
		if (FAILED(m_pd3dd->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE))) return false;

		return true;
	}
};
