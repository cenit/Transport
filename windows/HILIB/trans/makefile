
# Makefile for tpmenu program

CPU = i386

!include <ntwin32.mak>

WIN32 = 1

OB1 = tpmenu.obj dispfile.obj dispdlg.obj flatdlg.obj toutfile.obj tcp.obj dosound.obj
OB2 = stextwin.obj textwin.obj tranfile.obj tsecfile.obj tgrafile.obj teliwin.obj teliwin2.obj teliwin3.obj
OB3 = filebuff.obj trinfile.obj calidata.obj varidlg.obj tgrafwin.obj popwin.obj

YX  = /I..\MYWPP /Fptpmenu.sym /YX /DWIN95
mylibs = @..\MYWPP\W95.LIB

ALL : tpmenu.exe

tpmenu.res : tpmenu.rc
    rc -r -iD:\HILIB\MYWPP -DWIN32 -DWIN95 tpmenu.rc

tpmenu.obj : tpmenu.cpp tpmenu.hpp tpmenu.h
    $(cc) $(cflags) $(YX) $(cvars) tpmenu.cpp

flatdlg.obj : flatdlg.cpp flatdlg.hpp tpmenu.h
    $(cc) $(cflags) $(YX) $(cvars) flatdlg.cpp

dispdlg.obj : dispdlg.cpp dispdlg.hpp
    $(cc) $(cflags) $(YX) $(cvars) dispdlg.cpp

popwin.obj : popwin.cpp popwin.hpp
    $(cc) $(cflags) $(YX) $(cvars) popwin.cpp

tcp.obj : tcp.cpp tcp.hpp
    $(cc) $(cflags) $(YX) $(cvars) tcp.cpp

textwin.obj : textwin.cpp textwin.hpp
    $(cc) $(cflags) $(YX) $(cvars) textwin.cpp

stextwin.obj : stextwin.cpp stextwin.hpp
    $(cc) $(cflags) $(YX) $(cvars) stextwin.cpp

toutfile.obj : toutfile.cpp toutfile.hpp
    $(cc) $(cflags) $(YX) $(cvars) toutfile.cpp

tranfile.obj : tranfile.cpp tranfile.hpp
    $(cc) $(cflags) $(YX) $(cvars) tranfile.cpp

tgrafile.obj : tgrafile.cpp tgrafile.hpp
    $(cc) $(cflags) $(YX) $(cvars) tgrafile.cpp

tsecfile.obj : tsecfile.cpp tsecfile.hpp
    $(cc) $(cflags) $(YX) $(cvars) tsecfile.cpp

tgrafwin.obj : tgrafwin.cpp tgrafwin.hpp
    $(cc) $(cflags) $(YX) $(cvars) tgrafwin.cpp

teliwin.obj : teliwin.cpp teliwin.hpp
    $(cc) $(cflags) $(YX) $(cvars) teliwin.cpp

teliwin2.obj : teliwin2.cpp teliwin2.hpp
    $(cc) $(cflags) $(YX) $(cvars) teliwin2.cpp

teliwin3.obj : teliwin3.cpp teliwin3.hpp
    $(cc) $(cflags) $(YX) $(cvars) teliwin3.cpp

filebuff.obj : filebuff.cpp filebuff.hpp
    $(cc) $(cflags) $(YX) $(cvars) filebuff.cpp

dispfile.obj : dispfile.cpp dispfile.hpp
    $(cc) $(cflags) $(YX) $(cvars) dispfile.cpp

trinfile.obj : trinfile.cpp trinfile.hpp
    $(cc) $(cflags) $(YX) $(cvars) trinfile.cpp

calidata.obj : calidata.cpp calidata.hpp
    $(cc) $(cflags) $(YX) $(cvars) calidata.cpp

varidlg.obj : varidlg.cpp varidlg.hpp tpmenu.h
    $(cc) $(cflags) $(YX) $(cvars) varidlg.cpp

dosound.obj : dosound.c
    $(cc) $(cflags) $(cvars) dosound.c

tpmenu.exe: $(OB1) tpmenu.res tpmenu.def $(OB2) $(OB3)
    $(link) $(guiflags) /MACHINE:IX86 $(OB1) tpmenu.res $(OB2) $(OB3) $(mylibs) $(guilibs) wsock32.lib -out:..\tpmenu32.exe

    del *.sym
    del *.obj
    del *.res
