# Nmake macros for building Windows 32-Bit dll

WIN32 = 1
RUN_TIME_INC = 1

!include <ntwin32.mak>

all: anamtr32.lib anamtr32.dll

# Update the object files if necessary

INIT.OBJ: INIT.C ANAMTR.H COMMON.H
    $(cc) $(cflags) $(cvarsdll) init.c

MAIN.OBJ: MAIN.C ANAMTR.H COMMON.H
    $(cc) $(cflags) $(cvarsdll) main.c

DIALOG.OBJ: MAIN.C ANAMTR.H COMMON.H
    $(cc) $(cflags) $(cvarsdll) dialog.c

!IFDEF RUN_TIME_INC
dmain.obj: dmain.c
    $(cc) -c dmain.c
!ENDIF

ANAMTR.RES : ANAMTR.RC
   rc -r anamtr.rc

# Update the import library

anamtr32.lib: init.obj main.obj dialog.obj anamtr.res anamtr32.def
    $(implib) -machine:$(CPU) \
    -def:anamtr32.def \
    init.obj main.obj dialog.obj anamtr.res \
    -out:anamtr32.lib

# Update the dynamic link library

!IFDEF RUN_TIME_INC
anamtr32.dll: init.obj main.obj dialog.obj anamtr.res anamtr32.def dmain.obj
!ELSE
anamtr32.dll: init.obj main.obj dialog.obj anamtr.res anamtr32.def dmain.obj
!ENDIF
    $(link) /NODEFAULTLIB \
    -base:0x1C000000 \
    -dll \
    -entry:LibMain$(DLLENTRY) \
    -out:anamtr32.dll \
!IFDEF RUN_TIME_INC
    anamtr32.exp init.obj main.obj dialog.obj dmain.obj anamtr.res $(guilibsmt)
!ELSE
    anamtr32.exp init.obj main.obj dialog.obj anamtr.res $(guilibsdll)
!ENDIF

    copy anamtr32.lib ..\Libs\anamtr32.lib
    copy anamtr32.dll ..\anamtr32.dll
    del anamtr32.exp
    del anamtr32.lib
    del anamtr32.dll
    del anamtr.res
    del *.obj

