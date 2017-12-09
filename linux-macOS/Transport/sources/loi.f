        SUBROUTINE LOI
C
        CHARACTER TITEL*10, LAB*4, DIM1*2, DIM2*2, DIM3*2
        LOGICAL SLITS
C
        INCLUDE 'incom.f'
C
C   READ IN BEAMLINE DATA FILE
C
        CALL READD(SLITS)
C
C   READ OUTPUT DATA FROM FOR003.DAT
C
        Z  = 0.
        I1 = 1
        N  = 0
        OPEN(UNIT=3,FILE='FOR003.DAT',STATUS='OLD',
     *       FORM='UNFORMATTED',ERR=999)
    1   ZOLD = Z
        READ(3,END=300,ERR=999) TITEL,LAB,Z,DIM1,X,DIM2,Y,DIM2,
     *                          DX,DIM2,DIM3,DY,DIM2,DIM3
        GO TO 350
C
C       WRITE NEW FILE
C
  300   IF (N .EQ. 0) THEN
         WRITE(6,*) 'No modified lengths found.'
         CALL SLEEP(3)
        ELSE
         CALL WRITED(1)
        ENDIF
        RETURN
  350   CONTINUE
C
        IF (LAB .EQ. ' ') GO TO 1
        ZLENG = Z - ZOLD
C
C   INSERT LENGTH OF LABELED ELEMENTS IN BEAMLINE
C
        DO 700 NUM=I1,NEL
         IF (LABEL(NUM) .EQ. LAB) THEN
          I1 = NUM + 1
          I=ISTOR(NUM)
          ITYPE=DATA(I)
          ITYPE=IABS(ITYPE)
          IF (ITYPE.NE. 3 .AND. ITYPE.NE. 4 .AND. ITYPE.NE.5 .AND.
     *        ITYPE.NE.18 .AND. ITYPE.NE.19) GO TO 1
          IF (ABS(ZLENG - DATA(I+1)) .LT. .0005) GO TO 1
          SAVE = DATA(I+1)
          DATA(I+1) = FLOAT(NINT(1000. * ZLENG)) / 1000.
          N = N + 1
          IF (N .EQ. 1)
     *    WRITE(6,*) 'the following lengths have been transferred:'
          WRITE(6,1001) N, TITEL, LAB, DATA(I+1), DIM1
 1001     FORMAT(I3,') ',A10,2X,A4,F10.3,1X,A2)
          GO TO 1
         ENDIF
  700   CONTINUE
        PRINT *,'Label mismatch on input and output'
        CALL SLEEP(3)
        RETURN
C
  999   WRITE(6,*) 'You have to run Transport first.'
        CALL SLEEP(3)
        RETURN
        END
