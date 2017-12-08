        SUBROUTINE BERO
C
        INCLUDE 'incom.f'
C
        CHARACTER       ATEXT*100, NAM*6, NAM1*6, LAB1*4
        LOGICAL         ANIN, SLITS, EQUAD
        DATA            ANIN  /.FALSE./
        DATA            EQUAD /.FALSE./
C           
C       READ IN DATA FILE
C           
    1   CALL READD(SLITS)
C
C       FIND OLD BRHO, GET NEW BRHO
C
        DO 720 NUM = 1, 20
         I     = ISTOR(NUM)
         ITYPE = DATA(I)
         IF (ITYPE .EQ. 1) GO TO 725
  720   CONTINUE
  725   P    = DATA(I+7)
        BRHO = 33.356*P
C
        WRITE(6,3000) BRHO
 3000   FORMAT(' B*Rho is now =',F8.3,' KGm'/' Enter new B*Rho : ',$)
        BRHON = 0.0
        READ(5,3001) BRHON
 3001   FORMAT(F10.5)
        IF (BRHON .LT. 0.00001) BRHON = BRHO
        P    = BRHON / 33.356
        SCAL = BRHON / BRHO
C
        DO 730 NUM = 1, NEL
         I     = ISTOR(NUM)
         ITYPE = DATA(I)
         IF (ITYPE.EQ.1 .AND. DATA(I+8).NE.0.0) DATA(I+7) = P
  730   CONTINUE
C
C       CHANGE FIELDS OF MAGNETIC ELEMENTS
C
        MAL = 0
        DO 750 NUM = 1, NEL
         I     = ISTOR(NUM)
         ITYPE = DATA(I)
         K     = DATA(I+1)
         IF (ITYPE.EQ.13. .AND. K.EQ.47) ANIN = .FALSE.
         IF (ITYPE.EQ.13. .AND. K.EQ.48) ANIN = .TRUE.
         IF (ITYPE.EQ.16. .AND. K.EQ.95) EQUAD = .FALSE.
         IF (ITYPE.EQ.16. .AND. K.EQ.96) EQUAD = .TRUE.
         IF      (ITYPE.EQ.4 .AND. .NOT.ANIN) THEN
          NAM = '*BEND*'
         ELSE IF (ITYPE.EQ.5 .AND. .NOT.EQUAD) THEN
          NAM = '*QUAD*'
         ELSE IF (ITYPE .EQ. 18) THEN
          NAM = '*SEXT*'
         ELSE IF (ITYPE .EQ. 19) THEN
          NAM = '*SOLO*'
         ELSE
          GO TO 750
         ENDIF
         OLDVAL    = DATA(I+2)
         DATA(I+2) = OLDVAL * SCAL
         MAL       = MAL + 1
         IF (MAL - 2 * (MAL/2) .EQ. 1) THEN
          NAM1 = NAM
          LAB1 = LABEL(NUM)
          OLDV1= OLDVAL
          DTA1 = DATA(I+2)
         ELSE
          WRITE(6,1001) NAM1,LAB1,OLDV1,DTA1,
     1                  NAM,LABEL(NUM),OLDVAL,DATA(I+2)
 1001     FORMAT(A6,2X,A4,F9.5,'  -->',F9.5,4X,
     1           A6,2X,A4,F9.5,'  -->',F9.5)
         ENDIF
  750   CONTINUE
        IF (MAL - 2 * (MAL/2) .EQ. 1) THEN
         WRITE(6,1000) NAM1,LAB1,OLDV1,DTA1
 1000    FORMAT(A6,2X,A4,F9.5,'  -->',F9.5)
        ENDIF
C           
C       WRITE NEW FILE  
C           
        CALL WRITED(1)
C
        RETURN
        END   
