rc waru_bench.rc
cl waru_bench.cpp waru_bench.res /O2 /GA /link gdi32.lib user32.lib winmm.lib d3d9.lib dsound.lib dxguid.lib
waru_bench
