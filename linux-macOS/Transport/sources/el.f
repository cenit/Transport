        PROGRAM ELEMENT
C
        CHARACTER*130 CARD, TITEL, ANSW*1
        DATA IO /6/
C
        OPEN(UNIT=2,FILE='FOR002.DAT',STATUS='OLD',ERR=2001)
 1000   FORMAT(A)
 1001   FORMAT(11A4)
        TITEL = CARD
        I = 0
C
C       FOR002.DAT VON T ODER TFO ERZEUGT ?
C
        READ(2,1000,ERR=2001) CARD
        IF ((CARD(9:10).EQ.'MR') .AND. (CARD(17:18).EQ.'MR'))
     X     GO TO 2002
C
C   FINDE *PLOT* 
C
    1   CONTINUE
        READ(2,1000,END=200,ERR=2001) CARD
        IF (CARD(1:7) .NE. '1*PLOT*') GO TO 1
C
C       FINDE ELEMENT KARTEN
C
    2   CONTINUE
        READ(2,1000,END=200,ERR=2001) CARD
        IF(CARD(2:2)  .NE. '*')           GO TO 2
        IF(CARD(3:11) .EQ. 'TRANSFORM')   GO TO 2
        IF(CARD(3:11) .EQ. '2ND ORDER')   GO TO 2
        IF(CARD(3:13) .EQ. 'CORRECTIONS') GO TO 2
C
        CARD(1:1) = ' '
        CALL TYPE2(IO,CARD,NRL)
        I = I + NRL
        IF ((I.GE.23) .AND. IO.EQ.6) THEN
         WRITE(6,1235)
         READ(5,1234,END=2000) ANSW
         I = 0
        ENDIF
        GO TO 2
 1235   FORMAT(' <CR> for continue ',$)
 1234   FORMAT(A)
C
C   File-OUTPUT
C
  200   IF (IO .EQ. 7) THEN
         CLOSE(UNIT=7)
         WRITE(6,1502)
 1502    FORMAT(' ===> Output to printer [Y/N] ? ',$)
         READ(5,1000,END=2000) ANSW
         IF (ANSW.EQ.'Y' .OR. ANSW.EQ.'y') THEN
           CALL SYSTEM('./print text ELEMENT.DAT')
         ENDIF
         GO TO 2000
        ENDIF
C
        WRITE(6,1500)
 1500   FORMAT(' ===> Output to file ELEMENT.DAT [Y/N] ? ',$)
        READ(5,1000,END=2000) ANSW
        IF (ANSW.EQ.'Y' .OR. ANSW.EQ.'y') THEN
         IO = 7
         OPEN(UNIT=7,FILE='ELEMENT.DAT',STATUS='UNKNOWN')
         REWIND 2
         JJ = 3
         IF (TITEL(3:8) .EQ. '*PLOT*') JJ = 9
         WRITE(IO,1501) TITEL(JJ:79)
 1501    FORMAT(1X,A,//)
         GO TO 2
        ELSE
         GO TO 2000
        ENDIF
C
C   ERROR
C
 2001   WRITE(6,*) 'Run Transport first.'
        GOTO 2000
 2002   CALL EMU
 2000   CONTINUE
        STOP
        END
C
        SUBROUTINE TYPE2(IO,LINE,NRL)
C
        CHARACTER*130 LINE
        CHARACTER*10 IFORM2
        CHARACTER*6  IFORM1
        DATA IFORM1 /'(A130)'/
        DATA IFORM2 /'(12X,A130)'/
C
        IF (LINE(1:1) .EQ. '1') LINE(1:1)=' '
        DO 100 J = 1, 130
         I = 131 - J
         IF (LINE(I:I) .NE. ' ') GO TO 200
  100   CONTINUE
  200   IF (I .GT. 80) GO TO 201
        WRITE(IFORM1(3:5),1001) I
        WRITE(IO,IFORM1) LINE
 1001   FORMAT(I3)
        NRL = 1
        RETURN
  201   WRITE(IO,1002) LINE(1:75)
 1002   FORMAT(A75)
        K = I - 75
        WRITE(IFORM2(7:9),1001) K
        WRITE(IO,IFORM2) LINE(76:I)
        NRL = 2
        RETURN
        END

        SUBROUTINE EMU
C
        PARAMETER (LDM=2000)
        CHARACTER*80 UEBER
        CHARACTER*20 INPUT
        CHARACTER*10 TITEL, ANSW
        CHARACTER*2 DIM1,DIM2,DIM3,DIM4
        CHARACTER*4 LABEL,LAB,DIM5,DIM6,LBI,LABI(LDM)
        INTEGER X1(101),X2(101),Y1(101),Y2(101)
        REAL LWX,LWY,LENG
        LOGICAL PRINT
        DIMENSION
     *  Z(LDM),XMAX(LDM),YMAX(LDM),D(LDM),DPY(LDM),ANG(LDM),
     *  DY(LDM),LENG(LDM),AP(LDM),B(LDM),PXMAX(LDM),R12(LDM),
     *  PYMAX(LDM),DP(LDM),R34(LDM),X0(LDM),Y0(LDM),X0P(LDM),
     *  Y0P(LDM),CX(LDM),SX(LDM),CPX(LDM),SPX(LDM),CY(LDM),
     *  SY(LDM),CPY(LDM),SPY(LDM),XN(LDM),TITEL(LDM),LAB(LDM)
        DATA IO /6/
C
C   TEILWEISES LESEN VON FILE 15
C
        OPEN(UNIT=15,FILE='FOR015.DAT',STATUS='OLD',
     X       FORM='UNFORMATTED',ERR=2002)
        READ(15,ERR=2002) DIM1,DIM2,DIM3,DIM4,DIM5,DIM6,P,DELP,INDMAX
     X,                   IND,IN,IS,ICH,CHI,IPOINT,SAVE1,SAVE2,SAVE3
        READ(15) (TITEL(I),LAB(I),Z(I),XMAX(I),YMAX(I),D(I)
     X  ,DY(I),LENG(I),AP(I),B(I),PXMAX(I),R12(I),DPY(I),PYMAX(I)
     X  ,DP(I),R34(I),X0(I),Y0(I),X0P(I),Y0P(I),CX(I),SX(I),CPX(I)
     X  ,SPX(I),CY(I),SY(I),CPY(I),SPY(I),XN(I),ANG(I),I=1,INDMAX)
        IF (DIM1 .EQ. 'M ') DIM1 = 'm '
        IF (DIM2 .EQ. 'MM') DIM2 = 'mm'
        IF (DIM2 .EQ. 'CM') DIM2 = 'cm'
C
C   TYPE ELEMENT PARAMETERS
C
    2   TOTLENG = 0.
        TITEL(INDMAX+1) = '*LENGTH*'
        J = 0
        DO 100 I = 1, INDMAX+1
          CALL TYPE1(IO,TITEL(I),LAB(I),LENG(I),AP(I),B(I),XN(I),ANG(I),
     X               DIM1, DIM2, DIM4, DIM5, TOTLENG, J)
          IF (J.GE.23 .AND. IO.EQ.6 .AND. I.NE.INDMAX+1) THEN
           J = 0
           WRITE(6,1235)
           READ(5,1234,END=2000) ANSW
          ENDIF
  100   CONTINUE
 1235   FORMAT('<CR> for continue ',$)
 1234   FORMAT(A)
C
C   File-OUTPUT
C
        IF (IO .EQ. 7) THEN
          CLOSE(UNIT=7)
          WRITE(6,1503)
 1503     FORMAT(' ===> Output to printer [Y/N] ? ',$)
          READ(5,1000,END=2000) ANSW
          IF (ANSW.EQ.'Y' .OR. ANSW.EQ.'y') THEN
            CALL SYSTEM('./print text ELEMENT.DAT')
          ENDIF
          GO TO 2000
        ENDIF
C
        WRITE(6,1500)
 1500   FORMAT(' ===> Output to file ELEMENT.DAT [Y/N] ? ',$)
        READ(5,1000,END=2000) ANSW
 1000   FORMAT(A)
        IF (ANSW(1:1).EQ.'Y' .OR. ANSW(1:1).EQ.'y') THEN
         IO = 7
         OPEN(UNIT=7,FILE='ELEMENT.DAT',STATUS='UNKNOWN')
         OPEN(UNIT=9,FILE='FOR009.DAT',STATUS='OLD')
         READ(9,1502,ERR=2002) UEBER
         READ(9,1502) UEBER
         CLOSE(UNIT=9)
 1502    FORMAT(A79)
         REWIND(UNIT=7)
         WRITE(7,1501) UEBER
 1501    FORMAT(1X,A,//)
         GO TO 2
        ELSE
         GO TO 2000
        ENDIF
C
C   ERROR
C
 2002   WRITE(6,*) 'Run Transport first'
 2000   RETURN
        END
C
        SUBROUTINE TYPE1(IO,TITEL,LAB,LENG,AP,B,XN,ANG,DIM1,DIM2,DIM4,
     X      DIM5,TL,J)
C
        CHARACTER*10 TITEL
        CHARACTER*2 DIM1, DIM2, DIM4
        CHARACTER*4 LAB, DIM5
        REAL LENG
C
        IF      (TITEL .EQ. '*DRIFT*') THEN
         WRITE(IO,1000) TITEL, LAB, LENG, DIM1
 1000    FORMAT(A10,2X,'"',A4,'"',F12.4,1X,A2)
         TL = TL + LENG
        ELSE IF (TITEL .EQ. '*QUAD*')  THEN
         WRITE(IO,1001) TITEL, LAB, LENG, DIM1, AP, DIM2, B, DIM4
 1001    FORMAT(A10,2X,'"',A4,'"',F12.4,1X,A2,F10.2,1X,A2,F10.5,1X,A2)
         TL = TL + LENG
        ELSE IF (TITEL .EQ. '*EQUAD*')  THEN
         WRITE(IO,7001) TITEL, LAB, LENG, DIM1, AP, DIM2, B
 7001   FORMAT(A10,2X,'"',A4,'"',F12.4,1X,A2,F10.2,1X,A2,F10.5,' kV/cm')
         TL = TL + LENG
        ELSE IF (TITEL .EQ. '*SOLO*')  THEN
         WRITE(IO,1002) TITEL, LAB, LENG, DIM1, AP, DIM2, B, DIM4
 1002    FORMAT(A10,2X,'"',A4,'"',F12.4,1X,A2,F10.2,1X,A2,F10.5,1X,A2)
         TL = TL + LENG
        ELSE IF (TITEL .EQ. '*SEXT*')  THEN
         WRITE(IO,1003) TITEL, LAB, LENG, DIM1, AP, DIM2, B, DIM4
 1003    FORMAT(A10,2X,'"',A4,'"',F12.4,1X,A2,F10.2,1X,A2,F10.5,1X,A2)
         TL = TL + LENG
        ELSE IF (TITEL .EQ. '*WIEN*')  THEN
         WRITE(IO,1004) TITEL, LAB, LENG, DIM1, AP, DIM2, B, DIM4
 1004    FORMAT(A10,2X,'"',A4,'"',F12.4,1X,A2,F10.2,1X,A2,F10.5,1X,A2)
         TL = TL + LENG
        ELSE IF (TITEL .EQ. '*BEND*')  THEN
         WRITE(IO,1005) TITEL, LAB, LENG, DIM1, AP, DIM2, B, DIM4,XN,ANG
 1005    FORMAT(A10,2X,'"',A4,'"',F12.4,1X,A2,F10.2,1X,A2,F10.5,1X,A2,
     X          F10.4,F7.2,' Deg')
         TL = TL + LENG
        ELSE IF (TITEL .EQ. '*E-PRISM*')  THEN
         WRITE(IO,7005) TITEL, LAB, LENG, DIM1, AP, DIM2, B, XN,ANG
 7005    FORMAT(A10,2X,'"',A4,'"',F12.4,1X,A2,F8.2,1X,A2,F10.5,' kV/cm',
     X          F8.4,F7.2,' Deg')
         TL = TL + LENG
        ELSE IF (TITEL .EQ. '*ROTAT*') THEN
         WRITE(IO,1006) TITEL, LAB, LENG, AP, B, XN
 1006    FORMAT(A10,2X,'"',A4,'"',F12.4,' Deg',3F10.5)
        ELSE IF (TITEL .EQ. '*Z RO*')  THEN
         WRITE(IO,1007) TITEL, LAB, ANG
 1007    FORMAT(A10,2X,'"',A4,'"',F12.4,' Deg')
        ELSE IF (TITEL .EQ. '*ACC*')   THEN
         WRITE(IO,1008) TITEL, LAB, LENG, DIM1, AP, DIM2, B, DIM5
 1008    FORMAT(A10,2X,'"',A4,'"',F12.4,1X,A2,F10.2,1X,A2,F10.5,1X,A4)
         TL = TL + LENG
        ELSE IF (TITEL .EQ. '*BEAM*')  THEN
         WRITE(IO,1009) TITEL, LAB, ANG, DIM5
 1009    FORMAT(A10,2X,'"',A4,'"',F12.4,1X,A4)
        ELSE IF (TITEL .EQ. '*SCINT*') THEN
         J = J - 1
        ELSE IF (TITEL .EQ. '*SP CH*') THEN
          WRITE(IO,1010) TITEL, LAB, LENG, DIM1, B, XN, ANG
 1010     FORMAT(A10,2X,'"',A4,'"',F12.4,1X,A2,F10.4,' A ',F10.2,
     X      ' MHz',F10.1)
        ELSE IF (TITEL .EQ. '*RF CAV*')THEN
         WRITE(IO,1011) TITEL, LAB
 1011    FORMAT(A10,2X,'"',A4,'"')
        ELSE IF (TITEL .EQ. '*SLIT*')  THEN
         WRITE(IO,1012) TITEL, LAB, ANG, DIM2, AP, DIM2
 1012    FORMAT(A10,2X,'"',A4,'"',6X,'0.0000 m ',F10.2,1X,A2,
     X          F10.2,1X,A2)
        ELSE IF (TITEL .EQ. '*MATRIX*')  THEN
         WRITE(IO,1013) TITEL, LAB, LENG, DIM1
 1013    FORMAT(A10,2X,'"',A4,'"',F12.4,1X,A2)
         TL = TL + LENG
        ELSE IF (TITEL .EQ. '*LENGTH*')THEN
         WRITE(IO,1019) TITEL, TL, DIM1
 1019    FORMAT(A10,8X,F12.4,1X,A2)
        ELSE
         WRITE(IO,1020) TITEL, LAB, LENG, AP, B, XN, ANG
 1020    FORMAT(A10,2X,'"',A4,'"',F12.4,4F10.5)
        ENDIF
        J = J + 1
        RETURN
        END
