rem IE�ɓ����URL�Ƀ��O�C������A�h�I����o�^
rem created at 2007-09-04
rc ieal.rc
cl ieal.cpp ieal.res /O2 /LD /link /def:ieal.def advapi32.lib oleaut32.lib shlwapi.lib

rem COM�o�^
regsvr32 ieal.dll

rem COM�o�^����
rem regsvr32 /u ieal.dll
