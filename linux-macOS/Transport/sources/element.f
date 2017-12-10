        PROGRAM ELEMENT
C
        CHARACTER*130 CARD, TITEL, ANSW*1
        DATA IO /6/
C
        OPEN(UNIT=2,FILE='FOR002.DAT',STATUS='OLD',ERR=2001)
 1000   FORMAT(A)
 1001   FORMAT(11A4)
        TITEL = CARD
        I = 0
C
C       FOR002.DAT VON T ODER TFO ERZEUGT ?
C
        READ(2,1000,ERR=2001) CARD
        IF ((CARD(9:10).EQ.'MR') .AND. (CARD(17:18).EQ.'MR'))
     X     GO TO 2002
C
C   FINDE *PLOT* 
C
    1   CONTINUE
        READ(2,1000,END=200,ERR=2001) CARD
        IF (CARD(1:7) .NE. '1*PLOT*') GO TO 1
C
C       FINDE ELEMENT KARTEN
C
    2   CONTINUE
        READ(2,1000,END=200,ERR=2001) CARD
        IF(CARD(2:2)  .NE. '*')           GO TO 2
        IF(CARD(3:11) .EQ. 'TRANSFORM')   GO TO 2
        IF(CARD(3:11) .EQ. '2ND ORDER')   GO TO 2
        IF(CARD(3:13) .EQ. 'CORRECTIONS') GO TO 2
C
        CARD(1:1) = ' '
        CALL TYPE1(IO,CARD,NRL)
        I = I + NRL
        IF ((I.GE.23) .AND. IO.EQ.6) THEN
         WRITE(6,1235)
         READ(5,1234,END=2000) ANSW
         I = 0
        ENDIF
        GO TO 2
 1235   FORMAT(' <CR> for continue ',$)
 1234   FORMAT(A)
C
C   File-OUTPUT
C
  200   IF (IO .EQ. 7) THEN
         CLOSE(UNIT=7)
         WRITE(6,1502)
 1502    FORMAT(' ===> Output to printer [Y/N] ? ',$)
         READ(5,1000,END=2000) ANSW
         IF (ANSW.EQ.'Y' .OR. ANSW.EQ.'y') THEN
           CALL SYSTEM('./print text ELEMENT.DAT')
         ENDIF
         GO TO 2000
        ENDIF
C
        WRITE(6,1500)
 1500   FORMAT(' ===> Output to file ELEMENT.DAT [Y/N] ? ',$)
        READ(5,1000,END=2000) ANSW
        IF (ANSW.EQ.'Y' .OR. ANSW.EQ.'y') THEN
         IO = 7
         OPEN(UNIT=7,FILE='ELEMENT.DAT',STATUS='UNKNOWN')
         REWIND 2
         JJ = 3
         IF (TITEL(3:8) .EQ. '*PLOT*') JJ = 9
         WRITE(IO,1501) TITEL(JJ:79)
 1501    FORMAT(1X,A,//)
         GO TO 2
        ELSE
         GO TO 2000
        ENDIF
C
C   ERROR
C
 2001   WRITE(6,*) 'FOR002 not found, run TFO first.'
        GOTO 2000
 2002   WRITE(6,*) 'FOR002 is unformatted, run TFO first.'
 2000   STOP
        END
C
        SUBROUTINE TYPE1(IO,LINE,NRL)
C
        CHARACTER*130 LINE
        CHARACTER*10 IFORM2
        CHARACTER*6  IFORM1
        DATA IFORM1 /'(A130)'/
        DATA IFORM2 /'(12X,A130)'/
C
        IF (LINE(1:1) .EQ. '1') LINE(1:1)=' '
        DO 100 J = 1, 130
         I = 131 - J
         IF (LINE(I:I) .NE. ' ') GO TO 200
  100   CONTINUE
  200   IF (I .GT. 80) GO TO 201
        WRITE(IFORM1(3:5),1001) I
        WRITE(IO,IFORM1) LINE
 1001   FORMAT(I3)
        NRL = 1
        RETURN
  201   WRITE(IO,1002) LINE(1:75)
 1002   FORMAT(A75)
        K = I - 75
        WRITE(IFORM2(7:9),1001) K
        WRITE(IO,IFORM2) LINE(76:I)
        NRL = 2
        RETURN
        END
