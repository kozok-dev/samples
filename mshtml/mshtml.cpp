#include <windows.h>
#include <shlwapi.h>
#include <mshtml.h>
#include <mshtmdid.h>
#define Pix2Him(size, w, h, hdc) size.cx = (w) * 2540 / GetDeviceCaps(hdc, LOGPIXELSX); \
	size.cy = (h) * 2540 / GetDeviceCaps(hdc, LOGPIXELSY);
#define Him2Pix(size_out, size, hdc) size_out.cx = size.cx * GetDeviceCaps(hdc, LOGPIXELSX) / 2540; \
	size_out.cy = size.cy * GetDeviceCaps(hdc, LOGPIXELSY) / 2540;

class MhTest : public IOleClientSite, public IOleInPlaceFrame, public IOleInPlaceSite, public IDispatch {
	ULONG m_refcnt;
	DWORD m_oac, m_dac, m_cookie;
	RECT m_rect;
	HWND m_hwnd;
	IUnknown *m_pu;
	IConnectionPoint *m_pcp;
	IOleObject *m_poo;
	IViewObject *m_pvo;
	IOleInPlaceObject *m_poipo;
	IOleInPlaceActiveObject *m_poipao;

public:
	MhTest() {
		m_refcnt = 1;	// newのときはAddRefしたことにする。
		m_oac = m_dac = m_cookie = 0;
		m_rect.left = m_rect.top = m_rect.right = m_rect.bottom = 0;
		m_hwnd = NULL;
		m_pu = NULL;
		m_pcp = NULL;
		m_poo = NULL;
		m_pvo = NULL;
		m_poipo = NULL;
		m_poipao = NULL;
		CoInitialize(NULL);
	}

	~MhTest() {
		CoUninitialize();
	}

	// 各種OLE設定とウィンドウ作成。
	bool Create(HINSTANCE hinst) {
		bool flag;
		WNDCLASS wc;
		SIZEL sl;
		HDC hdc;
		BSTR bstr;
		IConnectionPointContainer *pcpc;
		IPersistStreamInit *pps;
		IHTMLDocument2 *phd;
		IHTMLElement *phe;
		IHTMLStyle *phs;

		if (m_hwnd != NULL) return false;
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
		wc.lpszClassName = TEXT("mhtest");
		if (RegisterClass(&wc) == 0) return false;

		if ((m_hwnd = CreateWindow(wc.lpszClassName, TEXT("MSHTML Test"), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		200, 150, 300, 300, NULL, NULL, hinst, this)) == NULL) return false;

		// HTMLのOLEを読み込む。
		if (FAILED(CoCreateInstance(CLSID_HTMLDocument, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID *)&m_pu)))
			return false;

		// HTMLの内容を初期化。
		if (SUCCEEDED(m_pu->QueryInterface(IID_IPersistStreamInit, (void **)&pps))) {
			if (SUCCEEDED(pps->InitNew()) &&
			SUCCEEDED(m_pu->QueryInterface(IID_IHTMLDocument2, (void **)&phd))) {
				phd->get_body(&phe);
				if (phe != NULL) {
					// スタイルシート設定。
					if (SUCCEEDED(phe->get_style(&phs))) {
						bstr = SysAllocString(L"none");
						phs->put_borderStyle(bstr);
						SysFreeString(bstr);

						bstr = SysAllocString(L"default");
						phs->put_cursor(bstr);
						SysFreeString(bstr);

						bstr = SysAllocString(L"auto");
						phs->put_overflow(bstr);
						SysFreeString(bstr);

						phs->Release();
					}

					phe->put_innerHTML(L"MSHTML Test <a href=\"\" fid=\"1\">Notepad</a>");
					phe->Release();
				}
				phd->Release();
			}
			pps->Release();
		}
		// イベントを受け取れるようにする。
		if (SUCCEEDED(m_pu->QueryInterface(IID_IConnectionPointContainer, (void **)&pcpc))) {
			if (SUCCEEDED(pcpc->FindConnectionPoint(DIID_HTMLDocumentEvents2, &m_pcp)))
				m_pcp->Advise((IDispatch *)this, &m_cookie);
			pcpc->Release();
		}

		// IViewObject設定。
		m_pu->QueryInterface(IID_IViewObject, (void **)&m_pvo);

		// IOleObject設定。
		if (FAILED(m_pu->QueryInterface(IID_IOleObject, (void **)&m_poo))) return false;
		m_poo->SetClientSite(this);

		// インプレースアクティベーション。
		GetClientRect(m_hwnd, &m_rect);
		hdc = ::GetDC(m_hwnd);
		Pix2Him(sl, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top, hdc);
		m_poo->SetExtent(DVASPECT_CONTENT, &sl);
		m_poo->GetExtent(DVASPECT_CONTENT, &sl);
		Him2Pix(sl, sl, hdc);
		::ReleaseDC(m_hwnd, hdc);
		m_rect.right = m_rect.left + sl.cx;
		m_rect.bottom = m_rect.top + sl.cy;
		m_poo->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, this, 0, m_hwnd, &m_rect);
		RedrawWindow(m_hwnd, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW);

		return true;
	}

	// 描画処理。
	void onPaint(HDC hdc) {
		if (m_pvo != NULL) m_pvo->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hdc, (LPRECTL)&m_rect, (LPRECTL)&m_rect, NULL, 0);
	}

	// ウィンドウサイズ変更時。
	void onSize() {
		RECT rect;
		SIZEL sl;
		HDC hdc;

		GetClientRect(m_hwnd, &rect);

		m_rect.right = m_rect.left + (rect.right - rect.left);
		m_rect.bottom = m_rect.top + (rect.bottom - rect.top);

		if (m_poo != NULL) {
			hdc = ::GetDC(m_hwnd);
			Pix2Him(sl, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top, hdc);
			::ReleaseDC(m_hwnd, hdc);
			m_poo->SetExtent(DVASPECT_CONTENT, &sl);
		}

		if (m_poipo == NULL) RedrawWindow(m_hwnd, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW);
		else m_poipo->SetObjectRects(&m_rect, &m_rect);
	}

	// 各種オブジェクト終了処理。
	void onDestroy() {
		if (m_pvo != NULL) m_pvo->Release();
		if (m_poo != NULL) {
			m_poo->Close(OLECLOSE_NOSAVE);
			m_poo->SetClientSite(NULL);
			m_poo->Release();
		}
		if (m_pcp != NULL) {
			m_pcp->Unadvise(m_cookie);
			m_pcp->Release();
		}
		if (m_poipao != NULL) m_poipao->Release();
		if (m_pu != NULL) m_pu->Release();
	}

	// ウィンドウプロシージャ。
	static LRESULT CALLBACK procWnd(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		static MhTest *pmt;
		HDC hdc;
		PAINTSTRUCT ps;

		switch (uMsg) {
		case WM_CREATE:
			pmt = (MhTest *)((LPCREATESTRUCT)lParam)->lpCreateParams;
			return 0;
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			pmt->onPaint(hdc);
			EndPaint(hwnd, &ps);
			return 0;
		case WM_SIZE:
			pmt->onSize();
			return 0;
		case WM_DESTROY:
			pmt->onDestroy();
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	//--------------------------------------------------
	// IUnknown

	STDMETHODIMP QueryInterface(REFIID iid, void **ppvObject) {
		if (iid == IID_IUnknown) *ppvObject = this;
		else if (iid == IID_IOleClientSite) *ppvObject = (IOleClientSite *)this;
		else if (iid == IID_IOleWindow) *ppvObject = (IOleInPlaceUIWindow *)this;
		else if (iid == IID_IOleInPlaceUIWindow) *ppvObject = (IOleInPlaceUIWindow *)this;
		else if (iid == IID_IOleInPlaceFrame) *ppvObject = (IOleInPlaceFrame *)this;
		else if (iid == IID_IOleInPlaceSite) *ppvObject = (IOleInPlaceSite *)this;
		else if (iid == IID_IDispatch) *ppvObject = (IDispatch *)this;
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
	// IOleClientSite

	STDMETHODIMP SaveObject() {
		return E_NOTIMPL;
	}

	STDMETHODIMP GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk) {
		return E_NOTIMPL;
	}

	STDMETHODIMP GetContainer(LPOLECONTAINER FAR *ppContainer) {
		return E_NOTIMPL;
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
		return E_NOTIMPL;
	}

	STDMETHODIMP RequestNewObjectLayout() {
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
		return S_OK;
	}

	STDMETHODIMP RequestBorderSpace(LPCBORDERWIDTHS pborderwidths) {
		return INPLACE_E_NOTOOLSPACE;
	}

	STDMETHODIMP SetBorderSpace(LPCBORDERWIDTHS pborderwidths) {
		return S_OK;
	}

	STDMETHODIMP SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName) {
		m_poipao = pActiveObject;
		return S_OK;
	}

	//--------------------------------------------------
	// IOleInPlaceFrame

	STDMETHODIMP InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths) {
		return S_OK;
	}

	STDMETHODIMP SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject) {
		return S_OK;
	}

	STDMETHODIMP RemoveMenus(HMENU hmenuShared) {
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
		m_pu->QueryInterface(IID_IOleInPlaceObject, (void **)&m_poipo);
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

		if (lprcPosRect != NULL) GetClientRect(m_hwnd, lprcPosRect);
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
		return S_OK;
	}

	STDMETHODIMP OnInPlaceDeactivate() {
		if (m_poipo == NULL) return S_OK;
		m_poipo->Release();
		m_poipo = NULL;
		return S_OK;
	}

	STDMETHODIMP DiscardUndoState() {
		return E_NOTIMPL;
	}

	STDMETHODIMP DeactivateAndUndo() {
		return E_NOTIMPL;
	}

	STDMETHODIMP OnPosRectChange(LPCRECT lprcPosRect) {
		return E_NOTIMPL;
	}

	//--------------------------------------------------
	// IDispatch

	STDMETHODIMP GetTypeInfoCount(unsigned int FAR *pctinfo) {
		return E_NOTIMPL;
	}

	STDMETHODIMP GetTypeInfo(unsigned int iTInfo, LCID lcid, ITypeInfo FAR* FAR* ppTInfo) {
		return E_NOTIMPL;
	}

	STDMETHODIMP GetIDsOfNames(REFIID riid, OLECHAR FAR* FAR* rgszNames, unsigned int cNames,
	LCID lcid, DISPID FAR *rgDispId) {
		return E_NOTIMPL;
	}

	// イベント処理。
	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS FAR *pDispParams,
	VARIANT FAR *pVarResult, EXCEPINFO FAR *pExcepInfo, unsigned int FAR *puArgErr) {
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		BSTR bstr;
		VARIANT v;
		IHTMLElement *phe;
		IHTMLEventObj *pheo;

		switch (dispIdMember) {
		case DISPID_HTMLDOCUMENTEVENTS2_ONCLICK:
			if (pDispParams == NULL || pDispParams->cArgs < 1 ||
			pDispParams->rgvarg[0].vt != VT_DISPATCH || pDispParams->rgvarg[0].pdispVal == NULL ||
			FAILED(pDispParams->rgvarg[0].pdispVal->QueryInterface(IID_IHTMLEventObj, (void **)&pheo))) break;

			if (SUCCEEDED(pheo->get_srcElement(&phe))) {
				//--- 特定の属性がある場合、その値に応じて処理を行う。

				bstr = SysAllocString(L"fid");
				phe->getAttribute(bstr, 0, &v);
				SysFreeString(bstr);
				phe->Release();

				if (v.vt == VT_BSTR) {
					switch (_wtoi(v.bstrVal)) {
					case 1:
						ZeroMemory(&si, sizeof(si));
						si.cb = sizeof(STARTUPINFO);
						CreateProcess(NULL, TEXT("notepad"), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
						CloseHandle(pi.hThread);
						CloseHandle(pi.hProcess);
						break;
					}
				}
			}
			pheo->Release();
		case DISPID_HTMLDOCUMENTEVENTS2_ONCONTEXTMENU:
		case DISPID_HTMLDOCUMENTEVENTS2_ONSELECTSTART:
			if (pVarResult == NULL) break;
			// 無効化する。
			pVarResult->vt = VT_BOOL;
			pVarResult->boolVal = VARIANT_FALSE;
			break;
		}
		return S_OK;
	}
};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	MhTest *pmt;

	pmt = new MhTest();
	if (pmt->Create(hInstance) == NULL) return 0;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
