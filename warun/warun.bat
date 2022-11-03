rc warun.rc
cl warun.cpp warun.res /O2 /GA /link gdi32.lib user32.lib winmm.lib d3d9.lib dsound.lib dxguid.lib
warun
