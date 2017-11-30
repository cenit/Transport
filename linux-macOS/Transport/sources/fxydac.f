    SUBROUTINE FXYDAC       
C
    CHARACTER*19 QUADD(30)
    CHARACTER*15 XDIM(30),YDIM(30)
    CHARACTER    STR1*5, STR2*13, STR3*10
    DIMENSION    IQ(30),IM(30),IV(30)
    DIMENSION    LEFF(100),JA(100),A0(100),A1(100),A2(100),A3(100)
    REAL         J0(100),JMAX(100),DMAX(100)
        CHARACTER*4  QTYP(100), QLAB(100)
        CHARACTER*100 DUMMY
C    CHARACTER*4  LABLE
C
    INCLUDE 'incom.f'
C
        LOGICAL SLITS
C
C       WRITE TIME STAMP
C
        CALL TIMESTAMP(0)
C
C   READ IN QUAD CALIBRATION FACTORS
C           
        OPEN(UNIT=50,FILE='QUADS.CAL',STATUS='OLD')
    K = 1
    1   CONTINUE
    IF (K .GT. 100) STOP 'FILE QUAD.CAL OVERFLOW'
    READ(50,2000,END=10,ERR=10)
     1   QTYP(K),QLAB(K),LEFF(K),JA(K),A0(K),A1(K),A2(K),A3(K),
     2   J0(K),JMAX(K),DMAX(K)
    K = K + 1
    GO TO 1
   10   NREC = K - 1
    A0(K) = 0.
    A1(K) = 1.
    A2(K) = 0.
    A3(K) = 0.
    J0(K) = 1000.
    JMAX(K) = 1.
    DMAX(K) = 1000.
    CLOSE(UNIT=50)
C
C   READ IN BEAMLINE DATA FILE
C           
        CALL READD(SLITS)
C           
C   READ QUAD AND ENVELOP DATA
C       FIND OUT IF IT IS WRITTEN IN 10I6 OR 10I10 FORMAT
C           
        OPEN(UNIT=22,FILE='FOR022.DAT',STATUS='OLD')
    READ(22,2221) DUMMY
    IF ((DUMMY(1:4)   .EQ. '    ') .AND.
    1   (DUMMY(11:14) .EQ. '    ') .AND.
    2   (DUMMY(21:24) .EQ. '    ') .AND.
    3   (DUMMY(31:34) .EQ. '    ') .AND.
    4   (DUMMY(41:44) .EQ. '    ') .AND.
    5   (DUMMY(51:54) .EQ. '    ') .AND.
    6   (DUMMY(61:64) .EQ. '    ') .AND.
    7   (DUMMY(71:74) .EQ. '    ') .AND.
    8   (DUMMY(81:84) .EQ. '    ') .AND.
    9   (DUMMY(91:94) .EQ. '    ')) THEN
         REWIND 22
         GO TO 20
        ELSE
         REWIND 22
        ENDIF
C
C       WRITTEN IN 10I6 FORMAT
C
        READ(22,2002,ERR=20) IQ,(IM(JJ),JJ=1,20),(IV(KK),KK=1,20)   
        GO TO 30
C
C       WRITTEN IN 10I10 FORMAT
C
   20   READ(22,2222,ERR=21) IQ,(IM(JJ),JJ=1,20),(IV(KK),KK=1,20)
    GO TO 30
   21   WRITE(6,*) 'FOR022.DAT read error'
    CALL SLEEP(3)
    RETURN
   30   CONTINUE
    CLOSE(UNIT=22)
        DO 110 KK = 1, 5
     IM(20+KK) = IQ(20+KK)
     IV(20+KK) = IQ(25+KK)
  110   CONTINUE
C
C   INSERT DATA IN BEAMLINE
C
    MFX = 0
    MFY = 0
    MQ  = 0
    DO 700 NUM = 1, NEL     
    I     = ISTOR(NUM)         
    ITYPE = DATA(I)        
    ITYPE = IABS(ITYPE)    
    LABLE = LABEL(NUM)
    IF (ITYPE .EQ.  5) GO TO 710     
    IF (ITYPE .EQ. 10) GO TO 720     
    GO TO 700  
C
C   QUADS FIELD VALUES
C
  710   IA  = NINT(10.0 * DATA(I+3))
    LL  = NINT(1000.0 * DATA(I+1))
    MQ  = MQ + 1
    IF (MQ .GT. 30) GO TO 700
    IF (IQ(MQ) .GT. 2**15-1) IQ(MQ) = IQ(MQ) - 2**16        
    DO 715 K = 1, NREC
     IF (LABLE .EQ. QLAB(K)) THEN
          GO TO 716
     ENDIF
     IF (IA.EQ.JA(K) .AND. LL.EQ.LEFF(K) .AND. QLAB(K).EQ.'%%%%') THEN
          GO TO 716
     ENDIF
  715   CONTINUE
    GO TO 700
C
  716   DATA(I+2) =
     1   GETFIELD(IQ(MQ),A0(K),A1(K),A2(K),A3(K),J0(K),JMAX(K),DMAX(K))
    WRITE(STR1,1010) LABLE
    WRITE(STR2,1011) DATA(I+2)
    QUADD(MQ) = STR1//STR2
    GO TO 700  
C
C   FIT CONSTRAINTS
C
  720   IF (LABLE(1:1).EQ.'X' .AND. LABLE(2:2).EQ.'X') GO TO 700
    L1 = DATA(I+1)    
    L2 = DATA(I+2)         
    IF (L1 .EQ. 1 .AND. L2 .EQ. 1) GO TO 730  
    IF (L1 .EQ. 3 .AND. L2 .EQ. 3) GO TO 740  
    GO TO 700  
  730   MFX = MFX+1
    IF (MFX .GT. 25) GO TO 700
    DATA(I+3) = IM(MFX) /10. + .000005       
    IF (IM(MFX) .EQ. 0) DATA(I+4) = 1000.
    WRITE(STR1,1010) LABLE
    WRITE(STR3,1012) DATA(I+3)
    XDIM(MFX) = STR1//STR3
    GO TO 700  
  740   MFY = MFY + 1
    IF (MFY .GT. 25) GO TO 700
    DATA(I+3) = IV(MFY) /10. + .000005       
    IF (IV(MFY) .EQ. 0) DATA(I+4) = 1000.
    WRITE(STR1,1010) LABLE
    WRITE(STR3,1012) DATA(I+3)
    YDIM(MFY) = STR1//STR3
  700   CONTINUE
C
C   WRITE INFORMATION ON SCREEN AND NEW FILE
C
    IF (MQ.NE.0 .OR. MFX.NE.0 .OR. MFY.NE.0) THEN
         WRITE(6,*)
     1   'The following Quad- and Fit-Data have been transferred:'
     IF (MQ  .NE. 0) WRITE(6,1020) (QUADD(I),I=1,MQ)
     IF (MFX .NE. 0) WRITE(6,1021) (XDIM(I),I=1,MFX)
     IF (MFY .NE. 0) WRITE(6,1021) (YDIM(I),I=1,MFY)
         CALL WRITED(1)
    ENDIF
C
 1010   FORMAT(A4,'=')
 1011   FORMAT(F8.4,' kG  ')
 1012   FORMAT(F5.1,' mm  ')
 1020   FORMAT(' ',4A19)
 1021   FORMAT(' ',5A15)
 2000   FORMAT(1X,A4,2X,A4,2X,I4,2X,I4,7F7.1)
 2002   FORMAT(10I6)
 2221   FORMAT(A100)
 2222   FORMAT(10I10)
    RETURN
    END   

    REAL FUNCTION GETFIELD(IDAC,A0,A1,A2,A3,J0,JMAX,DMAX)
C
    REAL J0, JMAX
C
    SIG = 1.0
    IF (IDAC .LT. 0) SIG = -1.0
    SIG2 = 1.0
    IF (DMAX .LT. 0) SIG2 = -1.0
    AMPS = (ABS(IDAC) / ABS(DMAX)) * JMAX
    B    = A0 * SIG + (AMPS / JMAX) * A1
    IF (AMPS .GT. J0 .AND. AMPS .LE. JMAX) THEN
      X = (AMPS - J0) / (JMAX - J0)
      B = B + (A2 + A3 * X) * X * X
    ENDIF
C
    B = B * SIG * SIG2
    GETFIELD = B
C
    RETURN
    END

