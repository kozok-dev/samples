#pragma once
#include "script.cpp"

// ���C���E�B���h�E�N���X�B
class AppWindow {
public:
	static const BYTE FPS = 60;	// 1�b�Ԃɍs���`���(frame per second)�B
	static const WORD WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;	// �E�B���h�E�T�C�Y�B
	static const WORD WIDTH_MIN = 640, HEIGHT_MIN = 480;	// �ŏ��T�C�Y�B
	static const BYTE WINDOW_SIZE_RATIO = 75;	// �T�C�Y�䗦(100�{)�B

	// �I�u�W�F�N�g�擾�B
	static AppWindow *getObject() {
		static AppWindow aw;
		return &aw;
	}

	// �f�X�g���N�^�B
	~AppWindow() {
		if (m_pd3dd != NULL) m_pd3dd->Release();
		if (m_pd3d != NULL) m_pd3d->Release();

		timeEndPeriod(1);
	}

	// �쐬�B
	// �����Fhinst �A�v���P�[�V�����J�n���Ɏ擾�����C���X�^���X�n���h���B
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
	bool Create(HINSTANCE hinst) {
		if (m_hwnd != NULL) return false;
		if (!createWindow(hinst)) return false;

		// Direct3D�I�u�W�F�N�g�쐬�B
		m_pd3d = Direct3DCreate9(D3D_SDK_VERSION);
		if (m_pd3d == NULL) return false;

		// �f�o�C�X�쐬�B
		if (FAILED(m_pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd_draw
			, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_d3dpp, &m_pd3dd))) return false;

		if (!setDraw()) return false;

		m_pspr_mng = SpriteManager::getObject(m_hwnd_draw, m_pd3dd);	// �X�v���C�g�Ǘ��I�u�W�F�N�g�擾�B
		m_psnd_mng = SoundManager::getObject();	// �����Ǘ��I�u�W�F�N�g�擾�B
		m_pscript = Script::getObject(m_hwnd, m_pspr_mng, m_psnd_mng);	// �X�N���v�g�I�u�W�F�N�g�擾�B

		if (!m_psnd_mng->Create(m_hwnd)) return false;
		if (!m_pscript->setIntro()) return false;
		ShowWindow(m_hwnd, SW_SHOWNORMAL);
		return true;
	}

	// ���b�Z�[�W���[�v�B
	// �Ԓl�F���b�Z�[�W��wParam�B
	int Run() {
		UINT frame = 0, time_before = timeGetTime(), time_frame;
		BOOL msg_flag;
		MSG msg;

		// �`��n�ƃC�x���g�����B
		for (;;) {
			// �`�揈��������K�v���Ȃ��ꍇ�̓C�x���g���󂯎��܂őҋ@����悤�ɂ��邪�A
			// �����łȂ��ꍇ�́A�C�x���g���Ȃ���Ε`�揈��������B
			msg_flag = IsIconic(m_hwnd) ? GetMessage(&msg, NULL, 0, 0) : PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
			if (msg_flag == -1) return 0;	// �G���[�B
			if (msg.message == WM_QUIT) break;
			if (msg_flag) {
				DispatchMessage(&msg);
				continue;
			}

			// PeekMessage���u�C�x���g�Ȃ��v��Ԃ����̂ŕ`�揈����������B

			if (timeGetTime() - time_before >= 1000) {
				frame = 0;
				time_before = timeGetTime();
			}

			m_pscript->Process();

			// fps���Œ�ɂȂ�悤�ɕ`�揈��������B�`�揈�����w�肵��fps���x���ꍇ�͕`�揈�������Ȃ��B
			frame++;
			time_frame = 1000 / (float)FPS * (float)frame;
			if (timeGetTime() - time_before < time_frame) {
				Draw();
				while (timeGetTime() - time_before < time_frame) Sleep(1);
			}
		}

		return msg.wParam;
	}

	// �`��B
	void Draw() {
		if (m_d3d_enable > 0) {
			switch (m_pd3dd->TestCooperativeLevel()) {
			case D3D_OK:
				// �ȑO�̃��[�v��D3DDevice�̃��Z�b�g�ɐ������Ă����̂ŁA�`��̍Đݒ肪��������Ε`��ł���悤�ɂ���B
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

	// �}�E�X�C�x���g�ɉ����������B
	// �����Ftype �}�E�X�C�x���g�^�C�v�B
	//       pos_x X�ʒu�B
	//       pos_y Y�ʒu�B
	void eventMouse(BYTE type, WORD pos_x, WORD pos_y) {
		if (m_d3d_enable == 0) m_pscript->eventMouse(type, pos_x, pos_y, false);
	}

	// �L�[�{�[�h�C�x���g�ɉ����������B
	// �����Fkey ���������L�[�B
	void keyDown(BYTE key) {
		if (m_d3d_enable == 0) m_pscript->onKeyDown(key);
	}

	// �`��̈�̃T�C�Y��䗦�����ɂȂ�悤�ɕύX����B
	// �����Fwidth �E�B���h�E�̕��B
	//       height �E�B���h�E�̍����B
	void changeSize(WORD width, WORD height) {
		WORD width_draw, height_draw;	// �\���T�C�Y�B

		// �܂��A������ɂ��č�����ݒ肵�A���̍������E�B���h�E�̍������傫���ꍇ��
		// ��������ɂ��ĕ���ݒ肷��B����ł��A���̕����E�B���h�E�̕����傫���ꍇ�͂��肦�Ȃ��B
		height_draw = width * ((float)WINDOW_SIZE_RATIO / 100);
		if (height_draw <= height) {
			width_draw = width;	// ������ɂ��č�����ݒ肵�A���̍������E�B���h�E�̍����ȉ��������̂ŕ����E�B���h�E�Ɠ������ɐݒ�B
		} else {
			// ��������ɂ��ĕ���ݒ肷��B
			width_draw = height / ((float)WINDOW_SIZE_RATIO / 100);
			height_draw = height;
		}
		// �T�C�Y���w�肵���ŏ��T�C�Y��菬�����Ȃ�Ȃ��悤�ɂ���B
		if (width_draw < WIDTH_MIN) width_draw = WIDTH_MIN;
		if (height_draw < HEIGHT_MIN) height_draw = HEIGHT_MIN;

		// �\���ʒu�͏c���Ƃ��ɒ����ɂȂ�悤�ɐݒ肷��B�E�B���h�E�T�C�Y���\���T�C�Y�ȉ��̏ꍇ�́A�\���ʒu��0�ɂ���B
		SetWindowPos(
			m_hwnd_draw, NULL
			, width > width_draw ? width / 2 - width_draw / 2 : 0
			, height > height_draw ? height / 2 - height_draw / 2 : 0
			, width_draw, height_draw, SWP_NOZORDER
		);
	}

	// �I���ݒ�B
	void Close() {
		m_pscript->Close(false);
	}

	// �E�B���h�E�v���V�[�W���B
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

	// �`��E�B���h�E�v���V�[�W���B
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
	HWND m_hwnd;	// �g�b�v�E�B���h�E�B
	HWND m_hwnd_draw;	// �`��E�B���h�E�B

	LPDIRECT3D9 m_pd3d;
	LPDIRECT3DDEVICE9 m_pd3dd;
	D3DPRESENT_PARAMETERS m_d3dpp;
	BYTE m_d3d_enable;	// Direct3D�L�����(0:�L���A1:�������Ă��Ȃ���ԁA2:D3DDevice�������)�B

	SpriteManager *m_pspr_mng;
	SoundManager *m_psnd_mng;
	Script *m_pscript;

	// �R���X�g���N�^�B
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

	// �E�B���h�E�쐬�B
	// �����Fhinst �A�v���P�[�V�����J�n���Ɏ擾����HINSTANCE�B
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
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
			// �^�X�N�o�[��������ʂ̃T�C�Y��ݒ�B
			rect.left = rect.right - rect.left;
			rect.top = rect.bottom - rect.top;
		}
		// �N���C�A���g�̈�ɍ��킹�ăE�B���h�E�T�C�Y��ݒ�B
		rect.right = WINDOW_WIDTH + GetSystemMetrics(SM_CXSIZEFRAME) * 2;
		rect.bottom = WINDOW_HEIGHT + GetSystemMetrics(SM_CYSIZEFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);

		// �ʒu����ʂ̒����ɂȂ�悤�ɃE�B���h�E���쐬����B
		m_hwnd = CreateWindow(TEXT("warun1"), TEXT("WaruN"), WS_OVERLAPPEDWINDOW
			, rect.left > rect.right ? rect.left / 2 - rect.right / 2 : CW_USEDEFAULT
			, rect.top > rect.bottom ? rect.top / 2 - rect.bottom / 2 : CW_USEDEFAULT
			, rect.right, rect.bottom, NULL, NULL, hinst, this);
		if (m_hwnd == NULL) return false;

		// �`��̈�p�̃E�B���h�E�쐬�B
		m_hwnd_draw = CreateWindow(TEXT("warun2"), NULL, WS_CHILD | WS_VISIBLE, 0, 0
			, WINDOW_WIDTH, WINDOW_HEIGHT, m_hwnd, NULL, hinst, this);
		if (m_hwnd_draw == NULL) return false;

		SendMessage(m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hinst, MAKEINTRESOURCE(1)));
		return true;
	}

	// �`��ݒ�B
	// �Ԓl�F�����Ȃ�true�A����ȊO��false�B
	bool setDraw() {
		// 2D�`���̒��_���ݒ�B
		if (FAILED(m_pd3dd->SetFVF(Sprite::D3DFVF))) return false;

		// ���_���̃A���t�@�ݒ�B
		if (FAILED(m_pd3dd->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE))) return false;
		if (FAILED(m_pd3dd->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA))) return false;
		if (FAILED(m_pd3dd->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA))) return false;

		// �X�v���C�g�̃A���t�@�ݒ�B
		if (FAILED(m_pd3dd->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE))) return false;

		return true;
	}
};
