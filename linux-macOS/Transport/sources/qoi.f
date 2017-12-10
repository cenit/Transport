        SUBROUTINE QOI
C
        DIMENSION    QUAD(150), LABELF(150)
        CHARACTER*19 QUADD(150)
        CHARACTER*10 TITEL
        CHARACTER*4  LAB, LABELF
        CHARACTER    STR1*5, STR2*13
        CHARACTER*2  DIM1, DIM2, DIM4
C
        INCLUDE 'incom.f'
C
        LOGICAL SLITS
C
C   READ IN DATA FILE
C
        CALL READD(SLITS)
C
C   READ QUAD AND SOLENOID DATA
C
        J = 1
        OPEN(UNIT=11,FILE='FOR011.DAT',STATUS='OLD',
     X       FORM='UNFORMATTED',ERR=99)
  440   READ(11,END=450,ERR=99) TITEL,LAB,XLENG,DIM1,AP,DIM2,B,DIM4,XN
        IF (TITEL.NE.'*QUAD*' .AND. TITEL.NE.'*SOLO*' .AND.
     X      TITEL.NE.'*EQUAD*'.AND. TITEL.NE.'*SEXT*') GO TO 440
        QUAD(J)  = B + ABS(B) / (B + .000001) *.000001
        LABELF(J) = LAB
        J = J + 1
        IF (J .GT. 150)
     1  STOP 'more than 150 quads, solenoids and/or sextupoles.'
        GO TO 440
  450   CONTINUE
        NNS = J-1
C
C   COMPARE INPUT WITH OUTPUT
C
        MQ  = 0
        NNQ = 0
        DO 700 NUM = 1, NEL
         I     = ISTOR(NUM)
         ITYPE = DATA(I)
         ITYPE = IABS(ITYPE)
         INUMB = DATA(I+1)
         IF (ITYPE.EQ.5 .OR. ITYPE.EQ.19 .OR. ITYPE.EQ.18) THEN
          MQ  = MQ + 1
          IF (MQ .GT. 150)
     1     STOP 'more than 150 quads, solenoids and/or sextupoles.'
          IF (LABEL(NUM) .EQ. LABELF(MQ)) THEN
           DATA(I+2) = QUAD(MQ)
           NNQ = NNQ + 1
           WRITE(STR1,1010) LABELF(MQ)
           WRITE(STR2,1011) QUAD(MQ)
           QUADD(NNQ) = STR1//STR2
          ELSE
           WRITE(6,7776) LABEL(NUM),LABELF(MQ)
          ENDIF
         ENDIF
  700   CONTINUE
C
        IF (NNQ .GT. 0) THEN
         WRITE(6,*)
     X   'The following quad-, solenoid and sextupole-data',
     X   ' have been transferred:'
         WRITE(6,*) ' '
         WRITE(6,1020) (QUADD(I),I=1,NNQ)
         WRITE(6,*) ' '
        ENDIF
C
        IF (NNQ.NE.NNS .OR. MQ.NE.NNS) THEN
         WRITE(6,7778) NNS
         WRITE(6,7779) NNQ
         WRITE(6,7777) MQ
        ENDIF
C
C   WRITE NEW FILE
C
        CALL WRITED(1)
        RETURN
C
C   ERROR
C
   99   WRITE(6,7780)
        CALL SLEEP(3)
        RETURN
C
C   FORMATS
C
 1010   FORMAT(A4,'=')
 1011   FORMAT(F8.4,' kG  ')
 1020   FORMAT(' ',4A19)
 7776   FORMAT(
     X  'FOR001: /',A4,'/ <-- label mismatch --> /',A4,'/ :FOR002.')
 7777   FORMAT(
     X  'space for',I3,' quad-, solenoid-',
     X  ' or sextupole-data found on  FOR001.')
 7778   FORMAT(
     X  'data  for',I3,' quads, solenoids',
     X  ' or sextupoles read in from  FOR002.')
 7779   FORMAT(
     X  'data  for',I3,' quads solenoids',
     X  ' or sextupoles transfered to FOR001.')
 7780   FORMAT('You have to run Transport first.')
        END
