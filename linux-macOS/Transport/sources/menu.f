      SUBROUTINE MENU(IFLAG)
C
      COMMON /BLOC2/ NIN, NOUT, FORMIO
      LOGICAL FORMIO
C
      INTEGER IRET
      LOGICAL FX
C
      IF (IFLAG .EQ. 1) THEN
       OPEN(UNIT=99,FILE='ERROR.TRA',STATUS='UNKNOWN')
        write(99,*) 'error'
       CLOSE(UNIT=99)
c
       INQUIRE(FILE='tpmenu.is.running',EXIST=FX)
       IF (FX) THEN
c        CALL SYSTEM('dialogs f1error')
       ELSE
        print *,'error in FOR001.DAT input'
       ENDIF
       IRET = 1
c
      ELSE IF (IFLAG .EQ. 2) THEN
       OPEN(UNIT=99,FILE='ERROR.TRA',STATUS='UNKNOWN')
        write(99,*) 'error'
       CLOSE(UNIT=99)
c
       INQUIRE(FILE='tpmenu.is.running',EXIST=FX)
       IF (FX) THEN
c        CALL SYSTEM('dialogs f1notf')
       ELSE
        print *,'input FOR001.DAT not found'
       ENDIF
       IRET = 2
c
      ELSE
       print *,'Transport finished.'
       IRET = 0
      ENDIF
c
c      CLOSE(UNIT=0)
c      CLOSE(UNIT=NIN)
c      CLOSE(UNIT=NOUT)
c      CLOSE(UNIT=NOUT+1)
      CALL EXIT(IRET)
C
      RETURN
      END
