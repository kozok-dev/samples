#include <stdio.h>
#include <windows.h>
#include <wininet.h>
#define FTP_SERVERNAME "example.jp"
#define FTP_USERNAME "user"
#define FTP_PASSWORD "password"

void main()
{
	HINTERNET hinet, hconn, hfind;
	char buf[INTERNET_MAX_PATH_LENGTH];
	int size = INTERNET_MAX_PATH_LENGTH;
	WIN32_FIND_DATA fd;
	SYSTEMTIME st;

	hinet = InternetOpen("InetTest", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (!hinet) {
		printf("InternetOpen failed.\n");
		getchar();
		return;
	}

	hconn = InternetConnect(hinet, FTP_SERVERNAME, INTERNET_INVALID_PORT_NUMBER, FTP_USERNAME, FTP_PASSWORD, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
	if (hconn) {
		hfind = FtpFindFirstFile(hconn, NULL, &fd, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
		if (hfind) {
			do {
				FileTimeToSystemTime(&fd.ftLastWriteTime, &st);
				printf("%20s %010u-%010u %d/%02d/%02d-%02d:%02d\n",
					fd.cFileName,
					fd.ftLastWriteTime.dwHighDateTime, fd.ftLastWriteTime.dwLowDateTime,
					st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
			} while (InternetFindNextFile(hfind, (LPVOID)&fd));
		} else {
			printf("FtpFindFirstFile failed.\n");
		}
		InternetCloseHandle(hconn);
	} else {
		printf("InternetConnect failed.\n");
	}

	InternetCloseHandle(hinet);
	getchar();
}
