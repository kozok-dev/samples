#include <stdio.h>
#include <windows.h>

void main()
{
	HMODULE dll=LoadLibrary("hook.dll");
	FARPROC setHook=GetProcAddress(dll,"setHook");
	FARPROC unHook=GetProcAddress(dll,"unHook");

	printf("Hook test program");
	setHook();
	getchar();
	unHook();
}