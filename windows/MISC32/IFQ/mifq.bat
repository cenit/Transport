call win3240.bat
C:\Visual\df98\bin\df /c ifq.for
link ifq.obj @libs.lst /out:..\ifq32.exe
del *.obj
