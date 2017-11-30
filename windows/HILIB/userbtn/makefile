# Nmake macros for building Windows 32-Bit dll

WIN32 = 1

!include <ntwin32.mak>

all: usrbtn32.lib usrbtn32.dll

# Update the object files if necessary

userbtn.OBJ: userbtn.CPP userbtn.HPP
    $(cc) $(cflags) $(cvarsdll) userbtn.CPP

userbtn.RES : userbtn.RC
   rc -r userbtn.rc

# Update the import library

usrbtn32.lib: userbtn.obj userbtn.res usrbtn32.def
    $(implib) -machine:$(CPU)     \
    -def:usrbtn32.def	    \
    userbtn.obj userbtn.res \
    -out:usrbtn32.lib

# Update the dynamic link library

usrbtn32.dll: userbtn.obj userbtn.res usrbtn32.def
    $(link)     \
    -base:0x1C000000  \
    -dll	      \
    -entry:LibMain$(DLLENTRY)    \
    -out:usrbtn32.dll   \
    usrbtn32.exp userbtn.obj userbtn.res $(guilibsmt)

    copy usrbtn32.lib ..\Libs\usrbtn32.lib
    copy usrbtn32.dll ..\usrbtn32.dll
    del userbtn.obj
    del usrbtn32.exp
    del usrbtn32.lib
    del usrbtn32.dll
    del userbtn.res

