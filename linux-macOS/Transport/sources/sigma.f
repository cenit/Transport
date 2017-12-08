      PROGRAM SI
C
      CHARACTER*80 UEBER
      CHARACTER*20 INPUT
      CHARACTER*10 CARD,CARDS
      CHARACTER*2 TESTM,DIM
      CHARACTER*4 LABEL,TESTL
      CHARACTER*130 LINE
      LOGICAL PRINT
      DIMENSION CARD(13),CARDS(13,12)
      EQUIVALENCE(CARD,LINE)
C
      JJ=0
      KK=0
      OPEN(UNIT=2,FILE='FOR002.DAT',STATUS='OLD',ERR=2003)
      READ(2,3010,ERR=2003) UEBER
 3010   FORMAT(A79)
      GO TO 100
C
C   FIND USER CHOICE
C
  100   CONTINUE
      LABEL='%%%%'
      WRITE(6,4000)
 4000   FORMAT(/' Input or HELP<CR> ',$)
      READ(5,4001,END=2400) INPUT
 4001   FORMAT(A20)
C
C   laser printer
C
      IF (INPUT.EQ.'P' .OR. INPUT.EQ.'p') THEN
       PRINT = .TRUE.
       OPEN(UNIT=7,FILE='SIGMA.DAT',STATUS='UNKNOWN')
       JT = 3
       IF (UEBER(3:8) .EQ. '*PLOT*') JT = 9
       WRITE(7,3012) UEBER(JT:79)
 3012    FORMAT(1X,A,//)
       WRITE(6,*) 'Matrices shown on screen will also be printed',
     X              ' on file SIGMA.DAT'
       GO TO 100
      ENDIF
C
C   HELP
C
      IF   (INPUT     .EQ. 'HELP' .OR.
     1      INPUT(1:2).EQ. 'H '   .OR.
     2      INPUT     .EQ. 'help' .OR.
     3      INPUT(1:2).EQ. 'h ')  THEN
       PRINT *,'Legal inputs are:'
       PRINT *,
     1   'z-coordinate in meters or desired label at which the Sigma-',
     2   ' and'
       PRINT *,
     1   'R-matrices have to be shown.  Enter <CR> for showing the',
     2   ' matrices'
       PRINT *,
     1   'of the next element. P<CR> produces a laser printer output',
     2   ' of all'
       PRINT *,
     1   'subsequent shown matrices. Enter HALT or QUIT to exit the',
     2   ' program.'
       GO TO 100
      ENDIF
C
      IF   (INPUT .EQ. 'HALT'    .OR.
     1      INPUT .EQ. 'QUIT'    .OR.
     2      INPUT .EQ. 'halt'    .OR.
     3      INPUT .EQ. 'quit') GO TO 2400
C
      READ(INPUT,4002,ERR=200) ZS
 4002   FORMAT(F20.0)
      GO TO 1
C
  200   READ(INPUT,4003) LABEL
      ZS = 9999.
 4003   FORMAT(A4)
C
C   FINDE PLOT SEKTION
C
    1   IF (JJ .EQ. 1) GO TO 2
      READ(2,1000,END=2000) CARD
 1000   FORMAT(13(A10))
      IF (LINE(1:7) .EQ. '1*PLOT*') GO TO 12
      GO TO 1
C
C   FINDE GESUCHTES Z
C
   12   JJ = 1
    2   IF (KK .EQ. 0) READ(2,1000,END=2001) CARD
        READ(CARD(6),1010) TESTM
 1010   FORMAT(1X,A2,7X)
        READ(CARD(4),1011) TESTL
 1011   FORMAT(3X,A4,3X)
        IF (TESTL .EQ. LABEL) GO TO 14
        IF (LINE(2:8) .EQ. '*SP CH*') GO TO 2
        IF (TESTM.EQ.' M' .OR. TESTM.EQ.' D' .OR. TESTM.EQ.' G') GO TO 3
        IF (TESTM.EQ.' m' .OR. TESTM.EQ.' C' .OR. TESTM.EQ.' c') GO TO 3
        IF (CARD(1)(2:10) .EQ. '     0.00') GO TO 30
        KK = 0
        GO TO 2
    3   READ(2,1000) CARD
   30   READ(LINE(1:14),1012,ERR=2002) ZF,DIM
 1012   FORMAT(F11.3,1X,A2)
        IF (ZF - ZS) 2,4,4
C
C   GESUCHTES Z GEFUNDEN
C
   14   READ(2,1000) CARD
        READ(LINE(1:14),1012,ERR=2002) ZF,DIM
    4   DO 5 M = 1, 5
         READ(2,1000) (CARDS(N,M),N=1,13)
    5   CONTINUE
C
C   AUSSCHREIBEN AUF SCHIRM
C
        IF (DIM .EQ. 'M ') DIM = 'm '
        IF (DIM .EQ. 'CM') DIM = 'cm'
        WRITE(6,3000) ZF,DIM,TESTL
        IF (PRINT) WRITE(7,3000) ZF,DIM,TESTL
 3000   FORMAT(/F10.3,1X,A2,4X,A4,/)
        WRITE(6,3001) (CARD(N),N=8,13)
        IF (PRINT) WRITE(7,3001) (CARD(N),N=8,13)
        DO 6 M = 1, 5
         WRITE(6,3001) (CARDS(N,M),N=8,13)
         IF (PRINT) WRITE(7,3001) (CARDS(N,M),N=8,13)
 3001    FORMAT(7(A10))
    6   CONTINUE
C
   77   READ(2,1000,END=2001) CARD
        IF (CARD(1)(2:10) .NE. '*TRANSFOR') THEN
         KK = 1
         GO TO 100
        ENDIF
C
        WRITE(6,3001) (CARD(N),N=1,2)
        IF (PRINT) WRITE(7,3001) (CARD(N),N=1,2)
        DO 7 M = 7, 12
         READ(2,1000) (CARDS(N,M),N=1,13)
         WRITE(6,3101) (CARDS(N,M),N=2,8)
         IF (PRINT) WRITE(7,3101) (CARDS(N,M),N=2,8)
 3101    FORMAT(6A10,A5)
    7   CONTINUE
        KK = 0
        GO TO 77
C
C   FEHLERMELDUNGEN
C
 2000   WRITE(6,*) 'EOF, *PLOT* on last title-card not found.'
        GO TO 2500
C
 2002   WRITE(6,*) 'Desired sigma-matrix is suppressed.'
        GO TO 2500
C
 2003   WRITE(6,*) 'Run Transport first.'
        GO TO 2500
C
 2001   WRITE(6,*) 'EOF, desired z or label not found.'
        REWIND 2
        JJ = 0
        GO TO 100
C
C   PRINT
C
 2400   IF (PRINT) THEN
         CLOSE(UNIT=7)
         CALL SYSTEM('./print text SIGMA.DAT')
        ENDIF
 2500   STOP
        END

