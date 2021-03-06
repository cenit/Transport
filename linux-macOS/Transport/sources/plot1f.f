        SUBROUTINE PLOT1F
C
C   INTERFACE ROUTINE FOR GRAPHIC TRANSPORT
C   AUTHOR: U.ROHRER (SIN), NOVEMBER 1983
C
        PARAMETER (LDM=2000)
        CHARACTER*2  DIM1,DIM2,DIM3,DIM4,DIM7
        CHARACTER*4  LAB,LABEL,DIM5,DIM6,LABF,NOFI,CHI2
        CHARACTER*9  TIM
        CHARACTER*10 DAT, BASCII
        CHARACTER*10 TITEL,NAME,LABS
        CHARACTER*90 UEBER,TITLE,LINE
C
       DIMENSION
     * Y0(LDM),X0P(LDM),Y0P(LDM),RAX(LDM),RAY(LDM),TITEL(LDM),
     * PXMAX(LDM),XMAX(LDM),R12X(LDM),YMAX(LDM),PYMAX(LDM),X0(LDM),
     * R34(LDM),QXMAX(LDM),QYMAX(LDM),XQ(LDM),YQ(LDM),ZQ(LDM),XN(LDM)
     * ,LENG(LDM),AP(LDM),D(LDM),DP(LDM),DQ(LDM),QDP(LDM),LABEL(LDM),
     * DY(LDM),DPY(LDM),DQY(LDM),QDPY(LDM),ZRO(LDM),Z(LDM),B(LDM),
     * CX(LDM),SX(LDM),CPX(LDM),SPX(LDM),CY(LDM),SY(LDM),CPY(LDM),
     * SPY(LDM),QX0P(LDM),QY0P(LDM),QCPX(LDM),QSPX(LDM),QCPY(LDM),
     * QSPY(LDM),R66(LDM),DLMAX(LDM),DPPMAX(LDM),R56(LDM),
     * R13(LDM),R24(LDM)
       DIMENSION
     * X0A(100),X0PA(100),Y0A(100),Y0PA(100),ZZ(200),
     * F1(200),F2(200),CON(200),DEL(200),VAL(200),LABF(200),LABS(100)
     * ,LABVS(LDM),ANG(LDM),XDA(4),SIG(17),RM(13),
     * CHI(50),IPOINT(50),IREM(50),FAIL(50),XDIME(11),
     * DUM(6),ZA(3),LABV(20)
C
       LOGICAL     IT,FAILED,FAIL,RAUM,SLITS,EQUAD
       REAL        LENG
       INTEGER     TYP
       EQUIVALENCE (NL,SIG(3))
       EQUIVALENCE (SIG(4),LABV(1))
C       EQUIVALENCE (RM(1),LABV(12))
       EQUIVALENCE (RM(10),FAILED)
C
        DATA IT,RAUM,SLITS,EQUAD /.FALSE.,.FALSE.,.FALSE.,.FALSE./
        DATA NOFI,CHI2 /'NOFI','CHI2'/
        DATA G2,XK1,XK2,R1INV,R2INV,CHIST /10.,.5,.0,.0,.0,-0.1/
        DATA IND,JND,IBC,IN,ICH,IS,IYTIT,ZROT /0,0,0,0,0,0,730,0./
        DATA ZMIN,ZMAX,XMPL,YMPL,APFAC /0.,50.,10.,10.,1./
        DATA ISPEZ,ISPEZ2,IDISP,KDISP,IENV /0,0,0,0,1/
        DATA IRAY,IRAX,MEASX,MEASY,MEASD /0,0,0,0,0/
        DATA ICSX,ICSY,ILABS /0,0,0/
C
C   ANALYSIERE PLOTFILE
C
        OPEN(UNIT=2,FILE='FOR002.DAT',FORM='UNFORMATTED',
     X       STATUS='OLD',ERR=777)
        READ(2) XDIME,AMP
C       METER (M)
        WRITE(DIM1,3001) XDIME(8)
C       MILLIMETER (MM)
        WRITE(DIM2,3001) XDIME(1)
C       PROZENT (PC)
        WRITE(DIM3,3001) XDIME(6)
C       KILOGAUSS (KG)
        WRITE(DIM4,3001) XDIME(9)
C       GEV
        WRITE(DIM5,3000) XDIME(11)
C       DEG
        WRITE(DIM6,3000) XDIME(7)
C       CENTIMETER (CM)
        WRITE(DIM7,3001) XDIME(5)

        IF(AMP.NE.0.0) RAUM=.TRUE.
C
        IF(DIM1.EQ.'M ') DIM1='m '
        IF(DIM1.EQ.'CM') DIM1='cm'
        IF(DIM2.EQ.'MM') DIM2='mm'
        IF(DIM2.EQ.'CM') DIM2='cm'
        IF(DIM3.EQ.'PC') DIM3='% '
        IF(DIM3.EQ.'PM') DIM3='%o'
        IF(DIM4.EQ.'KG') DIM4='kG'
        IF(DIM5.EQ.'GEV ') DIM5='GeV '
        IF(DIM6.EQ.'DEG ') DIM6='deg '
        IF(DIM7.EQ.'CM') DIM7='cm'
        IF(DIM7.EQ.'MM') DIM7='mm'
C
    1   CONTINUE
        READ(2,END=20,ERR=800) TYP,LAB,XDA,ZS,SIG,RM
C       WRITE(*,'(I4,1X,A4,F7.3)') TYP,LAB,ZS
        IF(TYP.EQ.3) GO TO 310
        IF(TYP.EQ.5 .OR. TYP.EQ.18) GO TO 320
        IF(TYP.EQ.4 .OR. TYP.EQ.19) GO TO 330
        IF(TYP.EQ.2) GO TO 340
        IF(TYP.EQ.10) GO TO 360
        IF(TYP.EQ.30) GO TO 430
        IF(TYP.EQ.20) GO TO 350
        IF(TYP.EQ.11) GO TO 450
        IF(TYP.EQ.7) GO TO 370
        IF(TYP.EQ.14) GO TO 380
        IF(TYP.EQ.1) GO TO 390
        IF(TYP.EQ.12) GO TO 410
        IF(TYP.EQ.16) GO TO 420
        IF(TYP.EQ.22) GO TO 435
        IF(TYP.EQ.23) GO TO 440
        IF(TYP.EQ.21) GO TO 330
        IF(TYP.EQ.6 .AND. SLITS) GO TO 460
        GO TO 1
C
C   *DRIFT*
C
  310   IND=IND+1
        LENG(IND)=XDA(1)    !!!!!!!!!!!!!!!!!!!!!!NEW FOR PROG EMU
        TITEL(IND)='*DRIFT*'
        GO TO 8
C
C   *QUAD*
C
  320   IND=IND+1
        LENG(IND)=XDA(1)
        B(IND)=XDA(2)
        AP(IND)=XDA(3)
        IF(TYP.EQ.5) TITEL(IND)='*QUAD*'
        IF(TYP.EQ.18) TITEL(IND)='*SEXT*'
        IF(TYP.EQ.5 .AND. EQUAD) TITEL(IND)='*EQUAD*'
        GO TO 8
C
C   *BEND*
C
  330   IND=IND+1
        AP(IND)=G2
        IF(IZROT.EQ.90 .OR. IZROT.EQ.-90 .OR. IZROT.EQ.270) AP(IND)=-G2
        LENG(IND)=XDA(1)
        B(IND)=XDA(2)
        XN(IND)=XDA(3)
        ANG(IND)=XDA(4)
        IF(TYP.EQ.4) TITEL(IND)='*BEND*'
        IF(TYP.EQ.19) TITEL(IND)='*SOLO*'
        IF(TYP.EQ.21) TITEL(IND)='*WIEN*'
        GO TO 8
C
C   *ROTAT*
C
  340   IND=IND+1
        IF (XDA(1) .EQ. 1000.) THEN
         TITEL(IND)='*FRINGE*'
         LENG(IND)=XDA(2)
         AP(IND)=XDA(3)
         B(IND)=XDA(4)
         GO TO 8
        ENDIF
        TITEL(IND)='*ROTAT*'
        LENG(IND)=XDA(1)
        AP(IND)=XK1
        B(IND)=XK2
        XN(IND)=R1INV
        IF(TITEL(IND-1).EQ.'*BEND*') XN(IND)=R2INV
        GO TO 8
C
C   *Z RO*
C
  350   IND=IND+1
        X=XDA(1)
        ANG(IND)=XDA(1)     !!!!!!!!!!!!!!!!!!!!!!NEW FOR PROG EMU
        ZROT=ZROT+X*3.14159/180.
        IZROT=NINT(ZROT*180./3.14159)
        TITEL(IND)='*Z RO*'
        GO TO 8
C
C   *FIT*
C
  360   IN=IN+1
        IF (IN .GT. 200) STOP 'To many Fit Constraints'
        LABF(IN)=LAB
8765    FORMAT(I5,2X,A4)
        F1(IN)=XDA(1)
        F2(IN)=XDA(2)
        CON(IN)=XDA(3)
        DEL(IN)=XDA(4)
        VAL(IN)=ZS
        ZZ(IN)=SIG(1)
        IF(SIG(2).EQ.CHIST) GO TO 1
        IF(ICH.EQ.50) GO TO 1
        ICH=ICH+1
        IPOINT(ICH)=IN-1
        CHIST=SIG(2)
        FAIL(ICH)=FAILED
        DO 25 J=1,NL
        JND=JND+1
   25   LABVS(JND)=LABV(J)
        IREM(ICH)=JND
        JMAX=JND
        CHI(ICH)=CHIST
        GO TO 1
C
C   *AXIS SHIFT*
C
  370   IS=IS+1
        LABS(IS)=LAB
        X0A(IS)=XDA(1)
        X0PA(IS)=XDA(2)
        Y0A(IS)=XDA(3)
        Y0PA(IS)=XDA(4)
        IND=IND+1
        TITEL(IND)='*AXIS SHI*'
        GO TO 8
C
C   *ELEMENT MATRIX*
C
  380   IF (ZS .EQ. 0.0) THEN
         I=IND
         CALL FILLIN(ZS,SIG,RM,Z(I),X0(I),X0P(I),Y0(I),Y0P(I),
     X   XMAX(I),PXMAX(I),YMAX(I),PYMAX(I),R12X(I),R34(I),
     X   CX(I),SX(I),D(I),CPX(I),SPX(I),DP(I),
     X   CY(I),SY(I),DY(I),CPY(I),SPY(I),DPY(I),R66(I),
     X   DLMAX(I),DPPMAX(I),R56(I),R13(I),R24(I))
         GO TO 1
        ELSE
         IND = IND + 1
         LENG(IND) = ZS - Z(IND-1)
         TITEL(IND) = '*MATRIX*'
         GO TO 8
        ENDIF
C
C   *BEAM*
C
  390   P=XDA(1)
        DELP=SIG(12)
        IND=IND+1
        ANG(IND)=XDA(1)     !!!!!!!!!!!!!!!!!!!!!!NEW FOR PROG EMU
        TITEL(IND)='*BEAM*'
        IBC=IBC+1
        I=IND
        CALL FILLIN(ZS,SIG,RM,Z(I),X0(I),X0P(I),Y0(I),Y0P(I),
     1  XMAX(I),PXMAX(I),YMAX(I),PYMAX(I),R12X(I),R34(I),
     2  CX(I),SX(I),D(I),CPX(I),SPX(I),DP(I),
     3  CY(I),SY(I),DY(I),CPY(I),SPY(I),DPY(I),R66(I),
     4  DLMAX(I),DPPMAX(I),R56(I),R13(I),R24(I))
        IF(IBC.GT.1) GO TO 110
        SAVE1=XMAX(1)
        SAVE2=PXMAX(1)
        SAVE3=R12X(1)
        GO TO 110
C
C   *CORR*
C
  410   I=IND
        CALL FILLIN(ZS,SIG,RM,Z(I),X0(I),X0P(I),Y0(I),Y0P(I),
     1  XMAX(I),PXMAX(I),YMAX(I),PYMAX(I),R12X(I),R34(I),
     2  CX(I),SX(I),D(I),CPX(I),SPX(I),DP(I),
     3  CY(I),SY(I),DY(I),CPY(I),SPY(I),DPY(I),R66(I),
     4  DLMAX(I),DPPMAX(I),R56(I),R13(I),R24(I))
        IF(IBC.GT.1) GO TO 1
        SAVE1=XMAX(1)
        SAVE2=PXMAX(1)
        SAVE3=R12X(1)
        GO TO 1
C
C   * G/2 *
C   * K1 *
C   * K2 *
C   * 1/R1 *
C   * 1/R2 *
C       * PARAM *
C
  420   IDENT=XDA(1)+.001
        IF(IDENT.EQ.5) G2=XDA(2)
        IF(IDENT.EQ.7) XK1=XDA(2)
        IF(IDENT.EQ.8) XK2=XDA(2)
        IF(IDENT.EQ.12) R1INV=XDA(2)
        IF(IDENT.EQ.13) R2INV=XDA(2)
        IF(IDENT.EQ.14) SLITS=.TRUE.
        IF(IDENT.EQ.95) EQUAD=.FALSE.
        IF(IDENT.EQ.96) EQUAD=.TRUE.
        GO TO 1
C
C   *SCINT*
C
  430   IND=IND+1
        TITEL(IND)='*SCINT*'
        LAB='    '
        RAUM=.TRUE.
        GO TO 8
C
C   *SP CH*
C
  435   IND=IND+1
        TITEL(IND)='*SP CH*'
        LENG(IND)=XDA(1)
        B(IND)=XDA(2)
        XN(IND)=XDA(3)
        ANG(IND)=XDA(4)
        GO TO 8
C
C   *RF CAV*
C
  440   IND=IND+1
        TITEL(IND)='*RF CAV*'
        GO TO 8
C
C   *ACC*
C
  450   IND=IND+1
        TITEL(IND)='*ACC*'
        AP(IND)=G2
        LENG(IND)=XDA(1)
        B(IND)=XDA(2)
        GO TO 8
C
C       *SLIT*
C
  460   IND=IND+1
        IS=IS+1
        LABS(IS)=LAB
        TITEL(IND)='*SLIT*'
        ICO1=XDA(1)
        ICO2=XDA(3)
        IF(ICO1.EQ.1) X0A(IS)=XDA(2)
        IF(ICO1.EQ.3) Y0A(IS)=XDA(2)
        IF(ICO2.EQ.3) Y0A(IS)=XDA(4)
        IF(ICO2.EQ.1) X0A(IS)=XDA(4)
        IF(ICO1.EQ.1) ANG(IND)=XDA(2)
        IF(ICO1.EQ.3) AP(IND)=XDA(2)
        IF(ICO2.EQ.3) AP(IND)=XDA(4)
        IF(ICO2.EQ.1) ANG(IND)=XDA(4)
        GO TO 8
C
C   SIGMA-MATRIX FOR OTHER THAN BEAM,CORR AND ELEMENT MATRIX
C
    8   I=IND
        CALL FILLIN(ZS,SIG,RM,Z(I),X0(I),X0P(I),Y0(I),Y0P(I),
     1  XMAX(I),PXMAX(I),YMAX(I),PYMAX(I),R12X(I),R34(I),
     2  CX(I),SX(I),D(I),CPX(I),SPX(I),DP(I),
     3  CY(I),SY(I),DY(I),CPY(I),SPY(I),DPY(I),R66(I),
     4  DLMAX(I),DPPMAX(I),R56(I),R13(I),R24(I))
  110   CONTINUE
        LABEL(IND)=LAB
        ZRO(IND)=ZROT
        IF(IND.GE.2000) GO TO 19
        IF(IND.EQ.1) GO TO 1
        IF(ABS(Z(IND)-Z(IND-1)).LE..0009) Z(IND)=Z(IND-1)+.0001
        GO TO 1
C
C   DATA MAX ERREICHT
C
   19   IT = .TRUE.
   20   CONTINUE
        INDMAX=IND
        IF(IND.EQ.0) GO TO 800
C
C   ZURUECKROTIEREN UM DEN WINKEL ZRO(I)
C
        DO 23 I=1,INDMAX
        IF(ZRO(I).EQ.0.) GO TO 23
C
        SN=SIN(ZRO(I))
        CS=COS(ZRO(I))
C
C   DISPERSION
        X=D(I)*CS-DY(I)*SN
        DY(I)=D(I)*SN+DY(I)*CS
        D(I)=X
        X=DP(I)*CS-DPY(I)*SN
        DPY(I)=DP(I)*SN+DPY(I)*CS
        DP(I)=X
C
C   C&S TRAJEKTORIEN
        X=CX(I)*CS-CY(I)*SN
        CY(I)=CX(I)*SN+CY(I)*CS
        CX(I)=X
        X=SX(I)*CS-SY(I)*SN
        SY(I)=SX(I)*SN+SY(I)*CS
        SX(I)=X
        X=CPX(I)*CS-CPY(I)*SN
        CPY(I)=CPX(I)*SN+CPY(I)*CS
        CPX(I)=X
        X=SPX(I)*CS-SPY(I)*SN
        SPY(I)=SPX(I)*SN+SPY(I)*CS
        SPX(I)=X
C
C   BEAMCENTROID
        X=X0(I)*CS-Y0(I)*SN
        Y0(I)=X0(I)*SN+Y0(I)*CS
        X0(I)=X
        X=X0P(I)*CS-Y0P(I)*SN
        Y0P(I)=X0P(I)*SN+Y0P(I)*CS
        X0P(I)=X
C
C   BEAM
C   GILT NUR FUER VIELFACHE VON +-90 GRAD
C   SONST NUR ANNAEHERUNGSWEISE FALLS MISCHTERME DER SIGMA
C   MATRIX VERNACHLAESSIGBAR SIND
C
        SN=SN**2
        CS=CS**2
C
        S11=(XMAX(I))**2
        S22=(PXMAX(I))**2
        S21=R12X(I)*XMAX(I)*PXMAX(I)
        S33=(YMAX(I))**2
        S44=(PYMAX(I))**2
        S43=R34(I)*YMAX(I)*PYMAX(I)
C
        XMAX(I)=SQRT(S11*CS+S33*SN)
        YMAX(I)=SQRT(S11*SN+S33*CS)
        PXMAX(I)=SQRT(S22*CS+S44*SN)
        PYMAX(I)=SQRT(S22*SN+S44*CS)
        R12X(I)=(S21*CS+S43*SN)/(XMAX(I)*PXMAX(I))
        R34(I)=(S21*SN+S43*CS)/(YMAX(I)*PYMAX(I))
C
   23   CONTINUE
      OPEN(UNIT=15,FILE='FOR015.DAT',FORM='UNFORMATTED',
     1       STATUS='UNKNOWN')
      REWIND 15
      WRITE(15) DIM1,DIM2,DIM3,DIM4,DIM5,DIM6,P,DELP,INDMAX,IND
     1,IN,IS,ICH,CHI,IPOINT,SAVE1,SAVE2,SAVE3
      WRITE(15) (TITEL(I),LABEL(I),Z(I),XMAX(I),YMAX(I),D(I)
     1,DY(I),LENG(I),AP(I),B(I),PXMAX(I),R12X(I),DPY(I),PYMAX(I)
     2,DP(I),R34(I),X0(I),Y0(I),X0P(I),Y0P(I),CX(I),SX(I),CPX(I)
     3,SPX(I),CY(I),SY(I),CPY(I),SPY(I),XN(I),ANG(I),I=1,INDMAX)
      WRITE(15) (LABF(I),ZZ(I),F1(I),F2(I),CON(I),DEL(I),VAL(I)
     1,I=1,IN)
      WRITE(15) (X0A(I),X0PA(I),Y0A(I),Y0PA(I),LABS(I),I=1,IS)
C
C   TRAILER=INFORMATION ABOUT VARIED PARAMETERS AND FAILED FITS
C
      WRITE(15) JMAX,ICH
      WRITE(15) (LABVS(J),J=1,JMAX),(IREM(K),K=1,ICH)
     1,(FAIL(L),L=1,ICH)
      WRITE(15) SLITS
      WRITE(15) (R66(I),I=1,INDMAX)
      WRITE(15) (DLMAX(I),DPPMAX(I),R56(I),I=1,INDMAX)
      WRITE(15) DIM7
      DO 5678 I=1,INDMAX
       WRITE(15) R13(I),R24(I)
 5678 CONTINUE
      CLOSE(UNIT=15)

      GO TO 850
C
      ENTRY PLOT2
C
      OPEN(UNIT=15,FILE='FOR015.DAT',STATUS='OLD',
     1     FORM='UNFORMATTED',ERR=777)
      READ(15,ERR=777) DIM1,DIM2,DIM3,DIM4,DIM5,DIM6,P,DELP
     1,INDMAX,IND,IN,IS,ICH,CHI,IPOINT,SAVE1,SAVE2,SAVE3
      READ(15) (TITEL(I),LABEL(I),Z(I),XMAX(I),YMAX(I),D(I)
     1,DY(I),LENG(I),AP(I),B(I),PXMAX(I),R12X(I),DPY(I),PYMAX(I)
     2,DP(I),R34(I),X0(I),Y0(I),X0P(I),Y0P(I),CX(I),SX(I),CPX(I)
     3,SPX(I),CY(I),SY(I),CPY(I),SPY(I),XN(I),ANG(I),I=1,INDMAX)
      READ(15) (LABF(I),ZZ(I),F1(I),F2(I),CON(I),DEL(I),VAL(I)
     1,I=1,IN)
      READ(15) (X0A(I),X0PA(I),Y0A(I),Y0PA(I),LABS(I),I=1,IS)
C
C   TRAILER=INFORMATION ABOUT VARIED PARAMETERS AND FAILED FITS
C
      READ(15) JMAX,ICH
      READ(15) (LABVS(J),J=1,JMAX),(IREM(K),K=1,ICH)
     1,(FAIL(L),L=1,ICH)
      READ(15) SLITS
      READ(15) (R66(I),I=1,INDMAX)
      READ(15) (DLMAX(I),DPPMAX(I),R56(I),I=1,INDMAX)
      READ(15) DIM7
      DO 3456 I=1,INDMAX
      READ(15) R13(I),R24(I)
 3456 CONTINUE
  850 CONTINUE
C
C   LESEN DER INFORMATION FUER DAS PLOTTEN
C
        OPEN(UNIT=4,FILE='FOR004.DAT',STATUS='OLD',ERR=778)
100     READ(4,'(A)',END=180,ERR=778) LINE
        DO 111 J=1,90
         IF (LINE(J:J) .EQ. CHAR(13)) LINE(J:J) = ' '
         IF (LINE(J:J) .EQ. CHAR(10)) LINE(J:J) = ' '
 111    CONTINUE
        READ(LINE,1020,ERR=778) NAME,PAR
        IF(NAME.EQ.'ZMIN') ZMIN=PAR
        IF(NAME.EQ.'ZMAX') ZMAX=PAR
        IF(NAME.EQ.'XMPL') XMPL=PAR
        IF(NAME.EQ.'YMPL') YMPL=PAR
        IF(NAME.EQ.'APFAC') APFAC=PAR
        IF(NAME.EQ.'SPEZ') ISPEZ=PAR
        IF(NAME.EQ.'SPEZ2') ISPEZ2=PAR
        IF(NAME.EQ.'DISP') IDISP=PAR
        IF(NAME.EQ.'YDISP') KDISP=PAR
        IF(NAME.EQ.'RAX') IRAX=PAR
        IF(NAME.EQ.'RAY') IRAY=PAR
        IF(NAME.EQ.'MEASX') MEASX=PAR
        IF(NAME.EQ.'MEASY') MEASY=PAR
        IF(NAME.EQ.'MEASD') MEASD=PAR
        IF(NAME.EQ.'CSX') ICSX=PAR
        IF(NAME.EQ.'CSY') ICSY=PAR
        IF(NAME.EQ.'YTIT') IYTIT=PAR
        IF(NAME.EQ.'STEER') ILABS=PAR+.05*SIGN(1.0,PAR)
        IF(ICSX.NE.0) IDISP=1
        IF(ICSX.NE.0) IRAX=1
        IF(ICSY.NE.0) KDISP=1
        IF(ICSY.NE.0) IRAY=1
        GO TO 100
  180   CONTINUE
        ICOMP = 0
        IF (APFAC .LT. 0.0) THEN
          APFAC = -1.0 * APFAC
          ICOMP = 1
        ENDIF
        IF(IRAX.EQ.1 .OR. IRAY.EQ.1) IENV=0
        IF(ICSX.NE.0 .OR. ICSY.NE.0) IENV=0
        MOD=ISPEZ2/10
        ISPEZ2=ISPEZ2-10*MOD
C
C   LIST BEAMLINE
C
      OPEN(UNIT=3,FILE='FOR003.DAT',FORM='UNFORMATTED',
     1     STATUS='UNKNOWN')
      REWIND 3
      DO 30 L = 1,IND
      WRITE(3) TITEL(L),LABEL(L),Z(L),DIM1,XMAX(L),DIM2,YMAX(L)
     1,DIM2,D(L),DIM2,DIM3,DY(L),DIM2,DIM3
   30 CONTINUE
      CLOSE(UNIT=3)
C
C   LISTE MAGNETELEMENTE
C
      OPEN(UNIT=11,FILE='FOR011.DAT',FORM='UNFORMATTED',
     1       STATUS='UNKNOWN')
      REWIND 11
      DO 35 L=1,IND
      IF(TITEL(L).NE.'*QUAD*'.AND.TITEL(L).NE.'*BEND*'.AND.
     1TITEL(L).NE.'*SEXT*'.AND.TITEL(L).NE.'*SOLO*'.AND.
     2TITEL(L).NE.'*WIEN*'.AND.TITEL(L).NE.'*ROTAT*') GO TO 355
      WRITE(11) TITEL(L),LABEL(L),LENG(L),DIM1,ABS(AP(L)),DIM2
     1,B(L),DIM4,XN(L),ANG(L),DIM6
  355 IF (TITEL(L).EQ.'*EQUAD*')
     1 WRITE(11) TITEL(L),LABEL(L),LENG(L),DIM1,ABS(AP(L)),DIM2
     2,B(L),' kV/cm',XN(L),ANG(L),DIM6
   35 CONTINUE
      CLOSE(UNIT=11)
C
C   INITIAL BEAM CONDITION
C
        OPEN(UNIT=12,FILE='FOR012.DAT',FORM='UNFORMATTED',
     1       STATUS='UNKNOWN')
        DO 31 L=1,IND,1
        IF(Z(L).GT.ZMIN) GO TO 32
   31   CONTINUE
   32   I0=L-1
        WRITE(12) Z(I0),XMAX(I0),PXMAX(I0),R12X(I0),
     1                  YMAX(I0),PYMAX(I0),R34(I0),
     2                  D(I0),DP(I0),P,DELP,
     3                  DIM1,DIM2,DIM3,DIM5
        CLOSE(UNIT=12)
C
C   START BEAM
C
        OPEN(UNIT=17,FILE='FOR017.DAT',FORM='UNFORMATTED',
     1       STATUS='UNKNOWN')
        REWIND 17
        WRITE(17) SAVE1,SAVE2,YMAX(1),PYMAX(1),SAVE3,R34(1),D(1),DP(1)
     1 ,P,DELP
        CLOSE(UNIT=17)
C
C   FINAL BEAM CONDITION
C
        I=INDMAX
        OPEN(UNIT=16,FILE='FOR016.DAT',FORM='UNFORMATTED',
     1       STATUS='UNKNOWN')
        REWIND 16
        WRITE(16) XMAX(I),PXMAX(I),YMAX(I),PYMAX(I),DELP,P,R12X(I)
     1 ,R34(I),D(I),DP(I),DY(I),DPY(I),X0(I),X0P(I),Y0(I),Y0P(I)
        CLOSE(UNIT=16)
C
C   FIT BEDINGUNGEN
C
        OPEN(UNIT=14,FILE='FOR014.DAT',FORM='UNFORMATTED',
     1       STATUS='UNKNOWN')
        REWIND 14
        OPEN(UNIT=13,FILE='FOR013.DAT',FORM='UNFORMATTED',
     1       STATUS='UNKNOWN')
        REWIND 13
        WRITE(14) MEASX,MEASY,MEASD
        IF(IN.LT.1) GO TO 34
        DO 33 I=1,IN
        IF(ZZ(I).LT.ZMIN) GO TO 40
        IF(ZZ(I).GT.ZMAX) GO TO 40
        IF(F1(I) .EQ. 1. .AND. F2(I) .EQ. 1.) GO TO 36
        IF(F1(I) .EQ. 3. .AND. F2(I) .EQ. 3.) GO TO 37
        IF(F1(I) .EQ. 9. .AND. F2(I) .EQ. 3.) GO TO 38
        IF(F1(I) .EQ. 0. .AND. F2(I) .EQ. 1.) GO TO 138
        IF(F1(I) .EQ. 0. .AND. F2(I) .EQ. 3.) GO TO 139
        GO TO 40
   36   IYP = MAX (0, IFIX (350. - CON(I) * 350. / XMPL) )
        ITYP=1
        GO TO 39
   37   IYP = MIN (700, IFIX (350. + CON(I) * 350. / YMPL) )
        ITYP=2
        GO TO 39
   38   IYP = MAX (0,MIN (700,IFIX (350.-CON(I)*350./(XMPL*DELP) ) ) )
        ITYP=3
        GO TO 39
  138   IYP = MAX (0, MIN (700, IFIX (350.-CON(I)*350./XMPL) ) )
        ITYP=3
        GO TO 39
  139   IYP = MAX (0, MIN (700, IFIX (350.+CON(I)*350./YMPL) ) )
        ITYP=3
   39   IXP=1000.*(ZZ(I)-ZMIN)/(ZMAX-ZMIN)
        WRITE(14) ITYP,IXP,IYP
   40   CONTINUE
        IF(ICH.EQ.0) GO TO 141
        DO 140 J=1,ICH
        IF(I-1.EQ.IPOINT(J)) WRITE(13) CHI2,CHI(J),(DUM(K),K=1,5)
  140   CONTINUE
  141   CONTINUE
        WRITE(13) LABF(I),ZZ(I),F1(I),F2(I),CON(I),DEL(I),VAL(I)
   33   CONTINUE
   34   CONTINUE
        CLOSE(UNIT=14)
        IF(IN.LT.1) WRITE(13) NOFI,DUM
        CLOSE(UNIT=13)
C
C   AXIS SHIFT AND SLIT TABLE
C
        OPEN(UNIT=18,FILE='FOR018.DAT',FORM='UNFORMATTED',
     1       STATUS='UNKNOWN')
        REWIND 18
        I1=1
        DO 43 I=1,IS
        DO 143 J=I1,2000
        IF (LABS(I).EQ.LABEL(J) .AND. (TITEL(J).EQ.'*AXIS SHI*'
     1    .OR. TITEL(J).EQ.'*SLIT*')) THEN
         I1=J+1
         GO TO 43
        ENDIF
  143   CONTINUE
   43   WRITE(18) TITEL(I1-1),LABS(I)(1:4),X0A(I),DIM2,X0PA(I)
     1,Y0A(I),DIM2,Y0PA(I),Z(I1-1)
        CLOSE(UNIT=18)
C
C   ERZEUGUNG DER ENVELOPPEN DURCH INTERPOLATION
C
        NQ=401
        INDP=NQ-1
        FAC=1.0
        IF(DIM2.EQ.'cm') FAC=.1
        IF(DIM1.EQ.'cm') FAC=FAC*0.01
        DO 5 I=1,INDMAX
         QXMAX(I)=R12X(I)*PXMAX(I)*FAC
         QYMAX(I)=R34(I)*PYMAX(I)*FAC
         QDP(I)=DP(I)*FAC/R66(I)
         QDPY(I)=DPY(I)*FAC/R66(I)
         D(I)=D(I)/R66(I)
         DY(I)=DY(I)/R66(I)
         QX0P(I)=X0P(I)*FAC
         QY0P(I)=Y0P(I)*FAC
         QSPX(I)=SPX(I)*PXMAX(1)*FAC
         QCPX(I)=CPX(I)*XMAX(1)*FAC
         QSPY(I)=SPY(I)*PYMAX(1)*FAC
         QCPY(I)=CPY(I)*YMAX(1)*FAC
         CX(I)=CX(I)*XMAX(1)
         SX(I)=SX(I)*PXMAX(1)
         CY(I)=CY(I)*YMAX(1)
         SY(I)=SY(I)*PYMAX(1)
         IF(TITEL(I).NE.'*ADD TO BE') GO TO 5
         QXMAX(I)=PXMAX(I)*FAC
         QYMAX(I)=PYMAX(I)*FAC
    5   CONTINUE

        ZQD=(ZMAX-ZMIN)/(NQ-1)
        ZQS=ZMIN-ZQD
        NQQ=NQ-1
        IPOIN=0
        DO 6 I=1,NQQ
         ZQS=ZQS+ZQD
         IF(ZQS .LT. Z(1)) GO TO 6
         IF(ZQS .GT. Z(INDMAX)) GO TO 6
         IPOIN=IPOIN+1
         ZQ(I)=ZQS
         ZQ(I)=1000.*(ZQ(I)-ZMIN)/(ZMAX-ZMIN)
         IF (IENV .NE. 0) THEN
          XQ(I)=SPLINT(0,INDMAX,Z,XMAX,QXMAX,ZQS,ZA)
          XQ(I)=MIN(MAX(0.,350.-XQ(I)*350./XMPL),700.)
          YQ(I)=SPLINT(0,INDMAX,Z,YMAX,QYMAX,ZQS,ZA)
          YQ(I)=MIN(MAX(0.,350.+YQ(I)*350./YMPL),700.)
         END IF
         IF (ICSX .NE. 0 .OR. ICSY .NE. 0) GO TO 106
         IF (IDISP .NE. 0) THEN
          DQ(I)=-SPLINT(0,INDMAX,Z,D,QDP,ZQS,ZA)
          DQ(I)=MIN(MAX(0.,350.+DQ(I)*350./XMPL),700.)
         END IF
         IF (KDISP .NE. 0) THEN
          DQY(I)=SPLINT(0,INDMAX,Z,DY,QDPY,ZQS,ZA)
          DQY(I)=MIN(MAX(0.,350.+DQY(I)*350./YMPL),700.)
         END IF
         IF (IRAX .NE. 0) THEN
          RAX(I)=SPLINT(0,INDMAX,Z,X0,QX0P,ZQS,ZA)
          RAX(I)=MIN(MAX(0.,350.-RAX(I)*350./XMPL),700.)
         END IF
         IF (IRAY .NE. 0) THEN
          RAY(I)=SPLINT(0,INDMAX,Z,Y0,QY0P,ZQS,ZA)
          RAY(I)=MIN(MAX(0.,350.+RAY(I)*350./YMPL),700.)
         END IF
         GO TO 107
  106    CONTINUE
         IF (ICSX .NE. 0) THEN
          RAX(I)=ABS(SPLINT(0,INDMAX,Z,CX,QCPX,ZQS,ZA))
          RAX(I)=MIN(MAX(0.,350.-RAX(I)*350./YMPL),700.)
          DQ(I)=ABS(SPLINT(0,INDMAX,Z,SX,QSPX,ZQS,ZA))
          DQ(I)=MIN(MAX(0.,350.-DQ(I)*350./XMPL),700.)
         END IF
         IF (ICSY .NE. 0) THEN
          RAY(I)=ABS(SPLINT(0,INDMAX,Z,CY,QCPY,ZQS,ZA))
          RAY(I)=MIN(MAX(0.,350.+RAY(I)*350./YMPL),700.)
          DQY(I)=ABS(SPLINT(0,INDMAX,Z,SY,QSPY,ZQS,ZA))
          DQY(I)=MIN(MAX(0.,350.+DQY(I)*350./XMPL),700.)
         END IF
  107    CONTINUE
    6   CONTINUE
        OPEN(UNIT=7,FILE='FOR007.DAT',FORM='UNFORMATTED',
     1       STATUS='UNKNOWN')
        WRITE(7) IENV,IENV,IDISP,KDISP,IRAX,IRAY,IPOIN,ZQ(IPOIN)
        WRITE(7) (ZQ(I),XQ(I),YQ(I),DQ(I),DQY(I)
     1  ,RAX(I),RAY(I),I=1,IPOIN)
        CLOSE(UNIT=7)
C
C   KONTROLLIERE, OB WAIST-DURCHGANG INNERHALB DRIFT LIEGT.
C   FALLS WAHR, KONTROLLIERE GENAUIGKEIT DER INTERPOLATION.
C
        IF(RAUM .OR. IENV.EQ.0 .OR. MOD.EQ.0) GO TO 4
        DO 3 I=2,INDMAX
        IF(TITEL(I).NE.'*DRIFT*') GO TO 3
        J=I-1
        IF(Z(I).LT.ZMIN .OR. Z(J).GT.ZMAX) GO TO 3
        IF(R12X(J).GT.0.0) GO TO 2
        IF(R12X(I).LT.0.0) GO TO 2
        IF(R12X(J)**2.GE.1.0) GO TO 3
        XMIN=XMAX(J)*SQRT(1.0-R12X(J)**2)
        IF(PXMAX(J).EQ.0.0) GO TO 3
        ZXMIN=Z(J)-XMAX(J)*R12X(J)/(PXMAX(J)*FAC)
        XINT=SPLINT(1,2,Z(J),XMAX(J),QXMAX(J),ZXMIN,ZA)
        IF(ABS(XINT-XMIN) .LT. XMPL/50.0 .AND.
     1     ABS(ZA(1)) .LT. 0.1*XMPL/(ZMAX-ZMIN)) GO TO 2
        WRITE(6,1000) Z(J),DIM1,Z(I),DIM1
        GO TO 3
    2   IF(R34(J).GT.0.0) GO TO 3
        IF(R34(I).LT.0.0) GO TO 3
        IF(R34(J)**2.GE.1.0) GO TO 3
        YMIN=YMAX(J)*SQRT(1.0-R34(J)**2)
        IF(PYMAX(J).EQ.0.0) GO TO 3
        ZYMIN=Z(J)-YMAX(J)*R34(J)/(PYMAX(J)*FAC)
        YINT=SPLINT(1,2,Z(J),YMAX(J),QYMAX(J),ZYMIN,ZA)
        IF(ABS(YINT-YMIN) .LT. YMPL/50.0 .AND.
     X     ABS(ZA(1)) .LT. 0.1*YMPL/(ZMAX-ZMIN)) GO TO 3
        WRITE(6,1000) Z(J),DIM1,Z(I),DIM1
    3   CONTINUE
    4   CONTINUE
C
C   EINZEICNNEN DER POSITIONEN VON QUADRUPOLEN,DIPOLEN
C   SEXTUPOLEN UND SOLENOIDEN
C
        OPEN(UNIT=8,FILE='FOR008.DAT',FORM='UNFORMATTED',
     1       STATUS='UNKNOWN')
        REWIND 8
        WRITE(8) ICOMP
        DO 12 I=1,INDMAX
C   IF(LENG(I).EQ.0.0 .OR. TITEL(I).EQ.'*ROTAT*') GO TO 12  !!!! EMU-CHANGE
        IF(AP(I).EQ.0.0 .OR. TITEL(I).EQ.'*ROTAT*'
     1                  .OR. TITEL(I).EQ.'*FRINGE*'
     2                  .OR. TITEL(I).EQ.'*SLIT*') GO TO 12
        IF(Z(I).LT.ZMIN+LENG(I)) GO TO 12
        IF(Z(I).GT.ZMAX) GO TO 12
        Z1 =1000.*(Z(I)-ZMIN)/(ZMAX-ZMIN)
        Z2 =1000.*(Z(I)-ZMIN-LENG(I))/(ZMAX-ZMIN)
        Y  =350.*(1.+ABS(AP(I))*APFAC/YMPL)
        X  =350.*(1.-ABS(AP(I))*APFAC/XMPL)
        IF(X.LT.0.0 .OR. Y.GT.700.0) GO TO 12
        IF(TITEL(I).EQ.'*BEND*' .AND. AP(I).GT.0.) X=25.
        IF(TITEL(I).EQ.'*BEND*' .AND. AP(I).LT.0.) Y=675.
        WRITE(8) Z1,Z2,X,Y
   12   CONTINUE
C
C   SLITS
C
        IF (SLITS) THEN
        Z1=2000.
        WRITE(8) Z1,Z2,X,Y
        DO 112 I=1,INDMAX
        IF(TITEL(I).NE.'*SLIT*') GO TO 112
        IF(Z(I).GT.ZMAX .OR. Z(I).LT.ZMIN) GO TO 112
        Z1 =1000.*(Z(I)-ZMIN)/(ZMAX-ZMIN)
        Y  =350.*(1.+AP(I)*APFAC/YMPL)
        IF(AP(I).EQ.0.0) Y=1000.
        X  =350.*(1.-ANG(I)*APFAC/XMPL)
        IF(ANG(I).EQ.0.0) X=0.
        IF(X.LT.0.0 .AND. Y.GT.700.0) GO TO 112
        WRITE(8) Z1,X,Y
  112   CONTINUE
        ENDIF
        CLOSE(UNIT=8)
C
C   BESCHRIFTUNG DES DISPLAYS
C
        CALL GETDAT(DAT)
        CALL GETTIM(TIM)
        OPEN(UNIT=9,FILE='FOR009.DAT',FORM='FORMATTED',STATUS='UNKNOWN')
        REWIND 9
        WRITE(TITLE,1024) ZMIN,DIM1,ZMAX,DIM1,XMPL,DIM2,YMPL,DIM2,APFAC
     1      ,DAT(1:9),TIM(1:8)
        WRITE(9,1001) TITLE
C
C   TITEL DES DISPLAYS
C
        OPEN(UNIT=1,FILE='FOR001.DAT',STATUS='OLD')
        READ(1,1001) UEBER
        CLOSE(UNIT=1)
        DO 50 I=1,83
        IF(UEBER(I:I).EQ.'/') UEBER(I:I)=' '
        IF(UEBER(I:I+5).EQ.'*PLOT*') UEBER(I:I+5)='      '
   50   CONTINUE
        DO 52 I=1,83
        IF(UEBER(I:I).NE.' ') GO TO 53
   52   CONTINUE
   53   NNN=I-1
        DO 54 I=1,83-NNN
        UEBER(I:I)=UEBER(I+NNN:I+NNN)
   54   CONTINUE
        UEBER(84-NNN:83)=' '
        WRITE(9,1001) UEBER
        WRITE(9,1022) IYTIT
C
C   ANSCHREIBEN DER MAGNETE UND LINSEN
C
        WRITE(9,1022) ISPEZ2
        DO 220 I=1,INDMAX
        IF(TITEL(I).NE.'*BEND*'.AND.TITEL(I).NE.'*QUAD*'.AND.
     1     TITEL(I).NE.'*EQUAD*'.AND.
     2     TITEL(I).NE.'*SOLO*'.AND.TITEL(I).NE.'*SEXT*'.AND.
     3     TITEL(I).NE.'*WIEN*'.AND.TITEL(I).NE.'*ACC*') GO TO 220
        ZP=1000.*(Z(I)-ZMIN-LENG(I)/2.)/(ZMAX-ZMIN)
  215   IF(ZP) 220,217,217
  217   IF(Z(I)-ZMAX) 216,216,220
  216   IF(LABEL(I).NE.'    ') THEN
           WRITE(BASCII,'(F10.5)') B(I)
           IF (TITEL(I).EQ.'*BEND*' .OR.
     X         TITEL(I).EQ.'*WIEN*') THEN ! BRING IN RED/BLUE COLOR INFO
            BASCII(10:10) = '1'  ! ONLY 3 DIGITS AFTER . ARE USED
           ELSE
            BASCII(10:10) = '0'  ! FOR FIELD STRNGTHS
           ENDIF
           WRITE(9,1117) ZP,LABEL(I),BASCII
        ENDIF
  220   CONTINUE
C
C   ANSCHREIBEN DER BLENDEN
C
        IF (SLITS) THEN
        ZP=2000.
        WRITE(9,1017) ZP
        DO 240 I=1,INDMAX
        IF(TITEL(I).NE.'*SLIT*') GO TO 240
        IF(Z(I).GT.ZMAX .OR. Z(I).LT.ZMIN) GO TO 240
        ZP=1000.*(Z(I)-ZMIN)/(ZMAX-ZMIN)
        IF(LABEL(I).NE.'    ') WRITE(9,1017) ZP,LABEL(I)
  240   CONTINUE
        ENDIF
        CLOSE(UNIT=9)
C
C   EINZEICHNEN VON MONITOREN UND SONDEN (DRIFT)
C
        OPEN(UNIT=10,FILE='FOR010.DAT',FORM='UNFORMATTED',
     1       STATUS='UNKNOWN')
        REWIND 10
        WRITE(10) ISPEZ,ILABS
        DO 230 I=1,INDMAX
        IF(TITEL(I).NE.'*DRIFT*') GO TO 230
        IF(LABEL(I).EQ.'    ') GO TO 230
        ZP=1000.*(Z(I)-ZMIN)/(ZMAX-ZMIN)
        IF(ZP) 230,231,231
  231   IF(Z(I)-ZMAX) 234,234,230
  234   WRITE(10) ZP,LABEL(I)
  230   CONTINUE
        CLOSE(UNIT=10)
        IF(.NOT.IT) GO TO 400
        WRITE (6,2003)
  400   CONTINUE
        WRITE(*,*) 'Plot files and data tables written.'
        RETURN
  800   WRITE(*,*) 'Missing *PLOT* code on last title card'
        WRITE(*,*) 'or wrong type code 13. cards in input file.'
        CALL EXIT(1)
  777   WRITE(*,*) 'You have to run Transport first.'
        CALL EXIT(2)
  778   WRITE(*,*) 'Display file (FOR004.DAT) not found.'
        CALL EXIT(3)
C
 1000   FORMAT(' Interpolation of envelopes from',F8.3,1X,A2,' to'
     X        ,F8.3,1X,A2,' is inaccurate.')
 1001   FORMAT(A)
 1017   FORMAT(F10.1,6X,A4,1X,F10.5)
 1117   FORMAT(F10.1,6X,A4,1X,A10)
 1020   FORMAT(A10,F10.0)
 1022   FORMAT(I10)
 1024   FORMAT(' Zmin=',F6.2,1X,A2,' Zmax=',F6.2,1X,A2,' Xmax=',F5.1,
     X 1X,A2,' Ymax=',F5.1,1X,A2,' Ap *',F5.2,1X,A9,1X,A8)
 2003   FORMAT (10X,'more than 2000 elements.')
 3000   FORMAT(A4)
 3001   FORMAT(A2)
        END

        SUBROUTINE FILLIN(ZS,SIG,RM,Z,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,
     *  B1,B2,B3,B4,B5,B6,B7,B8,B9,B10,B11,B12,B13,C1,C2,C3,D1,D2)
C
        DIMENSION SIG(17),RM(13)
        Z=ZS
        A1=SIG(1)
        A2=SIG(2)
        A3=SIG(3)
        A4=SIG(4)
        A5=SIG(7)
        A6=SIG(8)
        A7=SIG(9)
        A8=SIG(10)
        A9=SIG(13)
        A10=SIG(14)
C
        B1=RM(1)
        B2=RM(2)
        B3=RM(3)
        B4=RM(4)
        B5=RM(5)
        B6=RM(6)
        B7=RM(7)
        B8=RM(8)
        B9=RM(9)
        B10=RM(10)
        B11=RM(11)
        B12=RM(12)
        B13=RM(13)
C
C       LONGITUDINAL BEAM PARAMETERS
C
        C1=SIG(11)
        C2=SIG(12)
        C3=SIG(15)
C
C       R13 AND R24 CORRELATIONS
C
        D1=SIG(16)
        D2=SIG(17)
C
        RETURN
        END
