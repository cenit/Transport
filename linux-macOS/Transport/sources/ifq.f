    PROGRAM IFQ
C
C     RECOMPILED BY URS ROHRER (PSI) NOVEMBER 1993
C     FOR MS-POWERSTATION FORTRAN
C     
    CHARACTER*18 IFORM1
    CHARACTER*5  IFORM2
    CHARACTER*9  IFORM3
    CHARACTER*8  IFORM4
    CHARACTER*12 STRINGI
    CHARACTER*8 TEST
    CHARACTER*4 STRING,LAB
    CHARACTER*15 RES
    CHARACTER*80 LINE,BUF
    CHARACTER*80 ENTL,ENTLN
    DIMENSION LAB(20,4),II(5)
    DIMENSION LINE(1000),IANF(20),IEND(20)
    DIMENSION ENTLN(20)
    EQUIVALENCE(II(1),I1),(II(2),I2),(II(3),I3),(II(4),I4)
    EQUIVALENCE(II(5),I5)
    LOGICAL TC,M9
    DATA IFORM1 /'(1X,1H#,I2,2X,A80)'/
    DATA IFORM2 /'(A83)'/
    DATA IFORM3 /'(A83,1H/)'/
    DATA IFORM4 /'(1X,A80)'/
C
C   FINDE ERSTE SENTINEL KARTE
C
    I=1
    OPEN (UNIT=1,FILE='FOR001.DAT',STATUS='OLD',ERR=777)
    1   READ(1,1001,END=5000,ERR=777) LINE(I)
 1001   FORMAT(A80)
    DO 2 J=1,10
    TEST=LINE(I)(J:J+7)
    IF(TEST.EQ.'SENTINEL') GO TO 3
    2   CONTINUE
    I=I+1
    GO TO 1
    3   IMAX=I
    IANF(1)=1
    IEND(1)=IMAX
C
C   FINDE ANDERE SENTINEL KARTEN,ORDNE IN GRUPPEN
C
    K=2
    4   I=I+1
    IANF(K)=I
    5   READ(1,1001,END=8) BUF
    DO 6 J=1,10
    TEST=BUF(J:J+7)
    IF(TEST.EQ.'SENTINEL') GO TO 7
    6   CONTINUE
    LINE(I)=BUF
    I=I+1
    GO TO 5
    7   IEND(K)=I
    LINE(I)=BUF
    K=K+1
    GO TO 4
    8   KMAX=K-1
    IF(KMAX.EQ.1) GO TO 779
C
C   FILE ENDE ERREICHT
C   DELETE ALTES *PLOT* 
C
    DO 455 I=1,KMAX
    K1=IANF(I)
    DO 455 J=1,10
    IF(LINE(K1)(J:J+5).EQ.'*PLOT*') GO TO 456
  455   CONTINUE
  456   NP=J
    DO 457 J=NP,74 
    LINE(K1)(J:J)=LINE(K1)(J+6:J+6)
  457   CONTINUE
    LINE(K1)(75:80)='      '
  458   IF(KMAX.GT.2) WRITE(6,4000) KMAX-1
 4000   FORMAT(' Input_file contains',I3,' sequences.')
    IF(KMAX.EQ.2) WRITE(6,*)
     X               'Input_file contains only basic sequence,'
    CALL CHECK(LINE,IANF,KMAX)
    GO TO 99
C
C   SCHREIBE FIT SEQUENCEN AUF SCHIRM
C
  100   IND1=2
    IND2=KMAX-1
  150   DO 50 J=IND1,IND2
    KJ=0
    WRITE(6,1002) J
 1002   FORMAT(' *** Sequence #',I3,' ***')
    DO 40 K=IANF(J),IEND(J)
    KJ=KJ+1
    IF(KJ.EQ.100) IFORM1(10:10)='3'
    DO 30 L=80,2,-1
    IF(LINE(K)(L:L).NE.' ') GO TO 31
   30   CONTINUE
   31   WRITE(IFORM1(16:17),1004) L
    WRITE(6,IFORM1) KJ,LINE(K)
 1004   FORMAT(I2)
   40   CONTINUE
   50   CONTINUE
    IFORM1(10:10)='2'
    GO TO 99
C
C   SCHREIBE AUSSCHNITT VON SEQUENCE NR AUF SCHIRM
C
  600   WRITE(6,3002) NR
 3002   FORMAT(' *** Sequence #',I3,' ***')
    DO 610 K=LMIN,LMAX
    IP=K-IND1+1
    IF(IP.GE.100) IFORM1(10:10)='3'
    DO 630 L=80,2,-1
    IF(LINE(K)(L:L).NE.' ') GO TO 620
  630   CONTINUE
  620   WRITE(IFORM1(16:17),1004) L
    WRITE(6,IFORM1) IP,LINE(K)
  610   CONTINUE
    IFORM1(10:10)='2'
C
C   FIND INPUT CODE
C
   99   IF(KMAX.EQ.2) WRITE(6,2200)
 2200   FORMAT(' No fit_sequence found')
  199   WRITE(6,2000)
 2000   FORMAT(' >Enter code or press HELP : ',$)
    READ(5,2020) STRINGI
 2020   FORMAT(A12)
    IF (STRINGI(1:4).NE.'HELP' .AND. STRINGI(1:1).NE.'H' .AND.
     X      STRINGI(1:4).NE.'help' .AND. STRINGI(1:1).NE.'h') GO TO 125
C
C   HELP SECTION
C
    WRITE(6,*)
     X' Input codes are:'
    WRITE(6,*)
     X' Entering:  1         Insert first fit_sequence if none',
     X                       ' found yet'
    WRITE(6,*)
     X'            NR        Insert new sequence after sequence',
     X                       ' number NR' 
    WRITE(6,*)
     X'            NR,MR     Insert lines in sequence NR after line MR'
    WRITE(6,*)
     X'           -NR        Delete sequence number NR'
    WRITE(6,*)
     X'            NR,-MR    Delete line MR in sequence NR'
    WRITE(6,*)
     X'            NR,-MR:LR Delete lines MR through LR in sequence NR'
    WRITE(6,*)
     X'            DK        Deactivate Sequence K (Indicator = -9)'
    WRITE(6,*)
     X'            AK        Activate Sequence K (Indicator = -1)'
    WRITE(6,*)
     X'            P         Print all fit_sequences on screen'
    WRITE(6,*)
     X'            PK        Print sequence K on screen'
    WRITE(6,*)
     X'            PK,L      Print window of sequence K,',
     X                       ' line L centred'
    WRITE(6,*)
     X'            S         Print number of sequences on screen'
    WRITE(6,*)
     X'            <CR>      Exit, write new FOR001.DAT'
    GO TO 199
  125   IF (STRINGI(1:1).NE.'P' .AND. STRINGI(1:1).NE.'p') GO TO 136
C
C   PRINT SECTION
C
    STRINGI(1:1)=' '
    CALL STR3INT(STRINGI,NR,MR,LR,IER)
    IF(IER.EQ.1) GO TO 299
    IF(NR.EQ.0) GO TO 100
    IF(NR.LT.0) GO TO 101
    IF(NR.GE.KMAX) GO TO 101
    IF(MR.GT.0) GO TO 135
    IND1=NR
    IND2=NR
    GO TO 150
  135   MROLD=MR
    GO TO 441
C
C   ACTIVATE OR DEACTIVATE ONE SEQUENCE
C
  136   IF (STRINGI(1:1).NE.'D' .AND. STRINGI(1:1).NE.'A' .AND.
     X      STRINGI(1:1).NE.'d' .AND. STRINGI(1:1).NE.'a') GO TO 140
    M9=.FALSE.
    IF (STRINGI(1:1).EQ.'A' .OR. STRINGI(1:1).EQ.'a') M9=.TRUE.
    STRINGI(1:1)=' '
    CALL STR3INT(STRINGI,NR,MR,LR,IER)
    IF(IER.EQ.1 .OR. MR.NE.0 .OR. LR.NE.0) GO TO 299
    IF(NR.LE.1 .OR. NR.GE.KMAX) GO TO 101
    GO TO 445
C
C   <CR> AND S SECTION
C
  140   IF (STRINGI.EQ.' ') GO TO 450
    IF (STRINGI.EQ.'S' .OR. STRINGI.EQ.'s') GO TO 458
C
C   DELETING AND INSERTING SECTION
C
    CALL STR3INT(STRINGI,NR,MR,LR,IER)
    IF(IER.EQ.1) GO TO 299
    NLIN=IABS(LR)-IABS(MR)+1
    IF(LR.EQ.0)NLIN=1
    IF(NR.GE.KMAX) GO TO 101
    IF(NR.EQ.-1) GO TO 101
    IF(NR.EQ.0) GO TO 101
    IF(NR.LE.-KMAX) GO TO 101
    IF(NR.LT.-1) GO TO 200
    DO 141 I=1,12
    IF(STRINGI(I:I).EQ.',') GO TO 142
  141   CONTINUE
    GO TO 102
  142   IF(MR.GE.0) GO TO 420
    IF(MR.LT.0) GO TO 430
  101   WRITE(6,2001,ERR=199) NR
 2001   FORMAT(' Sequence #',I3,' not allowed')
    GO TO 199
C
C   ENTER DATA FOR ONE NEW SEQUENCE
C
  102   I1=0
  103   WRITE(6,2002)
 2002   FORMAT('  Enter Label of Fit to be activated:    ',$)
    READ(5,2003) STRING
 2003   FORMAT(A4)
    IF(STRING.EQ.' ') GO TO 104
    I1=I1+1
    LAB(I1,1)=STRING
    GO TO 103
C
  104   I2=0
  105   WRITE(6,2004)
 2004   FORMAT('  Enter Label of Fit to be deactivated:  ',$)
    READ(5,2003) STRING
    IF(STRING.EQ.' ') GO TO 106
    I2=I2+1
    LAB(I2,2)=STRING
    GO TO 105
C
  106   I3=0
  107   WRITE(6,2005)
 2005   FORMAT('  Enter Label of Quad to be activated:   ',$)
    READ(5,2003)STRING
    IF(STRING.EQ.' ') GO TO 108
    I3=I3+1
    LAB(I3,3)=STRING
    GO TO 107
C
  108   I4=0
  109   WRITE(6,2006)
 2006   FORMAT('  Enter Label of Quad to be deactivated: ',$)
    READ(5,2003) STRING
    IF(STRING.EQ.' ') GO TO 110
    I4=I4+1
    LAB(I4,4)=STRING
    GO TO 109
C
  110   I5=0
  111   WRITE(6,2007)
 2007   FORMAT('  Enter entire arbitrary line:           ',$)
    READ(5,1001) ENTL
    IF(ENTL.EQ.' ') GO TO 400
    I5=I5+1
    ENTLN(I5)=ENTL
    GO TO 111
C
C   SEQUENCE NR WIRD GESTRICHEN
C
  200   K1=IANF(IABS(NR))
    K2=IANF(IABS(NR)+1)
    K3=IEND(KMAX)
    DO 201 I=K2,K3
  201   LINE(I+K1-K2)=LINE(I)
    KMAX=KMAX-1
    NR=IABS(NR)
    DO 202 I=NR,KMAX
    IANF(I)=IANF(I+1)-K2+K1
    IEND(I)=IEND(I+1)-K2+K1
  202   CONTINUE
    WRITE(6,3000,ERR=199) NR
 3000   FORMAT(' Sequence #',I3,' deleted')
    GO TO 199
C
C   EINSCHIEBEN EINER SEQUENCE
C
  400   LENG=I1+I2+I3+I4+I5
    K1=IANF(IABS(NR))
    K2=IANF(IABS(NR)+1)
    K3=IEND(KMAX)
    IR=LENG+2
    DO 401 I=1,K3-K2+1
    J=K3-I+1
  401   LINE(J+IR)=LINE(J)
    K=IEND(NR)+1
    LINE(K)(1:3)=' -1'
    LINE(K)(4:80)=' '
    K=K+1
    DO 410 I=1,5
    IF(II(I).EQ.0) GO TO 410
    DO 409 J=1,II(I)
    IF(I.EQ.5) GO TO 411
    CALL FILLIN(I,LAB(J,I),RES)
    LINE(K)(1:15)=RES
    LINE(K)(16:80)=' '
    GO TO 412
  411   LINE(K)=ENTLN(J)
  412   CONTINUE
    K=K+1
  409   CONTINUE
  410   CONTINUE
    LINE(K)(1:9)=' SENTINEL'
    LINE(K)(10:80)=' '
    KMAX=KMAX+1
    DO 402 I=1,KMAX-NR-1
    J=KMAX-I+1
    IANF(J)=IANF(J-1)+LENG+2
    IEND(J)=IEND(J-1)+LENG+2
  402   CONTINUE
    IANF(NR+1)=IEND(NR)+1
    IEND(NR+1)=IANF(NR+1)+LENG+1
    IND1=NR+1
    IND2=NR+1
    GO TO 150
C
C   EINSCHIEBEN EINER ZEILE IN SEQUENCE # NR
C
  420   NR=IABS(NR)
    IF(MR.GT.IEND(NR)-IANF(NR)) GO TO 421
    MROLD=MR
    GO TO 423
  421   WRITE(6,2008,ERR=199) MR
 2008   FORMAT(' Line #',I3,' not allowed')
    GO TO 199
  423   WRITE(6,2007)
    READ(5,1001) ENTL
    IF(ENTL.EQ.' ') GO TO 441
    DO 424 I=IEND(KMAX),IANF(NR)+MR,-1
  424   LINE(I+1)=LINE(I)
    LINE(IANF(NR)+MR)=ENTL
    IEND(NR)=IEND(NR)+1
    DO 426 I=NR+1,KMAX
    IEND(I)=IEND(I)+1
  426   IANF(I)=IANF(I)+1
    MR=MR+1
    GO TO 423
C
C   STREICHEN VON NLIN ZEILEN IN SEQUENCE # NR
C
  430   NR=IABS(NR)
    MR=IABS(MR)-1
    MROLD=MR
    DO 440 L=1,NLIN
    IF(MR.GT.IEND(NR)-IANF(NR)-1) GO TO 421
    IF(MR.LT.0) GO TO 421
    DO 434 I=IANF(NR)+MR,IEND(KMAX)-1
  434   LINE(I)=LINE(I+1)
    IEND(NR)=IEND(NR)-1
    DO 436 I=NR+1,KMAX
    IEND(I)=IEND(I)-1
  436   IANF(I)=IANF(I)-1
  440   CONTINUE
C
C   CONSTRUCTING DISPLAY WINDOW
C
  441   IND1=IANF(NR)
    IND2=IEND(NR)
    IF(IND2-IND1.LE.10) THEN
    LMIN=IND1
    LMAX=IND2
    GO TO 600
    END IF
    MRM=(MR+MROLD)/2
    IF(MR.NE.MROLD) MRM=MRM+1
    MRM=MRM+IND1-1
    LMIN=MAX(IND1,MRM-5)
    LMAX=LMIN+10
    IF(LMAX.LE.IND2) GO TO 600
    LMAX=IND2
    LMIN=MAX(IND1,LMAX-10)
    GO TO 600
C
C   SET INDICATOR EQUAL -1 OR -9
C
  445   K1=IANF(NR)
    DO 446 I=1,10
    IF(LINE(K1)(I:I).EQ.'/') K1=IANF(NR)+1
  446   CONTINUE
    IF(.NOT.M9) LINE(K1)=' -9'
    IF(M9) LINE(K1)=' -1'
    IF(.NOT.M9) WRITE(6,2010,ERR=199) NR
 2010   FORMAT(' Sequence #',I3,' deactivated')
    IF(M9) WRITE(6,2011,ERR=199) NR
 2011   FORMAT(' Sequence #',I3,' activated')
    GO TO 199
C
C   HAT LETZTE SEQUENZ MIT INDICATOR <> -9 EINE TITELKARTE ?
C
  450   DO 463 J=KMAX-1,1,-1
    K1=IANF(J)
    TC=.FALSE.
    DO 460 I=1,10
    IF(LINE(K1)(I:I).EQ.'/') TC=.TRUE.
  460   CONTINUE
    IF(TC) K1=K1+1
    M9=.FALSE.
    DO 461 I=1,10
    IF(LINE(K1)(I:I+1).EQ.'-9') M9=.TRUE.
  461   CONTINUE
    IF(.NOT.M9) GO TO 464
  463   CONTINUE
  464   IF(TC) GO TO 480
C
C   SETZE /*PLOT*/ TITELKARTE EIN
C
    K2=IEND(KMAX)
    DO 465 K=K2,K1,-1
  465   LINE(K+1)=LINE(K)
    LINE(K1)=' /*PLOT*/'
    IEND(KMAX)=IEND(KMAX)+1
    GO TO 489
C
C   SETZE *PLOT* IN TITELKARTE EIN
C
  480   DO 481 I=1,10
    NP=I+1
    IF(LINE(K1-1)(I:I).EQ.'/') GO TO 482
  481   CONTINUE
  482   DO 483 J=74,NP,-1
    LINE(K1-1)(J+6:J+6)=LINE(K1-1)(J:J)
  483   CONTINUE
    LINE(K1-1)(NP:NP+5)='*PLOT*'
    IF(LINE(K1-1)(80:80).NE.' ') LINE(K1-1)(80:80)='/'
    CALL CHECK(LINE,IANF,KMAX)
C
C   SCHREIBE GEAENDERTE FILE AUF FOR001
C
  489   REWIND(UNIT=1)
    WRITE(6,*) 'FOR001.DAT modified.'
    DO 550 I=1,IEND(KMAX)
    IF(LINE(I)(80:80).EQ.' ' .OR.
     Z     LINE(I)(80:80).EQ.'/') GO TO 500
    LINE(I)(80:80)='/'
    WRITE(6,7778)
  500   DO 510 J=1,80
    K=81-J
    IF(LINE(I)(K:K).NE.' ') GO TO 520
  510   CONTINUE
  520   IF(LINE(I)(1:1).NE.' ') GO TO 531
    WRITE(IFORM2(3:4),1004) K
    WRITE(1,IFORM2) LINE(I)
    GO TO 550
  531   WRITE(IFORM4(6:7),1004) K
    WRITE(1,IFORM4) LINE(I)
  550   CONTINUE
    GO TO  6000
C
C   FEHLERMELDUNG
C
 5000   IF (I .EQ. 1) GOTO 777
    WRITE(6,5001)
 5001   FORMAT(' No SENTINEL card found')
    GO TO 6000
  777   WRITE(6,7777)
    GO TO 6000
  779   WRITE(6,7779)
    GO TO 6000
  299   WRITE(6,7780)
    GO TO 199
C
 6000   STOP
C
 7777   FORMAT(' Beamline input_file (FOR001.DAT) not found.')
 7778   FORMAT(' Caution, title_card truncated.')
 7779   FORMAT(' Only one SENTINEL card found.')
 7780   FORMAT(' Input doesn''t make sense, try again or',
     X         ' consult HELP feature.')
    END

    SUBROUTINE FILLIN(ICASE,LABEL,RES)
    CHARACTER*15 RES
    CHARACTER*4 LABEL
    CHARACTER*8 HEADER,TRAILER
    DIMENSION HEADER(4)
    HEADER(1)='  10.0 /'
    HEADER(2)=' -10.0 /'
    HEADER(3)='  5.01 /'
    HEADER(4)='  5.0  /'
    TRAILER='/ ;     '
    DO 1 I=1,4
    L=5-I
    IF(LABEL(L:L).NE.' ') GO TO 2
    1   CONTINUE
    2   RES(1:8)=HEADER(ICASE)
    RES(9:8+L)=LABEL(1:L)
    RES(9+L:15)=TRAILER(1:7-L)
    RETURN 
    END

    SUBROUTINE STR3INT(STRING,I1,I2,I3,IER)
C
C   THIS ROUTINE EXTRACTS 3 INTERGERS (1 TO 4 DIGITS)
C   OUT OF ONE STRING (I1,I2:I3) .
C   FIRST CHARACTER OF STRING MAY BE BLANK
C
    CHARACTER*12 STRING
    CHARACTER*4  R1,R2,R3,RR(3)
    CHARACTER*1  R
    EQUIVALENCE(RR(1),R1),(RR(2),R2),(RR(3),R3)
    IER=0
    N1=0
    N2=0
    R1='    '
    R2='    '
    R3='    '
    N=1
    IF(STRING(1:1).EQ.' ') N=2
    DO 10 I=N,12
    R=STRING(I:I)
    IF(R.EQ.',') N1=I
    IF(R.EQ.':') N2=I
    IF(R.EQ.' ') GO TO 15
   10   CONTINUE
   15   N3=I
    IF(N1.EQ.0) GO TO 25
    IF(N2.NE.0) GO TO 17
    N2=N3
    N3=N2+2
   17   CONTINUE
    IF(N2.LT.N1) GO TO 40
    R1(6-N1:4)=STRING(1:N1-1)
    R2(6+N1-N2:4)=STRING(N1+1:N2-1)
    R3(6+N2-N3:4)=STRING(N2+1:N3-1)
    GO TO 30
   25   R1(6-N3:4)=STRING(1:N3-1)
   30   CONTINUE
    READ(R1,1001,ERR=40) I1
    READ(R2,1001,ERR=40) I2
    READ(R3,1001,ERR=40) I3
 1001   FORMAT(I4)
    RETURN
   40   IER=1
    RETURN
    END

    SUBROUTINE CHECK(LINE,IANF,KMAX)
C   KEEPS TRACK OVER DEACTEVATED FIT SEQUENCES
    CHARACTER*80 LINE(1)
    DIMENSION IANF(1),INR9(100)
    IF(KMAX .LT. 3) RETURN
    NR9=0
    DO 100 N=2,KMAX-1
    K1=IANF(N)
    DO 10 I=1,10
    IF (LINE(K1)(I:I) .EQ. '/') K1=IANF(N)+1
   10   CONTINUE
    IND=-1
    DO 20 I=1,10
    IF (LINE(K1)(I:I+1) .EQ. '-9') GO TO 30
   20   CONTINUE
    GO TO 100
   30   NR9=NR9+1
    INR9(NR9)=N
  100   CONTINUE
    IF (NR9 .EQ. KMAX-2) WRITE(6,1001)
 1001   FORMAT(' All Fit-Sequences are deactivated.')
    IF (NR9 .EQ. 0) WRITE(6,1002)
 1002   FORMAT(' All Fit-Sequences are activated.')
    IF (NR9 .NE. 0 .AND. NR9 .NE. KMAX-2)
     X  WRITE(6,1003) (INR9(J), J=1,NR9)
 1003   FORMAT(' Deactivated Sequences are: ',30(I2,','))
    RETURN
    END
