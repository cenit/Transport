        SUBROUTINE PIREXF
C
        INCLUDE 'incom.f'
        COMMON /ERROR/ IER
C
        CHARACTER       ATEXT*100, NAM*6, NAM1*6, LAB1*4
        LOGICAL         ANIN, SLITS, EQUAD
        DATA            ANIN  /.FALSE./
        DATA            EQUAD /.FALSE./
C           
C       READ IN DATA FILE
C           
    1   CALL READD(SLITS)
C
C       GET USER INPUT
C
        WRITE(6,1000)
 1000   FORMAT(' Input: required',
     1         ' horizontal beam spot diameter in mm (4*sigx) ',$)
        READ(5,1002,ERR=999) VALX 
 1002   FORMAT(F10.0)
        WRITE(6,1001)
 1001   FORMAT(' Input: required',
     1         ' vertical beam spot diameter in mm (4*sigy) ',$)
        READ(5,1002,ERR=999) VALY 
C
C       FILL INTO FOR001.DAT
C
        MAL = 0
        DO 750 NUM = 1, NEL
         I     = ISTOR(NUM)
         ITYPE = DATA(I)
         ITYPE = IABS(ITYPE)
         IF (ITYPE .EQ. 10) THEN
          IF      (LABEL(NUM) .EQ. 'FITX') THEN
           DATA(I+3) = VALX/2.
           MAL = MAL + 1
          ELSE IF (LABEL(NUM) .EQ. 'FITY') THEN
           DATA(I+3) = VALY/2.
           MAL = MAL + 1
          ELSE
          ENDIF
         ENDIF
  750   CONTINUE
C
C       CHECK IF FOR001.DAT HAS TO BE MODIFIED
C
        IF (MAL .NE. 2) THEN
         WRITE(6,*) 'Label "FITX" and/or "FITY" not found in FOR001.DAT'
         CALL SLEEP(3)
         RETURN
        ENDIF
C           
C       WRITE NEW FILE  
C           
        CALL WRITED(1)
C
        IER = 0
        RETURN
c
c       ERROR HANDLING
c
 999    WRITE(6,*) 'wrong input'
        IER = 1
        CALL SLEEP(3)
        RETURN
        END   
