# Makefile : Builds the EDITOR32 application

CPU = i386

!include <ntwin32.mak>

WIN32 = 1

inc = /I..\MYWPP

mylibs = @..\mywpp\w95.lib

YX = /Fpeditor.sym /YX -DWIN95

ALL : editor.exe

editor.obj : editor.cpp editor.hpp editor.h
    $(cc) $(cflags) $(inc) $(YX) $(cvars) editor.cpp

editdlgs.obj : editdlgs.cpp editdlgs.hpp editor.h
    $(cc) $(cflags) $(inc) $(YX) $(cvars) editdlgs.cpp

editor.res : editor.rc
    rc -r -i..\MYWPP -DWIN95 editor.rc

editor.exe:  editdlgs.obj editor.obj editor.def editor.res editor.res
    $(link) $(guiflags) /MACHINE:IX86 -out:..\editor32.exe editor.res editor.obj editdlgs.obj $(guilibs) $(mylibs)

   del *.obj
   del *.sym
   del *.res


