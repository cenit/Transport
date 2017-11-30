# Nmake macros for building Windows 32-Bit dll

WIN32 = 1

!include <ntwin32.mak>

all: lsthsc32.lib lsthsc32.dll

# Update the object files if necessary

listhscr.OBJ: listhscr.C listhscr.H
    $(cc) $(cflags) $(cvarsdll) listhscr.c

# Update the import library

lsthsc32.lib: listhscr.obj lsthsc32.def
    $(implib) -machine:$(CPU)     \
    -def:lsthsc32.def	    \
    listhscr.obj \
    -out:lsthsc32.lib

# Update the dynamic link library

lsthsc32.dll: listhscr.obj lsthsc32.def
    $(link)     \
    -base:0x1C000000  \
    -dll	      \
    -entry:LibMain$(DLLENTRY)    \
    -out:lsthsc32.dll   \
    lsthsc32.exp listhscr.obj $(guilibsmt)

    copy lsthsc32.lib ..\Libs\lsthsc32.lib
    copy lsthsc32.dll ..\lsthsc32.dll
    del listhscr.obj
    del lsthsc32.dll
    del lsthsc32.exp
    del lsthsc32.lib

