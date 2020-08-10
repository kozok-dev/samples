#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#define CONNSTR "DRIVER={Microsoft Access Driver (*.mdb)}; DBQ=C:\\test.mdb"
#define STMTSTR "SELECT col FROM test"

void main()
{
	char i;
	SQLRETURN ret;

	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt;

	SQLCHAR buf[1024];
	SQLSMALLINT col, retlen;
	SQLLEN len;

	/* SQL環境領域確保。*/
	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	printf("SQLAllocHandle(ENV): %d\n", ret);

	/* SQL環境設定。*/
	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
	printf("SQLSetEnvAttr: %d\n", ret);

	/* SQL接続領域確保。*/
	ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	printf("SQLAllocHandle(DBC): %d\n", ret);

	/* SQL接続。*/
	ret = SQLDriverConnect(hdbc, NULL, CONNSTR, SQL_NTS, buf, sizeof(buf) / sizeof(buf[0]), &retlen, SQL_DRIVER_NOPROMPT);
	printf("SQLDriverConnect: %d\n", ret);

	/* SQLステートメント領域確保。*/
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	printf("SQLAllocHandle(STMT): %d\n", ret);

	/* SQLステートメント実行。*/
	ret = SQLExecDirect(hstmt, STMTSTR, SQL_NTS);
	printf("SQLExecDirect: %d\n", ret);

	/* SQLステートメント実行結果変数割り当てと取得。*/
	SQLBindCol(hstmt, 1, SQL_C_SSHORT, &col, sizeof(col), &len);
	for (i = 0; i < 10; i++) {
		SQLFetch(hstmt);
		printf("%d\n", col);
	}

	/* SQLステートメント領域開放。*/
	ret = SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	printf("SQLFreeHandle(STMT): %d\n", ret);

	/* SQL切断。*/
	ret = SQLDisconnect(hdbc);
	printf("SQLDisconnect: %d\n", ret);

	/* SQL接続領域開放。*/
	ret = SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	printf("SQLFreeHandle(DBC): %d\n", ret);

	/* SQ環境領域開放。*/
	ret = SQLFreeHandle(SQL_HANDLE_ENV, henv);
	printf("SQLFreeHandle(ENV): %d\n", ret);
}
