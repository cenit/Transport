        SUBROUTINE FXYGAUSS       
C
        CHARACTER*19 QUADD(30)
        CHARACTER*15 XDIM(30),YDIM(30)
        CHARACTER    STR1*5, STR2*13, STR3*10
        DIMENSION    Q(30),X(30),Y(30)
C
        INCLUDE 'incom.f'
C
        LOGICAL SLITS
C
C       WRITE TIME STAMP
C
        CALL TIMESTAMP(0)
C
C   READ IN BEAMLINE DATA FILE
C           
        CALL READD(SLITS)
C           
C   READ QUAD AND PROFILE DATA
C           
        OPEN(UNIT=22,FILE='FOR022.DAT',STATUS='OLD')
        READ(22,2002,ERR=20,END=20) Q, X, Y
        CLOSE(UNIT=22)
        GO TO 23
   20   WRITE(6,*) 'FOR022.DAT read error'
        CALL SLEEP(3)
        RETURN
   23   CONTINUE
C
C   INSERT DATA IN BEAMLINE
C
        MFX = 0
        MFY = 0
        MQ  = 0
C
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
  710   MQ  = MQ + 1
        IF (MQ .GT. 30) GO TO 700
        DATA(I+2) = Q(MQ) / 1000.
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
C
C       X
C
  730   MFX = MFX+1
        IF (MFX .GT. 30) GO TO 700
        DATA(I+3) = ABS(X(MFX))       
        IF (X(MFX) .LE. 0.0) DATA(I+4) = 999.9
        WRITE(STR1,1010) LABLE
        WRITE(STR3,1012) DATA(I+3)
        XDIM(MFX) = STR1//STR3
        GO TO 700  
C
C       Y
C
  740   MFY = MFY + 1
        IF (MFY .GT. 30) GO TO 700
        DATA(I+3) = ABS(Y(MFY))        
        IF (Y(MFY) .LE. 0.0) DATA(I+4) = 999.9
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
 2002   FORMAT(10F10.1)
 2221   FORMAT(A100)
 2222   FORMAT(10I10)
        RETURN
    END   
