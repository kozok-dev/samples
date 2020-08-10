#include <windows.h>
#include <mshtml.h>
#define Pix2Him(size, w, h, hdc) size.cx = (w) * 2540 / GetDeviceCaps(hdc, LOGPIXELSX); \
	size.cy = (h) * 2540 / GetDeviceCaps(hdc, LOGPIXELSY);
#define Him2Pix(size_out, size, hdc) size_out.cx = size.cx * GetDeviceCaps(hdc, LOGPIXELSX) / 2540; \
	size_out.cy = size.cy * GetDeviceCaps(hdc, LOGPIXELSY) / 2540;

class OleTest : public IAdviseSink, public IObjectWithSite, public IOleClientSite,
public IOleContainer, public IOleControlSite, public IOleInPlaceFrame, public IOleInPlaceSite {
	bool m_capture, m_cpf;
	ULONG m_refcnt;
	DWORD m_oac, m_dac;
	SIZE m_size;
	RECT m_rect;
	BORDERWIDTHS m_bw;
	HWND m_hwnd;
	HMENU m_hmenu;
	HENHMETAFILE m_hemf;
	IUnknown *m_pu, *m_pus;
	IStorage *m_ps;
	IOleObject *m_poo;
	IDataObject *m_pdo;
	IViewObject2 *m_pvo;
	IOleInPlaceObjectWindowless *m_poipow;
	IOleInPlaceActiveObject *m_poipao;

public:
	OleTest() {
		m_capture = m_cpf = false;
		m_refcnt = 1;	// newのときはAddRefしたことにする。
		m_oac = m_dac = 0;
		m_size.cx = m_size.cy = 0;
		m_rect.left = m_rect.top = m_rect.right = m_rect.bottom = 0;
		m_bw.left = m_bw.top = m_bw.right = m_bw.bottom = 0;
		m_hwnd = NULL;
		m_hmenu = NULL;
		m_hemf = NULL;
		m_pu = m_pus = NULL;
		m_ps = NULL;
		m_poo = NULL;
		m_pdo = NULL;
		m_pvo = NULL;
		m_poipow = NULL;
		m_poipao = NULL;
		CoInitialize(NULL);
	}

	~OleTest() {
		CoUninitialize();
	}

	// 各種OLE設定とウィンドウ作成。
	bool Create(HINSTANCE hinst, LPSTR cmdline) {
		bool flag;
		WCHAR wfn[256];
		WNDCLASS wc;
		SIZE size;
		HDC hdc;
		IPersistFile *ppf = NULL;
		IMoniker *pm = NULL;
		IObjectWithSite *pows;
		FORMATETC fe;

		if (m_hwnd != NULL ||
		MultiByteToWideChar(CP_ACP, 0, cmdline, -1, wfn, sizeof(wfn) / sizeof(wfn[0])) == 0) return false;
		flag = false;

		wc.style = 0;
		wc.lpfnWndProc = procWnd;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hinst;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = TEXT("oletest");
		if (RegisterClass(&wc) == 0) goto procend;

		if ((m_hwnd = CreateWindow(wc.lpszClassName, TEXT("OLE Test"), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		200, 150, 350, 350, NULL, NULL, hinst, this)) == NULL) goto procend;
		GetClientRect(m_hwnd, &m_rect);

		// インプロセスハンドラを使用してファイルから適切なOLEを読み込む。
		if ((FAILED(StgCreateDocfile(NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_TRANSACTED, 0, &m_ps)) ||
		FAILED(OleCreateFromFile(CLSID_NULL, wfn, IID_IUnknown, OLERENDER_DRAW, NULL, this, m_ps, (LPVOID *)&m_pu))) &&
		// 失敗したらモニカを使用してファイルから適切なOLEを読み込む。
		(FAILED(CreateFileMoniker(wfn, &pm)) ||
		FAILED(BindMoniker(pm, 0, IID_IUnknown, (LPVOID *)&m_pu))) &&
		// さらに失敗したらファイルからHTMLのOLEを読み込む。
		(FAILED(CoCreateInstance(CLSID_HTMLDocument, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID *)&m_pu)) ||
		FAILED(m_pu->QueryInterface(IID_IPersistFile, (void **)&ppf)) ||
		FAILED(ppf->Load(wfn, STGM_READWRITE | STGM_SHARE_EXCLUSIVE)))) goto procend;

		// IDataObject設定。
		if (FAILED(m_pu->QueryInterface(IID_IDataObject, (void **)&m_pdo))) goto procend;
		fe.cfFormat = 0;
		fe.ptd = NULL;
		fe.dwAspect = -1;
		fe.lindex = -1;
		fe.tymed = -1;
		m_pdo->DAdvise(&fe, ADVF_NODATA, this, &m_dac);
		getViewData();

		// IViewObject2設定。
		m_pu->QueryInterface(IID_IViewObject2, (void **)&m_pvo);
		if (m_pvo != NULL)
			if (SUCCEEDED(m_pvo->SetAdvise(DVASPECT_CONTENT, 0, this)) &&
			SUCCEEDED(m_pvo->GetExtent(DVASPECT_CONTENT, -1, NULL, &m_size))) {
				hdc = ::GetDC(NULL);
				Him2Pix(size, m_size, hdc);
				m_rect.right -= size.cx;
				m_rect.bottom -= size.cy;
				::ReleaseDC(NULL, hdc);
			} else {
				m_pvo->Release();
				m_pvo = NULL;
			}

		// IOleObject設定。
		if (FAILED(m_pu->QueryInterface(IID_IOleObject, (void **)&m_poo))) goto procend;
		m_poo->Advise(this, &m_oac);
		m_poo->SetClientSite(this);
		m_poo->SetHostNames(L"OLE Test", L"oletest");

		setControlMenu();

		m_poo->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, this, 0, m_hwnd, &m_rect);
		onSize();

		m_pu->QueryInterface(IID_IObjectWithSite, (void **)&pows);
		if (pows != NULL) {
			pows->SetSite((IOleClientSite *)this);
			pows->Release();
		}

		flag = true;
procend:
		if (ppf != NULL) ppf->Release();
		if (pm != NULL) pm->Release();
		return flag;
	}

	// メタファイル形式のデータ取得。
	void getViewData() {
		BYTE *buf;
		UINT size;
		METAFILEPICT mfp;
		FORMATETC fe;
		STGMEDIUM sm;

		if (m_hemf != NULL) {
			DeleteEnhMetaFile(m_hemf);
			m_hemf = NULL;
		}

		// CF_ENHMETAFILE形式でデータ取得。
		fe.cfFormat = CF_ENHMETAFILE;
		fe.ptd = NULL;
		fe.dwAspect = DVASPECT_CONTENT;
		fe.lindex = -1;
		fe.tymed = TYMED_ENHMF;
		if (SUCCEEDED(m_pdo->GetData(&fe, &sm))) {
			if ((m_hemf = CopyEnhMetaFile(sm.hEnhMetaFile, NULL)) == NULL) return;
		} else {
			// 失敗した場合はCF_METAFILEPICT形式でデータ取得。
			fe.cfFormat = CF_METAFILEPICT;
			fe.tymed = TYMED_MFPICT;
			if (FAILED(m_pdo->GetData(&fe, &sm))) return;

			if ((buf = (BYTE *)GlobalLock(sm.hMetaFilePict)) == NULL) return;
			mfp.mm = *(LONG *)buf;
			mfp.xExt = *(LONG *)&buf[4];
			mfp.yExt = *(LONG *)&buf[8];
			mfp.hMF = *(HMETAFILE *)&buf[12];
			GlobalUnlock(sm.hMetaFilePict);

			// 旧式メタファイルを拡張メタファイルに変換。
			if ((size = GetMetaFileBitsEx(mfp.hMF, 0, NULL)) == 0) return;
			buf = new BYTE[size];
			if (GetMetaFileBitsEx(mfp.hMF, size, buf) == 0) return;
			m_hemf = SetWinMetaFileBits(size, buf, NULL, &mfp);
			delete[] buf;
		}
		ReleaseStgMedium(&sm);
	}

	// FormatEtcを列挙。
	void enumFormatEtc(DWORD dir, LPWSTR buf) {
		WCHAR buf1[512], buf2[512];
		HRESULT hr;
		CLSID clsid;
		IEnumFORMATETC *pefe;
		FORMATETC fe;

		hr = m_pdo->EnumFormatEtc(dir, &pefe);
		if (FAILED(hr)) return;
		if (hr == OLE_S_USEREG && SUCCEEDED(m_poo->GetUserClassID(&clsid)))
			OleRegEnumFormatEtc(clsid, dir, &pefe);
		if (pefe == NULL) return;

		wsprintfW(buf, L"%x %x\n", dir, hr);
		while (pefe->Next(1, &fe, NULL) == S_OK) {
			if (fe.cfFormat < 0)
				wsprintfW(buf2, L"%x", fe.cfFormat);
			else
				GetClipboardFormatNameW(fe.cfFormat, buf2, sizeof(buf2) / sizeof(buf2[0]));
			wsprintfW(buf1, L"%s %x %d %d %d\n", buf2, fe.ptd, fe.dwAspect, fe.lindex, fe.tymed);
			lstrcatW(buf, buf1);
		}

		pefe->Release();
	}

	// 操作メニュー設定。
	void setControlMenu() {
		WCHAR buf[256];
		HMENU hm, hsm1, hsm2;
		MENUITEMINFOW mii;
		HRESULT hr;
		CLSID clsid;
		IEnumOLEVERB *peov;
		OLEVERB ov;

		if (m_hmenu != NULL) DestroyMenu(m_hmenu);
		mii.cbSize = sizeof(MENUITEMINFOW);

		// DoVerbメニュー作成。
		hsm1 = CreateMenu();
		mii.fMask = MIIM_DATA | MIIM_ID | MIIM_STRING;
		mii.wID = 100;
		hr = m_poo->EnumVerbs(&peov);
		if (SUCCEEDED(hr)) {
			if ((hr == OLE_S_USEREG || peov == NULL) && SUCCEEDED(m_poo->GetUserClassID(&clsid)))
				OleRegEnumVerbs(clsid, &peov);
			if (peov != NULL) {
				while (peov->Next(1, &ov, NULL) == S_OK) {
					wsprintfW(buf, L"%d %s %x %x\n", ov.lVerb, ov.lpszVerbName, ov.fuFlags, ov.grfAttribs);
					mii.dwItemData = ov.lVerb;
					mii.dwTypeData = buf;
					InsertMenuItemW(hsm1, -1, FALSE, &mii);
					mii.wID++;
				}
				peov->Release();
			}
		}
		mii.dwItemData = OLEIVERB_SHOW; mii.dwTypeData = L"OLEIVERB_SHOW";
		InsertMenuItemW(hsm1, -1, FALSE, &mii); mii.wID++;
		mii.dwItemData = OLEIVERB_OPEN; mii.dwTypeData = L"OLEIVERB_OPEN";
		InsertMenuItemW(hsm1, -1, FALSE, &mii); mii.wID++;
		mii.dwItemData = OLEIVERB_HIDE; mii.dwTypeData = L"OLEIVERB_HIDE";
		InsertMenuItemW(hsm1, -1, FALSE, &mii); mii.wID++;
		mii.dwItemData = OLEIVERB_UIACTIVATE; mii.dwTypeData = L"OLEIVERB_UIACTIVATE";
		InsertMenuItemW(hsm1, -1, FALSE, &mii); mii.wID++;
		mii.dwItemData = OLEIVERB_INPLACEACTIVATE; mii.dwTypeData = L"OLEIVERB_INPLACEACTIVATE";
		InsertMenuItemW(hsm1, -1, FALSE, &mii); mii.wID++;
		mii.dwItemData = OLEIVERB_DISCARDUNDOSTATE; mii.dwTypeData = L"OLEIVERB_DISCARDUNDOSTATE";
		InsertMenuItemW(hsm1, -1, FALSE, &mii); mii.wID++;

		// IOleObjectメニュー作成。
		hsm2 = CreateMenu();
		mii.fMask = MIIM_STRING | MIIM_SUBMENU;
		mii.hSubMenu = hsm1;
		mii.dwTypeData = L"DoVerb";
		InsertMenuItemW(hsm2, -1, FALSE, &mii);
		mii.fMask = MIIM_ID | MIIM_STRING;
		mii.wID = 1; mii.dwTypeData = L"EnumVerbs"; InsertMenuItemW(hsm2, -1, FALSE, &mii);
		mii.wID = 2; mii.dwTypeData = L"Update"; InsertMenuItemW(hsm2, -1, FALSE, &mii);
		mii.wID = 3; mii.dwTypeData = L"IsUpToDate"; InsertMenuItemW(hsm2, -1, FALSE, &mii);
		mii.wID = 4; mii.dwTypeData = L"GetUserClassID"; InsertMenuItemW(hsm2, -1, FALSE, &mii);
		mii.wID = 5; mii.dwTypeData = L"GetUserType"; InsertMenuItemW(hsm2, -1, FALSE, &mii);
		mii.wID = 6; mii.dwTypeData = L"SetExtent"; InsertMenuItemW(hsm2, -1, FALSE, &mii);
		mii.wID = 7; mii.dwTypeData = L"GetExtent"; InsertMenuItemW(hsm2, -1, FALSE, &mii);
		mii.wID = 8; mii.dwTypeData = L"GetMiscStatus"; InsertMenuItemW(hsm2, -1, FALSE, &mii);
		m_hmenu = CreateMenu();
		mii.fMask = MIIM_STRING | MIIM_SUBMENU;
		mii.hSubMenu = hsm2;
		mii.dwTypeData = L"IOleObject";
		InsertMenuItemW(m_hmenu, -1, FALSE, &mii);

		// IDataObjectメニュー作成。
		hsm1 = CreateMenu();
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_ID | MIIM_STRING;
		mii.wID = 9; mii.dwTypeData = L"EnumFormatEtc"; InsertMenuItemW(hsm1, -1, FALSE, &mii);
		mii.fMask = MIIM_STRING | MIIM_SUBMENU;
		mii.hSubMenu = hsm1;
		mii.dwTypeData = L"IDataObject";
		InsertMenuItemW(m_hmenu, -1, FALSE, &mii);

		// IOleInPlaceObjectメニュー作成。
		hsm1 = CreateMenu();
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_ID | MIIM_STRING;
		mii.wID = 10; mii.dwTypeData = L"InPlaceDeactivate"; InsertMenuItemW(hsm1, -1, FALSE, &mii);
		mii.wID = 11; mii.dwTypeData = L"UIDeactivate"; InsertMenuItemW(hsm1, -1, FALSE, &mii);
		mii.wID = 12; mii.dwTypeData = L"ReactivateAndUndo"; InsertMenuItemW(hsm1, -1, FALSE, &mii);
		mii.fMask = MIIM_STRING | MIIM_SUBMENU;
		mii.hSubMenu = hsm1;
		mii.dwTypeData = L"IOleInPlaceObject";
		InsertMenuItemW(m_hmenu, -1, FALSE, &mii);

		::SetMenu(m_hwnd, m_hmenu);
	}

	// メニュー実行時。
	void onCommand(WORD id) {
		WCHAR buf1[512], buf2[512], buf3[512];
		DWORD status;
		RECT rect;
		SIZEL sl;
		HDC hdc;
		MENUITEMINFO mii;
		HRESULT hr;
		LPOLESTR str;
		CLSID clsid;
		IEnumOLEVERB *peov;
		OLEVERB ov;

		switch (id) {
		case 1:
			hr = m_poo->EnumVerbs(&peov);
			if (FAILED(hr)) break;
			if ((hr == OLE_S_USEREG || peov == NULL) && SUCCEEDED(m_poo->GetUserClassID(&clsid)))
				OleRegEnumVerbs(clsid, &peov);
			if (peov == NULL) break;
			wsprintfW(buf1, L"%x\n", hr);

			while (peov->Next(1, &ov, NULL) == S_OK) {
				wsprintfW(buf2, L"%d %s %x %x\n", ov.lVerb, ov.lpszVerbName, ov.fuFlags, ov.grfAttribs);
				lstrcatW(buf1, buf2);
			}
			MessageBoxW(m_hwnd, buf1, L"EnumVerbs", MB_OK);

			peov->Release();
			break;
		case 2:
			wsprintfW(buf1, L"%x", m_poo->Update());
			MessageBoxW(m_hwnd, buf1, L"Update", MB_OK);
			break;
		case 3:
			wsprintfW(buf1, L"%x", m_poo->IsUpToDate());
			MessageBoxW(m_hwnd, buf1, L"IsUpToDate", MB_OK);
			break;
		case 4:
			hr = m_poo->GetUserClassID(&clsid);
			StringFromCLSID(clsid, &str);
			wsprintfW(buf1, L"%x %s", hr, str);
			MessageBoxW(m_hwnd, buf1, L"GetUserClassID", MB_OK);
			break;
		case 5:
			hr = m_poo->GetUserType(USERCLASSTYPE_FULL, &str);
			if (hr == OLE_S_USEREG) {
				if (SUCCEEDED(m_poo->GetUserClassID(&clsid))) {
					hr = OleRegGetUserType(clsid, USERCLASSTYPE_FULL, &str);
					wsprintfW(buf2, L"%x %s\n", hr, str);
					lstrcpyW(buf1, buf2);
					hr = OleRegGetUserType(clsid, USERCLASSTYPE_SHORT, &str);
					wsprintfW(buf2, L"%x %s\n", hr, str);
					lstrcatW(buf1, buf2);
					hr = OleRegGetUserType(clsid, USERCLASSTYPE_APPNAME, &str);
					wsprintfW(buf2, L"%x %s", hr, str);
					lstrcatW(buf1, buf2);
				}
			} else {
				wsprintfW(buf2, L"%x %s\n", hr, str);
				lstrcpyW(buf1, buf2);
				hr = m_poo->GetUserType(USERCLASSTYPE_SHORT, &str);
				wsprintfW(buf2, L"%x %s\n", hr, str);
				lstrcatW(buf1, buf2);
				hr = m_poo->GetUserType(USERCLASSTYPE_APPNAME, &str);
				wsprintfW(buf2, L"%x %s\n", hr, str);
				lstrcatW(buf1, buf2);
			}

			MessageBoxW(m_hwnd, buf1, L"GetUserType", MB_OK);
			break;
		case 6:
			GetClientRect(m_hwnd, &rect);
			hdc = ::GetDC(m_hwnd);
			Pix2Him(sl, rect.right - rect.left, rect.bottom - rect.top, hdc);
			::ReleaseDC(m_hwnd, hdc);
			m_poo->SetExtent(DVASPECT_CONTENT, &sl);
			RedrawWindow(m_hwnd, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW);
			break;
		case 7:
			hr = m_poo->GetExtent(DVASPECT_CONTENT, &sl);
			wsprintfW(buf1, L"%x %d %d", hr, sl.cx, sl.cy);
			MessageBoxW(m_hwnd, buf1, L"GetExtent", MB_OK);
			break;
		case 8:
			hr = m_poo->GetMiscStatus(DVASPECT_CONTENT, &status);
			if (hr == OLE_S_USEREG && SUCCEEDED(m_poo->GetUserClassID(&clsid)))
				hr = OleRegGetMiscStatus(clsid, DVASPECT_CONTENT, &status);
			wsprintfW(buf1, L"%x %d", hr, status);
			MessageBoxW(m_hwnd, buf1, L"GetMiscStatus", MB_OK);
			break;
		case 9:
			if (m_pdo == NULL) break;
			enumFormatEtc(DATADIR_GET, buf1);
			enumFormatEtc(DATADIR_SET, buf2);
			lstrcatW(buf1, buf2);
			MessageBoxW(m_hwnd, buf1, L"EnumFormatEtc", MB_OK);
			break;
		case 10:
			if (m_poipow != NULL) m_poipow->InPlaceDeactivate();
			break;
		case 11:
			if (m_poipow != NULL) m_poipow->UIDeactivate();
			break;
		case 12:
			if (m_poipow != NULL) m_poipow->ReactivateAndUndo();
			break;
		default:
			if (id < 100) break;

			mii.cbSize = sizeof(MENUITEMINFO);
			mii.fMask = MIIM_DATA;
			GetMenuItemInfo(GetSubMenu(GetSubMenu(GetMenu(m_hwnd), 0), 0), id, FALSE, &mii);
			m_poo->DoVerb(mii.dwItemData, NULL, this, 0, m_hwnd, &m_rect);
			onSize();
		}
	}

	// 描画処理。
	void onPaint(HDC hdc) {
		SIZE size;
		RECT rect;

		Him2Pix(size, m_size, hdc);
		rect.left = m_rect.left;
		rect.top = m_rect.top;
		rect.right = m_rect.left + size.cx;
		rect.bottom = m_rect.top + size.cy;

		if (m_pvo == NULL) PlayEnhMetaFile(hdc, m_hemf, &rect);
		else m_pvo->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hdc, (LPRECTL)&rect, NULL, NULL, 0);
	}

	// ウィンドウサイズ変更時。
	void onSize() {
		RECT rect;
		SIZEL sl;
		HDC hdc;

		GetClientRect(m_hwnd, &rect);

		/*m_rect.right = m_rect.left + (rect.right - rect.left);
		m_rect.bottom = m_rect.top + (rect.bottom - rect.top);

		if (m_poo != NULL) {
			hdc = ::GetDC(m_hwnd);
			Pix2Him(sl, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top, hdc);
			::ReleaseDC(m_hwnd, hdc);
			m_poo->SetExtent(DVASPECT_CONTENT, &sl);
		}*/

		if (m_poipow == NULL) RedrawWindow(m_hwnd, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW);
		else if (m_poipao != NULL) m_poipao->ResizeBorder(&rect, this, TRUE);
	}

	// タイマー処理。
	void onTimer() {
		RECT rect;

		if (!m_cpf) return;

		if (m_poipao != NULL) {
			GetClientRect(m_hwnd, &rect);
			m_poipow->SetObjectRects(&m_rect, &rect);
		}

		m_cpf = false;

		if (m_pvo == NULL) m_poo->GetExtent(DVASPECT_CONTENT, &m_size);
		else m_pvo->GetExtent(DVASPECT_CONTENT, -1, NULL, &m_size);

		//RedrawWindow(m_hwnd, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW);
	}

	// 各種オブジェクト終了処理。
	void onDestroy() {
		if (m_pdo != NULL) {
			m_pdo->DUnadvise(m_dac);
			m_pdo->Release();
		}
		if (m_pvo != NULL) {
			m_pvo->SetAdvise(DVASPECT_CONTENT, 0, NULL);
			m_pvo->Release();
		}
		if (m_poo != NULL) {
			m_poo->Unadvise(m_oac);
			m_poo->Close(OLECLOSE_NOSAVE);
			m_poo->SetClientSite(NULL);
			m_poo->Release();
		}
		if (m_pus != NULL) m_pus->Release();
		if (m_poipao != NULL) m_poipao->Release();
		if (m_pu != NULL) m_pu->Release();
		if (m_ps != NULL) m_ps->Release();
		if (m_hemf != NULL) DeleteEnhMetaFile(m_hemf);
		if (m_hmenu != NULL) DestroyMenu(m_hmenu);
	}

	// ウィンドウプロシージャ。
	static LRESULT CALLBACK procWnd(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		static OleTest *pot;
		HDC hdc;
		PAINTSTRUCT ps;

		switch (uMsg) {
		case WM_CREATE:
			pot = (OleTest *)((LPCREATESTRUCT)lParam)->lpCreateParams;
			SetTimer(hwnd, 1, 50, OleTest::procTimer);
			procTimer(hwnd, WM_CREATE, 1, (DWORD)pot);
			return 0;
		case WM_COMMAND:
			pot->onCommand(LOWORD(wParam));
			return 0;
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			pot->onPaint(hdc);
			EndPaint(hwnd, &ps);
			return 0;
		case WM_SIZE:
			pot->onSize();
			return 0;
		case WM_DESTROY:
			KillTimer(hwnd, 1);
			pot->onDestroy();
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	// タイマープロシージャ。
	static VOID CALLBACK procTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
		static OleTest *pot;

		if (pot == NULL) {
			if (uMsg == WM_CREATE) pot = (OleTest *)dwTime;
		} else
			pot->onTimer();
	}

	//--------------------------------------------------
	// IUnknown

	STDMETHODIMP QueryInterface(REFIID iid, void **ppvObject) {
		if (iid == IID_IUnknown) *ppvObject = this;
		else if (iid == IID_IAdviseSink) *ppvObject = (IAdviseSink *)this;
		else if (iid == IID_IObjectWithSite) *ppvObject = (IObjectWithSite *)this;
		else if (iid == IID_IOleClientSite) *ppvObject = (IOleClientSite *)this;
		else if (iid == IID_IParseDisplayName) *ppvObject = (IParseDisplayName *)this;
		else if (iid == IID_IOleContainer) *ppvObject = (IOleContainer *)this;
		else if (iid == IID_IOleControlSite) *ppvObject = (IOleControlSite *)this;
		else if (iid == IID_IOleWindow) *ppvObject = (IOleInPlaceUIWindow *)this;
		else if (iid == IID_IOleInPlaceUIWindow) *ppvObject = (IOleInPlaceUIWindow *)this;
		else if (iid == IID_IOleInPlaceFrame) *ppvObject = (IOleInPlaceFrame *)this;
		else if (iid == IID_IOleInPlaceSite) *ppvObject = (IOleInPlaceSite *)this;
		//else if (iid == IID_IOleInPlaceSiteEx) *ppvObject = (IOleInPlaceSiteEx *)this;
		//else if (iid == IID_IOleInPlaceSiteWindowless) *ppvObject = (IOleInPlaceSiteWindowless *)this;
		else {
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}

		AddRef();
		return S_OK;
	}

	STDMETHODIMP_(ULONG) AddRef() {
		return ++m_refcnt;
	}

	STDMETHODIMP_(ULONG) Release() {
		if (--m_refcnt > 0) return m_refcnt;
		return 0;
	}

	//--------------------------------------------------
	// IAdviseSink

	void STDMETHODCALLTYPE OnDataChange(FORMATETC *pFormatetc, STGMEDIUM *pStgmed) {
		getViewData();
		//RedrawWindow(m_hwnd, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW);
	}

	void STDMETHODCALLTYPE OnViewChange(DWORD dwAspect, LONG lindex) {
		//RedrawWindow(m_hwnd, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW);
	}

	void STDMETHODCALLTYPE OnRename(IMoniker *pmk) {
	}

	void STDMETHODCALLTYPE OnSave() {
	}

	void STDMETHODCALLTYPE OnClose() {
	}

	//--------------------------------------------------
	// IObjectWithSite

	STDMETHODIMP GetSite(REFIID riid, void **ppvSite) {
		if (ppvSite == NULL) return E_POINTER;
		if (m_pus == NULL) {
			*ppvSite = NULL;
			return E_FAIL;
		}
		return m_pus->QueryInterface(riid, ppvSite);
	}

	STDMETHODIMP SetSite(IUnknown *pUnkSite) {
		m_pus = pUnkSite;
		return S_OK;
	}

	//--------------------------------------------------
	// IOleClientSite

	STDMETHODIMP SaveObject() {
		return S_OK;
	}

	STDMETHODIMP GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk) {
		return E_FAIL;
	}

	STDMETHODIMP GetContainer(LPOLECONTAINER FAR *ppContainer) {
		if (ppContainer == NULL) return E_POINTER;
		if (m_pus == NULL) return E_NOTIMPL;
		return m_pus->QueryInterface(IID_IOleContainer, (void **)ppContainer);
	}

	STDMETHODIMP ShowObject() {
		HDC hdc;

		if (m_pvo == NULL) return S_OK;
		hdc = ::GetDC(m_hwnd);
		m_pvo->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hdc, (LPRECTL)&m_rect, NULL, NULL, 0);
		::ReleaseDC(m_hwnd, hdc);
		return S_OK;
	}

	STDMETHODIMP OnShowWindow(BOOL fShow) {
		return S_OK;
	}

	STDMETHODIMP RequestNewObjectLayout() {
		return E_NOTIMPL;
	}

	//--------------------------------------------------
	// IParseDisplayName

	STDMETHODIMP ParseDisplayName(IBindCtx *pbc, LPOLESTR pszDisplayName,
	ULONG *pchEaten, IMoniker **ppmkOut) {
		return E_NOTIMPL;
	}

	//--------------------------------------------------
	// IOleContainer

	STDMETHODIMP EnumObjects(DWORD grfFlags, IEnumUnknown **ppenum) {
		return E_NOTIMPL;
	}

	STDMETHODIMP LockContainer(BOOL fLock) {
		return S_OK;
	}

	//--------------------------------------------------
	// IOleControlSite

	STDMETHODIMP OnControlInfoChanged() {
		return S_OK;
	}

	STDMETHODIMP LockInPlaceActive(BOOL fLock) {
		return S_OK;
	}

	STDMETHODIMP GetExtendedControl(IDispatch **ppDisp) {
		if (ppDisp == NULL) return E_POINTER;
		return m_pu->QueryInterface(IID_IDispatch, (void **)ppDisp);
	}

	STDMETHODIMP TransformCoords(POINTL* pPtlHimetric, POINTF *pPtfContainer, DWORD dwFlags) {
		return S_OK;
	}

	STDMETHODIMP TranslateAccelerator(LPMSG pMsg, DWORD grfModifiers) {
		return S_FALSE;
	}

	STDMETHODIMP OnFocus(BOOL fGotFocus) {
		return S_OK;
	}

	STDMETHODIMP ShowPropertyFrame() {
		return E_NOTIMPL;
	}

	//--------------------------------------------------
	// IOleWindow

	STDMETHODIMP GetWindow(HWND *phwnd) {
		*phwnd = m_hwnd;
		return S_OK;
	}

	STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode) {
		return S_OK;
	}

	//--------------------------------------------------
	// IOleInPlaceUIWindow

	STDMETHODIMP GetBorder(LPRECT lprectBorder) {
		if (lprectBorder == NULL) return E_POINTER;
		GetClientRect(m_hwnd, lprectBorder);
		return S_OK;
	}

	STDMETHODIMP RequestBorderSpace(LPCBORDERWIDTHS pborderwidths) {
		if (pborderwidths == NULL) return E_POINTER;
		return S_OK;
	}

	STDMETHODIMP SetBorderSpace(LPCBORDERWIDTHS pborderwidths) {
		if (pborderwidths == NULL) return E_POINTER;

		m_rect.left += pborderwidths->left - m_bw.left;
		m_rect.top += pborderwidths->top - m_bw.top;
		m_rect.right += pborderwidths->left - m_bw.left;
		m_rect.bottom += pborderwidths->top - m_bw.top;

		m_bw = *pborderwidths;
		m_cpf = true;

		return S_OK;
	}

	STDMETHODIMP SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName) {
		if (m_poipao != NULL) m_poipao->Release();
		m_poipao = pActiveObject;
		return S_OK;
	}

	//--------------------------------------------------
	// IOleInPlaceFrame

	STDMETHODIMP InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths) {
		MENUITEMINFO mii;

		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_STRING | MIIM_SUBMENU;
		mii.hSubMenu = m_hmenu;
		mii.dwTypeData = TEXT("Control");
		InsertMenuItem(hmenuShared, -1, FALSE, &mii);
		return S_OK;
	}

	STDMETHODIMP SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject) {
		if (hmenuShared == NULL) setControlMenu();
		else ::SetMenu(m_hwnd, hmenuShared);
		OleSetMenuDescriptor(holemenu, m_hwnd, hwndActiveObject, this, m_poipao);
		return S_OK;
	}

	STDMETHODIMP RemoveMenus(HMENU hmenuShared) {
		while (GetMenuItemCount(hmenuShared) > 0) DeleteMenu(hmenuShared, 0, MF_BYPOSITION);
		return S_OK;
	}

	STDMETHODIMP SetStatusText(LPCOLESTR pszStatusText) {
		return S_OK;
	}

	STDMETHODIMP EnableModeless(BOOL fEnable) {
		return S_OK;
	}

	STDMETHODIMP TranslateAccelerator(LPMSG lpmsg, WORD wID) {
		return S_FALSE;
	}

	//--------------------------------------------------
	// IOleInPlaceSite

	STDMETHODIMP CanInPlaceActivate() {
		return S_OK;
	}

	STDMETHODIMP OnInPlaceActivate() {
		m_pu->QueryInterface(IID_IOleInPlaceObject, (void **)&m_poipow);
		return S_OK;
	}

	STDMETHODIMP OnUIActivate() {
		return S_OK;
	}

	STDMETHODIMP GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc,
	LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo) {
		if (ppFrame == NULL || ppDoc == NULL || lpFrameInfo == NULL) return E_POINTER;
		if (lpFrameInfo->cb != sizeof(OLEINPLACEFRAMEINFO)) return E_INVALIDARG;

		*ppFrame = this;
		*ppDoc = NULL;

		if (lprcPosRect != NULL) *lprcPosRect = m_rect;
		if (lprcClipRect != NULL) GetClientRect(m_hwnd, lprcClipRect);

		lpFrameInfo->fMDIApp = FALSE;
		lpFrameInfo->hwndFrame = m_hwnd;
		lpFrameInfo->haccel = NULL;
		lpFrameInfo->cAccelEntries = 0;

		return S_OK;
	}

	STDMETHODIMP Scroll(SIZE scrollExtent) {
		return E_NOTIMPL;
	}

	STDMETHODIMP OnUIDeactivate(BOOL fUndoable) {
		BORDERWIDTHS bw = {0, 0, 0, 0};

		SetMenu(NULL, NULL, NULL);
		SetBorderSpace(&bw);
		return S_OK;
	}

	STDMETHODIMP OnInPlaceDeactivate() {
		if (m_poipow == NULL) return S_OK;
		m_poipow->Release();
		m_poipow = NULL;
		return S_OK;
	}

	STDMETHODIMP DiscardUndoState() {
		return S_OK;
	}

	STDMETHODIMP DeactivateAndUndo() {
		return E_NOTIMPL;
	}

	STDMETHODIMP OnPosRectChange(LPCRECT lprcPosRect) {
		RECT rect;

		if (m_cpf) return S_OK;

		if (m_poipow != NULL && lprcPosRect != NULL) {
			m_rect = *lprcPosRect;
			//GetClientRect(m_hwnd, &rect);
			//m_poipow->SetObjectRects(&m_rect, &rect);
		}
		m_cpf = true;

		return S_OK;
	}

	//--------------------------------------------------
	// IOleInPlaceSiteEx

	STDMETHODIMP OnInPlaceActivateEx(BOOL *pfNoRedraw, DWORD dwFlags) {
		HRESULT hr = (dwFlags & ACTIVATE_WINDOWLESS) ? m_pu->QueryInterface(IID_IOleInPlaceObjectWindowless, (void **)&m_poipow) : E_FAIL;

		if (FAILED(hr)) m_pu->QueryInterface(IID_IOleInPlaceObject, (void **)&m_poipow);
		if (m_poipow != NULL) m_poipow->SetObjectRects(&m_rect, &m_rect);
		return S_OK;
	}

	STDMETHODIMP OnInPlaceDeactivateEx(BOOL fNoRedraw) {
		return S_OK;
	}

	STDMETHODIMP RequestUIActivate() {
		return S_OK;
	}

	//--------------------------------------------------
	// IOleInPlaceSiteWindowless

	STDMETHODIMP CanWindowlessActivate() {
		return S_OK;
	}

	STDMETHODIMP GetCapture() {
		return m_capture ? S_OK : S_FALSE;
	}

	STDMETHODIMP SetCapture(BOOL fCapture) {
		if (fCapture) {
			::SetCapture(m_hwnd);
			m_capture = true;
		} else {
			ReleaseCapture();
			m_capture = false;
		}
		return S_OK;
	}

	STDMETHODIMP GetFocus() {
		return S_OK;
	}

	STDMETHODIMP SetFocus(BOOL fFocus) {
		return S_OK;
	}

	STDMETHODIMP GetDC(LPCRECT pRect, DWORD grfFlags, HDC *phDC) {
		if (phDC == NULL) return E_POINTER;
		*phDC = ::GetDC(m_hwnd);
		return S_OK;
	}

	STDMETHODIMP ReleaseDC(HDC hDC) {
		::ReleaseDC(m_hwnd, hDC);
		return S_OK;
	}

	STDMETHODIMP InvalidateRect(LPCRECT pRect, BOOL fErase) {
		::InvalidateRect(m_hwnd, pRect, fErase);
		return S_OK;
	}

	STDMETHODIMP InvalidateRgn(HRGN hRGN, BOOL fErase) {
		::InvalidateRgn(m_hwnd, hRGN, fErase);
		return S_OK;
	}

	STDMETHODIMP ScrollRect(int dx, int dy, LPCRECT pRectScroll, LPCRECT pRectClip) {
		return S_OK;
	}

	STDMETHODIMP AdjustRect(LPRECT prc) {
		return S_OK;
	}

	STDMETHODIMP OnDefWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult) {
		*plResult = DefWindowProc(m_hwnd, msg, wParam, lParam);
		return S_OK;
	}
};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	OleTest *pot;

	if (lstrlenA(lpCmdLine) == 0) return 0;
	pot = new OleTest();
	if (pot->Create(hInstance, lpCmdLine) == NULL) return 0;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
