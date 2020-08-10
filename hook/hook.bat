rem created at 2004-04-20
cl hook.c /O2 /LD /link user32.lib /def:hook.def
cl test.c /O2
