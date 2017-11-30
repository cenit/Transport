# Nmake macros for building Windows 32-Bit dll

WIN32 = 1

!include <ntwin32.mak>

all: turtle32.lib turtle32.dll

# Update the object files if necessary

turtdll.OBJ: turtdll.C
    $(cc) $(cflags) $(cvarsdll) turtdll.C

turtle32.OBJ : turtle32.FOR
    FL32 -c -Ox turtle32.FOR

# Update the import library

turtle32.lib: turtdll.obj turtdll.def
    $(implib) -machine:$(CPU)     \
    -def:turtdll.def	    \
    turtdll.obj \
    -out:turtle32.lib

# Update the dynamic link library

turtle32.dll: turtdll.obj turtle32.obj turtdll.def
    $(link)     \
    -base:0x1C000000  \
    -dll	      \
    -entry:_DllMainCRTStartup$(DLLENTRY)    \
    -out:turtle32.dll   \
    turtle32.exp turtdll.obj turtle32.obj $(guilibs) $(mylibs)

    copy turtle32.dll ..\turtle32.dll
    copy turtle32.lib ..\Hilib\Libs\turtdll32.lib
    copy turtle32.lib ..\Hilib\Libs\turdll32.lib
    copy turtle32.lib ..\Hilib\Libs\turdll.lib
    del *.obj
    del *.dll
    del *.lib
    del *.exp
