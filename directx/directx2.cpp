// DirectX Graphics sprite sample

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <d3d9.h>
#include <d3dx9.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
	LPDIRECT3D9 lpd3d;
	LPDIRECT3DDEVICE9 lpd3ddev;
	LPDIRECT3DTEXTURE9 lpd3dtexture1, lpd3dtexture2;
	LPD3DXFONT lpd3dxfont;
	LPD3DXSPRITE lpd3dxsprite;
	D3DPRESENT_PARAMETERS d3dpp;
	DWORD frame = 0, lasttime, currtime;
	RECT rect = {130, 3, 190, 30};
	D3DXVECTOR3 d3dxvect(20, 20, 0);
	float time;
	char buf[64] = "? fps";

	hmutex = CreateMutex(NULL, FALSE, "test");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(hmutex);
		return 0;
	}

	wc.style = 0;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "test";

	if (!RegisterClass(&wc)) return 0;

	if (!(hwnd = CreateWindow(
		"test", "Test DirectX",
		WS_SYSMENU | WS_MINIMIZEBOX,
		200, 200, 200, 200,
		NULL, NULL, hInstance, NULL
	))) return 0;

	// Create Direct3D object
	lpd3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!lpd3d) return 0;

	// Create Direct3D device
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed = TRUE;
	if (FAILED(lpd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
	D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &lpd3ddev))) {
		lpd3d->Release();
		return 0;
	}

	// Create texture from file (Transparent setting etc)
	if (FAILED(D3DXCreateTextureFromFile(lpd3ddev, "bg.bmp", &lpd3dtexture1))) {
		lpd3ddev->Release();
		lpd3d->Release();
		return 0;
	}
	if (FAILED(D3DXCreateTextureFromFileEx(lpd3ddev, "obj.bmp", D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
	0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0x00ff00ff, NULL, NULL, &lpd3dtexture2))) {
		lpd3dtexture1->Release();
		lpd3ddev->Release();
		lpd3d->Release();
		return 0;
	}

	// Create sprite object
	if (FAILED(D3DXCreateSprite(lpd3ddev, &lpd3dxsprite))) {
		lpd3dtexture1->Release();
		lpd3dtexture2->Release();
		lpd3ddev->Release();
		lpd3d->Release();
		return 0;
	}

	// Create Direct3D font
	if (FAILED(D3DXCreateFont(lpd3ddev, 14, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET,
	OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH , "Verdana", &lpd3dxfont))) {
		lpd3dxsprite->Release();
		lpd3dtexture1->Release();
		lpd3dtexture2->Release();
		lpd3ddev->Release();
		lpd3d->Release();
		return 0;
	}

	timeBeginPeriod(1);
	lasttime = timeGetTime();
	ShowWindow(hwnd, nCmdShow);
	do {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			lpd3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 128, 64), 1.0, 0);
			if (SUCCEEDED(lpd3ddev->BeginScene())) {
				if (SUCCEEDED(lpd3dxsprite->Begin(D3DXSPRITE_ALPHABLEND))) {
					// Get fps
					currtime = timeGetTime();
					frame++;
					time = (float)currtime - (float)lasttime;
					if (time >= 1000.0) {
						time = (float)frame / time * 1000.0;
						lasttime = currtime;
						frame = 0;
						sprintf(buf, "%.2f fps", time);
					}

					// Display texture and fps
					lpd3dxsprite->Draw(lpd3dtexture1, NULL, NULL, &d3dxvect, 0xffffffff);
					lpd3dxsprite->Draw(lpd3dtexture2, NULL, NULL, NULL, 0xffffffff);
					lpd3dxfont->DrawText(lpd3dxsprite, buf, -1, &rect, 0, D3DCOLOR_XRGB(0, 255, 0));
					lpd3dxsprite->End();
				}
				lpd3ddev->EndScene();
			}
			lpd3ddev->Present(NULL, NULL, NULL, NULL);
		}
	} while (msg.message != WM_QUIT);

	lpd3dxfont->Release();
	lpd3dxsprite->Release();
	lpd3dtexture1->Release();
	lpd3dtexture2->Release();
	lpd3ddev->Release();
	lpd3d->Release();
	return msg.wParam;
}