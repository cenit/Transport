call win3240.bat
C:\Visual\df98\bin\df /c input.for
link input.obj @libs.lst /out:..\input32.exe
del *.obj
