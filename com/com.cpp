#include <windows.h>
#include <objbase.h>
#include <unknwn.h>
#include <olectl.h>

// Globals
HINSTANCE hinstDLL = NULL;
long refCnt = 0;

// Implemented categories
#define StrIID_SafetyScr "{7dd95801-9882-11cf-9fa9-00aa006c42c4}"
#define StrIID_SafetyIni "{7dd95802-9882-11cf-9fa9-00aa006c42c4}"

// My interface
#define StrIID_ITest "{d1fca0e1-823e-4585-ac9f-2f847d6276f1}"
const IID IID_ITest = {0xd1fca0e1, 0x823e, 0x4585, {0xac, 0x9f, 0x2f, 0x84, 0x7d, 0x62, 0x76, 0xf1}};
interface ITest : public IUnknown {
	virtual void MsgBox() = 0;
};

//-------------------- "Test" Class [
class Test : public IDispatch, ITest {
	long refCnt;

public:
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *pctinfo);
	HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
	HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, OLECHAR **rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

	Test();
	void MsgBox();
};

HRESULT STDMETHODCALLTYPE Test::QueryInterface(REFIID riid, void **ppvObject)
{
	if (riid == IID_IUnknown || riid == IID_IDispatch) {
		*ppvObject = (IDispatch *)this;
	} else {
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE Test::AddRef()
{
	InterlockedIncrement(&refCnt);
	return refCnt;
}

ULONG STDMETHODCALLTYPE Test::Release()
{
	InterlockedDecrement(&refCnt);
	if (refCnt > 0) return refCnt;
	delete this;
	return 0;
}

HRESULT STDMETHODCALLTYPE Test::GetTypeInfoCount(UINT *pctinfo)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Test::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Test::GetIDsOfNames(REFIID riid, OLECHAR **rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	// Check method name
	for (UINT i = 0; i < cNames; i++) {
		if (!lstrcmpiW(rgszNames[i], L"MsgBox")) {
			rgDispId[i] = 1;
			return S_OK;
		} else if (!lstrcmpiW(rgszNames[i], L"ArgTest")) {
			rgDispId[i] = 2;
			return S_OK;
		} else if (!lstrcmpiW(rgszNames[i], L"StrTest")) {
			rgDispId[i] = 3;
			return S_OK;
		} else if (!lstrcmpiW(rgszNames[i], L"ArrayTest")) {
			rgDispId[i] = 4;
			return S_OK;
		} else {
			rgDispId[i] = DISPID_UNKNOWN;
		}
	}
	return DISP_E_UNKNOWNNAME;
}

HRESULT STDMETHODCALLTYPE Test::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	// Run method
	switch (dispIdMember) {
	case 1:	// MsgBox
		MsgBox();
		return S_OK;
	case 2:	// ArgTest
		// Arguments
		char arglist[512], param[256];
		arglist[0] = NULL;
		for (UINT i = 0; i < pDispParams->cArgs; i++) {
			WideCharToMultiByte(CP_ACP, 0, pDispParams->rgvarg[i].bstrVal, -1, param, sizeof(param), NULL, NULL);
			lstrcat(arglist, param);
			lstrcat(arglist, "\n");
		}

		MessageBox(NULL, arglist, "Interface-TestEX", MB_OK);

		// Return value
		if (pVarResult) {
			V_VT(pVarResult) = VT_INT;
			V_INT(pVarResult) = 1234;
		}

		return S_OK;
	case 3:	// StrTest
		// Return value
		if (pVarResult) {
			V_VT(pVarResult) = VT_BSTR;
			V_BSTR(pVarResult) = L"StrTest is succeeded.";
		}
		return S_OK;
	case 4:	// ArrayTest
		SAFEARRAY *psa;
		SAFEARRAYBOUND sab[1];
		BSTR *pbstr;
		HRESULT hr;

		// Create array
		sab[0].lLbound = 0;
		sab[0].cElements = 5;
		if ((psa = SafeArrayCreate(VT_BSTR, 1, sab)) == NULL) return E_OUTOFMEMORY;

		// Write array
		if (FAILED(hr = SafeArrayAccessData(psa, (void **)&pbstr))) {
			SafeArrayDestroy(psa);
			return hr;
		}
		pbstr[0] = L"Array";
		pbstr[1] = L"Test";
		pbstr[2] = L"ArrayTest";
		pbstr[3] = L"COM";
		pbstr[4] = L"COMTest";
		SafeArrayUnaccessData(psa);

		// Return value
		if (pVarResult) {
			V_VT(pVarResult) = VT_ARRAY | VT_BSTR;
			V_ARRAY(pVarResult) = psa;
		}
		return S_OK;
	}
	return DISP_E_MEMBERNOTFOUND;
}

Test::Test()
{
	refCnt = 0;
}

void Test::MsgBox()
{
	MessageBox(NULL, "Hello COM", "Interface-Test", MB_OK);
}
//-------------------- "Test" Class ]

//-------------------- "TestFactory" Class [
class TestFactory : public IClassFactory {
	long refCnt;

public:
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	HRESULT STDMETHODCALLTYPE CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, void **ppvObject);
	HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock);

	TestFactory();
	~TestFactory();
};

HRESULT STDMETHODCALLTYPE TestFactory::QueryInterface(REFIID riid, void **ppvObject)
{
	if (riid == IID_IUnknown || riid == IID_IClassFactory) {
		*ppvObject = (IClassFactory *)this;
	} else {
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}

ULONG STDMETHODCALLTYPE TestFactory::AddRef()
{
	InterlockedIncrement(&refCnt);
	return refCnt;
}

ULONG STDMETHODCALLTYPE TestFactory::Release()
{
	InterlockedDecrement(&refCnt);
	if (refCnt > 0) return refCnt;
	delete this;
	return 0;
}

HRESULT STDMETHODCALLTYPE TestFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, void **ppvObject)
{
	Test *test;

	*ppvObject = NULL;

	if (pUnkOuter) return CLASS_E_NOAGGREGATION;

	test = new Test();
	if (test == NULL) return E_OUTOFMEMORY;
	return test->QueryInterface(riid, ppvObject);
}

HRESULT STDMETHODCALLTYPE TestFactory::LockServer(BOOL fLock)
{
	return S_OK;
}

TestFactory::TestFactory()
{
	InterlockedIncrement(&::refCnt);
	refCnt = 0;
}

TestFactory::~TestFactory()
{
	InterlockedDecrement(&::refCnt);
}
//-------------------- "TestFactory" Class ]


//--- DLL

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		::hinstDLL = hinstDLL;
		DisableThreadLibraryCalls(hinstDLL);
	}
	return TRUE;
}

STDAPI DllCanUnloadNow()
{
	return refCnt > 0 ? S_FALSE : S_OK;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppvObject)
{
	*ppvObject = NULL;
	if (rclsid == IID_ITest) {
		TestFactory *test = new TestFactory();
		return test->QueryInterface(riid, ppvObject);
	}
	return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllRegisterServer()
{
	HKEY hkey;
	char dllpath[MAX_PATH];

	GetModuleFileName(hinstDLL, dllpath, sizeof(dllpath));

	RegCreateKeyEx(
		HKEY_CLASSES_ROOT,
		"clsid\\" StrIID_ITest "\\inprocserver32",
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hkey,
		NULL
	);
	RegSetValueEx(
		hkey,
		NULL,
		0,
		REG_SZ,
		(BYTE *)dllpath,
		lstrlen(dllpath) + 1
	);
	RegCloseKey(hkey);

	RegCreateKeyEx(
		HKEY_CLASSES_ROOT,
		"clsid\\" StrIID_ITest "\\implemented categories\\" StrIID_SafetyScr,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hkey,
		NULL
	);
	RegCloseKey(hkey);

	RegCreateKeyEx(
		HKEY_CLASSES_ROOT,
		"clsid\\" StrIID_ITest "\\implemented categories\\" StrIID_SafetyIni,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hkey,
		NULL
	);
	RegCloseKey(hkey);

	RegCreateKeyEx(
		HKEY_CLASSES_ROOT,
		"COMTest\\clsid",
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hkey,
		NULL
	);
	RegSetValueEx(
		hkey,
		NULL,
		0,
		REG_SZ,
		(BYTE *)StrIID_ITest,
		lstrlen(StrIID_ITest) + 1
	);
	RegCloseKey(hkey);

	return S_OK;
}

STDAPI DllUnregisterServer()
{
	HKEY hkey;

	// Delete "InprocServer32" key
	RegOpenKeyEx(
		HKEY_CLASSES_ROOT,
		"clsid\\" StrIID_ITest,
		0,
		KEY_ALL_ACCESS,
		&hkey
	);
	RegDeleteKey(
		hkey,
		"inprocserver32"
	);
	RegCloseKey(hkey);

	// Delete "Implemented Categories"'s sub key
	RegOpenKeyEx(
		HKEY_CLASSES_ROOT,
		"clsid\\" StrIID_ITest "\\implemented categories",
		0,
		KEY_ALL_ACCESS,
		&hkey
	);
	RegDeleteKey(
		hkey,
		StrIID_SafetyScr
	);
	RegDeleteKey(
		hkey,
		StrIID_SafetyIni
	);
	RegCloseKey(hkey);

	// Delete "Implemented Categories" key
	RegOpenKeyEx(
		HKEY_CLASSES_ROOT,
		"clsid\\" StrIID_ITest,
		0,
		KEY_ALL_ACCESS,
		&hkey
	);
	RegDeleteKey(
		hkey,
		"implemented categories"
	);
	RegCloseKey(hkey);

	// Delete this CLSID key
	RegOpenKeyEx(
		HKEY_CLASSES_ROOT,
		"clsid",
		0,
		KEY_ALL_ACCESS,
		&hkey
	);
	RegDeleteKey(
		hkey,
		StrIID_ITest
	);
	RegCloseKey(hkey);

	// Delete this ProgID key
	RegOpenKeyEx(
		HKEY_CLASSES_ROOT,
		"clsid",
		0,
		KEY_ALL_ACCESS,
		&hkey
	);
	RegDeleteKey(
		hkey,
		"COMTest"
	);
	RegCloseKey(hkey);

	return S_OK;
}
