
# Nmake macros for building Windows 32-Bit dll

WIN32 = 1

!include <ntwin32.mak>


all: trans32.lib trans32.dll

# Update the object files if necessary

transdll.OBJ: transdll.C
    $(cc) $(cflags) $(cvarsdll) transdll.C

TRANS32.OBJ: TRANS32.FOR
    fl32 -c -Ox TRANS32.FOR

# Update the import library

trans32.lib: transdll.obj transdll.def
    $(implib) -machine:$(CPU)     \
    -def:transdll.def	    \
    transdll.obj \
    -out:trans32.lib

# Update the dynamic link library

trans32.dll: transdll.obj trans32.obj transdll.def
    $(link)     \
    -base:0x1C000000  \
    -dll	      \
    -entry:_DllMainCRTStartup$(DLLENTRY)    \
    -out:trans32.dll   \
    trans32.exp transdll.obj trans32.obj  $(guilibs) $(mylibs)

    copy trans32.dll ..\trans32.dll
    copy trans32.lib ..\Hilib\Libs\trndll32.lib
    copy trans32.lib ..\Hilib\Libs\transdll.lib
    del *.obj
    del *.lib
    del *.dll
    del *.exp


