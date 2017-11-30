      PROGRAM MUSCAT
C
C*********************************************************************
C*    Derivated from TRIUMF's REVMOC 2 by U. Rohrer, SIN, March 1983 *
C*    Adapted for MS Powerstation Fortran by U. Rohrer, PSI, Nov. 93 *
C*********************************************************************
C
      CHARACTER*1 INPUT
      COMMON T,FRAC,RHO,PROP(3),Z(3),A(3),RMS0,SIGA,SIGE,P0
      DATA RMS0X,SIGAX,SIGEX,P0X,RHOX,FRACX,P1X,Z1X,P2X,Z2X,P3Z,Z3X,TX
     1/.9382796,31.,-1.,1.206,8.96,1.,1.,29.,0.,0.,0.,0.,0.01/
C
      OPEN(UNIT=7,STATUS='UNKNOWN',FILE='MUSCAT.LOG')
      REWIND(UNIT=7)
C
    1 WRITE(6,*) ' '
      WRITE(6,1000)
 1000 FORMAT('Enter: mass (GeV/c2), siga (mb), sige (mb) ',$)
      READ(5,1001,ERR=999) RMS0,SIGA,SIGE
 1001 FORMAT(3F10.1)
      IF(RMS0.EQ.0.0) RMS0=RMS0X
      IF(SIGA.EQ.0.0) SIGA=SIGAX
      IF(SIGE.EQ.0.0) SIGE=SIGEX
      WRITE(6,1002)
 1002 FORMAT('Enter: p0 (GeV/c), rho (g/cm3), frac ( <=1 ) ',$)
      READ(5,1001) P0,RHO,FRAC
      IF(P0.EQ.0.0) P0=P0X
      IF(RHO.EQ.0.0) RHO=RHOX
      IF(FRAC.EQ.0.0) FRAC=FRACX
      WRITE(6,1003)
 1003 FORMAT('Enter: length (m), Z1, prop1, Z2, prop2, Z3, prop3 ',$)
      READ(5,1004) T,Z(1),PROP(1),Z(2),PROP(2),Z(3),PROP(3)
 1004 FORMAT(7F10.1)
      IF(PROP(1).EQ.0.0) PROP(1)=P1X
      IF(Z(1).EQ.0.0) Z(1)=Z1X
      IF(PROP(2).EQ.0.0) PROP(2)=P2X
      IF(Z(2).EQ.0.0) Z(2)=Z2X
      IF(PROP(3).EQ.0.0) PROP(3)=P3X
      IF(Z(3).EQ.0.0) Z(3)=Z3X
      IF(T.EQ.0.0) T=TX
      T=100.*T
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
      TX=T/100.
C
      WRITE(6,777)
  777 FORMAT(' continue [Y/N] ? ',$)
      READ(5,888) INPUT
  888 FORMAT(A1)
      IF (INPUT.EQ.'N' .OR. INPUT.EQ.'n') GO TO 999
      GO TO 1
C
  999 CALL EXIT(0)
      END

      SUBROUTINE ENRG0(CHIA,A3,A4,A5,RAD,DELTA,RMS,PF,EF,BTAF)
C*****************************************************************
C*                                                               *
C*    SUBROUTINE ENRG0 IS USED TO CALCULATE VARIOUS PARAMETERS   *
C*    FOR ENERGY LOSSES.  THESE ARE THEN USED IN ENRG1 WHERE THE *
C*    ENERGY LOSS DISTRIBUTIONS ARE CALCULATED.                  *
C*                                                               *
C*****************************************************************
      DIMENSION EION(92),AT(92),CE(7),CA(7),CM(7),CX1(7),CX0(7)
      DIMENSION ALPHA(3),R0(3),V0(3)
      COMMON T,FRAC,RHO,PROP(3),Z(3),A(3),RMS0,SIGA,SIGE,P0
      DATA CE/2.83,3.18,4.21,4.38,5.09,6.16,5.89/
      DATA CA/0.413,0.509,0.091,0.107,0.183,0.344,0.318/
      DATA CM/2.82,2.67,3.51,3.39,3.05,2.66,2.66/
      DATA CX1/2*2.0,5*3.0/
      DATA CX0/-0.1,-0.05,0.05,0.17,0.02,0.39,0.20/
      DATA AT/1.008,4.003,6.940,9.013,10.82,12.01,14.01,16.00,19.00,
     120.18,22.99,24.32,26.98,28.09,30.98,32.07,35.46,39.94,39.10,40.08,
     244.96,47.90,50.95,52.01,54.94,55.85,58.94,58.69,63.54,65.38,
     369.72,72.60,74.91,78.96,79.92,83.80,85.48,87.63,88.92,91.22,
     492.91,95.95,99.00,101.1,102.9,106.7,107.9,112.4,114.8,118.7,
     5121.8,127.6,126.9,131.3,132.9,137.4,138.9,140.1,140.9,144.3,
     6145.0,150.4,152.0,156.9,158.9,162.5,164.9,167.2,168.9,173.0,
     7175.0,178.6,181.0,183.9,186.3,190.2,192.2,195.2,197.0,200.6,
     8204.4,207.2,209.0,210.0,210.0,222.0,223.0,226.0,227.0,232.0,
     9231.0,238.1/
C
C    MEAN IONIZATION POTENTIALS FROM NUCL. INSTR & METHODS 168 (1980) P17-24
C
      DATA EION/
     #  19., 42., 47., 63., 75., 79., 86., 99.,119.,135.,
     # 141.,149.,162.,159.,169.,179.,170.,180.,189.,195.,
     # 215.,228.,237.,257.,275.,284.,304.,314.,330.,323.,
     # 335.,323.,355.,343.,339.,347.,350.,353.,365.,382.,
     # 391.,393.,416.,429.,436.,456.,470.,466.,479.,512.,
     # 492.,491.,452.,459.,485.,486.,494.,513.,520.,540.,
     # 537.,546.,547.,567.,577.,578.,612.,583.,629.,637.,
     # 655.,663.,682.,695.,714.,727.,744.,760.,742.,768.,
     # 765.,761.,763.,765.,762.,733.,762.,760.,768.,776.,
     # 807.,808. /
      RAD=1.0E-30
      CHIA=0.
      A3=0.
      A4=0.
      EC=0.
      EA=0.
      EM=0.
      EX1=0.
      EX0=0.
      DO 30 J=1,3
      IF (PROP(J) .LE. 0.0) GO TO 30
      IZ=Z(J)
C
C     A IS THE ATOMIC MASS NUMBER, IZ IS THE ATOMIC NUMBER OF A COMPONENT
C
      A(J)=AT(IZ)
      RAD=RAD+PROP(J)*(1.4E-3*Z(J)*(Z(J)+1.0)*ALOG(183./Z(J)**0.33333))
     1/(A(J)*(1.0+0.12*((Z(J)/82.0)**2)))
      B3=PROP(J)*Z(J)/A(J)
      A3=A3+B3
      A4=A4+B3*ALOG(EION(IZ))
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
      R0(J)=0.468E-8/Z(J)**0.333333
      V0(J)=0.197327E-13/(PF*R0(J))
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
      CHIA=CHIA+PROP(J)*V0(J)*SQRT(0.59+3.44*ALPHA(J)**2)
   30 CONTINUE
      A4=A4/A3
      A5=ALOG(PF/RMS)
      EC=EC/A3
      EA=EA/A3
      EM=EM/A3
      EX1=EX1/A3
      EX0=EX0/A3
      IF (EX1 .GT. 0.4343*A5) EA=EA*(EX1-0.4343*A5)**EM
      DELTA=0.0
      IF (0.4343*A5 .GT. EX0) DELTA=2.0*A5-EC+EA
      IF (0.4343*A5 .GT. EX1) DELTA=2.0*A5-EC
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
      COMMON T,FRAC,RHO,PROP(3),Z(3),A(3),RMS0,SIGA,SIGE,P0
      DIMENSION V(20)
C
      PF=SQRT(EF**2-RMS**2)
      A5=ALOG(PF/RMS)
      BTAF=PF/EF
      AC=0.1536E-3*A3*T*RHO/BTAF**2
      QMAX=1.02E-3*PF**2/(RMS**2+0.26E-6+1.02E-3*EF)
      R=AC*(1.0-0.5*BTAF**2)*QMAX
      G=R/QMAX**2
      IF (G .GE. 10) GO TO 10
      ENLOSS=AC*(34.92+2.0*A5-2.0*A4+ALOG(AC)-BTAF**2-DELTA)
      DELT=1.48*AC
      W=2.9*DELT
      IFLAGE=2
      LX=10*G+1
      IF (G .LE. 0.1) GO TO 20
      CALL STAR
      WRITE(6,140) LX
      WRITE(7,140) LX
      CALL STAR
  140 FORMAT(' Landau  <  Energy loss distribution  <  Gauss:',
     1' divide into',I3,' smaller elements.')
      GO TO 20
   10 ENLOSS=2.0*AC*(13.84+A5-A4-BTAF**2-0.5*DELTA+A5)
      DELT=SQRT(R*T*RHO)
      W=2.4*DELT
      IFLAGE=1
   20 CONTINUE
      DEDX=2.0*AC*(13.84+A5-A4-BTAF**2-0.5*DELTA+A5)
      TF=EF-RMS0
C
C     CHECKS FOR ERROR ARISING IN DEDX
C
      IF (DEDX .GE. TF) GO TO 170
      IF (DEDX .LE. 0.1*TF) GO TO 40
      NCORR=DEDX/(0.1*TF)+1.
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
  160 FORMAT(' *Landau*  prob. energy loss =',F7.3,', width ='
     1,F7.3,', average loss =',F7.3,' MeV')
   50 IF(RMS.GT.0.52E-3)RETURN
      VA=RAD/ALOG(2.0)
      VC=VA-1.0
      VB=1.0
   60 IF (VA-1.0) 90,70,80
   70 VB=1.0
      GO TO 100
   80 VA=VA-1.0
      VB=VB*VA
      GO TO 60
   90 VB=VB/( VA+0.57721*VA**2-0.65588*VA**3-0.042*VA**4+0.16654*VA**5-
     10.04220*VA**6-0.00926*VA**7+0.00721*VA**8)
  100 CONTINUE
      VA=VC+1.0
      BL=VA
      E=EF
      DO 120 I=2,11
      E=E-0.05*EF
      Y0=ALOG(EF/E)
      X=1.0/VA
      FJ=1
      DO 110 J=1,10
      FJ=FJ*J
  110 X=X+((-Y0)**J)/((VA+FLOAT(J))*FJ)
      X=X*((Y0)**VA)/VB
  120 V(I)=X
      Y1=Y0
      X1=EXP(-Y1)*Y1**VC
      DO 130 I=1,9
      E=E-0.05*EF
      Y0=ALOG(EF/E)
      X0=EXP(-Y0)*Y0**VC
      X=X+0.5*(Y0-Y1)*(X0+X1)/VB
      Y1=Y0
      X1=X0
  130 V(I+11)=X
      V(1)=0.
      RETURN
 170  NCOR=DEDX/TF+1.
      NCORR=DEDX/(TF*0.1)+1.
      CALL STAR
      IF (FRAC .LT. 1.0) GO TO 200
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
      DIMENSION DILAM(3),AS(3),RMT(3),EPSLON(3),BETACM(3),BETPCM(3)
      DIMENSION PFCM(3),SIMOT(3),SIMET(3),PDI(3)
      COMMON T,FRAC,RHO,PROP(3),Z(3),A(3),RMS0,SIGA,SIGE,P0
C
      E0=SQRT(P0**2+RMS0**2)
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
      WRITE(6,270) 100.*FRAC, T, RHO
      WRITE(7,270) 100.*FRAC, T, RHO
  270 FORMAT(F7.2,' % of length =',F8.4,' cm covered by material with'
     1,' density =',F7.3,' g/cm3')
      WRITE(6,290) 1000.*TF, Z(1), PROP(1)
      WRITE(7,290) 1000.*TF, Z(1), PROP(1)
  290 FORMAT(' Entr. energy =',F7.1,
     1' MeV, Composition of material:  Z1 =',F5.1,',  prop1 =',F7.4) 
      IF (PROP(2) .GT. 0.0) THEN
       WRITE(6,291) Z(2),PROP(2)
       WRITE(7,291) Z(2),PROP(2)
      ENDIF
  291 FORMAT(1X,51('.'),'  Z2 =',F5.1,',  prop2 =',F7.4) 
      IF(PROP(3).GT.0.0) THEN
       WRITE(6,292) Z(3),PROP(3)
       WRITE(7,292) Z(3),PROP(3)
      ENDIF
  292 FORMAT(1X,51('.'),'  Z3 =',F5.1,',  prop3 =',F7.4) 
      CALL POINT(1)
C
      DO 340 J=1,3
      DILAM(J)=0.0
  340 AS(J)=0.0
      DELA=1.0E+30
      TLAM=1.0/DELA
      IF (T .LE. 0.0) GO TO 520
      IF (RHO .LE. 0.0) GO TO 480
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
      CHIC=0.
      DO 370 IM=1,3
      IF(A(IM).LE.0.) GO TO 370
      RMT(IM)=0.938256*A(IM)
      DENOM=SQRT(RMS0**2+RMT(IM)**2+2.*E0*RMT(IM))
      EPSLON(IM)=(2.*E0+RMT(IM)+RMS0**2/RMT(IM))/DENOM
      BETACM(IM)=P0/(SQRT(P0**2+RMS0**2)+RMT(IM))
      BETPCM(IM)=(BTA0-BETACM(IM))/(1-BTA0*BETACM(IM))
      PFCM(IM)=RMS0*BETPCM(IM)/SQRT(1.-BETPCM(IM)**2)
C
C     CHIC IS SQRT(EQUATION 14) OF NUCLEAR PHYSICS A197(1972) P.515
C
      CHIC=CHIC+PROP(IM)*3.96E-4*Z(IM)*SQRT(RHO*T/A(IM))/
     1(PFCM(IM)*BETPCM(IM)*EPSLON(IM))
  370 CONTINUE
      RAD=RHO*RAD*T
      RADLOS=SQRT(P0**2+RMS0**2)*(1.0-EXP(-RAD))
      RADLOS=RADLOS*(0.51E-3/RMS0)**2
C
C     FOR DEFINITION OF OMEGA,B,BY ETC SEE NUCLEAR PHYSICS A197 (1972)
C     P.515 EQUATION 3
C
      WRITE(6,375)
      WRITE(7,375)
  375 FORMAT(' ......................... Moliere angular',
     1 'distribution .........................')
      OMEG0=(CHIC/CHIA)**2
      IF (OMEG0 .LT. 5.0) GO TO 400
      B=1.153+2.583*ALOG(OMEG0)/2.3026
      IF (B) 400,400,390
  390 S=CHIC*SQRT(B)
      BY=1.153+2.096*ALOG(OMEG0)/2.3026
      SY=CHIC*SQRT(BY)/1.7321
      SE=CHIC*SQRT(1.007*B-1.33)
      SYE=CHIC*SQRT(1.007*BY-1.33)*T/1.7321
      GO TO 420
  400 S=0.0
      SY=0.0
      B=1.0
      BY=1.0
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
     1 'distribution ...........................')
      CALL ENRG1(A3,A4,RAD,DELTA,RMS0,E0)
      IF (RMS0 .LE. 0.52E-3) THEN
       WRITE(6,620) 1000.*RADLOS
       WRITE(7,620) 1000.*RADLOS
      ENDIF
  620 FORMAT(' Radiative energy loss =',F7.1,' MeV ',44('.'))
      CALL POINT(1)
      WRITE(6,625)
      WRITE(7,625)
  625 FORMAT(' ....................... Nuclear interaction',
     1 'distribution .......................')
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
  450 AL=0.    
      DO 470 J=1,3
      SIMOT(J)=0.
      SIMET(J)=0.
      IF (PROP(J)) 470,470,460
  460 SIM=44.0*((A(J)**0.69)+(A(J)**0.36)*(SIGA-33.0)*(0.039-0.0009*
     1(SIGA-33.0)))
      ALAM=6.03*SIM*RHO*PROP(J)/A(J)
      ALAM=1.0E-4*ALAM
      AL=AL+ALAM
      SIMOT(J)=SIM
      SIME=SIGE
      IF (SIGE .GE. 0.) GO TO 462
      IF (A(J) .GT. 70.) GO TO 464
      SIME=47.*A(J)**.82-42.*A(J)**.67
      GO TO 462
 464  SIME=47.*A(J)**.67
 462  SIMET(J)=SIME
      DILAM(J)=ALAM*SIME/SIM
      AS(J)=3.0*(0.1396/P0)**2/(A(J)**0.67)
      AS(J)=SQRT(AS(J))
      TLAM=TLAM+ALAM+DILAM(J)
  470 CONTINUE
  480 TL=TLAM-AL
      AL=AL/TLAM
      DO 490 J=1,3
      PDI(J)=DILAM(J)/TL
      DILAM(J)=0.0
      IF (SIGE .EQ. 0.0) GO TO 490
      IF (RHO .LE. 0.0) GO TO 490
      IF (PDI(J) .GT. 0.0) THEN
       WRITE(6,590) J, PDI(J), 1000.*AS(J), SIMET(J), SIMOT(J)
       WRITE(7,590) J, PDI(J), 1000.*AS(J), SIMET(J), SIMOT(J)
      ENDIF
  590 FORMAT(' Atom',I1,': int.prob =',F6.3,', RMS.ang =',
     1F6.1,' mr, el.cs =',F6.1,' mb, ab.cs =',F6.1,' mb')
  490 CONTINUE
      TLAM=1.0/TLAM
      IF (TLAM .GT. 999.) GO TO 630 
      WRITE(6,610) TLAM, AL
      WRITE(7,610) TLAM, AL
  610 FORMAT(' Total interaction length ='
     1,F7.3,' cm, .......... absorbtion probability =',F6.3)
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
