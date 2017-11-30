# Nmake macros for building Windows 32-Bit dll

WIN32 = 1

!include <ntwin32.mak>

all: colrbt32.lib colrbt32.dll

# Update the object files if necessary

colorbut.OBJ: colorbut.C colorbut.H clrbuttn.H
    $(cc) $(cflags) $(cvarsdll) colorbut.c

colorbut.RES : colorbut.RC colorbut.DLG
   rc -r colorbut.rc

# Update the import library

colrbt32.lib: colorbut.obj colorbut.res colrbt32.def
    $(implib) -machine:$(CPU)     \
    -def:colrbt32.def	    \
    colorbut.obj colorbut.res \
    -out:colrbt32.lib

# Update the dynamic link library

colrbt32.dll: colorbut.obj colorbut.res colrbt32.def
    $(link)     \
    -base:0x1C000000  \
    -dll	      \
    -entry:LibMain$(DLLENTRY)    \
    -out:colrbt32.dll   \
    colrbt32.exp colorbut.obj colorbut.res $(guilibsmt)

    copy colrbt32.lib ..\Libs\colrbt32.lib
    copy colrbt32.dll ..\colrbt32.dll
    del colorbut.obj
    del colrbt32.dll
    del colrbt32.exp
    del colrbt32.lib
    del colorbut.res

