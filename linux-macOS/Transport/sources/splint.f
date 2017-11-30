      FUNCTION SPLINT (M,N,X,Y,YA,Z,ZA)
C
C     LIBRARY-ROUTINE
C
C                                                5. MAERZ 1971
C                                                E. SEILER SIN
C
C
C
C     KUBISCHE INTERPOLATION ZWISCHEN ZWEI STUETZSTELLEN
C     **************************************************
C
C
C
C     DIE 1.ABLEITUNG IN DEN STUETZSTELLEN WIRD NACH
C        ZWEI VERSCHIEDENEN METHODEN BERECHNET
C
C     3 ENTRIES MIT GLEICHEN, SPAETER BESCHRIEBENEN FORMALEN PARAMETERN:
C          SPLINI1 -- INITIALISIERUNG FUER INTERPOLATION MIT
C                     ABLEITUNGEN BESTIMMT DURCH PARABEL DURCH
C                     3 STUETZSTELLEN
C          SPLINI2 -- INITIALISIERUNG FUER INTERPOLATION MIT
C                     ABLEITUNGEN NACH METHODE VON H. AKIMA
C                     (JOURNAL OF ACM VOL.17.4.589 OKT.1970)
C          SPLINT  -- BERECHNUNG VON WEITEREN INTERPOLATIONSWERTEN
C                     FUER INITIALISIERTEN STUETZSTELLENARRAY
C
C
C     DIE INITIALISIERUNG MUSS FUER JEDEN NEUEN STUETZSTELLENARRAY
C     NEU AUSGEFUEHRT WERDEN (ERSTER INTERPOLIERTER WERT WIRD
C     DABEI EBENFALLS SCHON GELIEFERT)
C
C
C     FORMALE PARAMETER:
C
C          M          =ORDNUNG DER ZU BERECHNENDEN ABLEITUNGEN
C          N          =ANZAHL STUETZPUNKTE (INPUT)
C          X(N)       =STUETZARGUMENTE (INPUT)
C          Y(N)       =STUETZWERTE (INPUT)
C          YA(N)      =ABLEITUNGEN DER FUNKTION AN STUETZPUNKTEN
C                      (WIRD GELIEFERT)
C          Z          =BELIEBIGES ARGUMENT (INPUT)
C          ZA(M)      =ABLEITUNGEN DER ORDNUNG M ZU Z (WIRD GELIEFERT)
C          SPLINT     =INTERPOLIERTER FUNKTIONSWERT ZU Z (WIRD GELIEFERT
C
C     ACHTUNG:        STUETZWERTE (X,Y) MUESSEN AUFSTEIGEND SORTIERT
C                     SEIN
C
C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C
C
C
      DIMENSION         X(N),Y(N),YA(N),ZA(3)
C
      GO TO 70
C
      ENTRY SPLINI1 (M,N,X,Y,YA,Z,ZA)
C
C     BESTIMMUNG DER ABLEITUNG YA(I) AN DER STUETZSTELLE X(I),Y(I)
C        MIT EINER PARABEL DURCH DREI PUNKTE
C
      IF(N.LT.1) RETURN
      IF(N.EQ.1) GO TO 200
      IF(N.EQ.2) GO TO 210
    5 R23=1./(X(2)-X(3))
      X12 = X(1)-X(2)
      X13 = X(1)-X(3)
      YA(1) = (Y(1)-Y(2)*R23*X13)/X12 + (Y(1)+R23*Y(3)*X12) / X13
      X23 = X12
      X32 = -X23
      X31 = -X13
      R12 = R23
      R21 = -R12
      R13 = .5*R23
      MM = N-1
      DO10 I=2, MM
      X12 = X(I-1) -X(I)
      X23 = X(I) - X(I+1)
      R12 = 1./X12
      R13 = 1./(X(I-1)-X(I+1))
      R23 = 1./X23
   10 YA(I) = R12*(X23*R13*Y(I-1)-Y(I)) + R23*(Y(I)-X12*R13*Y(I+1) )
      R21 = 1./(X(N-1)-X(N-2) )
      X32 = X(N)-X(N-1)
      X31 = X(N)-X(N-2 )
      YA(N) = (Y(N)- Y(N-1)*R21*X31)/X32 + (Y(N)+R21*Y(N-2)*X32)/X31
      GO TO 70
C
      ENTRY SPLINI2 (M,N,X,Y,YA,Z,ZA)
C
C     BESTIMMUNG DER ABLEITUNGEN NACH H. AKIMA
C        JOURNAL OF ACM VOL.17,4,589 (OKT. 1970)
C
      IF(N.LT.1) RETURN
      IF(N.EQ.1) GO TO 200
      IF(N.EQ.2) GO TO 210
      IF(N.EQ.3) GO TO 5
      STG1=(Y(2)-Y(1))/(X(2)-X(1))
      STG2=(Y(3)-Y(2))/(X(3)-X(2))
      STG3=(Y(4)-Y(3))/(X(4)-X(3))
      F1=ABS(STG1-STG2)
      F2=ABS(STG2-STG3)
      YA(1)=.5*(3.*STG1-STG2)
      IF(F1+F2 .NE. 0.0) YA(2)=(STG2*F1+STG1*F2)/(F1+F2)
      MM=N-2
      DO 60 I=3,MM
      STG4=(Y(I+2)-Y(I+1))/(X(I+2)-X(I+1))
      F1=ABS(STG4-STG3)
      F2=ABS(STG2-STG1)
      IF(STG1-STG2)40,20,40
   20 IF(STG3-STG4)40,30,40
   30 YA(I)=.5*(STG2+STG3)
      GO TO 50
   40 IF(F1+F2 .NE. 0.0) YA(I)=(STG2*F1+STG3*F2)/(F1+F2)
   50 STG1=STG2
      STG2=STG3
   60 STG3=STG4
C
      F1=ABS(STG3-STG2)
      F2=ABS(STG2-STG1)
      IF(F1+F2 .NE.0.0) YA(N-1)=(STG2*F1+STG3*F2)/(F1+F2)
      YA(N)=.5*(3.*STG3-STG2)
C
C     2. BESTIMMUNG DER ZWEI STUETZSTELLEN
C
   70 IF(N.GT.1) GO TO 71
      IF(N.LT.1) RETURN
      SPLINT=Y(1)
      IF(M.LT.1) RETURN
      DO 69 I=1,M
   69 ZA(I)=0.
      RETURN
   71 IF(Z.GE.X(1).AND.Z.LE.X(N)) GO TO 72
      SPLINT=0.
      RETURN
   72 IL=1
      IH=N
   73 IF((IH-IL).EQ.1) GO TO 100
      I=(IL+IH)/2
      IF(Z-X(I))74,76,75
   74 IH=I
      GO TO 73
   75 IL=I
      GO TO 73
   76 IL=I
      IH=I+1
  100 X1=X(IL)
      X2=X(IH)
      Y1=Y(IL)
      Y2=Y(IH)
      Y1A=YA(IL)
      Y2A=YA(IH)
C
C     3. BESTIMMUNG DES GESUCHTEN FUNKTIONSWERTES UND DESSEN ABLEITUNGEN
C
      DX = X2 -X1
      DY = Y2-Y1
      U= (Z -X1) /DX
      U2= U*U
      U3 = U2*U
      DX2 = DX*DX
      DX3 = DX2*DX
      DY2 = -2.*DY
      YA2 = Y2A+2.*Y1A
      DY3 = 3.*DY
      YA3 = Y2A+Y1A
      YB2 = DY2+ DX*YA3
      YB4 = DY3 - DX*YA2
      SPLINT= Y1+U*DX*Y1A + U2*YB4 + U3*YB2
C
      IF(M)110,110,120
  110 IORD=1
      GO TO 150
  120 IF(M-3)140,130,130
  130 IORD=4
      GO TO 150
  140 IORD=M+1
  150 GO TO (160,170,180,190),IORD
  160 RETURN
  170 ZA(1) = Y1A +2.*U/DX*YB4 +3.*U2/DX*YB2
      RETURN
  180 ZA(1) = Y1A +2.*U/DX*YB4 +3.*U2/DX*YB2
      ZA(2) = 2./DX2*YB4 + 6.*U/DX2*YB2
      RETURN
  190 ZA(1) = Y1A +2.*U/DX*YB4 +3.*U2/DX*YB2
      ZA(2) = 2./DX2*YB4 + 6.*U/DX2*YB2
      ZA(3) = 6./DX3*YB2
      RETURN
C
C     SPEZIALFAELLE
C
  200 YA(1)=0.
      RETURN
C
  210 YA(1)=(Y(2)-Y(1))/(X(2)-X(1))
      YA(2)=YA(1)
      RETURN
         END