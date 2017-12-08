      PROGRAM IO
C
C     ORIGINAL VERSION FOR ATARI ST CONVERTED
C     FOR MS-DOS BY URS ROHRER (PSI), JUNE 1989
C
C     FOR MS-FORTRAN V5.1 FOR WINDOWS BY URS ROHRER (PSI), APRIL 1992
C     FOR MS-POWERSTATION FORTRAN BY URS ROHRER (PSI), NOVEMBER 1993
C
C     FOR LINUX, NOVEMBER 1994, BY URS ROHRER, PSI
C
      CHARACTER*20 ARG
      COMMON /REST2/ N
      COMMON /ERROR/ IER
C
      CALL INICOM
      IER = 0
C
      CALL GETARG(1,ARG)
C
      IF (ARG(1:1) .EQ. 'I') THEN
        CALL DISPI
      ELSE IF (ARG(1:4) .EQ. 'BERO') THEN
        CALL BERO
      ELSE IF (ARG(1:4) .EQ. 'QOI2') THEN
        CALL QOI2
      ELSE IF (ARG(1:3) .EQ. 'BOI') THEN
        CALL BOI
      ELSE IF (ARG(1:3) .EQ. 'LOI') THEN
        CALL LOI
      ELSE IF (ARG(1:3) .EQ. 'QOI') THEN
        CALL QOI
      ELSE IF (ARG(1:4) .EQ. 'FXY2') THEN
        CALL FXYGAUSS
      ELSE IF (ARG(1:3) .EQ. 'FXY') THEN
        CALL FXYDAC
      ELSE IF (ARG(1:5) .EQ. 'PIREX') THEN
        CALL PIREXF
      ELSE
        WRITE(*,*) 'IO: unknown switch.'
      ENDIF
C
      IF (IER .NE. 0) THEN
        CALL EXIT(1)
      ELSE
        STOP
      ENDIF
      END

      SUBROUTINE INICOM
C
C     BLOCK DATA
C
      INCLUDE 'incom.f'
C
C     DATA NIN, NOUT /1,6/
      NIN  = 1
      NOUT = 6
C     DATA   UNIT   / .01, .001, .01, .001, .01, .01, .0174533, 1.,
C    A                .1, 1.704523E-3, 3.335641/
      UNIT(1)  = 0.01
      UNIT(2)  = 0.001
      UNIT(3)  = 0.01
      UNIT(4)  = 0.001
      UNIT(5)  = 0.01
      UNIT(6)  = 0.01
      UNIT(7)  = 0.0174533
      UNIT(8)  = 1.0
      UNIT(9)  = 0.1
      UNIT(10) = 1.704523E-3
      UNIT(11) = 3.335641
C     DATA   XDIME  / 'CM  ', 'MR  ', 'CM  ', 'MR  ', 'CM  ', 'PC  ',
C    A                'DEG ', 'M   ', 'KG  ', 'MO  ', 'GEV ' /
      XDIME(1)  = 'CM  '
      XDIME(2)  = 'MR  '
      XDIME(3)  = 'CM  '
      XDIME(4)  = 'MR  '
      XDIME(5)  = 'CM  '
      XDIME(6)  = 'PC  '
      XDIME(7)  = 'DEG '
      XDIME(8)  = 'M   '
      XDIME(9)  = 'KG  '
      XDIME(10) = 'MO  '
      XDIME(11) = 'GEV '
C     DATA   UORIG  / .01, .001, .01, .001, .01, .01, .0174533, 1.,
C    A                .1, 1.704523E-3, 3.335641/
      UORIG(1)  = 0.01
      UORIG(2)  = 0.001
      UORIG(3)  = 0.01
      UORIG(4)  = 0.001
      UORIG(5)  = 0.01
      UORIG(6)  = 0.01
      UORIG(7)  = 0.0174533
      UORIG(8)  = 1.0
      UORIG(9)  = 0.1
      UORIG(10) = 1.704523E-3
      UORIG(11) = 3.335641
C     DATA   XORIG  / 'CM  ', 'MR  ', 'CM  ', 'MR  ', 'CM  ', 'PC  ',
C    A                'DEG ', 'M   ', 'KG  ', 'MO  ', 'GEV ' /
      XORIG(1)  = 'CM  '
      XORIG(2)  = 'MR  '
      XORIG(3)  = 'CM  '
      XORIG(4)  = 'MR  '
      XORIG(5)  = 'CM  '
      XORIG(6)  = 'PC  '
      XORIG(7)  = 'DEG '
      XORIG(8)  = 'M   '
      XORIG(9)  = 'KG  '
      XORIG(10) = 'MO  '
      XORIG(11) = 'GEV '
C     DATA UNTAB / 0.001, 0.01, 1.0, 1000.0, 0.0254, 0.3048,
C    A             1.0, 0.001, 0.0174532925,
C    B             1.0, 0.01, 0.001, 0.001, 1.0E-6,
C    C             0.0001, 0.1, 1.0,
C    D             3.335641E-6, 3.335641E-3, 3.335641, 1.704523E-3 /
      UNTAB(1)  = 0.001
      UNTAB(2)  = 0.01
      UNTAB(3)  = 1.0
      UNTAB(4)  = 1000.0
      UNTAB(5)  = 0.0254
      UNTAB(6)  = 0.3048
      UNTAB(7)  = 1.0
      UNTAB(8)  = 0.001
      UNTAB(9)  = 0.0174532925
      UNTAB(10) = 1.0
      UNTAB(11) = 0.01
      UNTAB(12) = 0.001
      UNTAB(13) = 0.001
      UNTAB(14) = 1.0E-6
      UNTAB(15) = 0.0001
      UNTAB(16) = 0.1
      UNTAB(17) = 1.0
      UNTAB(18) = 3.335641E-6
      UNTAB(19) = 3.335641E-3
      UNTAB(20) = 3.335641
      UNTAB(21) = 1.704523E-3
C     DATA XDTAB / 'MM  ', 'CM  ', 'M   ', 'KM  ', 'IN  ', 'FT  ',
C    A             'R   ', 'MR  ', 'DEG ',
C    B             'N   ', 'PC  ', 'PM  ', 'P/10', 'PPM ',
C    C             'G   ', 'KG  ', 'T   ',
C    D             'KEV ', 'MEV ', 'GEV ', 'MO  ' /
      XDTAB(1)  = 'MM  '
      XDTAB(2)  = 'CM  '
      XDTAB(3)  = 'M   '
      XDTAB(4)  = 'KM  '
      XDTAB(5)  = 'IN  '
      XDTAB(6)  = 'FT  '
      XDTAB(7)  = 'R   '
      XDTAB(8)  = 'MR  '
      XDTAB(9)  = 'DEG '
      XDTAB(10) = 'N   '
      XDTAB(11) = 'PC  '
      XDTAB(12) = 'PM  '
      XDTAB(13) = 'P/10'
      XDTAB(14) = 'PPM '
      XDTAB(15) = 'G   '
      XDTAB(16) = 'KG  '
      XDTAB(17) = 'T   '
      XDTAB(18) = 'KEV '
      XDTAB(19) = 'MEV '
      XDTAB(20) = 'GEV '
      XDTAB(21) = 'MO  '
C     DATA    TABLE / '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
C    A                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
C    B                'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
C    C                'U', 'V', 'W', 'X', 'Y', 'Z'  /
      TABLE(1)  = '0'
      TABLE(2)  = '1'
      TABLE(3)  = '2'
      TABLE(4)  = '3'
      TABLE(5)  = '4'
      TABLE(6)  = '5'
      TABLE(7)  = '6'
      TABLE(8)  = '7'
      TABLE(9)  = '8'
      TABLE(10) = '9'
      TABLE(11) = 'A'
      TABLE(12) = 'B'
      TABLE(13) = 'C'
      TABLE(14) = 'D'
      TABLE(15) = 'E'
      TABLE(16) = 'F'
      TABLE(17) = 'G'
      TABLE(18) = 'H'
      TABLE(19) = 'I'
      TABLE(20) = 'J'
      TABLE(21) = 'K'
      TABLE(22) = 'L'
      TABLE(23) = 'M'
      TABLE(24) = 'N'
      TABLE(25) = 'O'
      TABLE(26) = 'P'
      TABLE(27) = 'Q'
      TABLE(28) = 'R'
      TABLE(29) = 'S'
      TABLE(30) = 'T'
      TABLE(31) = 'U'
      TABLE(32) = 'V'
      TABLE(33) = 'W'
      TABLE(34) = 'X'
      TABLE(35) = 'Y'
      TABLE(36) = 'Z'
C     DATA    PLUS  / '+', '&' /
      PLUS(1) = '+'
      PLUS(2) = '&'
C     DATA    MINUS / '-' /
      MINUS = '-'
C     DATA    BLANK / ' ' /
      BLANK = ' '
C     DATA    PERIOD/ '.' /
      PERIOD = '.'
C      DATA    SEMI  / ';', ',', '*', '!', '?' /
      SEMI(1) = ';'
      SEMI(2) = ','
      SEMI(3) = '*'
      SEMI(4) = '!'
      SEMI(5) = '?'
C     DATA    QUOTE / '/', '''', '#', '$', '=', '"' /
      QUOTE(1) = '/'
      QUOTE(2) = ''''
      QUOTE(3) = '#'
      QUOTE(4) = '$'
      QUOTE(5) = '='
      QUOTE(6) = '"'
C     DATA    PAR1  / '(', '<' /
      PAR1(1) = '('
      PAR1(2) = '<'
C     DATA    PAR2  / ')', '>' /
      PAR2(1) = ')'
      PAR2(2) = '>'
C
C     DATA    MULT  / 1001 * 0 /
      DO 10 J = 1, 1001
10    MULT(J) = 0
C
      RETURN
      END

      SUBROUTINE RDELMT
C
C     READ IN DATA FOR ONE ELEMENT
C
      INCLUDE 'incom.f'
C
      INTEGER      NO1(25)
      LOGICAL      SNTNL, SEPAR
      SAVE         SNTNL, NUC
      CHARACTER*1  IARROW, IWORD(4)
      DATA SNTNL  / .FALSE. /
      DATA NO1    / 8,2,2,4,4,5,7,8,2,5,3,16,2,8,4,4,1,4,3,2,3,5,4,4,5/
      DATA IARROW / '^' /
      DATA NUC    / 0 /
C
C     SET DEFAULT VALUES
C
   10 DO 20 JJ  = 1, 30
      DATUM(JJ) = 0.0
   20 VARY (JJ) = 0
      NWORD = 0
      NVARY = 1
      LABLE = '    '
C
C     HAS SENTINEL BEEN READ AT PREVIOUS CALL
C
      IF (SNTNL) THEN
       SNTNL = .FALSE.
       RETURN
      ENDIF
      SEPAR = .TRUE.
C
      IF (ITEM .EQ. PAR1(1) .OR. ITEM .EQ. PAR1(2))
     1 CALL RDSKIP(NOUT,ITEM,PAR1,PAR2,QUOTE,MC,CARD)
      GO TO 110
C
C     SEPARATORS
C
  100 SEPAR = ITEM .EQ. BLANK
      CALL RDNEXT (-1)
  110 CONTINUE
C
      IF (ITEM .EQ. SEMI(1)) THEN
       GO TO 500
      ELSE IF (ITEM .EQ. QUOTE(1)) THEN
       GO TO 350
      ELSE IF (ITEM .EQ. SEMI(2)) THEN
       GO TO 500
      ELSE IF (ITEM .EQ. QUOTE(2)) THEN
       GO TO 350
      ELSE IF (ITEM .EQ. SEMI(3)) THEN
       GO TO 500
      ELSE IF (ITEM .EQ. SEMI(4)) THEN
       GO TO 500
      ELSE IF (ITEM .EQ. SEMI(5)) THEN
       GO TO 500
      ELSE IF (ITEM .EQ. QUOTE(3)) THEN
       GO TO 350
      ELSE IF (ITEM .EQ. QUOTE(4)) THEN
       GO TO 350
      ELSE IF (ITEM .EQ. QUOTE(5)) THEN
       GO TO 350
      ELSE IF (ITEM .EQ. QUOTE(6)) THEN
       GO TO 350
      ELSE IF (ITEM .EQ. TABLE(29)) THEN
       GO TO 450
      ELSE
       CONTINUE
      ENDIF
C
      IF (SEPAR) GO TO 150
  130  WRITE (NOUT, 9130) CARD, (BLANK, JJ = 1, MC), IARROW
       WRITE(6,*) 'error in FOR001.DAT'
       STOP
  150 IF (NWORD) 300, 200, 300
C
C     TYPE CODE
C
  200 CALL RDFIX(NTYPE,IFLAG,ITEM,MINUS,PLUS,PERIOD,TABLE,MC,CARD)
      IF (IFLAG .NE. 0) GO TO 130
      NWORD    = 1
      DATUM(1) = NTYPE
      NTYPE    = IABS(NTYPE)
      IF (ITEM .NE. PERIOD) GO TO 100
C
C     VARY CODES
C
  210 MC    = MC + 1
      ITEM  = CARD(MC)
      IF (ITEM .EQ. BLANK) GO TO 100
      IFLAG = 0
      IF (ITEM .EQ. MINUS) GO TO 220
      ISIG  = 1
      IF (ITEM .EQ. PLUS(1) .OR. ITEM .EQ. PLUS(2)) GO TO 230
      GO TO 240
  220 ISIG  = - 1
  230 MC    = MC + 1
      ITEM  = CARD(MC)
      IFLAG = 1
  240 DO 250 JJ = 1, 36
       IF (ITEM .EQ. TABLE(JJ)) GO TO 260
  250 CONTINUE
      IF (IFLAG) 130, 100, 130
  260 IF (NVARY .GE. 30) GO TO 210
      NVARY = NVARY + 1
      VARY(NVARY) = ISIGN(JJ - 1, ISIG)
      GO TO 210
C
C     DATA VALUE
C
  300 CALL RDFLT(VALUE,IFLAG,ITEM,MINUS,PLUS,PERIOD,TABLE,MC,CARD)
      IF (IFLAG .NE. 0) GO TO 130
      NWORD = NWORD + 1
      IF (NWORD .LE. 30) DATUM(NWORD) = VALUE
      GO TO 100
C
C     LABEL
C
  350 CALL RDSTRG(IWORD,4,LL,ITEM,PAR1,PAR2,QUOTE,MC,CARD)
      IF (LL .GT. 4) WRITE (NOUT, 9350)
      SEPAR = .TRUE.
      IF (NWORD .EQ. 2 .AND. NTYPE .EQ. 15) GO TO 360
      IF (LABLE .NE. BLANK) WRITE (NOUT, 9360) LABLE
      DO 355 K = 1, LL
       LABLE(K:K) = IWORD(K)
  355 CONTINUE
      GO TO 110
  360 NUC = NUC + 1
      DSTOR(NUC) = '    '
      DO 370 K = 1, LL
       DSTOR(NUC)(K:K) = IWORD(K)
  370 CONTINUE
      DATUM(3) = NUC
      NWORD = 3
      GO TO 110
C
C     SENTINEL
C
  450 EMPTY = .TRUE.
      IF (NWORD .EQ. 0) RETURN
      SNTNL = .TRUE.
      GO TO 502
C
C     CHECK VALIDITY OF ELEMENT JUST READ
C
  500 IF(NWORD .EQ. 0) GO TO 130
      CALL RDNEXT (1)
  502 IF(NTYPE .EQ. 0) GO TO 510
      IF(NTYPE .LE. 25) GO TO 530
      IF(NTYPE .EQ. 27) GO TO 540
      IF(NTYPE .EQ. 29) GO TO 540
      IF(NTYPE .EQ. 30) GO TO 540
      IF(NTYPE .LT. 50) GO TO 510
      IF(NTYPE .GT. 53) GO TO 510
      LENGTH = 5
      IF(NTYPE .EQ. 53) LENGTH = 1
      IF(NTYPE .EQ. 53) VARY(1)=VARY(2)
      GO TO 535
  510 WRITE (NOUT, 9510)
      FLUSH = .TRUE.
      GO TO 700
  530 LENGTH = NO1(NTYPE)
      IF(NTYPE .NE. 16) GO TO 535
      IF(DATUM(2) .GE. 0.0 .AND. DATUM(2) .LT. 201.0) GO TO 535
      LENGTH = 4
      GO TO 10
  535 IF(NWORD - LENGTH) 600, 700, 550
  540 IF(NTYPE.EQ.27) LENGTH = 4
      IF(NTYPE.EQ.29) LENGTH = 8
      IF(NTYPE.EQ.30) LENGTH = 12
      GO TO 535
C
C     DATA OVERFLOW
C
  550 IF(DATUM(9) .NE. 0.0) GO TO 560
      IF(NTYPE .EQ. 1) LENGTH = 9
      IF(NTYPE .EQ. 14) LENGTH = 30
      IF(NWORD - LENGTH) 600, 700, 560
  560 CALL PRINT1(NOUT,LABEL,LENGTH,DATUM,VARY)
      WRITE (NOUT, 9560)
      GO TO 690
C
C     FILL IN INCOMPLETE 15 ELEMENT
C
  600 IF (INDIC .NE. 0) GO TO 700
      IF (NTYPE .EQ. 15) CALL RDUNIT
     1 (FLUSH,.TRUE.,NOUT,NWORD,DATUM,DSTOR,XDTAB,UNTAB,UORIG)
  690 NWORD = LENGTH
C
C
  700 IF (FLUSH) GO TO 10
      RETURN
C
 9130 FORMAT(/' SCANNING STOPS DUE TO ERROR AT POSITION SHOWN BELOW'/
     A       11X,83A1/10X,84A1)
 9350 FORMAT(/' NEXT LABEL TRUNCATED TO 4 CHARS.')
 9360 FORMAT(/' LABEL "',A4,'" ON NEXT ELEMENT WAS OVERWRITTEN')
 9500 FORMAT(/' NEXT ELEMENT IS FOR TURTLE ONLY - IGNORED')
 9510 FORMAT(/' NEXT ELEMENT IS ILLEGAL - RUN FLUSHED')
 9560 FORMAT(/' DATA LIST FOR NEXT ELEMENT IS TOO LONG')
 9650 FORMAT(/' ERROR ON FOLLOWING 15. ELEMENT')
 9660 FORMAT(/
     1' ATTEMPT TO VARY 1ST PARAMETER OF 16. CARD - RUN FLUSHED')
      END

      SUBROUTINE RDFIX
     1 (IVALUE,IFLAG,ITEM,MINUS,PLUS,PERIOD,TABLE,MC,CARD)
C
C     READ AN INTEGER
C
      CHARACTER*1 ITEM, MINUS, PLUS(2), PERIOD, TABLE(36), CARD(120)
C
      IFLAG = - 1
      IVAL = 0
      IF (ITEM .EQ. MINUS) GO TO 10
      ISIG = 1
      IF (ITEM .EQ. PLUS(1) .OR. ITEM .EQ. PLUS(2)) GO TO 20
      GO TO 30
   10 ISIG = - 1
   20 MC   = MC + 1
      ITEM = CARD(MC)
   30 DO 40 JJ = 1, 10
      IF (ITEM .EQ. TABLE(JJ)) GO TO 50
   40 CONTINUE
      IVALUE = ISIGN (IVAL, ISIG)
      RETURN
   50 IVAL = 10 * IVAL + JJ - 1
      IFLAG = 0
      GO TO 20
C
      END

      SUBROUTINE RDFLT(VALUE,IFLAG,ITEM,MINUS,PLUS,PERIOD,TABLE,MC,CARD)
C
C     READ A FLOATING-POINT NUMBER
C
      CHARACTER*1 ITEM, MINUS, PLUS(2), PERIOD, TABLE(36), CARD(120)
C
      IFLAG = - 1
      VAL = 0.0
      NPL = 0
      JPL = 0
      NEX = 0
      IF (ITEM .EQ. MINUS) GO TO 10
      SIG = 1.0
      IF (ITEM .EQ. PLUS(1) .OR. ITEM .EQ. PLUS(2)) GO TO 20
      GO TO 30
   10 SIG  = - 1.0
   20 MC   = MC + 1
      ITEM = CARD(MC)
   30 DO 40 JJ = 1, 10
      IF (ITEM .EQ. TABLE(JJ)) GO TO 50
   40 CONTINUE
      IF (ITEM .NE. PERIOD) GO TO 70
      IF (JPL .NE. 0) GO TO 60
      JPL = 1
      FF = 0.1
      GO TO 20
   50 IFLAG = 0
      IF (JPL .NE. 0) GO TO 55
      VAL = 10.0 * VAL + FLOAT(JJ - 1)
      GO TO 20
   55 VAL = VAL + FF * FLOAT(JJ - 1)
      FF  = 0.1 * FF
      GO TO 20
   60 IFLAG = 1
      GO TO 90
   70 IF (ITEM .NE. TABLE(15)) GO TO 80
      MC   = MC + 1
      ITEM = CARD(MC)
      CALL RDFIX(NEX,IFLAG,ITEM,MINUS,PLUS,PERIOD,TABLE,MC,CARD)
      GO TO 90
   80 IF (ITEM .EQ. PLUS(1) .OR. ITEM .EQ. PLUS(2) .OR. ITEM .EQ. MINUS)
     1   CALL RDFIX(NEX,IFLAG,ITEM,MINUS,PLUS,PERIOD,TABLE,MC,CARD)
   90 VALUE = SIGN (VAL, SIG)
      IF(NEX .NE. 0) VALUE = VALUE * 10.0 ** NEX
      RETURN
      END

      SUBROUTINE RDNEXT (ISKIP)
C
C     READ NEXT CHARACTER, OPTIONNALY SKIPPING BLANKS
C
      INCLUDE 'incom.f'
C
      IF (ISKIP .LT. 0) GO TO 40
      IF (ISKIP .GE. 2) GO TO 20
      IF (EMPTY)        GO TO 20
   10 IF (MC .LT. 120)  GO TO 30
      IF (ISKIP .EQ. 0) THEN
       EMPTY = .TRUE.
       ITEM  = BLANK
       RETURN
      ENDIF
   20 MC = 0
      EMPTY = .FALSE.
      READ (NIN, 100, END = 70, ERR = 80) CARD
C
C     SPECIAL (overwrite CR and/or LF from Windows format)
C
      DO 25 IJK = 1, 120
       IF (CARD(IJK) .EQ. CHAR(13)) CARD(IJK) = BLANK
       IF (CARD(IJK) .EQ. CHAR(10)) CARD(IJK) = BLANK
   25 CONTINUE   
C
      NCD  = NCD + 1
   30 MC   = MC + 1
      ITEM = CARD(MC)
      IF (ISKIP .EQ. 0) RETURN
   40 IF (ITEM .EQ. BLANK) GO TO 10
      IF (ITEM.EQ.TABLE(29) .AND. ISKIP.EQ.2) CONTINUE !!!!!!
      RETURN
C
   70 WRITE(6,*) 'FOR001.DAT EOF encountered'
      STOP
   80 WRITE(6,*) 'FOR001.DAT read error'
      STOP
C
  100 FORMAT(120A1)
      END

      SUBROUTINE RDSKIP(NOUT,ITEM,PAR1,PAR2,QUOTE,MC,CARD)
C
C     SKIP COMMENTS
C
      CHARACTER*1 LINE(120), ITEM, PAR1(2), PAR2(2), QUOTE(6)
      CHARACTER*1 CARD(120)
C
   10 CALL RDSTRG(LINE,80,LL,ITEM,PAR1,PAR2,QUOTE,MC,CARD)
      IF (LL .GT. 80) WRITE (NOUT, 100)
      CALL WRCOMM(LINE,LL)
      IF (ITEM .EQ. PAR1(1) .OR. ITEM .EQ. PAR1(2)) GO TO 10
      RETURN
C
  100 FORMAT(' COMMENT TRUNCATED TO 80 CHARS.')
  110 FORMAT(' (',80A1,')')
      END

      SUBROUTINE RDSTRG(STRING,LMAX,LL,ITEM,PAR1,PAR2,QUOTE,MC,CARD)
C
C     READ A CHARACTER STRING
C
      CHARACTER*1 STRING(*), CHAR(4), ISTOP, ITEM, PAR1(2), PAR2(2)
      CHARACTER*1 QUOTE(6), CARD(120)
C
      DO 10 LL = 1, LMAX
   10 STRING(LL) = ' '
      LL = 0
      IF      (ITEM .EQ. QUOTE(1)) THEN
       ISTOP = QUOTE(1)
      ELSE IF (ITEM .EQ. PAR1(1)) THEN
       ISTOP = PAR2(1)
      ELSE IF (ITEM .EQ. QUOTE(2)) THEN
       ISTOP = QUOTE(2)
      ELSE IF (ITEM .EQ. PAR1(2)) THEN
       ISTOP = PAR2(2)
      ELSE IF (ITEM .EQ. QUOTE(3)) THEN
       ISTOP = QUOTE(3)
      ELSE IF (ITEM .EQ. QUOTE(4)) THEN
       ISTOP = QUOTE(4)
      ELSE IF (ITEM .EQ. QUOTE(5)) THEN
       ISTOP = QUOTE(5)
      ELSE IF (ITEM .EQ. QUOTE(6)) THEN
       ISTOP = QUOTE(6)
      ELSE
       RETURN
      ENDIF
C
   40 MC   = MC + 1
      IF (CARD(MC) .EQ. ISTOP) GO TO 80
      LL   = LL + 1
      STRING(LL) = CARD(MC)
      GO TO 40
C
   80 CALL RDNEXT(1)
      RETURN
      END

      SUBROUTINE RDUNIT(FLH,FIO,NO,NWORD,DATUM,DSTOR,XDTAB,UNTAB,UORIG)
C
      LOGICAL     FIO, FLH
      CHARACTER*4 DSTOR(20), XDTAB(21), TEXT
      DIMENSION   DATUM(30), UNTAB(21), UORIG(11)
C
      JJ = DATUM(2)
      NUC = DATUM(3)
      GO TO (10, 20, 30, 40), NWORD
C
   20 IF (JJ .EQ. 0) GO TO 10
      IF (FIO) WRITE (NO, 910)
  910 FORMAT(/' BAD 15. CARD FOUND - RUN FLUSHED')
      FLH = .TRUE.
      RETURN
C
   10 DATUM(2) = 0.0
      DSTOR(NUC) = '    '
      DATUM(4) = 0.0
   40 RETURN
C
   30 GO TO (110, 120, 110, 120, 110, 130, 120, 110, 140, 150, 160), JJ
C
C     UNITS FOR LENGTHS
C
  110 K1 = 1
      K2 = 6
      GO TO 200
C
C     UNITS FOR ANGLES
C
  120 K1 = 7
      K2 = 9
      GO TO 200
C
C     UNITS FOR DP/P
C
  130 K1 = 10
      K2 = 14
      GO TO 200
C
C     UNITS FOR MAGNETIC FLUX DENSITY
C
  140 K1 = 15
      K2 = 17
      GO TO 200
C
C     UNITS FOR PARTICLE MASSES
C
  150 K1 = 18
      K2 = 21
      GO TO 200
C
C     UNITS FOR PARTICLE MOMENTUM
C
  160 K1 = 18
      K2 = 20
C
  200 TEXT = DSTOR(NUC)
      DO 210 K = K1, K2
       IF (TEXT .EQ. XDTAB(K)) GO TO 220
  210 CONTINUE
      IF (FIO) WRITE (NO, 920) TEXT, JJ
  920 FORMAT(/' UNIT "',A4,'" IS MEANINGLESS ON 15.',I3,
     A       '. CARD - RUN FLUSHED')
      DATUM(4) = 1.0
      FLH = .TRUE.
      RETURN
C
  220 DATUM(4) = UNTAB(K) / UORIG(JJ)
      RETURN
      END

      SUBROUTINE WRCOMM(LINEX,LL)
C
      INCLUDE 'incom.f'
C
      CHARACTER*1  LINEX(80)
      INTEGER      IENTRY, LPOINT
      SAVE         IENTRY, LPOINT
      DATA         IENTRY, LPOINT /0,0/
C
      IF (IENTRY .GE. 1000) RETURN
      IENTRY = IENTRY + 1
      DO 20 J = 1, LL
       IF (LPOINT .GE. 4000) RETURN
       LPOINT = LPOINT + 1
       ICOMM(LPOINT) = LINEX(J)
   20 CONTINUE
      LENG(IENTRY) = LPOINT
      N = MAX(1,NEL)
      MULT(N) = MULT(N) + 1
      RETURN
      END

      SUBROUTINE CITA(IN,CAR,N)
C
C     CONVERTS A INTEGER INTO N LEFTBOUND CHARACTERS
C     LIMITS: +-99999999
C
      CHARACTER*1  CHAR(12),DIGIT(10),MINUS,STAR,BLANK
      CHARACTER*12 CAR
      DATA         MINUS,STAR,BLANK/'-','*',' '/
      DATA         DIGIT/'0','1','2','3','4','5','6','7','8','9'/
C
      M = IN
      N = 1
C
C     INSPECT FOR SIGN AND OVERFLOW
C
      IF (M .GE. 0) GO TO 10
      M = -M
   10 IF (M .GE. 10**8) GO TO 60
C
C     CONVERT INTO CHARACTERS
C
      DO 20 J = 1, 9
       K = 9 - J
       I = M / 10**K
       CHAR(N) = DIGIT(I+1)
       M = M - I * 10**K
       N = N + 1
   20 CONTINUE
C
C     L LEADING CHARACTERS ARE ZERO
C
      DO 30 L = 1, 8
      IF (CHAR(L) .NE. '0') GO TO 40
   30 CONTINUE
   40 L = L - 1
C
C     COMPRESSION
C
      K = 0
      N = 9 - L
      IF (IN .GE. 0) GO TO 50
      CHAR(1) = MINUS
      K = 1
   50 DO 55 J = 1, N
   55 CHAR(J+K) = CHAR(J+L)
      IF (IN .LT. 0) N = N + 1
      GO TO 70
C
C     OVERFLOW
C
   60 N       = 1
      CHAR(1) = STAR
C
C     TRANSFORMATION INTO CHARACTER*12
C
   70 WRITE(CAR,1000) CHAR
 1000 FORMAT(12A1)
      CAR(N+1:12) = BLANK
      RETURN
      END

      SUBROUTINE CFTA(F,CARR,N)
C
C     CONVERTS FLOATING NUMBER INTO N LEFTBOUND CHARACTERS
C     LIMITED TO UP TO 6 SIGNIFICANT DIGITS
C
      CHARACTER*1  CHAR(12)
      CHARACTER*12 CAR, CARR
      EQUIVALENCE  (CHAR,CAR)
C
      X = F
      N = 1
C
C     INSPECT FOR SIGN AND OVERFLOW
C
      IF (X .LT. 0.0) X = -X
      IF (X .GE. 1.E+5) GO TO 80
C
C     CONVERT INTO CHARACTERS
C
      WRITE(CAR,1001) X
1001  FORMAT(F12.5)
C
C     M LEADING CHARACTERS ARE BLANK
C
      DO 30 M = 1, 6
       IF (CHAR(M) .NE. ' ') GO TO 40
   30 CONTINUE
   40 M = M - 1
C
C     I TRAILING CHARACTERS ARE ZERO
C
      DO 50 I = 1, 4
       L = 13 - I
       IF (CHAR(L) .NE. '0') GO TO 60
   50 CONTINUE
   60 I = I - 1
C
C     COMPRESSION
C
      L = 0
      N = 12 - M - I
      IF (F .LT. 0.0) THEN
       CHAR(1) = '-'
       L = 1
      ENDIF
      DO 75 J = 1, N
       CHAR(J+L) = CHAR(J+M)
   75 CONTINUE
      N = MIN0(7,N)             !       NOT MORE THAN 7 DIGITS
      IF (F .LT. 0.0) N = N + 1
      GO TO 90
C
C     EXPONENTIAL FORMAT
C
   80 N = 11
      WRITE(CARR(1:11),1002) F
 1002 FORMAT(E11.4)
      GO TO 99
C
C     COPY IN RESULT BUFFER
C
   90 DO 95 J = 1, N
       CARR(J:J) = CHAR(J)
   95 CONTINUE
   99 CARR(N+1:N+1) = ' '
      RETURN
      END

      SUBROUTINE CATA(INT,CHAR,CAR)
C
C     CONVERTS I*4 INTO CHARACTER*4 AND GIVES FIRST CHARACTER
C     AS CHARACTER*1 VARIABLE.
C
      CHARACTER*4 CHAR
      CHARACTER*1 CAR
C
      WRITE(CHAR,1000) INT
      CAR = CHAR(1:1)
 1000 FORMAT(A)
      RETURN
      END

      SUBROUTINE PRINT1 (IO,LLABEL,MWORD,XDATA,IVARY)
C
C     PRINT DATA FOR ONE ELEMENT
C
      INCLUDE 'incom.f'
C
      CHARACTER*4   LLABEL
      INTEGER       IVARY(30),NV(25), IWORD(7)
      REAL          XDATA(30)
      CHARACTER*283 OUT
      CHARACTER*83  BLNK
      CHARACTER*30  CARL
      CHARACTER*12  NUMR,CVR(40)
      CHARACTER*6   IFORM
      CHARACTER*4   IDATA
      CHARACTER*7   LABP
      CHARACTER*1   CHAR(30)
      INTEGER       NC(40)
      DATA          BLNK /'                                              
     *                                      '/
      DATA          IFORM /'(A83)'/
      DATA NV  /6,1,1,3,2,0,0,6,0,1,0,15,0,6,0,2,0,2,2,1,2,0,1,0,0/
      DATA IWORD /9,6,5,4,3,2,1/
C
C     CONVERT INTERNAL IN EXTERNAL VARY CODES
C
      OUT   = BLNK
      NTYPE = XDATA(1)
      KTYPE = IABS(NTYPE)
      DO 10 J = 1,30
   10 CHAR(J) = BLANK
      IF(NTYPE .LE. 0) GO TO 40
      KV = 1
      IF(NTYPE .LE. 22) KV = NV(NTYPE)
      IF(KV .EQ. 0) GO TO 40
      LV = 0
      DO 30 JV = 1, KV
      JJJ=IVARY(JV+1)
      IF(NTYPE.EQ.53) JJJ=IVARY(JV)
      K = IABS(JJJ)
      IF(JJJ .GE. 0) GO TO 20
      LV = LV + 1
      CHAR(LV) = MINUS
   20 LV = LV + 1
   30 CHAR(LV) = TABLE(K + 1)
C
C     TYPE.VARYCODES
C
   40 CALL TC30(CHAR,CARL,J)
      CALL CITA(NTYPE,NUMR,NN)
      OUT(1:NN)        = NUMR(1:NN)
      OUT(NN+1:NN+1)   = '.'
      OUT(NN+2:NN+1+J) = CARL(1:J)
      N1               = NN + 2 + J
C
C     ELEMENT DATA
C
      IF (MWORD .LE. 1) GO TO 65
      IF (KTYPE .EQ.15) GO TO 55
      DO 50 J = 2, MWORD
       CALL CFTA(XDATA(J),CVR(J),NC(J))
   50 CONTINUE
      GO TO 60
C
   55 CALL CFTA(XDATA(2),CVR(2),NC(2))
      CALL CFTA(XDATA(4),CVR(4),NC(4))
      JJJJ  = XDATA(3)
      IDATA = DSTOR(JJJJ)
      CALL COLIS(IDATA,CVR(3),NC(3))
C
   60 DO 70 J = 2, MWORD
       N2         = N1 + NC(J)
       OUT(N1:N2) = CVR(J)(1:NC(J)+1)
       N1         = N2 + 1
   70 CONTINUE
C
C     LABEL AND SEMI
C
   65 CALL COLIS(LLABEL,LABP,NL)
      IF (NL .EQ. 0) GO TO 75
      OUT(N1:N1+NL) = LABP(1:1+NL)
      N1            = N1 + NL + 1
   75 OUT(N1:N1)    = ';'
C
C     PRINT ELEMENT
C
      IF (N1 .LT. 100) THEN
       WRITE(IFORM(3:4),1002) N1
 1002  FORMAT(I2)
       WRITE(IO,IFORM) OUT(1:N1)
      ELSE IF (MWORD .EQ. 30) THEN  ! ARBITRAY MATRIX FOR SECOND ORDER
       K = 1                        ! CHAR COUNTER
       M = 0                        ! WORD COUNTER
       DO 110 I = 1, 6
        DO 100 J = K, N1
         IF (OUT(J:J).NE.' ' .AND. OUT(J+1:J+1).EQ.' ') M = M + 1
         IF (M .EQ. IWORD(I)) THEN
          WRITE(IFORM(3:4),1002) J - K + 1
          WRITE(IO,IFORM) OUT(K:J)  ! PRINT WORD GROUP
          K = J + 1
          M = 0
          GO TO 101
         ENDIF
  100   CONTINUE
  101   CONTINUE
  110  CONTINUE
       IF (K .LT. N1) THEN
        WRITE(IFORM(3:4),1002) N1 - K + 1
        WRITE(IO,IFORM) OUT(K:N1) ! PRINT LAST WORD GROUP WITH LABEL AND ;
       ENDIF
      ELSE
       CONTINUE
      ENDIF
C
      RETURN
      END

      SUBROUTINE TC30(CHAR,CARL,KV)
C
C     CONCATINATES CHAR(1) .. CHAR(KV) INTO CARL(1:KV)
C
      CHARACTER*1  CHAR(30)
      CHARACTER*30 CARL
      CHARACTER*4  CAR
C
      DO 2 K = 30,1,-1
       IF (CHAR(K).NE.' ' .AND. CHAR(K).NE.'0') GO TO 3
    2 CONTINUE
      KV = 0
      GO TO 6
    3 KV = K
    4 DO 5 K = 1, KV
       CARL(K:K) = CHAR(K)
    5 CONTINUE
    6 KV = KV + 1
      CARL(KV:KV) = ' '
      RETURN
      END

      SUBROUTINE COLIS(CAR,LAB,N)
C
C     CONVERTS LABEL INTO STRING OF TYPE /ABCD/
C
      CHARACTER*7 LAB
      CHARACTER*4 CAR
C
      LAB(1:7) = '       '
      DO 1 J = 1, 4
       K = 5 - J
       IF (CAR(K:K) .NE. ' ') GO TO 2
    1 CONTINUE
      N = 0
      RETURN
    2 LAB(1:1)     = '/'
      LAB(2:K+1)   = CAR(1:K)
      LAB(K+2:K+2) = '/'
      N = K + 2
      RETURN
      END

      SUBROUTINE READD(SLITS)
C
      INCLUDE 'incom.f'
C
      COMMON /REST2/  N
C
      LOGICAL SLITS
      INTEGER NO1(53)
      DATA IZERO /  0 /
      DATA NO1/8,2,2,4,4,5,7,8,2,5,3,16,2,8,4,4,1,4,3,2,3,5,4,4,5
     X          ,0,4,0,8,12,19*0,3*4,1/
 
      SLITS = .FALSE.
C
C     OPEN FILE 
C
      OPEN (UNIT=NIN,FILE='FOR001.DAT',STATUS='OLD',ERR=60)
C
C     READ TITLE AND INDICATOR
C
      CALL RDNEXT(2)
      IF (ITEM .EQ. PAR1(1) .OR. ITEM .EQ. PAR1(2))
     1   CALL RDSKIP(NOUT,ITEM,PAR1,PAR2,QUOTE,MC,CARD)
      CALL RDSTRG(IMAGE,80,LW,ITEM,PAR1,PAR2,QUOTE,MC,CARD)
      IF (ITEM .EQ. PAR1(1) .OR. ITEM .EQ. PAR1(2))
     1   CALL RDSKIP(NOUT,ITEM,PAR1,PAR2,QUOTE,MC,CARD)
      CALL RDFIX(INDIC,IFLAG,ITEM,MINUS,PLUS,PERIOD,TABLE,MC,CARD)
      CALL RDNEXT(-1)
C
C     READ OLD SYSTEM
C
      NEL = 0
      I = 1
      ISTOR(1) = 1
      FLUSH = .FALSE.
  110 CALL RDELMT
      IF(NWORD .EQ. 0) GO TO 400
      IF(FLUSH) GO TO 110
      NEL = NEL + 1
      IF(NEL .GE. 1000 .OR. I + LENGTH .GE. 4000) GO TO 150
      LABEL(NEL) = LABLE
      DO 120 J = 1, LENGTH
      DATA(I) = DATUM(J)
      TIE (I) = VARY (J)
  120 I = I + 1
      DATA(I) = 0.0
      ISTOR(NEL+1) = I
      IF(IFIX(DATUM(1)).EQ.16 .AND. IFIX(DATUM(2)).EQ.14) SLITS=.TRUE.
      GO TO 110
  150 WRITE (6, 9150) NEL, I
      FLUSH = .TRUE.
      GO TO 110
  400 CONTINUE
C
C     READ REST OF FILE
C
      N=1
  600 READ(NIN,1000,END=650) CARDS(N)
       N=N+1
      GO TO 600
  650 N=N-1
C
      CLOSE(UNIT=NIN)
      RETURN
C
   60 WRITE(6,*) 'FOR001.DAT not found'
      STOP
      RETURN
c
 1000 FORMAT(A80)
 9150 FORMAT(' DATA OVERFLOW, NEL =',I5,',  I =',I5)
      END

      SUBROUTINE WRITED(IFLAG)
C
      INCLUDE 'incom.f'
C
      COMMON /REST2/  N
C
      INTEGER         NO1(53)
      CHARACTER*15    IFORM1, IFORM4
      CHARACTER*5     IFORM2, IFORM5
      CHARACTER*8     IFORM3
      CHARACTER*1     ANS
      DATA NO1/8,2,2,4,4,5,7,8,2,5,3,16,2,8,4,4,1,4,3,2,3,5,4,4,5
     X          ,0,4,0,8,12,19*0,3*4,1/
      DATA IFORM1 /'(''/'',80A1,''/'') '/
      DATA IFORM2 /'(A80)'/
      DATA IFORM4 /'(''('',80A1,'')'') '/
      DATA IFORM5 /'(I10)'/
      DATA IENTRY /0/
      DATA ISTART /1/
C
C     OPEN FILE FOR WRITE
C
      IF (IFLAG .NE. 0) THEN
       WRITE(6,1234) 
 1234  FORMAT(' Updating FOR001.DAT [Y/N] ? ',$)
       READ(5,'(A1)',ERR=1999,END=1999) ANS
       IF (ANS.EQ.'N' .OR. ANS.EQ.'n') GO TO 700
      ENDIF
C
      OPEN(UNIT=NIN,FILE='FOR001.DAT',STATUS='UNKNOWN')
      REWIND(UNIT=NIN)
C
C     WRITE TITLE
C
      DO 1 J=80,1,-1
       IF(IMAGE(J) .NE. ' ') GO TO 2
    1 CONTINUE
    2 NN=J
      WRITE(IFORM1(6:7),9000) NN
      WRITE(NIN,IFORM1) (IMAGE(J), J=1,NN)
C
C     WRITE INDICATOR OR RAYS
C
      IF(INDIC.GE.10) NN=2.+LOG10(FLOAT(INDIC)+.1)
      IF(INDIC.LT.10) NN=2
      WRITE(IFORM5(3:4),9000) NN
      WRITE(NIN,IFORM5) INDIC
C
C     WRITE ELEMENTS
C
      DO 500 NUM = 1, NEL
       I      = ISTOR(NUM)
       ITYPE  = DATA(I)
       ITYPE  = IABS(ITYPE)
       LABLE  = LABEL(NUM)
       LENGTH = NO1(ITYPE)
       IF (ITYPE.EQ.6 .AND. DATA(I+3).EQ.0.0)     LENGTH = 3
       IF (ITYPE.EQ.16 .AND. DATA(I+3).EQ.0.0)    LENGTH = 3
       IF (ITYPE .EQ. 1 .AND. DATA(I+8) .EQ. 0.0) LENGTH = 9
       IF (ITYPE .EQ.13 .AND. DATA(I+1) .EQ. 0.0) LENGTH = 1
       IF (ITYPE .EQ.14 .AND. DATA(I+8) .EQ. 0.0) LENGTH = 30
       CALL PRINT1(NIN,LABLE,LENGTH,DATA(I),TIE(I))
C
C     WRITE STORED COMMENT IN PARENTESES
C
       IF (MULT(NUM) .NE. 0) THEN
        DO 50 J = 1, MULT(NUM)
         IENTRY = IENTRY + 1
         IL = LENG(IENTRY)
         L = IL - ISTART + 1
         WRITE (IFORM4(6:7), 9000) L
         WRITE (NIN, IFORM4) (ICOMM(K), K =ISTART, IL)
   50   ISTART = IL + 1
       END IF
  500 CONTINUE
C
C     TRANSFER REST OF FILE (AFTER FIRST SENTINEL)
C
      WRITE(NIN,9010)
      DO 600 L = 1, N
       DO 610 J = 1, 80
        K = 81 - J
        IF (CARDS(L)(K:K) .NE. ' ') GO TO 611
  610  CONTINUE
  611  WRITE(IFORM2(3:4),9000) K
       WRITE(NIN,IFORM2) CARDS(L)
  600 CONTINUE
C
      WRITE(6,*) 'new FOR001.DAT written'
      CLOSE(UNIT=NIN)
      call sleep(1)
  700 STOP
C
 1999 CALL EXIT(1)  
C
 9000 FORMAT(I2)
 9010 FORMAT('SENTINEL')
      RETURN
      END

      REAL FUNCTION BV(DAC,A0,A1,A2,A3,J0,JMAX,DMAX)
C
C     COMPUTE FIELD FROM DAC-VALUE (NON-LINEAR EXITATION-CURVE)
C     PARAMETRISATION ACCORDING TO SIN-MAGNET-GROUP.
C
      REAL J0, JMAX
C
      SIG  = SIGN(1.0,DAC)
      SIG2 = SIGN(1.0,DMAX)
      AMPS = ABS(DAC) * JMAX/ABS(DMAX)
      BV = A0 * SIG + AMPS * A1/JMAX
      IF (AMPS.GT.J0 .AND. JMAX.GT.J0) THEN
       X = (AMPS-J0) / (JMAX-J0)
       BV = BV + (A2 + A3*X) * X*X
      ENDIF
      BV = SIG * SIG2 * BV
      RETURN
      END

CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

        SUBROUTINE DISPI
C
C       INPUT EDITIER PROGRAM FUER TURTLE UND TRANSPORT
C
        INCLUDE 'incom.f'
C
        INTEGER*2       INUM(1000)
        INTEGER         NO1(53)
        DIMENSION       WORD(31), NUMB(31)
        DIMENSION       VAL(10)
        CHARACTER*100   KARD
        CHARACTER*6     WORD, KEY
        CHARACTER*4     INPUT
        CHARACTER*2     NUMB
        CHARACTER*1     IMAG(80)
        LOGICAL         PLOT,SLITS
C
        DATA WORD/'TITLE ','BEAM  ','EDGE  ','DRIFT ','BEND  ','QUAD  ',
     X   'UPDAT ','SHIFT ','TOLER ','REPEAT','FIT   ','ACCEL ','CORR  ',
     X   'IO    ','MATRIX','UNITS ','SPEC  ','SEC   ','SEXT  ','SOLO  ',
     X   'ROT   ','WIEN  ','SPACE ','RFCAV ','LDREG ','COMBIN','QDAC  ',
     X   '      ','      ','      ','      '/
        DATA NUMB/'0 ','1 ','2 ','3 ','4 ','5 ','6 ','7 ','8 ','9 ','10'
     X      ,'11','12','13','14','15','16','17','18','19','20'
     X      ,'21','22','23','24','25','26','27','28','29','30'/
        DATA NO1/8,2,2,4,4,5,7,8,2,5,3,16,2,8,4,4,1,4,3,2,3,5,4,4,5
     X          ,0,4,0,8,12,19*0,3*4,1/
C
C       READ IN DATA FILE
C
        CALL READD(SLITS)
C
        IF (SLITS) THEN
         WORD(7)  = 'SLIT  '
        ENDIF
        IF (INDIC .NE. 0) THEN
         WORD(29)  = 'RAYS  '
         WORD(7)   = 'SLIT  '
         WORD(28)  = 'ExB   '
         WORD(31)  = 'dE/dx '
         WORD(30)  = 'KOLSPZ'
        ELSE
         WORD(28)  = 'BDAC  '
        END IF
C
C       FIND KEYWORD
C
  698   WRITE(6,3000)
  699   WRITE(6,2002)
        READ(5,1000) KEY
    CALL TOUPPER(KEY)
        IF (KEY(1:1).EQ.'H' .OR. KEY(1:1).EQ.'h') THEN
         CALL HELP1(WORD)
         GO TO 699
        END IF
        IF (KEY(1:1) .EQ. ' ') GO TO 800
        ISWITCH = 0
        IMAL    = 0
        IF (KEY .EQ. WORD(27) .OR. KEY(1:2) .EQ. NUMB(27)   .OR.
     X     (KEY .EQ. WORD(28) .AND. INDIC .EQ. 0)      .OR.
     X     (KEY .EQ. NUMB(28) .AND. INDIC .EQ. 0)      .OR.
     X      KEY .EQ. 'QAMP  ' .OR.  KEY .EQ. 'BAMP  ') THEN
         CALL QUADAC(KEY)
         GO TO 699
        ENDIF
        DO 700 N=2,30
         IF (KEY .EQ. WORD(N) .OR. KEY .EQ. NUMB(N)) GO TO 701
  700   CONTINUE
        IF (KEY(1:2) .EQ. '50') N=51
        IF (KEY(1:2) .EQ. '51') N=52
        IF (KEY(1:2) .EQ. '52') N=53
        IF (KEY(1:2) .EQ. '60') N=61
        IF (KEY(1:2) .EQ. '50' .OR. KEY(1:2) .EQ. '51' .OR.
     1      KEY(1:2) .EQ. '52' .OR. KEY(1:2) .EQ. '60') GO TO 701
        IF ((KEY.EQ.'RAYS  ') .AND. INDIC.NE.0) GO TO 710
        IF (KEY(1:2).EQ.'28' .AND. INDIC.NE.0) GO TO 710
        IF (KEY.EQ.WORD(1) .OR. KEY(1:2).EQ.NUMB(1)) GO TO 705
        WRITE(6,*) 'Illegal keyword, try again'
        GO TO 699
C
C       ENTER NEW TITLE
C
  705   WRITE(6,2004) IMAGE
        WRITE(6,2400)
        READ (5,2410) IMAG
        IF   (IMAG(1) .EQ. ' ' ) GO TO 699
        DO 706 J = 1, 80
  706   IMAGE(J) = IMAG(J)
        GO TO 699
C
C       ENTER NEW NUMBER OF RAYS (FOR TURTLE ONLY)
C
  710   WRITE(6,2500) INDIC
        IND = 0
        READ(5,2501,ERR=715) IND
        IF (IND .NE. 0) INDIC=IND
        GO TO 699
  715   WRITE(6,*) 'Wrong input format, try again'
        GO TO 710
C
C       LIST ELEMENTS OF DESIRED TYPE
C
  701   M   = -1
        MUM =  1
        DO 750 NUM = 1, NEL
         I     = ISTOR(NUM)
         ITYPE = DATA(I)
         ITYPE = IABS(ITYPE)
         IF (ITYPE .NE. N-1) GO TO 750
         LENGTH = NO1(ITYPE)
         IF (ITYPE.EQ.6 .AND. DATA(I+3).EQ.0.0) LENGTH=3
         IF (ITYPE.EQ.16 .AND. DATA(I+3).EQ.0.0) LENGTH=3
         IF (ITYPE .EQ. 1 .AND. DATA(I+8) .EQ. 0.0) LENGTH = 9
         LABLE = LABEL(NUM)
         M     = LENGTH - 1
         IF (MUM.EQ.1) WRITE(6,3000)
         CALL PRINT2(ITYPE,MUM,KEY,LABLE,DATA(I+1),M)
         INUM(MUM)=NUM
         MUM=MUM+1
  750   CONTINUE
        IF (ISWITCH .EQ. 1) GO TO 698
        IF (N.EQ.7 .AND. INDIC.NE.0) M=4
        IF (N.EQ.2) M=7
        IF (M .EQ. -1) THEN
        WRITE(6,*) 'No elements of this type found'
         GO TO 699
        END IF
C
C       ASK FOR NEW ENTRIES
C
        WRITE(6,3000)
  760   WRITE(6,2005)
        READ(5,1000) KARD
        IF (KARD(1:1).EQ.'H' .OR. KARD(1:1).EQ.'h') THEN
         CALL HELP2
         GO TO 760
        END IF
        IF (KARD(1:1) .EQ. ' ') THEN
         ISWITCH=1
         IF (IMAL .NE. 0) GO TO 701
         GO TO 699
        END IF
C
C       READ POINTER(S)
C
        CALL STR3INT(KARD(1:12),I3,I4,I2,ICONT)
        IF (I3.LT.1) GO TO 790
        IF (I4 .GE. MUM) GO TO 790
        IF (I4.LT.I3) GO TO 790
        IF (I2 .GT. M) GO TO 790
        IF (I2 .LT. 0) GO TO 790
        IF (I2 .EQ. 0) GO TO 770
        IF (N.EQ.16 .AND. I2.EQ.2) GO TO 767
C
C       READ NEW ENTRIES
C
        CALL STRNFLO(KARD(ICONT:ICONT+79),VAL,NJ)
        DO 766 IJ = I3, I4, 1
         I1=INUM(IJ)
         DO 1766 KJ = 1, NJ
          J = ISTOR(I1) + I2 + KJ - 1
          IF (I2+KJ-1.GT.M) GO TO 1766
          DATA(J) = VAL(KJ) + .000002 * SIGN(1.0,VAL(KJ))
 1766    CONTINUE
  766   CONTINUE
        IMAL = IMAL + 1
        GO TO 760
C
C       NEW DIMENSION FOR TYPE CODE 15
C
  767   DO 768 IJ = I3, I4
         I1 = INUM(IJ)
         J  = ISTOR(I1) + 2
         JJ = DATA(J)
         DO 768 JX = 1, 4
          JXX = ICONT + JX - 1
          IF (KARD(JXX:JXX) .NE. ' ')
     *        DSTOR(JJ)(JX:JX) = KARD(JXX:JXX)
  768   CONTINUE
        IMAL = IMAL + 1
        GO TO 760
C
C       NEW LABEL(S)
C
  770   READ(KARD(ICONT:ICONT+3),1000,ERR=780) INPUT
        DO 772 IJ = I3, I4, 1
         I1 = INUM(IJ)
         LABEL(I1)=INPUT
  772   CONTINUE
        IMAL = IMAL + 1
        GO TO 760
C
C       ERROR HANDLING
C
  780   WRITE(6,*) 'Wrong input format, try again'
        GO TO 760
  790   WRITE(6,*) 'Incorrect range, try again'
        GO TO 760
C
C       WRITE NEW FILE
C
  800   CALL WRITED(1)
        GO TO 699
C
 1000   FORMAT(A)
 2002   FORMAT(' Keyword or HELP<CR>  ',$)
 2004   FORMAT(/' Title is now :'/1X,80A1)
 2005   FORMAT(' Input or HELP<CR>  ',$)
 2009   FORMAT(A6)
 2400   FORMAT(' Enter new title (D = as is) :')
 2410   FORMAT(80A1)
 2500   FORMAT(/' # of rays =',I7,'. New entry (D = as is) : ',$)
 2501   FORMAT(I7)
 3000   FORMAT(1H )
C
        RETURN
        END

        SUBROUTINE HELP1(WORD)
C
        CHARACTER*6 WORD(31)
C
        WRITE(6,*)
     X  'Keywords for Transport/Turtle input-files are:'
        WRITE(6,1000) (WORD(I),I=1,9)
        WRITE(6,1000) (WORD(I),I=10,18)
        WRITE(6,1000) (WORD(I),I=19,28)
        WRITE(6,1000) (WORD(I),I=29,31)
1000    FORMAT(1X,10(A6,' '))
        WRITE(6,*)
     X  'The numbers 0 to 30 (Type-codes) may be used instead.'
        WRITE(6,*)
     X  'Use QAMP/BAMP for getting Ampere-settings of quads/bends.'
        WRITE(6,*)
     X  'For Turtle the numbers 50, 51, 52 and 60 may also be used.'
        WRITE(6,*)
     X  'Press <CR> for new FOR001.DAT (with confirmation) for Exit.'
        RETURN
C
        ENTRY HELP2
C
        WRITE(6,*)
     X  'Input codes at this level are :'
        WRITE(6,*)
     X  'I1,I2;ENTRY       ENTRY replaces I2th value of line I1.'
        WRITE(6,*)
     X  'I1;ENTRY          ENTRY replaces Label of line I1.'
        WRITE(6,*)
     X  'I1,I2;ENTRIES     ENTRIES replace values of line I1'//
     X  ' starting at position I2.'
        WRITE(6,*)
     X  'I1:I2,I3;ENTRY    ENTRY replaces I3th value of lines I1'//
     X  ' through I2.'
        WRITE(6,*)
     X  'I1:I2;ENTRY       ENTRY replaces Label of lines I1'//
     X  ' through I2.'
        WRITE(6,*)
     X  'I1:I2,I3;ENTRIES  ENTRIES replace values of lines I1'//
     X  ' through I2'
        WRITE(6,*)
     X  '                  starting at position I3.'
        WRITE(6,*)
     X  '<CR>              Ends input at this level. (Back to'//
     X  ' keyword selection).'
        WRITE(6,*)
     X  '   I1, I2 and I3 (1-3 digit integers) >= 1 together with'//
     X  ' (: , ;)'
        WRITE(6,*)
     X  '   define the range. ENTRIES are 2 to 10 values separated '//
     X  'by 1 blank.'
        RETURN
C        
C
        ENTRY HELP3
        WRITE(6,*)
     X  'Special input code for Keyword QDAC/BDAC (26/27) :'
        WRITE(6,*)
     X  'I;IVAL   IVAL replaces field of Ith Quad/Bend in 2 ways:'
        WRITE(6,*)
     X  '         IVAL = DAC-value if Quad/Bend found in'//
     X  ' QUADS.CAL/BENDS.CAL'
        WRITE(6,*)
     X  '         IVAL = Value in Gauss otherwise.'
        WRITE(6,*)
     X  'or Keyword QAMP/BAMP :'
        WRITE(6,*)
     X  'I;VAL    VAL replaces field of Ith Quad/Bend in 2 ways:'
        WRITE(6,*)
     X  '         VAL = Amp-setting if Quad/Bend found in'//
     X  ' QUADS.CAL/BENDS.CAL'
        WRITE(6,*)
     X  '         VAL = Value in kG otherwise.'
        WRITE(6,*)
     X  '<CR>     Ends input for QDAC/QAMP/BDAC/BAMP (Back to keyword'//
     X  ' selection).'
C
        RETURN
        END

        SUBROUTINE QUADAC(KEY)
C
C       DAC I/O ROUTINE FOR QUADS
C
        INCLUDE 'incom.f'
C
        CHARACTER*30 INPUT
        CHARACTER*6  KEY
        INTEGER*2    INUM(1000)
        INTEGER      INDEX(100)
        REAL         VAL(10),J0(100),JMAX(100),DMAX(100)
        DIMENSION    LEFF(100),JA(100),A0(100),A1(100),A2(100),A3(100)
        CHARACTER*4  QTYP(100),QLAB(100)
        LOGICAL      FIRST, ANIN
        SAVE         FIRST, ANIN
        DATA         FIRST, ANIN /.TRUE.,.FALSE./
        DATA         G2 /100./
C
C       TYPE CODE CONVERSION TABLE
C
        IF (KEY(1:2) .EQ. '26') INRT = 5
        IF (KEY .EQ. 'QDAC  ') INRT = 5
        IF (KEY .EQ. 'QAMP  ') INRT = 5
        IF (KEY(1:2) .EQ. '27') INRT = 4
        IF (KEY .EQ. 'BDAC  ') INRT = 4
        IF (KEY .EQ. 'BAMP  ') INRT = 4
C
C       READ IN QUAD CALIBRATION FACTORS
C
        IF (FIRST) THEN
         FIRST = .FALSE.
         OPEN(UNIT=50,FILE='QUADS.CAL',STATUS='OLD')
         K = 1
    1    CONTINUE
         READ(50,2000,END=2,ERR=2)
     *    QTYP(K),QLAB(K),LEFF(K),JA(K),A0(K),A1(K),A2(K),A3(K),
     *    J0(K),JMAX(K),DMAX(K)
          IF (K.GT.100) STOP 'FILE QUADS.CAL OVERFLOW'
          K = K + 1
         GO TO 1
    2    CLOSE(UNIT=50)
         OPEN(UNIT=50,FILE='BENDS.CAL',STATUS='OLD')
    3    CONTINUE
         READ(50,2000,END=4,ERR=3)
     *    QTYP(K),QLAB(K),LEFF(K),JA(K),A0(K),A1(K),A2(K),A3(K),
     *    J0(K),JMAX(K),DMAX(K)
          IF (K.GT.100) STOP 'FILE BENDS.CAL OVERFLOW'
          K = K + 1
         GO TO 3
    4    NREC = K - 1
         A0(K) = 0.
         A1(K) = 1.
         A2(K) = 0.
         A3(K) = 0.
         J0(K) = 1000.
         JMAX(K) = 1.
         DMAX(K) = 1000.
         CLOSE(UNIT=50)
        ENDIF
C
C       FIND AND LIST QUADS AND CONVERT INTO DAC VALUES
C
        ISWITCH =  0
        IMAL    =  0
  701   M       = -1
        MUM     =  1
        WRITE(6,3000)
        DO 750 NUM = 1, NEL
         I     = ISTOR(NUM)
         ITYPE = DATA(I)
         ITYPE = IABS(ITYPE)
         K     = DATA(I+1)
         IF (ITYPE.EQ.13. .AND. K.EQ.47) ANIN = .FALSE.
         IF (ITYPE.EQ.13. .AND. K.EQ.48) ANIN = .TRUE.
         IF (ITYPE.EQ.16. .AND. K.EQ.5)  G2   = DATA(I+2)
         IF (ITYPE.EQ.4   .AND. ANIN) GO TO 750
         IF (ITYPE .NE. INRT) GO TO 750
         IF (ITYPE .EQ. 5) THEN
          IA = 10.0 * DATA(I+3) + .001
         ELSE
          IA = 10.0 * G2        + .001
         ENDIF
         LL = 1000.0 * DATA(I+1) + .001
         LABLE    = LABEL(NUM)
C
         DO 50 K  = 1, NREC
          IF ( (LABLE .EQ. QLAB(K)) .OR.
     *       (IA.EQ.JA(K) .AND. LL.EQ.LEFF(K) .AND. QLAB(K).EQ.'%%%%')
     *       ) GO TO 51
   50    CONTINUE
         K = NREC + 1
   51    B = DATA(I+2)
         INDEX(MUM) = K
         CALL GETDAC
     *   (B,IDAC,AMPS,A0(K),A1(K),A2(K),A3(K),J0(K),JMAX(K),DMAX(K))
         M    = 3
         IF (KEY(2:4).EQ.'AMP') THEN
          WRITE(6,1001) MUM,KEY, LABLE, AMPS
         ELSE
          WRITE(6,1000) MUM,KEY, LABLE, IDAC
         ENDIF
         INUM(MUM) = NUM
         MUM       = MUM + 1
  750   CONTINUE
        IF (ISWITCH.EQ.1) THEN
         WRITE(6,3000)
         RETURN
        ENDIF
        IF (M .EQ. -1) THEN
     WRITE(6,*) 'No element of desired type found'
         RETURN
        END IF
C
C       ASK FOR NEW ENTRY
C
        WRITE(6,3000)
  760   WRITE(6,2005)
        INPUT(1:1) = CHAR(0)
        READ(5,2001,ERR=781) INPUT
        IF (INPUT(1:1).EQ.'H' .OR. INPUT(1:1).EQ.'h') THEN
         CALL HELP3
         GO TO 760
        ENDIF
        IF (INPUT(1:1) .EQ. ' ') THEN
         ISWITCH = 1
         IF (IMAL.NE.0) GO TO 701
         RETURN
        ENDIF
C
C       READ POINTER
C
        CALL STR3INT(INPUT(1:12),I3,I1,I2,ICONT)
        IF (I1.NE.I3 .OR. I2.NE.0) GO TO 781
        IF (I3 .LT. 1) GO TO 790
        IF (I3 .GE. MUM) GO TO 790
        I1 = INUM(I3)
        I  = INDEX(I3)
C
C       READ NEW AMP VALUE OR NEW DAC VALUE
C
        IF (KEY(2:4).EQ.'AMP') THEN
         CALL STRNFLO(INPUT(ICONT:30),VAL,NJ)
         IF (NJ .NE. 1) GO TO 781
         DAC = DMAX(I) * VAL(1) / JMAX(I)
        ELSE
         CALL STRINT(INPUT(ICONT:ICONT+9),IVAL)
         DAC = IVAL
        ENDIF
        IMAL    = IMAL + 1
        J       = ISTOR(I1) + 2
        DATA(J) =
     *  BV(DAC,A0(I),A1(I),A2(I),A3(I),J0(I),JMAX(I),DMAX(I))
        GO TO 760
C
C       ERROR HANDLING
C
  781   WRITE(6,*) 'Wrong input format, try again'
        GO TO 760
  790   WRITE(6,*) 'Incorrect range, try again'
        GO TO 760
C
 1000   FORMAT(' #',I3,2X,A6,A4,I7)
 1001   FORMAT(' #',I3,2X,A6,A4,1X,F9.3)
 2000   FORMAT(1X,A4,2X,A4,2X,I4,2X,I4,7F7.1)
 2001   FORMAT(A)
 2005   FORMAT(' Input or HELP<CR>  ',$)
 3000   FORMAT(1H )
        END

        SUBROUTINE PRINT2(ITYPE,NNUM,KEY,LLABLE,XDATA,NNEL)
C
        INCLUDE 'incom.f'
C
        CHARACTER*1  DUMY
        CHARACTER*4  LLABLE, IDATA
        CHARACTER*6  KEY
        CHARACTER*5  IFORM
        CHARACTER*12 CHAR, CAR(15)
        CHARACTER*79 OUT
        DIMENSION    XDATA(15), N(15)
        DATA IFORM   /'(A79)'/
C
C       LINENUMBER , KEY , LABEL
C
        CALL CITA(NNUM,CHAR,J)
        OUT(1:1)   = '#'
        OUT(2:5)   = CHAR(1:4)
        OUT(6:11)  = KEY
        OUT(12:12) = ' '
        OUT(13:16) = LLABLE
        OUT(17:17) = ' '
        N1=18
C
C       ELEMENT DATA
C
        IF (ITYPE .EQ. 15) GO TO 20
        IF (ITYPE .EQ. 17) GO TO 40
        DO 10 J = 1, NNEL
         CALL CFTA(XDATA(J),CAR(J),N(J))
   10   CONTINUE
        GO TO 30
C
   20   CALL CFTA(XDATA(1),CAR(1),N(1))
        CALL CFTA(XDATA(3),CAR(3),N(3))
        JJJJ = XDATA(2)
        CAR(2)(1:6) = DSTOR(JJJJ)//'  '
        N(2) = 5
C
   30   DO 35 J = 1, NNEL
         N2 = N1 + N(J)
         OUT(N1:N2) = CAR(J)(1:N(J)+1)
         N1 = N2 + 1
   35   CONTINUE
   40   OUT(N1:N1) = ' '
C
C       PRINT ELEMENT
C
        WRITE(IFORM(3:4),1000) N1
        WRITE(6,IFORM) OUT(1:N1)
 1000   FORMAT(I2)
        RETURN
        END

        SUBROUTINE STRINT(STRING,IVAL)
C
C       THIS SUBROUTINE CONVERTS A STRING INTO AN INTEGER (1-10 DIGITS)
C
        CHARACTER*10 STRING
        CHARACTER*5  FORM
        DATA         FORM /'(I10)'/
C
        DO 10 J=10,1,-1
         IF (STRING(J:J).NE.' ') GO TO 20
   10   CONTINUE
        IVAL=0
        GO TO 25
   20   WRITE(FORM(3:4),1000) J
        READ(STRING(1:J),FORM) IVAL
   25   RETURN
   30   IVAL=0
        RETURN
 1000   FORMAT(I2)
        END

        SUBROUTINE STR3INT(STRING,I1,I2,I3,ICONT)
C
C       THIS ROUTINE EXTRACTS 3 INTERGERS (1-3 DIGITS)
C       OUT OF ONE STRING (I1:I2,I3;)
C
        CHARACTER*12 STRING
        CHARACTER*3  R1,R2,R3,RR(3)
        CHARACTER*1  R
        EQUIVALENCE  (RR(1),R1),(RR(2),R2),(RR(3),R3)
        N1 = 0
        N2 = 0
        R1 = '   '
        R2 = '   '
        R3 = '   '
C
C       SEPARATION
C
        DO 10 I = 1, 12
         R = STRING(I:I)
         IF (R .EQ. ':') N1 = I
         IF (R .EQ. ',') N2 = I
         IF (R .EQ. ';') GO TO 15
   10   CONTINUE
        I1 = 0
        RETURN
C
   15   N3    = I
        ICONT = I + 1
C
        IF (N1 .EQ. 0) GO TO 20
        IF (N2 .NE. 0) GO TO 17
C
C       ONLY ':' FOUND --> (I1:I2,0;)
C
        R1(5-N1:3)    = STRING(1:N1-1)
        R2(5+N1-N3:3) = STRING(N1+1:N3-1)
        GO TO 30
C
C       ':' AND ',' FOUND --> (I1:I2,I3;)
C
   17   CONTINUE
        R1(5-N1:3)=STRING(1:N1-1)
        R2(5+N1-N2:3)=STRING(N1+1:N2-1)
        R3(5+N2-N3:3)=STRING(N2+1:N3-1)
        GO TO 30
C
   20   IF (N2.EQ.0) GO TO 25
C
C       ONLY ',' FOUND --> (I1:I1,I3;)
C
        R1(5-N2:3)=STRING(1:N2-1)
        R3(5+N2-N3:3)=STRING(N2+1:N3-1)
        R2=R1
        GO TO 30
C
C       WETHER ':' NOR ',' FOUND --> (I1:I1,0;)
C
   25   R1(5-N3:3)=STRING(1:N3-1)
        R2=R1
C
C       CONVERSION
C
   30   CONTINUE
        READ(R1,1000) I1
        READ(R2,1000) I2
        READ(R3,1000) I3
 1000   FORMAT(I3)
        RETURN
        END

        SUBROUTINE STRNFLO(STRING,VALX,N)
C
C       THIS ROUTINE CONVERTS A STRING INTO N FLOATING NUMBERS
C       SEPARATOR IS ONE BLANK. SIZE OF EACH NUMBER <= 10.
C
        CHARACTER*80 STRING
        CHARACTER*10 VAL(10)
        DIMENSION    IND(20),VALX(10)
C
        IND(1) = 0
        K      = 1
        DO 5 I =1, 10
         VAL(I)='0000000000'
    5   CONTINUE
        DO 7 I=1,10
         IF (STRING(I:I).NE.' ') GO TO 8
    7   CONTINUE
    8   M=I
        DO 10 I=M,80
         IF (STRING(I:I).NE.' ') GO TO 10
         K=K+1
         IND(K)=I
         IF (I-IND(K-1).EQ.1) GO TO 15
   10   CONTINUE
   15   N = K - 2
        IF (N.GT.10) N=10
        IF (N.EQ.0) N=1
        DO 30 I=1,N
         IND1 = 12-IND(I+1)+IND(I)
         IND2 = IND(I)+1
         IND3 = IND(I+1)-1
         VAL(I)(IND1:10) = STRING(IND2:IND3)
   30   CONTINUE
 1000   FORMAT(F10.0)
        DO 40 I=1,N
         DO 35 L=1,10
          IF (VAL(I)(L:L).EQ.'-') GO TO 37
   35    CONTINUE
         GO TO 38
   37    VAL(I)(L:L)='0'
         VAL(I)(1:1)='-'
   38    CONTINUE
         READ(VAL(I),1000) VALX(I)
   40   CONTINUE
        RETURN
        END

      SUBROUTINE GETDAC(BX,IDAC,AMPS,A0,A1,A2,A3,J0,JMAX,DMAX)
C
C     GET AMPS AND DAC-VALUES FROM NON-LINEAR EXITATION CURVE
C     PARAMETRISATION ACCORDING TO SIN-MAGNET-GROUP.
C
      REAL J0, JMAX
C
      BV   = ABS(BX)
      SIG  = SIGN(1.0,BX)
      SIG2 = SIGN(1.0,DMAX)
      AMPS = (BV-A0*SIG) * JMAX/A1              ! LINEAR TERM
      IF (AMPS.GT.J0 .AND. JMAX.GT.J0) THEN     ! NON-LINEAR CONTRIBUTIONS ?
       DO 20 I = 1, 100
        X  = (AMPS-J0) / (JMAX-J0)
        X2 = X  * X
        X3 = X2 * X
        AJ = A1 / JMAX
        B  = A0*SIG + AJ*AMPS + A2*X2 + A3*X3   ! NON-LINEAR CURVE
        DB = BV - B
        IF  (DB .LT. 0.0001) GO TO 30           ! CONVERGENCE CRITERIA
        BJ = AJ + ((2*A2+3*A3*X)*X) / (JMAX-J0) ! DERIVATIVE
        DJ = DB /BJ
        AMPS  = AMPS + DJ
   20  CONTINUE
   30  CONTINUE
      ENDIF
      AMPS = SIG * SIG2 * AMPS
      IDAC = NINT(AMPS*ABS(DMAX)/JMAX)
      RETURN
      END

      SUBROUTINE TOUPPER(KEY)
C
      CHARACTER*6 KEY
C
      DO 10 J = 1, 6
       IC = ICHAR(KEY(J:J))
       IF (IC.GE.97 .AND. IC.LE.122) THEN
        IC = IC - 32
        KEY(J:J) = CHAR(IC)
       ENDIF
   10 CONTINUE
C
      RETURN
      END
