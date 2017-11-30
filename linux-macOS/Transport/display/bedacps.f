      SUBROUTINE BEDACPS
C
C     COMPILED BY U. ROHRER (SIN) MAY, 1987
C     PLOTS BEAM AND QUAD-DACS BESIDES TRANSPORT ENVELOPES
C     IF DESIRED WITH PD (was PX before)
C     MODIFIED FOR LN03-PLUS OUTPUT BY URS ROHRER (PSI), MAY 1990
C
      CHARACTER*4  QTYP(100), QLAB(100), LABEL, SAVE4
      DIMENSION    A0(100),A1(100),A2(100),A3(100),DMAX(100)
      REAL         J0(100),JMAX(100)
      CHARACTER*10 TITEL
      DIMENSION    LEFF(100), JA(100)
      DIMENSION    QFAC(100), QOFF(100), QAMP(100)
      CHARACTER*12 LINE(0:13), QLIN(0:100)
      CHARACTER    DIM1*2, DIM2*2, DIM3*2, DIM5*4
      DATA QLIN(0) /'QUAD     DAC'/
      DATA LINE /'INITIAL BEAM', 'z      0.000',
     1      'x      0.000', 'x''     0.000',
     2      'r12   -0.000', 'epsx   0.000',
     3          'y      0.000', 'y''     0.000',
     4      'r34   -0.000', 'epsy   0.000',
     5      'R16  -20.000', 'R26  -20.000',
     6      'p    1.00000', 'dp/p   0.000'/
C
C     BEAM
C
      OPEN (UNIT=12,STATUS='UNKNOWN',FILE='FOR012.DAT',
     1             FORM='UNFORMATTED',ERR=600)
      READ (12,ERR=600) Z, X, PX, R12, Y, PY, R34, D, DP,
     1                 P, DELP, DIM1, DIM2, DIM3, DIM5
      EPSX = X * PX * SQRT(ABS(1.-R12**2))
      EPSY = Y * PY * SQRT(ABS(1.-R34**2))
      WRITE(LINE(1)(5:12), 2000) Z
      WRITE(LINE(2)(5:12), 2000) X
      WRITE(LINE(3)(5:12), 2000) PX
      WRITE(LINE(4)(5:12), 2000) R12
      WRITE(LINE(5)(5:12), 2000) EPSX
      WRITE(LINE(6)(5:12), 2000) Y
      WRITE(LINE(7)(5:12), 2000) PY
      WRITE(LINE(8)(5:12), 2000) R34
      WRITE(LINE(9)(5:12), 2000) EPSY
      WRITE(LINE(10)(5:12),2000) D
      WRITE(LINE(11)(5:12),2000) DP
      WRITE(LINE(12)(2:12),2001) P
      WRITE(LINE(13)(5:12),2000) DELP
      CLOSE(UNIT=12)
 2000 FORMAT(F8.3)
 2001 FORMAT(F11.5)
C
C     QUAD CALIBRATION
C
      OPEN(UNIT=50,FILE='QUADS.CAL',STATUS='OLD',ERR=600)
      K = 1
 50   CONTINUE
      IF (K.GT.100) STOP 'FILE QUADS.CAL OVERFLOW'
      READ(50,3000,END=60,err=600)
     1  QTYP(K),QLAB(K),LEFF(K),JA(K),A0(K),A1(K),A2(K),A3(K),
     2  J0(K),JMAX(K),DMAX(K)
 3000 FORMAT(1X,A4,2X,A4,2X,I4,2X,I4,7F7.1)
      K = K + 1
      GO TO 50
 60   NREC = K - 1
      CLOSE (UNIT=50)
C
      K     = 0
      I     = 0
      SAVE1 = 0.
      SAVE2 = 0.
      ISAV3 = 0.
      SAVE4 = '   '
      OPEN(UNIT=11,STATUS='OLD',FILE='FOR011.DAT',
     1        FORM='UNFORMATTED',ERR=600)
C
C     DAC VALUES
C
 200  READ(11,END=300,ERR=600) TITEL,LABEL,XL,DIM1,AP,DIM2,B,DIM3,XN
      IF (TITEL.NE.'*QUAD*' .AND. TITEL.NE.'*SOLO*'
     1                      .AND. TITEL.NE.'*SEXT*'
     2                      .AND. TITEL.NE.'*EQUAD*') GO TO 200
      I = I + 1
      IF (TITEL.EQ.'*SOLO*' .OR. TITEL.EQ.'*SEXT*') GO TO 200
      IXL = NINT(XL*1000.)
      IAP = NINT(AP*10.)
      XA0 = 0.0
      XA1 = 1.0
      XA2 = 0.0
      XA3 = 0.0
      XJ0 = 1000.0
      XJMAX = 1.0
      XDMAX = 1000.0
      DO 205 J=1,NREC
       IF (LABEL .EQ. QLAB(J)) THEN
        XA0 = A0(J)
        XA1 = A1(J)
        XA2 = A2(J)
        XA3 = A3(J)
        XJ0 = J0(J)
        XJMAX = JMAX(J)
        XDMAX = DMAX(J)
            GO TO 210
       ENDIF
       IF (IAP.EQ.JA(J).AND.IXL.EQ.LEFF(J).AND.QLAB(J).EQ.'%%%%') THEN
        XA0 = A0(J)
        XA1 = A1(J)
        XA2 = A2(J)
        XA3 = A3(J)
        XJ0 = J0(J)
        XJMAX = JMAX(J)
        XDMAX = DMAX(J)
            GO TO 210
       ENDIF
 205   CONTINUE
 210   IF (XA1 .EQ. 0.0) GO TO 200
C
       CALL GETDAC(B,IDAC,AMPS,XA0,XA1,XA2,XA3,XJ0,XJMAX,XDMAX)

      IF (AP.NE.SAVE1 .OR. XL.NE.SAVE2 .OR. IDAC.NE.ISAV3 .OR.
     1   SAVE4.NE.LABEL) K = K + 1
      QLIN(K)(1:4) = LABEL
C
      WRITE(QLIN(K)(5:12),5000) IDAC
 5000 FORMAT(I8)
      SAVE1  = AP
      SAVE2  = XL
      ISAV3  = IDAC
      SAVE4  = LABEL
      GO TO 200
C
 300  CONTINUE
      CLOSE(UNIT=11)
C
C     GRAPHIC OUTPUT
C
      Y = 690.
      DO 400 J = 0, 13
       Y = Y - 12.
       CALL PSTEXT(1005.,Y,LINE(J))
 400   CONTINUE
      Y = Y - 12.
      DO 500 J = 0, K
       Y = Y - 12.
       CALL PSTEXT(1005.,Y,QLIN(J))
 500   CONTINUE
 600   RETURN
      END
