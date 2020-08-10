#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <mshtml.h>
#include <exdispid.h>
#define MY_CLSID TEXT("{02F21723-AD8A-4223-A020-4617FFC84D6F}")
const CLSID my_clsid = {0x2f21723, 0xad8a, 0x4223, {0xa0, 0x20, 0x46, 0x17, 0xff, 0xc8, 0x4d, 0x6f}};

const TCHAR *g_seurl[] = {
	TEXT("Yahoo"), TEXT("http://search.yahoo.co.jp/search?ei=Shift_JIS&p="),
	TEXT("Google"), TEXT("http://www.google.com/search?q="),
	TEXT("goo"), TEXT("http://search.goo.ne.jp/web.jsp?IE=Shift_JIS&MT="),
	TEXT("Excite"), TEXT("http://www.excite.co.jp/search.gw?c=japan&search="),
	TEXT("infoseek"), TEXT("http://search.www.infoseek.co.jp/Seek?qt="),
	TEXT("OCNサーチ"), TEXT("http://ocnsearch.goo.ne.jp/ocn.jsp?IE=Shift_JIS&MT="),
	TEXT("フレッシュアイ"), TEXT("http://search.fresheye.com/?kw="),
	TEXT("BIGLOBEサーチ"), TEXT("http://cgi.search.biglobe.ne.jp/cgi-bin/search-st?q=")
};

UINT g_refcnt;
HINSTANCE g_hinst;

class IETT : public IDeskBand, public IObjectWithSite, public IInputObject, public IDispatch {
	ULONG m_refcnt;
	DWORD m_cookie;
	HWND m_hwnd, m_hcb, m_hedt, m_hbtn;
	HFONT m_hfont;
	IInputObjectSite *m_pios;
	IWebBrowser2 *m_pwb;
	IConnectionPoint *m_pcp;

public:
	IETT() {
		g_refcnt++;
		m_refcnt = 1;
		m_cookie = 0;
		m_hwnd = m_hcb = m_hedt = m_hbtn = NULL;
		m_hfont = NULL;
		m_pios = NULL;
		m_pwb = NULL;
		m_pcp = NULL;
	}

	~IETT() {
		g_refcnt--;
		if (m_hfont != NULL) {
			DeleteObject(m_hfont);
			m_hfont = NULL;
		}
		if (m_pios != NULL) {
			m_pios->Release();
			m_pios = NULL;
		}
		if (m_pcp != NULL) {
			m_pcp->Unadvise(m_cookie);
			m_pcp->Release();
			m_pcp = NULL;
		}
		if (m_pwb != NULL) {
			m_pwb->Release();
			m_pwb = NULL;
		}
	}

	//--------------------------------------------------
	// IUnknown

	STDMETHODIMP_(ULONG) AddRef() {
		return ++m_refcnt;
	}

	STDMETHODIMP QueryInterface(REFIID iid, void **ppvObject) {
		if (iid == IID_IUnknown) *ppvObject = this;
		else if (iid == IID_IOleWindow) *ppvObject = (IOleWindow *)this;
		else if (iid == IID_IDockingWindow) *ppvObject = (IDockingWindow *)this;
		else if (iid == IID_IDeskBand) *ppvObject = (IDeskBand *)this;
		else if (iid == IID_IObjectWithSite) *ppvObject = (IObjectWithSite *)this;
		else if (iid == IID_IInputObject) *ppvObject = (IInputObject *)this;
		else if (iid == IID_IDispatch) *ppvObject = (IDispatch *)this;
		else {
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}

		AddRef();
		return S_OK;
	}

	STDMETHODIMP_(ULONG) Release() {
		if (--m_refcnt > 0) return m_refcnt;
		delete this;
		return 0;
	}

	//--------------------------------------------------
	// IOleWindow

	STDMETHODIMP GetWindow(HWND *phwnd) {
		if (phwnd == NULL) return E_FAIL;
		*phwnd = m_hwnd;
		return S_OK;
	}

	//--------------------------------------------------
	// IDockingWindow

	STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode) {
		return E_NOTIMPL;
	}

	STDMETHODIMP CloseDW(DWORD dwReserved) {
		ShowDW(FALSE);
		if (IsWindow(m_hwnd)) DestroyWindow(m_hwnd);
		m_hwnd = NULL;
		return S_OK;
	}

	STDMETHODIMP ResizeBorderDW(LPCRECT prcBorder, IUnknown *punkToolbarSite, BOOL fReserved) {
		return E_NOTIMPL;
	}

	STDMETHODIMP ShowDW(BOOL bShow) {
		if (IsWindow(m_hwnd)) ShowWindow(m_hwnd, bShow ? SW_SHOW : SW_HIDE);
		return S_OK;
	}

	//--------------------------------------------------
	// IDeskBand

	STDMETHODIMP GetBandInfo(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO *pdbi) {
		if (pdbi == NULL) return E_INVALIDARG;

		if (pdbi->dwMask & DBIM_MINSIZE) {
			pdbi->ptMinSize.x = 350;
			pdbi->ptMinSize.y = 24;
		}
		if (pdbi->dwMask & DBIM_MAXSIZE) pdbi->ptMaxSize.x = pdbi->ptMaxSize.y = -1;
		if (pdbi->dwMask & DBIM_INTEGRAL) pdbi->ptIntegral.x = pdbi->ptIntegral.y = 1;
		if (pdbi->dwMask & DBIM_ACTUAL) pdbi->ptActual.x = pdbi->ptActual.y = 0;
		if (pdbi->dwMask & DBIM_TITLE) lstrcpyW(pdbi->wszTitle, L"IE Toolbar Test");
		if (pdbi->dwMask & DBIM_MODEFLAGS) pdbi->dwModeFlags = DBIMF_NORMAL | DBIMF_VARIABLEHEIGHT;
		if (pdbi->dwMask & DBIM_BKCOLOR) pdbi->dwMask &= ~DBIM_BKCOLOR;

		return S_OK;
	}

	//--------------------------------------------------
	// IObjectWithSite

	// IEツールバーの表示処理を行う。
	STDMETHODIMP SetSite(IUnknown *pUnkSite) {
		int i;
		HWND hwnd, htt;
		WNDCLASS wc;
		RECT rect;
		INITCOMMONCONTROLSEX iccex;
		TOOLINFO ti;
		REBARINFO rbi;
		REBARBANDINFO rbbi;
		HRESULT hr;
		IOleWindow *pow;
		IServiceProvider *psp;
		IConnectionPointContainer *pcpc;

		if (m_hfont != NULL) {
			DeleteObject(m_hfont);
			m_hfont = NULL;
		}
		if (m_pios != NULL) {
			m_pios->Release();
			m_pios = NULL;
		}
		if (m_pcp != NULL) {
			m_pcp->Unadvise(m_cookie);
			m_pcp->Release();
			m_pcp = NULL;
		}
		if (m_pwb != NULL) {
			m_pwb->Release();
			m_pwb = NULL;
		}

		if (pUnkSite == NULL) return S_OK;

		// IEツールバーの親ウィンドウ取得。
		hwnd = NULL;
		if (SUCCEEDED(pUnkSite->QueryInterface(IID_IOleWindow, (void **)&pow))) {
			pow->GetWindow(&hwnd);
			pow->Release();
		}
		if (hwnd == NULL) return E_FAIL;

		// ブラウザオブジェクト取得。
		if (FAILED(pUnkSite->QueryInterface(IID_IServiceProvider, (void **)&psp))) return E_FAIL;
		hr = psp->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void **)&m_pwb);
		psp->Release();

		// ブラウザイベント監視。
		if (FAILED(hr) ||
		FAILED(m_pwb->QueryInterface(IID_IConnectionPointContainer, (void **)&pcpc))) return E_FAIL;
		hr = pcpc->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_pcp);
		pcpc->Release();
		if (FAILED(hr)) return E_FAIL;
		m_pcp->Advise((IDispatch *)this, &m_cookie);

		// 入力に関するオブジェクト取得。
		if (FAILED(pUnkSite->QueryInterface(IID_IInputObjectSite, (void **)&m_pios))) return E_FAIL;

		if (m_hwnd == NULL) {
			// ウィンドウクラス取得/登録。
			if (!GetClassInfo(g_hinst, TEXT("iett"), &wc)) {
				wc.style = 0;
				wc.lpfnWndProc = procWnd;
				wc.cbClsExtra = 0;
				wc.cbWndExtra = 0;
				wc.hInstance = g_hinst;
				wc.hIcon = NULL;
				wc.hCursor = LoadCursor(NULL, IDC_ARROW);
				wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
				wc.lpszMenuName = NULL;
				wc.lpszClassName = TEXT("iett");
				if (RegisterClass(&wc) == 0) return E_FAIL;
			}

			// IEツールバーウィンドウ作成。
			if (GetClientRect(hwnd, &rect) == 0 ||
			(m_hwnd = CreateWindow(wc.lpszClassName, NULL, WS_CHILD | WS_TABSTOP, rect.left, rect.top,
			350, rect.bottom - rect.top, hwnd, NULL, g_hinst, (LPVOID)this)) == NULL) return E_FAIL;

			// フォント作成。
			if ((m_hfont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, NULL)) == NULL)
				return E_FAIL;

			// CommonControl初期化。
			iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
			iccex.dwICC = ICC_BAR_CLASSES | ICC_COOL_CLASSES;
			InitCommonControlsEx(&iccex);

			// ToolTipウィンドウ作成。
			htt = CreateWindow(TOOLTIPS_CLASS, NULL, 0, 0, 0, 0, 0, m_hwnd, NULL, g_hinst, NULL);
			ti.cbSize = sizeof(TOOLINFO);
			ti.uFlags = TTF_SUBCLASS;
			ti.hinst = NULL;

			// Rebarウィンドウ作成。
			if ((hwnd = CreateWindow(REBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0, m_hwnd, NULL, g_hinst, NULL)) == NULL) return E_FAIL;

			// Rebar設定。
			rbi.cbSize = sizeof(REBARINFO);
			rbi.fMask = 0;
			rbi.himl = NULL;
			if (!SendMessage(hwnd, RB_SETBARINFO, 0, (LPARAM)&rbi)) return E_FAIL;
			rbbi.cbSize = sizeof(REBARBANDINFO);
			rbbi.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE;
			rbbi.fStyle = RBBS_FIXEDSIZE;

			// RebarにCombobox追加。
			rbbi.hwndChild = m_hcb = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("COMBOBOX"), NULL,
			WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS,
			0, 0, 0, 200, hwnd, (HMENU)1, g_hinst, NULL);
			SendMessage(m_hcb, WM_SETFONT, (WPARAM)m_hfont, FALSE);
			for (i = 0; i < 16; i += 2)
				if (SendMessage(m_hcb, CB_ADDSTRING, 0, (LPARAM)g_seurl[i]) < 0 ||
				SendMessage(m_hcb, CB_SETITEMDATA, i >> 1, (LPARAM)g_seurl[i + 1]) == CB_ERR) return E_FAIL;
			SendMessage(m_hcb, CB_SETCURSEL, 0, 0);
			rbbi.cxMinChild = 130;
			rbbi.cyMinChild = 20;
			if (!SendMessage(hwnd, RB_INSERTBAND, -1, (LPARAM)&rbbi)) return E_FAIL;
			ti.hwnd = m_hcb;
			GetClientRect(m_hcb, &ti.rect);
			ti.lpszText = TEXT("検索サイトを選択します");
			SendMessage(htt, TTM_ADDTOOL, 0, (LPARAM)&ti);

			// Rebarに空白用ウィンドウ追加。
			rbbi.hwndChild = CreateWindow(TEXT("STATIC"), NULL, WS_CHILD, 0, 0, 0, 0,
			hwnd, NULL, g_hinst, NULL);
			rbbi.cxMinChild = 10;
			rbbi.cyMinChild = 0;
			if (!SendMessage(hwnd, RB_INSERTBAND, -1, (LPARAM)&rbbi)) return E_FAIL;

			// RebarにEdit追加。
			rbbi.hwndChild = m_hedt = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), NULL,
			WS_CHILD | ES_AUTOHSCROLL, 0, 0, 0, 0, hwnd, (HMENU)2, g_hinst, NULL);
			SendMessage(m_hedt, WM_SETFONT, (WPARAM)m_hfont, FALSE);
			SendMessage(m_hedt, EM_SETLIMITTEXT, 512, 0);
			rbbi.cxMinChild = 150;
			rbbi.cyMinChild = 18;
			if (!SendMessage(hwnd, RB_INSERTBAND, -1, (LPARAM)&rbbi)) return E_FAIL;
			ti.hwnd = m_hedt;
			GetClientRect(m_hedt, &ti.rect);
			ti.lpszText = TEXT("検索する文字を入力します");
			SendMessage(htt, TTM_ADDTOOL, 0, (LPARAM)&ti);

			// Rebarに空白用ウィンドウ追加。
			rbbi.hwndChild = CreateWindow(TEXT("STATIC"), NULL, WS_CHILD, 0, 0, 0, 0,
			hwnd, NULL, g_hinst, NULL);
			rbbi.cxMinChild = 10;
			rbbi.cyMinChild = 0;
			if (!SendMessage(hwnd, RB_INSERTBAND, -1, (LPARAM)&rbbi)) return E_FAIL;

			// RebarにButton追加。
			rbbi.hwndChild = m_hbtn = CreateWindow(TEXT("BUTTON"), TEXT("Search"),
			WS_CHILD | BS_NOTIFY, 0, 0, 0, 0, hwnd, (HMENU)3, g_hinst, NULL);
			SendMessage(m_hbtn, WM_SETFONT, (WPARAM)m_hfont, FALSE);
			rbbi.cxMinChild = 50;
			rbbi.cyMinChild = 18;
			if (!SendMessage(hwnd, RB_INSERTBAND, -1, (LPARAM)&rbbi)) return E_FAIL;
			ti.hwnd = m_hbtn;
			GetClientRect(m_hbtn, &ti.rect);
			ti.lpszText = TEXT("検索を開始します");
			SendMessage(htt, TTM_ADDTOOL, 0, (LPARAM)&ti);
		}

		return S_OK;
	}

	STDMETHODIMP GetSite(REFIID riid, void **ppvSite) {
		if (ppvSite == NULL) return E_FAIL;
		*ppvSite = NULL;
		if (m_pios != NULL) return m_pios->QueryInterface(riid, ppvSite);
		return E_FAIL;
	}

	//--------------------------------------------------
	// IInputObject

	STDMETHODIMP HasFocusIO() {
		HWND hwnd = GetFocus();

		return hwnd == m_hcb || hwnd == m_hedt || hwnd == m_hbtn ? S_OK : S_FALSE;
	}

	STDMETHODIMP TranslateAcceleratorIO(LPMSG lpMsg) {
		int vk = lpMsg->wParam;

		if (lpMsg->message == WM_KEYDOWN && vk == VK_RETURN) {
			Command();
			return S_OK;
		} else if ((lpMsg->message == WM_KEYDOWN || lpMsg->message == WM_KEYUP) && vk == VK_BACK ||
		vk >= VK_END && vk <= VK_DOWN || vk == VK_DELETE) {
			TranslateMessage(lpMsg);
			DispatchMessage(lpMsg);
			return S_OK;
		}
		return S_FALSE;
	}

	STDMETHODIMP UIActivateIO(BOOL fActivate, LPMSG lpMsg) {
		if (fActivate) SetFocus(m_hedt);
		return S_OK;
	}

	//--------------------------------------------------
	// IDispatch

	STDMETHODIMP GetTypeInfoCount(unsigned int FAR* pctinfo) {
		return E_NOTIMPL;
	}

	STDMETHODIMP GetTypeInfo(unsigned int iTInfo, LCID lcid, ITypeInfo FAR* FAR* ppTInfo) {
		return E_NOTIMPL;
	}

	STDMETHODIMP GetIDsOfNames(REFIID riid, OLECHAR FAR* FAR* rgszNames, unsigned int cNames,
	LCID lcid, DISPID FAR* rgDispId) {
		return E_NOTIMPL;
	}

	// ブラウザのイベントを取得して処理する。
	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
	DISPPARAMS FAR* pDispParams, VARIANT FAR* pVarResult, EXCEPINFO FAR* pExcepInfo,
	unsigned int FAR* puArgErr) {
		VARIANT v;
		IDispatch *pd;
		IHTMLDocument2 *phd2;
		IHTMLDocument3 *phd3;
		IHTMLElement *phe;
		IHTMLElement2 *phe2;

		switch (dispIdMember) {
		case DISPID_DOCUMENTCOMPLETE:
			if (FAILED(m_pwb->get_Document(&pd))) break;

			// 右クリックメニューをデフォルトにする。
			v.vt = VT_DISPATCH;
			v.pdispVal = NULL;
			if (SUCCEEDED(pd->QueryInterface(IID_IHTMLDocument2, (void **)&phd2))) {
				phd2->get_body(&phe);
				if (phe != NULL) {
					if (SUCCEEDED(phe->QueryInterface(IID_IHTMLElement2, (void **)&phe2))) {
						phe2->put_oncontextmenu(v);
						phe2->Release();
					}
					phe->Release();
				}
				phd2->Release();
			}
			if (SUCCEEDED(pd->QueryInterface(IID_IHTMLDocument3, (void **)&phd3))) {
				phd3->put_oncontextmenu(v);
				phd3->Release();
			}
			pd->Release();

			break;
		case DISPID_ONQUIT:
			if (m_pcp == NULL) break;
			m_pcp->Unadvise(m_cookie);
			m_pcp->Release();
			m_pcp = NULL;
			break;
		}
		return S_OK;
	}

	//--------------------------------------------------

	void Focus(BOOL f) {
		if (m_pios != NULL) m_pios->OnFocusChangeIS((IOleWindow *)this, f);
	}

	void Command() {
		int cur = SendMessage(m_hcb, CB_GETCURSEL, 0, 0);
		TCHAR buf[1024];
		LPTSTR str;
		BSTR bstr;

		if (cur == CB_ERR ||
		(LRESULT)(str = (LPTSTR)SendMessage(m_hcb, CB_GETITEMDATA, cur, 0)) == CB_ERR) return;

		lstrcpy(buf, str);
		GetWindowText(m_hedt, buf + lstrlen(buf), sizeof(buf) / sizeof(buf[0]));
#ifdef UNICODE
		bstr = SysAllocString(buf);
#else
		WCHAR wbuf[1024];
		if (MultiByteToWideChar(CP_ACP, 0, buf, -1, wbuf, sizeof(wbuf) / sizeof(wbuf[0])) == 0) return;
		bstr = SysAllocString(wbuf);
#endif
		m_pwb->Navigate(bstr, NULL, NULL, NULL, NULL);
		SysFreeString(bstr);
	}

	static LRESULT CALLBACK procWnd(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		// 新しいウィンドウが開かれたときにDLLを共有するのでstatic IETT *としてはいけない。
		IETT *piett = (IETT *)GetWindowLong(hwnd, GWL_USERDATA);

		switch (uMsg) {
		case WM_CREATE:
			piett = (IETT *)((LPCREATESTRUCT)lParam)->lpCreateParams;
			SetWindowLong(hwnd, GWL_USERDATA, (LONG)piett);
			return 0;
		case WM_KILLFOCUS:
			piett->Focus(FALSE);
			return 0;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
			case 2:
				switch (HIWORD(wParam)) {
				case EN_SETFOCUS:
					piett->Focus(TRUE);
					break;
				}
				break;
			case 3:
				switch (HIWORD(wParam)) {
				case BN_CLICKED:
					piett->Command();
					break;
				}
				break;
			}
			return 0;
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
};

class CF : public IClassFactory {
	CLSID m_clsid;
	ULONG m_refcnt;

public:
	CF(CLSID clsid) {
		g_refcnt++;
		m_clsid = clsid;
		m_refcnt = 1;
	}

	~CF() {
		g_refcnt--;
	}

	//--------------------------------------------------
	// IUnknown

	STDMETHODIMP_(ULONG) AddRef() {
		return ++m_refcnt;
	}

	STDMETHODIMP QueryInterface(REFIID iid, void **ppvObject) {
		*ppvObject = NULL;
		if (IsEqualIID(iid, IID_IUnknown)) *ppvObject = this;
		else if (IsEqualIID(iid, IID_IClassFactory)) *ppvObject = (IClassFactory *)this;
		else return E_NOINTERFACE;

		AddRef();
		return S_OK;
	}

	STDMETHODIMP_(ULONG) Release() {
		if (--m_refcnt > 0) return m_refcnt;
		delete this;
		return 0;
	}

	//--------------------------------------------------
	// IClassFactory

	// S_OKを返したthis->QueryInterfaceの後に呼ばれる関数。
	STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject) {
		HRESULT hr;
		IETT *piett;

		if (ppvObject == NULL) return E_INVALIDARG;
		*ppvObject = NULL;

		if (pUnkOuter != NULL) return CLASS_E_NOAGGREGATION;

		if (m_clsid != my_clsid) return E_FAIL;

		piett = new IETT();
		if (piett == NULL) return E_OUTOFMEMORY;
		hr = piett->QueryInterface(riid, ppvObject);
		piett->Release();
		return hr;
	}

	STDMETHODIMP LockServer(BOOL fLock) {
		return E_NOTIMPL;
	}
};

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		g_hinst = hinstDLL;
		break;
	}
	return TRUE;
}

STDAPI DllCanUnloadNow()
{
	return g_refcnt > 0 ? S_FALSE : S_OK;
}

// Windows Shellで始めに呼び出される関数。
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv)
{
	HRESULT hr;
	CF *pcf;

	if (ppv == NULL) return E_INVALIDARG;
	*ppv = NULL;

	if (rclsid != my_clsid) return CLASS_E_CLASSNOTAVAILABLE;

	pcf = new CF(rclsid);
	if (pcf == NULL) return E_OUTOFMEMORY;
	hr = pcf->QueryInterface(riid, ppv);
	pcf->Release();
	return hr;
}

// レジストリにIEツールバーのための登録を行う。
STDAPI DllRegisterServer()
{
	TCHAR buf[MAX_PATH];
	HKEY hkey;
	HRESULT hr;

	if (RegCreateKeyEx(HKEY_CLASSES_ROOT, TEXT("CLSID\\") MY_CLSID, 0, NULL, REG_OPTION_NON_VOLATILE,
	KEY_ALL_ACCESS, NULL, &hkey, NULL) != 0) return SELFREG_E_CLASS;

	hr = SELFREG_E_CLASS;

	if (RegSetValueEx(hkey, NULL, 0, REG_SZ, (BYTE *)TEXT("IE Toolbar Test"), 15 * sizeof(TCHAR)) != 0 ||
	RegCreateKeyEx(hkey, TEXT("InProcServer32"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
	NULL, &hkey, NULL) != 0 || GetCurrentDirectory(sizeof(buf) / sizeof(buf[0]), buf) == 0)
		goto procend;

	if (buf[lstrlen(buf) - 1] != TEXT('\\') && lstrcat(buf, TEXT("\\")) == 0) goto procend;

	if (lstrcat(buf, TEXT("iett.dll")) == 0 ||
	RegSetValueEx(hkey, NULL, 0, REG_SZ, (BYTE *)buf, lstrlen(buf) * sizeof(TCHAR)) != 0 ||
	RegSetValueEx(hkey, TEXT("ThreadingModel"), 0, REG_SZ, (BYTE *)TEXT("Apartment"), 9 * sizeof(TCHAR)) != 0) goto procend;

	RegCloseKey(hkey);

	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Internet Explorer\\Toolbar"),
	0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL) != 0) return SELFREG_E_CLASS;

	if (RegSetValueEx(hkey, MY_CLSID, 0, REG_BINARY, NULL, 0) != 0) goto procend;

	hr = S_OK;

procend:
	RegCloseKey(hkey);
	return hr;
}

// レジストリからIEツールバーのための登録情報を削除する。
STDAPI DllUnregisterServer()
{
	HKEY hkey;
	HRESULT hr;

	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, TEXT("CLSID"), 0, KEY_ALL_ACCESS, &hkey) != 0) return S_OK;
	hr = SELFREG_E_CLASS;
	if (SHDeleteKey(hkey, MY_CLSID) != 0) goto procend;
	RegCloseKey(hkey);

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Internet Explorer\\Toolbar"),
	0, KEY_ALL_ACCESS, &hkey) != 0) return S_OK;
	hr = SELFREG_E_CLASS;
	if (RegDeleteValue(hkey, MY_CLSID) != 0) goto procend;

	hr = S_OK;

procend:
	RegCloseKey(hkey);
	return hr;
}
