rem ���삵��COM��VBScript���ŌĂяo����悤�ɂ���T���v��
rem created at 2007-08-25
cl com.cpp /O2 /LD /link advapi32.lib oleaut32.lib user32.lib /def:com.def

rem COM�o�^
regsvr32 com.dll

rem cab�t�@�C���쐬
echo .Set CabinetNameTemplate=com.cab > com.ddf
echo .Set DiskDirectoryTemplate= >> com.ddf
echo com.dll >> com.ddf
echo com.inf >> com.ddf
makecab /f com.ddf

rem �_�~�[�ؖ����쐬
makecert -n CN=comCA -sv com-ca.pvk -r com-ca.cer
makecert -n CN=com -sv com.pvk -ic com-ca.cer -iv com-ca.pvk com.cer
cert2spc com.cer com.spc
pvk2pfx -pvk com.pvk -spc com.spc -pfx com.pfx

echo ���O�ɍ쐬����com.pfx���C���|�[�g���Ă�������
pause

rem ������t����
signtool sign /f com.pfx com.cab

rem COM�o�^����
rem regsvr32 /u com.dll
