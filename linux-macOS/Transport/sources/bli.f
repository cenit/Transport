        PROGRAM BLI
C
C   PROGRAMM ZUM UMKEHREN VON BEAMLINES.
C
        CHARACTER*3 CH,CH2,CH3
        CHARACTER*5 IFORM1
        CHARACTER*8 IFORM2
        CHARACTER*8 TEST
        CHARACTER*80 LINE,SAVE,SIXT,TEMP
        CHARACTER*1 ANS
        DIMENSION LINE(1000),SIXT(16),PR(16)
        LOGICAL PR, BEAM
        DIMENSION I9(50),I9F(50)
        LOGICAL I9F, SLITS
c
        DATA IFORM1 /'(A80)'/
        DATA IFORM2 /'(1X,A80)'/
        DATA SLITS /.FALSE./
c
c   Open File
c
        OPEN (UNIT=1,FILE='FOR001.DAT',STATUS='OLD',ERR=777)
C
C   FINDE ERSTE SENTINEL KARTE
C   AUSSORTIEREN VON TOLERANCE KARTEN
C   AUSSORTIEREN VON UPDATE KARTEN
c
        I=1
        J=2
    1   READ(1,1001,ERR=777,END=5000) SAVE
 1001   FORMAT(A80)
        DO 2 K = 1, 10
         IF (SAVE(K:K) .NE. ' ') GO TO 15
    2   CONTINUE
        GO TO 1
c
   15   N = K
        TEMP = ' '
        TEMP(2:81-N) = SAVE(N:79)
        SAVE = TEMP
c
        IF ( (SAVE(J:J+6).EQ.'16. 14.') .OR.
     1       (SAVE(J:J+7).EQ.'16.0 14.') ) THEN
           SLITS = .TRUE.
        ENDIF
c
        IF (SAVE(J:J+7) .EQ. 'SENTINEL') GO TO 3
        IF (SAVE(J:J+1) .EQ. '8.') GO TO 1
        IF (.NOT.  SLITS) THEN
         IF (SAVE(J:J+1) .EQ. '6.') GO TO 1
        ENDIF
  100   LINE(I) = SAVE
        I = I+1
        GO TO 1
    3   IMAX = I - 1
C
        WRITE(6,*) '==>> Beamline will be reversed <<=='
        WRITE(6,*) 'All fit sequences will be deleted.'
        WRITE(6,*) 'Type codes 6 (update) and 8 will be deleted.'
        WRITE(6,*) 'Caution: Not all features may be treated correctly!'
        WRITE(6,1234) 
 1234   FORMAT(' Modify FOR001.DAT [Y/N] ? ',$)
        READ(5,'(A1)') ANS
        IF (ANS.EQ.'N' .OR. ANS.EQ.'n') GO TO 6000
C
C   FINDE KOPF DES BEAMLINE-DECKS
C
        BEAM = .FALSE.
        DO 10 I=1,IMAX
        CH=LINE(I)(J:J+2)
        IF (CH(1:2) .EQ. '1.') THEN
          BEAM = .TRUE.
          GO TO 10
        ENDIF
        IF (CH(1:2) .EQ. '7.') GO TO 10
        IF (CH .EQ. '12.') GO TO 10
        IF (CH .EQ. '13.') GO TO 10
        IF (CH .EQ. '14.') GO TO 10
        IF (CH .EQ. '15.') GO TO 10
        IF (CH .EQ. '17.') GO TO 10
        IF (CH .EQ. '-17') GO TO 10
        IF (CH .EQ. '22.') GO TO 10

        IF (LINE(I)(J:J+5).EQ.'16. 3.') GO TO 10
        IF (LINE(I)(J:J+5).EQ.'16. 9.') GO TO 10
        IF (LINE(I)(J:J+6).EQ.'16. 10.') GO TO 10
        IF (LINE(I)(J:J+6).EQ.'16. 14.') GO TO 10
        IF (LINE(I)(J:J+6).EQ.'16. 16.') GO TO 10
        IF (LINE(I)(J:J+6).EQ.'16. 17.') GO TO 10
        IF (LINE(I)(J:J+6).EQ.'16. 18.') GO TO 10
        IF (LINE(I)(J:J+6).EQ.'16. 19.') GO TO 10
        IF (LINE(I)(J:J+6).EQ.'16. 20.') GO TO 10

        IF (LINE(I)(J:J+6).EQ.'16.0 3.') GO TO 10
        IF (LINE(I)(J:J+6).EQ.'16.0 9.') GO TO 10
        IF (LINE(I)(J:J+7).EQ.'16.0 10.') GO TO 10
        IF (LINE(I)(J:J+7).EQ.'16.0 14.') GO TO 10
        IF (LINE(I)(J:J+7).EQ.'16.0 16.') GO TO 10
        IF (LINE(I)(J:J+7).EQ.'16.0 17.') GO TO 10
        IF (LINE(I)(J:J+7).EQ.'16.0 18.') GO TO 10
        IF (LINE(I)(J:J+7).EQ.'16.0 19.') GO TO 10
        IF (LINE(I)(J:J+7).EQ.'16.0 20.') GO TO 10

        IF (CH(1:1) .EQ. '/') GO TO 10
        IF (CH(1:1) .EQ. '(') GO TO 10   ! intervening comment
        IF (CH(1:2) .EQ. '0 ') GO TO 10
    4   IF (.NOT.BEAM) GO TO 10 ! leave only after beam-card
        GO TO 5
   10   CONTINUE
        GO TO 6000
    5   IKEND=I-1
C
C   SCHMEISSE RESTLICHE 1. UND 13. KARTEN RAUS
C   AENDERE VORZEICHEN VON XP & YP BEI TYPE CODE 7
C
        DO 20 I=IKEND+1,IMAX
        CH=LINE(I)(J:J+2)
        IF(CH.EQ.'13.') GO TO 16
        IF(CH(1:2).EQ.'1.') GO TO 16
        IF(CH(1:2).EQ.'7.') GO TO 17
        GO TO 20
   16   LINE(I)=' '
        GO TO 20
   17   CALL CHSIGN(LINE(I),2)
        CALL CHSIGN(LINE(I),4)
   20   CONTINUE
C
C   SORTIERE 20. +-180. VOR UND HINTER BEND AUS
C   MACHE AUS RECHTS EINEN LINKSBEND
C
        L=0
        DO 60 M=IKEND+1,IMAX+50
        I=M+L
        CH=LINE(I)(J:J+2)
        CH2=LINE(I+1)(J:J+2)
        CH3=LINE(I+2)(J:J+2)
        IF(CH(1:2).EQ.'2.' .AND. CH2.EQ.'20.') GO TO 36
        IF(CH.EQ.'20.' .AND. CH2(1:2).EQ.'2.') GO TO 37
        IF(CH.NE.'20.'.AND.CH2(1:2).EQ.'2.'.AND.CH3(1:2).EQ.'4.') 
     1   GO TO 38
        IF(CH(1:2).EQ.'4.'.AND.CH2(1:2).EQ.'2.'.AND.CH3.NE.'20.') 
     1   GO TO 50
        L=0
        GO TO 60
   36   IF(LINE(I+1)(J+4:J+7).EQ.'180.' .OR.
     1     LINE(I+1)(J+4:J+8).EQ.'-180.' ) LINE(I+1)=' '
        GO TO 60
   37   IF(LINE(I)(J+4:J+7).EQ.'180.' .OR.
     1     LINE(I)(J+4:J+8).EQ.'-180.' ) LINE(I)=' '
        GO TO 60
   38   DO 39 JXJ=I+1,IMAX
        K=IMAX+I+1-JXJ
   39   LINE(K+1)=LINE(K)
        LINE(I+1)=' 20. 180.0 ;'
        IMAX=IMAX+1
        L=1
        GO TO 60
   50   DO 51 JXJ=I+2,IMAX
        K=IMAX+I+2-JXJ
   51   LINE(K+1)=LINE(K)
        LINE(I+2)=' 20. -180.0 ; '
        IMAX=IMAX+1
        L=1
   60   CONTINUE
C
C   ARRANGIERE 16. KARTEN
C
        DO 62 JXJ=1,16
   62   PR(JXJ)=.FALSE.
        IN=0
        DO 90 II=IKEND+1,IMAX+50
        I=II+IN
        CH=LINE(I)(J:J+2)
        CH2=LINE(I+1)(J:J+2)
        CH3=LINE(I+2)(J:J+2)
        IF(CH.EQ.'16.'.AND.LINE(I)(J+4:J+5).EQ.'5.') GO TO 80
        IF(CH.EQ.'16.'.AND.LINE(I)(J+4:J+5).EQ.'7.') GO TO 82
        IF(CH.EQ.'16.'.AND.LINE(I)(J+4:J+5).EQ.'8.') GO TO 84
        IF(CH.EQ.'16.'.AND.LINE(I)(J+4:J+6).EQ.'12.') GO TO 86
        IF(CH.EQ.'16.'.AND.LINE(I)(J+4:J+6).EQ.'13.') GO TO 88
        IF(CH(1:2).EQ.'4.'.AND.CH2(1:2).EQ.'2.'.AND.CH3.EQ.'20.') 
     1     GO TO 64
        IF(CH(1:2).EQ.'4.'.AND.CH2(1:2).EQ.'2.'.AND.CH3.NE.'20.')
     1     GO TO 70
        GO TO 90
C
C   EINSETZEN DER 16.KARTEN NACH SEQUENZ 4. 2. 20.
C
   64   N=0
        DO 65 JXJ=1,16
        IF(PR(JXJ)) N=N+1
   65   CONTINUE
        DO 66 JXJ=I+3,IMAX
        K=IMAX+I+3-JXJ
   66   LINE(K+N)=LINE(K)
        L=0
        DO 68 JXJ=1,16
        IF(PR(JXJ)) GO TO 67
        GO TO 68
   67   L=L+1
        LINE(I+2+L)=SIXT(JXJ)
   68   CONTINUE
        IN=IN+L+2
        IMAX=IMAX+N
        GO TO 90
C
C   EINSETZEN DER 16. KARTEN NACH SEQUENZ 4. 2.
C
   70   N=0
        DO 71 JXJ=1,16
        IF(PR(JXJ)) N=N+1
   71   CONTINUE
        DO 72 JXJ=I+2,IMAX
        K=IMAX+I+2-JXJ
   72   LINE(K+N)=LINE(K)
        L=0
        DO 74 JXJ=1,16
        IF(PR(JXJ)) GO TO 73
        GO TO 74
   73   L=L+1
        LINE(I+1+L)=SIXT(JXJ)
   74   CONTINUE
        IN=IN+L+1
        IMAX=IMAX+N
        GO TO 90
C
C   ABSPEICHERN DER NEUESTEN 16. 5. KARTE
C
   80   PR(5)=.TRUE.
        SIXT(5)=LINE(I)
        LINE(I)=' '
        GO TO 90
C
C   ABSPEICHERN DER NEUESTEN 16. 7. KARTE
C
   82   PR(7)=.TRUE.
        SIXT(7)=LINE(I)
        LINE(I)=' '
        GO TO 90
C
C   ABSPEICHERN DER NEUESTEN 16. 8. KARTE
C
   84   PR(8)=.TRUE.
        SIXT(8)=LINE(I)
        LINE(I)=' '
        GO TO 90
C
C   ABSPEICHERN DER NEUESTEN 16. 12. KARTE
C   AUS 12 WIRD 13
C
   86   PR(12)=.TRUE.
        SIXT(12)=LINE(I)
        SIXT(12)(J+4:J+5)='13'
        LINE(I)=' '
        GO TO 90
C
C   ABSPEICHERN DER NEUESTEN 16. 13. KARTE
C   AUS 13 WIRD 12
C
   88   PR(13)=.TRUE.
        SIXT(13)=LINE(I)
        SIXT(13)(J+4:J+5)='12'
        LINE(I)=' '
   90   CONTINUE
C
C   SORTIERE BLANK KARTEN AUS
C
        N=0
        DO 30 II=1,IMAX
        I=II+N
        IF(LINE(I).EQ.' ') GO TO 25
        GO TO 30
   25   DO 26 JXJ=I,IMAX-1
   26   LINE(JXJ)=LINE(JXJ+1)
        IMAX=IMAX-1
        N=N-1
   30   CONTINUE
C
C   INVERSION DER BEAMLINE
C
        IND=(IMAX-IKEND)/2
        DO 400 JXJ=1,IND
        SAVE=LINE(JXJ+IKEND)
        LINE(JXJ+IKEND)=LINE(IMAX+1-JXJ)
        LINE(IMAX+1-JXJ)=SAVE
  400   CONTINUE
C
C   VERSCHIEBE LABELS NACHEINANDERFOLGENDER DRIFTS UM EINS NACH OBEN
C       INTERVENING CARDS MAY BE 13.,10., 6.
C
        DO 420 I=IKEND+1,IMAX-1
        IF (LINE(I)(J:J+1) .EQ. '3.') THEN
             DO 405 IJK = 1, 10
              IF (LINE(I+IJK)(J:J+1) .EQ. '6.') THEN
               GO TO 405
              ELSE IF (LINE(I+IJK)(J:J+2) .EQ. '-6.') THEN
               GO TO 405
              ELSE IF (LINE(I+IJK)(J:J+2) .EQ. '10.') THEN
               GO TO 405
              ELSE IF (LINE(I+IJK)(J:J+3) .EQ. '-10.') THEN
               GO TO 405
              ELSE IF (LINE(I+IJK)(J:J+3) .EQ. '-13.') THEN
               GO TO 405
              ELSE IF (LINE(I+IJK)(J:J+1) .EQ. '3.') THEN
               GOTO 411
              ELSE
               GO TO 420
              ENDIF
  405    CONTINUE
        ENDIF
        GO TO 420
  411   CALL LOLA(LINE(I),IPS1,LE1)
        CALL LOLA(LINE(I+IJK),IPS2,LE2)
        LINE(I)(IPS1:IPS1+LE1)=' '
        LINE(I)(IPS1:IPS1+LE2)=LINE(I+IJK)(IPS2:IPS2+LE2)
        LINE(I+IJK)(IPS2:IPS2+LE2)=' '
        LINE(I+IJK)(IPS2:IPS2)=';'
  420   CONTINUE
C
C   REPEAT CODES (9. X. ; -- 9. 0. ;)
C
        L=0
        DO 440 I=IKEND+1,IMAX
        CH=LINE(I)(J:J+1)
        IF(CH.EQ.'9.') GO TO 433
        GO TO 440
  433   L=L+1
        IF(LINE(I)(J+3:J+4) .EQ. '.0') GO TO 436
        I9F(L)=.FALSE.
        GO TO 439
  436   I9F(L)=.TRUE.
  439   I9(L)=I
  440   CONTINUE
        IF(L.EQ.0) GO TO 470
        I=1
  450   CONTINUE
        IF(I9F(I).AND..NOT.I9F(I+1)) GO TO 455
        IF(I9F(I).AND..NOT.I9F(I+3).AND.I9F(I+1).AND..NOT.
     1      I9F(I+2)) GO TO 460
        GO TO 470
C
C   EINFACHE SCHACHTELUNG
C
  455   SAVE=LINE(I9(I))
        LINE(I9(I))=LINE(I9(I+1))
        LINE(I9(I+1))=SAVE
        I=I+2
        IF(I.LT.L) GO TO 450
        GO TO 470
C
C   ZWEIFACHE SCHACHTELUNG
C
  460   SAVE=LINE(I9(I))
        LINE(I9(I))=LINE(I9(I+3))
        LINE(I9(I+3))=SAVE
        SAVE=LINE(I9(I+1))
        LINE(I9(I+1))=LINE(I9(I+2))
        LINE(I9(I+2))=SAVE
        I=I+4
        IF(I.LT.L) GO TO 450
  470   CONTINUE
C
C   SETZE *PLOT* IN TITELKARTE EIN
C
        IF(LINE(1)(2:2) .NE. '/') GO TO 490
        IF(LINE(1)(3:8) .EQ. '*PLOT*') GO TO 490 
        LINE(1)(9:80)=LINE(1)(3:74) 
        LINE(1)(3:8)='*PLOT*'
        IF(LINE(1)(80:80).NE.' ') LINE(1)(80:80)='/'
C
C   SCHREIBE GEAENDERTES FILE AUF FOR001
C
  490   REWIND(UNIT=1)
        DO 550 I=1,IMAX
        DO 510 JXJ=1,80
        K=81-JXJ
        IF(LINE(I)(K:K).NE.' ') GO TO 520
  510   CONTINUE
  520   IF(LINE(I)(1:1).NE.' ') GO TO 531
        WRITE(IFORM1(3:4),3002) K
        WRITE(1,IFORM1) LINE(I)
 3002   FORMAT(I2)
        GO TO 550
  531   WRITE(IFORM2(6:7),3002) K
        WRITE(1,IFORM2) LINE(I)
  550   CONTINUE
C
C   SCHREIBE ZWEI SENTINEL KARTEN
C
        DO 480 JXJ=1,2
        WRITE(1,3113)
 3113   FORMAT(' SENTINEL')
  480   CONTINUE
        GO TO  6000
C
C   FEHLERMELDUNG
C
 5000   WRITE(6,5001)
 5001   FORMAT(' No SENTINEL card found.')
        GO TO 6000
  777   WRITE(6,7777)
 7777   FORMAT(' Beamline Input_file (FOR001.DAT) not found.') 
C
C
C
 6000   CALL EXIT
        END


        SUBROUTINE LOLA(LINE,IPS,LE)
C
C   LOKALISIERT LABEL ANFANG(IPS) UND SEINE LAENGE INC ; (LE) .
C
        CHARACTER*80 LINE
        DO 10 I=1,80
        J=81-I
        IF(LINE(J:J).EQ.';') GO TO 11
   10   CONTINUE
   11   IF(LINE(J-2:J-2).EQ.'/') GO TO 12
        IPS=J
        LE=0
        RETURN
   12   DO 15 I=J-7,J-4
        IF(LINE(I:I).EQ.'/') GO TO 20
   15   CONTINUE
   20   LE=J-I
        IPS=I
        RETURN
        END


        SUBROUTINE CHSIGN(LINE,N)
        CHARACTER*80 LINE
C
C   VORZEICHENWECHSEL DES NTEN DATUMS AUF KARTE LINE
C
        L=0
        DO 10 I=3,80
        IF(LINE(I:I).EQ.' ') L=L+1
        IF(L.EQ.N) GO TO 20
   10   CONTINUE
        RETURN
   20   IF(LINE(I+1:I+1).EQ.'-') GO TO 50
        DO 30 J=I+1,79
        K=80+I-J
   30   LINE(K+1:K+1)=LINE(K:K)
        LINE(I+1:I+1)='-'
        RETURN
   50   DO 60 J=I+1,79
   60   LINE(J:J)=LINE(J+1:J+1)
        RETURN
        END
