///////////////////////////////////////////////////////////////////////////////
//////////////////// class TransEllipseWin implementation /////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

/////////////////////////// some drawing macros ///////////////////////////////

#define GraMove(a,b)      opDev->MoveTo(a+XOFFSET,b+YOFFSET)
#define GraDraw(a,b)      opDev->LineTo(a+XOFFSET,b+YOFFSET)
#define PutChar(x,y,c)    opDev->PosStr(x+XOFFSET,y+YOFFSET,c)
#define PutString(x,y,s)  opDev->PosStr(x+XOFFSET,y+YOFFSET,s)

///////////////////////////////////////////////////////////////////////////////

TransEllipseWin :: TransEllipseWin(int nCmdShow, LPCSTR szWindowName,
                   RECT rc, Window *opParent, TransGrafFile *f,
                   BOOL cf, BOOL pcf, int lw)
                 : Window(nCmdShow,szWindowName,rc)
{
    int i;

    // Append to System menu

    HMENU hMenu = GetSystemMenu(GetHandle(),FALSE);
    AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
    AppendMenu(hMenu,MF_STRING,IDM_PRINT, "Print Ellipses\tP");
    AppendMenu(hMenu,MF_STRING,IDM_PRINT2, "Print Ellipses to PS File\tF");
    EnableMenuItem(hMenu, SC_CLOSE, MF_GRAYED);

    opGrafFile     = f;
    colorflag      = cf;
    printcolorflag = pcf;
    plw            = lw;
    magn_fac       = (float)1.0;

    XOFFSET = 0;
    YOFFSET = 0;
    ww = -1;

    // Fonts

    opFontA = new Font(String(IDS_SMALLF));
    opFontB = new Font(String(IDS_TIMESNR));
    opFontC = new Font(String(IDS_SANSERIF));
    if (plw <= 1)
	opFontD = new Font(String(IDS_TIMESNR),0,-1);   // 0 ==> normal face
    else
	opFontD = new Font(String(IDS_TIMESNR),1,-1);   // 1 ==> bold face

    // Brushes

    opWhiteBrush = new Brush(RGB(255,255,255));
    opBlackBrush = new Brush(RGB(0,0,0));

    counter        = -1;
    NumberOfFrames = 1;

    // compute sine/cosine-table

    for (i=0; i<101; i++)
    {  float arg = float(i) * (float)0.0628318;
       cosd[i] = (float)cos(arg);
       sind[i] = (float)sin(arg);
    }

    for (i=0; i<4; i++)  // uninitialized mem found with Bounds Checker
    {  x[i]   = (float)0.0;
       xp[i]  = (float)0.0;
       r12[i] = (float)0.0;
       y[i]   = (float)0.0;
       yp[i]  = (float)0.0;
       r34[i] = (float)0.0;
       d[i]   = (float)0.0;
       dp[i]  = (float)0.0;
       strcpy(&infoline[i][0],"");
       xlx[i] = 0;
       xly[i] = 0;
       ylx[i] = 0;
       yly[i] = 0;
       xscal[i]  = (float)1.0;
       xpscal[i] = (float)1.0;
       yscal[i]  = (float)1.0;
       ypscal[i] = (float)1.0;
    }

    opDev = this;
    closed = FALSE;
    Invalidate2();
}

TransEllipseWin :: ~TransEllipseWin(VOID)
{
    TransEllipseWin::WmClose();
}

BOOL TransEllipseWin :: WmDestroy( void )
{
    return FALSE;
}

BOOL TransEllipseWin :: WmClose( void )
{
    if (!closed)
    {
	if (opFontA) delete opFontA;
	if (opFontB) delete opFontB;
	if (opFontC) delete opFontC;
	if (opFontD) delete opFontD;
	if (opWhiteBrush) delete opWhiteBrush;
	if (opBlackBrush) delete opBlackBrush;
	DestroyPens();
	closed = TRUE;
    }
    return FALSE;
}


long TransEllipseWin :: WmSysMenuCommand(WPARAM MenuItem)
{
    switch (MenuItem)
    {
	case IDM_PRINT2: PrintToFile(); return TRUE;
        case IDM_PRINT: Print(); return TRUE;
        default: return DefaultEvent();
    }
    return FALSE;
}

long  TransEllipseWin :: WmKeyDown(WPARAM Key)
{
    if (Key == 17)  // do not react here when Control key pressed
       return TRUE;
    if (Key == 16)  // do not react here when Shift key pressed
       return TRUE;

    KeyCode = Key;

    if (KeyCode=='f' || KeyCode=='F')
       PrintToFile();
    if (KeyCode=='p' || KeyCode=='P')
       Print();

    return TRUE;
}

VOID TransEllipseWin :: CreatePens(int w)
{
    if (w != ww)
    {
        if (ww != -1) DestroyPens();
	ww = w;
	LOGPEN lpWhite = {PS_SOLID,w,0,RGB(255,255,255)};
	LOGPEN lpBlack = {PS_SOLID,w,0,RGB(0,0,0)};
	opWhitePen   = new Pen(&lpWhite);
	opBlackPen   = new Pen(&lpBlack);

	if (colorflag || printcolorflag)
	{
	    LOGPEN lpGreen = {PS_SOLID,w,0,RGB(0,255,0)};
	    LOGPEN lpRed   = {PS_SOLID,w,0,RGB(255,0,0)};
	    opGreenPen     = new Pen(&lpGreen);
	    opRedPen       = new Pen(&lpRed);
	}
	else
	{
	    LOGPEN lpGreen = {PS_SOLID,w,0,RGB(0,0,0)};
	    LOGPEN lpRed   = {PS_SOLID,w,0,RGB(0,0,0)};
	    opGreenPen     = new Pen(&lpGreen);
	    opRedPen       = new Pen(&lpRed);
	}
    }
}

VOID TransEllipseWin :: DestroyPens(VOID)
{
	if (opWhitePen)   delete opWhitePen;
	if (opBlackPen)   delete opBlackPen;
	if (opGreenPen)   delete opGreenPen;
	if (opRedPen)     delete opRedPen;
}

VOID TransEllipseWin :: DrawFrame(VOID)
{
    Brush *opOld;
    int   i, j, fs, index;
    float finc;

//  draw and write white on black screen

    if (color && !colorprint)
    {  opDev->SetBackColor(RGB(0,0,0));
       opOld = opDev->SetBrush(opBlackBrush);
       opDev->PutRectangle(XOFFSET,YOFFSET,XOFFSET+w_user,YOFFSET+h_user);
       opDev->SetBrush(opOld);
       opOldPen = opDev->SetPen(opWhitePen);
       opDev->SetTextColor(RGB(255,255,255));
    }
    else
    {  opDev->SetBackColor(RGB(255,255,255));
       opOld = opDev->SetBrush(opWhiteBrush);
       opDev->PutRectangle(XOFFSET,YOFFSET,XOFFSET+w_user,YOFFSET+h_user);
       opDev->SetBrush(opOld);
       opOldPen = opDev->SetPen(opBlackPen);
       opDev->SetTextColor(RGB(0,0,0));
    }

//  Frame

    POINT Poly[5] = {{XOFFSET,          YOFFSET},
                     {XOFFSET+w_user-1, YOFFSET},
                     {XOFFSET+w_user-1, YOFFSET+h_user-1},
                     {XOFFSET,          YOFFSET+h_user-1},
                     {XOFFSET,          YOFFSET}};
    opDev->PutPolyline(Poly,5);
    if (NumberOfFrames == 4)
    {  GraMove(0,h_user/2); GraDraw(w_user,h_user/2);
       GraMove(w_user/2,0); GraDraw(w_user/2,h_user);
    }
    if (counter == -1) return;

//  Draw axis crosses for (x,x') and (y,y')  [1 or 4]

    if (color)
    {
       opDev->SetPen(opRedPen);
       opDev->SetTextColor(RGB(255,0,0));
    }
    else
    {
       opDev->SetPen(opBlackPen);
       opDev->SetTextColor(RGB(0,0,0));
    }

    int fac = (NumberOfFrames == 1) ? 2 : 1;
    for (i=0; i<NumberOfFrames; i++)
    {  int fac1 = (i % 2 == 0) ? 1 : 5;
       int fac2 = (i < 2) ? 1 : 3;
       x0x[i] =  fac * fac1 * w_user / 8;
       y0x[i] =  fac * fac2 * h_user / 4;
       x0y[i] =  x0x[i] + fac * w_user / 4;
       y0y[i] =  fac * fac2 * h_user / 4;
    }
    len = fac * h_user / 8;

//  Set Font

    if (NumberOfFrames == 1)
    {
       fs = 2 * (h_user / 70);
       opFont = opFont2;
#ifdef WIN32
       opFont->SetPointSize((3*fs)/2);    
#else
       opFont->SetPointSize(fs);    
#endif
       opOldFont = opDev->SetFont(opFont);
    }
    else
    {
       fs = (h_user / 70);
       opFont = opFont1;
#ifdef WIN32
       opFont->SetPointSize((3*fs)/2);    
#else
       opFont->SetPointSize(fs);    
#endif
       opOldFont = opDev->SetFont(opFont);
    }
    int imax = (NumberOfFrames == 1) ? 0 : counter;
    for (i=0; i<=imax; i++)
    {  GraMove(x0x[i]-len,y0x[i]); GraDraw(x0x[i]+len,y0x[i]);
       PutString(x0x[i]+len+fs/2,y0x[i]-fs,"x");
       Scaling(x[i], xscal[i], xlx[i], finc, index);
       for (j=0; j<=index; j++)
       {  GraMove(x0x[i]-int((float)j*finc),y0x[i]-len/30);
          GraDraw(x0x[i]-int((float)j*finc),y0x[i]+len/30+1);
          GraMove(x0x[i]+int((float)j*finc),y0x[i]-len/30);
          GraDraw(x0x[i]+int((float)j*finc),y0x[i]+len/30+1);
       }
       GraMove(x0x[i],y0x[i]+len); GraDraw(x0x[i],y0x[i]-len);
       PutString(x0x[i],y0x[i]-len-fs-3*fs/4,"x'");
       Scaling(xp[i], xpscal[i], ylx[i], finc, index);
       for (j=0; j<=index; j++)
       {  GraMove(x0x[i]-len/30,y0x[i]-int((float)j*finc));
          GraDraw(x0x[i]+len/30+1,y0x[i]-int((float)j*finc));
          GraMove(x0x[i]-len/30,y0x[i]+int((float)j*finc));
          GraDraw(x0x[i]+len/30+1,y0x[i]+int((float)j*finc));
       }
       GraMove(x0y[i]-len,y0y[i]); GraDraw(x0y[i]+len,y0y[i]);
       PutString(x0y[i]+len+fs/2,y0y[i]-fs,"y");
       Scaling(y[i], yscal[i], xly[i], finc, index);
       for (j=0; j<=index; j++)
       {  GraMove(x0y[i]-int((float)j*finc),y0y[i]-len/30);
          GraDraw(x0y[i]-int((float)j*finc),y0y[i]+len/30+1);
          GraMove(x0y[i]+int((float)j*finc),y0y[i]-len/30);
          GraDraw(x0y[i]+int((float)j*finc),y0y[i]+len/30+1);
       }

       GraMove(x0y[i],y0y[i]+len); GraDraw(x0y[i],y0y[i]-len);
       PutString(x0y[i],y0y[i]-len-fs-3*fs/4,"y'");
       Scaling(yp[i], ypscal[i], yly[i], finc, index);
       for (j=0; j<=index; j++)
       {  GraMove(x0y[i]-len/30,y0y[i]-int((float)j*finc));
          GraDraw(x0y[i]+len/30+1,y0y[i]-int((float)j*finc));
          GraMove(x0y[i]-len/30,y0y[i]+int((float)j*finc));
          GraDraw(x0y[i]+len/30+1,y0y[i]+int((float)j*finc));
       }
    }
}

VOID TransEllipseWin :: Scaling(float p, float& scal, int& length,
                                         float& incr, int& index)
{
       int   j, m;
       float hscal;

       m = (int)(p * (float)10.0 + (float)0.9999);
       scal = (float)m / (float)10.0;
       if (scal < 0.1) scal = (float)0.1;
       length = int((float)len * p / scal);

       hscal = scal;
       for (j=0; j<10; j++)
       {  if (hscal <  1.0) hscal *= (float)10.0; 
          if (hscal > 10.0) hscal /= (float)10.0; 
       }
       incr  = (float)len / hscal;
       index = (int)hscal;
       if (index == 1)
       {  index = int((float)5.0 * hscal);
          incr /= (float)5.0;
       }
       if (index == 2)
       {  index = int((float)2.0 * hscal);
          incr /= (float)2.0;
       }
       if (index == 3)
       {  index = int((float)2.0 * hscal);
          incr /= (float)2.0;
       }
}

long TransEllipseWin :: WmEraseBackGround(void)
{
    return FALSE; // tell Windows to not erase background (white flickering)
//    return DefaultEvent();
}

long TransEllipseWin :: WmSize(int w, int h)
{
    h_user = h;
    w_user = w;

    return DefaultEvent();
}

long TransEllipseWin :: WmPaint( const PAINTSTRUCT & )
{
//  Handle paint events.

    h_user = GetUserHeight();
    w_user = GetUserWidth();

    f_scal  = (float)1.0;
    if (w_user < 750 && h_user < 480)
      opFont1 = opFontA;
    else
      opFont1  = opFontC;
    opFont2    = opFontB;
    color      = FALSE;
    if (colorflag)
       color   = TRUE;
    colorprint = FALSE;
    h_off   = 0;

    Dsplay(0);

    return FALSE;
}

VOID TransEllipseWin :: Print(VOID)
{
//  print drawing.

    f_scal     = (float)0.6;
    opFont1    = opFontD;
    opFont2    = opFontD;
    color      = FALSE;
    colorprint = FALSE;
    if (printcolorflag)
    {  color      = TRUE;
       colorprint = TRUE;
    }
    h_off   = h_user / 5;

    Printer *opPrt = new Printer;

    opPrt->Start(String(IDS_PLOT),this);

    DisplayDevice *opDevSaved = opDev;
    opDev = opPrt;

//  change scaling

    RECT rc;
    GetWindowRect(this->GetHandle(),&rc);
    int X0 = rc.left;  // upper left corner
    int Y0 = rc.top;   // upper left corner
    if (X0 <= 0) X0 = 1;
    if (Y0 <= 0) Y0 = 1;

    int h_user_saved = h_user;
    int w_user_saved = w_user;

    magn_fac = (float)opPrt->GetUserWidth() / (float)GetScreenWidth();
    magn_fac *= (float)0.9;
    h_user   = int((float)h_user * magn_fac);
    w_user   = int((float)w_user * magn_fac);

    XOFFSET = int(magn_fac * (float)X0);
    YOFFSET = int(magn_fac * (float)Y0);
    XOFFSET += (int)(opPrt->GetUserWidth()/30.);  // shift to the right (~1cm)

    Dsplay(plw);

//    if ( opPrt->NextPage() )
       opPrt->End(this);
    delete opPrt;

//  reset old scales

    opDev    = opDevSaved;
    magn_fac = (float)1.0;
    h_user   = h_user_saved;
    w_user   = w_user_saved;

    XOFFSET = 0;
    YOFFSET = 0;

    if (w_user < 750 && h_user < 480)
      opFont1 = opFontA;
    else
      opFont1 = opFontC;
    opFont2 = opFontB;

//  execute VPP if PrinterPort = c:\ps810.prn

    char info[80];
    GetProfileString(String(IDS_WINDOWS),String(IDS_DEVICE,1),"",info,80);
    AnsiLowerBuff(info,strlen(info));
    if (strstr(info,String(IDS_PS810)) != NULL)
       WinExec(String(IDS_VPP),SW_MAXIMIZE);
}

VOID TransEllipseWin :: PrintToFile(VOID)
{
//  print drawing.

    f_scal     = (float)0.6;
    opFont1    = opFontD;
    opFont2    = opFontD;
    color      = FALSE;
    colorprint = FALSE;
    if (printcolorflag)
    {  color      = TRUE;
       colorprint = TRUE;
    }
    h_off   = h_user / 5;

    Printer *opPrt = new Printer;

    opPrt->StartPrintToFile(String(IDS_PLOT),this);

    DisplayDevice *opDevSaved = opDev;
    opDev = opPrt;

//  change scaling

    RECT rc;
    GetWindowRect(this->GetHandle(),&rc);
    int X0 = rc.left;  // upper left corner
    int Y0 = rc.top;   // upper left corner
    if (X0 <= 0) X0 = 1;
    if (Y0 <= 0) Y0 = 1;

    int h_user_saved = h_user;
    int w_user_saved = w_user;

    magn_fac = (float)opPrt->GetUserWidth() / (float)GetScreenWidth();
    magn_fac *= (float)0.9;
    h_user   = int((float)h_user * magn_fac);
    w_user   = int((float)w_user * magn_fac);

    XOFFSET = int(magn_fac * (float)X0);
    YOFFSET = int(magn_fac * (float)Y0);
    XOFFSET += (int)(opPrt->GetUserWidth()/30.);  // shift to the right (~1cm)

    Dsplay(plw);

//    if ( opPrt->NextPage() )
       opPrt->End(this);
    delete opPrt;

//  reset old scales

    opDev    = opDevSaved;
    magn_fac = (float)1.0;
    h_user   = h_user_saved;
    w_user   = w_user_saved;

    XOFFSET = 0;
    YOFFSET = 0;

    if (w_user < 750 && h_user < 480)
      opFont1 = opFontA;
    else
      opFont1 = opFontC;
    opFont2 = opFontB;

//  execute VPP if PrinterPort = c:\ps810.prn

    char info[80];
    GetProfileString(String(IDS_WINDOWS),String(IDS_DEVICE,1),"",info,80);
    AnsiLowerBuff(info,strlen(info));
    if (strstr(info,String(IDS_PS810)) != NULL)
       WinExec(String(IDS_VPP),SW_MAXIMIZE);
}

VOID TransEllipseWin :: GetDimensions(char *p0, char *p1,
                                      char *p2, char *p3, char *p4)
{
    strcpy(&dim0[0],p0);
    strcpy(&dim1[0],p1);
    strcpy(&dim2[0],p2);
    strcpy(&dim3[0],p3);
    strcpy(&dim7[0],p4);
}

VOID TransEllipseWin :: TransferEllipsePar(float p1, float p2, float p3,
                                           float p4, float p5, float p6,
                                           float p7, float p8, char *info)
{
    counter++;
    if (counter > 3)
        counter = 3;

//  shift upwards:  2 -> 3, 1 -> 2, 0 -> 1

    for (int i=2; i>-1; i--)
    {  x[i+1]   = x[i];
       xp[i+1]  = xp[i];
       r12[i+1] = r12[i];
       y[i+1]   = y[i];
       yp[i+1]  = yp[i];
       r34[i+1] = r34[i];
       d[i+1]   = d[i];
       dp[i+1]  = dp[i];
       strcpy(&infoline[i+1][0],&infoline[i][0]);
    }

//  repalace 0 by new 

    x[0]   = p1;
    xp[0]  = p2;
    r12[0] = p3;
    y[0]   = p4;
    yp[0]  = p5;
    r34[0] = p6;
    d[0]   = p7;
    dp[0]  = p8;
    strcpy(&infoline[0][0],info);

    Invalidate2();
}

VOID TransEllipseWin :: FourFrames(VOID)
{
    NumberOfFrames = 4;
    Invalidate2();
}

VOID TransEllipseWin :: SingleFrame(VOID)
{
    NumberOfFrames = 1;
    Invalidate2();
}

VOID TransEllipseWin :: DrawEllipses(VOID)
{
    int j, x, y;

    if (counter == -1) return;

    POINT Poly[101];

    if (color)
       opDev->SetPen(opGreenPen);
    else
       opDev->SetPen(opBlackPen);

    int imax = (NumberOfFrames == 1) ? 0 : counter;
    for (int i=0; i<=imax; i++)
    {
       if (fabs(r12[i]) < 1.0e-10) r12[i] = (float)0.0;
       float sinch1 = r12[i];
       float cosch1 = (float)sqrt(1.0 - sinch1 * sinch1);

       for (j=0; j<101; j++)
       {
          x = int(float(xlx[i]) *  cosd[j]);
          y = int(float(ylx[i]) * (sind[j]*cosch1+cosd[j]*sinch1));
          Poly[j].x = x0x[i] + x + XOFFSET;
          Poly[j].y = y0x[i] - y + YOFFSET;
       }
       opDev->PutPolyline(Poly,101);

       GraMove(x0x[i]+xlx[i],y0x[i]-int((float)ylx[i]*r12[i]));  // conjugated diameters
       GraDraw(x0x[i]-xlx[i],y0x[i]+int((float)ylx[i]*r12[i]));
       GraMove(x0x[i]+int((float)xlx[i]*r12[i]),y0x[i]-ylx[i]);
       GraDraw(x0x[i]-int((float)xlx[i]*r12[i]),y0x[i]+ylx[i]);

       if (fabs(r34[i]) < 1.0e-10) r34[i] = (float)0.0;
       float sinch2 = r34[i];
       float cosch2 = (float)sqrt(1.0 - sinch2 * sinch2);

       for (j=0; j<101; j++)
       {  x = int(float(xly[i]) *  cosd[j]);
          y = int(float(yly[i]) * (sind[j]*cosch2+cosd[j]*sinch2));
          Poly[j].x = x0y[i] + x + XOFFSET;
          Poly[j].y = y0y[i] - y + YOFFSET;
       }
       opDev->PutPolyline(Poly,101);

       GraMove(x0y[i]+xly[i],y0y[i]-int((float)yly[i]*r34[i]));  // conjugated diameters
       GraDraw(x0y[i]-xly[i],y0y[i]+int((float)yly[i]*r34[i]));
       GraMove(x0y[i]+int((float)xly[i]*r34[i]),y0y[i]-yly[i]);
       GraDraw(x0y[i]-int((float)xly[i]*r34[i]),y0y[i]+yly[i]);
    }
}

VOID TransEllipseWin :: DrawCaptions(VOID)
{
    char string[80];
    int  i, fs;

    if (counter == -1) return;
    if (color && !colorprint)
    {
       opOldPen = opDev->SetPen(opWhitePen);
       opDev->SetTextColor(RGB(255,255,255));
    }
    else
    {
       opOldPen = opDev->SetPen(opBlackPen);
       opDev->SetTextColor(RGB(0,0,0));
    }

//  Set Font

    if (NumberOfFrames == 1)
    {
       fs = 2 * (h_user / 70);
       opFont = opFont2;
#ifdef WIN32
//       opFont->SetPointSize((3*fs)/2);    
#else
//       opFont->SetPointSize(fs);    
#endif
       opOldFont = opDev->SetFont(opFont);
    }
    else
    {
       fs = (h_user / 70);
       opFont = opFont1;
#ifdef WIN32
//       opFont->SetPointSize((3*fs)/2+2);    
#else
//       opFont->SetPointSize(fs+2);    
#endif
       opOldFont = opDev->SetFont(opFont);
    }

//  Write Captions

    int imax = (NumberOfFrames == 1) ? 0 : counter;
    for (i=0; i<=imax; i++)
    {  if (xp[i] != 0.0)
       {  float lwx = -r12[i] * x[i] / xp[i];
          sprintf(string,String(IDS_LWX_FORM),lwx, dim7);
       }
       else
          sprintf(string,String(IDS_ERX_FORM), dim7);
       PutString(x0x[i]-len, y0x[i] - 2 * len + 1 * fs, string);

       if (yp[i] != 0.0)
       {  float lwy = -r34[i] * y[i] / yp[i];
          sprintf(string,String(IDS_LWY_FORM),lwy, dim7);
       }
       else
          sprintf(string,String(IDS_ERY_FORM), dim7);
       PutString(x0y[i]-len, y0y[i] - 2 * len + 1 * fs, string);

       float chix = float(180.0 / 3.14159) * float(asin(r12[i]));
       sprintf(string,String(IDS_CHX_FORM),chix, r12[i]);
       PutString(x0x[i]-len, y0x[i] - 2 * len + 3 * fs, string);

       float chiy = float(180.0 / 3.14159) * float(asin(r34[i]));
       sprintf(string,String(IDS_CHY_FORM),chiy, r34[i]);
       PutString(x0y[i]-len, y0y[i] - 2 * len + 3 * fs, string);

       float aux  = (float)1.0 - r12[i] * r12[i];
       if (aux < 0.0) aux = (float)0.0;
       float epsx = x[i] * xp[i] * (float)sqrt(aux);
       sprintf(string,String(IDS_EPX_FORM),epsx,dim0,dim1);
       PutString(x0x[i]-len, y0x[i] - 2 * len + 5 * fs, string);

       aux  = (float)1.0 - r34[i] * r34[i];
       if (aux < 0.0) aux = (float)0.0;
       float epsy = y[i] * yp[i] * (float)sqrt(aux);
       sprintf(string,String(IDS_EPY_FORM),epsy,dim2,dim3);
       PutString(x0y[i]-len, y0y[i] - 2 * len + 5 * fs, string);

       sprintf(string,String(IDS_XXM_FORM),
                      xscal[i], dim0, xpscal[i], dim1);
       PutString(x0x[i]-len, y0x[i] + 2 * len - 7 * fs, string);

       sprintf(string,String(IDS_YYM_FORM),
                      yscal[i], dim2, ypscal[i], dim3);
       PutString(x0y[i]-len, y0y[i] + 2 * len - 7 * fs, string);

       sprintf(string,String(IDS_DDP_FORM),
                      d[i], dim0, dp[i], dim1);
       PutString(x0x[i]-len, y0x[i] + 2 * len - 5 * fs, string);

       strcpy(string,&infoline[i][0]);
       PutString(x0x[i]-len, y0x[i] + 2 * len - 3 * fs, string);
    }
}

VOID TransEllipseWin :: Dsplay(int w)
{
    CreatePens(w);
    DrawFrame();
    DrawEllipses();
    DrawCaptions();
    if (counter == -1)
	return;
    opDev->SetFont(opOldFont);
    opDev->SetPen(opOldPen);
}

VOID TransEllipseWin :: Erase(VOID)
{
    counter = -1;
    Invalidate2();
}

///////////////////////////////// EOF /////////////////////////////////////////

