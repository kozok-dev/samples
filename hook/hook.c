#include <windows.h>

HINSTANCE instdll;
#pragma data_seg(".sha")
HHOOK hook=0;
#pragma data_seg()

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	if(fdwReason==DLL_PROCESS_ATTACH) instdll=hinstDLL;
	return TRUE;
}

LRESULT CALLBACK Hook(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(nCode==HC_ACTION) {
		CWPSTRUCT *msg=(CWPSTRUCT *)lParam;
		if(msg->message==WM_COPY) {
			MessageBox(msg->hwnd,"WM_COPY","Message",0);
		}
	}
	return CallNextHookEx(hook,nCode,wParam,lParam);
}

void WINAPI setHook()
{
	hook=SetWindowsHookEx(WH_CALLWNDPROC,Hook,instdll,0);
}

void WINAPI unHook()
{
	UnhookWindowsHookEx(hook);
}