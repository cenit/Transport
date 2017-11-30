      PROGRAM DISPLAW
C
C     PROGRAM DRAWS FRAME FOR GRAPHIC TRANSPORT OUTPUT
C     OUTPUTS ENVELOPES FOR GRAPHIC TRANSPORT
C     OPTIMIZED FOR SMOOTH HP-PLOTTER OUTPUT
C     AUTHOR: U.ROHRER (SIN), JANUARY 1984
C     ADAPTED FOR WORKSTATION UIS BY U.ROHRER (PSI), JANUARY 1989
C     ADAPTED FOR X-WINDOWS UNDER VMS BY U.ROHRER (PSI), DATE UNKNOWN
C     ADAPTED FOR LINUX BY U.ROHRER (PSI), DECEMBER 1994
C     WINDOW SIZE AND OFFSET HANDLING ADDED, U.R.(PSI), MAR.2005
C
      CHARACTER*80 STR
      CHARACTER*11 MULTIST
      CHARACTER*90 UEBER,TEXT
      character*30 menut(8)
      CHARACTER NAMEX*80,IJK*20
      CHARACTER*1 NAM(4,200), EXI(9,200), NAM1
      INTEGER*4 ITT(3),JUMP(7),MM(100),IM(1000)
      INTEGER*4 GETPAR
      REAL Z(401),X(401),Y(401),DX(401),DY(401)
      REAL RAX(401),RAY(401)
      REAL IZ1(1000),IZ2(1000),IX(1000),IY(1000)
      LOGICAL FIRST,SLITCONT,SLITLAB,LTR,BYPASS,OPEN,COLOR
      LOGICAL SECORD
C
      DATA JUMP /3*0,3,3*0/
      DATA SLITCONT,SLITLAB /.FALSE.,.FALSE./
      DATA BYPASS, OPEN /.FALSE., .TRUE./
      DATA SECORD /.FALSE./
C
      COLOR = .TRUE.
      SCALX=0.8
      SCALY=0.8
      SCALY2=0.842
      XOFF=5.0
      YOFF=50.0
C
C       TRY TO OVERRIDE XOFF AND YOFF WITH VALUES FROM TPMENU
C
      OPEN(UNIT=49,FILE='.dispoff',STATUS='OLD',ERR=10)
      READ(49,'(4I6)',ERR=10) IXOFF,IYOFF,IWIDTH,IHEIGHT
      CLOSE(UNIT=49)
      XOFF = IXOFF
      YOFF = IYOFF
      SCALX = FLOAT(IWIDTH) / 1000.0
      SCALY = FLOAT(IHEIGHT) / 800.0
      SCALY2 = FLOAT(IHEIGHT) / 760.0
C
 10   CONTINUE
C
C     (dl,dp/p) ELLIPSEN DISPLAY
C
      CALL GETARG(1,STR)
      IF (STR(1:1) .EQ. 'L') THEN
       CALL MYSLEEP(500)
       CALL INIXWI('ELLIPSE (dl,dp/p)'//char(0),XOFF,YOFF,500.*SCALX,
     1             760.*SCALY2)
       CALL ERASE
       CALL ELIW2(SCALX,SCALY2)
       STOP   
      ENDIF
C
C     (x,x' and y,y') ELLIPSEN DISPLAY
C
      IF (STR(1:1) .EQ. 'E') THEN
       CALL MYSLEEP(500)
       CALL INIXWI('ELLIPSES (x,x'' and y,y'')'//char(0),XOFF,YOFF,
     1              1000.*SCALX,760.*SCALY2)
       CALL ERASE
       CALL ELIW(SCALX,SCALY2)
       STOP   
      ENDIF
C
C     (x,y and x',y') ELLIPSEN DISPLAY
C
      IF (STR(1:1) .EQ. 'S') THEN
       CALL MYSLEEP(500)
       CALL INIXWI('ELLIPSES (x,y and x'',y'')'//char(0),XOFF,YOFF,
     1              1000.*SCALX,760.*SCALY2)
       CALL ERASE
       CALL ELIW3(SCALX,SCALY2)
       STOP   
      ENDIF
C
C     SAVE RUN
C
 1    IF (STR(1:1) .EQ. 'K') THEN
       CALL SAVERUN
       STOP
      ENDIF
C
C     INITIALIZE WORKSTATION GRAPHICS
C   
 101  CALL INIXWI('TRANSPORT'//char(0),XOFF,YOFF,1000.0*SCALX,
     1               800.0*SCALY)
 2    CALL ERASE
      CALL SET_COL(1) ! WEISS
      ICOUNT = 0
C
C     EINLESEN VON TITEL , MAGNETLABELS & ERREGUNGEN
C
      OPEN(UNIT=9,FILE='FOR009.DAT',STATUS='OLD')
      READ(9,1004,ERR=100) TEXT    
      READ(TEXT,1007) ZMIN,ZMAX
      READ(9,1004) UEBER
      READ(9,1005) IYUEB
      READ(9,1005) ISPEZ2
C
C     EINZEICHNEN DES RAHMENS UND X/Y-ACHSEN EINTEILUNGEN
C
      CALL GRAMOV(0.,0.)
      CALL GRADRW(0.,700.*SCALY)
      CALL GRADRW(1000.*SCALX,700.*SCALY)    
      CALL GRADRW(1000.*SCALX,1.)
      CALL GRADRW(0.,1.)

      DO 45 I=1,10         
      CALL GRAMOV(1000.*SCALX,70.*FLOAT(I)*SCALY)       
      CALL GRADRW(995.*SCALX,70.*FLOAT(I)*SCALY)      
 45   CONTINUE
        
      DO 46 I=1,10         
      CALL GRAMOV(0.,70.*FLOAT(I-1)*SCALY)         
      CALL GRADRW(5.*SCALX,70.*FLOAT(I-1)*SCALY)         
 46   CONTINUE        

C
C     EINZEICHNEN DER Z-ACHSE UND DEREN EINTEILUNG
C
      YO=355.*SCALY
      YM=350.*SCALY
      YU=345.*SCALY
      TAUS=1000.*SCALX
      SOLD=TAUS
      SCALE=(1000.*SCALX)/(ZMAX-ZMIN)
      IZMIN=ZMIN
      IZMAX=ZMAX
      N=IZMAX-IZMIN
      K=1
      IF (N .GT. 20)  K=2
      IF (N .GT. 50)  K=5
      IF (N .GT. 200) K=20
      IF (N .GT. 500) K=50
      ANF=SCALE*(ZMIN-FLOAT(IZMIN))
      DO 60 I=N/K+1,1,-1
      S=-ANF+FLOAT(I)*K*SCALE
C     S=S*SCALX                   ! longtime error
      IF (S .GT. TAUS) GO TO 60
      CALL GRAMOV(SOLD,YU)
      CALL GRADRW(SOLD,YM)
      CALL GRADRW(S,YM)
      CALL GRADRW(S,YO)
      SOLD=S
 60   CONTINUE
      CALL GRAMOV(SOLD,YU)
      CALL GRADRW(SOLD,YM)
      CALL GRADRW(0.,YM)
C
C     SCHREIBEN DER UEBERSCHRIFT
C     SCHREIBEN DES TITELS
C
      IF(IYUEB.NE.0)
     1  CALL TEXT_OUT(UEBER//char(0), 0., IYUEB*SCALY,1)
      OPEN(UNIT=10,FILE='FOR010.DAT',STATUS='OLD',
     1     FORM='UNFORMATTED')
      READ(10,ERR=100) ISPEZ,ILABS
      IF (ISPEZ.NE.2 .AND. ISPEZ.NE.3)
     1  CALL TEXT_OUT(TEXT//char(0), 0.,705.*SCALY,1)
C
C     EINLESEN DER MAGNETPOSITIONEN
C
      I=1        
      OPEN(UNIT=8,FILE='FOR008.DAT',STATUS='OLD',
     1     FORM='UNFORMATTED')
      READ(8,ERR=151) ICOMP
 30   READ(8,END=40,ERR=151) Z1,Z2,XX,YY     
      IF(Z1.GT.1500.0) THEN
       SLITCONT=.TRUE.
       GO TO 40
      ENDIF
      IM(I) = 0
      IZ1(I)=Z1*SCALX
      IZ2(I)=Z2*SCALX
      IX(I)=XX*SCALY
      IY(I)=YY*SCALY
      IF (XX.EQ.25.0 .OR. YY.EQ.675.0) IM(I) = 1  !   BEND
      I=I+1      
      GO TO 30   
 40   N=I-1 
C
C     EINZEICHNEN DER MAGNETE (Y-APERTUREN)
C
      FIRST = .TRUE.
      DO 950 I=N,1,-1
      IF (IM(I) .EQ. 0) THEN
       CALL SET_COL(2) ! ROT
      ELSE
       CALL SET_COL(4) ! BLAU
      ENDIF
      IF (IY(I)/SCALY .GE. 700.) GO TO 950
      IF(I.EQ.1) GO TO 920
      IF(IZ1(I-1).EQ.IZ2(I)) GO TO 905
          IF(.NOT.FIRST) FIRST=.TRUE.
      GO TO 920
 905  IF(FIRST) GO TO 915
      CALL GRAMOV(IZ1(I),IY(I))
      CALL GRADRW(IZ1(I-1),IY(I-1))
      GO TO 950
 915  FIRST=.FALSE.
 920  IF (ICOMP .EQ. 1) THEN
        CALL GRAMOV(IZ1(I),350.*SCALY)    
        CALL GRADRW(IZ1(I),IY(I))       
        CALL GRADRW(IZ2(I),IY(I))       
        CALL GRADRW(IZ2(I),350.*SCALY)
      ELSE
        CALL GRAMOV(IZ1(I),IY(I)+25.*SCALY)    
        CALL GRADRW(IZ1(I),IY(I))       
        CALL GRADRW(IZ2(I),IY(I))       
        CALL GRADRW(IZ2(I),IY(I)+25.*SCALY)
      ENDIF
 950  CONTINUE        
C
C     EINZEICHNEN DER MAGNETE (X-APERTUREN)
C
      FIRST = .TRUE.
      DO 50 I=1,N
      IF (IM(I) .EQ. 0) THEN
       CALL SET_COL(2) ! ROT
      ELSE
       CALL SET_COL(4) ! BLAU
      ENDIF
      IF (IX(I)/SCALY .LE. 0.) GO TO 50
      IF(I.EQ.N) GO TO 20
      IF(IZ1(I).EQ.IZ2(I+1)) GO TO 5
          IF(.NOT.FIRST) FIRST=.TRUE.
      GO TO 20
 5    IF(FIRST) GO TO 15
      CALL GRAMOV(IZ2(I),IX(I))
      CALL GRADRW(IZ2(I+1),IX(I+1))
      GO TO 50
 15   FIRST=.FALSE.
 20   IF (ICOMP .EQ. 1) THEN
        CALL GRAMOV(IZ2(I),350.*SCALY)    
        CALL GRADRW(IZ2(I),IX(I))       
        CALL GRADRW(IZ1(I),IX(I))       
        CALL GRADRW(IZ1(I),350.*SCALY)    
      ELSE
        CALL GRAMOV(IZ2(I),IX(I)-25.*SCALY)    
        CALL GRADRW(IZ2(I),IX(I))       
        CALL GRADRW(IZ1(I),IX(I))       
        CALL GRADRW(IZ1(I),IX(I)-25.*SCALY)    
      ENDIF
 50   CONTINUE        
      LTR=.FALSE.
C
C     EINLESEN DER BLENDEN
C
      IF(SLITCONT) THEN
      I=1        
 130  READ(8,END=140,ERR=140) Z1,XX,YY     
      IZ1(I)=Z1*SCALX
      IX(I)=XX*SCALY
      IY(I)=YY*SCALY
      I=I+1      
      GO TO 130   
 140  N=I-1 
C
C     EINZEICHNEN DER BLENDEN (X-APERTUREN)
C
      CALL SET_COL(1) ! WEISS
      CALL LOGIC(LTR,N,1,J1,J2,J3)
      SX=2.*SCALX
      SY=4.*SCALY
      SZ=30.*SCALY
      DO 350 I=J1,J2,J3
      IF (IX(I)/SCALY .LE. 0.) GO TO 350
      CALL GRAMOV(IZ1(I),IX(I)-SZ)    
      CALL GRADRW(IZ1(I),IX(I))       
      CALL GRAMOV(IZ1(I)-SX,IX(I)-SY)
      CALL GRADRW(IZ1(I),IX(I))
      CALL GRADRW(IZ1(I)+SX,IX(I)-SY)
 350  CONTINUE        
      CALL REVERSE(LTR)
C
C     EINZEICHNEN DEL BLENDEN (Y-APERTUREN)
C
      CALL LOGIC(LTR,N,1,J1,J2,J3)
      DO 352 I=J1,J2,J3
      IF (IY(I)/SCALY .GE. 700.) GO TO 352
      CALL GRAMOV(IZ1(I),IY(I)+SZ)    
      CALL GRADRW(IZ1(I),IY(I))       
      CALL GRAMOV(IZ1(I)-SX,IY(I)+SY)
      CALL GRADRW(IZ1(I),IY(I))
      CALL GRADRW(IZ1(I)+SX,IY(I)+SY)
 352  CONTINUE        
      CALL REVERSE(LTR)
      ENDIF
C
C     EINLESEN VON MAGNETLABELS & ERREGUNGEN
C
 151  I=1        
      REM=-10.0
      REM2=-10.0  !!
 51   READ(9,1013,END=53) Z1,(NAM(K,I),K=1,4),(EXI(K,I),K=1,9)
      IF(Z1.GT.1500.0) THEN 
       SLITLAB=.TRUE.
       GO TO 53
      ENDIF
      Z1=MAX(2.*SCALX,Z1*SCALX-3.5*SCALY)
      IF(Z1.LT.REM2) GO TO 51  !!
      IF(Z1-REM .LT. 10.*SCALY) THEN
       IMAL=IMAL+1
       IF(IMAL.GT.4) THEN
        IMAL=0
        REM2=REM  !!
        GO TO 51
       ENDIF
       IF(IMAL.EQ.1 .AND. I.GT.1) THEN
        Z(I-1)=Z(I-1)-(5.-((Z1-REM)/(2.*SCALY)))*SCALY
        REM=Z(I-1)
       ENDIF
       Z1=REM+10.*SCALY
      ELSE
           IMAL=0
      ENDIF
      REM=Z1
      Z(I)=Z1  
      I=I+1      
      GO TO 51   
 53   NN=I-1 
C
C     ANSCHREIBEN DER MAGNETE
C
      IF (NN .EQ. 0) GO TO 357
      IF (ISPEZ .NE. 2 .AND. ISPEZ.NE.3) THEN
       CALL LOGIC(LTR,NN,1,J1,J2,J3)
       DO 59 I=J1,J2,2*J3
        IF (EXI(9,I).EQ.'1') THEN ! 9TH BYTE USED AS COLOR INFO
         CALL SET_COL(4) ! BLAU
        ELSE
         CALL SET_COL(2) ! ROT
        ENDIF
        DO 58 K=1,4
         XX=700.-15.*FLOAT(K)
             NAM1 = NAM(K,I)
 58     CALL TEXT_OUT(NAM1//char(0), Z(I), SCALY*XX,1)
      IF(I+J3.LT.1 .OR. I+J3.GT.NN) GO TO 59
      IF (EXI(9,I+J3).EQ.'1') THEN
       CALL SET_COL(4) ! BLAU
      ELSE
       CALL SET_COL(2) ! ROT
      ENDIF
      DO 158 K=4,1,-1
       XX=700.-15.*FLOAT(K)
           NAM1 = NAM(K,I+J3)
 158     CALL TEXT_OUT(NAM1//char(0), Z(I+J3), SCALY*XX,1)
 59    CONTINUE
      CALL REVERSE(LTR)
      ENDIF
C
C     AUSSCHREIBEN DER ERREGUNGEN
C
      IF (ISPEZ2 .NE. 0) THEN
       CALL LOGIC(LTR,NN,1,J1,J2,J3)
       DO 56 I=J1,J2,2*J3
        IF (EXI(9,I).EQ.'1') THEN ! 9TH BYTE USED AS COLOR INFO
         CALL SET_COL(4) ! BLAU
        ELSE
         CALL SET_COL(2) ! ROT
        ENDIF
        DO 55 K=1,7
         XX=140.-15.*FLOAT(K)+FLOAT(JUMP(K))
             NAM1 = EXI(K,I)
 55     CALL TEXT_OUT(NAM1//char(0), Z(I), SCALY*XX,1)
      IF(I+J3.LT.1 .OR. I+J3.GT.NN) GO TO 56
      IF (EXI(9,I+J3).EQ.'1') THEN
       CALL SET_COL(4) ! BLAU
      ELSE
       CALL SET_COL(2) ! ROT
      ENDIF
      DO 155 K=7,1,-1
       XX=140.-15.*FLOAT(K)+FLOAT(JUMP(K))
           NAM1 = EXI(K,I+J3)
 155     CALL TEXT_OUT(NAM1//char(0), Z(I+J3), SCALY*XX,1)
 56    CONTINUE
       CALL REVERSE(LTR)
      ENDIF
      CALL SET_COL(1) ! WEISS
C
C     EINLESEN VON SLIT LABELS
C
 357  IF(SLITLAB) THEN
      I=1        
      REM=-10.0
      REM2=-10.0
 351  READ(9,1003,END=353) Z1,(NAM(K,I),K=1,4)
      Z1=MAX(2.*SCALX,Z1*SCALX-3.5*SCALY)
      IF(Z1.LT.REM2) GO TO 351
      IF(Z1-REM .LT. 10.*SCALY) THEN
      IMAL=IMAL+1
      IF(IMAL.GT.4) THEN
       IMAL=0
       REM2=REM
       GO TO 351
      ENDIF
      IF(IMAL.EQ.1 .AND. I.GT.1) THEN
       Z(I-1)=Z(I-1)-(5.-((Z1-REM)/(2.*SCALY)))*SCALY
       REM=Z(I-1)
      ENDIF
      Z1=REM+10.*SCALY
      ELSE
           IMAL=0
      ENDIF
      REM=Z1
      Z(I)=Z1  
      I=I+1      
      GO TO 351
 353    NN=I-1 
C
C     ANSCHREIBEN DER SLITS
C
      IF (NN .EQ. 0) GO TO 57
      IF (ISPEZ.NE.2 .AND. ISPEZ.NE.3) THEN
      CALL LOGIC(LTR,NN,1,J1,J2,J3)
      DO 359 I=J1,J2,2*J3
       DO 358 K=1,4
        XX=65.-15.*FLOAT(K)
            NAM1 = NAM(K,I)
 358      CALL TEXT_OUT(NAM1//char(0), Z(I), SCALY*XX,1)
       IF(I+J3.LT.1 .OR. I+J3.GT.NN) GO TO 359
       DO 458 K=4,1,-1
        XX=65.-15.*FLOAT(K)
            NAM1 = NAM(K,I+J3)
 458      CALL TEXT_OUT(NAM1//char(0), Z(I+J3), SCALY*XX,1)
 359     CONTINUE
      CALL REVERSE(LTR)
      ENDIF
      ENDIF
C
C     EINLESEN DER POSITIONEN & LABELS VON SPEZIALELEM
C
 57   IF(ISPEZ.EQ.0 .OR. ISPEZ.EQ.2) GO TO 90
      I=1        
      REM=-10.
      REM2=-10. !!!
 80   READ(10,END=82) Z1,(NAM(K,I),K=1,4)
      IF(NAM(1,I).EQ.'S' .AND. ILABS.NE.0) GO TO 80
      IZ1(I)=Z1*SCALX
      Z(I)=MAX(2.*SCALX,Z1*SCALX-4.*SCALY)
      IX(I)=0
      IF(Z(I).LT.REM2) GO TO 180  !!!
      IF(Z(I)-REM .LT. 10.*SCALY) THEN
       IMAL=IMAL+1
       IF(IMAL.GT.4) THEN
        IMAL=0
        REM2=REM  !!!
        GO TO 180
       ENDIF
       IF(IMAL.EQ.1 .AND. I.GT.1) THEN
        Z(I-1)=Z(I-1)-(5.-((Z(I)-REM)/(2.*SCALY)))*SCALY
        REM=Z(I-1)
       ENDIF
       Z(I)=REM+10.*SCALY
      ELSE
           IMAL=0
      ENDIF
      IX(I)=1.
      REM=Z(I)
 180   I=I+1      
 182   GO TO 80   
 82   N=I-1 
C
C     EINZEICHNEN & ANSCHREIBEN DER SPEZIALELEMENTE
C
      CALL SET_COL(5) ! CYAN
      IF (N. EQ. 0) GO TO 90
      CALL LOGIC(LTR,N,1,J1,J2,J3)
      DO 89 I=J1,J2,2*J3
      IF (IX(I) .NE. 0.) THEN
       DO 87 K=1,4
        XX=640.-15.*FLOAT(K)
        NAM1 = NAM(K,I)
 87    CALL TEXT_OUT(NAM1//char(0), Z(I), SCALY*XX,1)
      ENDIF
      CALL GRAMOV(IZ1(I),575.*SCALY)    
      CALL GRADRW(IZ1(I),350.*SCALY)         
      IF (I+J3.LT.1 .OR. I+J3.GT.N) GO TO 89
      CALL GRAMOV(IZ1(I+J3),350.*SCALY)         
      CALL GRADRW(IZ1(I+J3),575.*SCALY)    
      IF (IX(I+J3) .NE. 0.) THEN
       DO 287 K=4,1,-1
        XX=640.-15.*FLOAT(K)
        NAM1 = NAM(K,I+J3)
 287    CALL TEXT_OUT(NAM1//char(0), Z(I+J3), SCALY*XX,1)
      ENDIF
 89   CONTINUE
      CALL REVERSE(LTR)
C
C     EINLESEN DER POSITIONEN & LABELS VON STEUERMAGNETEN
C
 90   IF(ILABS.LE.0) GO TO 190
      REWIND 10
      READ(10) ISPEZ,ILABS
      I=1        
      REM=-10.
      REM2=-10.
 280  READ(10,END=282) Z1,(NAM(K,I),K=1,4)
      IF(NAM(1,I).NE.'S') GO TO 280
      IZ1(I)=Z1*SCALX
      Z(I)=MAX(2.*SCALX,Z1*SCALX-4.*SCALY)
      IX(I)=0
      IF(Z(I).LT.REM2) GO TO 283
      IF(Z(I)-REM .LT. 10.*SCALY) THEN
       IMAL=IMAL+1
       IF(IMAL.GT.4) THEN
        IMAL=0
        REM2=REM
        GO TO 283
       ENDIF
       IF(IMAL.EQ.1 .AND. I.GT.1) THEN
        Z(I-1)=Z(I-1)-(5.-((Z(I)-REM)/(2.*SCALY)))*SCALY
        REM=Z(I-1)
       ENDIF
       Z(I)=REM+10.*SCALY
      ELSE
       IMAL=0
      ENDIF
      IX(I)=1.
      REM=Z(I)
 283   I=I+1      
 284   GO TO 280   
 282   N=I-1 
      IF (N .EQ. 0) GO TO 190
C
C     EINZEICHNEN & ANSCHREIBEN DER STEUERMAGNETE
C
      CALL SET_COL(3) ! GRUEN
      CALL LOGIC(LTR,N,1,J1,J2,J3)
      DO 189 I=J1,J2,2*J3
      CALL GRAMOV(IZ1(I),350.*SCALY)         
      CALL GRADRW(IZ1(I),125.*SCALY)    
      IF (IX(I) .NE. 0.) THEN
       DO 187 K=1,4
        XX=125.-15.*FLOAT(K)
            NAM1 = NAM(K,I)
 187    CALL TEXT_OUT(NAM1//char(0), Z(I),SCALY*XX,1)
      ENDIF
      IF (I+J3.LT.1 .OR. I+J3.GT.N) GO TO 189
      IF (IX(I+J3) .NE. 0.) THEN
       DO 387 K=4,1,-1
        XX=125.-15.*FLOAT(K)
        NAM1 = NAM(K,I+J3)
 387    CALL TEXT_OUT(NAM1//char(0), Z(I+J3), SCALY*XX,1)
      ENDIF
      CALL GRAMOV(IZ1(I+J3),125.*SCALY)    
      CALL GRADRW(IZ1(I+J3),350.*SCALY)         
 189  CONTINUE
      CALL REVERSE(LTR)
C
C     EINLESEN DER MESSUNGEN
C
 190  OPEN(UNIT=14,FILE='FOR014.DAT',STATUS='OLD',
     1       FORM='UNFORMATTED')
      READ(14,ERR=500) ITT
      IF(ITT(1)+ITT(2)+ITT(3) .NE. 0) THEN
      I=1        
 91   READ(14,END=92) L1,L2,L3
      MM(I)=L1
      IZ1(I)=L2
      IX(I)=L3
      I=I+1      
      GO TO 91   
 92   N=I-1 
      IF (N .EQ. 0) GO TO 500
      ENDIF
C
C     EINZEICHNEN DER X-MESSPUNKTE
C
      CALL SET_COL(1) ! WEISS
      IF (ITT(1) .GE. 1) THEN
       CALL LOGIC(LTR,N,1,J1,J2,J3)
       DO 99 J=J1,J2,J3
        IF (MM(J) .EQ. 1) THEN
         CALL GRAMOV(SCALX*(IZ1(J)-4.),SCALY*IX(J))     
         CALL GRADRW(SCALX*(IZ1(J)+4.),SCALY*IX(J))     
         CALL GRAMOV(SCALX*IZ1(J),SCALY*IX(J))       
         CALL GRADRW(SCALX*IZ1(J),SCALY*(IX(J)-8.))       
        ENDIF
 99    CONTINUE        
       CALL REVERSE(LTR)
      ENDIF
C
C     EINZEICHNEN DER Y-MESSPUNKTE
C
      IF (ITT(2) .GE. 1) THEN
       CALL LOGIC(LTR,N,1,J1,J2,J3)
       DO 98 J=J1,J2,J3
        IF (MM(J) .EQ. 2) THEN
         CALL GRAMOV(SCALX*(IZ1(J)-4.),SCALY*IX(J))     
         CALL GRADRW(SCALX*(IZ1(J)+4.),SCALY*IX(J))     
         CALL GRAMOV(SCALX*IZ1(J),SCALY*IX(J))       
         CALL GRADRW(SCALX*IZ1(J),SCALY*(IX(J)+8.))
        ENDIF
 98    CONTINUE        
       CALL REVERSE(LTR)
      ENDIF
C
C     EINZEICHNEN DER DP/P- UND SHIFT-MESSPUNKTE
C
      IF (ITT(3) .GE. 1) THEN
       CALL LOGIC(LTR,N,1,J1,J2,J3)
       DO 97 J=J1,J2,J3
        IF (MM(J) .EQ. 3) THEN
         CALL GRAMOV(SCALX*(IZ1(J)-4.),SCALY*IX(J))
         CALL GRADRW(SCALX*(IZ1(J)+4.),SCALY*IX(J))
         CALL GRAMOV(SCALX*IZ1(J),SCALY*(IX(J)-4.))
         CALL GRADRW(SCALX*IZ1(J),SCALY*(IX(J)+4.))
        ENDIF
 97    CONTINUE        
       CALL REVERSE(LTR)
      ENDIF
C
C     SECOND ORDER TIJK DISPLAY
C
 500  CONTINUE
      CALL GETARG(1,STR)
      IF (STR(1:3) .NE. '2ND') GO TO 540
      SECORD = .TRUE.
      CALL SET_COL(1) ! WEISS
      CALL GETARG(2,IJK(2:4))
      CALL GETARG(3,IJK(6:8))
      CALL GETARG(4,IJK(10:12))
      CALL GETARG(5,IJK(14:16))
      CALL DTMW(IJK,ZMIN,ZMAX,IYUEB,LTR,SCALX,SCALY)
      GO TO 100
 540  CONTINUE
C
C     LOOP OVER DIFFERENT ENVELOPES
C
      ICNT = 0
      IV = 0
 579  CONTINUE
      IV = IV + 1
      IF (IV .EQ. 12) GO TO 600
      IF (IV .EQ. 1) THEN
            NAMEX = 'FOR007.DAT'
      ELSE
        NAMEX = 'FOR007.DAT.0'
            NAMEX(12:12) = CHAR(48+IV-2)
      ENDIF
C  
        ICOL = MOD(ICNT,4) + 1
        IF (ICOL .EQ. 4) ICOL = 5
        ICNT = ICNT + 1
        CALL SET_COL(ICOL) ! WEISS, ROT, GRUEN, CYAN, WEISS, ROT, ...
C  
C     EINLESEN DER ENVELOPPEN
C
 508  CONTINUE
      OPEN(UNIT=7,FILE=NAMEX,STATUS='OLD',FORM='UNFORMATTED',ERR=600)
      READ(7) IENV,IENV,IDISP,KDISP,IRAX,IRAY,N,ZM
      READ(7) (Z(I),X(I),Y(I),DX(I),DY(I),RAX(I),RAY(I),
     1         I=1,N)
      CLOSE(UNIT=7)
C
      DO 506 I=1,N
      Z(I)=Z(I)*SCALX
      X(I)=X(I)*SCALY
      Y(I)=Y(I)*SCALY
      DX(I)=DX(I)*SCALY
      DY(I)=DY(I)*SCALY
      RAX(I)=RAX(I)*SCALY
      RAY(I)=RAY(I)*SCALY
 506  CONTINUE
C
C     ZEICHEN DER ENVELOPPEN
C
 507  CONTINUE
      LTR = .FALSE.
      IF(IENV.EQ.0) GO TO 501
      CALL LOGIC(LTR,N,1,L1,L2,L3)
      CALL GRAMOV(Z(L1),X(L1))
      DO 511 I=L1,L2,L3
 511  CALL GRADRW(Z(I),X(I))
      CALL REVERSE(LTR)
C
      CALL LOGIC(LTR,N,1,L1,L2,L3)
      CALL GRAMOV(Z(L1),Y(L1))
      DO 512 I=L1,L2,L3
 512  CALL GRADRW(Z(I),Y(I))
      CALL REVERSE(LTR)
C
 501  IF(IDISP.EQ.0) GO TO 502
      CALL LOGIC(LTR,N,2,L1,L2,L3)
      DO 515 I=L1,L2,L3
      CALL GRAMOV(Z(I),DX(I))
 515  CALL GRADRW(Z(I+L3/2),DX(I+L3/2))
      CALL REVERSE(LTR)
C
 502  IF(KDISP.EQ.0) GO TO 503
      CALL LOGIC(LTR,N,2,L1,L2,L3)
      DO 516 I=L1,L2,L3
      CALL GRAMOV(Z(I),DY(I))
 516  CALL GRADRW(Z(I+L3/2),DY(I+L3/2))
      CALL REVERSE(LTR)
C
 503  IF(IRAX.EQ.0) GO TO 504
      CALL LOGIC(LTR,N,1,L1,L2,L3)
      CALL GRAMOV(Z(L1),RAX(L1))
      DO 513 I=L1,L2,L3
 513  CALL GRADRW(Z(I),RAX(I))
      CALL REVERSE(LTR)
C
 504  IF(IRAY.EQ.0) GO TO 505
      CALL LOGIC(LTR,N,1,L1,L2,L3)
      CALL GRAMOV(Z(L1),RAY(L1))
      DO 514 I=L1,L2,L3
 514  CALL GRADRW(Z(I),RAY(I))
      CALL REVERSE(LTR)
 505  CONTINUE
      IF (.NOT.BYPASS) GO TO 579
C
 600  CONTINUE  !!  ENDE VERSIONEN LOOP
C
C   FINISH
C
 100  CALL GRAMOV(0.,0.)
      CALL SET_COL(1)       ! WEISS
      IF (BYPASS) THEN
       GO TO (700,701,702),LFLAG
      ENDIF
 1109 CLOSE(UNIT=0)
      CLOSE(UNIT=7)
      CLOSE(UNIT=8)
      CLOSE(UNIT=9)
      CLOSE(UNIT=10)
      CLOSE(UNIT=14)
C
      OPEN(UNIT=24,FILE='FOR024.DAT',STATUS='UNKNOWN')
      REWIND(UNIT=24)
C
 1110    STR(1:2) = '  '
      CALL ERASER(0.,760.*SCALY,1000.*SCALX,800.*SCALY)
      CALL GETINP(5.,800.*SCALY,'L: Lasercopy, <CR>: Exit '
     1          //char(0),STR)
C
C       get eventual new window dimension and save it on file
C
      call getwindim(nw,nh)
      OPEN(UNIT=49,FILE='.dispoff',STATUS='OLD',ERR=1112)
      READ(49,'(4I6)',ERR=1112) IXOFF,IYOFF,IWIDTH,IHEIGHT
      REWIND(UNIT=49)
      WRITE(49,'(4I6)') IXOFF,IYOFF,nw,nh
      CLOSE(UNIT=49)
C
 1112 CALL ERASER(0.,760.*SCALY,1000.*SCALX,800.*SCALY)
C
 1111 CONTINUE
      IF (STR(1:6).EQ.'MOUSE1') THEN
       READ(STR(7:20),'(2F7.1)') X1, Y1
       MENUT(1) = ' Print with Dev1 (via GS)  P1 '
       MENUT(2) = ' Print with Dev2 (via GS)  P2 '
       MENUT(3) = ' Postscript Laser Copy (bw) L '
       MENUT(4) = ' Colored PS-Laser Copy      C '
       MENUT(5) = ' Edit Input and new Run    NE '
       MENUT(6) = ' I-Editor and new Run      NI '
       MENUT(7) = ' Exit without new Action  <CR>'
       MENUT(8) = ' No Action                    '
       NX       = 30
       NY       = 8
       NR = MENU(X1,Y1,NX,NY,MENUT)
       IF (NR .EQ. 1) STR(1:2) = 'P1'
       IF (NR .EQ. 2) STR(1:2) = 'P2'
       IF (NR .EQ. 3) STR(1:1) = 'L'
       IF (NR .EQ. 4) STR(1:1) = 'C'
       IF (NR .EQ. 5) STR(1:2) = 'NE'
       IF (NR .EQ. 6) STR(1:2) = 'NI'
       IF (NR .EQ. 7) STR(1:1) = ' '
           IF (NR .EQ. 8) GO TO 1110
       GO TO 1111
       ELSE IF (STR(1:6).EQ.'MOUSE3') THEN
       READ(STR(7:20),'(2F7.1)') X1, Y1
         ZZ = ZMIN + (X1/(1000.*SCALX)) * (ZMAX-ZMIN)
         IRET = GETPAR(X1,Y1,ZZ,OPEN,SCALX,SCALY)
         IF (IRET .EQ. 0) THEN
C     OPEN = .FALSE.
          GO TO 1110
         ELSE IF (IRET .EQ. 1) THEN
 703   CLOSE(UNIT=24)
      OPEN = .TRUE.
      IF (COLOR) THEN
       CALL SET_COL(1) ! WEISS
       BYPASS = .TRUE.
       LFLAG  = 3
       GO TO 507
      ENDIF
 702   CALL VT220_POP
      CALL SAVERUN
      CALL SYSTEM('./qoi2')
      CALL SYSTEM('./s')
      ICOUNT = ICOUNT + 1
      CALL SET_COL(2) ! ROT
      BYPASS = .FALSE.
      CALL VT220_PUSH
      NAMEX = 'FOR007.DAT'
      IV = 11
      GO TO 508
      ELSE
      BYPASS = .FALSE.
      GO TO 2
         ENDIF
      ELSE IF (STR(1:6).EQ.'MOUSE2') THEN
           GO TO 1110
      ELSE IF (STR(1:9).EQ.'CONTROL_Z') THEN
       CALL VT220_POP
      ELSE IF (STR(1:2).EQ.'P1' .OR. STR(1:2).EQ.'p1') THEN
       CALL VT220_POP
           WRITE(MULTIST(6:11),'(I4,'';1'')')IVERSNR+ICOUNT
       CALL DISPLAPS(SECORD,IJK,.TRUE.,.FALSE.,.FALSE.)
       CALL SYSTEM('./print dev1 TRANS.PS')
       CALL VT220_PUSH
       GO TO 1109
      ELSE IF (STR(1:2).EQ.'P2' .OR. STR(1:2).EQ.'p2') THEN
       CALL VT220_POP
           WRITE(MULTIST(6:11),'(I4,'';1'')')IVERSNR+ICOUNT
       CALL DISPLAPS(SECORD,IJK,.FALSE.,.FALSE.,.FALSE.)
       CALL SYSTEM('./print dev2 TRANS.PS')
       CALL VT220_PUSH
       GO TO 1109
      ELSE IF (STR(1:2).EQ.'PX' .OR. STR(1:2).EQ.'px') THEN
       CALL VT220_POP
           WRITE(MULTIST(6:11),'(I4,'';1'')')IVERSNR+ICOUNT
       CALL DISPLAPS(SECORD,IJK,.TRUE.,.FALSE.,.TRUE.)
       CALL SYSTEM('./print px TRANS.PS')
       CALL VT220_PUSH
       GO TO 1109
      ELSE IF (STR(1:2).EQ.'PG' .OR. STR(1:2).EQ.'pg') THEN
       CALL VT220_POP
           WRITE(MULTIST(6:11),'(I4,'';1'')')IVERSNR+ICOUNT
       CALL DISPLAPS(SECORD,IJK,.TRUE.,.TRUE.,.FALSE.)
       CALL SYSTEM('./print px TRANS.PS')
       CALL VT220_PUSH
       GO TO 1109
      ELSE IF (STR(1:1).EQ.'L' .OR. STR(1:1).EQ.'l') THEN
       CALL VT220_POP
           WRITE(MULTIST(6:11),'(I4,'';1'')')IVERSNR+ICOUNT
       CALL DISPLAPS(SECORD,IJK,.FALSE.,.FALSE.,.FALSE.)
       CALL SYSTEM('./print bw TRANS.PS')
       CALL VT220_PUSH
       GO TO 1109
      ELSE IF (STR(1:1).EQ.'C' .OR. STR(1:1).EQ.'c') THEN
       CALL VT220_POP
           WRITE(MULTIST(6:11),'(I4,'';1'')')IVERSNR+ICOUNT
       CALL DISPLAPS(SECORD,IJK,.TRUE.,.FALSE.,.FALSE.)
       CALL SYSTEM('./print c TRANS.PS')
       CALL VT220_PUSH
       GO TO 1109
      ELSE IF (STR(1:2).EQ.'NE' .OR. STR(1:2).EQ.'ne') THEN
       IF (COLOR) THEN
        CALL SET_COL(1) ! WEISS
        BYPASS = .TRUE.
        LFLAG  = 1
        GO TO 507
       ENDIF
 700     CONTINUE
         CALL VT220_POP
         CALL SAVERUN
         CALL SYSTEM('./edit')
         CALL SYSTEM('./s')
         CALL SET_COL(2) ! ROT
         BYPASS = .FALSE.
         CALL VT220_PUSH
         NAMEX = 'FOR007.DAT'
         IV = 11
         GO TO 508
        ELSE IF (STR(1:2).EQ.'NI' .OR. STR(1:2).EQ.'ni') THEN
         IF (COLOR) THEN
          CALL SET_COL(1) ! WEISS
          BYPASS = .TRUE.
          LFLAG  = 2
          GO TO 507
         ENDIF
 701  CALL VT220_POP
       CALL SAVERUN
       CALL SYSTEM('./i')
       CALL SYSTEM('./s')
       CALL SET_COL(2) ! ROT
       BYPASS = .FALSE.
       CALL VT220_PUSH
       NAMEX = 'FOR007.DAT'
       IV = 11
       GO TO 508
      ELSE IF (STR(1:2).EQ.'R ' .OR. STR(1:2).EQ.'r ') THEN
           GO TO 703
      ELSE IF (STR(1:2).EQ.'N ' .OR. STR(1:2).EQ.'n ') THEN
       BYPASS = .FALSE.
       GO TO 2
      ELSE
       CALL VT220_POP
      ENDIF
C
C     FORMAT STATEMENTS
C
 1003 FORMAT(F10.1,6X,4A1,2X,7A1)
 1013 FORMAT(F10.1,6X,4A1,2X,9A1)
 1004 FORMAT(A)    
 1005 FORMAT(I10)
 1007 FORMAT(6X,F6.2,9X,F6.2)
C5000 FORMAT(I)
 5001 FORMAT(I1)
 5002 FORMAT(I2)
 5003 FORMAT(I3)
 5004 FORMAT(I4)
      END

      SUBROUTINE REVERSE(LTR)
      LOGICAL LTR
      IF (LTR) THEN 
       LTR=.FALSE.
      ELSE
       LTR=.TRUE.
      ENDIF
      RETURN
      END
  
      SUBROUTINE LOGIC(LTR,NN,INC,I1,I2,I3)
C
C     SUBROUTINE SETS INDICES FOR DO-LOOP ACCORDING TO DIRECTION
C
      LOGICAL LTR
      IF(LTR) THEN
       I1=1
       I2=NN+1-INC
       I3=INC
      ELSE
       I1=NN
       I2=INC
       I3=-INC
      ENDIF
      RETURN
      END

      INTEGER FUNCTION GETPAR(X1,Y1,ZZ,OPEN,SCALX,SCALY)
C
      CHARACTER*10 TIT
      CHARACTER*4  LABI
      PARAMETER    (LDM=1000)
      character*30 MENUT(8)
      CHARACTER*80 STR
          CHARACTER*20 INPUT
      CHARACTER*37 MENU5
      CHARACTER*10 TITEL
      CHARACTER*2  DIM1,DIM2,DIM3,DIM4
      CHARACTER*4  LAB,DIM5,DIM6
          REAL         LENG
      LOGICAL      OPEN, FIRST
          INTEGER      KSAVE
C
      COMMON /ELDATA/
     *  Z(LDM),XMAX(LDM),YMAX(LDM),D(LDM),DPY(LDM),ANG(LDM),
     *  DY(LDM),LENG(LDM),AP(LDM),B(LDM),PXMAX(LDM),R12(LDM),
     *  PYMAX(LDM),DP(LDM),R34(LDM),X0(LDM),Y0(LDM),X0P(LDM),
     *  Y0P(LDM),CX(LDM),SX(LDM),CPX(LDM),SPX(LDM),CY(LDM),
     *  SY(LDM),CPY(LDM),SPY(LDM),XN(LDM),TITEL(LDM),LAB(LDM)
C
      COMMON /QDATA/ QTYP(100), QLAB(100), LEFF(100), JA(100),
     *                 A0(100), A1(100), A2(100), A3(100),
     *                 J0(100), JMAX(100), DMAX(100)
      REAL J0, JMAX
      CHARACTER*4 QTYP, QLAB

      DATA FIRST /.TRUE./
C
C     TEILWEISES LESEN VON FILE 15
C
      IF (OPEN) THEN
       OPEN(UNIT=15,FILE='FOR015.DAT',STATUS='OLD',
     1        FORM='UNFORMATTED')
       READ(15) DIM1,DIM2,DIM3,DIM4,DIM5,DIM6,P,DELP,INDMAX,
     1            IND,IN,IS,ICH,CHI,IPOINT,SAVE1,SAVE2,SAVE3
       READ(15) (TITEL(I),LAB(I),Z(I),XMAX(I),YMAX(I),D(I),
     1       DY(I),LENG(I),AP(I),B(I),PXMAX(I),R12(I),DPY(I),
     2       PYMAX(I),DP(I),R34(I),X0(I),Y0(I),X0P(I),Y0P(I),
     3       CX(I),SX(I),CPX(I),SPX(I),CY(I),SY(I),CPY(I),SPY(I),
     4       XN(I),ANG(I),I=1,INDMAX)
       IF (DIM1 .EQ. 'M ') DIM1 = 'm '
       IF (DIM2 .EQ. 'MM') DIM2 = 'mm'
       IF (DIM2 .EQ. 'CM') DIM2 = 'cm'
       CLOSE(UNIT=15)
      ENDIF
C
C     QUAD CALIBRATION
C
      IF (FIRST) THEN
       FIRST = .FALSE.
       OPEN(UNIT=50,FILE='QUADS.CAL',STATUS='OLD')
       K = 1
 4    CONTINUE
      IF (K.GT.100) STOP 'FILE QUADS.CAL OVERFLOW'
      READ(50,2077,END=5)
     1   QTYP(K),QLAB(K),LEFF(K),JA(K),A0(K),A1(K),A2(K),A3(K),
     2   J0(K),JMAX(K),DMAX(K)
 2077 FORMAT(1X,A4,2X,A4,2X,I4,2X,I4,7F7.1)
      K = K + 1
      GO TO 4
 5    NREC = K - 1
      CLOSE(UNIT=50)
      ENDIF
C
      DO 100 J = 1, INDMAX
       IF (Z(J) .GT. ZZ) THEN
          TIT  = TITEL(J)
          LABI = LAB(J)
          XL   = LENG(J)
      IXL  = NINT(XL * 1000. * SCALX)
      IXLR = NINT(XL * 1000.)
      BF   = B(J)
          AF   = AP(J)
      IAP  = NINT(AF * 10.)
          IDAC = NINT(1000. * BF)
      DO 50 K = 1, NREC
        IF ( (QLAB(K) .EQ. LABI) .OR.
     1       (QLAB(K) .EQ.'%%%%' .AND. 
     2        IAP .EQ. JA(K) .AND. IXLR .EQ. LEFF(K)) ) THEN
              CALL GETDAC(BF,IDAC,AMPS,A0(K),A1(K),A2(K),A3(K),J0(K),
     1                   JMAX(K),DMAX(K))
              KSAVE = K
          GO TO 300
        ENDIF
 50    CONTINUE
       GO TO 300
       ENDIF
 100  CONTINUE
C
      TIT  = '**********'
      LABI = '****'
      XL   = 0.0
      BF   = 0.0
      AF   = 0.0
      IDAC = 0
C
 300  MENUT(1) = '                              '
      MENUT(2) = '                              '
      MENUT(3) = '                              '
      MENUT(4) = '                              '
      MENUT(5) = '                              '
      MENUT(6) = '                              '
      MENUT(7) = ' New Run                    R '
      MENUT(8) = ' New  Reference             N '
      WRITE(MENUT(1)(1:14),1234) ZZ
 1234    FORMAT(' Z =',F8.3,' m')
      WRITE(MENUT(2)(1:11),1235) TIT
 1235    FORMAT(' ',A10)
      WRITE(MENUT(3)(1:5),1236) LABI
 1236    FORMAT(' ',A4)
      WRITE(MENUT(4)(1:16),1237) XL
 1237    FORMAT(' leff =',F7.4,' m')
      WRITE(MENUT(5)(1:14),1238) IDAC
 1238    FORMAT(' B =',I6,' DAC')
      WRITE(MENUT(6)(1:14),1239) AF
 1239    FORMAT(' a =',F7.2,' mm')
C
      NX = 30
      NY = 8
      NR = MENU(X1,Y1,NX,NY,MENUT)
      GETPAR = 0
          IF (NR .EQ. 7) THEN
       GETPAR = 1
          ELSE IF (NR .EQ. 8) THEN
       GETPAR = 2
      ELSE IF (NR .EQ. 5) THEN
           IF (MENUT(2)(2:7) .EQ. '*QUAD*') THEN
        CALL ERASER(0.,760.*SCALY,1000.*SCALX,800.*SCALY)
        INPUT(1:20) = '                    '
            MENU5 = MENUT(5)(1:14)//', Enter new DAC value: '
        CALL GETINP(5.,800.*SCALY,MENU5//char(0),INPUT)
        CALL ERASER(0.,760.*SCALY,1000.*SCALX,800.*SCALY)
            STR = INPUT
            IF (STR(1:1) .NE. ' ') THEN
         IF (STR(2:2) .EQ. ' ') THEN
              READ(STR(1:1),'(I1)') IDACNEW
         ELSE IF (STR(3:3) .EQ. ' ') THEN
              READ(STR(1:2),'(I2)') IDACNEW
         ELSE IF (STR(4:4) .EQ. ' ') THEN
              READ(STR(1:3),'(I3)') IDACNEW
         ELSE IF (STR(5:5) .EQ. ' ') THEN
              READ(STR(1:4),'(I4)') IDACNEW
         ELSE IF (STR(6:6) .EQ. ' ') THEN
              READ(STR(1:5),'(I5)') IDACNEW
         ELSE
              READ(STR(1:6),'(I6)',ERR=777) IDACNEW
         ENDIF
         K = KSAVE
         BNEW = GETFIELD(IDACNEW,A0(K),A1(K),A2(K),A3(K),
     1                     J0(K),JMAX(K),DMAX(K))
           WRITE(24,5000)  XL, BNEW, AF, LABI
 5000      FORMAT(' 5.',F7.4,F8.3,F7.2,' /',A4,'/ ;')
      ENDIF
      ENDIF
      ENDIF
C
 777  CONTINUE
      RETURN
      END

      SUBROUTINE SAVERUN
C
      CHARACTER*26 COMMAND
      CHARACTER*12 FILEN
      LOGICAL      ANS
C
C     ENVELOPES
C
      INQUIRE(FILE = 'FOR007.DAT', EXIST = ANS)
      IF (ANS) THEN
       DO 10 J = 1, 10
        FILEN = 'FOR007.DAT.0'
        FILEN(12:12) = CHAR(48+J-1)
        INQUIRE(FILE = FILEN, EXIST = ANS)
        IF (.NOT.ANS) THEN
         COMMAND(1:14) = 'cp FOR007.DAT '
         COMMAND(15:26) = FILEN
         CALL SYSTEM(COMMAND)
         GO TO 15
        ENDIF
 10    CONTINUE
      ENDIF
C
C       SECOND ORDER
C
 15   CONTINUE
      INQUIRE(FILE = 'FOR000.DAT', EXIST = ANS)
      IF (ANS) THEN
       DO 20 J = 1, 10
        FILEN = 'FOR000.DAT.0'
        FILEN(12:12) = CHAR(48+J-1)
        INQUIRE(FILE = FILEN, EXIST = ANS)
        IF (.NOT.ANS) THEN
         COMMAND(1:14) = 'cp FOR000.DAT '
         COMMAND(15:26) = FILEN
         CALL SYSTEM(COMMAND)
         GO TO 25
        ENDIF
 20    CONTINUE
      ENDIF
C
 25   CONTINUE
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
      IF (AMPS.GT.J0 .AND. JMAX.GT.J0) THEN     ! NON-LINEAR CONTRIB?
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
 20   CONTINUE
 30   CONTINUE
      ENDIF
      AMPS = SIG * SIG2 * AMPS
      IDAC = NINT(AMPS*ABS(DMAX)/JMAX)
      RETURN
      END

      REAL FUNCTION GETFIELD(IDAC,A0,A1,A2,A3,J0,JMAX,DMAX)
C
      REAL J0, JMAX
C
      AMPS = (ABS(IDAC) / ABS(DMAX)) * JMAX
      B    = A0 + (AMPS / JMAX) * A1
      IF (AMPS .GT. J0 .AND. AMPS .LE. JMAX) THEN
        X = (AMPS - J0) / (JMAX - J0)
        B = B + (A2 + A3 * X) * X * X
      ENDIF
C
      SIG = 1.0
      IF (IDAC .LT. 0) SIG = -1.0
      SIG2 = 1.0
      IF (DMAX .LT. 0) SIG2 = -1.0
      B = B * SIG * SIG2
      GETFIELD = B
C
      RETURN
      END

