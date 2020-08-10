rem リモートで任意のコマンドを実行(サーバー側)
rem クライアントが接続してくると"command>"と表示され、任意のコマンドを実行できるようになる
rem created at 2007-08-06
cl recoserv.c /O2 /link ws2_32.lib
