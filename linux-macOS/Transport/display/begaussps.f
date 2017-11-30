      SUBROUTINE BEGAUSSPS
C
C     COMPILED BY U. ROHRER (SIN) FEB, 2005
C     PLOTS BEAM AND QUAD-GAUSS-VALUES BESIDES TRANSPORT ENVELOPES
C     IF DESIRED WITH PX
C
      CHARACTER*4  LABEL
      CHARACTER*10 TITEL
      CHARACTER*12 LINE(0:13), QLIN(0:100)
      CHARACTER    DIM1*2, DIM2*2, DIM3*2, DIM5*4
      DATA QLIN(0) /'QUAD   Gauss'/
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
 2000    FORMAT(F8.3)
 2001    FORMAT(F11.5)
C
      I     = 0
      OPEN(UNIT=11,STATUS='OLD',FILE='FOR011.DAT',
     1        FORM='UNFORMATTED',ERR=600)
C
C     GAUSS VALUES
C
 200  READ(11,END=300,ERR=600) TITEL,LABEL,XL,DIM1,AP,DIM2,B,DIM3,XN
      IF (TITEL.NE.'*QUAD*' .AND. TITEL.NE.'*SOLO*'
     1                     .AND. TITEL.NE.'*SEXT*'
     2                     .AND. TITEL.NE.'*EQUAD*') GO TO 200
      IF (TITEL.EQ.'*SOLO*' .OR. TITEL.EQ.'*SEXT*') GO TO 200
C
      I = I + 1
      QLIN(I)(1:4) = LABEL
      IGAUSS = NINT(B*1000.)
      WRITE(QLIN(I)(5:12),5000) IGAUSS
 5000 FORMAT(I8)
      GO TO 200
C
 300  IMAX = I
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
      DO 500 J = 0, IMAX
       Y = Y - 12.
       CALL PSTEXT(1005.,Y,QLIN(J))
 500  CONTINUE
 600  RETURN
      END
