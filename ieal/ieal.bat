rem IEに特定のURLにログインするアドオンを登録
rem created at 2007-09-04
rc ieal.rc
cl ieal.cpp ieal.res /O2 /LD /link /def:ieal.def advapi32.lib oleaut32.lib shlwapi.lib

rem COM登録
regsvr32 ieal.dll

rem COM登録解除
rem regsvr32 /u ieal.dll
