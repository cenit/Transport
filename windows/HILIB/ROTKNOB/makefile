# Nmake macros for building Windows 32-Bit dll

WIN32 = 1
RUN_TIME_INC = 1

!include <ntwin32.mak>

all: ROTKNB32.lib ROTKNB32.dll

# Update the object files if necessary

INIT.OBJ: INIT.C ROTKNOB.H COMMON.H
    $(cc) $(cflags) $(cvarsdll) init.c

MAIN.OBJ: MAIN.C ROTKNOB.H COMMON.H
    $(cc) $(cflags) $(cvarsdll) main.c

DIALOG.OBJ: MAIN.C ROTKNOB.H COMMON.H
    $(cc) $(cflags) $(cvarsdll) dialog.c

!IFDEF RUN_TIME_INC
dmain.obj: dmain.c
    $(cc) -c dmain.c
!ENDIF

ROTKNOB.RES : ROTKNOB.RC
   rc -r ROTKNOB.rc

# Update the import library

ROTKNB32.lib: init.obj main.obj dialog.obj ROTKNOB.res ROTKNB32.def
    $(implib) -machine:$(CPU) \
    -def:ROTKNB32.def \
    init.obj main.obj dialog.obj ROTKNOB.res \
    -out:ROTKNB32.lib

# Update the dynamic link library

!IFDEF RUN_TIME_INC
ROTKNB32.dll: init.obj main.obj dialog.obj ROTKNOB.res ROTKNB32.def dmain.obj
!ELSE
ROTKNB32.dll: init.obj main.obj dialog.obj ROTKNOB.res ROTKNB32.def
!ENDIF
    $(link) /NODEFAULTLIB \
    -base:0x1C000000 \
    -dll \
    -entry:LibMain$(DLLENTRY) \
    -out:ROTKNB32.dll \
!IFDEF RUN_TIME_INC
    ROTKNB32.exp init.obj main.obj dialog.obj dmain.obj ROTKNOB.res $(guilibsmt)
!ELSE
    ROTKNB32.exp init.obj main.obj dialog.obj ROTKNOB.res $(guilibsdll)
!ENDIF

    copy ROTKNB32.lib ..\Libs\rotknb32.lib
    copy ROTKNB32.dll ..\rotknb32.dll
    del *.obj
    del ROTKNB32.dll
    del ROTKNB32.exp
    del ROTKNB32.lib
    del ROTKNOB.res

