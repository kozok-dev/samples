rem IE�Ɏ���c�[���o�[�̃A�h�I����o�^
rem created at 2007-10-06
cl iet.cpp /O2 /LD /link /def:iet.def advapi32.lib comctl32.lib gdi32.lib oleaut32.lib shlwapi.lib user32.lib

rem COM�o�^
regsvr32 iet.dll

rem COM�o�^����
rem regsvr32 /u iet.dll
