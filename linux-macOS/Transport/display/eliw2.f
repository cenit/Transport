      SUBROUTINE ELIW2(SCALXX,SCALYY)
C
C   PLOTS BEAM ELLIPSES
C   ADAPTED FOR WORKSTATION-UIS BY U.ROHRER (PSI), JANUARY 1989
C       For longitudinal phase space by U. Rohrer (PSI), Apr. 2005
C
      PARAMETER (LDM=2000)
      CHARACTER*80 TEXT
      CHARACTER*20 INPUT
      CHARACTER*21 MENUE(11)
      CHARACTER*10 TITEL,LABS
      CHARACTER*2 DIM1,DIM2,DIM3,DIM4,DIM7
      CHARACTER*4 LABEL,LAB,DIM5,DIM6,LABI(LDM),LABF
      REAL X1(101),X2(101),Y1(101),Y2(101)
      REAL LWX,LWY
      DIMENSION
     *  Z(LDM),XMAX(LDM),YMAX(LDM),D(LDM),DPY(LDM),ANG(LDM),
     *  DY(LDM),LENG(LDM),AP(LDM),B(LDM),PXMAX(LDM),R12(LDM),
     *  PYMAX(LDM),DP(LDM),R34(LDM),X0(LDM),Y0(LDM),X0P(LDM),
     *  Y0P(LDM),CX(LDM),SX(LDM),CPX(LDM),SPX(LDM),CY(LDM),
     *  SY(LDM),CPY(LDM),SPY(LDM),XN(LDM),TITEL(LDM),LAB(LDM)
C
      DIMENSION
     *  LABVS(LDM),R66(LDM),DLMAX(LDM),DPPMAX(LDM),R56(LDM)
      DIMENSION
     *  X0A(100),X0PA(100),Y0A(100),Y0PA(100),LABS(100),
     *  ZZ(200),F1(200),F2(200),CON(200),DEL(200),VAL(200),LABF(200),
     *  CHI(50),IPOINT(50),IREM(50),FAIL(50)
      LOGICAL FAIL,SLITS
C
      COMMON /EP/ Z,DLMAX,DPPMAX,R56,LAB,DIM1,DIM3,DIM7
C
      DIMENSION SCAL(4),FSCAL(4),OFFX(4),OFFY(4),NRJ(4)
      EQUIVALENCE(SCAL(1),XSCAL), (SCAL(2),YSCAL),
     1             (SCAL(3),PXSCAL),(SCAL(4),PYSCAL)
      INTEGER SIZE
C
c     DATA OFFX /250.,1270.,250.,1270./
      DATA OFFX /250.,760.,250.,760./
      DATA OFFY /1100.,1100.,325.,325./
      DATA PI   /3.14159/
      DATA NNR  /1/
C
      SCALX = 1.0 * SCALXX
      SCALY = 1.0 * SCALYY
      SIZE  = 3
      SIZE  = IFONTSIZE2(SCALX)
C
C   MENU ENTRIES
C
      MENUE(1)  = 'Help             HELP'
      MENUE(2)  = '1 Frame/page       *1' 
      MENUE(3)  = '2 Frames/page      *2'
      MENUE(4)  = '3 Frames/page      *3'
      MENUE(5)  = '4 Frames/page      *4'
      MENUE(6)  = 'Print w Dev1 (GS)  P1'
      MENUE(7)  = 'Print w Dev2 (GS)  P2'
      MENUE(8)  = 'PS Laser Copy       L'
      MENUE(9)  = 'Next z           <CR>'
      MENUE(10) = 'Previous z         -1'
      MENUE(11) = 'Quit           CNTR_Z'
C
C   LESEN VON FILE 15
C
      OPEN(UNIT=15,FILE='FOR015.DAT',STATUS='OLD',
     1       FORM='UNFORMATTED')
      READ(15,ERR=77) DIM1,DIM2,DIM3,DIM4,DIM5,DIM6,P,DELP,INDMAX
     1,IND,IN,IS,ICH,CHI,IPOINT,SAVE1,SAVE2,SAVE3
      READ(15) (TITEL(I),LAB(I),Z(I),XMAX(I),YMAX(I),D(I)
     1,DY(I),LENG(I),AP(I),B(I),PXMAX(I),R12(I),DPY(I),PYMAX(I)
     2,DP(I),R34(I),X0(I),Y0(I),X0P(I),Y0P(I),CX(I),SX(I),CPX(I)
     3,SPX(I),CY(I),SY(I),CPY(I),SPY(I),XN(I),ANG(I),I=1,INDMAX)
      READ(15) (LABF(I),ZZ(I),F1(I),F2(I),CON(I),DEL(I),VAL(I)
     1,I=1,IN)
      READ(15) (X0A(I),X0PA(I),Y0A(I),Y0PA(I),LABS(I),I=1,IS)
      READ(15) JMAX,ICH
      READ(15) (LABVS(J),J=1,JMAX),(IREM(K),K=1,ICH)
     1,(FAIL(L),L=1,ICH)
      READ(15) SLITS
      READ(15) (R66(I),I=1,INDMAX)
      READ(15) (DLMAX(I),DPPMAX(I),R56(I),I=1,INDMAX)
      READ(15) DIM7
      CLOSE(UNIT=15)
C
      IF(DIM1.EQ.'M ') DIM1='m '
      IF(DIM2.EQ.'MM') DIM2='mm'
      IF(DIM2.EQ.'CM') DIM2='cm'
      J = 0
C
C   FIND USER CHOICE
C
      CALL SET_COL(1) ! WEISS
 100  CONTINUE
      LABEL='%%%%'
      INPUT = '                    '
      CALL GETINP(20.*SCALXX,765.*SCALYY,
     1           'Input or HELP<CR> '//char(0),INPUT)
C
C     get eventual new window dimension and save it on file
C
      call getwindim(nw,nh)
      OPEN(UNIT=49,FILE='.dispoff',STATUS='OLD',ERR=1112)
      READ(49,'(4I6)',ERR=1112) IXOFF,IYOFF,IWIDTH,IHEIGHT
      REWIND(UNIT=49)
      WRITE(49,'(4I6)') IXOFF,IYOFF,2*nw,nh
      CLOSE(UNIT=49)
 1112 CONTINUE
C
C   MENU
C
      IF (INPUT(1:6) .EQ. 'MOUSE1') THEN
       READ(INPUT(7:20),'(2F7.1)') XX1, YY1
       NX = 21
       NY = 11
       NR = MENU(XX1,YY1,NX,NY,MENUE)
       IF (NR .EQ. 1)  INPUT(1:4)  = 'HELP'
       IF (NR .EQ. 2)  INPUT(1:2)  = '*1'
       IF (NR .EQ. 3)  INPUT(1:2)  = '*2'
       IF (NR .EQ. 4)  INPUT(1:2)  = '*3'
       IF (NR .EQ. 5)  INPUT(1:2)  = '*4'
       IF (NR .EQ. 6)  INPUT(1:2)  = 'P1'
       IF (NR .EQ. 7)  INPUT(1:2)  = 'P2'
       IF (NR .EQ. 8)  INPUT(1:1)  = 'L'
       IF (NR .EQ. 9)  INPUT(1:20) = '                    '   
       IF (NR .EQ. 10) INPUT(1:20) = '-1                  '   
       IF (NR .EQ. 11) GO TO 2500
      ENDIF
C
C   POSTSCRIPT LASER COPY
C
      IF (INPUT(1:1).EQ.'L' .OR. INPUT(1:1).EQ.'l') THEN
       CALL VT220_POP
       CALL ELIPS2(NNR,NRJ,SCALX/SCALXX,SCALY/SCALYY,SIZE,.FALSE.)
       CALL SYSTEM('./print bw TRANS.PS')
C      CALL ERASE
       CALL VT220_PUSH
       GO TO 100
      ENDIF
C
C       POSTSCRIPT LASER COPY IN COLOR
C
      IF ( (INPUT(1:1).EQ.'C' .OR. INPUT(1:1).EQ.'c') .AND.
     1       (INPUT(2:2).NE.'O') ) THEN
       write(6,'(A)') INPUT(2:2)
       CALL VT220_POP
       CALL ELIPS2(NNR,NRJ,SCALX/SCALXX,SCALY/SCALYY,SIZE,.TRUE.)
       CALL SYSTEM('./print c TRANS.PS')
C      CALL ERASE
       CALL VT220_PUSH
       GO TO 100
      ENDIF
C
C       PRINT VIA LOCAL PRINT DEVICE 1
C
      IF (INPUT(1:2).EQ.'P1' .OR. INPUT(1:2).EQ.'p1') THEN
       CALL VT220_POP
       CALL ELIPS2(NNR,NRJ,SCALX/SCALXX,SCALY/SCALYY,SIZE,.TRUE.)
       CALL SYSTEM('./print dev1 TRANS.PS')
       CALL VT220_PUSH
C      CALL ERASE
       GO TO 100
      ENDIF
C
C       PRINT VIA LOCAL PRINT DEVICE 2
C
      IF (INPUT(1:2).EQ.'P2' .OR. INPUT(1:2).EQ.'p2') THEN
       CALL VT220_POP
       CALL ELIPS2(NNR,NRJ,SCALX/SCALXX,SCALY/SCALYY,SIZE,.FALSE.)
       CALL SYSTEM('./print dev2 TRANS.PS')
       CALL VT220_PUSH
C      CALL ERASE
       GO TO 100
      ENDIF
C
C   HELP
C
      IF (INPUT(1:4).EQ.'HELP' .OR. INPUT(1:4).EQ.'help') THEN

      PRINT *,'Legal inputs are:'
      PRINT *,
     1'z-coordinate in meters or desired label at which the (x,x'')-'
      PRINT *,
     1'and (y,y'')-ellipses have to be drawn.  Enter <CR> or -1<CR>'
      PRINT *,
     1'for drawing the ellipses of the next or of the previous',
     2' element'
      PRINT *,
     1'respectively. Entering *n<CR> (with 1 < n < 5) means that',
     2' up to n'
      PRINT *,
     1'previously displayed or the next n selected ellipse paires',
     2' will be'
      PRINT *,
     1'drawn together on screen with half the size.  *1<CR>',
     2' restores the'
      PRINT *,
     1'standard representation. Enter HALT or QUIT to exit the',
     2' program.'
      PRINT *,
     1'In case the input is illegal, the following message appears:'

      GO TO 2000
      ENDIF
C
C   HOW MANY ELLIPSE-PAIRES ON SCREEN ?
C
      IF (INPUT(1:2) .EQ. '*1') THEN
       SCALX = SCALXX * 1.
       SCALY = SCALXX * 1.
       SIZE  = 3
       SIZE  = IFONTSIZE2(SCALX)
       NNR   = 1
       IF (NRJ(1) .NE. 0) GO TO 6
       GO TO 100
      ELSE IF (INPUT(1:2) .EQ. '*2') THEN
       SCALX = SCALXX * .5
       SCALY = SCALXX * .5
       SIZE  = 2
       SIZE  = IFONTSIZE2(SCALX)
       NNR   = 2
       IF (NRJ(1) .NE. 0) GO TO 6
       GO TO 100
      ELSE IF (INPUT(1:2) .EQ. '*3') THEN
       SCALX = SCALXX * .5
       SCALY = SCALXX * .5
       SIZE  = 2
       SIZE  = IFONTSIZE2(SCALX)
       NNR   = 3
       IF (NRJ(1) .NE. 0) GO TO 6
       GO TO 100
      ELSE IF (INPUT(1:2) .EQ. '*4') THEN
       SCALX = SCALXX * .5
       SCALY = SCALXX * .5
       SIZE  = 2
       SIZE  = IFONTSIZE2(SCALX)
       NNR   = 4
       IF (NRJ(1) .NE. 0) GO TO 6
       GO TO 100
      ELSE
       CONTINUE
      ENDIF
C
      IF (INPUT(1:4).EQ.'quit' .OR.
     1    INPUT(1:4).EQ.'QUIT' .OR.
     2    INPUT(1:9).EQ.'CONTROL_Z') GO TO 2500
C
      IF (INPUT(1:6).EQ.'MOUSE2')
     1    INPUT(1:20) = '                    '
C
      IF (INPUT(1:6).EQ.'MOUSE3')
     1    INPUT(1:20) = '-1.                 '
C
      READ(INPUT,4002,ERR=200) ZS
 4002 FORMAT(F20.1)
C
C   DECREMENTAL STEP
C
      IF (ZS .LT. 0.) THEN
       J = J - 1
        IF (J .LT. 1) J = INDMAX
       GO TO 5
C
C   INCREMENTAL STEP
C
      ELSE IF (ZS .EQ. 0.) THEN
       J = J + 1
        IF (J .GT. INDMAX) J = 1
       GO TO 5
C
C   FINDE GESUCHTES Z
C
      ELSE
       GO TO 1
      END IF
C
 200  READ(INPUT,4003) LABEL
      ZS=9999.
 4003 FORMAT(A4)
C
 1    DO 2 J = 1, INDMAX
      IF (LAB(J) .EQ. LABEL) GO TO 5
      IF (Z(J) .GE. ZS) GO TO 5
 2    CONTINUE
      GO TO 2000
C
C   KEEP LAST 3 J'S WITH NEW J
C
 5    CONTINUE
      DO 98 I  = 3, 1, -1
 98   NRJ(I+1) = NRJ(I)
      NRJ(1)   = J
      J1       = J
C
C     DISPLAY 4 ELLIPSE PAIRES
C
 6    DO 97 NJ = 1, NNR
      JJJ = NRJ(NJ)
      IF (JJJ .EQ. 0) GO TO 97
      J = JJJ
C
C     AUTOSCALING
C
      M=DLMAX(J)*10.+1.001
      XSCAL=FLOAT(M)/10.+.00001
      IF(XSCAL.LT.0.1) XSCAL=.1
      M=DPPMAX(J)*10.+1.001
      PXSCAL=FLOAT(M)/10.+.00001
      IF(PXSCAL.LT.0.1) PXSCAL=.1

      CH1=ASIN(R56(J))
      X1M=200.*DLMAX(J)/XSCAL
      Y1M=200.*DPPMAX(J)/PXSCAL
C
C     BERECHNE SKALENABSCHNITTE
C
      DO 110 I=1,4
       X=SCAL(I)
       DO 99 JX=1,10
        IF(X.LT.1.0) X=X*10.
        IF(X.GT.10.0) X=X/10.
 99    CONTINUE
      ABSCH=200./X
      IIX=X
      IF(IIX.EQ.1) ABSCH=ABSCH/5.
      IF(IIX.EQ.2) ABSCH=ABSCH/2.
      IF(IIX.EQ.3) ABSCH=ABSCH/2.
      FSCAL(I)=ABSCH
 110  CONTINUE
C
C   KONSTRUIERE ELLIPSEN
C
      DO 10 I=1,101
       DE=2.*(I-1)*PI/100.
       COSD=COS(DE)
       X1(I)=X1M*COSD
       Y1(I)=Y1M*SIN(DE+CH1)
 10   CONTINUE
C
C   EMITTANZ
C
      EX=DLMAX(J)*DPPMAX(J)*SQRT(1.-R56(J)**2)
C
C   WINKEL CHIX
C
      CHIX=180./PI*ASIN(R56(J))
C
C   ZEICHNE ELLIPSEN
C
      OFFX1 = OFFX(NJ)
      OFFY1 = OFFY(NJ)
       IF (NNR .EQ. 1) THEN
        OFFX1 = OFFX(3)
        OFFY1 = OFFY(3)
       ENDIF
      OFFX2 = OFFX1+500.
      BEGX1 = OFFX1-200.
      ENDX1 = OFFX1+200.
      BEGX2 = OFFX2-200.
      ENDX2 = OFFX2+200.
      BEGY1 = OFFY1-200.
      ENDY1 = OFFY1+200.
      BEGT1 = BEGX1-30.
      BEGT2 = BEGX2-30.

      IF (NJ .EQ. 1) CALL ERASE
      CALL SET_COL(3) ! GRUEN
      CALL GRAMOV((OFFX1+X1(1))*SCALX,(OFFY1+Y1(1))*SCALY)
      DO 15 I=2,101
      CALL GRADRW((OFFX1+X1(I))*SCALX,(OFFY1+Y1(I))*SCALY)
 15   CONTINUE
C
C   KONJUGIERTE DURCHMESSER
C
      XK1=R56(J)*X1M
      XK2=R56(J)*Y1M
      XK3=R56(J)*X2M
      XK4=R56(J)*Y2M
      XL1=X1M
      XL2=Y1M
      XL3=X2M
      XL4=Y2M
      CALL GRAMOV((OFFX1+XK1)*SCALX,(OFFY1+XL2)*SCALY)
      CALL GRADRW((OFFX1-XK1)*SCALX,(OFFY1-XL2)*SCALY)
      CALL GRAMOV((OFFX1+XL1)*SCALX,(OFFY1+XK2)*SCALY)
      CALL GRADRW((OFFX1-XL1)*SCALX,(OFFY1-XK2)*SCALY)
C
C   ZEICHNE ACHSEN
C
      CALL SET_COL(2) ! ROT
      CALL GRAMOV(BEGX1*SCALX,OFFY1*SCALY)
      CALL GRADRW(ENDX1*SCALX,OFFY1*SCALY)
      CALL GRAMOV(OFFX1*SCALX,BEGY1*SCALY)
      CALL GRADRW(OFFX1*SCALX,ENDY1*SCALY)
C
C   ZEICHNE ACHSEN-ABSCHNITTE
C
      DO 300 K=1,10
      XX1=(OFFX1+FSCAL(1)*FLOAT(K))*SCALX
      XX2=(OFFX1-FSCAL(1)*FLOAT(K))*SCALX
      IF(XX1.GT.ENDX1*SCALX) GO TO 300
      IF(XX2.LT.BEGX1*SCALX) GO TO 300
      CALL GRAMOV(XX1,(OFFY1-3.)*SCALY)
      CALL GRADRW(XX1,(OFFY1+3.)*SCALY)
      CALL GRAMOV(XX2,(OFFY1-3.)*SCALY)
      CALL GRADRW(XX2,(OFFY1+3.)*SCALY)
 300  CONTINUE

      DO 302 K=1,10
      YY1=(OFFY1+FSCAL(3)*FLOAT(K))*SCALY
      YY2=(OFFY1-FSCAL(3)*FLOAT(K))*SCALY
      IF(YY1.GT.ENDY1*SCALY) GO TO 302
      IF(YY2.LT.BEGY1*SCALY) GO TO 302
      CALL GRAMOV((OFFX1-3.)*SCALX,YY1)
      CALL GRADRW((OFFX1+3.)*SCALX,YY1)
      CALL GRAMOV((OFFX1-3.)*SCALX,YY2)
      CALL GRADRW((OFFX1+3.)*SCALX,YY2)
 302  CONTINUE

      CALL GRAMOV(0.,0.)
C
C   BESCHRIFTE ACHSEN
C
      CALL TEXT_OUT('dp/p'//char(0),(OFFX1+5.)*SCALX,
     1                              (ENDY1+10.)*SCALY,SIZE)
      CALL TEXT_OUT('dl'//char(0),  (ENDX1+5.)*SCALX,OFFY1*SCALY,SIZE)
C
C   ZEICHNE PARAMETER
C
      CALL SET_COL(1) ! WEISS

      WRITE(TEXT,1006) CHIX, R56(J), char(0)
 1006 FORMAT('Chi=',F7.1,' Deg',' (',F7.4,')',A1)
      CALL TEXT_OUT(TEXT,BEGT1*SCALX,(ENDY1+85.)*SCALY,SIZE)

      WRITE(TEXT,1008) EX, DIM7, DIM3, char(0)
 1008 FORMAT('Emit=',F6.2,' * Pi ',A2,A2,A1)
      CALL TEXT_OUT(TEXT,BEGT1*SCALX,(ENDY1+50.)*SCALY,SIZE)

      WRITE(TEXT,1013) XSCAL,DIM7,PXSCAL,DIM3,char(0)
 1013 FORMAT('dlmax=',F5.1,1X,A2,'  (dp/p)max=',F5.1,1X,A2,A1)
      CALL TEXT_OUT(TEXT,BEGT1*SCALX,(BEGY1-30.)*SCALY,SIZE)

      WRITE(TEXT,1010) Z(J),DIM1,LAB(J),char(0)
 1010 FORMAT('z= ',F7.3,1X,A2,2X,A4,A1)
      CALL TEXT_OUT(TEXT,BEGT1*SCALX,(BEGY1-100.)*SCALY,SIZE)
C
 97   CONTINUE
      J = J1
C
C   FINITO
C
      GO TO 100
C
C   FEHLERMELDUNG
C
 2000 CALL VT220_POP
      WRITE(6,2001) Z(INDMAX)
 2001 FORMAT(' Z > Zmax =',F8.3,' m or none of listed labels found:')
      JJ=0
      DO 550 II=1,INDMAX
      IF (LAB(II) .NE. ' ') THEN
       JJ=JJ+1
       LABI(JJ)=LAB(II)
      ENDIF
 550  CONTINUE
      WRITE(6,2002) (LABI(II),II=1,JJ)
 2002 FORMAT(12(1X,A4,1X))
      WRITE(6,2003)
 2003 FORMAT(' <CR> for continue ',$)
      READ(5,2004) IJK
 2004 FORMAT(I1)
      CALL VT220_PUSH
      CALL ERASE
      GO TO 100
 77   WRITE(6,78)
 78   FORMAT(' Run functions T, S, TB or R first.')
C
C   FINISH
C
 2500 CONTINUE
C
      CALL VT220_POP
      RETURN
      END

      INTEGER FUNCTION IFONTSIZE2(SCALX)
C
      IFONTSIZE2 = NINT(5.0 * SCALX - 1.0)
      IF (IFONTSIZE2 .LT. 0) IFONTSIZE2 = 0
      IF (IFONTSIZE2 .GT. 3) IFONTSIZE2 = 3
C
      RETURN
      END

