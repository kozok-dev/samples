#define UNICODE
#define _UNICODE
#include <stdio.h>
#include <locale.h>
#include <tchar.h>
#include <windows.h>

typedef struct {
	TCHAR name[MAX_PATH];
	DWORD size;
	char flag;
} FILE_LIST;
FILE_LIST *g_file_list = NULL;
DWORD g_file_list_count = 0;

void setFileList(LPCTSTR dirname)
{
	TCHAR filepath[MAX_PATH];
	DWORD size, size_hi, i;
	HANDLE dir, file;
	WIN32_FIND_DATA wfd;
	FILE_LIST *file_list, tmp;

	lstrcpy(filepath, dirname);
	if (filepath[lstrlen(filepath) - 1] != TEXT('\\')) lstrcat(filepath, TEXT("\\"));
	lstrcat(filepath, TEXT("*"));
	dir = FindFirstFile(filepath, &wfd);
	if (dir == INVALID_HANDLE_VALUE) return;

	do {
		if (lstrcmp(wfd.cFileName, TEXT(".")) == 0 || lstrcmp(wfd.cFileName, TEXT("..")) == 0) continue;
		lstrcpy(filepath, dirname);
		if (filepath[lstrlen(filepath) - 1] != TEXT('\\')) lstrcat(filepath, TEXT("\\"));
		lstrcat(filepath, wfd.cFileName);

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			setFileList(filepath);
		} else {
			file = CreateFile(filepath, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if (file == INVALID_HANDLE_VALUE) {
				size = 0;
			} else {
				size = GetFileSize(file, &size_hi);
				if (size == INVALID_FILE_SIZE) size = 0;
			}
			CloseHandle(file);

			g_file_list_count++;
			g_file_list = (FILE_LIST *)realloc(g_file_list, sizeof(FILE_LIST) * g_file_list_count);
			file_list = &g_file_list[g_file_list_count - 1];
			lstrcpy(file_list->name, filepath);
			file_list->size = size;
			file_list->flag = 0;

			for (i = g_file_list_count - 1; i > 0; i--) {
				if (g_file_list[i - 1].size <= g_file_list[i].size) break;
				tmp = g_file_list[i - 1];
				g_file_list[i - 1] = g_file_list[i];
				g_file_list[i] = tmp;
			}
		}
	} while (FindNextFile(dir, &wfd));

	FindClose(dir);
}

void _tmain(int argc, TCHAR **argv)
{
	char flag1, flag2;
	FILE *file, *f1, *f2;
	TCHAR dirpath[MAX_PATH], *tmp;
	BYTE *m1 = NULL, *m2 = NULL;
	DWORD i, j;

	if (argc != 2) return;
	_tsetlocale(LC_ALL, TEXT(""));
	GetModuleFileName(NULL, dirpath, MAX_PATH);
	tmp = _tcsrchr(dirpath, TEXT('\\'));
	if (tmp != NULL) *tmp = TEXT('\0');
	_tchdir(dirpath);

	setFileList(argv[1]);

	file = _tfopen(TEXT("samefile.txt"), TEXT("w"));
	for (i = 0; i < g_file_list_count; i++) {
		if (g_file_list[i].flag) continue;

		flag1 = flag2 = 0;
		for (j = i + 1; j < g_file_list_count; j++) {
			if (g_file_list[i].size != g_file_list[j].size) break;
			if (g_file_list[j].flag) continue;

			if (!flag1) {
				m1 = (BYTE *)realloc(m1, g_file_list[i].size);
				f1 = _tfopen(g_file_list[i].name, TEXT("rb"));
				fread(m1, g_file_list[i].size, 1, f1);
				fclose(f1);
				flag1 = 1;
			}
			m2 = (BYTE *)realloc(m2, g_file_list[j].size);
			f2 = _tfopen(g_file_list[j].name, TEXT("rb"));
			fread(m2, g_file_list[j].size, 1, f2);
			fclose(f2);
			if (memcmp(m1, m2, g_file_list[i].size) == 0) {
				if (!flag2) {
					g_file_list[i].flag = 1;
					_ftprintf(file, TEXT("%s (%u)\n"), g_file_list[i].name, g_file_list[i].size);
					flag2 = 1;
				}
				g_file_list[j].flag = 1;
				_ftprintf(file, TEXT("%s\n"), g_file_list[j].name);
			}
		}
		if (flag2) _ftprintf(file, TEXT("\n"));
		_tprintf(TEXT("\b\b\b\b\b%u"), i);
	}
	free(g_file_list);
	free(m1);
	free(m2);
	fclose(file);
}
