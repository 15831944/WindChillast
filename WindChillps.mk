
WinChillps.dll: dlldata.obj WinChill_p.obj WinChill_i.obj
	link /dll /out:WinChillps.dll /def:WinChillps.def /entry:DllMain dlldata.obj WinChill_p.obj WinChill_i.obj \
		kernel32.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \
.c.obj:
	cl /c /Ox /DREGISTER_PROXY_DLL \
		$<

clean:
	@del WinChillps.dll
	@del WinChillps.lib
	@del WinChillps.exp
	@del dlldata.obj
	@del WinChill_p.obj
	@del WinChill_i.obj
