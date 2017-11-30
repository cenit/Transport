///////////////////////////////////////////////////////////////////////////////
/////////////////////// class GrafWin implementation //////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

#define PREC  40    // precission for mouse 
#define PREC2 30    // precission for mouse 

extern "C"
{
   double power(double x, double y);
}

GrafWin :: GrafWin(int nCmdShow, LPCSTR szWindowName, RECT rc, Window *opParent,
           BOOL cf, BOOL pcf, IOFILE *iodp, COLORS *cl) 
         : Window(nCmdShow,szWindowName,rc)
{
    hPrntWnd  = opParent->GetHandle();
    opPar     = opParent;
    Col       = cl;
    dp        = iodp;
    colorflag = cf;
    printcolorflag = pcf;
    opDev = this;
    toprint = FALSE;

    opFont1 = new Font("Times New Roman");
    opFont2 = new Font("Times New Roman");

    // Load Popup Menu

    opPopupMenu = new Menu(IDR_POPUP_MENU,(Window *)NULL);

    hSubMenu = GetSubMenu(opPopupMenu->GetHandle(),0);

    // Append to System menu

    SystemMenu oSystemMenu(this);   /* Get system menu	      */
    oSystemMenu.AppendSep();	      /* Append separator bar and     */
    oSystemMenu.Append(IDM_PRINT,"Print Graphics\tP");
    oSystemMenu.Append(IDM_PRINT2,"Print Graphics to File\tF");
    oSystemMenu.AppendSep();	      /* Append separator bar and     */
    oSystemMenu.Append(IDM_PASTE,"Graphics to Clipboard\tC");
    oSystemMenu.Disable(SC_CLOSE);

    opPen1   = new Pen(PS_SOLID,1,
                   RGB(Col->ContLine1.r,Col->ContLine1.g,Col->ContLine1.b));
    opPen2   = new Pen(PS_SOLID,1,
                   RGB(Col->ContLine2.r,Col->ContLine2.g,Col->ContLine2.b));
    opPen3   = new Pen(PS_SOLID,1,
                   RGB(Col->AxisLines.r,Col->AxisLines.g,Col->AxisLines.b));
    opPen4   = new Pen(PS_SOLID,1,
                   RGB(Col->GridLines.r,Col->GridLines.g,Col->GridLines.b));
    opOldPen = NULL;

    hCursor    = LoadCursor(NULL,IDC_ARROW);
    hArrow     = LoadCursor(NULL,IDC_ARROW);
    hUpDown    = LoadCursor(Application::GetInstance(),"udcursor");
    hLeftRight = LoadCursor(Application::GetInstance(),"lrcursor");

    KeyCode = 0;
    statistics = FALSE;

    Histo = FALSE;
    Cont  = FALSE;
    Lego  = FALSE;
    Grid  = FALSE;
    CheckMenu(Cont,Lego,Grid);

    pon = FALSE;
    u1 = v1 = u2 = v2 = 0;
    u3 = v3 = u4 = v4 = 0;
    signumx = 1;
    signumy = -1;

    opDev = this;
    opSaved = opDev;
    BMWidth = GetUserWidth();
    BMHeight = GetUserHeight();
    opMem = new MemoryDevice(BMWidth, BMHeight);    

    closed = FALSE;
    Invalidate2();
}

GrafWin :: ~GrafWin(VOID)
{
    GrafWin::WmClose();
}

BOOL GrafWin :: WmClose( void )
{
    if (!closed)
    {
       delete opPen1;
       delete opPen2;
       delete opPen3;
       delete opPen4;

       delete opFont1;
       delete opFont2;

       delete opMem;

       delete opPopupMenu;

       closed = TRUE;
    }
    return FALSE;
}

BOOL GrafWin :: WmDestroy( void )
{
    return FALSE;
}

VOID GrafWin :: DrawFrame2(VOID)
{
    Brush *opBrush, *opOld;

    if (color)
    {  opOldPen = opDev->SetPen(opPen3);
       opDev->SetTextColor(RGB(Col->AxisText.r,
                               Col->AxisText.g,
                               Col->AxisText.b));
    }

//  draw screen (default yellow)

    opDev->SetBackMode(OPAQUE);
    if (color)
    {  opDev->SetBackColor(RGB(Col->Screen.r,Col->Screen.g,Col->Screen.b));
       opBrush = new Brush(RGB(Col->Screen.r,Col->Screen.g,Col->Screen.b));
    }
    else
    {  opDev->SetBackColor(RGB(255,255,255));
       opBrush = new Brush(RGB(255,255,255));
    }
    opOld = opDev->SetBrush(opBrush);

    hu = (float)h_user;
    wu = (float)w_user;
    if (toprint)
	hu = wu / (float) 1.43 ;  // only landscape aspect ratio
    opDev->PutRectangle(-1,-1,int(wu+1.),int(hu+1.));
    opDev->SetBrush(opOld);
}

VOID GrafWin :: DrawFrame(VOID)
{
    Brush *opBrush, *opOld;

    h_user = opDev->GetUserHeight();    // under Windows 3.1 gives 0 for clipboard
    w_user = opDev->GetUserWidth();     // under Windows 3.1 gives 0 for clipboard
    h_char = opDev->GetCharHeight('H'); // gives nonesens for clipboard
    if (clipb)
    {
       h_user = H_USER;
       h_char = H_CHAR;
    }
    else
    {
       H_USER = h_user;
       H_CHAR = h_char;
    }
    if (clipb)
       w_user = W_USER;
    else
       W_USER = w_user;

    if (color)
    {  opOldPen = opDev->SetPen(opPen3);
       opDev->SetTextColor(RGB(Col->AxisText.r,
                               Col->AxisText.g,
                               Col->AxisText.b));
    }

//  draw screen (default yellow)

    opDev->SetBackMode(OPAQUE);
    if (color)
    {  opDev->SetBackColor(RGB(Col->Screen.r,Col->Screen.g,Col->Screen.b));
       opBrush = new Brush(RGB(Col->Screen.r,Col->Screen.g,Col->Screen.b));
    }
    else
    {  opDev->SetBackColor(RGB(255,255,255));
       opBrush = new Brush(RGB(255,255,255));
    }
    opOld = opDev->SetBrush(opBrush);
    hu = (float)h_user;
    wu = (float)w_user;
    if (toprint)
	hu = wu / (float) 1.43 ;  // only landscape aspect ratio
    opDev->PutRectangle(-1,-1,int(wu+1.),int(hu+1.));
    opDev->SetBrush(opOld);

#ifndef WIN32
//    int char_height = (w_user > h_user) ? h_user/36 : w_user/36 ;
    int char_height = int(hu/36.) ;
#else
//    int char_height = (w_user > h_user) ? h_user/24 : w_user/24 ;
    int char_height = int(hu/24.) ;
#endif
    char_height = (GetScreenWidth()>800) ? (4 * char_height) /5 : char_height;
    opFont1->SetPointSize(char_height);
    opFont2->SetPointSize(char_height);
    opFont2->SetEscapement(900);

    if (color)
       opDev->SetPen(opOldPen);

    delete opBrush;
}

long GrafWin :: WmEraseBackGround(void)
{
    return FALSE; // tell Windows to not erase background (white flickering)
//    return DefaultEvent();
}

long GrafWin :: WmSize(int w, int h)
{
    delete opMem;
    BMWidth  = w;
    BMHeight = h;
    w_user   = w;
    h_user   = h;
//    h_char   = opDev->GetCharHeight('H');
    opMem    = new MemoryDevice(BMWidth, BMHeight);    

#ifndef WIN32
    int char_height = (w_user > h_user) ? h_user/36 : w_user/36 ;
#else
    int char_height = (w_user > h_user) ? h_user/24 : w_user/24 ;
#endif
    char_height = (GetScreenWidth()>800) ? (4 * char_height) /5 : char_height;
    opFont1->SetPointSize(char_height);
    opFont2->SetPointSize(char_height);
    opFont2->SetEscapement(900);

    return DefaultEvent();
}

BOOL GrafWin :: WmSetCursor( void )
{
   SetCursor(hCursor);
   return FALSE;
}

long GrafWin :: WmPaint( const PAINTSTRUCT &)
{
//  Handle paint events.

    clipb = FALSE;
    if (colorflag)
       color = TRUE;
    else
       color = FALSE;
    colorprint = FALSE;

    if (Histo)
    {
	Cont = FALSE; Lego = FALSE; Grid = FALSE;
	CheckMenu(Cont,Lego,Grid);
	opDev = opMem;  // draw into memory context
	DrawFrame2();
	DrawHist();
	Timds();
	opDev = opSaved;
	BitBlt(this->GetDC(),0,0,BMWidth,BMHeight,opMem->GetDC(),0,0,SRCCOPY);
    }
    else if (Cont)
    {
	Lego = FALSE; Grid = FALSE;
	CheckMenu(Cont,Lego,Grid);
	opDev = opMem;  // draw into memory context
	DrawFrame2();
	DrawCont();
	Timds();
	opDev = opSaved;
	BitBlt(this->GetDC(),0,0,BMWidth,BMHeight,opMem->GetDC(),0,0,SRCCOPY);
    }
    else if (Lego)
    {
	Cont = FALSE; Grid = FALSE;
	CheckMenu(Cont,Lego,Grid);
	opDev = opMem;  // draw into memory context
	DrawFrame2();
	DrawLego();
	Timds();
	opDev = opSaved;
	BitBlt(this->GetDC(),0,0,BMWidth,BMHeight,opMem->GetDC(),0,0,SRCCOPY);
    }
    else if (Grid)
    {
	Cont = FALSE; Lego = FALSE;
	CheckMenu(Cont,Lego,Grid);
	opDev = opMem;  // draw into memory context
	DrawFrame2();
	DrawGrid();
	Timds();
	opDev = opSaved;
	BitBlt(this->GetDC(),0,0,BMWidth,BMHeight,opMem->GetDC(),0,0,SRCCOPY);
    }
    else
    {
	DrawFrame();
    }
    return FALSE;
}

VOID GrafWin :: NewPenColors(VOID)
{
    delete opPen1;
    delete opPen2;
    delete opPen3;
    delete opPen4;
    opPen1  = new Pen(PS_SOLID,1,
                  RGB(Col->ContLine1.r,Col->ContLine1.g,Col->ContLine1.b));
    opPen2  = new Pen(PS_SOLID,1,
                  RGB(Col->ContLine2.r,Col->ContLine2.g,Col->ContLine2.b));
    opPen3  = new Pen(PS_SOLID,1,
                  RGB(Col->AxisLines.r,Col->AxisLines.g,Col->AxisLines.b));
    opPen4  = new Pen(PS_SOLID,1,
                  RGB(Col->GridLines.r,Col->GridLines.g,Col->GridLines.b));
}

long GrafWin :: WmKeyDown(WPARAM Key)
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
    if (KeyCode=='c' || KeyCode=='C')
       ToClipboard();
    if (KeyCode=='e' || KeyCode=='E')
       KeyCode = 777;

    hCursor = hArrow;

    return TRUE;
}

UINT GrafWin :: GetKey(VOID)
{
    return KeyCode;
}

UINT GrafWin :: WaitForKeyDown(VOID)
{
    KeyCode = 0;
    for (int i=0; i<3000; i++)
    {  wait(100);
       if (KeyCode)
          break;
    }
    return KeyCode;
}

long GrafWin :: WmMenuCommand(WPARAM MenuItem)
{
    switch (MenuItem)
    {
	case IDM_PRINT2: PrintToFile(); break;
        case IDM_PRINT: Print(); break;
        case IDM_PASTE: ToClipboard(); break;
        case IDM_CONTINUE: KeyCode = 13; break;
        case IDM_EXIT: KeyCode = 777; break;
	case IDC_LIN: scltyp = LIN; CheckMenu(scltyp); Invalidate2(); break;
	case IDC_LOG: scltyp = LOG; CheckMenu(scltyp); Invalidate2(); break;
	case IDC_CONT: Cont = TRUE; Lego = FALSE; Grid = FALSE;
		       if (Nniv == 0) Nniv = 10;
                       CheckMenu(Cont,Lego,Grid); Invalidate2(); break;
	case IDC_LEGO: Lego = TRUE; Cont = FALSE; Grid = FALSE;
		       if (Phi == 0) Phi = 45;	if (Tet == 0) Tet = 20;
		       CheckMenu(Cont,Lego,Grid); Invalidate2(); break;
	case IDC_GRID: Grid = TRUE; Cont = FALSE; Lego = FALSE;
		       if (Phi == 0) Phi = 45; if (Tet == 0) Tet = 20;
		       CheckMenu(Cont,Lego,Grid); Invalidate2(); break;
        default: break;
    }
    return FALSE;
}

long GrafWin :: WmSysMenuCommand(WPARAM MenuItem)
{
    switch (MenuItem)
    {
	case IDM_PRINT: Print(); return TRUE;
 	case IDM_PRINT2: PrintToFile(); return TRUE;
        case IDM_PASTE: ToClipboard(); return TRUE;
        case IDM_CONTINUE: KeyCode = 13; return TRUE;
        case IDM_EXIT: KeyCode = 777; return TRUE;
        default: return DefaultEvent();
    }
}

VOID GrafWin :: Erase(VOID)
{
    if (colorflag)
       color = TRUE;
    else
       color = FALSE;

    Histo = FALSE;
    Cont  = FALSE;
    Lego  = FALSE;
    Grid  = FALSE;

    Invalidate2();
}

VOID GrafWin :: ToClipboard(VOID)
{
//  copy to clipboard

    MetafileDevice *opMetaDev = new MetafileDevice(NULL); // makes it scaleable
    opDev = opMetaDev;

    color = FALSE;
    clipb = TRUE;

    opMetaDev->SpecialIni(0,W_USER,0,H_USER);

    DrawFrame();

    if (Histo)
       DrawHist();
    else if (Cont)
       DrawCont();
    else if (Lego)
       DrawLego();
    else if (Grid)
       DrawGrid();

    if (Histo || Cont || Lego || Grid)
       Timds();

    HMETAFILE hMF  = opMetaDev->Close();
    HWND hWnd = this->GetHandle();
    opMetaDev->WriteClipboard(hWnd,hMF,w_user,h_user);

    delete opMetaDev;
    opDev = opSaved;

//  enforce repainting of everything

    RECT rc;
    GetWindowRect(this->GetHandle(),&rc);
    int x = rc.left;
    int y = rc.top;
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    MoveWindow(this->GetHandle(),x,y,w+1,h+1,TRUE);
    MoveWindow(this->GetHandle(),x,y,w  ,h  ,TRUE);
    Invalidate();
}

VOID GrafWin :: Print(VOID)
{
//  print drawing.

    Printer *opPrt = new Printer;

    opPrt->Start("Output of Graphic Data from TURTLE",this);

    opDev = opPrt;
    int hhh = h_char;
    int HHH = H_CHAR;

    toprint = TRUE;
    clipb = FALSE;
    color = FALSE;
    colorprint = FALSE;
    if (printcolorflag)
    {  color = TRUE;
       colorprint = TRUE;
    }

    DrawFrame();

    if (Histo)
       DrawHist();
    else if (Cont)
       DrawCont();
    else if (Lego)
       DrawLego();
    else if (Grid)
       DrawGrid();

    if (Histo || Cont || Lego || Grid)
       Timds();

//  finish
    
//    if ( opPrt->NextPage() )
       opPrt->End(this);
    delete opPrt;

    opDev = opSaved;
    toprint = FALSE;
    h_char = hhh;
    H_CHAR = HHH;

//  enforce repainting of everything

    RECT rc;
    GetWindowRect(this->GetHandle(),&rc);
    int x = rc.left;
    int y = rc.top;
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    MoveWindow(this->GetHandle(),x,y,w+1,h+1,TRUE);
    MoveWindow(this->GetHandle(),x,y,w  ,h  ,TRUE);
    Invalidate();
}

VOID GrafWin :: PrintToFile(VOID)
{
//  print drawing.

    Printer *opPrt = new Printer;

    opPrt->StartPrintToFile("Output of Graphic Data from TURTLE",this);

    opDev = opPrt;
    int hhh = h_char;
    int HHH = H_CHAR;

    toprint = TRUE;
    clipb = FALSE;
    color = FALSE;
    colorprint = FALSE;
    if (printcolorflag)
    {  color = TRUE;
       colorprint = TRUE;
    }

    DrawFrame();

    if (Histo)
       DrawHist();
    else if (Cont)
       DrawCont();
    else if (Lego)
       DrawLego();
    else if (Grid)
       DrawGrid();

    if (Histo || Cont || Lego || Grid)
       Timds();

//  finish
    
//    if ( opPrt->NextPage() )
       opPrt->End(this);
    delete opPrt;

    opDev = opSaved;
    toprint = FALSE;
    h_char = hhh;
    H_CHAR = HHH;

//  enforce repainting of everything

    RECT rc;
    GetWindowRect(this->GetHandle(),&rc);
    int x = rc.left;
    int y = rc.top;
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    MoveWindow(this->GetHandle(),x,y,w+1,h+1,TRUE);
    MoveWindow(this->GetHandle(),x,y,w  ,h  ,TRUE);
    Invalidate();
}

long GrafWin :: WmLeftMouseDown( POINT &Pos )
{
    if (Lego || Grid)
    {
	pon    = TRUE;
	x0     = Pos.x;
	y0     = Pos.y;
	Phi0   = Phi;
	Tet0   = Tet; 
	if ( Pos.x > u1 - PREC && Pos.x < u1 + PREC &&
             Pos.y > v1 - PREC && Pos.y < v1 + PREC )
	{
	   signumx = 1;
	   signumy = -1;
	   hCursor = hUpDown;
	}
	else if ( Pos.x > u4 - PREC && Pos.x < u4 + PREC &&
                  Pos.y > v4 - PREC && Pos.y < v4 + PREC )
	{
	   signumx = 1;
	   signumy = -1;
	   hCursor = hLeftRight;
	}
	else if ( Pos.x > u3 - PREC && Pos.x < u3 + PREC &&
                  Pos.y > v3 - PREC && Pos.y < v3 + PREC )
	{
	   signumx = 1;
	   signumy = -1;
	   hCursor = hUpDown;
	}
	else if ( Pos.x > u2 - PREC && Pos.x < u2 + PREC &&
                  Pos.y > v2 - PREC && Pos.y < v2 + PREC )
	{
	   signumx = -1;
	   signumy = -1;
	   hCursor = hLeftRight;
	}
	else
	{
	   pon = FALSE;
	   hCursor = hArrow;
	   TrackPopupMenu(hSubMenu,2,Pos.x,Pos.y,0,this->GetHandle(),NULL);
	}
    }
    else
    {
	pon = FALSE;
	hCursor = hArrow;
	TrackPopupMenu(hSubMenu,2,Pos.x,Pos.y,0,this->GetHandle(),NULL);
    }
    return TRUE;
}

long GrafWin :: WmMouseMove( POINT &Pos, WPARAM keyflags)
{
    char Text[180];

    if (Lego || Grid)
    {
	sprintf(Text,"Drag with left mouse button (+/- Ctrl or Shft) to change Theta = %d, Phi = %d",Tet,Phi);
	if (!pon)
	{
	   if ( Pos.x > u1 - PREC && Pos.x < u1 + PREC &&
                Pos.y > v1 - PREC && Pos.y < v1 + PREC )
	   {
	      hCursor = hUpDown;
	      SetWindowText(this->GetHandle(),Text);
	   }
	   else if ( Pos.x > u4 - PREC && Pos.x < u4 + PREC &&
                     Pos.y > v4 - PREC && Pos.y < v4 + PREC )
	   {
	      hCursor = hLeftRight;
	      SetWindowText(this->GetHandle(),Text);
	   }
	   else if (Pos.x > u3 - PREC && Pos.x < u3 + PREC &&
                    Pos.y > v3 - PREC && Pos.y < v3 + PREC )
	   {
	      hCursor = hUpDown;
	      SetWindowText(this->GetHandle(),Text);
	   }
	   else if ( Pos.x > u2 - PREC && Pos.x < u2 + PREC &&
                     Pos.y > v2 - PREC && Pos.y < v2 + PREC )
	   {
	      hCursor = hLeftRight;
	      SetWindowText(this->GetHandle(),Text);
	   }
	   else
	   {
	      hCursor = hArrow;
	      SetWindowText(this->GetHandle(),"Turtle Histogram");
	   }
	}
    }
    else
    {
	hCursor = hArrow;
    }

    if (pon)
    {
	Phi = MAX( MIN( Phi0 + signumx * (int(Pos.x) - x0) / 5, 90 ), 0 );
	Tet = MAX( MIN( Tet0 + signumy * (int(Pos.y) - y0) / 5, 90 ), 0 );
	sprintf(Text,"Theta = %d, Phi = %d",Tet,Phi);
	SetWindowText(this->GetHandle(),Text);
        if (Pos.x<=PREC2 || Pos.x>=w_user-PREC2 || Pos.y<=PREC2 || Pos.y>=h_user-PREC2)
	{
           pon = FALSE;
	   SetWindowText(this->GetHandle(),"Turtle Histogram");
	   hCursor = hArrow;
	}
	else
	{
	   if (keyflags & MK_CONTROL)  // if Control key is pressed ..
		Invalidate2();
	   if (keyflags & MK_SHIFT)  // or if Shift key is pressed ..
		Invalidate2();
	}
    }
    return TRUE;
}

long GrafWin :: WmLeftMouseUp( POINT & )
{
    if (pon)
    {
	pon = FALSE;
	hCursor = hArrow;
	SetWindowText(this->GetHandle(),"Turtle Histogram");
	Invalidate2();
    }
    return TRUE;
}

long GrafWin :: WmRightMouseDown( POINT &)
{
    hCursor = hArrow;
    KeyCode = 13;

    return TRUE;
}


VOID GrafWin :: Hist(BOOL LossHist, int nh, TUGDATA TugData)
{
   statistics = FALSE;
   if (nh >= 1000)
   {
      nh -= 1000; 
      statistics = TRUE;
   }

   if (!LossHist)
   {  np     = dp->nentry[nh]-1;
      N      = dp->intvls[nh];
      Nunit  = dp->ncoor[nh]-1;
      Xmin   = dp->begin[nh];
      Xmax   = dp->end[nh];
      Xstep  = dp->step[nh];
      zcoord = dp->z[nh];
      strcpy(zlabel,dp->lab[nh]);
      Xmom   = dp->xmom[nh];
      Xmom2  = dp->xmom2[nh];
      Xsum   = (dp->mtable[np+N+2]) - (dp->mtable[np+N+1]) - (dp->mtable[np+N]);
   }
   else
   {  np     = dp->nblk[nh]-1;
      N      = dp->lthx[nh];
      Nunit  = dp->nunx[nh]-1;
      Xmin   = dp->xstrt[nh];
      Xmax   = dp->xend[nh];
      Xstep  = dp->xbin[nh];
      zcoord = (float)-9999.9;
      strcpy(zlabel,"&&&&");
      Xsum   = (float)-999.0; // signal, that statistical data have to be computed
   }
   scltyp = TugData.scltyp;
   CheckMenu(scltyp); 
   Nhist  = TugData.nh;
   Histo = TRUE;

   Invalidate2();
}

VOID GrafWin :: ContPlot(BOOL LossHist, int nh, TUGDATA TugData)
{
   statistics = FALSE;
   if (nh >= 1000)
   {
      nh -= 1000; 
      statistics = TRUE;
   }

   if (!LossHist)
   {  
      zcoord = dp->z[nh];
      strcpy(zlabel,dp->lab[nh]);
      for (int i=nh-1; i>=0; i--)
      {  if (dp->mtype[i] == 2)     // search for nearest previous x-coordinate
         {  np     = dp->nentry[i]-1;
            N      = dp->intvls[i];
            Nunit  = dp->ncoor[i]-1;
            Xmin   = dp->begin[i];
            Xmax   = dp->end[i];
            Xstep  = dp->step[i];
            break;
         }      
      }
      Ny     = dp->intvls[nh];
      Nunity = dp->ncoor[nh]-1;
      Ymin   = dp->begin[nh];
      Ymax   = dp->end[nh];
      Ystep  = dp->step[nh];
   }
   else
   {  np     = dp->nblk[nh]-1;
      N      = dp->lthx[nh];
      Nunit  = dp->nunx[nh]-1;
      Xmin   = dp->xstrt[nh];
      Xmax   = dp->xend[nh];
      Xstep  = dp->xbin[nh];
      Ny     = dp->lthy[nh];
      Nunity = dp->nuny[nh]-1;
      Ymin   = dp->ystrt[nh];
      Ymax   = dp->yend[nh];
      Ystep  = dp->ybin[nh];
      zcoord = (float)-9999.9;
      strcpy(zlabel,"&&&&");
   }
   if (N>DIM || Ny>DIM)
   {  DispObjectMsg("Dimension of 3D-plot to big! (>100 bins)");
      return;
   }
   scltyp = TugData.scltyp;
   CheckMenu(scltyp); 
   Nhist  = TugData.nh;
   View   = TugData.view;
   Nniv   = TugData.nniv;
   Cont = TRUE;

   Invalidate2();
}

VOID GrafWin :: LegoPlot(BOOL LossHist, int nh, TUGDATA TugData)
{
   statistics = FALSE;
   if (nh >= 1000)
   {
      nh -= 1000; 
      statistics = TRUE;
   }

   if (!LossHist)
   {  
      zcoord = dp->z[nh];
      strcpy(zlabel,dp->lab[nh]);
      for (int i=nh-1; i>=0; i--)
      {  if (dp->mtype[i] == 2)     // search for nearest previous x-coordinate
         {  np     = dp->nentry[i]-1;
            N      = dp->intvls[i];
            Nunit  = dp->ncoor[i]-1;
            Xmin   = dp->begin[i];
            Xmax   = dp->end[i];
            Xstep  = dp->step[i];
            break;
         }      
      }
      Ny     = dp->intvls[nh];
      Nunity = dp->ncoor[nh]-1;
      Ymin   = dp->begin[nh];
      Ymax   = dp->end[nh];
      Ystep  = dp->step[nh];
   }
   else
   {  np     = dp->nblk[nh]-1;
      N      = dp->lthx[nh];
      Nunit  = dp->nunx[nh]-1;
      Xmin   = dp->xstrt[nh];
      Xmax   = dp->xend[nh];
      Xstep  = dp->xbin[nh];
      Ny     = dp->lthy[nh];
      Nunity = dp->nuny[nh]-1;
      Ymin   = dp->ystrt[nh];
      Ymax   = dp->yend[nh];
      Ystep  = dp->ybin[nh];
      zcoord = (float)-9999.9;
      strcpy(zlabel,"&&&&");
   }
   if (N>DIM || Ny>DIM)
   {  DispObjectMsg("Dimension of 3D-plot to big! (>100 bins)");
      return;
   }
   scltyp = TugData.scltyp;
   CheckMenu(scltyp); 
   Nhist  = TugData.nh;
   View   = TugData.view;
   Phi    = TugData.phi;
   Tet    = TugData.tet;
   PhiL   = TugData.phil;
   TetL   = TugData.tetl;
   Lego   = TRUE;

   Invalidate2();
}

VOID GrafWin :: GridPlot(BOOL LossHist, int nh, TUGDATA TugData)
{
   statistics = FALSE;
   if (nh >= 1000)
   {
      nh -= 1000; 
      statistics = TRUE;
   }

   if (!LossHist)
   {
      zcoord = dp->z[nh];
      strcpy(zlabel,dp->lab[nh]);
      for (int i=nh-1; i>=0; i--)
      {  if (dp->mtype[i] == 2)     // search for nearest previous x-coordinate
         {  np     = dp->nentry[i]-1;
            N      = dp->intvls[i];
            Nunit  = dp->ncoor[i]-1;
            Xmin   = dp->begin[i];
            Xmax   = dp->end[i];
            Xstep  = dp->step[i];
            break;
         }      
      }
      Ny     = dp->intvls[nh];
      Nunity = dp->ncoor[nh]-1;
      Ymin   = dp->begin[nh];
      Ymax   = dp->end[nh];
      Ystep  = dp->step[nh];
   }
   else
   {  np     = dp->nblk[nh]-1;
      N      = dp->lthx[nh];
      Nunit  = dp->nunx[nh]-1;
      Xmin   = dp->xstrt[nh];
      Xmax   = dp->xend[nh];
      Xstep  = dp->xbin[nh];
      Ny     = dp->lthy[nh];
      Nunity = dp->nuny[nh]-1;
      Ymin   = dp->ystrt[nh];
      Ymax   = dp->yend[nh];
      Ystep  = dp->ybin[nh];
      zcoord = (float)-9999.9;
      strcpy(zlabel,"&&&&");
   }
   if (N>DIM || Ny>DIM)
   {  DispObjectMsg("Dimension of 3D-plot to big! (>100 bins)");
      return;
   }
   scltyp = TugData.scltyp;
   CheckMenu(scltyp); 
   Nhist  = TugData.nh;
   View   = TugData.view;
   Phi    = TugData.phi;
   Tet    = TugData.tet;
   PhiL   = TugData.phil;
   TetL   = TugData.tetl;
   Grid   = TRUE;

   Invalidate2();
}

void GrafWin :: DrawHistTitle(char *type, int x, int y, int Nhist, SCLTYP scltyp,
                              float zcoord, char *zlabel)
{
   char Text[80], Num[5], zstr[20];

   strcpy(Text,type);
   strcat(Text," No ");
   int numb = Nhist;
   if (Nhist > 1000)
      numb = Nhist - 1000;
   sprintf(Num,"%d",numb);
   strcat(Text,Num);
   if (strcmp(zlabel,"&&&&") != 0)
   {
      x = (int)((float)0.9 * x);
      if (scltyp == LIN)
         strcat(Text," (lin) at z=");
      else
         strcat(Text," (log) at z=");
      sprintf(zstr,"%7.3f ",zcoord);
      strcat(Text,zstr);
      strcat(Text,dp->name[7]);  // m
      if (strcmp(zlabel,"$$$$") != 0)
      {
         x = (int)((float)0.93 * x);
         strcat(Text," (");
         strcat(Text,zlabel);
         strcat(Text,")");
      }
   }
   else
   {
      if (scltyp == LIN)
         strcat(Text," (lin)");
      else
         strcat(Text," (log)");
   }
   opDev->PosStr(x,y,Text);
}

VOID GrafWin :: DrawHist(VOID)
{
   Brush *opBrush, *opOld;
   POINT Poly[500];
   float fact, y[100];
   int   i;
   char Text[80];

   hu     = (float)h_user;
   wu     = (float)w_user;
   if (toprint)
	hu = wu / (float) 1.43 ;  // only landscape aspect ratio
   wh     = wu * float(0.6);
   w0     = wu * float(0.2);
   hh     = hu * float(0.6);
   h0     = hu * float(0.8);


   if (N>DIM)
   {  DispObjectMsg("Dimension of 1D-plot to big! (>100 bins)");
      return;
   }

// extract and scale histogram

   float ymax = (float)0.0;
   for (i=0; i<N; i++)
   {  y[i] = dp->mtable[np+i];
      if (scltyp == LOG)
      {  y[i] += float(1.0);
         y[i] = (float)log10(y[i]);
      }
      if (y[i] > ymax) ymax = y[i];
      ix[i] = (int)((float)i * wh / (float)(N));
   }
   ix[N] = (int)((float)N * wh / (float)(N));
   if (ymax < 0.000001) ymax = (float)1.0;

   ymax *= float(1.05);
   float magn = (float)power(10.0,double(int(log10(ymax))));
   float q = ymax / magn;
   if (q < 1.0)
      q = (float)1.0;
   else if (q < 1.2)
      q = (float)1.2;
   else if (q < 1.5)
      q = (float)1.5;
   else if (q < 2.0)
      q = (float)2.0;
   else if (q < 2.5)
      q = (float)2.5;
   else
      q = (float)ceil(q);

   ymax = q * magn;

   for (i=0; i<N; i++)
      iy[i] = (int)(y[i] * hh / ymax);

// draw frame

   Poly[0].x = (int)w0;
   Poly[0].y = (int)h0;
   Poly[1].x = (int)w0 + (int)wh;
   Poly[1].y = (int)h0;
   Poly[2].x = (int)w0 + (int)wh;
   Poly[2].y = (int)h0 - (int)hh-1;
   Poly[3].x = (int)w0;
   Poly[3].y = (int)h0 - (int)hh-1;
   Poly[4].x = (int)w0;
   Poly[4].y = (int)h0;
   if (color)
      opBrush = new Brush(RGB(Col->HistBack.r,Col->HistBack.g,Col->HistBack.b));
   else
      opBrush = new Brush(RGB(255,255,255));
   opOld = opDev->SetBrush(opBrush);

   if (color)
       opOldPen = opDev->SetPen(opPen3);
   opDev->PutPolygon(Poly,5);
   opDev->SetBrush(opOld);
   delete opBrush;

// draw histogram

   int j = 0;
   Poly[j].x = (int)w0 + ix[0];
   Poly[j].y = (int)h0 - iy[0];
   j++;
   int iyold = iy[0];

   for (i=1; i<N; i++)
   {
      Poly[j].x = (int)w0 + ix[i]; 
      Poly[j].y = (int)h0 - iyold; 
      j++;
      Poly[j].x = (int)w0 + ix[i]; 
      Poly[j].y = (int)h0 - iy[i]; 
      j++;
      iyold = iy[i];
   }
   Poly[j].x = (int)w0 + ix[N]; 
   Poly[j].y = (int)h0 - iyold; 
   j++;
   Poly[j].x = (int)w0 + ix[N]; 
   Poly[j].y = (int)h0; 
   j++;
   Poly[j].x = (int)w0; 
   Poly[j].y = (int)h0; 
   j++;
   Poly[j].x = (int)w0 + ix[0]; 
   Poly[j].y = (int)h0 - iy[0]; 

   if (color)
      opBrush = new Brush(RGB(Col->HistBody.r,Col->HistBody.g,Col->HistBody.b));
   else
      opBrush = new Brush(RGB(162,162,162));
   opOld = opDev->SetBrush(opBrush);
   opDev->PutPolygon(Poly,j);
   opDev->SetBrush(opOld);
   delete opBrush;

// Draw title and x- and y-captions

   opOldFont = opDev->SetFont(opFont1);

   DrawHistTitle("Histogram", int(wu*0.38), int(hu*0.14), Nhist, scltyp, zcoord, zlabel);

   Caption(Text,Nunit,fact);
   opDev->PosStr(int(wu*0.45),int(hu*0.87),Text);

   opDev->SetFont(opFont2);
   opDev->PosStr(int(wu*0.08),int(hu*0.55),"Counts");
   opDev->SetFont(opFont1);

// draw statictical info

   if ( (Xsum > 0.0) && statistics)
   {
	float xm = Xmom / (Xsum * fact);
	float argum = Xmom2 / (Xsum * fact * fact) - xm * xm;
	if (argum < 0.0) argum = 0.0;
	float xm2 = (float)sqrt(argum);

	int rv = Mean(&dp->mtable[np-1], Xmin/fact, N, Xstep/fact, &Xsum, &xm, &xm2);

	char temp[100];
	sprintf(temp,"Mean =%8.3f",xm);
	opDev->PosStr(int(wu*0.82),int(hu*0.2),temp);
	sprintf(temp,"RMS =%8.3f",xm2);
	opDev->PosStr(int(wu*0.82),int(hu*0.23),temp);
	sprintf(temp,"Sum =%8.0f",Xsum);
	opDev->PosStr(int(wu*0.82),int(hu*0.26),temp);
   }
   else if ( (Xsum == (float)-999.0) && statistics)
   {
        float sum, xm, xm2;
	int rv = Mean(&dp->mtable[np-1], Xmin/fact, N, Xstep/fact, &sum, &xm, &xm2);
        if (rv == 0)
	{
	  char temp[100];
	  Xsum = sum;
	  sprintf(temp,"Mean =%8.3f",xm);
	  opDev->PosStr(int(wu*0.82),int(hu*0.2),temp);
	  sprintf(temp,"RMS =%8.3f",xm2);
	  opDev->PosStr(int(wu*0.82),int(hu*0.23),temp);
	  sprintf(temp,"Sum =%8.0f",Xsum);
	  opDev->PosStr(int(wu*0.82),int(hu*0.26),temp);
	}
   }

// Draw Axis

   Axis *axis = new Axis(opDev,H_CHAR);
   float XTickMin = wh / float(40.0);
   axis->HorzAxis(w0,h0,wh,
                  hh/float(120.),hh/float(90.),hh/float(60.),hh/float(40.),
                  Xmin/fact,Xmax/fact,
                  XTickMin,FALSE);
   Ymin = (scltyp == LIN) ? float(0.0) : float(1.0);
   Ymax = (scltyp == LIN) ? ymax : (float)power(10,ymax);
   float YTickMin = hh / float(40.0);
   axis->VertAxis(w0,h0,hh,
                  -hh/float(120.),-hh/float(90.),-hh/float(60.),-hh/float(40.),
                  Ymin,Ymax,
                  YTickMin,(scltyp == LIN) ? FALSE : TRUE);
   axis->VertAxis(w0+wh,h0,hh,
                  hh/float(120.),hh/float(90.),hh/float(60.),float(0.),
                  Ymin,Ymax,
                  YTickMin,(scltyp == LIN) ? FALSE : TRUE);
   opDev->SetFont(opOldFont);
   if (color)
      opDev->SetPen(opOldPen);
   delete axis;
}

VOID GrafWin :: DrawLego(VOID)
{
   float fact;
   int i, j, l, k;

   float (*field)[DIM] = new float[DIM][DIM];  // global heap, not stack
   int  (*ifield)[DIM] = new int[DIM][DIM];

   hu     = (float)h_user;
   wu     = (float)w_user;
   if (toprint)
	hu = wu / (float) 1.43 ;  // only landscape aspect ratio

// extract and scale histogram
   
   float zmax = (float)0.0;
   int mm = 0;
   for (i=0; i<Ny; i++)
   {  for (j=0; j<N; j++)
      {  if (View == FRONT)
         {  l = i;
            k = j;
         }
         else
         {  l = Ny - i - 1;
            k = N  - j - 1;
         }
         field[l][k] = dp->mtable[np+mm];
         if (scltyp == LOG) 
            field[l][k] = (float)log10(1. + field[l][k]);
         if (field[l][k] > zmax)
            zmax = field[l][k];
         mm++;
      }
      mm++;
   }
   if (zmax < float(0.000001)) zmax = float(1.0);

   float zscal = hu / zmax;
   for (i=0; i<Ny; i++)
   {  for (j=0; j<N; j++)
        ifield[i][j] = (int)(field[i][j] * zscal);
   }

   float xscal = hu / float(N);
   for (i=0; i<=N; i++)
     ix[i] = int(float(i) * xscal);

   float yscal = hu / float(Ny);
   for (i=0; i<=Ny; i++)
     iy[i] = int(float(i) * yscal);

   int dim = int(hu) ;

   Graf3D *Lego = new Graf3D(color,opDev,0,int(0.9*hu),int(wu),int(0.8*hu),
                             Col->LegoBody.r,
                             Col->LegoBody.g,
                             Col->LegoBody.b);
   if (color)
       opOldPen = opDev->SetPen(opPen4);
   Lego->SetLight(PhiL,TetL);
   Lego->Graf3Di(0,dim,0,dim,0,dim,Phi,Tet);
   Lego->Graf3Dl(ix,iy,ifield,N+1,Ny+1);
   if (color)
       opDev->SetPen(opPen3);

   opOldFont = opDev->SetFont(opFont1);

   DrawHistTitle("Lego Plot", int(wu*0.41), int(hu*0.05), Nhist, scltyp, zcoord, zlabel);

   int a,b,c,u,v;
   char Xaxis[10];
   Caption(Xaxis,Nunit,fact,FALSE);
   a = int(0.5*dim);
   b = int(-0.1*dim);
   c = int(-0.02*dim);
   Lego->Graf3Dp(a,b,c,u,v);
   opDev->PosStr(u,v,Xaxis);
   float factx = fact;

   char Yaxis[10];
   Caption(Yaxis,Nunity,fact,FALSE);
   a = int(1.05*dim);
   b = int(0.5*dim);
   c = int(-0.02*dim);
   Lego->Graf3Dp(a,b,c,u,v);
   opDev->PosStr(u,v,Yaxis);
   float facty = fact;

// draw statictical info

   if (statistics)
   {
     float sum, xm, xxm, ym, yym, r12, eps;
     int ier = Ellips(&dp->mtable[np-1], Xmin/factx, N, Xstep/factx, Ymin/facty, Ny, Ystep/facty,
		         &sum, &xm, &xxm, &ym, &yym, &r12, &eps);
     if ( (ier == 0.0) && (sum > (float)0.0) )
     {
	char temp[100];
	sprintf(temp,"Mean1 =%8.3f",xm);
	opDev->PosStr(int(wu*0.82),int(hu*0.20),temp);
	sprintf(temp,"RMS1 =%8.3f",xxm);
	opDev->PosStr(int(wu*0.82),int(hu*0.23),temp);
	sprintf(temp,"Mean2 =%8.3f",ym);
	opDev->PosStr(int(wu*0.82),int(hu*0.26),temp);
	sprintf(temp,"RMS2 =%8.3f",yym);
	opDev->PosStr(int(wu*0.82),int(hu*0.29),temp);
	sprintf(temp,"Corr =%8.3f",r12);
	opDev->PosStr(int(wu*0.82),int(hu*0.32),temp);
	sprintf(temp,"Area/pi =%8.3f",eps);
	opDev->PosStr(int(wu*0.82),int(hu*0.35),temp);
	sprintf(temp,"Sum =%8.0f",sum);
	opDev->PosStr(int(wu*0.82),int(hu*0.38),temp);
     }
   }

   Lego->Graf3Dp(0,0,0,u1,v1);
   Lego->Graf3Dp(dim,0,0,u2,v2);
   Lego->Graf3Dp(dim,dim,0,u3,v3);
   Lego->Graf3Dp(0,dim,0,u4,v4);

   delete Lego;
   delete[] field;
   delete[] ifield;

   if (color)
       opDev->SetPen(opOldPen);
   opDev->SetFont(opOldFont);
}

VOID GrafWin :: DrawGrid(VOID)
{
   float fact;
   int i, j, l, k;

   float (*field)[DIM] = new float[DIM][DIM];  // global heap, not stack
   int  (*ifield)[DIM] = new int[DIM][DIM];

   hu     = (float)h_user;
   wu     = (float)w_user;
   if (toprint)
	hu = wu / (float) 1.43 ;  // only landscape aspect ratio

// extract and scale histogram

   float zmax = (float)0.0;
   int mm = 0;
   for (i=0; i<Ny; i++)
   {  for (j=0; j<N; j++)
      {  if (View == FRONT)
         {  l = i;
            k = j;
         }
         else
         {  l = Ny - i - 1;
            k = N  - j - 1;
         }
         field[l][k] = dp->mtable[np+mm];
         if (scltyp == LOG) 
            field[l][k] = (float)log10(1. + field[l][k]);
         if (field[l][k] > zmax)
            zmax = field[l][k];
         mm++;
      }
      mm++;
   }
   if (zmax < float(0.000001)) zmax = float(1.0);

   float zscal = hu / zmax;
   for (i=0; i<Ny; i++)
   {  for (j=0; j<N; j++)
        ifield[i][j] = (int)(field[i][j] * zscal);
   }

   float xscal = hu / float(N - 1);
   for (i=0; i<N; i++)
     ix[i] = int(float(i) * xscal);

   float yscal = hu / float(Ny- 1);
   for (i=0; i<Ny; i++)
     iy[i] = int(float(i) * yscal);
   
   int dim = int(hu) ;

   Graf3D *Grid = new Graf3D(color,opDev,0,int(0.9*hu),int(wu),int(0.8*hu),
                             Col->LegoBody.r,
                             Col->LegoBody.g,
                             Col->LegoBody.b);
   if (color)
       opOldPen = opDev->SetPen(opPen4);
   Grid->SetLight(PhiL,TetL);
   Grid->Graf3Di(0,dim,0,dim,0,dim,Phi,Tet);
   Grid->Graf3Dg(ix,iy,ifield,N,Ny);
   if (color)
       opDev->SetPen(opPen3);

   opOldFont = opDev->SetFont(opFont1);

   DrawHistTitle("Grid Plot", int(wu*0.41), int(hu*0.05), Nhist, scltyp, zcoord, zlabel);

   int a,b,c,u,v;
   char Xaxis[10];
   Caption(Xaxis,Nunit,fact,FALSE);
   a = int(0.5*dim);
   b = int(-0.1*dim);
   c = int(-0.02*dim);
   Grid->Graf3Dp(a,b,c,u,v);
   opDev->PosStr(u,v,Xaxis);
   float factx = fact;

   char Yaxis[10];
   Caption(Yaxis,Nunity,fact,FALSE);
   a = int(1.05*dim);
   b = int(0.5*dim);
   c = int(-0.02*dim);
   Grid->Graf3Dp(a,b,c,u,v);
   opDev->PosStr(u,v,Yaxis);
   float facty = fact;

// draw statictical info

   if (statistics)
   {
     float sum, xm, xxm, ym, yym, r12, eps;
     int ier = Ellips(&dp->mtable[np-1], Xmin/factx, N, Xstep/factx, Ymin/facty, Ny, Ystep/facty,
		         &sum, &xm, &xxm, &ym, &yym, &r12, &eps);
     if ( (ier == 0.0) && (sum > (float)0.0) )
     {
	char temp[100];
	sprintf(temp,"Mean1 =%8.3f",xm);
	opDev->PosStr(int(wu*0.82),int(hu*0.20),temp);
	sprintf(temp,"RMS1 =%8.3f",xxm);
	opDev->PosStr(int(wu*0.82),int(hu*0.23),temp);
	sprintf(temp,"Mean2 =%8.3f",ym);
	opDev->PosStr(int(wu*0.82),int(hu*0.26),temp);
	sprintf(temp,"RMS2 =%8.3f",yym);
	opDev->PosStr(int(wu*0.82),int(hu*0.29),temp);
	sprintf(temp,"Corr =%8.3f",r12);
	opDev->PosStr(int(wu*0.82),int(hu*0.32),temp);
	sprintf(temp,"Area/pi =%8.3f",eps);
	opDev->PosStr(int(wu*0.82),int(hu*0.35),temp);
	sprintf(temp,"Sum =%8.0f",sum);
	opDev->PosStr(int(wu*0.82),int(hu*0.38),temp);
     }
   }

   Grid->Graf3Dp(0,0,0,u1,v1);
   Grid->Graf3Dp(dim,0,0,u2,v2);
   Grid->Graf3Dp(dim,dim,0,u3,v3);
   Grid->Graf3Dp(0,dim,0,u4,v4);

   delete Grid;
   delete[] field;
   delete[] ifield;

   if (color)
       opDev->SetPen(opOldPen);
   opDev->SetFont(opOldFont);
}

VOID GrafWin :: DrawCont(VOID)
{
   Brush *opBrush, *opOld;
   POINT Poly[5];
   float factx, facty;
   int ix[100], iy[100];
   int i, j;
   char Text[80];

   float (*field)[DIM] = new float[DIM][DIM];  // global heap, not stack
   int  (*ifield)[DIM] = new int[DIM][DIM];

   hu     = (float)h_user;
   wu     = (float)w_user;
   if (toprint)
	hu = wu / (float) 1.43 ;  // only landscape aspect ratio
   hh     = hu * float(0.8);
   wh     = hh;
   h0     = hu * float(0.86);
   w0     = (wu - hh) / float(2.);

// draw frame

   Poly[0].x = (int)w0;
   Poly[0].y = (int)h0;
   Poly[1].x = (int)w0 + (int)wh + 1;
   Poly[1].y = (int)h0;
   Poly[2].x = (int)w0 + (int)wh + 1;
   Poly[2].y = (int)h0 - (int)hh;
   Poly[3].x = (int)w0;
   Poly[3].y = (int)h0 - (int)hh;
   Poly[4].x = (int)w0;
   Poly[4].y = (int)h0;

   if (color)
      opOldPen = opDev->SetPen(opPen3);
   if (color)
      opBrush = new Brush(RGB(Col->ContBack.r,Col->ContBack.g,Col->ContBack.b));
   else
      opBrush = new Brush(RGB(255,255,255));
   opOld = opDev->SetBrush(opBrush);
   opDev->PutPolygon(Poly,5);
   opDev->SetBrush(opOld);
   delete opBrush;

   opOldFont = opDev->SetFont(opFont1);

   DrawHistTitle("Contour Plot", int(wu*0.38), int(hu*0.01), Nhist, scltyp, zcoord, zlabel);

   Caption(Text,Nunit,factx);
   opDev->PosStr(int(wu*0.45),int(hu*0.92),Text);

   opDev->SetFont(opFont2);
   Caption(Text,Nunity,facty);
   opDev->PosStr(int(wu*0.08),int(hu*0.55),Text);
   opDev->SetFont(opFont1);

// draw statictical info

   if (statistics)
   {
     float sum, xm, xxm, ym, yym, r12, eps;
     int ier = Ellips(&dp->mtable[np-1], Xmin/factx, N, Xstep/factx, Ymin/facty, Ny, Ystep/facty,
		         &sum, &xm, &xxm, &ym, &yym, &r12, &eps);
     if ( (ier == 0.0) && (sum > (float)0.0) )
     {
	char temp[100];
	sprintf(temp,"Xmean =%8.3f",xm);
	opDev->PosStr(int(wu*0.82),int(hu*0.20),temp);
	sprintf(temp,"Xrms =%8.3f",xxm);
	opDev->PosStr(int(wu*0.82),int(hu*0.23),temp);
	sprintf(temp,"Ymean =%8.3f",ym);
	opDev->PosStr(int(wu*0.82),int(hu*0.26),temp);
	sprintf(temp,"Yrms =%8.3f",yym);
	opDev->PosStr(int(wu*0.82),int(hu*0.29),temp);
	sprintf(temp,"Corr =%8.3f",r12);
	opDev->PosStr(int(wu*0.82),int(hu*0.32),temp);
	sprintf(temp,"Area/pi =%8.3f",eps);
	opDev->PosStr(int(wu*0.82),int(hu*0.35),temp);
	sprintf(temp,"Sum =%8.0f",sum);
	opDev->PosStr(int(wu*0.82),int(hu*0.38),temp);
     }
   }

// Draw Axis

   Axis *axis = new Axis(opDev,H_CHAR);
   float XTickMin = wh / float(40.0);
   axis->HorzAxis(w0,h0,wh,
                  hh/float(120.),hh/float(90.),hh/float(60.),hh/float(40.),
                  Xmin/factx,Xmax/factx,XTickMin,FALSE);
   float YTickMin = hh / float(40.0);
   axis->VertAxis(w0,h0,hh,
                  -hh/float(120.),-hh/float(90.),-hh/float(60.),-hh/float(40.),
                  Ymin/facty,Ymax/facty,YTickMin,FALSE);
   axis->VertAxis(w0+wh,h0,hh,
                  hh/float(120.),hh/float(90.),hh/float(60.),float(0.),
                  Ymin/facty,Ymax/facty,YTickMin,FALSE);
   delete axis;

// extract and scale histogram

   float zmax = float(0.);
   int mm = 0;
   for (i=0; i<Ny; i++)
   {  for (j=0; j<N; j++)
      {  field[j][i] = dp->mtable[np+mm];
         if (scltyp == LOG) 
            field[j][i] = (float)log10(1. + field[j][i]);
         if (field[j][i] > zmax)
            zmax = field[j][i];
         mm++;
      }
      mm++;
   }
   if (zmax < float(0.000001)) zmax = float(1.0);

   float zscal = float(1000.) / zmax;
   for (i=0; i<Ny; i++)
   {  for (j=0; j<N; j++)
        ifield[j][i] = (int)(field[j][i] * zscal);
   }

   float delt = float(1000.) / float(Nniv-1);
   int ifniv[100];
   ifniv[0] = 0;
   for (i=1; i<Nniv; i++)
     ifniv[i] = ifniv[0] + int(float(i) * delt);

// improve resolution by multiplying x and y with factor nd

   int nd = 5;
   float ddx = float(nd) * wh / float(N-1);
   float ddy = float(nd) * hh / float(Ny-1);
   ix[0] = int(w0 * float(nd));
   for (i=1; i<N; i++)
     ix[i] = ix[0] + int(float(i) * ddx);
   iy[0] = int(h0 * float(nd));
   for (i=1; i<Ny; i++)
     iy[i] = iy[0] - int(float(i) * ddy);

   Contour(ix,N,iy,Ny,ifield,Nniv,ifniv,nd);

   delete[] field;
   delete[] ifield;

   if (color)
       opDev->SetPen(opOldPen);
   opDev->SetFont(opOldFont);
}

#define GraMove(a,b)  opDev->MoveTo(a,b)
#define GraDraw(a,b)  opDev->LineTo(a,b)


VOID GrafWin :: Contour(int *x, int Nox, int *y, int Noy, int z[DIM][DIM],
                        int Noc, int *zplan, int nd)
{
//  Translated from Fortran to C by U. Rohrer, Oct. 1992. From MENT program.

    int zt1, zt2, zt3, zt4;

    for (int ny=1; ny<Noy; ny++)
    {  int ix = (ny-2*(ny/2) == 0) ? 0 : 1;
       int dy = y[ny] - y[ny-1];
       for (int inx=1; inx<Nox; inx++)
       {  int nx = inx;
          if (ix) nx = Nox - inx;
          zt1 = z[nx-1][ny-1];
          zt2 = z[nx][ny-1];
          zt3 = z[nx][ny];
          zt4 = z[nx-1][ny];
          int dx = x[nx] - x[nx-1];
          if (abs(zt3-zt1) >= abs(zt4-zt2))
          {  TriCont(x[nx],y[ny],-dx,-dy,Noc,zplan,zt4,zt3,zt2,nd);
             TriCont(x[nx-1],y[ny-1],dx,dy,Noc,zplan,zt2,zt1,zt4,nd);
          }
          else
          {  TriCont(x[nx-1],y[ny],dx,-dy,Noc,zplan,zt3,zt4,zt1,nd);
             TriCont(x[nx],y[ny-1],-dx,dy,Noc,zplan,zt1,zt2,zt3,nd);
          }
       }
    }
}

VOID GrafWin :: TriCont(int x, int y, int dx, int dy, int Noc, int *zplan,
                        int zx, int zv, int zy, int nd)
{
//  Translated from Fortran to C by U. Rohrer, Oct. 1992. From MENT program.

    int j, k;
    int zt[4], xp[100][2], yp[100][2];

    zt[0] = zx;
    zt[1] = zv;
    zt[2] = zy;
    zt[3] = zx;
    int ztmina = MIN(zt[0],zt[1]); int ztmin = MIN(ztmina,zt[2]);
    int ztmaxa = MAX(zt[0],zt[1]); int ztmax = MAX(ztmaxa,zt[2]);
    int imin = Noc + 1;
    int imax = 0;
    for (k=0; k<Noc; k++)
    {  j = Noc - k - 1;
       if (zplan[j] >= ztmin) imin = j;
       if (zplan[k] <= ztmax) imax = k;
    }
    int int1 = imax - imin;
    if (int1 < 0 || ztmin == ztmax) return;
    int i2 = 0;
    for (k=0; k<3; k++)
    {  ztmax = MAX(zt[k],zt[k+1]);
       int zpmin = MIN(zt[k],zt[k+1]);
       int min1 = Noc + 1;
       int max1 = 0;
       for (j=0; j<Noc; j++)
       {  int inz = Noc - j - 1;
          if (zplan[inz] > zpmin || (zplan[inz] == zpmin && ztmin == zpmin))
             min1 = inz;
          if (zplan[j] <= ztmax)
             max1 = j;
       }
       int inz = max1 - min1;
       if (inz >= 0 && ztmax != zpmin)
       {  if (inz == int1)
            i2 = (i2==0) ? 1 : 0;
          else
            i2 = 0;
          for (j=min1; j<=max1; j++)
          {  switch (k)
             {  case 0:
                  xp[j][i2] = int(x + float(dx) * float(zplan[j]-zt[1]) /
                                                  float(zt[0]-zt[1]));
                  yp[j][i2] = int(y);
                  break;
                case 1:
                  xp[j][i2] = int(x);
                  yp[j][i2] = int(y + float(dy) * float(zplan[j]-zt[1]) /
                                                  float(zt[2]-zt[1]));
                  break;
                case 2:
                  xp[j][i2] = int(x + float(dx) * float(zplan[j]-zt[2]) /
                                                  float(zt[0]-zt[2]));
                  yp[j][i2] = int(y + float(dy) * float(zplan[j]-zt[0]) /
                                                  float(zt[2]-zt[0]));
                  break;
             }
          }
       }
    }
    for (j=imin; j<=imax; j++)
    {  if (xp[j][0]!=x1old    || xp[j][1]!=x2old || 
           yp[j][0]!=y1old    || yp[j][1]!=y2old ||
           xp[j][0]!=xp[j][1] || yp[j][0]!=yp[j][1])
       {  if (color)
          {  if (j-2*(j/2))
                opDev->SetPen(opPen1);
             else
                opDev->SetPen(opPen2);
          }
          GraMove(xp[j][0]/nd,yp[j][0]/nd);
          GraDraw(xp[j][1]/nd,yp[j][1]/nd);
          GraMove(xp[j][1]/nd,yp[j][1]/nd);   // last point of line has to be
          GraDraw(xp[j][1]/nd,yp[j][1]/nd+1); // drawn separately. stupid!!!
       }
       x1old = xp[j][0];
       y1old = yp[j][0];
       x2old = xp[j][1];
       y2old = yp[j][1];
    }
}

VOID GrafWin :: Timds(VOID)
{
    // time and date

    char *p, WeekDay[10], Month[10], Day[10], Time[10], Year[10], Date[12];

#ifdef __WATCOMC__
    time_t t;
    t = time(NULL);
    Tstring = ctime(&t);
#else
    long secs;
    time(&secs);
    Tstring = ctime(&secs);
#endif
    hu     = (float)h_user;
    wu     = (float)w_user;
    if (toprint)
	hu = wu / (float) 1.43 ;  // only landscape aspect ratio

    if (strlen(Tstring) > 22) // because of bug in Symantec C++ Vers. 6
    {
       p = strtok(Tstring," ");
       if (p) strcpy(WeekDay,p);
       p = strtok(NULL," ");
       if (p) strcpy(Month,p);
       p = strtok(NULL," ");
       if (p) strcpy(Day,p);
       p = strtok(NULL," ");
       if (p) strcpy(Time,p);
       p = strtok(NULL," ");
       if (p) strcpy(Year,p);

       Year[0] = Year[2];
       Year[1] = Year[3];
       Year[2] = '\0';               // cut off centuries

       strcpy(Date,Day);
       strcat(Date,"-");
       strcat(Date,Month);
       strcat(Date,"-");
       strcat(Date,Year);

       opOldFont = opDev->SetFont(opFont1);
       opDev->PosStr(w_user-w_user/6,int(hu/(float)100.),Date);
       opDev->PosStr(w_user-w_user/6,int(hu/(float)20.),Time);
       opDev->SetFont(opOldFont);
    }
}

VOID GrafWin :: Caption(char *text, int n, float &fact, BOOL dim)
{
    if (n<12)
    {  strcpy(text,dp->nom[n]);
       if (dim)
       {  strcat(text," (");
          strcat(text,dp->name[n]);
          strcat(text,")");
       }
       fact = dp->unit[n];
    }
    else if (n == 12)
    {  strcpy(text,"beta");
       fact = (float)1.0;
    }
    else if (n == 13)
    {  strcpy(text,"gamma");
       fact = (float)1.0;
    }
    else if (n == 14)
    {  strcpy(text,"l.pol");
       fact = (float)1.0;
    }
    else
    {  int j = n - 20;
       strcpy(text,dp->nom1[j]);
       if (j > 1) j = 6;           // deg
       if (dim)
       {  strcat(text," (");
          strcat(text,dp->name[j]);
          strcat(text,")");
       }
       fact = dp->unit[j];
    }
}

VOID GrafWin :: CheckMenu(SCLTYP scltyp)
{
    if (scltyp == LIN)
    {
	CheckMenuItem(hSubMenu,IDC_LIN,MF_CHECKED);
	CheckMenuItem(hSubMenu,IDC_LOG,MF_UNCHECKED);
    }
    else
    {
	CheckMenuItem(hSubMenu,IDC_LIN,MF_UNCHECKED);
	CheckMenuItem(hSubMenu,IDC_LOG,MF_CHECKED);
    }
}
VOID GrafWin :: CheckMenu(BOOL Cont, BOOL Lego, BOOL Grid)
{
    if (Cont)
    {
	CheckMenuItem(hSubMenu,IDC_CONT,MF_CHECKED);
	CheckMenuItem(hSubMenu,IDC_LEGO,MF_UNCHECKED);
	CheckMenuItem(hSubMenu,IDC_GRID,MF_UNCHECKED);
    }
    else if (Lego)
    {
	CheckMenuItem(hSubMenu,IDC_LEGO,MF_CHECKED);
	CheckMenuItem(hSubMenu,IDC_CONT,MF_UNCHECKED);
	CheckMenuItem(hSubMenu,IDC_GRID,MF_UNCHECKED);
    }
    else if (Grid)
    {
	CheckMenuItem(hSubMenu,IDC_GRID,MF_CHECKED);
	CheckMenuItem(hSubMenu,IDC_CONT,MF_UNCHECKED);
	CheckMenuItem(hSubMenu,IDC_LEGO,MF_UNCHECKED);
    }
    else
    {
	CheckMenuItem(hSubMenu,IDC_GRID,MF_UNCHECKED);
	CheckMenuItem(hSubMenu,IDC_CONT,MF_UNCHECKED);
	CheckMenuItem(hSubMenu,IDC_LEGO,MF_UNCHECKED);
    }
}

int GrafWin :: Ellips(float *iplot,  float amin, int na, float da,
				     float dmin, int nd, float dd,
		         float *sum, float *xm, float *xxm,
			             float *ym, float *yym, float *r12, float *eps)
{
    int    i, j, k;
    double x, y, z;

    int mit = (na+1) * (nd+1) + 5;
    i = int(iplot[mit]-iplot[mit-1]-iplot[mit-2]-iplot[mit-3]-iplot[mit-4]);
    if (i < 3) return 1;

    *sum  = (float)0.0;
    double sumd  = 0.0;
    double sumx  = 0.0;
    double sumxx = 0.0;
    double sumy  = 0.0;
    double sumyy = 0.0;
    double sumxy = 0.0;

    for (j=1; j<=nd; j++)
    {
       for (i=1; i<=na; i++)
       {
	 k = (j-1) * (na + 1) + i;
	 x = amin + (float(i-1) + (float)0.5) * da;
         y = dmin + (float(j-1) + (float)0.5) * dd;
	 z = iplot[k];
	 sumd  += z;
	 sumx  += x*z;
	 sumxx += x*x*z;
	 sumy  += y*z;
	 sumyy += y*y*z;
	 sumxy += x*y*z;
       }
    }

    if (sumd <= 0.0) return 1;
    double xmd  = sumx / sumd;
    double ymd  = sumy / sumd;
    double xxmd = sumxx / sumd;
    double yymd = sumyy / sumd;
    double xymd = sumxy / sumd;
    if (xxmd - (xmd) * (xmd) <= 0.0) return 1;
    if (yymd - (ymd) * (ymd) <= 0.0) return 1;
    xxmd = sqrt(xxmd - (xmd) * (xmd));
    yymd = sqrt(yymd - (ymd) * (ymd));
    xymd = xymd - (xmd) * (ymd);
    double r12d = xymd / ((xxmd) * (yymd));
    if (r12d > 1.0)  r12d  = 1.0;
    if (r12d < -1.0) r12d = -1.0;
    double epsd = (xxmd) * (yymd) * sqrt(1.0 - r12d * r12d);     

    *sum = (float)sumd;
    *xm  = (float)xmd;
    *ym  = (float)ymd;
    *xxm = (float)xxmd;
    *yym = (float)yymd;
    *r12 = (float)r12d;
    *eps = (float)epsd;

    return 0;
}

int GrafWin :: Mean(float *iplot,  float amin, int na, float da,
		         float *sum, float *xm, float *xxm)
{
    int    i;
    double x, z;

    *sum        = (float)0.0;
    double sumd  = 0.0;
    double sumx  = 0.0;
    double sumxx = 0.0;

    for (i=1; i<=na; i++)
    {
	 x = amin + (float(i-1) + (float)0.5) * da;
	 z = iplot[i];
	 sumd  += z;
	 sumx  += x*z;
	 sumxx += x*x*z;
    }

    if (sumd <= 0.0)
    {
	*sum = (float)0.0;
	return 1;
    }
    double xmd  = sumx / sumd;
    double xxmd = sumxx / sumd;
    if (xxmd - xmd * xmd <= 0.0)
    {
	*xxm = (float)0.0;
	return 1;
    }
    xxmd = sqrt(xxmd - xmd * xmd);
    *sum = (float)sumd;
    *xm  = (float)xmd;
    *xxm = (float)xxmd;

    return 0;
}

///////////////////////////////// EOF /////////////////////////////////////////
