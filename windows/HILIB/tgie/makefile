
# Makefile for TGIE program

CPU = i386

!include <ntwin32.mak>

#cc = bcompile -Zi
#link = bclink /debug:full

WIN32 = 1

OB1 = tgie.obj textwin.obj
OB2 = filebuff.obj trinfile.obj
OB3 = tgiedlg.obj dialogs.obj


YX  = /I..\MYWPP /Fptgie.sym /YX -DWIN95
mylibs = @..\mywpp\w95.lib

ALL : tgie.exe

tgie.res : tgie.rc
    rc -r -i..\MYWPP -DWIN95 tgie.rc

tgie.obj : tgie.cpp tgie.hpp tgie.h
    $(cc) $(cflags) $(YX) $(cvars) tgie.cpp

filebuff.obj : filebuff.cpp filebuff.hpp
    $(cc) $(cflags) $(YX) $(cvars) filebuff.cpp

textwin.obj : textwin.cpp textwin.hpp
    $(cc) $(cflags) $(YX) $(cvars) textwin.cpp

trinfile.obj : trinfile.cpp trinfile.hpp
    $(cc) $(cflags) $(YX) $(cvars) trinfile.cpp

tgiedlg.obj : tgiedlg.cpp tgiedlg.hpp tgie.h
    $(cc) $(cflags) $(YX) $(cvars) tgiedlg.cpp

dialogs.obj : dialogs.cpp dialogs.hpp tgie.h
    $(cc) $(cflags) $(YX) $(cvars) dialogs.cpp

tgie.exe: $(OB1) tgie.res tgie.def $(OB2) $(OB3)
    $(link) $(guiflags) /MACHINE:IX86 $(OB1) tgie.res $(OB2) $(OB3) $(mylibs) $(guilibs) -out:..\tgie32.exe

    del *.obj
    del *.sym
    del *.res

