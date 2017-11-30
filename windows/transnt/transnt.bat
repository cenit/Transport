call win3240.bat
fl32 -c  -Ox transnt.for
link transnt.obj @listopt
del *.obj

