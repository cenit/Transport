# Nmake macros for building Windows 32-Bit apps

WIN32 = 1

!include <ntwin32.mak>

all: camac32.lib camac32.dll

# Update the object files if necessary

camac.obj: camac.cpp
    $(cc) $(cflags) $(cvarsdll) -ML -DWIN95 camac.cpp

# Update the import library

camac32.lib: camac.obj camac32.def
    $(implib) -machine:$(CPU)     \
    -def:camac32.def	    \
    camac.obj	 	    \
    -out:camac32.lib

# Update the dynamic link library

camac32.dll: camac.obj camac32.def
    $(link)     \
    -base:0x1C000000  \
    -dll	      \
    -entry:LibMain$(DLLENTRY)    \
    -out:camac32.dll   \
    camac32.exp camac.obj $(guilibsdll)

    copy camac32.lib ..\Libs\camac32.lib
    copy camac32.dll ..\camac32.dll
    del *.obj
    del camac32.dll
    del camac32.exp
    del camac32.lib
