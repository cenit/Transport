      SUBROUTINE BOI
C
      INCLUDE 'incom.f'
C
      DIMENSION DA(10)
      LOGICAL SLITS
      LOGICAL BEAMC, CORRC, R16C, R26C
      DATA    BEAMC, CORRC, R16C, R26C/.FALSE.,.FALSE.,.FALSE.,.FALSE./
C
C     READ IN BEAMLINE DATA FILE
C
      CALL READD(SLITS)
C
C     TRANSFER INITIAL BEAM CONDITION
C
      OPEN(UNIT=17,FILE='FOR017.DAT',STATUS='OLD',
     X     FORM='UNFORMATTED',ERR=99)
      READ(17,ERR=99) DA
      WRITE(6,1000)
 1000 FORMAT(' The following initial beam parameters are transfered:'/)
      DO 720 NUM=1,NEL
      I=ISTOR(NUM)
      ITYPE=DATA(I)
C      ITYPE=IABS(ITYPE)
      IF(ITYPE.EQ.1 .AND. DATA(I+8).NE.0.0) GO TO 712
      IF(ITYPE.EQ.12) GO TO 716
      ID7=DATA(I+7)
      IF(ITYPE.EQ.14 .AND. ID7.EQ.1) GO TO 717
      IF(ITYPE.EQ.14 .AND. ID7.EQ.2) GO TO 718
      GO TO 720
C
C     BEAM
C
  712 DATA(I+1)=DA(1)+.000005
      WRITE(6,1001) DA(1)
 1001 FORMAT(15X,'X      = ',F10.3)
      DATA(I+2)=DA(2)+.000005
      WRITE(6,1002) DA(2)
 1002 FORMAT(15X,'XP     = ',F10.3)
      DATA(I+3)=DA(3)+.000005
      WRITE(6,1003) DA(3)
 1003 FORMAT(15X,'Y      = ',F10.3)
      DATA(I+4)=DA(4)+.000005
      WRITE(6,1004) DA(4)
 1004 FORMAT(15X,'YP     = ',F10.3)
      DATA(I+6)=DA(10)+.000005
      WRITE(6,1005) DA(10)
 1005 FORMAT(15X,'DP/P   = ',F10.3)
      DATA(I+7)=DA(9)+.000005
      WRITE(6,1006) DA(9)
 1006 FORMAT(15X,'P      = ',F10.5)
      BEAMC = .TRUE.
      GO TO 720
C
C     CORRELATION
C
  716 DATA(I+1)=DA(5)+.000005*SIGN(1.0,DA(5))
      WRITE(6,1007) DA(5)
 1007 FORMAT(15X,'r12    = ',F10.3)
      DATA(I+6)=DA(6)+.000005*SIGN(1.0,DA(6))
      WRITE(6,1008) DA(6)
 1008 FORMAT(15X,'r34    = ',F10.3)
      CORRC = .TRUE.
      GO TO 720
C
C     ARBITRAY MATRIX FOR R16
C
  717 DATA(I+6)=DA(7)+.000005*SIGN(1.0,DA(7))
      WRITE(6,1009) DA(7)
 1009 FORMAT(15X,'R16    = ',F10.3)
      R16C = .TRUE.
      GO TO 720
C
C     ARBITRAY MATRIX FOR R16
C
  718 DATA(I+6)=DA(8)+.000005*SIGN(1.0,DA(8))
      WRITE(6,1010) DA(8)
 1010 FORMAT(15X,'R26    = ',F10.3)
      R26C = .TRUE.
C
C     END OF DO-LOOP
C
  720 CONTINUE
C
C     PRINT WARNING MESSAGES
C
      IF (DA(5).EQ.0.0 .AND. DA(6).EQ.0.0) CORRC = .TRUE.
      IF (DA(7) .EQ. 0.0) R16C = .TRUE.
      IF (DA(8) .EQ. 0.0) R26C = .TRUE.
      WRITE(6,*) ' '
      IF (.NOT.BEAMC) WRITE(6,1020)
      IF (.NOT.CORRC) WRITE(6,1021)
      IF (.NOT.R16C)  WRITE(6,1022)
      IF (.NOT.R26C)  WRITE(6,1023)
 1020 FORMAT('Warning: no beam card found')
 1021 FORMAT('Warning: no card for beam correlations found')
 1022 FORMAT('Warning: no card for 1st line of arbitrary matrix found')
 1023 FORMAT('Warning: no card for 2nd line of arbitrary matrix found')
C
C     WRITE NEW FILE
C
      CALL WRITED(1)
      RETURN
C
C
   99 WRITE(6,999)
  999 FORMAT('You have to run Transport first.')
      CALL SLEEP(3)
      RETURN
      END
