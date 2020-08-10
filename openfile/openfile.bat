rem created at 2005-12-28
rc openfile.rc
cl openfile.cpp openfile.res /O2 /link comdlg32.lib gdi32.lib gdiplus.lib user32.lib
