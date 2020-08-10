#define DIRECTINPUT_VERSION 0x800
#include <d3dx9.h>
#include <dinput.h>
#include <dsound.h>
#include <mmsystem.h>
#define RELEASE(obj) if (obj) { \
	obj->Release(); \
	obj = NULL; \
}

// DirectGraphics
class DG {
	LPDIRECT3D9 lpd3d;
	LPDIRECT3DDEVICE9 lpd3ddev;
	LPDIRECT3DVERTEXBUFFER9 lpd3dvb[83];
	LPDIRECT3DTEXTURE9 lpd3dtexture1, lpd3dtexture2;
	struct VERTEX {
		float x, y, z;
		float nx, ny, nz;
		float tu, tv;
	};

	// 頂点バッファ設定
	void setVB() {
		VERTEX v[83][6];
		int i;
		void *lpv;
		static DWORD customvertex = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

		for (i = 0; i < 83; i++) {
			switch (i) {
			case 81:
				v[i][0].x = -50.0; v[i][0].y = -0.51; v[i][0].z = -50.0; v[i][0].nx =  0.0; v[i][0].ny = 1.0; v[i][0].nz = 0.0; v[i][0].tu = 0.0; v[i][0].tv = 1.0;
				v[i][1].x = -50.0; v[i][1].y = -0.51; v[i][1].z =  50.0; v[i][1].nx =  0.0; v[i][1].ny = 1.0; v[i][1].nz = 0.0; v[i][1].tu = 0.0; v[i][1].tv = 0.0;
				v[i][2].x =  50.0; v[i][2].y = -0.51; v[i][2].z = -50.0; v[i][2].nx =  0.0; v[i][2].ny = 1.0; v[i][2].nz = 0.0; v[i][2].tu = 1.0; v[i][2].tv = 1.0;
				v[i][3].x =  50.0; v[i][3].y = -0.51; v[i][3].z =  50.0; v[i][3].nx =  0.0; v[i][3].ny = 1.0; v[i][3].nz = 0.0; v[i][3].tu = 1.0; v[i][3].tv = 0.0;
				break;
			case 82:
				v[i][0].x = -50.0; v[i][0].y = -30.0; v[i][0].z = 49.0; v[i][0].nx =  0.0; v[i][0].ny = 0.0; v[i][0].nz = -1.0; v[i][0].tu = 0.0; v[i][0].tv = 1.0;
				v[i][1].x = -50.0; v[i][1].y =  30.0; v[i][1].z = 49.0; v[i][1].nx =  0.0; v[i][1].ny = 0.0; v[i][1].nz = -1.0; v[i][1].tu = 0.0; v[i][1].tv = 0.0;
				v[i][2].x =  50.0; v[i][2].y = -30.0; v[i][2].z = 49.0; v[i][2].nx =  0.0; v[i][2].ny = 0.0; v[i][2].nz = -1.0; v[i][2].tu = 1.0; v[i][2].tv = 1.0;
				v[i][3].x =  50.0; v[i][3].y =  30.0; v[i][3].z = 49.0; v[i][3].nx =  0.0; v[i][3].ny = 0.0; v[i][3].nz = -1.0; v[i][3].tu = 1.0; v[i][3].tv = 0.0;
				break;
			default:
				v[i][0].x = -0.5 + i % 9 * 3 - 12; v[i][0].y = -0.5; v[i][0].z = -0.5 + i / 9 * 3 - 12; v[i][0].nx = -0.5; v[i][0].ny = -0.5; v[i][0].nz = -0.5; v[i][0].tu = 0.0; v[i][0].tv = 1.0;
				v[i][1].x =  0.0 + i % 9 * 3 - 12; v[i][1].y =  0.5; v[i][1].z =  0.0 + i / 9 * 3 - 12; v[i][1].nx =  0.0; v[i][1].ny =  0.5; v[i][1].nz =  0.0; v[i][1].tu = 0.5; v[i][1].tv = 0.0;
				v[i][2].x =  0.5 + i % 9 * 3 - 12; v[i][2].y = -0.5; v[i][2].z = -0.5 + i / 9 * 3 - 12; v[i][2].nx =  0.5; v[i][2].ny = -0.5; v[i][2].nz = -0.5; v[i][2].tu = 0.0; v[i][2].tv = 1.0;
				v[i][3].x =  0.0 + i % 9 * 3 - 12; v[i][3].y = -0.5; v[i][3].z =  0.5 + i / 9 * 3 - 12; v[i][3].nx =  0.0; v[i][3].ny = -0.5; v[i][3].nz =  0.5; v[i][3].tu = 1.0; v[i][3].tv = 1.0;
				v[i][4].x = -0.5 + i % 9 * 3 - 12; v[i][4].y = -0.5; v[i][4].z = -0.5 + i / 9 * 3 - 12; v[i][4].nx = -0.5; v[i][4].ny = -0.5; v[i][4].nz = -0.5; v[i][4].tu = 0.0; v[i][4].tv = 1.0;
				v[i][5].x =  0.0 + i % 9 * 3 - 12; v[i][5].y =  0.5; v[i][5].z =  0.0 + i / 9 * 3 - 12; v[i][5].nx =  0.0; v[i][5].ny =  0.5; v[i][5].nz =  0.0; v[i][5].tu = 0.5; v[i][5].tv = 0.0;
				break;
			}

			// 頂点バッファ作成
			if (FAILED(lpd3ddev->CreateVertexBuffer(
				sizeof(VERTEX) * 6, 0,
				customvertex, D3DPOOL_DEFAULT,
				&lpd3dvb[i], NULL))
			) return;
			if (FAILED(lpd3dvb[i]->Lock(0, sizeof(v[i]), &lpv, 0))) return;
			CopyMemory(lpv, v[i], sizeof(v[i]));
			lpd3dvb[i]->Unlock();
		}

		lpd3ddev->SetFVF(customvertex);
	}

public:
	float vx, vy, wx, wy;

	~DG() {
		int i;

		RELEASE(lpd3dtexture1);
		RELEASE(lpd3dtexture2);
		for (i = 0; i < 83; i++) {
			RELEASE(lpd3dvb[i]);
		}
		RELEASE(lpd3ddev);
		RELEASE(lpd3d);
	}

	// 初期化
	void Init(HWND hwnd) {
		D3DCAPS9 d3dcaps;
		D3DPRESENT_PARAMETERS d3dpp;
		D3DMATERIAL9 material;
		D3DLIGHT9 light;
		D3DXMATRIX proj;
		int i;

		lpd3d = NULL;
		lpd3ddev = NULL;
		for (i = 0; i < 83; i++) lpd3dvb[i] = NULL;
		lpd3dtexture1 = lpd3dtexture2 = NULL;

		vx = 0.0;
		vy = -20.0;
		wx = 0.0;
		wy = 0.0;

		// オブジェクト作成
		if (!(lpd3d = Direct3DCreate9(D3D_SDK_VERSION))) return;

		// デバイス作成
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.Windowed = true;
		d3dpp.EnableAutoDepthStencil = true;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		if (FAILED(lpd3d->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &lpd3ddev
		))) return;

		// レンダリング設定
		lpd3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	// カリング
		lpd3ddev->SetRenderState(D3DRS_LIGHTING, true);	// ライト
		lpd3ddev->SetRenderState(D3DRS_SPECULARENABLE, true);	// スペキュラ

		// マテリアル設定
		ZeroMemory(&material, sizeof(D3DMATERIAL9));
		material.Diffuse.r = material.Diffuse.g = material.Diffuse.b = 1.0;
		material.Ambient.r = material.Ambient.g = material.Ambient.b = 0.3;
		material.Specular.r = material.Specular.g = material.Specular.b = 1.0;
		material.Power = 10.0;
		lpd3ddev->SetMaterial(&material);

		// ライト設定
		ZeroMemory(&light, sizeof(D3DLIGHT9));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse.r = light.Diffuse.g = light.Diffuse.b = light.Diffuse.a = 1.0;
		light.Ambient.r = light.Ambient.g = light.Ambient.b = light.Ambient.a = 1.0;
		light.Specular.r = light.Specular.g = light.Specular.b = light.Specular.a = 1.0;
		D3DXVec3Normalize((D3DXVECTOR3 *)&light.Position, &D3DXVECTOR3(0.0, 0.0, 0.0));
		D3DXVec3Normalize((D3DXVECTOR3 *)&light.Direction, &D3DXVECTOR3(0.0, -2.0, 1.0));
		light.Range = 100.0;
		light.Falloff = 1.0;
		light.Attenuation1 = 0.5;
		light.Theta = D3DXToRadian(90.0);
		light.Phi = D3DXToRadian(180.0);
		lpd3ddev->SetLight(0, &light);
		lpd3ddev->LightEnable(0, true);

		// フォグ
		/*
		float a = 0.7, b = 1.0, c = 0.2;
		lpd3ddev->SetRenderState(D3DRS_FOGENABLE, true);
		lpd3ddev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
		lpd3ddev->SetRenderState(D3DRS_FOGCOLOR, 0xeeeeff);
		lpd3ddev->SetRenderState(D3DRS_FOGSTART, *((DWORD*)&a));
		lpd3ddev->SetRenderState(D3DRS_FOGEND, *((DWORD*)&b));
		lpd3ddev->SetRenderState(D3DRS_FOGDENSITY, *((DWORD*)&c));
		*/

		// テクスチャ取得
		D3DXCreateTextureFromFile(lpd3ddev, TEXT("tile.jpg"), &lpd3dtexture1);
		D3DXCreateTextureFromFile(lpd3ddev, TEXT("sky.jpg"), &lpd3dtexture2);

		// 射影変換
		D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4, 1.0, 1.0, 100.0);
		lpd3ddev->SetTransform(D3DTS_PROJECTION, &proj);

		// 頂点設定
		setVB();
	}

	// レンダリング
	void Render() {
		D3DXMATRIX world, worldi, view;
		RECT rect;
		int i;

		if (!lpd3ddev) return;

		lpd3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(212, 208, 200), 1.0, 0);	// バッファをクリア
		if (SUCCEEDED(lpd3ddev->BeginScene())) {	// レンダリング開始
			// ワールド変換
			D3DXMatrixRotationYawPitchRoll(&world, wx, wy, 0.0);
			lpd3ddev->SetTransform(D3DTS_WORLD, &world);

			// ビュー変換
			D3DXMatrixLookAtLH(&view,
				&D3DXVECTOR3(vx, 0.0, vy),	// 位置
				&D3DXVECTOR3(vx, 0.0, vy + 10.0),	// 方向
				&D3DXVECTOR3(0.0, 1.0, 0.0)	// 上方
			);
			lpd3ddev->SetTransform(D3DTS_VIEW, &view);

			// 描画
			for (i = 0; i < 83; i++) {
				if (i == 0) {
					lpd3ddev->SetTexture(0, lpd3dtexture1);
				} else if (i == 82) {	// ビルボード用
					// ワールド変換無効
					D3DXMatrixIdentity(&worldi);
					worldi._11 = world._11; worldi._12 = world._21; worldi._13 = world._31;
					worldi._21 = world._12; worldi._22 = world._22; worldi._23 = world._32;
					worldi._31 = world._13; worldi._32 = world._23; worldi._33 = world._33;
					D3DXMatrixMultiply(&world, &world, &worldi);
					lpd3ddev->SetTransform(D3DTS_WORLD, &world);

					// ビュー変換無効
					D3DXMatrixIdentity(&view);
					lpd3ddev->SetTransform(D3DTS_VIEW, &view);

					lpd3ddev->SetTexture(0, lpd3dtexture2);
				}
				lpd3ddev->SetStreamSource(0, lpd3dvb[i], 0, sizeof(VERTEX));
				lpd3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, i < 81 ? 4 : 2);
			}

			lpd3ddev->EndScene();	// レンダリング終了
		}
		lpd3ddev->Present(NULL, NULL, NULL, NULL);	// バッファを表示
	}
};

// DirectSound
class DS {
	LPDIRECTSOUND8 lpds;
	LPDIRECTSOUNDBUFFER8 lpdsb;

	// 設定
	bool Set(HWND hwnd) {
		if (FAILED(DirectSoundCreate8(NULL, &lpds, NULL))) return false;	// オブジェクト作成
		if (FAILED(lpds->SetCooperativeLevel(hwnd, DSSCL_PRIORITY))) return false;	// 協調レベル設定
		return Read(TEXT("piko.wav"));
	}

	// 開放
	void Free() {
		RELEASE(lpdsb);
		RELEASE(lpds);
	}

public:
	~DS() {
		Free();
	}

	// 初期化
	void Init(HWND hwnd) {
		lpds = NULL;
		lpdsb = NULL;
		if (!Set(hwnd)) Free();
	}

	// 読み込み
	bool Read(TCHAR *fn) {
		LPDIRECTSOUNDBUFFER lpdsb;
		WAVEFORMATEX wf;
		DSBUFFERDESC dsbd;
		HRESULT hr;
		HMMIO f;
		MMCKINFO per, chi;
		char *buf;
		void *dsb;
		HANDLE hheap;
		int size;
		DWORD readsize;
		bool succflg = false;

		if (!this->lpds || (f = mmioOpen(fn, NULL, MMIO_READ)) == NULL) return false;

		per.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		if (mmioDescend(f, &per, NULL, MMIO_FINDRIFF) == MMSYSERR_NOERROR) {
			chi.ckid = mmioFOURCC('f', 'm', 't', ' ');
			if (mmioDescend(f, &chi, &per, MMIO_FINDCHUNK) == MMSYSERR_NOERROR) {
				wf.cbSize = 0;
				size = sizeof(WAVEFORMATEX) < chi.cksize ? sizeof(WAVEFORMATEX) : chi.cksize;
				if (mmioRead(f, (HPSTR)&wf, size) == size) {	// Waveフォーマット取得
					mmioAscend(f, &chi, 0);
					chi.ckid = mmioFOURCC('d', 'a', 't', 'a');
					if (mmioDescend(f, &chi, &per, MMIO_FINDCHUNK) == MMSYSERR_NOERROR) {
						hheap = GetProcessHeap();
						if ((buf = (char *)HeapAlloc(hheap, 0, chi.cksize)) != NULL) {
							if (mmioRead(f, (HPSTR)buf, chi.cksize) == chi.cksize) {
								// バッファ設定
								dsbd.dwSize          = sizeof(DSBUFFERDESC);
								dsbd.dwFlags         = 0;
								dsbd.dwReserved      = 0;
								dsbd.dwBufferBytes   = chi.cksize;
								dsbd.lpwfxFormat     = &wf;
								dsbd.guid3DAlgorithm = DS3DALG_DEFAULT;

								// バッファ作成
								if (SUCCEEDED(lpds->CreateSoundBuffer(&dsbd, &lpdsb, NULL))) {
									hr = lpdsb->QueryInterface(IID_IDirectSoundBuffer8, (void **)&this->lpdsb);
									lpdsb->Release();
									if (SUCCEEDED(hr)) {
										// バッファに書く
										if (SUCCEEDED(this->lpdsb->Lock(0, size, &dsb, &readsize, NULL, 0, DSBLOCK_ENTIREBUFFER))) {
											CopyMemory(dsb, buf, readsize);
											this->lpdsb->Unlock(dsb, readsize, NULL, 0);
											succflg = true;
										}
									}
								}
							}
							HeapFree(hheap, 0, buf);
						}
					}
				}
			}
		}
		mmioClose(f, 0);
		return succflg;
	}

	// 再生
	void Play() {
		if (!lpdsb) return;
		lpdsb->SetCurrentPosition(0);
		lpdsb->Play(0, 0, 0);
	}
};

// DirectInput
class DI {
	LPDIRECTINPUT8 lpdi;
	LPDIRECTINPUTDEVICE8 lpdidev;

	// 設定
	bool Set(HINSTANCE hinstance, HWND hwnd) {
		if (FAILED(DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&lpdi, NULL))) return false;	// オブジェクト作成
		if (FAILED(lpdi->CreateDevice(GUID_SysKeyboard, &lpdidev, NULL))) return false;	// デバイス作成
		if (FAILED(lpdidev->SetDataFormat(&c_dfDIKeyboard))) return false;	// データ形式設定
		if (FAILED(lpdidev->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) return false;	// 動作設定
		if (FAILED(lpdidev->Acquire())) return false;	// アクセス権取得
		return true;
	}

	// 開放
	void Free() {
		if (lpdidev) {
			lpdidev->Unacquire();
			lpdidev->Release();
			lpdidev = NULL;
		}
		RELEASE(lpdi);
	}

public:
	~DI() {
		Free();
	}

	// 初期化
	void Init(HINSTANCE hinstance, HWND hwnd) {
		lpdi = NULL;
		lpdidev = NULL;
		if (!Set(hinstance, hwnd)) Free();
	}

	// データ取得
	void Get(DG &dg, DS &ds) {
		BYTE buf[256];
		float x, y, os;

		if (!lpdidev) return;

		if (FAILED(lpdidev->GetDeviceState(sizeof(buf), (void *)buf))) {
			while (lpdidev->Acquire() == DIERR_INPUTLOST);	// アクセス権を取得できるまで繰り返す
			return;
		}

		if (buf[DIK_SPACE] & 0x80) ds.Play();

		x = (buf[DIK_LEFT] & 0x80) ? -1.0 : (buf[DIK_RIGHT] & 0x80) ? 1.0 : 0.0;
		y = (buf[DIK_UP] & 0x80) ? 1.0 : (buf[DIK_DOWN] & 0x80) ? -1.0 : 0.0;
		if (!x && !y) return;
		os = (buf[DIK_LCONTROL] & 0x80) ? 10.0 : 100.0;
		if (buf[DIK_LSHIFT] & 0x80) {
			dg.wx += x / os;
			dg.wy += y / os;
		} else {
			dg.vx += x / os;
			dg.vy += y / os;
		}
	}
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hwnd;
	WNDCLASS wc;
	MSG msg;
	HANDLE hmutex;
	DG dg;
	DI di;
	DS ds;

	hmutex = CreateMutex(NULL, false, TEXT("dxtest"));
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		hwnd = FindWindow(TEXT("dxtest"), TEXT("DirectX-Test"));
		SetForegroundWindow(hwnd);
		ShowWindow(hwnd, SW_SHOWNORMAL);
		CloseHandle(hmutex);
		return 0;
	}

	wc.style         = 0;
	wc.lpfnWndProc   = WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = TEXT("dxtest");
	if (!RegisterClass(&wc)) return 0;

	if (!(hwnd = CreateWindow(
		TEXT("dxtest"), TEXT("DirectX-Test"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		100, 100, 640, 480,
		NULL, NULL, hInstance, NULL
	))) return 0;

	// DirectX初期化
	dg.Init(hwnd);
	di.Init(hInstance, hwnd);
	ds.Init(hwnd);

	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			DispatchMessage(&msg);
		} else {
			dg.Render();
			di.Get(dg, ds);
		}
	}

	CloseHandle(hmutex);
	return msg.wParam;
}
