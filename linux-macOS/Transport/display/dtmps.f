      SUBROUTINE DTMPS(IJK,XMIN,XMAX,IYU,LTR,COL)
C
C   DISPLAYS SECOND ORDER MATRIX ELEMENTS Tijk ON SCREEN
C
      COMMON /COLTAG/ COLOR, XXXX, YYYYY, XOOOO, YOOOO, CHCH
      LOGICAL         COLOR
C
      REAL MAT(105),XDIME(11)
      REAL X(500),Y(500,4),DY(500)
      CHARACTER*80 NAMEX
      CHARACTER*4 INF(4),DUM
      CHARACTER*13 EXP
      CHARACTER*20 IJK
      INTEGER*4 I(4),J(4),K(4)
      LOGICAL LTR, COL
      DATA EXP /'TMAX=        '/
C
      COLOR = COL
      XSCAL = 1.0
      YSCAL = 1.0
      SIZE  = 10. * YSCAL
C
C   FIND OUT WHICH OF THE Tijk HAS TO BE DISPLAYED (UP TO 4)
C
      INF(1)=IJK(1:4)
      INF(2)=IJK(5:8)
      INF(3)=IJK(9:12)
      INF(4)=IJK(13:16)
      N=0
      DO 10 L = 1, 4
      READ(INF(L),1013,ERR=10) I(L)
      READ(INF(L),1014,ERR=10) J(L)
      READ(INF(L),1015,ERR=10) K(L)
      IF(I(L)+J(L)+K(L) .EQ. 0) GO TO 11
      IF(I(L).GT.5 .OR. J(L).GT.6 .OR. K(L).GT.6) GO TO 10
      IF(J(L).EQ.0 .OR. K(L).EQ.0) GO TO 10
      IF(J(L).GT.K(L)) THEN
      LL=K(L)
      K(L)=J(L)
      J(L)=LL
      END IF
      N=N+1
      I(N)=I(L)
      J(N)=J(L)
      K(N)=K(L)
 10   CONTINUE
 11   IF(N.EQ.0) GO TO 999
C
C   READ IN  THE Tijk
C
      IV = 0
 200  CONTINUE
      IV = IV + 1
      IF (IV .EQ. 12) GO TO 600
      IF (IV .EQ. 1) THEN
       NAMEX = 'FOR000.DAT'
      ELSE
       NAMEX = 'FOR000.DAT.0'
           NAMEX(12:12) = CHAR(48+IV-2)
      ENDIF
      OPEN(UNIT=0,FILE=NAMEX,FORM='UNFORMATTED',
     1     STATUS='OLD',ERR=999)
      READ(0) XDIME
      II=1
      IF (IV .EQ. 1) YMAX=0.
      AUX=0.
 1    IF (II .GT. 500) GO TO 100
      READ(0,END=100) ITYPE,X(II),XLAB,TOTROT,MAT
      IF (ITYPE .EQ. 20) GO TO 1
      IROT = TOTROT
      X(II)=1000.*(X(II)-XMIN)/(XMAX-XMIN)*XSCAL
      DO 20 L=1, N
C
C   BACK ROTATION (ONLY FOR +-90 AND +- 180 DEGREES)
C
      SIG=1.
      IL=I(L)
      IF ((IROT.EQ.180 .OR. IROT .EQ. -180) .AND. I(L) .NE. 5) SIG=-1.
      IF (IROT .EQ. 90 .AND. (I(L) .EQ. 1 .OR. I(L) .EQ. 2)) SIG=-1.
      IF (IROT .EQ. -90 .AND. (I(L) .EQ. 3 .OR. I(L) .EQ. 4)) SIG=-1.
      IF (IROT .EQ. 90 .AND. I(L) .NE. 5) IL=I(L)+2-4*((I(L)+1)/4)
      IF (IROT .EQ. -90 .AND. I(L) .NE. 5) IL=I(L)+2-4*((I(L)+1)/4)
      IND=21*(IL-1)+(K(L)-1)*K(L)/2+J(L)
      AUX=SIG*MAT(IND)
      Y(II,L)=AUX
      YMAX=MAX(ABS(AUX),YMAX)
 20   CONTINUE
      II=II+1
      GO TO 1
 100  M=II-1
      IF (YMAX .EQ. 0) GO TO 999
      YMPL=YMAX*1.25
      SCALY=350./YMPL
C
C   DISPLAY SCALE
C
      IF (IV .EQ. 1) THEN
       CALL SETCOL(1)
       WRITE(EXP(6:13),1111) YMPL
       CALL PSTEXT(870.*XSCAL,705.*YSCAL,EXP)
      ENDIF
C
C     DISPLAY THE Tijk AS CURVES WITH SYMBOLS
C
      ICNT = 0
      DO 120 L=1, N
       ICOL = MOD(ICNT,4) + 1
       IF (ICOL .EQ. 4) ICOL = 5
       ICNT = ICNT + 1
       CALL SETCOL(ICOL)
       CALL LOGIC(LTR,M,1,J1,J2,J3)
       DO 110 II=J1,J2,J3
        YY= (350.+SCALY*Y(II,L))*YSCAL
        IF (II .EQ. J1) CALL PSMOV(X(J1),YY)
        CALL PSDRW(X(II),YY)
         IF (L .EQ. 1 .AND. N .GT. 1) THEN
          CALL CROSSPS(X(II),YY,SIZE)
         ELSE IF (L .EQ. 2) THEN
          CALL TRIUPPS(X(II),YY,SIZE)
         ELSE IF (L .EQ. 3) THEN
          CALL TRIDOPS(X(II),YY,SIZE)
         ELSE
          CALL SQUAREPS(X(II),YY,SIZE)
         ENDIF
       CALL PSMOV(X(II),YY)
 110   CONTINUE
       CALL REVERSE(LTR)
       CALL PSFLUSH
 120   CONTINUE
C
C     DISPLAY MEANING OF SYMBOLS
C
      IF (IV .NE. 1) GO TO 300
      YU=FLOAT(IYU)*YSCAL+4.0
      YYU=YU+SIZE/2.
      XXU=(940.-85.*FLOAT(N))*XSCAL
      DO 150 L=1, N
      XXU=XXU+85.*XSCAL
      INF(L)(1:1)='T'
      IF (L .EQ. 1 .AND. N .GT. 1) THEN
       CALL SETCOL(1)
       CALL CROSSPS(XXU,YYU,SIZE)
       CALL PSTEXT(XXU+15.*XSCAL,YU,INF(L))
      ELSE IF (L .EQ. 2) THEN
       CALL SETCOL(2)
       CALL TRIUPPS(XXU,YYU,SIZE)
       CALL PSTEXT(XXU+15.*XSCAL,YU,INF(L))
      ELSE IF (L .EQ. 3) THEN
       CALL SETCOL(3)
       CALL TRIDOPS(XXU,YYU,SIZE)
       CALL PSTEXT(XXU+15.*XSCAL,YU,INF(L))
      ELSE IF (L .EQ. 4) THEN
       CALL SETCOL(5)
       CALL SQUAREPS(XXU,YYU,SIZE)
       CALL PSTEXT(XXU+15.*XSCAL,YU,INF(L))
      ELSE
      END IF
 150   CONTINUE
 300   CONTINUE
      GO TO 200
 600   CONTINUE
C
C     FINISH
C
 999  CALL PSMOV(0.,0.)
      RETURN
C
C
 1002   FORMAT(4A4)
 1009   FORMAT(I10)
 1111   FORMAT(E8.2)
 1013   FORMAT(1X,I1,2X)
 1014   FORMAT(2X,I1,1X)
 1015   FORMAT(3X,I1)
 1020   FORMAT(6X,F6.2,9X,F6.2)
      END

      SUBROUTINE CROSSPS(X,Y,SIZE)
      CALL PSMOV(X-SIZE/2.,Y+SIZE/2.)
      CALL PSDRW(X+SIZE/2.,Y-SIZE/2.)
      CALL PSMOV(X-SIZE/2.,Y-SIZE/2.)
      CALL PSDRW(X+SIZE/2.,Y+SIZE/2.)
      RETURN
      END

      SUBROUTINE TRIUPPS(X,Y,SIZE)
      CALL PSMOV(X,Y+SIZE/2.)
      CALL PSDRW(X+SIZE/2.,Y-SIZE/2.)
      CALL PSDRW(X-SIZE/2.,Y-SIZE/2.)
      CALL PSDRW(X,Y+SIZE/2.)
      RETURN
      END

      SUBROUTINE TRIDOPS(X,Y,SIZE)
      CALL PSMOV(X,Y-SIZE/2.)
      CALL PSDRW(X+SIZE/2.,Y+SIZE/2.)
      CALL PSDRW(X-SIZE/2.,Y+SIZE/2.)
      CALL PSDRW(X,Y-SIZE/2.)
      RETURN
      END

      SUBROUTINE SQUAREPS(X,Y,SIZE)
      CALL PSMOV(X-SIZE/2.,Y-SIZE/2.)
      CALL PSDRW(X+SIZE/2.,Y-SIZE/2.)
      CALL PSDRW(X+SIZE/2.,Y+SIZE/2.)
      CALL PSDRW(X-SIZE/2.,Y+SIZE/2.)
      CALL PSDRW(X-SIZE/2.,Y-SIZE/2.)
      RETURN
      END
