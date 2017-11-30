      SUBROUTINE MENU(IFLAG)
C
      IF (IFLAG .EQ. 1) THEN
       OPEN(UNIT=99,FILE='ERROR.TRA',STATUS='UNKNOWN')
        write(99,*) 'error'
       CLOSE(UNIT=99)
       write(*,1000)
 1000  format(' error in FOR001.DAT input ')
       CALL GETCHA(key)
       IRET = 1
      ELSE IF (IFLAG .EQ. 2) THEN
       OPEN(UNIT=99,FILE='ERROR.TRA',STATUS='UNKNOWN')
       write(99,*) 'error'
       CLOSE(UNIT=99)
       write(*,1001)
 1001  format(' input FOR001.DAT not found ')
       CALL GETCHA(key)
       IRET = 2
      ELSE
       write(*,*) ' '
       IRET = 0
      ENDIF
      CLOSE(UNIT=0)
      CLOSE(UNIT=5)
      CLOSE(UNIT=6)
      CLOSE(UNIT=7)
      CALL EXIT(IRET)
      STOP
C
      RETURN
      END
