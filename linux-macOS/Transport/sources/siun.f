      PROGRAM SIUN
C
      PARAMETER (LDM=2000)
      CHARACTER*80 UEBER
      CHARACTER*20 INPUT
      CHARACTER*10 TITEL
      CHARACTER*2 DIM1,DIM2,DIM3,DIM4
      CHARACTER*4 LABEL,LAB,DIM5,DIM6,LBI,LABI(LDM)
      INTEGER X1(101),X2(101),Y1(101),Y2(101)
      REAL LWX,LWY
      LOGICAL PRINT
      DIMENSION
     *  Z(LDM),XMAX(LDM),YMAX(LDM),D(LDM),DPY(LDM),ANG(LDM),
     *  DY(LDM),LENG(LDM),AP(LDM),B(LDM),PXMAX(LDM),R12(LDM),
     *  PYMAX(LDM),DP(LDM),R34(LDM),X0(LDM),Y0(LDM),X0P(LDM),
     *  Y0P(LDM),CX(LDM),SX(LDM),CPX(LDM),SPX(LDM),CY(LDM),
     *  SY(LDM),CPY(LDM),SPY(LDM),XN(LDM),TITEL(LDM),LAB(LDM)
C
C   TEILWEISES LESEN VON FILE 15
C
      OPEN(UNIT=15,FILE='FOR015.DAT',STATUS='OLD',
     1       FORM='UNFORMATTED')
      READ(15,ERR=2002) DIM1,DIM2,DIM3,DIM4,DIM5,DIM6,P,DELP,INDMAX
     X                   ,IND,IN,IS,ICH,CHI,IPOINT,SAVE1,SAVE2,SAVE3
      READ(15) (TITEL(I),LAB(I),Z(I),XMAX(I),YMAX(I),D(I)
     X      ,DY(I),LENG(I),AP(I),B(I),PXMAX(I),R12(I),DPY(I),PYMAX(I)
     X      ,DP(I),R34(I),X0(I),Y0(I),X0P(I),Y0P(I),CX(I),SX(I),CPX(I)
     X      ,SPX(I),CY(I),SY(I),CPY(I),SPY(I),XN(I),ANG(I),I=1,INDMAX)
      CLOSE(UNIT=15)
      IF (DIM1 .EQ. 'M ') DIM1 = 'm '
      IF (DIM2 .EQ. 'MM') DIM2 = 'mm'
      IF (DIM2 .EQ. 'CM') DIM2 = 'cm'
C
C   SOME AUXILIARY PARAMETERS FOR COMPUTING PHASE ADVANCE ANGLE
C   ACCORDING TO W.JOHO (MAY 1988)
C
       DO 10 J = 1, 20
        IF (Z(J) .NE. 0.0) GO TO 11
   10  CONTINUE
   11  J = J - 1   !   LAST ELEMENT WITH Z = 0.
       EX0    = XMAX(J)*PXMAX(J)*SQRT(1.-R12(J)**2)
       CHIX0  = 180./3.14159 * ASIN(R12(J))
       ALFAX0 = -TAN(CHIX0)
       EY0    = YMAX(J)*PYMAX(J)*SQRT(1.-R34(J)**2)
       CHIY0  = 180./3.14159 * ASIN(R34(J))
       ALFAY0 = -TAN(CHIY0)
C
C   FIND USER CHOICE
C
       J = 0
  100   CONTINUE
       LABEL = '%%%%'
       WRITE (6, 4000)
 4000   FORMAT(/' Input or HELP<CR> ',$)
       READ (5, 4001, END=2600) INPUT
 4001   FORMAT(A20)
C
C   laser printer
C
      IF (INPUT.EQ.'P' .OR. INPUT.EQ.'p') THEN
       READ(9,3010,ERR=2002) UEBER
       READ(9,3010) UEBER
 3010    FORMAT(A79)
       PRINT = .TRUE.
       OPEN(UNIT=7,FILE='SIGMA.DAT',STATUS='UNKNOWN')
       WRITE(7,3012) UEBER
 3012    FORMAT(/1X,A79///)
       WRITE(6,*) 'SIGMA.DAT data_file for laser printer generated.'
       GO TO 100
      ENDIF
C
C   HELP
C
      IF   (INPUT      .EQ. 'HELP' .OR.
     1      INPUT(1:2) .EQ. 'H '   .OR.
     2      INPUT      .EQ. 'help' .OR.
     3      INPUT(1:2) .EQ. 'h ')  THEN
       WRITE(*,*) 'Legal inputs are:'
       WRITE(*,*)
     1'z-coordinate in meters or desired label at which the Sigma- and'
       WRITE(*,*)
     1'R-matrices have to be shown.  Enter <CR> or -1<CR> for showing'
       WRITE(*,*)
     1'the matrices of the next or of the previous element respectively'
       WRITE(*,*)
     1'P<CR> produces a laser-output of all subsequent shown matrices.'
       WRITE(*,*)
     1'Enter HALT or QUIT to exit the program.'
       WRITE(*,*)
     1'In case the input is illegal, the following message appears:'
       GO TO 2000
      ENDIF
C
      IF   (INPUT .EQ. 'HALT'    .OR.
     1      INPUT .EQ. 'QUIT'    .OR.
     2      INPUT .EQ. 'halt'    .OR.
     3      INPUT .EQ. 'quit') GO TO 2600
C
       READ (INPUT, 4002, ERR=200) ZS
 4002   FORMAT(F20.0)
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
C   FINE GESUCHTES Z
C
       ELSE
        GO TO 1
       END IF
C
  200   READ (INPUT, 4003) LABEL
       ZS = 9999.
 4003   FORMAT(A4)
C
    1   DO 2 J = 1, INDMAX
         IF (LAB(J) .EQ. LABEL) GO TO 5
         IF (Z(J) .GE. ZS) GO TO 5
    2   CONTINUE
        GO TO 2000
C
C   Z GEFUNDEN
C
    5   CONTINUE
C
C   COMPUTE PHASE ADVANCE ANGLE
C
       SINPSIX = SX(J)*EX0 / (XMAX(1)*XMAX(J))
       SINPSIX = MIN(1.0,MAX(-1.0,SINPSIX))
       COSPSIX = CX(J)*XMAX(1)/XMAX(J) - ALFAX0*SINPSIX
       COSPSIX = MIN(1.0,MAX(-1.0,COSPSIX))
        PSIX   = 180./3.14159 * ASIN(ABS(SINPSIX))
       IF      (SINPSIX.GT.0.0 .AND. COSPSIX.LT.0.0) THEN
        PSIX   = 180. - PSIX
           ELSE IF (SINPSIX.LT.0.0 .AND. COSPSIX.LT.0.0) THEN
        PSIX   = 180. + PSIX
       ELSE IF (SINPSIX.LT.0.0 .AND. COSPSIX.GT.0.0) THEN
        PSIX   = 360. - PSIX
       ELSE
       ENDIF
C
       SINPSIY = SY(J)*EY0 / (YMAX(1)*YMAX(J))
       SINPSIY = MIN(1.0,MAX(-1.0,SINPSIY))
       COSPSIY = CY(J)*YMAX(1)/YMAX(J) - ALFAY0*SINPSIY
       COSPSIY = MIN(1.0,MAX(-1.0,COSPSIY))
        PSIY   = 180./3.14159 * ASIN(ABS(SINPSIY))
       IF      (SINPSIY.GT.0.0 .AND. COSPSIY.LT.0.0) THEN
        PSIY   = 180. - PSIY
           ELSE IF (SINPSIY.LT.0.0 .AND. COSPSIY.LT.0.0) THEN
        PSIY   = 180. + PSIY
       ELSE IF (SINPSIY.LT.0.0 .AND. COSPSIY.GT.0.0) THEN
        PSIY   = 360. - PSIY
       ELSE
       ENDIF
        WRITE (6, 1000) TITEL(J),Z(J),DIM1,LAB(J),
     X                  X0(J),DIM2,X0P(J),Y0(J),DIM2,Y0P(J),
     X                  XMAX(J),DIM2,PXMAX(J),R12(J),PSIX,
     X                  YMAX(J),DIM2,PYMAX(J),R34(J),PSIY,
     X                  CX(J),SX(J),D(J),CY(J),SY(J),DY(J),
     X                  CPX(J),SPX(J),DP(J),CPY(J),SPY(J),DPY(J)
       IF (PRINT) THEN
        WRITE (7, 1000) TITEL(J),Z(J),DIM1,LAB(J),
     X                  X0(J),DIM2,X0P(J),Y0(J),DIM2,Y0P(J),
     X                  XMAX(J),DIM2,PXMAX(J),R12(J),PSIX,
     X                  YMAX(J),DIM2,PYMAX(J),R34(J),PSIY,
     X                  CX(J),SX(J),D(J),CY(J),SY(J),DY(J),
     X                  CPX(J),SPX(J),DP(J),CPY(J),SPY(J),DPY(J)
       ENDIF
 1000   FORMAT(/
     X' ',A10,'  z =',F7.3,1X,A2,5X,A4/
     X' *SIGMA*'/
     X'  Zentren :   ',2(F10.3,1X,A2,F10.3,' mrad')/
     X'  horz. Par. :',
     X   F10.3,1X,A2,F10.3,' mrad',F10.3,F11.1,' deg (Ph.a.)'/
     X'  vert. Par. :',
     X   F10.3,1X,A2,F10.3,' mrad',F10.3,F11.1,' deg (Ph.a.)'/
     X' *TRANSFORM 1*'/
     X'     horz:                                    vert:'/
     X          3(F12.5),5X,3(F12.5)/
     X          3(F12.5),5X,3(F12.5))
       GO TO 100
C
C   FEHLERMELDUNG
C
 2000   WRITE (6, 2001) Z(INDMAX)
 2001   FORMAT(
     1' Z > Zmax =',F8.3,' m or none of the listed labels found:')
       JJ = 0
       DO 550 II = 1, INDMAX
        IF (LAB(II) .NE. ' ') THEN
         JJ = JJ + 1
         LABI(JJ) = LAB(II)
        ENDIF
  550   CONTINUE
       WRITE(6,2022) (LABI(II),II = 1, JJ)
 2022   FORMAT(12(1X,A4,1X))
       GO TO 100
C
C   FILE NOT FOUND
C
 2002   WRITE (6, 2003)
 2003   FORMAT(' Run Transport first.')
 2500   STOP
C
C   PRINT
C
 2600   IF (PRINT) THEN
        CLOSE(UNIT=7)
        CALL SYSTEM('./print text SIGMA.DAT')
       ENDIF
       STOP
       END

