rem 自作したCOMをVBScript等で呼び出せるようにするサンプル
rem created at 2007-08-25
cl com.cpp /O2 /LD /link advapi32.lib oleaut32.lib user32.lib /def:com.def

rem COM登録
regsvr32 com.dll

rem cabファイル作成
echo .Set CabinetNameTemplate=com.cab > com.ddf
echo .Set DiskDirectoryTemplate= >> com.ddf
echo com.dll >> com.ddf
echo com.inf >> com.ddf
makecab /f com.ddf

rem ダミー証明書作成
makecert -n CN=comCA -sv com-ca.pvk -r com-ca.cer
makecert -n CN=com -sv com.pvk -ic com-ca.cer -iv com-ca.pvk com.cer
cert2spc com.cer com.spc
pvk2pfx -pvk com.pvk -spc com.spc -pfx com.pfx

echo 事前に作成したcom.pfxをインポートしてください
pause

rem 署名を付ける
signtool sign /f com.pfx com.cab

rem COM登録解除
rem regsvr32 /u com.dll
