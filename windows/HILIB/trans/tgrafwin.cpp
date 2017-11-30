///////////////////////////////////////////////////////////////////////////////
///////////////////// class TransGrafWin implementation ///////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

extern char SCRDIR[80];  // get directory from tpmenu

/////////////////////////// some drawing macros ///////////////////////////////

#define GraMove(a,b)      opDev->MoveTo(a+XOFFSET,b+YOFFSET)
#define GraDraw(a,b)      opDev->LineTo(a+XOFFSET,b+YOFFSET)
#define PutChar(x,y,c)    opDev->PosStr(x+XOFFSET,y+YOFFSET,c)
#define DrawCross(x,y,l)  GraMove(x-l,y);\
                          GraDraw(x+l+1,y);\
                          GraMove(x,y+l);\
                          GraDraw(x,y-l-1);
#define DrawX(x,y,l)      GraMove(x-l,y-l);\
                          GraDraw(x+l+1,y+l+1);\
                          GraMove(x-l,y+l);\
                          GraDraw(x+l+1,y-l-1);
#define DrawSquare(x,y,l) GraMove(x-l,y-l);\
                          GraDraw(x-l,y+l);\
                          GraDraw(x+l,y+l);\
                          GraDraw(x+l,y-l);\
                          GraDraw(x-l,y-l);
#define DrawRombus(x,y,l) GraMove(x,y-l);\
                          GraDraw(x+l,y);\
                          GraDraw(x,y+l);\
                          GraDraw(x-l,y);\
                          GraDraw(x,y-l);

///////////////////////////////////////////////////////////////////////////////

TransGrafWin :: TransGrafWin(int nCmdShow, LPCSTR szWindowName,
                RECT rc, MainWindow *opParent, TransGrafFile *f,
                TransSecondFile *s, DISPAR *p, BOOL cf, BOOL pcf, int lw) 
              : Window(nCmdShow,szWindowName,rc)
{
    hPrntWnd   = opParent->GetHandle();
    opPar      = opParent;
    opGrafFile = f;
    opSecFile  = s;
    dispar     = p;
    colorflag  = cf;
    printcolorflag = pcf;
    magn_fac   = (float)1.0;
    Result     = 0;
    plw        = lw;

    XOFFSET = 0;
    YOFFSET = 0;

    if (plw < 1)
	plw = 1;

    // Load Popup Menu

    opPopupMenu = new Menu(IDR_POPUP_MENU,(Window *)NULL);

    hSubMenu = GetSubMenu(opPopupMenu->GetHandle(),0);

    // Append to System menu

    SystemMenu oSystemMenu(this);
    oSystemMenu.AppendSep();
    oSystemMenu.Append(IDM_PRINT,String(IDS_PRT_GRAF));
    oSystemMenu.Append(IDM_PRINT2,String(IDS_PRT_GRAF2));
    oSystemMenu.AppendSep();
    oSystemMenu.Append(IDM_PASTE,String(IDS_CLP_GRAF));

    // Fonts

    opFont1 = new Font(String(IDS_R_ASCII),OEM_CHARSET);
    opFont3 = new Font(String(IDS_R_ASCII),OEM_CHARSET);
    if (plw <= 1)
       opFont2 = new Font(String(IDS_COURIER),0,-1);  // 0 ==> normal face
    else
       opFont2 = new Font(String(IDS_COURIER),1,-1);  // 1 ==> bold face

    // Brushes and Pens

    opBlackBrush     = new Brush(RGB(0,0,0));


    // time and date

#ifdef __WATCOMC__
    time_t t;
    t = time(NULL);
    Tstring = ctime(&t);
#else
    long secs;
    time(&secs);
    Tstring = ctime(&secs);
#endif
    Tstring[strlen(Tstring)-1] = '\0';

    DisplayFlag = TRUE;
    opDev = this;
    closed = FALSE;
    Invalidate2();
}

TransGrafWin :: ~TransGrafWin(VOID)
{
    TransGrafWin::WmClose();
}

BOOL TransGrafWin :: WmClose( void )
{
    if (!closed)
    {
       if (opFont1) delete opFont1;
       if (opFont3) delete opFont3;
       if (opFont2) delete opFont2;
       if (opBlackBrush) delete opBlackBrush;   
       if (opPopupMenu)  delete opPopupMenu;
       closed = TRUE;
    }
    return FALSE;
}

VOID TransGrafWin :: DestroyPens( void )
{
       if (opWhitePen)   delete opWhitePen;
       if (opBlackPen)   delete opBlackPen;
       if (opGreenPen)   delete opGreenPen;
       if (opCyanPen)    delete opCyanPen;
       if (opBluePen)    delete opBluePen;
       if (opRedPen)     delete opRedPen;
       if (opYellowPen)  delete opYellowPen;
       if (opMagentaPen) delete opMagentaPen;

       if (opWhiteDottedPen) delete opWhiteDottedPen;
       if (opRedDottedPen)   delete opRedDottedPen;
       if (opBlackDottedPen) delete opBlackDottedPen;
}

VOID TransGrafWin :: CreatePens( int w )
{

    LOGPEN lpWhite = {PS_SOLID,w,0,RGB(255,255,255)};
    opWhitePen      = new Pen(&lpWhite);
    LOGPEN lpBlack = {PS_SOLID,w,0,RGB(0,0,0)};
    opBlackPen      = new Pen(&lpBlack);

    if (colorflag || printcolorflag)
    {
       LOGPEN lpGreen   = {PS_SOLID,w,0,RGB(0,255,0)};
       opGreenPen   = new Pen(&lpGreen);
       LOGPEN lpCyan    = {PS_SOLID,w,0,RGB(0,255,255)};
       opCyanPen    = new Pen(&lpCyan);
       LOGPEN lpBlue    = {PS_SOLID,w,0,RGB(0,0,255)};
       opBluePen    = new Pen(&lpBlue);
       LOGPEN lpRed     = {PS_SOLID,w,0,RGB(255,0,0)};
       opRedPen     = new Pen(&lpRed);
       LOGPEN lpYellow  = {PS_SOLID,w,0,RGB(255,255,0)};
       opYellowPen  = new Pen(&lpYellow);
       LOGPEN lpMagenta = {PS_SOLID,w,0,RGB(255,0,255)};
       opMagentaPen = new Pen(&lpMagenta);
    }
    else
    {
       LOGPEN lpGreen   = {PS_SOLID,w,0,RGB(0,255,255)};
       opGreenPen   = new Pen(&lpGreen);
       LOGPEN lpCyan    = {PS_SOLID,w,0,RGB(0,255,255)};
       opCyanPen    = new Pen(&lpCyan);
       LOGPEN lpBlue    = {PS_SOLID,w,0,RGB(0,255,255)};
       opBluePen    = new Pen(&lpBlue);
       LOGPEN lpRed     = {PS_SOLID,w,0,RGB(0,255,255)};
       opRedPen     = new Pen(&lpRed);
       LOGPEN lpYellow  = {PS_SOLID,w,0,RGB(0,255,255)};
       opYellowPen  = new Pen(&lpYellow);
       LOGPEN lpMagenta = {PS_SOLID,w,0,RGB(0,255,255)};
       opMagentaPen = new Pen(&lpMagenta);
    }
    if (w <= 1) // best for video screens
    {
       LOGPEN lpWhiteDotted   = {PS_DOT,0,0,RGB(255,255,255)};
       opWhiteDottedPen = new Pen(&lpWhiteDotted);
       LOGPEN lpRedDotted     = {PS_DOT,0,0,RGB(255,0,0)};
       opRedDottedPen   = new Pen(&lpRedDotted);
       LOGPEN lpBlackDotted   = {PS_DOT,0,0,RGB(0,0,0)};
       opBlackDottedPen = new Pen(&lpBlackDotted);
    }
    else  // better on most printers
    {
       LOGPEN lpWhiteDotted   = {PS_DASH,0,0,RGB(255,255,255)};
       opWhiteDottedPen = new Pen(&lpWhiteDotted);
       LOGPEN lpRedDotted     = {PS_DASH,0,0,RGB(255,0,0)};
       opRedDottedPen   = new Pen(&lpRedDotted);
       LOGPEN lpBlackDotted   = {PS_DASH,0,0,RGB(0,0,0)};
       opBlackDottedPen = new Pen(&lpBlackDotted);
    }
}

BOOL TransGrafWin :: WmDestroy( void )
{
    return FALSE;
}

VOID TransGrafWin :: DrawFrame(VOID)
{
    Brush *opOld;
    int i;

//  special case for apfac < 0.0: ===> draw magnet apertures complementary

    dcomp = FALSE;
    apfac = dispar->apfac;
    if (apfac < 0.0)
    {
	apfac = float(-1.0) * apfac;
	dcomp = TRUE;
    }

//  draw and write white on black screen

    if (color && !colorprint)
    {
       opDev->SetBackColor(RGB(0,0,0));
       opOld = opDev->SetBrush(opBlackBrush);
       opDev->PutRectangle(0,0,w_user,h_user);
       opDev->SetBrush(opOld);

       opOldPen = opDev->SetPen(opWhitePen);
       opDev->SetTextColor(RGB(255,255,255));
    }
    else
    {
       opOldPen = opDev->SetPen(opWhitePen);
       opDev->PutRectangle(0,0,w_user,h_user);
       opOldPen = opDev->SetPen(opBlackPen);
    }

#ifdef WIN32
    int yt = (int) (0.045 * (float) h_user);
    fs1    = (yt * 5) / 8;
#else
    int ns = (GetScreenWidth()>800) ? 5 : 4;
    int yt = (int) (0.045 * (float) (4 * (h_user / ns)));
    fs1    = (yt * 5) / 10;
    if (f_scal == 1.0 && ns == 4 && fs1 > 16) fs1 = 16;  // limit size on screen
    if (f_scal == 1.0 && ns == 5 && fs1 > 12) fs1 = 12;  // limit size on screen
#endif

    yt     = yt + h_off;  // make space for title in print out
//    if (clipb)
//     yt = 0;		  // no title (see below)
    h_user = h_user - yt;
    z0     = 0;
    x0     = yt + h_user / 2;

//  Time, Date and Dimensions.

//    if (!clipb)
    {
      char Text[200];
      sprintf(Text,String(IDS_CAP_GRAF),
                    dispar->zmin,opGrafFile->GetDimension(7),
                    dispar->zmax,opGrafFile->GetDimension(7),
                    dispar->xampl,opGrafFile->GetDimension(0),
                    dispar->yampl,opGrafFile->GetDimension(2),
                    apfac,Tstring);
      opFontX->SetPointSize(int((float)fs1 * f_scal));    
      opOldFont = opDev->SetFont(opFontX);
      if (dispar->aplab)
         opDev->PosStr(XOFFSET + w_user/300 + 1, YOFFSET + h_off, Text);
    }

//  Frame

    POINT Poly[5] = { {XOFFSET,         YOFFSET+yt},
                      {XOFFSET+w_user-1,YOFFSET+yt},
                      {XOFFSET+w_user-1,YOFFSET+h_user-1+yt},
                      {XOFFSET,         YOFFSET+h_user-1+yt},
                      {XOFFSET,         YOFFSET+yt} };
    opDev->PutPolyline(Poly,5);

    int xx = 1 + w_user/200;
    for (i=1; i<10; i++)
    {  int yy = yt + (i * h_user) / 10;
       GraMove(0,yy);
       GraDraw(xx,yy);
       GraMove(w_user-xx,yy);
       GraDraw(w_user,yy);
    }    

//  x-Axis

    GraMove(z0,x0);
    GraDraw(w_user,x0);
    float scale = w_user / (dispar->zmax - dispar->zmin);
    int   izmin = (int) dispar->zmin;
    int   izmax = (int) dispar->zmax;
    int   nn    = izmax - izmin;
    int   k     = 1;
    if (nn > 20)  k = 2;
    if (nn > 50)  k = 5;
    if (nn > 200) k = 20;
    if (nn > 500) k = 50;
    float zbeg = scale * (float(izmin) - dispar->zmin);
    for (i=0; i<=nn; i += k)
    {  int zz = int (zbeg + float(i) * scale);
       GraMove(zz,x0-xx);
       GraDraw(zz,x0+xx);
    }    

//  Set Font for next steps

#ifdef WIN32
    int n = 3;
#else
    int n = (GetScreenWidth()>800) ? 5 : 4;
#endif
    fs2 = (int) (0.015 * (float) (4 * (h_user / n)));
    opFont->SetPointSize(fs2);    
    opDev->SetFont(opFont);

    if (!clipb)
    {
       ch = opDev->GetCharHeight('H');
       cw = opDev->GetCharWidth('H');
    }
    else
    {
       ch = 8;
       cw = 4;
    }
}

long TransGrafWin :: WmEraseBackGround(void)
{
    return FALSE; // tell Windows to not erase background (white flickering)
//    return DefaultEvent();
}

long TransGrafWin :: WmPaint( const PAINTSTRUCT & )
{
//  Handle paint events.

    h_user = GetUserHeight();
    w_user = GetUserWidth();

    f_scal     = (float)1.0;
    opFont     = opFont1;
    opFontX    = opFont3;
    color      = FALSE;
    if (colorflag)
       color   = TRUE;
    colorprint = FALSE;
    clipb      = FALSE;
    h_off      = 0;

    if (DisplayFlag) Dsplay(1);

    return FALSE;
}

VOID TransGrafWin :: ToClipboard(VOID)
{
    f_scal     = (float)1.0;
    opFont     = opFont2;
    opFontX    = opFont2;
    color      = TRUE;
    colorflag  = TRUE;
    printcolorflag = TRUE;
    colorprint = TRUE;
    clipb      = TRUE;
//    h_off      = 0;
    h_off  = h_user / 15;

    DisplayDevice *opDevSaved = opDev;
    MetafileDevice *opMetaDev = new MetafileDevice(NULL);
    opDev = opMetaDev;

    h_user = GetUserHeight();
    w_user = GetUserWidth();

    opMetaDev->SpecialIni(0,w_user,0,h_user); // make picture scaleable

    Dsplay(plw);

    int fs = (int) ((float)fs1 * f_scal);
    opFont->SetPointSize(fs);    
    opDev->SetFont(opFont);
    opDev->PosStr(XOFFSET, YOFFSET + 10, opGrafFile->GetTitleOfRun());


    HMETAFILE hMF  = opMetaDev->Close();
    HWND hWnd = this->GetHandle();
    opMetaDev->WriteClipboard(hWnd,hMF,w_user,h_user);

    delete opMetaDev;
    opDev = opDevSaved;
}

long TransGrafWin :: WmKeyDown( WPARAM Key )
{
    KeyCode = Key;
    return TRUE;
}

long TransGrafWin :: WmLeftMouseDown( POINT &Pos )
{
    TrackPopupMenu(hSubMenu,2,Pos.x,Pos.y,0,this->GetHandle(),NULL);

    return TRUE;
}

long TransGrafWin :: WmRightMouseDown( POINT &Pos )
{
    int  type;
    float Field, par1, par2, par3;
    char *Label;
    RECT rc;

    GetWindowRect(this->GetHandle(),&rc);

    float scalz = (dispar->zmax - dispar->zmin) / float(w_user);
    float z = dispar->zmin + Pos.x * scalz;

    VariDialog *opVari = new VariDialog(IDD_VARI_DIALOG,this,
                                           Pos.x,Pos.y,rc.left,rc.top,
                                           z,opGrafFile);
    Result = opVari->GetResult();
    Field  = opVari->GetField();
    delete opVari;

    if (Result & 2 || Result & 4)
    {  DisplayFlag = FALSE;            // quick exit
       KeyCode = 13;
    }

    if (Result & 1)
    {  
       Label = new char[10];
       opGrafFile->GetParameters(type,par1,par2,par3,Label);
       char for001[150];
       strcpy(for001,SCRDIR);
       strcat(for001,String(IDS_FOR001));
       TransInFile *opInput = new TransInFile(for001);
#ifdef WIN32
#ifdef __BORLANDC__
 wait(10);   // very strange
#endif
#endif
       par2 = Field;
       opInput->ModifyInput(type,par1,par2,par3,Label);
       opInput->OverWrite();
       delete opInput;
       delete []Label; // [] found with Bounds Checker
    }
    return TRUE;
}

int TransGrafWin :: GetResult(VOID)
{
    return Result;
}

UINT TransGrafWin :: WaitForKeyDown(VOID)
{
    KeyCode = 0;
    for (int i=0; i<3000; i++)
    {  wait(100);
       if (KeyCode == 'P')
       {  Print();
          return KeyCode;
       }
       if (KeyCode == 'F')
       {  PrintToFile();
          return KeyCode;
       }
       if (KeyCode == 'C')
       {  ToClipboard();
          return KeyCode;
       }
       else if (KeyCode)   
          break;
    }
    return KeyCode;
}

long TransGrafWin :: WmMenuCommand( WPARAM MenuItem )
{
//  Handle menu command events.

    switch (MenuItem)
    {
	case IDM_PRINT: Print(); break;
	case IDM_PRINT2: PrintToFile(); break;
        case IDM_PASTE: ToClipboard(); break;
        case IDM_EXIT: KeyCode = 13; break;
        default: break;
    }
    return FALSE;
}

long TransGrafWin :: WmSysMenuCommand( WPARAM MenuItem )
{
//  Handle menu command events.

    switch (MenuItem)
    {
	case IDM_PRINT: Print(); return TRUE;
	case IDM_PRINT2: PrintToFile(); return TRUE;
        case IDM_PASTE: ToClipboard(); return TRUE;
        case SC_CLOSE: KeyCode = 13; return TRUE;
        default: return DefaultEvent();
    }
}

VOID TransGrafWin :: Print(VOID)
{
//  print drawing.

    f_scal     = (float)0.8;
    opFont     = opFont2;
    opFontX    = opFont2;
    clipb      = FALSE;
    color      = FALSE;
    colorprint = FALSE;
    if (printcolorflag) 
    {  color      = TRUE;
       colorprint = TRUE;
    }
    h_off  = h_user / 5;

    Printer *opPrt = new Printer;

    opPrt->Start(String(IDS_PLOT),this);

    DisplayDevice *opDevSaved = opDev;
    opDev = opPrt;

//  change scaling

    RECT rc;
    GetWindowRect(this->GetHandle(),&rc);
    int X0 = rc.left; // upper left corner
    int Y0 = rc.top;  // upper left corner
    if (X0 <= 0) X0 = 1;
    if (Y0 <= 0) Y0 = 1;

    int h_user_saved = h_user;
    int w_user_saved = w_user;

    magn_fac  = (float)opPrt->GetUserWidth() / (float)GetScreenWidth();
    magn_fac *= (float)0.9;
    w_user    = int((float)w_user * magn_fac);

//    float aspect_ratio = (float)GetScreenHeight() / (float)GetScreenWidth();
    float aspect_ratio = (float)0.7;

//    h_user    = int((float)h_user * magn_fac);
    h_user = int((float)w_user * aspect_ratio);

    XOFFSET = int(magn_fac * (float)X0);
    YOFFSET = int(magn_fac * (float)Y0);
    XOFFSET += (int)(opPrt->GetUserWidth()/30);  // shift to the right (~1cm)

    Dsplay(plw);

    int fs = (int) ((float)fs1 * f_scal);
    opFont->SetPointSize(fs);    
    opDev->SetFont(opFont);
    opDev->PosStr(XOFFSET + w_user/300 + 1, YOFFSET + fs/2,
		  opGrafFile->GetTitleOfRun());

//    if ( opPrt->NextPage() )
       opPrt->End(this);
    delete opPrt;

//  reset old scales

    opDev    = opDevSaved;
    magn_fac = (float)1.0;
    h_user   = h_user_saved;
    w_user   = w_user_saved;

//  execute VPP if PrinterPort = c:\ps810.prn

    char info[80];
    GetProfileString(String(IDS_WINDOWS),String(IDS_DEVICE,1),"",info,80);
    AnsiLowerBuff(info,strlen(info));
    if (strstr(info,String(IDS_PS810)) != NULL)
       WinExec(String(IDS_VPP),SW_MAXIMIZE);

    XOFFSET = 0;
    YOFFSET = 0;
}

VOID TransGrafWin :: PrintToFile(VOID)
{
//  print drawing.

    f_scal     = (float)0.8;
    opFont     = opFont2;
    opFontX    = opFont2;
    clipb      = FALSE;
    color      = TRUE;
    colorprint = TRUE;
    if (printcolorflag) 
    {  color      = TRUE;
       colorprint = TRUE;
    }
    h_off  = h_user / 5;

    Printer *opPrt = new Printer;

    opPrt->StartPrintToFile(String(IDS_PLOT),this);

    DisplayDevice *opDevSaved = opDev;
    opDev = opPrt;

//  change scaling

    RECT rc;
    GetWindowRect(this->GetHandle(),&rc);
    int X0 = rc.left; // upper left corner
    int Y0 = rc.top;  // upper left corner
    if (X0 <= 0) X0 = 1;
    if (Y0 <= 0) Y0 = 1;

    int h_user_saved = h_user;
    int w_user_saved = w_user;

    magn_fac  = (float)opPrt->GetUserWidth() / (float)GetScreenWidth();
    magn_fac *= (float)0.9;
    w_user    = int((float)w_user * magn_fac);

//    float aspect_ratio = (float)GetScreenHeight() / (float)GetScreenWidth();
    float aspect_ratio = (float)0.7;

//    h_user    = int((float)h_user * magn_fac);
    h_user = int((float)w_user * aspect_ratio);

    XOFFSET = int(magn_fac * (float)X0);
    YOFFSET = int(magn_fac * (float)Y0);
    XOFFSET += (int)(opPrt->GetUserWidth()/30);  // shift to the right (~1cm)

    Dsplay(plw);

    int fs = (int) ((float)fs1 * f_scal);
    opFont->SetPointSize(fs);    
    opDev->SetFont(opFont);
    opDev->PosStr(XOFFSET + w_user/300 + 1, YOFFSET + fs/2,
		  opGrafFile->GetTitleOfRun());

//    if ( opPrt->NextPage() )
       opPrt->End(this);
    delete opPrt;

//  reset old scales

    opDev    = opDevSaved;
    magn_fac = (float)1.0;
    h_user   = h_user_saved;
    w_user   = w_user_saved;

//  execute VPP if PrinterPort = c:\ps810.prn

    char info[80];
    GetProfileString(String(IDS_WINDOWS),String(IDS_DEVICE,1),"",info,80);
    AnsiLowerBuff(info,strlen(info));
    if (strstr(info,String(IDS_PS810)) != NULL)
       WinExec(String(IDS_VPP),SW_MAXIMIZE);

    XOFFSET = 0;
    YOFFSET = 0;
}

VOID TransGrafWin :: DrawEnvelope(int lw)
{
   int   xx, yy, zz;
   int   i, j;
   int   n, deltax, deltay;

   double circle_x[] = { 1., .92, .71, .38,
			 0.,-.38,-.71,-.92,
			-1.,-.92,-.71,-.38,
			.0,  .38, .71, .92,};
   double circle_y[] = { 0., .38, .71, .92,
			 1., .92, .71, .38,
			 0.,-.38,-.71,-.92,
			-1.,-.92,-.71,-.38};

   if (opSecFile) return;
   if (dispar->csx || dispar->csy) return;

   int   np    = w_user / 2;
   int   incz  = 2;
   if (np > 800)   // reduce resolution for printer
   {  np   = w_user / 8;
      incz = 8;
   }
   float scalx = (float) (h_user / 2) / dispar->xampl;
   float scaly = (float) (h_user / 2) / dispar->yampl;
   float *x    = new float[np+1];
   float *y    = new float[np+1];
   float *d    = new float[np+1];
   float *dy   = new float[np+1];
   float *dl   = new float[np+1];

   for (int jj=opGrafFile->GetNumberOfOpenFiles()-1; jj>=0; jj--)
   {  int indmax = 
             opGrafFile->GetEnveloppe(jj,dispar->zmin,dispar->zmax,np,x,y,d,dy,dl);

      POINT *Poly;
      Poly = new POINT[indmax+1];

      if (dispar->rax==1.0 || dispar->ray==1.0) goto DISP;

      if (color && !colorprint)
      {
         if (jj == 0)
           opDev->SetPen(opWhitePen);
         else
           opDev->SetPen(opRedPen);
      }
      else
      {
         if (jj == 0)
           opDev->SetPen(opBlackPen);
         else
           opDev->SetPen(opRedPen);
      }

      for (i=0; i<=indmax; i++)
      {  zz = incz * i;
         if (zz > w_user) break;
         xx = (int) (x[i] * scalx);
         Poly[i].x = XOFFSET + z0 + zz;
         Poly[i].y = YOFFSET + x0 + xx;
      }  
      opDev->PutPolyline(Poly,indmax+1);


      for (i=0; i<=indmax; i++)
      {  zz = incz * i;
         if (zz > w_user) break;
         yy = -(int) (y[i] * scaly);
         Poly[i].x = XOFFSET + z0 + zz;
         Poly[i].y = YOFFSET + x0 + yy;
      }  
      opDev->PutPolyline(Poly,indmax+1);

      if (dispar->bunl)
      {
        for (i=0; i<=indmax; i++)
        {  zz = incz * i;
           if (zz > w_user) break;
           xx = (int) (dl[i] * scalx);      // display bunch length like x
           Poly[i].x = XOFFSET + z0 + zz;
           Poly[i].y = YOFFSET + x0 + xx;
        }  
        opDev->PutPolyline(Poly,indmax+1);
      }

DISP:
      if (dispar->disp)
      {
	 if (color && !colorprint)
         {  
            if (jj == 0)
              opDev->SetPen(opWhiteDottedPen);
            else
              opDev->SetPen(opRedDottedPen);
         }
         else
         {
            if (jj == 0)
              opDev->SetPen(opBlackDottedPen);
            else
              opDev->SetPen(opRedDottedPen);
         }
         opDev->SetBackMode(TRANSPARENT);

	 n = (lw > 1) ? 16 : 1;
	 for (j=0; j<n; j++) // dotted line --> stray on a radius
	 {
	    deltax = (lw / 2) * (int)(circle_x[j] * cw) / 16;
	    deltay = (lw / 2) * (int)(circle_y[j] * cw) / 16;
	    for (i=0; i<=indmax; i++)
	    {  zz = incz * i;
               if (zz > w_user) break;
               xx = (int) (d[i] * scalx);
               Poly[i].x = XOFFSET + z0 + deltax + zz;
               Poly[i].y = YOFFSET + x0 + deltay + xx;
	    }  
	    opDev->PutPolyline(Poly,indmax+1);
	 }

         if (color)
            opDev->SetBackMode(OPAQUE);
      }

      if (dispar->ydisp)
      {
	 if (color && !colorprint)
         {  
            if (jj == 0)
              opDev->SetPen(opWhiteDottedPen);
            else
              opDev->SetPen(opRedDottedPen);
         }
         else
         {
            if (jj == 0)
              opDev->SetPen(opBlackDottedPen);
            else
              opDev->SetPen(opRedDottedPen);
         }
         opDev->SetBackMode(TRANSPARENT);

	 n = (lw > 1) ? 16 : 1;
	 for (j=0; j<n; j++) // dotted line --> stray on a radius
	 {
	    deltax = (lw / 2) * (int)(circle_x[j] * cw) / 16;
	    deltay = (lw / 2) * (int)(circle_y[j] * cw) / 16;
	    for (i=0; i<=indmax; i++)
	    {
	       zz = incz * i;
               if (zz > w_user) break;
               xx = - (int) (dy[i] * scalx);
               Poly[i].x = XOFFSET + z0 + deltax + zz;
               Poly[i].y = YOFFSET + x0 + deltay + xx;
	    }
            opDev->PutPolyline(Poly,indmax+1);
	 }

         if (color)
            opDev->SetBackMode(OPAQUE);
      }
      delete []Poly; // [] found with Bounds Checker
   }
   delete []x;       // dito
   delete []y;
   delete []d;
   delete []dy;
   delete []dl;
}

VOID TransGrafWin :: DrawCSTrajectories(int lw)
{
   int   xx, yy, zz;
   int   i, j;
   int   n, deltax, deltay;

   double circle_x[] = { 1., .92, .71, .38,
			 0.,-.38,-.71,-.92,
			-1.,-.92,-.71,-.38,
			.0,  .38, .71, .92,};
   double circle_y[] = { 0., .38, .71, .92,
			 1., .92, .71, .38,
			 0.,-.38,-.71,-.92,
			-1.,-.92,-.71,-.38};

   if (opSecFile) return;
   if (dispar->rax || dispar->ray) return;
   if (!dispar->csx && !dispar->csy) return;

   int   np    = w_user / 2;
   int   incz  = 2;
   if (np > 800)   // reduce resolution for printer
   {  np   = w_user / 8;
      incz = 8;
   }
   float scalx = (float) (h_user / 2) / dispar->xampl;
   float scaly = (float) (h_user / 2) / dispar->yampl;
   float *cx   = new float[np+1];
   float *cy   = new float[np+1];
   float *sx   = new float[np+1];
   float *sy   = new float[np+1];

   for (int jj=opGrafFile->GetNumberOfOpenFiles()-1; jj>=0; jj--)
   {  int indmax = 
                opGrafFile->GetCSTrajectories(jj,dispar->zmin,dispar->zmax,np,
                                              cx,sx,cy,sy);
      POINT *Poly;
      Poly = new POINT[indmax+1];

      if (dispar->csx)
      {
	 if (color && !colorprint)
         {
            if (jj == 0)
               opDev->SetPen(opWhitePen);
            else
               opDev->SetPen(opRedPen);
         }
         else
         {
            if (jj == 0)
               opDev->SetPen(opBlackPen);
            else
               opDev->SetPen(opRedPen);
         }
         for (i=0; i<=indmax; i++)
         {  zz = incz * i;
            if (zz > w_user) break;
            xx = (int) (fabs(sx[i]) * scaly);
            Poly[i].x = XOFFSET + z0 + zz;
            Poly[i].y = YOFFSET + x0 + xx;
         }  
         opDev->PutPolyline(Poly,indmax+1);

         if (color && !colorprint)
         {
            if (jj == 0)
               opDev->SetPen(opWhiteDottedPen);
            else
               opDev->SetPen(opRedDottedPen);
         }
         else
         {
            if (jj == 0)
               opDev->SetPen(opBlackDottedPen);
            else
               opDev->SetPen(opRedDottedPen);
         }
         opDev->SetBackMode(TRANSPARENT);

	 n = (lw > 1) ? 16 : 1;
	 for (j=0; j<n; j++) // dotted line --> stray on a radius
	 {
	    deltax = (lw / 2) * (int)(circle_x[j] * cw) / 16;
	    deltay = (lw / 2) * (int)(circle_y[j] * cw) / 16;
	    for (i=0; i<=indmax; i++)
	    {
	       zz = incz * i;
               if (zz > w_user) break;
               yy = (int) (fabs(cx[i]) * scalx);
               Poly[i].x = XOFFSET + z0 + deltax + zz;
               Poly[i].y = YOFFSET + x0 + deltay + yy;
	    }  
            opDev->PutPolyline(Poly,indmax+1);
	 }

         if (color)
            opDev->SetBackMode(OPAQUE);
      }

      if (dispar->csy)
      {
	 if (color && !colorprint)
         {
            if (jj == 0)
               opDev->SetPen(opWhitePen);
            else
               opDev->SetPen(opRedPen);
         }
         else
         {
            if (jj == 0)
               opDev->SetPen(opBlackPen);
            else
               opDev->SetPen(opRedPen);
         }
         for (i=0; i<=indmax; i++)
         {  zz = incz * i;
            if (zz > w_user) break;
            xx = -(int) (fabs(sy[i]) * scaly);
            Poly[i].x = XOFFSET + z0 + zz;
            Poly[i].y = YOFFSET + x0 + xx;
         }  
         opDev->PutPolyline(Poly,indmax+1);

         if (color && !colorprint)
         {
            if (jj == 0)
               opDev->SetPen(opWhiteDottedPen);
            else
               opDev->SetPen(opRedDottedPen);
         }
         else
         {
            if (jj == 0)
               opDev->SetPen(opBlackDottedPen);
            else
               opDev->SetPen(opRedDottedPen);
         }
         opDev->SetBackMode(TRANSPARENT);

	 n = (lw > 1) ? 16 : 1;
	 for (j=0; j<n; j++) // dotted line --> stray on a radius
	 {
	    deltax = (lw / 2) * (int)(circle_x[j] * cw) / 16;
	    deltay = (lw / 2) * (int)(circle_y[j] * cw) / 16;
	    for (i=0; i<=indmax; i++)
	    {
	       zz = incz * i;
               if (zz > w_user) break;
               yy = -(int) (fabs(cy[i]) * scalx);
               Poly[i].x = XOFFSET + z0 + deltax + zz;
               Poly[i].y = YOFFSET + x0 + deltay + yy;
	    }  
            opDev->PutPolyline(Poly,indmax+1);
	 }

         if (color)
            opDev->SetBackMode(OPAQUE);
      }
      delete []Poly;
   }
   delete []cx;
   delete []cy;
   delete []sx;
   delete []sy;
}

VOID TransGrafWin :: DrawRays(VOID)
{
   int   xx, yy, zz;
   int   i;

   if (opSecFile) return;
   if (!dispar->rax && !dispar->ray) return;
   if (dispar->csx || dispar->csy)   return;

   int   np    = w_user / 2;
   int   incz  = 2;
   if (np > 800)   // reduce resolution for printer
   {  np   = w_user / 8;
      incz = 8;
   }
   float scalx = (float) (h_user / 2) / dispar->xampl;
   float scaly = (float) (h_user / 2) / dispar->yampl;
   float *x    = new float[np+1];
   float *y    = new float[np+1];

   for (int jj=opGrafFile->GetNumberOfOpenFiles()-1; jj>=0; jj--)
   {  int indmax = opGrafFile->GetRays(jj,dispar->zmin,dispar->zmax,np,x,y);

      POINT *Poly;
      Poly = new POINT[indmax+1];

      if (color && !colorprint)
      {
         if (jj == 0)  
            opDev->SetPen(opWhitePen);
         else
            opDev->SetPen(opRedPen);
      }
      else
      {
         if (jj == 0)  
            opDev->SetPen(opBlackPen);
         else
            opDev->SetPen(opRedPen);
      }

      if (dispar->rax)
      {  for (i=0; i<=indmax; i++)
         {  zz = incz * i;
            if (zz > w_user) break;
            xx = (int) (x[i] * scalx);
            Poly[i].x = XOFFSET + z0 + zz;
            Poly[i].y = YOFFSET + x0 + xx;
         }  
         opDev->PutPolyline(Poly,indmax+1);
      }

      if (dispar->ray)
      {  for (i=0; i<=indmax; i++)
         {  zz = incz * i;
            if (zz > w_user) break;
            yy = -(int) (y[i] * scaly);
            Poly[i].x = XOFFSET + z0 + zz;
            Poly[i].y = YOFFSET + x0 + yy;
         }  
         opDev->PutPolyline(Poly,indmax+1);
      }
      delete []Poly;
   }
   delete []x;
   delete []y;
}

VOID TransGrafWin :: DrawDrifts(VOID)
{
   char  c[2]     = " ";
   float z;
   int   i;
   char  label[5];
   label[4] = '\0';

   if (!dispar->spez)
      return;

   float scalz = (float) (w_user) / (dispar->zmax - dispar->zmin);
   int barlen  = (int)((float)h_user * 0.35);
   int oldz    = -99;
   int signold = -99;
   int sign, offset, dz;

   while (opGrafFile->GetNextDrift(z,label))
   {  int zz = int((float)(z - dispar->zmin) * scalz);
      if (zz < 0) continue;
      if (zz > w_user) break;
      if (label[0] == 'S' && dispar->steer != 0.0) // steering magnet option
      {  sign   = 1;
         offset = ch;
         if (color)
         {  opDev->SetPen(opGreenPen);
            if (colorflag)
               opDev->SetTextColor(RGB(0,255,0));
            else
               opDev->SetTextColor(RGB(255,255,255));
         }
      }
      else
      {  sign   = -1;
         offset = 5 * ch;
         if (color)
         {  opDev->SetPen(opCyanPen);
            if (colorflag)
               opDev->SetTextColor(RGB(0,255,255));
            else
               opDev->SetTextColor(RGB(255,255,255));
         }
      }
      GraMove(zz, x0);
      GraDraw(zz, x0 + sign * barlen);
//if(clipb)
//{
//char buffer[100];
//sprintf(buffer,"%d %d",zz,x0+sign*barlen);
//MessageBox(NULL,buffer,"Bars",MB_OK);
//}

      int xx = x0 + sign * barlen - offset;
      for (i=0; i<4; i++)
      {  c[0] = label[i];
         xx = xx + ch;
         dz = - cw / 2;
         if (zz + dz - oldz < cw && sign == signold) // close together
         {  zz = oldz;
            dz = cw;
         }
         PutChar(zz+dz,xx,c);
      }
      oldz    = zz + dz;
      signold = sign;
   }

   opGrafFile->Rewind();
   if (color && !colorprint)
      opDev->SetTextColor(RGB(255,255,255));
   else
      opDev->SetTextColor(RGB(0,0,0));
}

VOID TransGrafWin :: DrawMagnets(VOID)
{
   char  field[8];
   char  c[2]     = " ";
   float z, l, B, a;
   int   index, i, acctype;
   char  label[5];
   label[4] = '\0';

   float scalx  = apfac * (float) (h_user / 2) / dispar->xampl;
   float scaly  = apfac * (float) (h_user / 2) / dispar->yampl;
   float scalz  = (float) (w_user) / (dispar->zmax - dispar->zmin);
   int   barlen = (int)((float)h_user * 0.05);
   int   oldind = -1;
   int   zoldl = - cw;
   int   zoldf = - cw;

   while (opGrafFile->GetNextMagnet(index,z,l,B,a,label,acctype))
   {
      if (color)
      {  if (oldind != index)
         {  if (index)  // BEND and WIEN
            {  opDev->SetPen(opBluePen);
               if (colorflag)
                  opDev->SetTextColor(RGB(0,0,255));
               else
                  opDev->SetTextColor(RGB(255,255,255));
            }
            else        // QUAD, SEXT and SOLO
            {  opDev->SetPen(opRedPen);
               if (colorflag)
                  opDev->SetTextColor(RGB(255,0,0));
               else
                  opDev->SetTextColor(RGB(255,255,255));
            }
            oldind = index;
         }
      }
      int zz = int((float)(z - dispar->zmin) * scalz);
      int dl = (int)(l * scalz);
      if (zz < 0) continue;
      if (zz - dl > w_user) break;
      int ax = (int)(a * scalx);
      int ay = (int)(a * scaly);

//    special for horizontal BEND and WIEN apertures

      if (index == 1)
      {
	 if (ax < 0)
         {
	    ax *= -1;
            ay = h_user / 2 - (int)((float)h_user * 0.088);
         }
	 else
            ax = h_user / 2 - barlen - 1;
      }

//    draw apertures

      if (acctype == 0)  // normal magnets
      {
	if (dcomp) // complementary representation
	{
          GraMove(zz - dl, x0 + ax);
          GraDraw(zz     , x0 + ax);
          GraDraw(zz     , x0 - ay);
          GraDraw(zz - dl, x0 - ay);
          GraDraw(zz - dl, x0 + ax);
	}
	else // old representation
	{
          GraMove(zz - dl, x0 + ax + barlen);
          GraDraw(zz - dl, x0 + ax);
          GraDraw(zz     , x0 + ax);
          GraDraw(zz     , x0 + ax + barlen + 1);
          GraMove(zz - dl, x0 - ay - barlen);
          GraDraw(zz - dl, x0 - ay);
          GraDraw(zz     , x0 - ay);
          GraDraw(zz     , x0 - ay - barlen - 1);
	}
      }
      else if (acctype == 1) // first accelerator element
      {
	if (dcomp) // complementary representation
	{
          GraMove(zz     , x0 + ax);
          GraDraw(zz - dl, x0 + ax);
          GraDraw(zz - dl, x0 - ay);
          GraDraw(zz     , x0 - ay);
	}
	else // old representation
	{
          GraMove(zz - dl, x0 + ax + barlen);
          GraDraw(zz - dl, x0 + ax);
          GraDraw(zz     , x0 + ax);
          GraMove(zz - dl, x0 - ay - barlen);
          GraDraw(zz - dl, x0 - ay);
          GraDraw(zz     , x0 - ay);
	}
      }
      else if (acctype == 2) // intermediate accelerator element
      {
          GraMove(zz - dl, x0 + ax);
          GraDraw(zz + 1 , x0 + ax);
          GraMove(zz - dl, x0 - ay);
          GraDraw(zz + 1 , x0 - ay);
      }
      else if (acctype == 3) // last accelerator element
      {
	if (dcomp) // complementary representation
	{
          GraMove(zz - dl, x0 + ax);
          GraDraw(zz     , x0 + ax);
          GraDraw(zz     , x0 - ay);
          GraDraw(zz - dl, x0 - ay);
	}
	else // old representation
	{
          GraMove(zz - dl, x0 + ax);
          GraDraw(zz     , x0 + ax);
          GraDraw(zz     , x0 + ax + barlen + 1);
          GraMove(zz - dl, x0 - ay);
          GraDraw(zz     , x0 - ay);
          GraDraw(zz     , x0 - ay - barlen - 1);
	}
      }

      int dz = - dl / 2 - cw / 2;

//    draw labels

      if (dispar->aplab && zz+dz-cw > zoldl)
      {  int xx = x0 - h_user / 2 - ch / 2;
         for (i=0; i<4; i++)
         {  c[0] = label[i];
            xx = xx + ch;
            if (c[0] == ' ') continue;   // don't draw blanks
            PutChar(zz+dz,xx,c);
         }
         zoldl = zz+dz;  // for minimal distance control
      }
//    draw magnetic field settings

      if (dispar->spez2 && zz+dz-cw > zoldf)
      {  sprintf(field,String(IDS_7_3_FORM),B);
         int xx = x0 + h_user / 2 - 8 * ch - ch / 2;
         if (index == 1) xx = xx - 3 * ch; // BEND special
         for (i=0; i<7; i++)
         {  c[0] = field[i];
            xx = xx + ch;
            if (c[0] == ' ') continue;   // don't draw blanks
            PutChar(zz+dz,xx,c);
         }
         zoldf = zz+dz; // for minimal distance control
      }
   }
   opGrafFile->Rewind();
   if (color && !colorprint)
      opDev->SetTextColor(RGB(255,255,255));
   else
      opDev->SetTextColor(RGB(0,0,0));
}

VOID TransGrafWin :: DrawSlits(VOID)
{
   char  c[2]     = " ";
   float z, x, y;
   int   i;
   char  label[5];
   label[4] = '\0';

   float scalx  = apfac * (float) (h_user / 2) / dispar->xampl;
   float scaly  = apfac * (float) (h_user / 2) / dispar->yampl;
   float scalz  = (float) (w_user) / (dispar->zmax - dispar->zmin);
   int   barlen = (int)((float)h_user * 0.05);
   int   dx     = barlen / 10;
   int   dy     = barlen / 5;
   int oldz     = -99;

   if (color && !colorprint)
      opDev->SetPen(opWhitePen);
   else
      opDev->SetPen(opBlackPen);

   while (opGrafFile->GetNextSlit(z,x,y,label))
   {  int zz = int((float)(z - dispar->zmin) * scalz);
      if (zz < 0) continue;
      if (zz > w_user) break;
      int ax = (int)(x * scalx);
      int ay = (int)(y * scaly);

//    draw apertures

      if (x > 0.)
      {  GraMove(zz,      x0 + ax + barlen);
         GraDraw(zz,      x0 + ax);
         GraMove(zz,      x0 + ax);
         GraDraw(zz - dx, x0 + ax + dy);
         GraMove(zz,      x0 + ax);
         GraDraw(zz + dx, x0 + ax + dy);
      }
      if (y > 0.)
      {  GraMove(zz,      x0 - ay - barlen);
         GraDraw(zz,      x0 - ay);
         GraMove(zz,      x0 - ay);
         GraDraw(zz - dx, x0 - ay - dy);
         GraMove(zz,      x0 - ay);
         GraDraw(zz + dx, x0 - ay - dy);
      }

//    draw labels

      if (dispar->aplab)
      {  if (label[0] == ' ' && label[1] == ' ') continue;
         int dz = - cw / 2;
         if (zz - oldz < cw) continue; // close together
         int xx = x0 + h_user / 2 - 5 * ch - ch / 2;
         for (i=0; i<4; i++)
         {  c[0] = label[i];
            xx = xx + ch;
            if (c[0] == ' ') continue;   // don't draw blanks
            PutChar(zz+dz,xx,c);
            oldz = zz;
         }

      }
   }
   opGrafFile->Rewind();
}

VOID TransGrafWin :: DrawFits(VOID)
{
   int   index, sign, dy;
   float z, val;
   int   x1, x2, x3, x4, y1, y2, y3, y4;

   if (opSecFile) return;
   if (dispar->measx == 0 && dispar->measy == 0 && dispar->measd == 0)
      return;
   if (dispar->csx || dispar->csy) return;

   float scalx  = (float) (h_user / 2) / dispar->xampl;
   float scaly  = (float) (h_user / 2) / dispar->yampl;
   float scalz  = (float) (w_user) / (dispar->zmax - dispar->zmin);
   int   barlen = (int)((float)h_user * 0.015);

   if (color && !colorprint)
      opDev->SetPen(opWhitePen);
   else
      opDev->SetPen(opBlackPen);

   while (opGrafFile->GetNextFit(index,z,val))
   {  int zz = int((float)(z - dispar->zmin) * scalz);
      if (zz < 0) continue;
      if (zz > w_user) break;
      int ax = (int)(val * scalx);
      int ay = (int)(val * scaly);
      if (index == 1 || index == 2)
      {  sign = 1; dy = ax;
         if (index == 2) {sign = -1; dy = ay;}
         x1 = zz - barlen / 2; y1 = x0 + sign * dy;
         x2 = x1 + barlen + 1; y2 = y1;
         x3 = zz;              y3 = y1;
         x4 = x3;              y4 = y3 + sign * barlen;
      }
      else if (index == 5)
      {  sign = 1; dy = ax;
         x1 = zz - barlen / 2; y1 = x0 + sign * dy;
         x2 = x1 + barlen + 1; y2 = y1;
         x3 = zz;              y3 = y1;
         x4 = x3;              y4 = y3 + sign * barlen;
      }
      else
      {  dy = ax;
         if (index == 4) dy = -ay;
         x1 = zz - barlen / 2; y1 = x0 + dy;
         x2 = x1 + barlen + 1; y2 = y1;
         x3 = zz;              y3 = y1 - barlen / 2;
         x4 = x3;              y4 = y3 + barlen + 1;
      }
      if ((dispar->measx == 1 &&  index == 1) ||
          (dispar->measy == 1 &&  index == 2) ||
          (dispar->bunl  == 1 &&  index == 5) ||    // BUNL
          (dispar->measd == 1 && (index == 3  || index == 4)))
      {  GraMove(x1,y1);
         GraDraw(x2,y2);
         GraMove(x3,y3);
         GraDraw(x4,y4);
      }
   }
   opGrafFile->Rewind();
}

VOID TransGrafWin :: DrawSecondOrder(VOID)
{
   char  Text[20];
   float x, ymax, YMAX;
   int   nr, indmax, xx, zz, xxold=0, zzold=0;
   BOOL  first;

   if (!opSecFile) return;

   nr = opSecFile->GetNumberOfElements();
   float *z     = new float[nr+1];
   float *t1    = new float[nr+1];
   float *t2    = new float[nr+1];
   float *t3    = new float[nr+1];
   float *t4    = new float[nr+1];

   for (int jj=0; jj<opSecFile->GetNumberOfOpenFiles(); jj++)
   {
      nr = opSecFile->GetMatrixElements(jj,indmax,ymax,z,t1,t2,t3,t4);
      if (jj == 0) YMAX = ymax;
      float scalz  = (float) (w_user) / (dispar->zmax - dispar->zmin);
      int dl       = h_user / 150;
      int px       = (int)((float)w_user * 0.96);
      int pxx      = (int)((float)w_user * 0.95);
      int py       = x0 - h_user / 2 - ch / 2;
      int yoff     = (int) (0.01 * (float) h_user);

      if (YMAX <= 0.000001)
      {
         KeyCode = 999; // all Tijks are zero, do not wait
	 delete []z;    // delete found with Bounds Checker
	 delete []t1;   // dito
	 delete []t2;
	 delete []t3;
	 delete []t4;
         return;
      }

      if (ymax > 0.000001)
      {  float scalx  = (float) (h_user / 2) / YMAX;
         for (int i=0; i<indmax; i++)
         {  first = TRUE;
            if (color && !colorprint)
            {  switch (i)
               {  case 0: opDev->SetPen(opYellowPen); 
                          opDev->SetTextColor(RGB(255,255,0));
                          break;
                  case 1: opDev->SetPen(opMagentaPen);
                          opDev->SetTextColor(RGB(255,0,255));
                          break;
                  case 2: opDev->SetPen(opWhitePen);
                          opDev->SetTextColor(RGB(255,255,255));
                          break;
                  case 3: opDev->SetPen(opGreenPen);
                          opDev->SetTextColor(RGB(0,255,0));
                          break;
                  default: break;
               }  
            }
            else if (color && colorprint)
            {  switch (i)
               {  case 0: opDev->SetPen(opBlackPen); 
                          opDev->SetTextColor(RGB(0,0,0));
                          break;
                  case 1: opDev->SetPen(opMagentaPen);
                          opDev->SetTextColor(RGB(255,0,255));
                          break;
                  case 2: opDev->SetPen(opGreenPen);
                          opDev->SetTextColor(RGB(0,255,0));
                          break;
                  case 3: opDev->SetPen(opRedPen);
                          opDev->SetTextColor(RGB(255,0,0));
                          break;
                  default: break;
               }  
            }
            else
            {   opDev->SetPen(opBlackPen);
                opDev->SetTextColor(RGB(0,0,0));
            }
            for (int j=0; j<nr; j++)
            {  switch(i)
               {  case 0: x = t1[j]; break;
                  case 1: x = t2[j]; break;
                  case 2: x = t3[j]; break;
                  case 3: x = t4[j]; break;
                  default:            break;
               }
               xx = x0 - (int) (x * scalx);
               zz = int((float)(z[j] - dispar->zmin) * scalz);
               if (zz >= 0 && zz <= w_user)
               {  if (first)
                    first = FALSE;
                  else
                  {  GraMove(zzold,xxold);
                     GraDraw(zz,xx);
                  }
                  zzold = zz;
                  xxold = xx;
                  switch(i)
                  {  case 0: DrawCross(zz,xx,dl);    break;
                     case 1: DrawX(zz,xx,dl);        break;
                     case 2: DrawRombus(zz,xx,dl);   break;
                     case 3: DrawSquare(zz,xx,dl);   break;
                     default:                        break;
                  }
               }
            }

//          Legend (identification for each curve)

            if (dispar->aplab)
            {  py = py + ch;
               opDev->PosStr(px,py,opSecFile->GetTijkLabel(i));
               switch(i)
               {  case 0: DrawCross(pxx,py+yoff,dl);    break;
                  case 1: DrawX(pxx,py+yoff,dl);        break;
                  case 2: DrawRombus(pxx,py+yoff,dl);   break;
                  case 3: DrawSquare(pxx,py+yoff,dl);   break;
                  default:                              break;
               }
            }
         }
      }
   }
      
   if (color && !colorprint)
      opDev->SetTextColor(RGB(255,255,255));
   else
      opDev->SetTextColor(RGB(0,0,0));

//   opFontX->SetPointSize(int((float)fs1 * f_scal));    
   opDev->SetFont(opFontX);
   sprintf(Text,String(IDS_TMAX),YMAX);
   if (dispar->aplab)
      opDev->PosStr(6*(w_user/7),h_off,Text);

   delete []z;
   delete []t1;
   delete []t2;
   delete []t3;
   delete []t4;
}

VOID TransGrafWin :: Dsplay(int w)
{
   CreatePens(w);
   DrawFrame();
   DrawMagnets();
   DrawSlits();
   DrawDrifts();
   DrawFits();
   DrawEnvelope(w);
   DrawRays();
   DrawCSTrajectories(w);
   DrawSecondOrder();
   opDev->SetFont(opOldFont);
   if (color && !colorprint)
      opDev->SetPen(opOldPen);
   DestroyPens();
}

///////////////////////////////// EOF /////////////////////////////////////////
