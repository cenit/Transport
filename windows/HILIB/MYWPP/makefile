# Makefile : Builds the MYWPP library

!include <ntwin32.mak>

WIN32 = 1

OB1 = dispdev.obj window.obj manager.obj drwtools.obj metadev.obj
OB2 = dialog.obj simpdlg.obj control.obj printer.obj
YX  = /Fphi.sym /YX -DWIN95

ALL : mywpp.lib


manager.obj : manager.cpp manager.hpp
    $(cc) $(cflags) $(YX) $(cvars) manager.cpp

drwtools.obj : drwtools.cpp drwtools.hpp
    $(cc) $(cflags) $(YX) $(cvars) drwtools.cpp

dispdev.obj : dispdev.cpp dispdev.hpp
    $(cc) $(cflags) $(YX) $(cvars) dispdev.cpp

metadev.obj : metadev.cpp metadev.hpp
    $(cc) $(cflags) $(YX) $(cvars) metadev.cpp

window.obj : window.cpp window.hpp
    $(cc) $(cflags) $(YX) $(cvars) window.cpp

dialog.obj : dialog.cpp dialog.hpp
    $(cc) $(cflags) $(YX) $(cvars) dialog.cpp

simpdlg.obj : simpdlg.cpp simpdlg.hpp
    $(cc) $(cflags) $(YX) $(cvars) simpdlg.cpp

control.obj : control.cpp control.hpp
    $(cc) $(cflags) $(YX) $(cvars) control.cpp

printer.obj : printer.cpp printer.hpp
    $(cc) $(cflags) $(YX) $(cvars) printer.cpp

mywpp.lib: $(OB1) $(OB2)
    $(implib) -machine:$(CPU)     \
    $(OB1) $(OB2) \
    -out:mywpp.lib


    copy mywpp.lib ..\libs\mywpp.lib
    del mywpp.lib
    del *.obj
    del *.sym

