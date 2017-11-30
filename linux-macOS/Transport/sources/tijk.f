    PROGRAM ABERA
C
C   TABULATES SECOND ORDER COMPONENTS OF BEAM
C
    REAL TIJK(21,5),XDIME(11)
    DIMENSION LREM(6),TREM(6),J(6),K(6),IND(5)
    CHARACTER*1 ST
    CHARACTER*4 LABEL,LABZ
    CHARACTER*10 TITEL
    CHARACTER*20 INPUT
    DATA IND /1,3,6,10,15/
    DATA ST /'*'/
C
    WRITE(6,*) ' '
    WRITE(6,*)
     X  'Table of 6 largest second order beam components per coordinate.'
    WRITE(6,*)
     X  '(components = Tijk * Xj * Xk , Xi = initial beam parameters.)'
    WRITE(6,*)
     X  '==============================================================='
C
C   OPEN FILE WITH TIJK
C
    1   OPEN(UNIT=0,FILE='FOR000.DAT',STATUS='OLD',
     X       FORM='UNFORMATTED',ERR=999)
    READ(0) XDIME
C
C   FIND USER SELECTION
C
    2   WRITE(6,1000)
    READ(5,1001) INPUT
    IF (INPUT.EQ.'QUIT' .OR. INPUT.EQ.'quit') GO TO 999
    LABEL='%%%%'
    IREM = 0
    READ(INPUT,1002,ERR=10) ZS
    IF (ZS .LT. 0.) THEN
    BACKSPACE(UNIT=0,ERR=601)
    BACKSPACE(UNIT=0,ERR=601)
    END IF
    GO TO 30
   10   READ(INPUT,1003) LABEL
    ZS=9999.
C
C   FIND DESIRED SECTION
C
   30   READ(0,END=600,ERR=601) ITYPE,Z,LABZ,TOTROT,TIJK
    IROT = TOTROT
    IF(Z .GE. ZS) GO TO 50
    IF(LABEL .EQ. LABZ) GO TO 50
    GO TO 30
   50   IF (ITYPE .EQ.  1) TITEL = '*BEAM*'
    IF (ITYPE .EQ.  2) TITEL = '*ROTAT*'
    IF (ITYPE .EQ.  3) TITEL = '*DRIFT*'
    IF (ITYPE .EQ.  4) TITEL = '*BEND*'
    IF (ITYPE .EQ.  5) TITEL = '*QUAD*'
    IF (ITYPE .EQ. 11) TITEL = '*ACC*'
    IF (ITYPE .EQ. 18) TITEL = '*SEXT*'
    IF (ITYPE .EQ. 19) TITEL = '*SOLO*'
    IF (ITYPE .EQ. 20) TITEL = '*Z RO*'
    IF (ITYPE .EQ. 21) TITEL = '*WIEN*'
    WRITE(6,1004) TITEL, Z, LABZ
C
C   PRINT THE 6 LARGEST CONTRIBUTIONS TO EACH COORDINATE.
C
    DO 500 I=1,5
    II=I
    SIG=1.
C
C   BACK ROTATION (ONLY FOR +-90 AND +-180 DEGREES)
C
    IF ((IROT .EQ. 180 .OR. IROT .EQ. -180) .AND. I .NE. 5) SIG=-1.
    IF (IROT .EQ. 90 .AND. (I .EQ. 1 .OR. I .EQ. 2)) SIG=-1.
    IF (IROT .EQ. -90 .AND. (I .EQ. 3 .OR. I .EQ. 4)) SIG=-1.
    IF (IROT .EQ. 90 .AND. I .NE. 5) II = I + 2 - 4 * ( (I+1)/4 )
    IF (IROT .EQ. -90 .AND. I .NE. 5) II = I + 2 - 4 * ( (I+1)/4 )
    NN=0
    DO 200 N=1,6
    TMAX=0.
    DO 100 L=1,21
    AUX=TIJK(L,II)*SIG
    IF(ABS(AUX).GE.ABS(TMAX)) THEN
    TMAX=AUX
    IREM=L
    END IF
  100   CONTINUE
    TREM(N)=TMAX
    LREM(N)=IREM
    TIJK(IREM,II)=0.
    IF(TMAX.NE.0.0) NN=NN+1
  200   CONTINUE
    DO 400 L=1,6
    J(L)=1
    K(L)=1
    DO 300 N=5,1,-1
    IF(LREM(L).GT.IND(N)) THEN
    K(L)=N+1
    J(L)=LREM(L)-IND(N)
    GO TO 400
    END IF
  300   CONTINUE
  400   CONTINUE
    IF(NN.EQ.0) GO TO 500
    WRITE(6,1010) (ST,I,J(L),K(L),ST,L=1,NN)
    WRITE(6,1011,ERR=450) (TREM(L),L=1,NN)
    GO TO 500
  450   WRITE(6,1012) (TREM(L),L=1,NN)
  500   CONTINUE
    GO TO 2
C
C   ERROR HANDLING
C
  600   IREM = IREM + 1
    IF (IREM .EQ. 2) WRITE(6,2000)
  601   REWIND(UNIT=0)
    IF (IREM .EQ. 1) THEN
    READ(0) XDIME
    GO TO 30
    END IF
    GO TO 1
C
C
  999   CONTINUE
C
 1000   FORMAT(/
     X  ' Input: z_coordinate in m or label (<CR>: next, QUIT: exit) ',$)
 1001   FORMAT(A20)
 1002   FORMAT(F20.0)
 1003   FORMAT(A4)
 1004   FORMAT(/4X,A10,F7.3,' m',7X,A4)
 1010   FORMAT(/6(4X,A1,I1,I2,I1,A1,3X))
 1011   FORMAT(6(F10.4,3X))
 1012   FORMAT(6(G10.3,3X))
 2000   FORMAT(' z groesser als zmax oder unbekanntes Label.')
    END
