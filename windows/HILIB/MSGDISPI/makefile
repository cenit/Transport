# Nmake macros for building Windows 32-Bit dll

WIN32 = 1

!include <ntwin32.mak>

all: msgdsp32.lib msgdsp32.dll

# Update the object files if necessary

msgdispi.OBJ: msgdispi.C msgdispi.H
    $(cc) $(cflags) $(cvarsdll) msgdispi.c

# Update the import library

msgdsp32.lib: msgdispi.obj msgdsp32.def
    $(implib) -machine:$(CPU)     \
    -def:msgdsp32.def	    \
    msgdispi.obj \
    -out:msgdsp32.lib

# Update the dynamic link library

msgdsp32.dll: msgdispi.obj msgdsp32.def
    $(link)     \
    -base:0x1C000000  \
    -dll	      \
    -entry:LibMain$(DLLENTRY)    \
    -out:msgdsp32.dll   \
    msgdsp32.exp msgdispi.obj $(guilibsmt)

    copy msgdsp32.lib ..\Libs\msgdsp32.lib
    copy msgdsp32.dll ..\msgdsp32.dll
    del msgdispi.obj
    del msgdsp32.dll
    del msgdsp32.exp
    del msgdsp32.lib

