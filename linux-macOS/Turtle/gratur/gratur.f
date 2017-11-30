      PROGRAM GRATUR
C
C     GRAPHIC HISTOGRAM OUTPUT ROUTINE FOR TURTLE
C     Original Version was running under VAX-VMS with PSI-GRAPHX 
C     Recompiled for LINUX in April 1997 by Urs Rohrer (PSI)
C     Graphics is now Done via CernLib's HBOOK/HPLOT package
C
      COMMON /BLOC2/ UNIT(12), NAME(12), NOM(12), NOM1(4)
      COMMON /BLOC6/ NHISTS, NHIST, LIMNO
      COMMON /BLOC7/ MTYPE(100), NENTRY(100), NCOOR(100), INTVLS(100),
     A                BEGIN(100), END(100), STEP(100), ZHIST(100)
     A               ,LTAG(100)
      COMMON /BLOC8/  MTABLE(50000)
      REAL MTABLE
      COMMON /BLOC12/ VALUE(100), XMOM(100), XMOM2(100), NFLAG(100),
     A                SET(100), KFLAG(100), IFLAG(3,9), ZFLAG(9),
     A                WZ(100), UFLAG(100), INVFL, ZC
      LOGICAL SET, IFLAG, UFLAG, INVFL
      COMMON /BLOC13/ RAY(6), RAYC(6), RAYN(6), P, PC, PN, FLD, MASS,
     A                MASSC, MASSN, FC, FN, PSTAR, ZMAX, DKFLAG, Z
      REAL MASS, MASSC, MASSN
      LOGICAL DKFLAG
      COMMON /BLOC16/ KPART(3), ISTOP(3,9998)
      REAL ISTOP
      COMMON /BLOC18/ LOSS, NBLK(20), KFLG(20),
     A                XSTRT(20), XEND(20), LTHX(20), XBIN(20), NUNX(20),
     A                YSTRT(20), YEND(20), LTHY(20), YBIN(20), NUNY(20)
      COMMON /FILCOL/ ICOLH, ICOLB, ICOLP, JCOLH, JCOLB, JCOLP, MODE
      CHARACTER*1     MODE
      common /ionumb/ id, mx, my, xl, yl, xl2, yl2, xl3, yl3, chs, open
      logical open
      common /scrsize/ width, height, xoff, yoff
      COMMON /STATIS/ STAT
      LOGICAL STAT
C
      CHARACTER*4 XLAB(100)
      REAL ZPOS(100)
      CHARACTER*4 LABEL
      character*40 UTI
      LOGICAL LEGAL
      INTEGER NUMB(100)
      CHARACTER INPUT*3, SCLTYP*3, TYP*2
      DATA IOUNIT, SCLTYP, TYP /50, 'lin', 'C1'/
      data id, mx, my, open /10, 2, 3, .FALSE./
      data MODE,ICOLH,ICOLB,ICOLP,JCOLH,JCOLB,JCOLP/'M',2,3,5,4,5,7/
      data width, height /18.,24./
      DATA STAT /.TRUE./
c
      call Header
c
c     Get Screen/Paper Size
c
   11 write(6,1015) width, height
 1015 format(
     &'Enter Width, Height of Screen in cm (D=',f4.1,',',f4.1,') ',$)
      read(5,1016,err=11) x, y
 1016 format(2f6.2)
      if (x .ne. 0.0) width  = x
      if (y .ne. 0.0) height = y
c
c     Get Zone Parameters for splitting picture
c
   22 write(6,1017) mx, my
 1017 format('Enter number of x and y zones (D=',i1,',',i1,') ',$)
      read(5,1018,err=22) m1, m2
 1018 format(2i2)
      if (m1 .ne. 0) mx = m1
      if (m2 .ne. 0) my = m2
c
c     scaling stuff
c
      if (mx.eq.1 .and. my.eq.1) then
       xoff = 2.0
       yoff = 2.0
       wx = width - (mx+1) * xoff
       if (wx < 2.0) then
        write(6,*) 'Width is to small !!'
        go to 11
       endif
       wy = height - (my+1) * yoff
       if (wy < 2.0) then
        write(6,*) 'Height is to small !!'
        go to 11
       endif
       xs = xoff +  wx / mx
       ys = yoff +  wy / my
       xl = xoff
       yl = height - 0.9 * yoff
       xl2 = xl + 0.5 * xs
       yl2 = yl - 0.97 * ys     
       chs = 0.28
       xl3 = xl + xs - xoff - 13 * chs
       yl3 = yl - 2.5 * chs
      else
       xoff = width / (5. * mx)
       yoff = height / (5. * my)
       wx = width - (mx+1) * xoff
       if (wx < 2.0) then
        write(6,*) 'Width is to small !!'
        go to 11
       endif
       wy = height - (my+1) * yoff
       if (wy < 2.0) then
        write(6,*) 'Height is to small !!'
        go to 11
       endif
       xs = xoff + wx / mx
       ys = yoff + wy / my
       xl = xoff
       yl = height - 0.9 * yoff
       xl2 = xl + 0.5 * xs
       yl2 = yl - 0.90 * ys     
       mm  = max(mx,my)
       chs = wx /(28*mm)
       xl3 = xl + xs - xoff - 13 * chs - max(0,my-mx) * chs
       yl3 = yl - 2.5 * chs - max(0,mx-my) * chs
      endif

      xl0  = xl
      yl0  = yl
      xl20 = xl2
      yl20 = yl2
      xl30 = xl3
      yl30 = yl3
c
c     Get colors
c
      write(6,1019)
 1019 format('Fill color indices are:'/
     & '2: red, 3: green, 4: blue, 5: yellow, 6: magenta, 7: cyan')

   33 write(6,1020) ICOLH, ICOLB, ICOLP
 1020 format('For 1-dim Histograms:'/
     &  'Enter color indices for Histogram, Zone and Picture (D=',
     &  I1,',',I1,',',I1,') ',$)
      read(5,1022,err=33) I1,I2,I3

   44 write(6,1021) JCOLH, JCOLB, JCOLP
 1021 format('For 3-dim Histograms:'/
     &  'Enter 3-dim indices for Histogram, Zone and Picture (D=',
     &  I1,',',I1,',',I1,') ',$)
      read(5,1022,err=44) J1,J2,J3
 1022 format(3I2)

      if (I1 .ne. 0) ICOLH = I1
      if (I2 .ne. 0) ICOLB = I2
      if (I3 .ne. 0) ICOLP = I3
      if (J1 .ne. 0) JCOLH = J1
      if (J2 .ne. 0) JCOLB = J2
      if (J3 .ne. 0) JCOLP = J3
      ICOLH = ICOLH + 1100
      ICOLB = ICOLB + 1100
      ICOLP = ICOLP + 1100
      JCOLH = JCOLH + 1100
      JCOLB = JCOLB + 1100
      JCOLP = JCOLP + 1100
C
C     READ DATA ON STORING FILE (FOR050.DAT)
C
      OPEN(UNIT=IOUNIT,ERR=10,FILE='FOR050.DAT',STATUS='OLD')
      READ(IOUNIT,1235)  NHISTS,LIMNO,LOSS,NORAYS
 1235 FORMAT(4I10)
      READ(IOUNIT,1236)  (MTABLE(I),I=1,LIMNO)
 1236 FORMAT(10F13.3)
      READ(IOUNIT,1237)  (XMOM(J),J=1,NHISTS)
 1237 FORMAT(10E13.5)
      READ(IOUNIT,1237)  (XMOM2(K),K=1,NHISTS)
      READ(IOUNIT,1236)  (ZPOS(K),K=1,NHISTS)
      READ(IOUNIT,1248)  (XLAB(K),K=1,NHISTS)
 1248 FORMAT(10(1X,A4))
      READ(IOUNIT,1236) ((ISTOP(K,J),K=1,3),J=1,9998)
      READ(IOUNIT,1238) UNIT,NAME,NOM,NOM1
 1238 FORMAT(12E13.5,/,12(A4,1X),/,12(A4,1X),/,12(A4,1X))
      READ(IOUNIT,1239) (MTYPE(I),NENTRY(I),NCOOR(I),INTVLS(I),
     X BEGIN(I),END(I),STEP(I),ZHIST(I),I=1,NHISTS)
 1239 FORMAT(I2,I6,2I4,4E13.5)
      READ(IOUNIT,1234) (NBLK(I),KFLG(I),XSTRT(I),XEND(I),LTHX(I),
     X XBIN(I),NUNX(I),YSTRT(I),YEND(I),LTHY(I),YBIN(I),NUNY(I),
     X I=1,LOSS)
 1234 FORMAT(I6,I2,2E13.5,I4,E13.5,I3,2E13.5,I4,E13.5,I3)
      CLOSE(UNIT=IOUNIT)
      go to 15
C
   10 write(6,'(/''-->    FOR050.DAT not found, CR for exit '',$)')
      read(5,'(i2)') ijk
      GO TO 999
C
   15 NHIST = 0
      DO 30 I = 1, NHISTS
      NAVEL = MTYPE(I)
      IF (NAVEL .EQ. 3) GO TO 30
      NHIST = NHIST + 1
      NUMB(NHIST) = I
   30 CONTINUE
      IF (MTYPE(NHISTS) .EQ. 2) NHIST = NHIST - 1 ! OVERFLOW
      WRITE(6,2000) NHIST + LOSS
 2000 FORMAT(/'There are',I3,' histograms stored on FOR050.DAT')
      WRITE(6,2100) NORAYS
 2100 FORMAT('Following histograms refer to',I7,' rays')
C
      NH = 0
    1 NH = NH + 1
      IF (NH .GT. NHIST + LOSS) NH = 1

   55 CONTINUE
      WRITE(6,7777)
 7777 FORMAT('Next entry: Add 1000 to toggle hist statistics on/off')
      WRITE(6,1000) NH
 1000 FORMAT(
     &'Enter Histogram_number [neg -> clear screen] (D =',I3,') ',$)
      READ(5,1001,err=55) IK
      if (IK .LT. 0) then
        IK  = -IK
        id  = 10  ! --> clears screen
        xl  = xl0 ! --> reset coordinates
        yl  = yl0
        xl2 = xl20
        yl2 = yl20
        xl3 = xl30
        yl3 = yl30
      endif
      if (IK .GT. 1000) then
       IK = IK - 1000
       if (STAT) THEN
        STAT = .FALSE.
       else
        STAT = .TRUE.
       endif
      endif
 1001 FORMAT(I5)
      IF (IK .GT. NHIST + LOSS) GO TO 999

   66 WRITE(6,1002) SCLTYP
 1002 FORMAT('Enter',
     X' LIN or LOG for type of vertical scaling (D = ',A3,') ',$)
      READ(5,1003,err=66) INPUT
 1003 FORMAT(A3)
      call toupper(INPUT)
      IF (INPUT .EQ. 'LIN') SCLTYP = 'lin'
      IF (INPUT .EQ. 'LOG') SCLTYP = 'log'
      IF (IK .NE. 0) NH = IK
      IF (NH.GT.NHIST .AND. NH.LE.NHIST+LOSS) GO TO 400
C
C     NORMAL HISTOGRAMMS
C
      K = NUMB(NH)
2     M = NENTRY(K)
      N = INTVLS(K)
      NUNIT = NCOOR(K)
      CALL HSTUN (NUNIT, FACT, LEGAL)
      XBEG = BEGIN(K) / FACT
      DX = STEP(K) / FACT
      ZC = ZHIST(K)/UNIT(8)
      KFL = KFLAG(K) + 1
      NFL = NFLAG(K)
      IF(UFLAG(K)) NFL = - NFL
      NAVEL = MTYPE(K)
      LABEL = XLAB(K)
      ZZZZZ = ZPOS(K)
      GO TO (100,200,300), NAVEL
C
C     ONE DIMENSIONAL HISTOGRAM
C
  100 CONTINUE
      SUM = MTABLE(M+N+2) - MTABLE(M+N+1) - MTABLE(M+N)
      IF(SUM .EQ. 0.0) GO TO 110
      XM = XMOM(K) / (SUM * FACT)
      ARGUM = XMOM2(K) / (SUM * FACT ** 2) - XM ** 2
      IF(ARGUM.LT.0.0) ARGUM = 0.0
      XM2 = SQRT( ARGUM )
      CALL MEAN(MTABLE(M), XBEG, N, DX, SUM, XM, XM2, IER)
      GO TO 111
  110 XM = 0.0
      XM2 = 0.0
      SUM = 0.0
  111 CALL HIST(MTABLE(M),N,XBEG,DX,SCLTYP,NUNIT,NH,ZZZZZ,LABEL
     X          ,XM,XM2,SUM)
      GO TO 1
C
C     X COORDINATE OF TWO DIMENSIONAL HISTOGRAM
C
  200 NA = N
      NUNA = NUNIT
      ZA = ZC
      AMIN = XBEG
      DA = DX
      KFLA = KFL
      NFLA = NFL
      K = K + 1
      GO TO 2
C
C     CONTOUR- OR LEGO-PLOT OF 2-DIM HISTOGRAM
C
  300 WRITE(6,1010) TYP
 1010 FORMAT('Enter',
     &  ' C1, C2, CC, CS, L, L1, L2,',
     &  ' S, S1, S2, S3, S4 (D = ',A2,') ',$)
      INPUT = ' '
      READ(5,1003,err=300) INPUT
      call toupper(INPUT(1:2))
      if (INPUT .ne. ' ') then
        TYP = INPUT(1:2)
      endif
      call GetTit(TYP,UTI)
      WRITE(6,'(''Selected: '',A40)') UTI
C
      IF (
     &    TYP.EQ.'C1' .or.
     &    TYP.EQ.'C2' .or.
     &    TYP.EQ.'CC' .or.
     &    TYP.EQ.'CS'
     &   ) THEN

        CALL CONTPL
     &  (MTABLE(M),AMIN,NA,DA,NUNA,XBEG,N,DX,NUNIT,SCLTYP,NH,TYP,
     &   ZZZZZ,LABEL)

      ELSE IF (
     &         TYP .EQ. 'L ' .or.
     &         TYP .EQ. 'L1' .or.
     &         TYP .EQ. 'L2' .or.
     &         TYP .EQ. 'S ' .or.
     &         TYP .EQ. 'S1' .or.
     &         TYP .EQ. 'S2' .or.
     &         TYP .EQ. 'S3' .or.
     &         TYP .EQ. 'S4'
     &        ) THEN

        CALL LEGOPL
     &  (MTABLE(M),AMIN,NA,DA,NUNA,XBEG,N,DX,NUNIT,SCLTYP,NH,TYP,
     &   ZZZZZ,LABEL)

      ENDIF
      GO TO 1
C
C     HISTOGRAMS OF PARTICLES LOST
C
  400 K = NH - NHIST
      KFL = KFLG(K) + 1
      M = NBLK(K)
      N = LTHX(K)
      NUNIT = NUNX(K)
      CALL HSTUN (NUNIT, FACT, LEGAL)
      XBEG = XSTRT(K) / FACT
      DX = XBIN(K) / FACT
      NUND = NUNY(K)
      LABEL = '&&&&&' ! NO LABEL FOR DISPLAY
      ZZZZZ = 0.0     ! DUMMY
      IF(NUND .NE. 0) GO TO 600
C
C     Z DISTRIBUTION
C
      CALL MEAN(MTABLE(M), XBEG/FACT, N, DX/FACT, SUM, XM, XM2, IER)
      IF (IER .EQ. 0) GO TO 111
      GO TO 110
C
C     Z VERSUS ANOTHER COORDINATE
C
  600 ND   = LTHY (K)
      CALL HSTUN (NUND, FACT, LEGAL)
      DMIN = YSTRT(K) / FACT
      DD = YBIN(K) / FACT

      AMIN = XBEG
      NA   = N
      DA   = DX
      NUNA = NUNIT
      XBEG = DMIN
      N    = ND
      DX   = DD
      NUNIT= NUND
      GO TO 300
C
  999 call hplend
      CALL EXIT
      END

      SUBROUTINE HIST(ARRAY,N,XBEG,DX,SCLTYP,NUNIT,NH,Z,LABEL,
     x                XMOM,XMOM2,SUM)
C
      COMMON /FILCOL/ ICOLH, ICOLB, ICOLP, JCOLH, JCOLB, JCOLP, MODE
      CHARACTER*1     MODE
      common /ionumb/ id, mx, my, xl, yl, xl2, yl2, xl3, yl3, chs, open
      logical open
      COMMON /STATIS/ STAT
      LOGICAL STAT
      CHARACTER*4 LABEL
      CHARACTER*40 TEMP
C
      REAL ARRAY(1), X(0:1000), Y(0:1000)
      CHARACTER SCLTYP*3, XACHSE*11, TITEL*41, INPUT*6
      DATA TITEL /'Histogram xxx (lin) at z=xxx.xxx m (AAAA)'/
C
   77 WRITE(6,1002) MODE
 1002 FORMAT('Enter',
     &' M(onochrome) or C(olored) (D = ',A1,') ',$)
      READ(5,1003,err=77) INPUT
 1003 FORMAT(A3)
      call toupper(INPUT)
      IF (INPUT .EQ. 'M') MODE = 'M'
      IF (INPUT .EQ. 'C') MODE = 'C'
C
      DO 100 I = 1, N
        Y(I) = ARRAY(I)
        IF (SCLTYP .EQ. 'log') Y(I) = Y(I) + 1.
        X(I) = XBEG + 0.5 * DX + FLOAT(I-1) * DX
  100 CONTINUE
c
      CALL GraOpen
c
      WRITE(TITEL(11:13),5000) NH
 5000 FORMAT(I3)
      TITEL(16:18) = SCLTYP
      NNN =19
      IF (LABEL .NE. '&&&&') THEN
        WRITE(TITEL(26:32),'(F7.3)') Z
        NNN = 35
        IF (LABEL .NE. '$$$$') THEN
          TITEL(37:40) = LABEL
          NNN =41
        ENDIF
      ENDIF
      call hbook1(id,' ',N,XBEG,XBEG+float(N)*DX,0.)
      do i = 1, N
        call hfill(id,X(I),0.,Y(I))
      enddo
c
      call hplopt('DATE',1)
      call hplset('DATE',2.)     
c
      call OpenPS
c      call htitle('Graphic Turtle Output')
      if (MODE .eq. 'C') then
        call hplset('PCOL',float(ICOLP))
        call hplset('HCOL',float(ICOLH))
        call hplset('BCOL',float(ICOLB))
      else
        call hplset('PCOL',0.)       ! mono
        call hplset('HCOL',0.)       ! mono
        call hplset('HTYP',354.)    ! /////
        call hplset('BCOL',0.)       ! mono
      endif
      IF (SCLTYP .EQ. 'lin') then
        call hplopt('LINY',1)
      else
        call hplopt('LOGY',1)
      endif
      call hplset('YLAB',0.6)
      call CheckZone
      call hplot(id,' ',' ',0)
      call hplcom(xl,yl,TITEL(1:NNN))
      CALL XCAP(NUNIT,XACHSE)
      call hplcom(xl2,yl2,XACHSE)
c
      IF ( (SUM .GT. 0.0) .AND. STAT) THEN
       WRITE(TEMP,333,ERR=1333) XMOM
  333  FORMAT('Mean=',G8.3)
 1333  WRITE(TEMP,2333) XMOM
 2333  FORMAT('Mean=',F8.3)
       call hplcom(xl3,yl3,TEMP)
       WRITE(TEMP,334) XMOM2
  334  FORMAT('RMS=',G8.3)
       call hplcom(xl3,yl3-1.5*chs,TEMP)
       WRITE(TEMP,335) SUM
  335  FORMAT('Sum=',G8.3)
       call hplcom(xl3,yl3-3*chs,TEMP)
      ENDIF
c      
      call GraClose
      RETURN
      END

      SUBROUTINE
     &  CONTPL(ARRAY,XBEG,NX,DX,NUNX,YBEG,NY,DY,NUNY,SCLTYP,NH,TYP,
     &         Z,LABEL)
C
      COMMON /FILCOL/ ICOLH, ICOLB, ICOLP, JCOLH, JCOLB, JCOLP, MODE
      CHARACTER*1     MODE
      common /ionumb/ id, mx, my, xl, yl, xl2, yl2, xl3, yl3, chs, open
      logical open
      COMMON /STATIS/ STAT
      LOGICAL STAT
      CHARACTER*4 LABEL
      CHARACTER*40 TEMP
C
      REAL ARRAY(1), FIELD(100,100), PX(100), PY(100)
      CHARACTER SCLTYP*3, XACHSE*11, YACHSE*11, TITEL*41, TYP*2
      dimension par(6)
      DATA TITEL /'Histogram xxx (lin) at z=xxx.xxx m (AAAA)'/
      DATA NNIV /10/
C
      MM = 0
      DO 200 I = 1, NY
        DO 100 J = 1, NX
          MM = MM + 1
          IF (SCLTYP .EQ. 'log') THEN
            FIELD(J,I) = LOG10(ARRAY(MM) + 1.)
          ELSE
            FIELD(J,I) = ARRAY(MM)
          ENDIF
  100   CONTINUE
        MM = MM + 1	!	SKIP SUM
  200 CONTINUE
c
      if (TYP .eq. 'C1' .or. TYP .eq. 'C2') then
  300   WRITE(6,1000) NNIV
 1000   FORMAT('Enter number of contour lines (D =',I3,') ',$)
        READ(5,1001,err=300) I
 1001   FORMAT(I3)
        IF (I .NE. 0) NNIV = I
      endif
c
      DO 400 I = 1, NX
        PX(I) = XBEG + 0.5 * DX + FLOAT(I-1) * DX
  400 CONTINUE
      DO 500 I = 1, NY
        PY(I) = YBEG + 0.5 * DY + FLOAT(I-1) * DY
  500 CONTINUE
c
      CALL GraOpen
c
      WRITE(TITEL(11:13),5000) NH
 5000 FORMAT(I3)
      TITEL(16:18) = SCLTYP
      NNN =19
      IF (LABEL .NE. '&&&&') THEN
        WRITE(TITEL(26:32),'(F7.3)') Z
        NNN = 35
        IF (LABEL .NE. '$$$$') THEN
          TITEL(37:40) = LABEL
          NNN =41
        ENDIF
      ENDIF
      call hbook2(id,' ',NX,XBEG,XBEG+float(NX)*DX,
     &                   NY,YBEG,YBEG+float(NY)*DY,0.)
      do i = 1, NX
        do j = 1, NY
          call hfill(id,PX(i),PY(j),FIELD(i,j))
        enddo
      enddo
c     
      call hplopt('DATE',1)
      call hplset('DATE',2.)     
c
      call OpenPS
c      call htitle('Graphic Turtle Output')
      if (MODE .eq. 'C') then
        call hplset('PCOL',float(JCOLP))
        call hplset('HCOL',float(JCOLH))
        call hplset('BCOL',float(JCOLB))
      else
        call hplset('PCOL',0.)       ! mono
        call hplset('HCOL',0.)       ! mono
        call hplset('BCOL',0.)       ! mono
      endif
      call hplopt('LINY',1)
      IF (SCLTYP .EQ. 'log') then
        call hplopt('LOGZ',1)
      else
        call hplopt('LINZ',1)
      endif
      call hplset('YLAB',0.6)
      call CheckZone
      if (TYP .eq. 'C1') then
        Noption = 1
        call hplcon(id,NNIV,Noption)
      else if (TYP .eq. 'C2') then
        Noption = 0
        call hplcon(id,NNIV,Noption)
      else if (TYP .eq. 'CC') then
        par(1) = 0
        call hpltab(id,1,par,'COL')
      else
        call hplot(id,' ',' ',0)
      endif
      CALL XCAP(NUNY,YACHSE)
      CALL XCAP(NUNX,XACHSE)
      call hplax(' ',YACHSE)
      call hplcom(xl,yl,TITEL(1:NNN))
      call hplcom(xl2,yl2,XACHSE)
c
      IF (STAT) THEN
       CALL ELLIPS(ARRAY(1), XBEG,NX,DX, YBEG,NY,DY,
     X             SUM, XM, XXM, YM, YYM, R12, EPS, IER)
       IF ( (IER .EQ. 0) .AND. (SUM .GT. 0.0) ) THEN
        WRITE(TEMP,333,ERR=1333) YM
  333   FORMAT('YMean=',G8.3)
 1333   WRITE(TEMP,2333) YM
 2333   FORMAT('YMean=',F8.3)
        call hplcom(xl3,yl3,TEMP)
        WRITE(TEMP,334) YYM
  334   FORMAT('YRMS=',G8.3)
        call hplcom(xl3,yl3-1.5*chs,TEMP)
        WRITE(TEMP,335) R12
  335   FORMAT('Corr=',F8.4)
        call hplcom(xl3,yl3-3*chs,TEMP)

        WRITE(TEMP,444,ERR=1444) XM
  444   FORMAT('XMean=',G8.3)
 1444   WRITE(TEMP,2444) XM
 2444   FORMAT('XMean=',F8.3)
        call hplcom(xl3-13*chs,yl3,TEMP)
        WRITE(TEMP,445) XXM
  445   FORMAT('XRMS=',G8.3)
        call hplcom(xl3-13*chs,yl3-1.5*chs,TEMP)
        WRITE(TEMP,446) EPS
  446   FORMAT('Area=',G8.3)
        call hplcom(xl3-13*chs,yl3-3*chs,TEMP)
       ENDIF
      ENDIF
C
      call GraClose
      RETURN
      END

      SUBROUTINE
     &  LEGOPL(ARRAY,XBEG,NX,DX,NUNX,YBEG,NY,DY,NUNY,SCLTYP,NH,TYP,
     &         Z,LABEL)
C
      COMMON /FILCOL/ ICOLH, ICOLB, ICOLP, JCOLH, JCOLB, JCOLP, MODE 
      CHARACTER*1     MODE
      common /ionumb/ id, mx, my, xl, yl, xl2, yl2, xl3, yl3, chs, open
      logical open
      COMMON /STATIS/ STAT
      LOGICAL STAT
      CHARACTER*4 LABEL
      CHARACTER*40 TEMP
C
      REAL ARRAY(1), FIELD(100,100), PX(100), PY(100)
      CHARACTER SCLTYP*3, XACHSE*11, YACHSE*11, TITEL*41, MOD*1, INP*1
      CHARACTER*2 TYP
      dimension par(6)
      DATA TITEL /'Histogram xxx (lin) at z=xxx.xxx m (AAAA)'/
      DATA PHI, TET /45., 30./
C
   10 WRITE(6,1000) PHI, TET
 1000 FORMAT('Enter Phi (D =',F5.0,') and Theta (D =',F5.0,') ',$)
      READ(5,1001,err=10) PAR1, PAR2
 1001 FORMAT(2F5.0)
      IF (PAR1 .NE. 0.0) PHI = PAR1
      IF (PAR2 .NE. 0.0) TET = PAR2
      PHI = MIN(359.,MAX(1.,PHI))
      TET = MIN(89.,MAX(1.,TET))
C
      MM = 0
      DO 200 I = 1, NY
        DO 100 J = 1, NX
          MM = MM + 1
          FIELD(J,I) = ARRAY(MM)
  100   CONTINUE
        MM = MM + 1	!	SKIP SUM
  200 CONTINUE
C
      DO 400 J = 1, NX
        PX(J) = XBEG + 0.5 * DX + FLOAT(J-1) * DX
  400 CONTINUE
      DO 500 I = 1, NY
        PY(I) = YBEG + 0.5 * DY + FLOAT(I-1) * DY
  500 CONTINUE
c
      CALL GraOpen
c
      WRITE(TITEL(11:13),5000) NH
 5000 FORMAT(I3)
      TITEL(16:18) = SCLTYP
      NNN =19
      IF (LABEL .NE. '&&&&') THEN
        WRITE(TITEL(26:32),'(F7.3)') Z
        NNN = 35
        IF (LABEL .NE. '$$$$') THEN
          TITEL(37:40) = LABEL
          NNN =41
        ENDIF
      ENDIF
      call hbook2(id,' ',NX,XBEG,XBEG+float(NX)*DX,
     &                   NY,YBEG,YBEG+float(NY)*DY,0.)
      do i = 1, NX
        do j = 1, NY
          call hfill(id,PX(i),PY(j),FIELD(i,j))
        enddo
      enddo
c     
      call hplopt('DATE',1)
      call hplset('DATE',2.)     
c
      call OpenPS
c      call htitle('Graphic Turtle Output')
      if (MODE .eq. 'C') then
        call hplset('PCOL',float(JCOLP))
        call hplset('HCOL',float(JCOLH))
        call hplset('BCOL',float(JCOLB))
      else
        call hplset('PCOL',0.)       ! mono
        call hplset('HCOL',0.)       ! mono
        call hplset('BCOL',0.)       ! mono
      endif
      call hplopt('LINY',1)
      IF (SCLTYP .EQ. 'log') then
        call hplopt('LOGZ',1)
      else
        call hplopt('LINZ',1)
      endif
      par(1) = TET
      par(2) = PHI
      par(3) = 0.
      call hplset('YLAB',0.8)
      call CheckZone
      if (TYP .eq. 'L ') then
        call hpltab(id,3,par,'LEGO')
      else if (TYP .eq. 'L1') then
        call hpltab(id,3,par,'LEGO1')
      else if (TYP .eq. 'L2') then
        call hpltab(id,3,par,'LEGO2')
      else if (TYP .eq. 'S ') then
        call hpltab(id,3,par,'SURF')
      else if (TYP .eq. 'S1') then
        call hpltab(id,3,par,'SURF1')
      else if (TYP .eq. 'S2') then
        call hpltab(id,3,par,'SURF2')
      else if (TYP .eq. 'S3') then
        call hpltab(id,3,par,'SURF3')
      else if (TYP .eq. 'S4') then
        call hpltab(id,3,par,'SURF4')
      endif
      CALL XCAP(NUNY,YACHSE)
      CALL XCAP(NUNX,XACHSE)
      if (mx.le.2 .or. my.le.2) then
       call hplax(XACHSE,' ')
      else
c       call hplcom(xl2,yl2,XACHSE)
      endif
      call hplcom(xl,yl,TITEL(1:NNN))
C
      IF (STAT) THEN
       CALL ELLIPS(ARRAY(1), XBEG,NX,DX, YBEG,NY,DY,
     X             SUM, XM, XXM, YM, YYM, R12, EPS, IER)
       IF ( (IER .EQ. 0) .AND. (SUM .GT. 0.0) ) THEN
	if ( (mx .eq. 1) .and. (my .eq. 1)) then
	  u = 6 * chs
	  v = 4 * chs
          WRITE(TEMP,332) SUM
  332     FORMAT('Sum=',G8.3)
          call hplcom(xl3+u,yl3-4.5*chs+v,TEMP)
	else
	  u = 0.0
          v = 0.0
	endif
        WRITE(TEMP,333,ERR=1333) YM
  333   FORMAT('Mean2=',G8.3)
 1333   WRITE(TEMP,2333) YM
 2333   FORMAT('Mean2=',F8.3)
        call hplcom(xl3+u,yl3+v,TEMP)
        WRITE(TEMP,334) YYM
  334   FORMAT('RMS2=',G8.3E1)
        call hplcom(xl3+u,yl3-1.5*chs+v,TEMP)
        WRITE(TEMP,335) R12
  335   FORMAT('Corr=',F8.4)
        call hplcom(xl3+u,yl3-3*chs+v,TEMP)

        WRITE(TEMP,444,ERR=1444) XM
  444   FORMAT('Mean1=',G8.3)
 1444   WRITE(TEMP,2444) XM
 2444   FORMAT('Mean1=',F8.3)
        call hplcom(xl3-13*chs+u,yl3+v,TEMP)
        WRITE(TEMP,445) XXM
  445   FORMAT('RMS1=',G8.3)
        call hplcom(xl3-13*chs+u,yl3-1.5*chs+v,TEMP)
        WRITE(TEMP,446) EPS
  446   FORMAT('Area=',G8.3)
        call hplcom(xl3-13*chs+u,yl3-3*chs+v,TEMP)
       ENDIF
      ENDIF
C
      call GraClose
      RETURN
      END

      SUBROUTINE HSTUN (NCO, FACT, LEGAL)
C
C     FIND CONVERSION UNIT FOR VAUE TO BE HISTOGRAMMED
C
      COMMON /BLOC2/ UNIT(12), NAME(12), NOM(12), NOM1(4)
      LOGICAL LEGAL
C
      LEGAL = .TRUE.
      IF(NCO .GT. 0 .AND. NCO .LE. 6) GO TO 20
      IF(NCO .EQ. 8) GO TO 30
      IF(NCO .EQ. 11) GO TO 30
      IF(NCO .EQ. 12) GO TO 30
      IF(NCO .EQ. 13 .OR. NCO .EQ. 14 .OR. NCO .EQ. 15) GO TO 35
      IF(NCO .GT. 20 .AND. NCO .LE. 24) GO TO 40
C
   10 LEGAL = .FALSE.
      WRITE (6, 100)
      RETURN
C
   20 IF(NCO .EQ. 5) GO TO 10
   30 FACT = UNIT(NCO)
      RETURN
C
   35 FACT = 1.
      RETURN
C
   40 J = NCO - 20
      IF(J .GT. 2) GO TO 50
      FACT = UNIT(J)
      RETURN
C
   50 FACT = UNIT(7)
      RETURN
C
  100 FORMAT('The coordinate specification on the histogram card '
     A      ,'above has no meaning to Turtle. Histogram ignored.')
      END

      SUBROUTINE XCAP(NUN,TEXT)
C
      COMMON /BLOC2/ UNIT(12), NAME(12), NOM(12), NOM1(4)
      CHARACTER TEXT*11
C
      TEXT(1:11) = ' '
      IF (NUN .LE. 12) THEN
       WRITE(TEXT(1:4),1000) NOM(NUN)
       if (nun .eq. 2) TEXT(1:4) = 'x^,!  '   ! x'
       if (nun .eq. 4) TEXT(1:4) = 'y^,!  '   ! y'
       TEXT(5:6) = ' ('
       WRITE(TEXT(7:10),1000) NAME(NUN)
       if (NUN.eq.1 .or. NUN.eq.3) then       ! MM -> mm, CM -> cm
         call tolower(TEXT(7:10))
       endif
       IF      (TEXT( 8:10) .EQ. '   ') THEN
        TEXT(8:8) = ')'
       ELSE IF (TEXT( 9:10) .EQ. '  ') THEN
        TEXT(9:9) = ')'
       ELSE IF (TEXT(10:10) .EQ. ' ') THEN
        TEXT(10:10) = ')'
       ELSE
        TEXT(11:11) = ')'
       ENDIF
      ELSE IF (NUN .EQ. 13) THEN
        TEXT(1:2) = '[b]! '   ! beta
      ELSE IF (NUN .EQ. 14) THEN
        TEXT(1:2) = '[g]! '   ! gamma
      ELSE IF (NUN .EQ. 15) THEN
       TEXT(1:5) = 'l.pol'
      ELSE
       J = NUN - 20
       WRITE(TEXT(1:4),1000) NOM1(J)
       if (j .eq. 2) TEXT(1:4) = '[x]!   '  ! xi
       if (j .eq. 3) TEXT(1:4) = '[y]!   '  ! psi
       if (j .eq. 4) TEXT(1:4) = '[z]!   '  ! zeta
       TEXT(5:6) = ' ('
       WRITE(TEXT(7:10),1000) NAME(J)
       IF (J .GT. 2) WRITE(TEXT(7:10),1000) NAME(7)
       if (J.eq.1) then         ! MM -> mm, CM -> cm
         call tolower(TEXT(7:10))
       endif
       IF      (TEXT( 8:10) .EQ. '   ') THEN
        TEXT(8:8) = ')'
       ELSE IF (TEXT( 9:10) .EQ. '  ') THEN
        TEXT(9:9) = ')'
       ELSE IF (TEXT(10:10) .EQ. ' ') THEN
        TEXT(10:10) = ')'
       ELSE
        TEXT(11:11) = ')'
       ENDIF
      ENDIF
 1000 FORMAT(A4)
      RETURN
      END

      subroutine GraOpen
c
      common /ionumb/ id, mx, my, xl, yl, xl2, yl2, xl3, yl3, chs, open
      logical open
      common /scrsize/ width, height, xoff, yoff
      parameter (nwpawc = 50000)
      common /pawc/ h(nwpawc)
      logical first
      data first /.TRUE./
c
      if (first) then
        call hlimit(nwpawc)
        call hplint(1)
        call hplsiz(width,height,' ')
        first = .FALSE.
        call hplset('YGTI',1.0)

      call hplset('ASIZ',chs)
      call hplset('CSIZ',chs)
      call hplset('GSIZ',chs)
      call hplset('KSIZ',chs)
      call hplset('PSIZ',chs)
      call hplset('SSIZ',chs)
      call hplset('VSIZ',chs)
      call hplset('TSIZ',chs)

      call hplset('XMGL',xoff)
      call hplset('XMGR',xoff)
      call hplset('XWIN',xoff)
      call hplset('YMGL',yoff)
      call hplset('YMGU',yoff)
      call hplset('YWIN',yoff)

      call hplset('XTIC',0.15*xoff)
      call hplset('YTIC',0.15*yoff)

      call hplset('XLAB',0.7*xoff)
      call hplset('XVAL',0.2*xoff)

      call hplset('YLAB',0.4*yoff)
      call hplset('YVAL',0.1*yoff)
      call igset('2BUF',11.)   ! hint from Thomas Prokscha
c
c       resize window: iw and ih correspond to 19.5 cm
c
        call ixgetwi(1,iwid)           ! get window ID
        call ixgetge(iwid,ix,iy,iw,ih) ! get width and height in pixels
        iw = int(iw * width/19.5)
        ih = int(ih * height/19.5)
        call ixrscwi(iwid,iw,ih)
      endif
c
      return
      end

      subroutine GraClose
c
      common /ionumb/ id, mx, my, xl, yl, xl2, yl2, xl3, yl3, chs, open
      logical open
      character*6  strin
      character*30 fname
c
      call igterm  ! go into alphanumeric mode
      strin = ' '
  100 write(6,1000)
 1000 format(' Enter p1, p2, p3, p4 for hardcopy',
     X       ' m for metafile or CR ',$)
      read(5,1001,err=100) strin
 1001 format(a6)
      call tolower(strin)
      if (strin(1:1) .ne. ' ') then
	if (strin(1:1) .eq. 'm') then
          write(6,1010)
 1010     format(' Enter file name (D = metafile.rz) : ',$)
          fname = ' '
          read(5,1011) fname
 1011     format(a32)
          if (fname(1:1) .eq. ' ') fname = 'metafile.rz'
	  call hrput(0,fname,'N')
	else
          call ClosePS
          call SYSTEM('./printg '//strin//' hist.ps')
          id = 0
	endif
      endif
      call hdelet(id)
      call NextID
c     
      return
      end
      
      subroutine OpenPS
c
      common /ionumb/ id, mx, my, xl, yl, xl2, yl2, xl3, yl3, chs, open
      logical open
c
      if (id .eq. 10) then
        if (open) call ClosePS
        open(unit=9,file='hist.ps',form='formatted',status='unknown')
        call igmeta(9,-111) ! A4 portrait
        open = .TRUE.
      endif
c
      return
      end

      subroutine ClosePS
c
      common /ionumb/ id, mx, my, xl, yl, xl2, yl2, xl3, yl3, chs, open
      logical open
c
      call igterm
      call igmeta(999,0)
      close(unit=9)
      open = .FALSE.
c
      return
      end

      subroutine CheckZone
c
      common /ionumb/ id, mx, my, xl, yl, xl2, yl2, xl3, yl3, chs, open
      logical open
c
      if (id .eq. 10) then
        call ixclrwi
        call hplzon(mx,my,1,'    ')
      endif
c
      return
      end

      subroutine NextID
c
      common /ionumb/ id, mx, my, xl, yl, xl2, yl2, xl3, yl3, chs, open
      logical open
      common /scrsize/ width, height, xoff, yoff
c
      id = id + 10
      if (id .gt. mx*my*10) then
        id = 10
      endif
c
      ic = (id - 10) / 10
      x0 = xoff
      y0 = height - 0.9 * yoff
      xs = xoff + (width - (mx+1) * xoff) / mx
      ys = yoff + (height - (my+1) * yoff) / my
      nrx = ic - ((ic/mx)*mx)
      xl = x0 + xs * nrx 
      yl = y0 - ys * (ic/mx)
      xl2 = xl + 0.5 * xs
      yl2 = yl - 0.90 * ys     
      xl3 = xl + xs - xoff - 13 * chs - max(0,my-mx) * chs
      yl3 = yl - 2.5 * chs - max(0,mx-my) * chs
      if (mx.eq.1 .and. my.eq.1) yl2 = yl - 0.97 * ys     
c
      return
      end

      subroutine toupper(str)
c
      character*(*) str
c
      n = len(str)
      do i = 1, n
        ic = ichar(str(i:i))
        if (ic.ge.97 .and. ic.le.122) then
          ic = ic - 32
          str(i:i) = char(ic)
        endif
      enddo
c
      return
      end

      subroutine tolower(str)
c
      character*(*) str
c
      n = len(str)
      do i = 1, n
        ic = ichar(str(i:i))
        if (ic.ge.65.and. ic.le.90) then
          ic = ic + 32
          str(i:i) = char(ic)
        endif
      enddo
c
      return
      end
      
      subroutine GetTit(TYP,UTI)
c
      character*(*) TYP, UTI
c
      if (TYP .eq. 'C1') UTI = 'Contour Plot (monochrome)'
      if (TYP .eq. 'C2') UTI = 'Contour Plot (color)'
      if (TYP .eq. 'CC') UTI = 'Color Plot '
      if (TYP .eq. 'CS') UTI = 'Scatter Plot '
      if (TYP .eq. 'L ') UTI = 'Lego Plot '
      if (TYP .eq. 'L1') UTI = 'Color Lego Plot (mode 1) '
      if (TYP .eq. 'L2') UTI = 'Color Lego Plot (mode 2) '
      if (TYP .eq. 'S ') UTI = 'Surface Plot '
      if (TYP .eq. 'S1') UTI = 'Color Surface Plot (mode 1) '
      if (TYP .eq. 'S2') UTI = 'Coror Surface Plot (mode 2) '
      if (TYP .eq. 'S3') UTI = 'Surface Plot with contous on top '
      if (TYP .eq. 'S4') UTI = 'Surface Plot with Gouraud shading '
c
      return
      end

      subroutine Header
c
      write(6,1000)
1000  format(/
     &  'Welcome to the TURTLE Graphic Histogram Option Programme.')
      write(6,1001)
1001  format(
     &  'Version 1.23 for Linux, using HBOOK/HPLOT from CernLib. ')
      write(6,1002)
1002  format(
     &  '=========================================================')
      write(6,1003)
1003  format(/
     &  '-->   Graphic Data are stored on FOR050.DAT via       <--')
      write(6,1004)
1004  format(
     &  '-->   16. 190. i. 50. ; Card in TURTLE Input-Deck.    <--')
      write(6,1005)
1005  format(
     &  '-->   i = 0 : create with each run a new FOR050.DAT   <--')
      write(6,1006)
1006  format(
     &  '-->   i = 2 : update FOR050.DAT with each run.        <--'/)
c
      return
      end

      SUBROUTINE ELLIPS(IPLOT,AMIN,NA,DA,DMIN,ND,DD,
     X                  SUM0, XM0, XXM0, YM0, YYM0, R12, EPS, IER)
C
      REAL IPLOT(1)      
C
      REAL*8 SUM, SUMX, SUMXX, SUMY, SUMYY, SUMXY
      REAL*8 XM, YM, XXM, YYM, XYM
C
      IER = 0
      MTT=(NA+1)*(ND+1)+5
      I=IPLOT(MTT)-IPLOT(MTT-1)-IPLOT(MTT-2)-IPLOT(MTT-3)-IPLOT(MTT-4)
      IF (I.LT.3) THEN
       IER = 1
       RETURN
      ENDIF
      SUM=0.0
      SUMX=0.0
      SUMXX=0.0
      SUMY=0.0
      SUMYY=0.0
      SUMXY=0.0
      DO 5 J=1,ND
       DO 5 I=1,NA
        K=(J-1)*(NA+1)+I
        X=AMIN+(FLOAT(I-1)+0.5)*DA
        Y=DMIN+(FLOAT(J-1)+0.5)*DD
        Z=IPLOT(K)
        SUM=SUM+DBLE(Z)
        SUMX=SUMX+DBLE(X*Z)
        SUMXX=SUMXX+DBLE(X*X*Z)
        SUMY=SUMY+DBLE(Y*Z)
        SUMYY=SUMYY+DBLE(Y*Y*Z)
        SUMXY=SUMXY+DBLE(X*Y*Z)
    5 CONTINUE
      IF (SUM .LE. 0.0) THEN
       IER = 1
       SUM0 = 0.0
       RETURN
      ENDIF
      XM=SUMX/SUM
      YM=SUMY/SUM
      XXM=SUMXX/SUM
      YYM=SUMYY/SUM
      XYM=SUMXY/SUM
      SUM0=SUM
      IF (XXM-XM*XM .LE. 0.0) THEN
       IER = 1
       XXM0 = 0.0
       RETURN
      ENDIF
      IF (YYM-YM*YM .LE. 0.0) THEN
       IER = 1
       YYM0 = 0.0
       RETURN
      ENDIF
      XM0=XM
      YM0=YM
      XXM0=DSQRT(XXM-XM*XM)
      YYM0=DSQRT(YYM-YM*YM)
      XYM0=XYM-XM*YM
C
      R12=XYM0/(XXM0*YYM0)
      IF (R12.GT.1.0) R12 = 1.0
      IF (R12.LT.-1.0) R12 = -1.0
      EPS = XXM0*YYM0*SQRT(1.-R12**2)
C
C      WRITE(6,1000) XM0, XXM0, YM0, YYM0, R12, EPS
C 1000 FORMAT(/25X,' **** Ellipse parameters ****'/,
C     X/15X,' horizontal :  center =',F8.3,3X,'RMS half width = ',G10.3
C     X/15X,' vertical   :  center =',F8.3,3X,'RMS half width = ',G10.3
C     X/15X,' correlation:  sinchi =',F8.3,3X,'RMS area / pi  = ',G10.3
C     X)
C
      RETURN
      END

      SUBROUTINE MEAN(IPLOT, AMIN, NA, DA, SUM, XM, XXM, IER)
C
      REAL IPLOT(1)      
      REAL*8 SUMD, SUMX, SUMXX
      REAL*8 XMD, XXMD
C
      IER = 0

      SUMD=0.0
      SUMX=0.0
      SUMXX=0.0
      DO 5 I=1,NA
        X=AMIN+(FLOAT(I-1)+0.5)*DA
        Z=IPLOT(I)
        SUMD=SUMD+DBLE(Z)
        SUMX=SUMX+DBLE(X*Z)
        SUMXX=SUMXX+DBLE(X*X*Z)
    5 CONTINUE
      IF (SUMD .LE. 0.0) THEN
       SUM = 0.0
       IER = 1
       RETURN
      ENDIF
      XMD=SUMX/SUMD
      XXMD=SUMXX/SUMD
      SUM=SUMD
      IF (XXMD-XMD*XMD .LE. 0.0) THEN
       XXM = 0.0
       IER = 1
       RETURN
      ENDIF
      XXM=DSQRT(XXMD-XMD*XMD)
      XM=XMD
C
      RETURN
      END
