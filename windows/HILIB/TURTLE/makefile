# Makefile for ttmenu program

CPU = i386

!include <ntwin32.mak>

WIN32 = 1

OB1 = ttmenu.obj iofile.obj axis.obj graf3d.obj
OB2 = stextwin.obj meterdlg.obj muscdlg.obj hplotdlg.obj
OB3 = popwin.obj tugdlg.obj sgrafwin.obj dosound.obj

YX  = /I..\MYWPP /Fpttmenu.sym /YX /DWIN95
mylibs = @..\MYWPP\W95.LIB

ALL : ttmenu.exe

ttmenu.res : ttmenu.rc
    rc -r -i..\MYWPP -DWIN32 -DWIN95 ttmenu.rc

ttmenu.obj : ttmenu.cpp ttmenu.hpp ttmenu.h
    $(cc) $(cflags) $(YX) $(cvars) ttmenu.cpp

tugdlg.obj : tugdlg.cpp tugdlg.hpp ttmenu.h
    $(cc) $(cflags) $(YX) $(cvars) tugdlg.cpp

popwin.obj : popwin.cpp popwin.hpp
    $(cc) $(cflags) $(YX) $(cvars) popwin.cpp

meterdlg.obj : meterdlg.cpp meterdlg.hpp
    $(cc) $(cflags) $(YX) $(cvars) meterdlg.cpp

stextwin.obj : stextwin.cpp stextwin.hpp
    $(cc) $(cflags) $(YX) $(cvars) stextwin.cpp

iofile.obj : iofile.cpp iofile.hpp
    $(cc) $(cflags) $(YX) $(cvars) iofile.cpp

sgrafwin.obj : sgrafwin.cpp sgrafwin.hpp
    $(cc) $(cflags) $(YX) $(cvars) sgrafwin.cpp

axis.obj : axis.cpp axis.hpp
    $(cc) $(cflags) $(YX) $(cvars) axis.cpp

graf3d.obj : graf3d.cpp graf3d.hpp
    $(cc) $(cflags) $(YX) $(cvars) graf3d.cpp

muscdlg.obj : muscdlg.cpp muscdlg.hpp
    $(cc) $(cflags) $(YX) $(cvars) muscdlg.cpp

hplotdlg.obj : hplotdlg.cpp hplotdlg.hpp
    $(cc) $(cflags) $(YX) $(cvars) hplotdlg.cpp

dosound.obj : dosound.c
    $(cc) $(cflags) $(cvars) dosound.c

ttmenu.exe: $(OB1) ttmenu.res ttmenu.def $(OB2) $(OB3)
    $(link) $(guiflags) /MACHINE:IX86 $(OB1) ttmenu.res $(OB2) $(OB3) $(mylibs) $(guilibs) -out:..\ttmenu32.exe

    del *.sym
    del *.obj
    del *.res
