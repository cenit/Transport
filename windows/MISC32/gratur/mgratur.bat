call win3240.bat
c:\Visual\df98\bin\df /c gratur.for
link gratur.obj @libs.lst /out:..\gratur32.exe
del *.obj
