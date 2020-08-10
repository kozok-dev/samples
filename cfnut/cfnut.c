#define UNICODE
#define _UNICODE
#include <tchar.h>
#include <windows.h>

void _tmain(int argc, TCHAR **argv)
{
	int i;
	TCHAR filepath1[MAX_PATH], filepath2[MAX_PATH], filepath3[MAX_PATH], filename[MAX_PATH], *ext;
	HANDLE dir, file;
	WIN32_FIND_DATA wfd;
	FILETIME ft;
	SYSTEMTIME st;

	if (argc != 2) return;

	lstrcpy(filepath1, argv[1]);
	if (filepath1[lstrlen(filepath1) - 1] != TEXT('\\')) lstrcat(filepath1, TEXT("\\"));
	lstrcat(filepath1, TEXT("*"));
	dir = FindFirstFile(filepath1, &wfd);
	if (dir == INVALID_HANDLE_VALUE) return;

	do {
		if (lstrcmp(wfd.cFileName, TEXT(".")) == 0 || lstrcmp(wfd.cFileName, TEXT("..")) == 0) continue;
		lstrcpy(filepath1, argv[1]);
		if (filepath1[lstrlen(filepath1) - 1] != TEXT('\\')) lstrcat(filepath1, TEXT("\\"));
		lstrcat(filepath1, wfd.cFileName);
		if (!FileTimeToLocalFileTime(&wfd.ftLastWriteTime, &ft)) continue;
		if (!FileTimeToSystemTime(&ft, &st)) continue;

		lstrcpy(filepath2, argv[1]);
		if (filepath2[lstrlen(filepath2) - 1] != TEXT('\\')) lstrcat(filepath2, TEXT("\\"));
		_stprintf(filename, TEXT("%d%02d%02d_%02d%02d%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		ext = _tcsrchr(wfd.cFileName, TEXT('.'));

		for (i = 1; ; i++) {
			lstrcpy(filepath3, filepath2);
			lstrcat(filepath3, filename);
			if (i > 1) _stprintf(filepath3 + lstrlen(filepath3), TEXT("_%d"), i);
			if (ext != NULL) lstrcat(filepath3, _tcslwr(ext));

			if (lstrcmpi(filepath1, filepath3) == 0) break;

			file = CreateFile(filepath3, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if (file == INVALID_HANDLE_VALUE) {
				MoveFile(filepath1, filepath3);
				break;
			}
			CloseHandle(file);
		}
	} while (FindNextFile(dir, &wfd));

	FindClose(dir);
}
