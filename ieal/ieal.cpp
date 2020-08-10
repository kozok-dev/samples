#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <mshtml.h>
#include <exdispid.h>
#define MY_CLSID TEXT("{019C9404-45C8-45E0-B836-E54FB775F1FD}")
const CLSID my_clsid = {0x19c9404, 0x45c8, 0x45e0, {0xb8, 0x36, 0xe5, 0x4f, 0xb7, 0x75, 0xf1, 0xfd}};

UINT g_refcnt;
HINSTANCE g_hinst;
bool g_flag = false;

class Work : public IObjectWithSite, public IDispatch {
	ULONG m_refcnt;
	DWORD m_cookie;
	IWebBrowser2 *m_pwb;
	IConnectionPoint *m_pcp;

public:
	Work() {
		g_refcnt++;
		m_refcnt = 1;
		m_cookie = 0;
		m_pwb = NULL;
		m_pcp = NULL;
	}

	~Work() {
		g_refcnt--;
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
		else if (iid == IID_IObjectWithSite) *ppvObject = (IObjectWithSite *)this;
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
	// IObjectWithSite

	STDMETHODIMP SetSite(IUnknown *pUnkSite) {
		HRESULT hr;
		BSTR bstr;
		IServiceProvider *psp;
		IConnectionPointContainer *pcpc;

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

		if (FAILED(pUnkSite->QueryInterface(IID_IServiceProvider, (void **)&psp))) return E_FAIL;
		hr = psp->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void **)&m_pwb);
		psp->Release();

		if (FAILED(hr) ||
		FAILED(m_pwb->QueryInterface(IID_IConnectionPointContainer, (void **)&pcpc))) return E_FAIL;
		hr = pcpc->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_pcp);
		pcpc->Release();
		if (FAILED(hr)) return E_FAIL;
		m_pcp->Advise((IDispatch *)this, &m_cookie);

		bstr = SysAllocString(L"http://localhost/");
		m_pwb->Navigate(bstr, NULL, NULL, NULL, NULL);
		SysFreeString(bstr);

		return S_OK;
	}

	STDMETHODIMP GetSite(REFIID riid, void **ppvSite) {
		if (ppvSite == NULL) return E_FAIL;
		*ppvSite = NULL;
		return E_FAIL;
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

	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
	DISPPARAMS FAR* pDispParams, VARIANT FAR* pVarResult, EXCEPINFO FAR* pExcepInfo,
	unsigned int FAR* puArgErr) {
		BSTR bstr;
		VARIANT v1, v2;
		IDispatch *pd1, *pd2;
		IHTMLDocument2 *phd2;
		IHTMLElementCollection *phec;
		IHTMLInputElement *phie;
		IHTMLFormElement *phfe;

		switch (dispIdMember) {
		case DISPID_DOCUMENTCOMPLETE:
			if (FAILED(m_pwb->get_Document(&pd1))) break;

			if (SUCCEEDED(pd1->QueryInterface(IID_IHTMLDocument2, (void **)&phd2))) {
				if (SUCCEEDED(phd2->get_all(&phec))) {
					v1.vt = VT_BSTR;
					v2.vt = VT_I4;
					v2.lVal = 0;

					v1.bstrVal = SysAllocString(L"cid");
					phec->item(v1, v2, &pd2);
					SysFreeString(v1.bstrVal);
					if (pd2 != NULL) {
						if (SUCCEEDED(pd2->QueryInterface(IID_IHTMLInputElement, (void **)&phie))) {
							bstr = SysAllocString(L"123");
							phie->put_value(bstr);
							SysFreeString(bstr);
							phie->Release();
						}
						pd2->Release();
					}

					v1.bstrVal = SysAllocString(L"pid");
					phec->item(v1, v2, &pd2);
					SysFreeString(v1.bstrVal);
					if (pd2 != NULL) {
						if (SUCCEEDED(pd2->QueryInterface(IID_IHTMLInputElement, (void **)&phie))) {
							bstr = SysAllocString(L"456");
							phie->put_value(bstr);
							SysFreeString(bstr);
							phie->Release();
						}
						pd2->Release();
					}

					v1.bstrVal = SysAllocString(L"password");
					phec->item(v1, v2, &pd2);
					SysFreeString(v1.bstrVal);
					if (pd2 != NULL) {
						if (SUCCEEDED(pd2->QueryInterface(IID_IHTMLInputElement, (void **)&phie))) {
							bstr = SysAllocString(L"789");
							phie->put_value(bstr);
							SysFreeString(bstr);
							phie->Release();
						}
						pd2->Release();
					}

					v1.bstrVal = SysAllocString(L"form");
					phec->item(v1, v2, &pd2);
					SysFreeString(v1.bstrVal);
					if (pd2 != NULL) {
						if (SUCCEEDED(pd2->QueryInterface(IID_IHTMLFormElement, (void **)&phfe))) {
							phfe->submit();
							phfe->Release();
						}
						pd2->Release();
					}

					phec->Release();
				}
				phd2->Release();
			}
			pd1->Release();

			if (m_pcp != NULL) {
				m_pcp->Unadvise(m_cookie);
				m_pcp->Release();
				m_pcp = NULL;
			}
			if (m_pwb != NULL) {
				m_pwb->Release();
				m_pwb = NULL;
			}
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
		if (iid == IID_IUnknown) *ppvObject = this;
		else if (iid == IID_IClassFactory) *ppvObject = (IClassFactory *)this;
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
	// IClassFactory

	STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject) {
		HRESULT hr;

		Work *pwork;

		if (ppvObject == NULL) return E_INVALIDARG;
		*ppvObject = NULL;

		if (pUnkOuter != NULL) return CLASS_E_NOAGGREGATION;

		if (!IsEqualIID(m_clsid, my_clsid)) return E_FAIL;

		pwork = new Work();
		if (pwork == NULL) return E_OUTOFMEMORY;
		hr = pwork->QueryInterface(riid, ppvObject);
		pwork->Release();
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

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv)
{
	HRESULT hr;
	CF *pcf;

	if (ppv == NULL) return E_INVALIDARG;
	*ppv = NULL;

	if (g_flag) return E_FAIL;
	g_flag = true;

	if (!IsEqualCLSID(rclsid, my_clsid)) return CLASS_E_CLASSNOTAVAILABLE;

	pcf = new CF(rclsid);
	if (pcf == NULL) return E_OUTOFMEMORY;
	hr = pcf->QueryInterface(riid, ppv);
	pcf->Release();
	return hr;
}

STDAPI DllRegisterServer()
{
	TCHAR buf[MAX_PATH];
	HKEY hkey;
	HRESULT hr;

	if (RegCreateKeyEx(HKEY_CLASSES_ROOT, TEXT("CLSID\\") MY_CLSID, 0, NULL, REG_OPTION_NON_VOLATILE,
	KEY_ALL_ACCESS, NULL, &hkey, NULL) != 0) return SELFREG_E_CLASS;

	hr = SELFREG_E_CLASS;

	if (RegSetValueEx(hkey, NULL, 0, REG_SZ, (BYTE *)TEXT("Work"), 15 * sizeof(TCHAR)) != 0 ||
	RegCreateKeyEx(hkey, TEXT("InProcServer32"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
	NULL, &hkey, NULL) != 0 || GetCurrentDirectory(sizeof(buf) / sizeof(buf[0]), buf) == 0)
		goto procend;

	if (buf[lstrlen(buf) - 1] != TEXT('\\') && lstrcat(buf, TEXT("\\")) == 0) goto procend;

	if (lstrcat(buf, TEXT("work.dll")) == 0 ||
	RegSetValueEx(hkey, NULL, 0, REG_SZ, (BYTE *)buf, lstrlen(buf) * sizeof(TCHAR)) != 0 ||
	RegSetValueEx(hkey, TEXT("ThreadingModel"), 0, REG_SZ, (BYTE *)TEXT("Apartment"), 9 * sizeof(TCHAR)) != 0) goto procend;

	RegCloseKey(hkey);

	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Internet Explorer\\Extensions\\") MY_CLSID,
	0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL) != 0) return SELFREG_E_CLASS;

	if (lstrcat(buf, TEXT(",1")) == 0 ||
	RegSetValueEx(hkey, TEXT("Default Visible"), 0, REG_SZ, (BYTE *)TEXT("Yes"), 3 * sizeof(TCHAR)) != 0 ||
	RegSetValueEx(hkey, TEXT("ButtonText"), 0, REG_SZ, (BYTE *)TEXT("Work"), 4 * sizeof(TCHAR)) != 0 ||
	RegSetValueEx(hkey, TEXT("HotIcon"), 0, REG_SZ, (BYTE *)buf, lstrlen(buf) * sizeof(TCHAR)) != 0 ||
	RegSetValueEx(hkey, TEXT("Icon"), 0, REG_SZ, (BYTE *)buf, lstrlen(buf) * sizeof(TCHAR)) != 0 ||
	RegSetValueEx(hkey, TEXT("CLSID"), 0, REG_SZ, (BYTE *)TEXT("{1FBA04EE-3024-11D2-8F1F-0000F87ABD16}"), 38 * sizeof(TCHAR)) != 0 ||
	RegSetValueEx(hkey, TEXT("ClsidExtension"), 0, REG_SZ, (BYTE *)MY_CLSID, sizeof(MY_CLSID)) != 0) goto procend;

	hr = S_OK;

procend:
	RegCloseKey(hkey);
	return hr;
}

STDAPI DllUnregisterServer()
{
	HKEY hkey;
	HRESULT hr;

	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, TEXT("CLSID"), 0, KEY_ALL_ACCESS, &hkey) != 0) return S_OK;
	hr = SELFREG_E_CLASS;
	if (SHDeleteKey(hkey, MY_CLSID) != 0) goto procend;
	RegCloseKey(hkey);

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Internet Explorer\\Extensions"),
	0, KEY_ALL_ACCESS, &hkey) != 0) return S_OK;
	hr = SELFREG_E_CLASS;
	if (SHDeleteKey(hkey, MY_CLSID) != 0) goto procend;

	hr = S_OK;

procend:
	RegCloseKey(hkey);
	return hr;
}
