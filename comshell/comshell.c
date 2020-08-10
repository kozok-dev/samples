#include <objbase.h>
#include <shlobj.h>

void main()
{
	IShellDispatch *isd;

	CoInitialize(NULL);

	if (CoCreateInstance(&CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, &IID_IShellDispatch, (void **)&isd) == S_OK) {
		isd->lpVtbl->FileRun(isd);
		isd->lpVtbl->Release(isd);
	}

	CoUninitialize();
}
