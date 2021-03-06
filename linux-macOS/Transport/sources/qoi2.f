        SUBROUTINE QOI2
C
        DIMENSION    QUAD(150), LABELF(150)
        CHARACTER*4  LAB, LABELF
C        CHARACTER*1  SEMI
        CHARACTER*60 TEXT
C
        INCLUDE 'incom.f'
C
        LOGICAL SLITS
C
C   READ IN DATA FILE
C
        CALL READD(SLITS)
C
C   READ QUAD DATA FROM FILE 'FOR024.DAT'
C
        J = 1
        OPEN(UNIT=24,FILE='FOR024.DAT',STATUS='OLD',ERR=99)
C
  440   CONTINUE
        READ(24,'(A)',END=450) TEXT
        READ(TEXT(1:2),'(I2)') ITYPE
        READ(TEXT(11:18),'(F8.3)') BX
        LAB = TEXT(28:31)
        IF (ITYPE .EQ. 5) THEN
         QUAD(J)  = BX
         LABELF(J) = LAB
         J = J + 1
            ENDIF
            GO TO 440
C
  450   CONTINUE
        CLOSE(UNIT=24)
        NNS = J - 1
C
C   PUT QUAD FIELDS OF 'FOR024.DAT' INTO INPUT FILE 'FOR001.DAT'
C
        DO 800 NUM = 1, NEL
         I     = ISTOR(NUM)
         ITYPE = DATA(I)
         ITYPE = IABS(ITYPE)
         IF (ITYPE.EQ.5) THEN
          DO 600 MQ = 1, NNS
           IF (LABEL(NUM) .EQ. LABELF(MQ)) THEN
            WRITE(6,1100) LABEL(NUM), DATA(I+2), QUAD(MQ)
 1100       FORMAT(' set Quad "',A4,'" from',F8.3,' to',F8.3,' kG')
            DATA(I+2) = QUAD(MQ)
           ENDIF
  600     CONTINUE
         ENDIF
  800   CONTINUE
C
C   WRITE NEW FILE
C
        CALL WRITED(0)
        RETURN
C
C   ERROR
C
   99   WRITE(6,7780)
 7780   FORMAT('You have to run Transport first.')
        CALL SLEEP(3)
        RETURN
        END

