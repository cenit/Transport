        PROGRAM DISPO2
C
C   REVISED DISPO FOR ANSI TERMINALS
C   REVISED FOR VAX-STATIONS BY U.ROHRER (PSI), NOV.1990
C
        DIMENSION TEXT(13),NAME(19),PAR(19),ITEM(19),QAMP(100),AM(100)
        DIMENSION ID(100),LAB(100),LEFF(100),JA(100),DMAX(100)
        DIMENSION A0(100),A1(100),A2(100),A3(100)
        REAL      J0(100),JMAX(100)
        CHARACTER*1  GAF, GA(100)
        CHARACTER*2  KGF, KG(100)
        CHARACTER*20 INPUT
        CHARACTER*10 TITEL,KEY,NAME,TEMP
        CHARACTER*12 TITELS
        CHARACTER*4 LABI,LABEL,LAB,DIM5,DIM6,QTYP(100),QLAB(100),SAVE4
        CHARACTER*5 ITEM
        CHARACTER*2 DIM1,DIM2,DIM3
        CHARACTER*79 UEBER
        CHARACTER*100 LINE
        DIMENSION LABV(1000),IREM(50),FAIL(50)
        LOGICAL FAIL
        CHARACTER*4 TEXT
        INTEGER*1 ESC
        LOGICAL PRINT,FIRST,FIRST2,FILE
C
        CHARACTER*2  DM1,DM2,DM3,DM4
        CHARACTER*4  DM5,DM6
        CHARACTER*4  LB(2000),LF(200)
        CHARACTER*10 TT(2000),LS(100)
        DIMENSION T1(2000),T2(2000),T3(2000),T4(2000),T5(2000)
        DIMENSION U1(2000),U2(2000),U3(2000),U4(2000)
        DIMENSION U5(2000),U6(2000),U7(2000),U8(2000)
        DIMENSION V1(2000),V2(2000),V3(2000),V4(2000),V5(2000)
        DIMENSION V6(2000),V7(2000),V8(2000),V9(2000)
        DIMENSION W1(2000),W2(2000),W3(2000),W4(2000)
        DIMENSION W5(2000),W6(2000),W7(2000)
        DIMENSION S1(200),S2(200),S3(200),S4(200),S5(200),S6(200)
C
        DATA TEXT/'z   ','x   ','x''  ','r12 ','y   ','y''  ',  
     1  'r34 ','R16 ','R26 ','p   ','dp/p','epsx','epsy'/
C
        DATA ITEM/'ZMIN ','ZMAX ','XMPL ','YMPL ','APFAC'
     1  ,'SPEZ ','DISP ','YDISP','RAX  ','RAY  ','MEASX'
     2  ,'MEASY','MEASD','CUMAX','SPEZ2','CSX  ','CSY  '
     3  ,'YTIT ','STEER'/

        PRINT = .FALSE.
        FILE  = .FALSE.
        FIRST = .TRUE.
        ESC   = 27
        IO    = 6
C
C   GET TITLE OF TRANSPORT_RUN
C
        OPEN(UNIT=9,FILE='FOR009.DAT',STATUS='OLD',ERR=777)
        READ(9,3010,ERR=777) UEBER
        READ(9,3010) UEBER
        DO 123 I = 79,1,-1
         IF(UEBER(I:I) .NE. ' ') THEN
          NLEN = I
          GO TO 124
         ENDIF
  123   CONTINUE
  124   CONTINUE
C
C   FIND USER CHOICE
C
        WRITE(6,3013) ESC,ESC
 1111   WRITE(6,3011) ESC,ESC,UEBER(1:NLEN),ESC
        WRITE(6,1015) ESC,ESC,ESC
    1   WRITE(6,3014) ESC,ESC
        READ(5,1000,END=999) KEY
        CALL TOUPPER(KEY)
    2   IF (KEY.EQ.'BEAM' .OR. KEY.EQ.'BE') GO TO 100
        IF (KEY.EQ.'QDAC' .OR. KEY(1:2).EQ.'QD') THEN
         KEY = 'QDAC'
         GO TO 200
        ENDIF
        IF (KEY.EQ.'QAMP' .OR. KEY(1:2).EQ.'QA') GO TO 200
        IF (KEY.EQ.'QUADS' .OR. KEY.EQ.'Q') GO TO 200
        IF (KEY.EQ.'BENDS' .OR. KEY.EQ.'B') GO TO 250
        IF (KEY.EQ.'BEAMLINE' .OR. KEY.EQ.'BL') GO TO 300
        IF (KEY.EQ.'FITS' .OR. KEY.EQ.'F') GO TO 400
        IF (KEY.EQ.'DISPLAY' .OR. KEY.EQ.'D') GO TO 500
        IF (KEY.EQ.'SHIFTS' .OR. KEY.EQ.'S') GO TO 600
        IF (KEY.EQ.'PRINT' .OR. KEY.EQ.'P') GO TO 50
        IF (KEY.EQ.'SCREEN' .OR. KEY.EQ.'SC') GO TO 60
        IF (KEY.EQ.'FILE' .OR. KEY.EQ.'FI') GO TO 70
        IF (KEY.EQ.'COOR' .OR. KEY.EQ.'C') GO TO 80
        IF (KEY.EQ.' ') GO TO 999
        WRITE(6,1001) ESC,ESC,ESC
        GO TO 1
C
   50   PRINT = .TRUE.
        FILE  = .FALSE.
        WRITE(6,5556) ESC,ESC
        PRINT *,'requested transport data will be queued for printing'
        OPEN(UNIT=7,FILE='OUTPUT.DAT',STATUS='UNKNOWN')
        REWIND(UNIT=7)
        WRITE(7,3012) UEBER
        IO = 7
        GO TO 1
C
   60   IF (PRINT) THEN
         CLOSE(UNIT=7)
         CALL SYSTEM('./print text OUTPUT.DAT')
        ENDIF
        PRINT = .FALSE.
        FILE  = .FALSE.
        WRITE(6,5556) ESC,ESC
        PRINT *, 'requested transport data will appear on this screen.'
        IO = 6
        GO TO 1
C
   70   IF (PRINT) THEN
         CLOSE(UNIT=7)
         CALL SYSTEM('./print text OUTPUT.DAT')
        ENDIF
        FILE  = .TRUE.
        PRINT = .FALSE.
        WRITE(6,5556) ESC,ESC
        PRINT *, 'requested transport data will appear as a data_file.'
        IO = 7
        GO TO 1
C
C       FLOOR COORDINATES LAYOUT
C
   80   IF (FILE) THEN
         OPEN(UNIT=7,FILE='COORDS.DAT',STATUS='UNKNOWN')
         WRITE(7,3012) UEBER
         WRITE(7,1144)
        ELSE IF (PRINT) THEN
         WRITE(7,1144)
        ELSE
         WRITE(6,1044) ESC
        ENDIF
        I=0
        OPEN(UNIT=29,FILE='FOR029.DAT',STATUS='OLD')
   85   READ(29,1234,END=96,ERR=95) LINE
        WRITE(IO,1234) LINE
        I = I + 1
        IF (.NOT.PRINT .AND. .NOT.FILE .AND. 19*(I/19)-I .EQ. 0) THEN
         WRITE(6,1235)
         READ(5,1234,END=96) INPUT 
        ENDIF
        GO TO 85
   96   REWIND 29
        GO TO 98

   95   WRITE(IO,1020)
        GO TO 999

   98   CLOSE(UNIT=29)
        IF (FILE) THEN
         CLOSE(UNIT=7)
         PRINT *, 'COORDS.DAT data_file generated.                     '
        ENDIF
        IF (PRINT) THEN
         WRITE(6,3016) ESC
         PRINT *, 'COORDS.DAT appended on print_file OUTPUT.DAT.'
            ENDIF
        IF (I .EQ. 0) THEN
         WRITE(IO,1020)
         GO TO 999
        ELSE
         GO TO 1
        ENDIF
C
C   BEAM
C
  100   IF (FILE) THEN
         OPEN(UNIT=7,FILE='BEAM.DAT',STATUS='UNKNOWN')
         WRITE(7,3012) UEBER
         WRITE(7,1104)
        ELSE IF (PRINT) THEN
         WRITE(7,1104)
        ELSE
         WRITE(6,1004) ESC
        ENDIF
C
        OPEN(UNIT=12,FILE='FOR012.DAT',STATUS='OLD',
     1       FORM='UNFORMATTED')
        READ(12,ERR=110) Z,X,PX,R12,Y,PY,R34,D,DP,P,DELP
     1                   ,DIM1,DIM2,DIM3,DIM5
        EPSX = X * PX * SQRT(ABS(1.-R12**2))
        EPSY = Y * PY * SQRT(ABS(1.-R34**2))
        GO TO 111
  110   WRITE(IO,1020)
        GO TO 999
  111   WRITE(IO,2006) TEXT(1),Z,DIM1
        WRITE(IO,2006) TEXT(2),X,DIM2
        WRITE(IO,2007) TEXT(3),PX
        WRITE(IO,2008) TEXT(4),R12
        WRITE(IO,2005) TEXT(12),EPSX,DIM2
        WRITE(IO,2006) TEXT(5),Y,DIM2
        WRITE(IO,2007) TEXT(6),PY
        WRITE(IO,2008) TEXT(7),R34
        WRITE(IO,2005) TEXT(13),EPSY,DIM2
        WRITE(IO,2012) TEXT(8),D,DIM2,DIM3
        WRITE(IO,2013) TEXT(9),DP,DIM3
        WRITE(IO,2009) TEXT(10),P,DIM5(1:3)
        WRITE(IO,2006) TEXT(11),DELP,DIM3
        CLOSE(UNIT=12)
        REWIND 12
C
        IF (FILE) THEN
         CLOSE(UNIT=7)
         PRINT *, 'BEAM.DAT data_file generated.                       '
        ENDIF
        IF (PRINT) THEN
         WRITE(6,3016) ESC
         PRINT *, 'BEAM.DAT appended on print_file OUTPUT.DAT.'
        ENDIF
        GO TO 1
C
C   QUADS
C
  200   IF (FIRST) THEN
         FIRST = .FALSE.
         OPEN(UNIT=50,FILE='QUADS.CAL',STATUS='OLD')
         K = 1
    5    CONTINUE
         IF (K.GT.100) STOP 'FILE QUADS.CAL OVERFLOW'
         READ(50,2077,END=6)
     1   QTYP(K),QLAB(K),LEFF(K),JA(K),A0(K),A1(K),A2(K),A3(K),
     2   J0(K),JMAX(K),DMAX(K)
         K = K + 1
         GO TO 5
    6    NREC = K - 1
         CLOSE(UNIT=50)
        END IF
C
        K     = 0
        I     = 0
        SAVE1 = 0.
        SAVE2 = 0.
        ISAV3 = 0.
        SAVE4 = '   '
        IF(KEY.EQ.'QUADS' .OR. KEY.EQ.'Q') THEN
         IF (FILE) THEN
          OPEN(UNIT=7,FILE='QUADS.DAT',STATUS='UNKNOWN')
          WRITE(7,3012) UEBER
          WRITE(7,1105)
         ELSE IF (PRINT) THEN
          WRITE(7,1105)
         ELSE
          WRITE(6,1005) ESC
         ENDIF
        ENDIF
C
        OPEN(UNIT=11,FILE='FOR011.DAT',STATUS='OLD',
     1       FORM='UNFORMATTED')
  201   READ(11,END=203,ERR=203) TITEL,LABEL,XL,DIM1,
     1                           AP,DIM2,B,DIM3,XN
        IF(TITEL.NE.'*QUAD*' .AND. TITEL.NE.'*SOLO*'
     1                       .AND. TITEL.NE.'*SEXT*'
     2                       .AND. TITEL.NE.'*EQUAD*') GO TO 201
        I = I + 1
        IF(TITEL.EQ.'*SOLO*' .OR. TITEL.EQ.'*SEXT*') GO TO 202
        IXL = NINT(XL*1000.)
        IAP = NINT(AP*10.)
        XA0 = 0.0
        XA1 = 1.0
        XA2 = 0.0
        XA3 = 0.0
        XJ0 = 1000.0
        XJMAX = 1.0
        XDMAX = 1000.0
        GAF   = 'G'
        KGF   = 'KG'
        DO 205 J = 1, NREC
        IF (LABEL.EQ.QLAB(J)) THEN
         XA0 = A0(J)
         XA1 = A1(J)
         XA2 = A2(J)
         XA3 = A3(J)
         XJ0 = J0(J)
         XJMAX = JMAX(J)
         XDMAX = DMAX(J)
         GAF   = ' '
         KGF   = 'A '
         GO TO 204
        ENDIF
        IF(IAP.EQ.JA(J) .AND. IXL.EQ.LEFF(J) .AND. QLAB(J).EQ.'%%%%')
     1  THEN
         XA0 = A0(J)
         XA1 = A1(J)
         XA2 = A2(J)
         XA3 = A3(J)
         XJ0 = J0(J)
         XJMAX = JMAX(J)
         XDMAX = DMAX(J)
         GAF   = ' '
         KGF   = 'A '
         GO TO 204
        ENDIF
  205   CONTINUE
  204   IF (KEY.EQ.'QUADS' .OR. KEY.EQ.'Q') THEN
         IF (TITEL .EQ. '*EQUAD*') THEN
           WRITE(IO,11018) TITEL,LABEL,XL,DIM1,AP,DIM2,B
         ELSE
           WRITE(IO,1018) TITEL,LABEL,XL,DIM1,AP,DIM2,B,DIM3
             ENDIF
         IF (.NOT.PRINT .AND. .NOT.FILE .AND. 19*(I/19)-I .EQ. 0) THEN
          WRITE(6,1235)
          READ(5,1234,END=203) INPUT 
         ENDIF
        ENDIF
        CALL GETDAC(B,IDAC,AMPS,XA0,XA1,XA2,XA3,XJ0,XJMAX,XDMAX)

        IF (AP   .NE. SAVE1 .OR.
     1      XL   .NE. SAVE2 .OR.
     2      IDAC .NE. ISAV3 .OR.
     3      SAVE4.NE. LABEL) K = K + 1
        ID(K)  = IDAC
        GA(K)  = GAF
        AM(K)  = AMPS
        KG(K)  = KGF
        LAB(K) = LABEL
        SAVE1  = AP
        SAVE2  = XL
        ISAV3  = IDAC
        SAVE4  = LABEL
        GO TO 201
C
  202   IF (KEY.EQ.'QUADS' .OR. KEY.EQ.'Q') THEN
         WRITE(IO,1006) TITEL,LABEL,XL,DIM1,AP,DIM2,B,DIM3
         IF (.NOT.PRINT .AND. .NOT.FILE .AND. 19*(I/19)-I .EQ. 0) THEN
          WRITE(6,1235)
          READ(5,1234,END=203) INPUT 
         ENDIF
        ENDIF
        GO TO 201
C
  203   IF(KEY.EQ.'QUADS' .OR. KEY.EQ.'Q') THEN
         IF(I.EQ.0) WRITE(IO,1020)
        ENDIF
        REWIND 11
C
        IF (KEY.EQ.'QUADS' .OR. KEY.EQ.'Q') THEN
         IF (FILE) THEN
          CLOSE(UNIT=7)
          PRINT *, 'QUADS.DAT data_file generated.                     '
         ENDIF
         IF (PRINT) THEN
          WRITE(6,3016) ESC
          PRINT *, 'QUADS.DAT appended on print_file OUTPUT.DAT.'
         ENDIF
        ENDIF
C
        IF (K .EQ. 0) GO TO 1
        IF (KEY.EQ.'QUADS' .OR. KEY.EQ.'Q') THEN
         WRITE(6,1030) ESC,ESC,ESC
         WRITE(6,3014) ESC,ESC
         READ(5,1000,END=1111) KEY
         CALL TOUPPER(KEY)
        ENDIF
        IF (KEY.EQ.'QDAC' .OR. KEY.EQ.'Q') GO TO 220
        IF (KEY.EQ.'QAMP' .OR. KEY.EQ.'QA') GO TO 230
        GO TO 1111
C
C   QUAD DAC
C
  220   IF (FILE) THEN
         OPEN(UNIT=7,FILE='QDAC.DAT',STATUS='UNKNOWN')
         WRITE(7,3012) UEBER
         WRITE(7,1131)
        ELSE IF (PRINT) THEN
         WRITE(7,1131)
        ELSE
         WRITE(6,1031) ESC
        ENDIF
C
        DO 221 I = 1, K
         WRITE(IO,1040) LAB(I),ID(I), GA(I)
         IF (.NOT.PRINT .AND. .NOT.FILE .AND. 19*(I/19)-I .EQ. 0) THEN
          WRITE(6,1235)
          READ(5,1234,END=1111) INPUT 
         ENDIF
  221   CONTINUE
C
        IF (FILE) THEN
         CLOSE(UNIT=7)
         PRINT *, 'QDAC.DAT data_file generated.                       '
        ENDIF
        IF (PRINT) THEN
         WRITE(6,3016) ESC
         PRINT *, 'QDAC.DAT appended on print_file OUTPUT.DAT.'
        ENDIF
        GO TO 1111
C
C   QUAD AMPS
C
  230   IF (FILE) THEN
         OPEN(UNIT=7,FILE='QAMP.DAT',STATUS='UNKNOWN')
         WRITE(7,3012) UEBER
         WRITE(7,1128)
        ELSE IF (PRINT) THEN
         WRITE(7,1128)
        ELSE
         WRITE(6,1028) ESC
        ENDIF
C
        DO 231 I = 1, K
         WRITE(IO,1041) LAB(I),AM(I), KG(I)
         IF (.NOT.PRINT .AND. .NOT.FILE .AND. 19*(I/19)-I .EQ. 0) THEN
          WRITE(6,1235)
          READ(5,1234,END=1111) INPUT 
         ENDIF
  231   CONTINUE
C
        IF (FILE) THEN
         CLOSE(UNIT=7)
         PRINT *, 'QAMP.DAT data_file generated.                       '
        ENDIF
        IF (PRINT) THEN
         WRITE(6,3016) ESC
         PRINT *, 'QAMP.DAT appended on print_file OUTPUT.DAT.'
        ENDIF
        GO TO 1111
C
C   BENDS
C
  250   I = 0
        IF (FILE) THEN
         OPEN(UNIT=7,FILE='BENDS.DAT',STATUS='UNKNOWN')
         WRITE(7,3012) UEBER
         WRITE(7,3105)
        ELSE IF (PRINT) THEN
         WRITE(7,3105)
        ELSE
         WRITE(6,3005) ESC
        ENDIF
C
        OPEN(UNIT=11,FILE='FOR011.DAT',STATUS='OLD',
     1       FORM='UNFORMATTED')
  251   READ(11,END=253,ERR=253)
     1  TITEL,LABEL,XL,DIM1,AP,DIM2,B,DIM3,XN,ANG,DIM6
        IF (TITEL.NE.'*BEND*' .AND. TITEL.NE.'*ROTAT*' .AND.
     1      TITEL.NE.'*E-PRISM*') GO TO 251
        I = I + 1
        IF(TITEL.EQ.'*BEND*')
     1  WRITE(IO,4006) TITEL,LABEL,XL,DIM1,AP,DIM2,B,DIM3,XN,ANG,DIM6
        IF(TITEL.EQ.'*E-PRISM*')
     1  WRITE(IO,4007) TITEL,LABEL,XL,DIM1,AP,DIM2,B,XN,ANG,DIM6
        IF(TITEL.EQ.'*ROTAT*')
     1  WRITE(IO,3006) TITEL,LABEL,XL,DIM6,AP,B,XN
        IF (.NOT.PRINT .AND. .NOT.FILE .AND. 19*(I/19)-I .EQ. 0) THEN
         WRITE(6,1235)
         READ(5,1234,END=253) INPUT 
        ENDIF
        GO TO 251
  253   IF (I .EQ. 0) WRITE(IO,1020)
        REWIND 11
C
        IF (FILE) THEN
         CLOSE(UNIT=7)
         PRINT *, 'BENDS.DAT data_file generated.                      '
        ENDIF
        IF (PRINT) THEN
         WRITE(6,3016) ESC
         PRINT *, 'BENDS.DAT appended on print_file OUTPUT.DAT.'
        ENDIF
        GO TO 1
C
C   BEAMLINE
C
  300   IF (FILE) THEN
         OPEN(UNIT=7,FILE='BEAMLINE.DAT',STATUS='UNKNOWN')
         WRITE(7,3012) UEBER
         WRITE(7,1107)
        ELSE IF (PRINT) THEN
         WRITE(7,1107)
        ELSE
         WRITE(6,1007) ESC
        ENDIF
C
        I = 0
        OPEN(UNIT=3,FILE='FOR003.DAT',STATUS='OLD',
     1       FORM='UNFORMATTED')
  301   READ(3,END=303,ERR=303) TITEL,LABEL,Z,DIM1,X,DIM2,Y,DIM2,
     1                          DX,DIM2,DIM3,DY,DIM2,DIM3
        WRITE(IO,1008) TITEL,LABEL,Z,DIM1,X,DIM2,Y,DIM2,DX,DIM2,
     1                 DIM3,DY,DIM2,DIM3
        I = I + 1
        IF (.NOT.PRINT .AND. .NOT.FILE .AND. 19*(I/19)-I .EQ. 0) THEN
         WRITE(6,1235)
         READ(5,1234,END=303) INPUT 
        ENDIF
        GO TO 301
  303   REWIND 3
C
        IF (FILE) THEN
         CLOSE(UNIT=7)
         PRINT *, 'BEAMLINE.DAT data_file generated.                   '
        ENDIF
        IF (PRINT) THEN
         WRITE(6,3016) ESC
         PRINT *, 'BEAMLINE.DAT appended on print_file OUTPUT.DAT.'
        ENDIF
C
        IF (I .EQ. 0) THEN
         WRITE(IO,1020)
         GO TO 999
        ELSE
         GO TO 1
        ENDIF
C
C   FITS
C
  400   IF (FILE) THEN
         OPEN(UNIT=7,FILE='FITS.DAT',STATUS='UNKNOWN')
         WRITE(7,3012) UEBER
         WRITE(7,1109)
        ELSE IF (PRINT) THEN
         WRITE(7,1109)
        ELSE
         WRITE(6,1009) ESC
        ENDIF
C
        FIRST2 = .TRUE.
        I      = 0
        IND1   = 1
        MAL    = 0
        OPEN(UNIT=13,FILE='FOR013.DAT',STATUS='OLD',
     1       FORM='UNFORMATTED')
  401   READ(13,END=403,ERR=402) LABEL,ZZ,X,Y,Z,DX,DY
        I = I + 1
        IF (LABEL .EQ. 'NOFI') GOTO 402
        IF (LABEL .EQ. 'CHI2') GOTO 1402
        WRITE(IO,1010) LABEL,Z,DX,DY,ZZ,X,Y
        IF (.NOT.PRINT .AND. .NOT.FILE .AND. 19*(I/19)-I .EQ. 0) THEN
         WRITE(6,1235)
         READ(5,1234,END=403) INPUT 
        ENDIF
        GO TO 401
C
  402   WRITE(IO,1020)
        IF (.NOT.PRINT .AND. .NOT.FILE .AND. 19*(I/19)-I .EQ. 0) THEN
         WRITE(6,1235)
         READ(5,1234,END=403) INPUT 
        ENDIF
  403   REWIND 13
C
        IF (FILE) THEN
         CLOSE(UNIT=7)
         PRINT *, 'FITS.DAT data_file generated.                       '
        ENDIF
        IF (PRINT) THEN
         WRITE(6,3016) ESC
         PRINT *, 'FITS.DAT appended on print_file OUTPUT.DAT.'
        ENDIF
        GO TO 1
C
C   ZUSAETZLICHE INFORMATION UEBER FIT AM SCHLUSS VON FOR015
C
 1402   MAL = MAL + 1
        IF (MAL .GT. 50) GO TO 401
        IF (FIRST2) THEN
         OPEN(UNIT=15,FILE='FOR015.DAT',STATUS='OLD',
     1        FORM='UNFORMATTED')
         READ(15) DM1,DM2,DM3,DM4,DM5,DM6,P,DELP,
     1            INDMAX,IND,IN,IS,ICH,CHI,IPOINT,SAV1,SAV2,SAV3
         READ(15) (TT(I),LB(I),T1(I),T2(I),T4(I),T5(I),
     1      U1(I),U2(I),U3(I),U4(I),U5(I),U6(I),U7(I),U8(I),
     2      V1(I),V2(I),V3(I),V4(I),V5(I),V6(I),V7(I),V8(I),V9(I),
     3      W1(I),W2(I),W3(I),W4(I),W5(I),W6(I),W7(I),I=1,INDMAX)
         READ(15) (LF(I),S1(I),S2(I),S3(I),S4(I),S5(I),S6(I),I=1,IN)
         READ(15) (T1(I),T2(I),T3(I),T4(I),LS(I),I=1,IS)
         READ(15) IMAX,ICH
         READ(15) (LABV(J),J=1,IMAX),(IREM(K),K=1,ICH),
     1            (FAIL(L),L=1,ICH)
         CLOSE(UNIT=15)
         FIRST2 = .FALSE.
        END IF
C
        IF (FAIL(MAL)) THEN
         WRITE(IO,2011) ZZ
        ELSE
         WRITE(IO,2010) ZZ
        ENDIF
        IF (.NOT.PRINT .AND. .NOT.FILE .AND. 19*(I/19)-I .EQ. 0) THEN
         WRITE(6,1235)
         READ(5,1234,END=403) INPUT 
        ENDIF
        IND2 = IREM(MAL)
        WRITE(IO,2014) (LABV(J),J=IND1,IND2)
        I = I + 1
        IF (.NOT.PRINT .AND. .NOT.FILE .AND. 19*(I/19)-I .EQ. 0) THEN
         WRITE(6,1235)
         READ(5,1234,END=403) INPUT 
        ENDIF
        IND1 = IND2 + 1
        GO TO 401
C
C   SHIFTS
C
  600   IF (FILE) THEN
         OPEN(UNIT=7,FILE='SHIFTS.DAT',STATUS='UNKNOWN')
         WRITE(7,3012) UEBER
         WRITE(7,1150)
        ELSE IF (PRINT) THEN
         WRITE(7,1150)
        ELSE
         WRITE(6,1050) ESC
        ENDIF
C
        I=1
  605   OPEN(UNIT=18,FILE='FOR018.DAT',STATUS='OLD',
     1       FORM='UNFORMATTED')
        READ(18,ERR=630,END=610) TITEL,LABI,X,DIM2,XP,Y,DIM2,YP,Z
        IF(TITEL.EQ.'*AXIS SHI*') TITELS='*AXIS SHIFT*'
        IF(TITEL.EQ.'*SLIT*')     TITELS='   *SLIT*   '
        WRITE(IO,1051) TITELS,LABI,X,DIM2,XP,Y,DIM2,YP,Z
        I=I+1
        IF (.NOT.PRINT .AND. .NOT.FILE .AND. 19*(I/19)-I .EQ. 0) THEN
         WRITE(6,1235)
         READ(5,1234,END=610) INPUT 
        ENDIF
        GO TO 605
  610   REWIND 18
  630   IF (I .EQ. 1) WRITE(IO,1052)
C
        IF (FILE) THEN
         CLOSE(UNIT=7)
         PRINT *, 'SHIFTS.DAT data_file generated.                     '
        ENDIF
        IF (PRINT) THEN
         WRITE(6,3016) ESC
         PRINT *, 'SHIFTS.DAT appended on print_file OUTPUT.DAT.'
            ENDIF
        GO TO 1
C
C   DISPLAY
C
  500   I=1
        OPEN(UNIT=4,FILE='FOR004.DAT',STATUS='OLD')
  501   READ(4,'(A)',END=502,ERR=570) LINE
        DO 511 J=1,90
           IF (LINE(J:J) .EQ. CHAR(13)) LINE(J:J) = ' '
           IF (LINE(J:J) .EQ. CHAR(10)) LINE(J:J) = ' '
  511   CONTINUE
        READ(LINE,1011,ERR=570) NAME(I),PAR(I)
        I=I+1
        GO TO 501
  502   IN=I-1
        CLOSE(UNIT=4)
C
        IF (FILE) THEN
         OPEN(UNIT=7,FILE='DISPLAY.DAT',STATUS='UNKNOWN')
         WRITE(7,3012) UEBER
         WRITE(7,1114)
        ELSE IF (PRINT) THEN
         WRITE(7,1114)
        ELSE
         WRITE(6,1014) ESC
        ENDIF
C
        DO 550 I=1,IN
  550   WRITE(IO,1011) NAME(I),PAR(I)
C
        IF(FILE) THEN
         CLOSE(UNIT=7)
         PRINT *, 'DISPLAY.DAT data_file generated.                    '
        ENDIF
        IF (PRINT) THEN
         WRITE(6,3016) ESC
         PRINT *, 'DISPLAY.DAT appended on print_file OUTPUT.DAT.'
        ENDIF
        IF(PRINT.OR.FILE) GO TO 1
C
        MAL=0
        WRITE(6,1016) ESC,ESC,ESC,ESC,ESC
  503   WRITE(6,3015) ESC,ESC
        READ(5,1002,END=560) INPUT
        CALL TOUPPER(INPUT)
        IF(INPUT(1:1) .EQ. ' ') GO TO 560
        DO 520 I = 1, 6
         IF (INPUT(I:I) .EQ. ',') GO TO 525
  520   CONTINUE
        GO TO 506
  525   N = I - 1
        KEY = INPUT(1:N)
        NNN = 0
        DO 504 I = 1, 19
         IF (KEY .EQ. NAME(I)) IND = I
         IF (KEY .EQ. ITEM(I)) NNN = 1
  504   CONTINUE
        IF (NNN .EQ. 0) THEN
         WRITE(6,1013) ESC,ESC,ESC
         GO TO 503
        ENDIF
        DO 530 J = 20, N+2, -1
         IF (INPUT(J:J) .NE. ' ') GO TO 535
  530   CONTINUE
  535   CONTINUE    
        TEMP(1:10) = '          '
        TEMP(10-J+N+2:10) = INPUT(N+2:J)
        READ(TEMP,1012,ERR=506) PAR(IND)
        MAL=MAL+1
        WRITE(6,1014) ESC
        DO 536 I=1,IN
  536   WRITE(6,1011) NAME(I),PAR(I)
        WRITE(6,1016) ESC,ESC,ESC,ESC,ESC
        GO TO 503
  506   WRITE(6,1019) ESC,ESC,ESC
        GO TO 503
C
C   WRITE NEW DISPLAY DATA FILE
C
  560   IF(MAL.EQ.0) GO TO 1111
        OPEN(UNIT=4,FILE='FOR004.DAT',STATUS='UNKNOWN')
        REWIND(UNIT=4)
        DO 510 I=1,IN
  510   WRITE(4,1011) NAME(I),PAR(I)
        CLOSE(UNIT=4)
        WRITE(6,1021) ESC
        GO TO 1111
C
  570   PRINT *, 'FOR004.DAT (Display data) not found.'
        GO TO 9999
  777   PRINT *, 'You have to run Transport first.'
        GO TO 9999
C
  999   WRITE(6,5555) ESC,ESC,ESC
        IF (PRINT) THEN
         CLOSE(UNIT=7)
         CALL SYSTEM('./print text OUTPUT.DAT')
        ENDIF
 9999   STOP
C
 1000   FORMAT(A10)
 1001   FORMAT(' ',A1,'[0J',A1,'[5mno match found, try again',A1,'[0m')
 1002   FORMAT(A)
 1004   FORMAT(' ',A1,'[0J    INITIAL  BEAM  CONDITION'/)
 1044   FORMAT(' ',A1,'[0J    Floor Layout Coordinates'/)
 1104   FORMAT(/'     INITIAL  BEAM  CONDITION'/)
 1144   FORMAT(/'     Floor Layout Coordinates'/)
 1005   FORMAT(' ',A1,'[0J',10X,'QUADS, SOLENOIDS & SEXTUPOLES'/)
 1105   FORMAT(/11X,'QUADS, SOLENOIDS & SEXTUPOLES'/)
 1006   FORMAT(1X,A10,A4,F8.3,1X,A2,F8.1,1X,A2,F10.5,1X,A2)
 1007   FORMAT(' ',A1,'[0JTITEL    LABEL    Z           X',
     X  '          Y          R16           R36')
 1107   FORMAT(/33X,'BEAMLINE'//
     X  ' TITEL    LABEL    Z           X',
     X  '          Y          R16           R36')
 1008   FORMAT(1X,A10,A4,F8.3,1X,A2,2(F8.2,1X,A2),2(F8.2
     1  ,1X,A2,'/',A2))
 1009   FORMAT(' ',A1,'[0J',24X,'FIT CONDITIONS'/)
 1109   FORMAT(/25X,'FIT CONDITIONS'/)
 1010   FORMAT(1X,A4,F14.4,2H /,F7.5,5X,1H(,F10.5,2H ),F10.3,F9.0,F5.0)
 1012   FORMAT(F10.1)
 1011   FORMAT(A10,F10.2)
 1013   FORMAT(' ',A1,'[2;25H',A1,'[5mkeyword not found, try again. ',
     X             A1,'[0m')
 1014   FORMAT(' ',A1,'[0J')
 1114   FORMAT(/2X,'DISPLAY PARAMETERS'/)
 1015   FORMAT(' ',A1,'[2;0H',A1,'[7m',
     X  'enter keyword: BE(AM), B(ENDS), Q(UADS), F(ITS), S(HIFTS),',
     X  ' C(OOR) '/
     X  ' QD(AC), B(EAM)L(INE), D(ISPLAY), P(RINT), SC(REEN), FI(LE):',
     X  A1,'[0m')
 1016   FORMAT(' ',A1,'[2;0H',A1,'[7mDISPLAY PARAMETERS   ',A1,'[0m',
     X         50(' ')/' ',A1,'[7menter keyword, value:',A1,'[0m',
     X         50(' '))
 1018   FORMAT(1X,A10,A4,F8.3,1X,A2,F8.1,1X,A2,F10.5,1X,A2,I10)
11018   FORMAT(1X,A10,A4,F8.3,1X,A2,F8.1,1X,A2,F10.5,1X,'kV/cm')
 1019   FORMAT(' ',A1,'[2;25H',A1,'[5mwrong input format, try again.',
     X             A1,'[0m')
 1020   FORMAT(/25X,'no data found.'/)
 1021   FORMAT(' ',A1,'[2;25H',30(' '))
 1028   FORMAT(' ',A1,'[0J       QUAD-AMP'/)
 1128   FORMAT(/'     QUAD-AMP'/)
 1029   FORMAT(5X,A4,1X,F9.3,' A')
 1041   FORMAT(5X,A4,1X,F9.3,1X,A2)
 1030   FORMAT(' ',A1,'[2;0H',A1,'[7menter: Q(DAC)/QA(MP) for DAC/AMP
     X values or RETURN:',A1,'[0m               '/,70(' '))
 1031   FORMAT(' ',A1,'[0J     QUAD-DAC'/)
 1131   FORMAT(/'     QUAD-DAC'/)
 1032   FORMAT(5X,A4,2X,I5)
 1040   FORMAT(5X,A4,2X,I5,1X,A1)
 1033   FORMAT(' ',A1,'[2;0H',A1,'[7menter: T for VAX->PDP transfer
     X-file or RETURN:',A1,'[0m               '/,70(' '))
 1034   FORMAT(1X,10I6)
 1035   FORMAT(1X,A1,'WOR 10 H K')
 1036   FORMAT(1X,A1,'MON K')
 1037   FORMAT(1X,A1,'MON H')
 1038   FORMAT(I3)
 1039   FORMAT(7F10.3)
 1050   FORMAT(' ',A1,'[0J   TYPE      LABEL      x',
     A  '          x''           y          y''          z')
 1150   FORMAT(/28X,'BEAM-AXIS SHIFTS and SLITS'//
     A  '    TYPE      LABEL      x',
     A  '          x''           y          y''          z')
 1051   FORMAT(' ',A12,'  ',A4,2(F8.2,1X,A2,F8.2,' mrad'),F9.3,' m')
 1052   FORMAT(/25X,'no data found.'/)
 1234   FORMAT(A)
 1235   FORMAT(' <CR> for continue ',$)
 2005   FORMAT(5X,A4,F10.3,1X,A2,'mrad')
 2006   FORMAT(5X,A4,F10.3,1X,A2,5X)
 2007   FORMAT(5X,A4,F10.3,' mrad')
 2008   FORMAT(5X,A4,F10.3,7X)
 2009   FORMAT(5X,A4,F10.5,1X,A3,'/c')
 2010   FORMAT(' CHI**2=',E12.5)
 2011   FORMAT(' CHI**2=',E12.5,'         *FAILED*')
 2012   FORMAT(5X,A4,F10.3,1X,A2,'/',A2,2X)
 2013   FORMAT(5X,A4,F10.3,1X,'mrad/',A2)
 2014   FORMAT(' *VAR* :',10(1X,A4,','))
 2077   FORMAT(1X,A4,2X,A4,2X,I4,2X,I4,7F7.1)
 3005   FORMAT(' ',A1,'[0J',24X,'BENDING MAGNETS'/)
 3105   FORMAT(/24X,'BENDING MAGNETS'/)
 3006   FORMAT(A10,A4,
     X  F8.3,' ',A4,' K1 =',F6.3,'  K2 =',F6.3,'   1/R =',F8.3)
 3010   FORMAT(A79)
c3011   FORMAT(' ',A1,'[0;0H',A1,'[1;4m',A,A1,'[0m')
 3011   FORMAT(' ',A1,'[0;0H',A1,'[0m',A,A1,'[0m')
 3012   FORMAT(/,1X,A79,//)
 3013   FORMAT(' ',A1,'[2J',A1,'[5;24r')
 3014   FORMAT(' ',A1,'[3;70H',10(' '),A1,'[3;70H',$)
 3015   FORMAT(' ',A1,'[3;30H',15(' '),A1,'[3;30H',$)
 3016   FORMAT(70(' '),A1,'[4;0H',$)
 4006   FORMAT(
     X  A10,A4,F8.3,1X,A2,F8.1,1X,A2,F10.4,1X,A2,'  n =',F8.3,
     X  '  B.A =',F6.2,1X,A4)
 4007   FORMAT(
     X  A10,A4,F8.3,1X,A2,F6.1,1X,A2,F10.4,' kV/cm  pq =',F6.3,
     X  '  D.A =',F6.2,1X,A4)
 5555   FORMAT(' ',A1,'0;0H',A1,'[0;24r',A1,'[2J')
 5556   FORMAT(' ',A1,'[0J',A1,'[4;0H',$)
        END

        SUBROUTINE TOUPPER(KEY)
C    
        CHARACTER*10 KEY
C    
        DO 10 J = 1, LEN(KEY)
         NUM = ICHAR(KEY(J:J))
         IF (NUM.GT.96 .AND. NUM.LT.123) NUM = NUM - 32
         KEY(J:J) = CHAR(NUM)
   10   CONTINUE
C
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
