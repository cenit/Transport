      PROGRAM MUSCAT
C
C*********************************************************************
C*    Derivated from TRIUMF's REVMOC 2 by U. Rohrer, SIN, March 1983 *
C*    Adapted for MS Powerstation Fortran by U. Rohrer, PSI, Nov. 93 *
C*    Modified for Input via File written via external Dialog Box    *
C*    from Ttmenu32 (C++) by Urs Rohrer, PSI, Feb. 2001              *
C*    Usage: MUSCAT32 Directory or: MUSCAT32 Directory 1234          *
C*********************************************************************
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      CHARACTER*80 DIR, COMM
      INTEGER*2 ISTATUS
      LOGICAL FROMFILE
C
      CHARACTER*1 INPUT
      COMMON T,FRAC,RHO,PROP(3),Z(3),A(3),RMS0,SIGA,SIGE,P0
      DATA RMS0X,SIGAX,SIGEX,P0X,RHOX,FRACX,P1X,Z1X,P2X,Z2X,P3Z,Z3X,TX
     1/.9382796D0, 31.0D0, -1.0D0, 1.206D0, 8.96D0, 1.0D0,1.0D0,29.0D0,
     2 0.0D0, 0.0D0, 0.0D0, 0.0D0, 1.0D0/
      DATA FROMFILE /.FALSE./
C
C     FIRST PARAMETER IS DIRECTORY NAME TERMINATED BY \
C
      DIR = ' '
      LDIR = 1
      CALL GETARG(1,COMM,ISTATUS)
      IF (ISTATUS .NE. -1) THEN
       IF (COMM(1:1) .NE. ' ') THEN
	DO 97 J = 40, 1, -1
	 IF (COMM(J:J) .NE. ' ') GO TO 98
   97   CONTINUE
   98   DIR = COMM(1:J)
        LDIR = J
       ENDIF
      ENDIF
      OPEN(UNIT=7,STATUS='UNKNOWN',FILE=DIR(1:LDIR)//'MUSCAT.LOG')
      REWIND(UNIT=7)
C
C     IS SECOND PARAMETER THERE AND IS IT 1234 ? YES -> FILE INPUT
C
      CALL GETARG(2,COMM,ISTATUS)
      IF (ISTATUS .NE. -1) THEN
       IF (COMM(1:4) .EQ. '1234') THEN
        FROMFILE = .TRUE.
        CALL SYSTEMQQ('DEL '//DIR(1:LDIR)//'MUSCAT.INP'//CHAR(0))
	WRITE(6,*) 'All output will go to this window and file muscat.log:'
       ENDIF
      ENDIF
C
    1 WRITE(6,*) ' '
      IF (FROMFILE) THEN
       IND = IWAITFOR(DIR(1:LDIR)//'MUSCAT.INP')
       IF (IND .EQ. -1) GO TO 999
       READ(10,1001,ERR=1999) RMS0,SIGA,SIGE
      ELSE
       WRITE(6,1000)
 1000  FORMAT(' Enter: mass (GeV/c2), siga (mb), sige (mb) ',$)
       READ(5,1001,ERR=999) RMS0,SIGA,SIGE
      ENDIF
 1001 FORMAT(3F10.1)
      IF(RMS0 .EQ. 0.0D0) RMS0=RMS0X
      IF(SIGA .EQ. 0.0D0) SIGA=SIGAX
      IF(SIGE .EQ. 0.0D0) SIGE=SIGEX
      IF (FROMFILE) THEN
       READ(10,1001,ERR=1999) P0,RHO,FRAC
      ELSE
       WRITE(6,1002)
 1002  FORMAT(' Enter: p0 (GeV/c), rho (g/cm3), frac ( <=1 ) ',$)
       READ(5,1001) P0,RHO,FRAC
      ENDIF
      IF(P0 .EQ. 0.0D0) P0=P0X
      IF(RHO .EQ. 0.0D0) RHO=RHOX
      IF(FRAC .EQ. 0.0D0) FRAC=FRACX
      IF (FROMFILE) THEN
       READ(10,1004,ERR=1999) T,Z(1),PROP(1),Z(2),PROP(2),Z(3),PROP(3)
       CLOSE(UNIT=10)
      ELSE
       WRITE(6,1003)
 1003  FORMAT(' Enter: length (mm), Z1, prop1, Z2, prop2, Z3, prop3 ',$)
       READ(5,1004) T,Z(1),PROP(1),Z(2),PROP(2),Z(3),PROP(3)
      ENDIF
 1004 FORMAT(7F10.1)
      IF(PROP(1) .EQ. 0.0D0) PROP(1)=P1X
      IF(Z(1). EQ. 0.0D0) Z(1)=Z1X
      IF(PROP(2) .EQ. 0.0D0) PROP(2)=P2X
      IF(Z(2) .EQ. 0.0D0) Z(2)=Z2X
      IF(PROP(3) .EQ. 0.0D0) PROP(3)=P3X
      IF(Z(3) .EQ. 0.0D0) Z(3)=Z3X
      IF(T .EQ. 0.0D0) T=TX
      T=0.1D0*T
      CALL SETUP
      RMS0X=RMS0
      SIGAX=SIGA
      SIGEX=SIGE
      P0X=P0
      RHOX=RHO
      FRACX=FRAC
      P1X=PROP(1)
      Z1X=Z(1)
      P2X=PROP(2)
      Z2X=Z(2)
      P3X=PROP(3)
      Z3X=Z(3)
      TX=T/100.D0
C
      IF (.NOT.FROMFILE) THEN
       WRITE(6,777)
  777  FORMAT(' continue [Y/N] ? ',$)
       READ(5,888) INPUT
  888  FORMAT(A1)
       IF (INPUT.EQ.'N' .OR. INPUT.EQ.'n') GO TO 999
      ENDIF
      GO TO 1
C
C     DELETE INPUT FILE AND EXIT
C
  999 IF (FROMFILE) THEN
       CALL SYSTEMQQ('DEL '//DIR(1:LDIR)//'MUSCAT.INP'//CHAR(0))
      ENDIF
      CALL EXIT
C
 1999 CONTINUE 
      WRITE(6,*) 'Input error, program exits.'
      CLOSE(UNIT=10)
      CALL SYSTEMQQ('DEL '//DIR(1:LDIR)//'MUSCAT.INP'//CHAR(0))
      CALL SLEEPQQ(3000)
      CALL EXIT

      END

      SUBROUTINE ENRG0(CHIA,A3,A4,A5,RAD,DELTA,RMS,PF,EF,BTAF)
C*****************************************************************
C*                                                               *
C*    SUBROUTINE ENRG0 IS USED TO CALCULATE VARIOUS PARAMETERS   *
C*    FOR ENERGY LOSSES.  THESE ARE THEN USED IN ENRG1 WHERE THE *
C*    ENERGY LOSS DISTRIBUTIONS ARE CALCULATED.                  *
C*                                                               *
C*****************************************************************
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      DIMENSION EION(92),AT(92),CE(7),CA(7),CM(7),CX1(7),CX0(7)
      DIMENSION ALPHA(3),R0(3),V0(3)
      COMMON T,FRAC,RHO,PROP(3),Z(3),A(3),RMS0,SIGA,SIGE,P0
      DATA CE/2.83D0,3.18D0,4.21D0,4.38D0,5.09D0,6.16D0,5.89D0/
      DATA CA/0.413D0,0.509D0,0.091D0,0.107D0,0.183D0,0.344D0,0.318D0/
      DATA CM/2.82D0,2.67D0,3.51D0,3.39D0,3.05D0,2.66D0,2.66D0/
      DATA CX1/2*2.0D0,5*3.0D0/
      DATA CX0/-0.1D0,-0.05D0,0.05D0,0.17D0,0.02D0,0.39D0,0.20D0/
      DATA AT/1.008D0,4.003D0,6.940D0,9.013D0,10.82D0,12.01D0,14.01D0,
     1 16.00D0,19.00D0,20.18D0,22.99D0,24.32D0,26.98D0,28.09D0,
     2 30.98D0,32.07D0,35.46D0,39.94D0,39.10D0,40.08D0,44.96D0,
     3 47.90D0,50.95D0,52.01D0,54.94D0,55.85D0,58.94D0,58.69D0,
     4 63.54D0,65.38D0,69.72D0,72.60D0,74.91D0,78.96D0,79.92D0,
     5 83.80D0,85.48D0,87.63D0,88.92D0,91.22D0,92.91D0,95.95D0,
     6 99.00D0,101.1D0,102.9D0,106.7D0,107.9D0,112.4D0,114.8D0,
     7 118.7D0,121.8D0,127.6D0,126.9D0,131.3D0,132.9D0,137.4D0,
     8 138.9D0,140.1D0,140.9D0,144.3D0,145.0D0,150.4D0,152.0D0,
     9 156.9D0,158.9D0,162.5D0,164.9D0,167.2D0,168.9D0,173.0D0,
     A 175.0D0,178.6D0,181.0D0,183.9D0,186.3D0,190.2D0,192.2D0,
     B 195.2D0,197.0D0,200.6D0,204.4D0,207.2D0,209.0D0,210.0D0,
     C 210.0D0,222.0D0,223.0D0,226.0D0,227.0D0,232.0D0,231.0D0,
     D 238.1D0/
C
C    MEAN IONIZATION POTENTIALS FROM NUCL. INSTR & METHODS 168 (1980) P17-24
C
      DATA EION/
     #  19.D0, 42.D0, 47.D0, 63.D0, 75.D0, 79.D0, 86.D0, 99.D0,
     # 119.D0,135.D0,141.D0,149.D0,162.D0,159.D0,169.D0,179.D0,
     # 170.D0,180.D0,189.D0,195.D0,215.D0,228.D0,237.D0,257.D0,
     # 275.D0,284.D0,304.D0,314.D0,330.D0,323.D0,335.D0,323.D0,
     # 355.D0,343.D0,339.D0,347.D0,350.D0,353.D0,365.D0,382.D0,
     # 391.D0,393.D0,416.D0,429.D0,436.D0,456.D0,470.D0,466.D0,
     # 479.D0,512.D0,492.D0,491.D0,452.D0,459.D0,485.D0,486.D0,
     # 494.D0,513.D0,520.D0,540.D0,537.D0,546.D0,547.D0,567.D0,
     # 577.D0,578.D0,612.D0,583.D0,629.D0,637.D0,655.D0,663.D0,
     # 682.D0,695.D0,714.D0,727.D0,744.D0,760.D0,742.D0,768.D0,
     # 765.D0,761.D0,763.D0,765.D0,762.D0,733.D0,762.D0,760.D0,
     # 768.D0,776.D0,807.D0,808.D0 /
      RAD=1.0D-30
      CHIA=0.0D0
      A3=0.0D0
      A4=0.0D0
      EC=0.0D0
      EA=0.0D0
      EM=0.0D0
      EX1=0.0D0
      EX0=0.0D0
      DO 30 J=1,3
      IF (PROP(J) .LE. 0.0D0) GO TO 30
      IZ=Z(J)
C
C     A IS THE ATOMIC MASS NUMBER, IZ IS THE ATOMIC NUMBER OF A COMPONENT
C
      A(J)=AT(IZ)
      RAD=RAD+PROP(J)*(1.4D-3*Z(J)*(Z(J)+1.0D0)
     1 *DLOG(183.0D0/Z(J)**0.33333D0))
     2 /(A(J)*(1.0D0+0.12D0*((Z(J)/82.0D0)**2)))
      B3=PROP(J)*Z(J)/A(J)
      A3=A3+B3
      A4=A4+B3*DLOG(EION(IZ))
      IE=1
      IF (IZ .GT. 4) IE=2
      IF (IZ .GT. 9) IE=3
      IF (IZ .GT. 21) IE=4
      IF (IZ .GT. 38) IE=5
      IF (IZ .GT. 65) IE=6
      IF (IZ .GT. 87) IE=7
      EC=EC+B3*CE(IE)
      EA=EA+B3*CA(IE)
      EM=EM+B3*CM(IE)
      EX1=EX1+B3*CX1(IE)
      EX0=EX0+B3*CX0(IE)
C
C     NEXT FOUR LINES ARE FROM NUCLEAR PHYSICS A197 (1972) P515.
C     EQUATION # 5
C
      ALPHA(J)=Z(J)/(137.*BTAF)
      R0(J)=0.468D-8/Z(J)**0.333333D0
      V0(J)=0.197327D-13/(PF*R0(J))
C
C     The constants 0.59 & 3.44 of the following equation were
C     obtained from ATOMIC DATA and NUCLEAR DATA TABLES 15,477(1975)
C     equation 3g and the two following paragraphs which essentially
C     say that these constants were obtained from fits to data for
C     600-MeV protons and 100-250 MeV pions on C,Al,Cu,Cd,and Pb
C     targets. For REVMOC applications other than these other
C     constants may be more appropriate. (These are good for SIN &
C     TRIUMF applications).
C
      CHIA=CHIA+PROP(J)*V0(J)*DSQRT(0.59D0+3.44D0*ALPHA(J)**2)
   30 CONTINUE
      A4=A4/A3
      A5=DLOG(PF/RMS)
      EC=EC/A3
      EA=EA/A3
      EM=EM/A3
      EX1=EX1/A3
      EX0=EX0/A3
      IF (EX1 .GT. 0.4343D0*A5) EA=EA*(EX1-0.4343D0*A5)**EM
      DELTA=0.0D0
      IF (0.4343D0*A5 .GT. EX0) DELTA=2.0D0*A5-EC+EA
      IF (0.4343D0*A5 .GT. EX1) DELTA=2.0D0*A5-EC
      RETURN
      END

      SUBROUTINE ENRG1(A3,A4,RAD,DELTA,RMS,EF)
C*******************************************************************
C*                                                                 *
C*    SUBROUTINE ENRG1, USING PARAMETERS CALCULATED FROM ENRG0,    *
C*    PROCEEDS TO CALCULATE THE ENERGY LOSS DISTRIBUTIONS AT EACH  *
C*    NON-VACUUM BEAM ELEMENT.  THE DISTRIBUTIONS ARE ACCURATE     * 
C*    ONLY IF THEY LIE IN EITHER THE LANDAU REGION OR THE GAUSSIAN *
C*    REGION.  IF THE ENERGY LOSS LIES IN BETWEEN THE TWO, ENRG1   *
C*    PRINTS OUT A NOTE INFORMING THE USER ABOUT THE PROBLEM.      *
C*    ALSO THE ENERGY LOSS IS INACCURATE UNLESS IT IS MUCH SMALLER *
C*    THAN THE ENTRANCE ENERGY.  ENRG1 PRINTS OUT A NOTE IF THE    *
C*    ENERGY LOSS EXCEEDS 10% OF THE ENTRANCE ENERGY.  SOMETIMES   *
C*    BOTH NOTES ARE PRINTED.  IF THE ENERGY LOSS IS GREATER THAN  *
C*    OR EQUAL TO ENTRANCE ENERGY, THE BEAM ELEMENT IS FAR TOO     *
C*    THICK AND THE PROGRAM CANNOT PROCEED UNLESS THE ELEMENT IS   *
C*    CHOPPED UP INTO SMALLER REGIONS.                             *
C*                                                                 *
C*******************************************************************
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      COMMON T,FRAC,RHO,PROP(3),Z(3),A(3),RMS0,SIGA,SIGE,P0
      DIMENSION V(20)
C
      PF=DSQRT(EF**2-RMS**2)
      A5=DLOG(PF/RMS)
      BTAF=PF/EF
      AC=0.1536D-3*A3*T*RHO/BTAF**2
      QMAX=1.02D-3*PF**2/(RMS**2+0.26D-6+1.02D-3*EF)
      R=AC*(1.0D0-0.5D0*BTAF**2)*QMAX
      G=R/QMAX**2
      IF (G .GE. 10.0D0) GO TO 10
      ENLOSS=AC*(34.92D0+2.0D0*A5-2.0D0*A4+DLOG(AC)-BTAF**2-DELTA)
      DELT=1.48D0*AC
      W=2.9D0*DELT
      IFLAGE=2
      LX=10.0D0*G+1.0D0
      IF (G .LE. 0.1D0) GO TO 20
      CALL STAR
      WRITE(6,140) LX
      WRITE(7,140) LX
      CALL STAR
  140 FORMAT(' Landau  <  Energy loss distribution  <  Gauss:',
     1' divide into',I3,' smaller elements.')
      GO TO 20
   10 ENLOSS=2.0D0*AC*(13.84D0+A5-A4-BTAF**2-0.5D0*DELTA+A5)
      DELT=DSQRT(R*T*RHO)
      W=2.4D0*DELT
      IFLAGE=1
   20 CONTINUE
      DEDX=2.0D0*AC*(13.84D0+A5-A4-BTAF**2-0.5D0*DELTA+A5)
      TF=EF-RMS0
C
C     CHECKS FOR ERROR ARISING IN DEDX
C
      IF (DEDX .GE. TF) GO TO 170
      IF (DEDX .LE. 0.1D0*TF) GO TO 40
      NCORR=DEDX/(0.1D0*TF)+1.0D0
      CALL STAR
      WRITE(6,30) NCORR
      WRITE(7,30) NCORR
      CALL STAR
   30 FORMAT(' Energy loss  >  10% of entrance kinetic energy,'
     1,' divide into',I3,' smaller regions.')
   40 IF (IFLAGE .EQ. 1) THEN
       WRITE(6,150) 1000.*ENLOSS, 1000.*W, 1000.*DEDX
       WRITE(7,150) 1000.*ENLOSS, 1000.*W, 1000.*DEDX
      ENDIF
  150 FORMAT(' *Gauss*   prob. energy loss =',F7.3,', width ='
     1,F7.3,', average loss =',F7.3,' MeV')
      IF (IFLAGE .EQ. 2) THEN
       WRITE(6,160) 1000.*ENLOSS, 1000.*W, 1000.*DEDX
       WRITE(7,160) 1000.*ENLOSS, 1000.*W, 1000.*DEDX
      ENDIF
  160 FORMAT(' Landau prob. energy loss =',F8.4,', width ='
     1,F8.4,', average loss =',F8.4,' MeV')
   50 IF(RMS.GT.0.52D-3)RETURN
      VA=RAD/DLOG(2.0D0)
      VC=VA-1.0D0
      VB=1.0D0
   60 IF (VA-1.0D0) 90,70,80
   70 VB=1.0D0
      GO TO 100
   80 VA=VA-1.0D0
      VB=VB*VA
      GO TO 60
   90 VB=VB/(VA+0.57721D0*VA**2-0.65588D0*VA**3-0.042D0*VA**4+
     1 0.16654D0*VA**5-0.04220D0*VA**6-0.00926D0*VA**7+0.00721D0*VA**8)
  100 CONTINUE
      VA=VC+1.0D0
      BL=VA
      E=EF
      DO 120 I=2,11
      E=E-0.05D0*EF
      Y0=DLOG(EF/E)
      X=1.0D0/VA
      FJ=1.0D0
      DO 110 J=1,10
      FJ=FJ*J
  110 X=X+((-Y0)**J)/((VA+FLOAT(J))*FJ)
      X=X*((Y0)**VA)/VB
  120 V(I)=X
      Y1=Y0
      X1=EXP(-Y1)*Y1**VC
      DO 130 I=1,9
      E=E-0.05D0*EF
      Y0=DLOG(EF/E)
      X0=EXP(-Y0)*Y0**VC
      X=X+0.5D0*(Y0-Y1)*(X0+X1)/VB
      Y1=Y0
      X1=X0
  130 V(I+11)=X
      V(1)=0.0D0
      RETURN
 170  NCOR=DEDX/TF+1.0D0
      NCORR=DEDX/(TF*0.1D0)+1.0D0
      CALL STAR
      IF (FRAC .LT. 1.0D0) GO TO 200
      IF (ISHAPE .LE. 0) THEN
       WRITE(6,180) NCOR, NCORR
       WRITE(7,180) NCOR, NCORR
      ENDIF
  180 FORMAT(' Energy loss >= kinetic energy,  divide into'
     1,I3,', better into',I3,' smaller regions.')
      CALL STAR
      RETURN
 200  WRITE(6,210) 1000.*DEDX, 1000.*TF
      WRITE(7,210) 1000.*DEDX, 1000.*TF
 210  FORMAT(' Energy loss would be',F7.1,
     1' MeV > entrance kinetic energy =',F7.1,' MeV.........')
      CALL STAR
      RETURN
      END

      SUBROUTINE SETUP
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      DIMENSION DILAM(3),AS(3),RMT(3),EPSLON(3),BETACM(3),BETPCM(3)
      DIMENSION PFCM(3),SIMOT(3),SIMET(3),PDI(3)
      COMMON T,FRAC,RHO,PROP(3),Z(3),A(3),RMS0,SIGA,SIGE,P0
C
      E0=DSQRT(P0**2+RMS0**2)
      BTA0=P0/E0
      TF=E0-RMS0
C
C    SET UP FOR NON-VACUUM REGION
C
      IF (3*(ICOUNT/3)-ICOUNT.EQ.0 .AND. ICOUNT.NE.0) THEN
       WRITE(7,1234)
1234   FORMAT('1')
      ELSE
       WRITE(7,1235)
1235   FORMAT('0')
      ENDIF
      ICOUNT = ICOUNT + 1
      CALL POINT(1)
      WRITE(6,270) 100.*FRAC, 10.*T, RHO
      WRITE(7,270) 100.*FRAC, 10.*T, RHO
  270 FORMAT( F7.2,' % of length =',F8.4,' mm covered by material with'
     1,' density =',F7.3,' g/cm3')
      WRITE(6,290) 1000.*TF, Z(1), PROP(1)
      WRITE(7,290) 1000.*TF, Z(1), PROP(1)
  290 FORMAT(' Entr. energy =',F7.1,
     1' MeV, Composition of material:  Z1 =',F5.1,',  prop1 =',F7.4) 
      IF (PROP(2) .GT. 0.0) THEN
       WRITE(6,291) Z(2),PROP(2)
       WRITE(7,291) Z(2),PROP(2)
      ENDIF
  291 FORMAT(' ',51('.'),'  Z2 =',F5.1,',  prop2 =',F7.4) 
      IF(PROP(3).GT.0.0) THEN
       WRITE(6,292) Z(3),PROP(3)
       WRITE(7,292) Z(3),PROP(3)
      ENDIF
  292 FORMAT(' ',51('.'),'  Z3 =',F5.1,',  prop3 =',F7.4) 
      CALL POINT(1)
C
      DO 340 J=1,3
      DILAM(J)=0.0D0
  340 AS(J)=0.0D0
      DELA=1.0D+30
      TLAM=1.0D0/DELA
      IF (T .LE. 0.0D0) GO TO 520
      IF (RHO .LE. 0.0D0) GO TO 480
C
C     CALL ENRG0 TO CALCULATE SOME PARAMETERS FOR ENERGY LOSS
C
      CALL ENRG0(CHIA,A3,A4,A5,RAD,DELTA,RMS0,P0,E0,BTA0)
      X=P0/RMS0
C
C     THE HUNGERFORD'S RELATIVISTIC CORRECTION FOR MULTIPLE SCATTERING
C     REF: NUCLEAR PHYSICS A197 (1972) P515-528
C     NOTE: There was an error in the above paper in the 'DENOM='
C           equation below. In the above reference (equation 11)
C           the last term was *RMS0 . The correct equation is
C           given in ATOMIC DATA and NUCLEAR DATA TABLES 15,477(1975)
C           (equation 3f) with the last term being the target mass
C           RMT(IM).
C
      CHIC=0.0D0
      DO 370 IM=1,3
      IF(A(IM) .LE. 0.0D0) GO TO 370
      RMT(IM) = 0.938256D0*A(IM)
      DENOM=DSQRT(RMS0**2+RMT(IM)**2+2.0D0*E0*RMT(IM))
      EPSLON(IM)=(2.0D0*E0+RMT(IM)+RMS0**2/RMT(IM))/DENOM
      BETACM(IM)=P0/(DSQRT(P0**2+RMS0**2)+RMT(IM))
      BETPCM(IM)=(BTA0-BETACM(IM))/(1.0D0-BTA0*BETACM(IM))
      PFCM(IM)=RMS0*BETPCM(IM)/DSQRT(1.0D0-BETPCM(IM)**2)
C
C     CHIC IS DSQRT(EQUATION 14) OF NUCLEAR PHYSICS A197(1972) P.515
C
      CHIC=CHIC+PROP(IM)*3.96D-4*Z(IM)*DSQRT(RHO*T/A(IM))/
     1(PFCM(IM)*BETPCM(IM)*EPSLON(IM))
  370 CONTINUE
      RAD=RHO*RAD*T
      RADLOS=DSQRT(P0**2+RMS0**2)*(1.0D0-EXP(-RAD))
      RADLOS=RADLOS*(0.51D-3/RMS0)**2
C
C     FOR DEFINITION OF OMEGA,B,BY ETC SEE NUCLEAR PHYSICS A197 (1972)
C     P.515 EQUATION 3
C
      WRITE(6,375)
      WRITE(7,375)
  375 FORMAT(' ......................... Moliere angular',
     1 ' distribution ........................')
      OMEG0=(CHIC/CHIA)**2
      IF (OMEG0 .LT. 5.0D0) GO TO 400
      B=1.153D0+2.583D0*DLOG(OMEG0)/2.3026D0
      IF (B) 400,400,390
  390 S=CHIC*DSQRT(B)
      BY=1.153D0+2.096D0*DLOG(OMEG0)/2.3026D0
      SY=CHIC*DSQRT(BY)/1.7321D0
      SE=CHIC*DSQRT(1.007D0*B-1.33D0)
      SYE=CHIC*DSQRT(1.007D0*BY-1.33D0)*T/1.7321D0
      GO TO 420
  400 S=0.0D0
      SY=0.0D0
      B=1.0D0
      BY=1.0D0
      CALL STAR
      WRITE(6,410) OMEG0
      WRITE(7,410) OMEG0
      CALL STAR
  410 FORMAT(' Number of Rutherford scatters is only ',F5.2,
     1';  Moliere theory is not applicable.')
  420 WRITE(6,580) 1000.*SE, 10.*SYE, RAD, B
      WRITE(7,580) 1000.*SE, 10.*SYE, RAD, B
  580 FORMAT(' 1/e sc.ang =',F8.3,' mr, 1/e disp =',F7.4,
     1' mm, t (rad.leng) =',F7.3,', B =',F6.2)
      CALL POINT(1)
C
C     CALL ENRG1 TO CALCULATE ENERGY LOSS DISTRIBUTIONS.
C
      WRITE(6,595)
      WRITE(7,595)
  595 FORMAT(' ........................... Energy loss',
     1 ' distribution ..........................')
      CALL ENRG1(A3,A4,RAD,DELTA,RMS0,E0)
      IF (RMS0 .LE. 0.52E-3) THEN
       WRITE(6,620) 1000.*RADLOS
       WRITE(7,620) 1000.*RADLOS
      ENDIF
  620 FORMAT(' Radiative energy loss =',F7.1,' MeV ',44('.'))
      CALL POINT(1)
      WRITE(6,625)
      WRITE(7,625)
  625 FORMAT('....................... Nuclear interaction',
     1 ' distribution .......................')
      IF (SIGA) 480,480,450
C
C     CALCULATE TLAM, THE 'INTERACTION LENGTH'.
C     THE INTERACTION LENGTH FOR ABSORPTION FOR THE J-TH
C     COMPONENT OF THE MATERIAL IN ELEMENT, ALAM, IS
C     CALCULATED FROM THE FORMULA:
C
C     ALAM=AG*SIM*RHO*PROP(J)/A(J)
C
C     WHERE,   AG=AVAGADRO,S NUMBER=6.03*10**23
C              SIM=ABSORPTION CROSS SECTION IN MILLIBARNS
C              (I.E. UNITS ARE 10**-27  CM**2)
C              RHO=DENSITY OF COMPONENTS J IN GM/CM**3
C              PROP(J)=PROPORTION OF MATERIAL OF TYPE J
C              A(J)=ATOMIC MASS OF COMPONENT.
C
C     THE INTERACTION LENGTH FOR ELASTIC NUCLEAR SCATTERING
C     FOR EACH COMPONENT, DILAM(J), IS CALCULATED AS ABOVE
C     BY REPLACING SIM BY SIME, THE ELASTIC SCATTERING CROSS-
C     SECTION. THE PROGRAM SUMS ALAM AND DILAM(J) OVER THE
C     COMPONENTS OF THE MATERIAL TO GET TLAM, THE
C     TOTAL INTERACTION LENGTH.
C 
  450 AL=0.0D0
      DO 470 J=1,3
      SIMOT(J)=0.0D0
      SIMET(J)=0.0D0
      IF (PROP(J)) 470,470,460
  460 SIM=44.0D0*((A(J)**0.69)+(A(J)**0.36)*(SIGA-33.0D0)*
     1(0.039D0-0.0009D0*(SIGA-33.0D0)))
      ALAM=6.03D0*SIM*RHO*PROP(J)/A(J)
      ALAM=1.0D-4*ALAM
      AL=AL+ALAM
      SIMOT(J)=SIM
      SIME=SIGE
      IF (SIGE .GE. 0.0D0) GO TO 462
      IF (A(J) .GT. 70.D0) GO TO 464
      SIME=47.0D0*A(J)**.82-42.0D0*A(J)**.67
      GO TO 462
 464  SIME=47.0D0*A(J)**.67
 462  SIMET(J)=SIME
      DILAM(J)=ALAM*SIME/SIM
      AS(J)=3.0D0*(0.1396D0/P0)**2/(A(J)**0.67)
      AS(J)=DSQRT(AS(J))
      TLAM=TLAM+ALAM+DILAM(J)
  470 CONTINUE
  480 TL=TLAM-AL
      AL=AL/TLAM
      DO 490 J=1,3
      PDI(J)=DILAM(J)/TL
      DILAM(J)=0.0D0
      IF (SIGE .EQ. 0.0D0) GO TO 490
      IF (RHO .LE. 0.0D0) GO TO 490
      IF (PDI(J) .GT. 0.0D0) THEN
       WRITE(6,590) J, PDI(J), 1000.*AS(J), SIMET(J), SIMOT(J)
       WRITE(7,590) J, PDI(J), 1000.*AS(J), SIMET(J), SIMOT(J)
      ENDIF
  590 FORMAT(' Atom',I1,': int.prob =',F6.3,', RMS.ang =',
     1F6.1,' mr, el.cs =',F6.1,' mb, ab.cs =',F6.1,' mb')
  490 CONTINUE
      TLAM=1.0D0/TLAM
      IF (TLAM .GT. 999.0D0) GO TO 630 
      WRITE(6,610) TLAM, AL
      WRITE(7,610) TLAM, AL
  610 FORMAT(' Total interaction length ='
     1,F7.3,' cm, .......... absorbtion propability =',F6.3)
      GO TO 640
  630 WRITE(6,611) TLAM/100., AL
      WRITE(7,611) TLAM/100., AL
  611 FORMAT(' Total interaction length ='
     1,F7.1,' m, ........... absorbtion probability =',F6.3)
  640 CALL POINT(1)
  520 CONTINUE
      RETURN
      END

      SUBROUTINE STAR
      WRITE(6,1000)
      WRITE(7,1000)
 1000 FORMAT(' ',79('*'))
      RETURN
      END

      SUBROUTINE POINT(L)
      WRITE(6,1000)
      IF (L .EQ. 1) WRITE(7,1000)
 1000 FORMAT(' ',79('.'))
      RETURN
      END

      SUBROUTINE UNDER(L)
      WRITE(6,1000)
      IF (L .EQ. 1) WRITE(7,1000)
 1000 FORMAT(' ',79('_'))
      RETURN
      END

      INTEGER FUNCTION IWAITFOR(FILE)
C
      USE DFLIB
C
      CHARACTER*(*) FILE
      DATA NUMBOLD /0/
C
    1 OPEN(UNIT=10,STATUS='OLD',NAME=FILE,ERR=999)
      READ(10,998,ERR=999) NUMB
 998  FORMAT(I10)
C
      IF (NUMB .EQ. -1) THEN
       CLOSE(UNIT=10) 
       IWAITFOR = -1
       RETURN
      ELSE IF (NUMB .EQ. NUMBOLD) THEN
       CLOSE(UNIT=10) 
       CALL SLEEPQQ(100)
       NUMBOLD = NUMB
       GO TO 1
      ELSE
       NUMBOLD = NUMB
       IWAITFOR = NUMB
       RETURN
      ENDIF
C
  999 CALL SLEEPQQ(100)
      GO TO 1
C
      END
