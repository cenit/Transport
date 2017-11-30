///////////////////////////////////////////////////////////////////////////////
////////////////////// Implementation of class Dialog /////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

#define MYWM_NOTIFYICON WM_USER+177

#ifdef WIN32
HBRUSH hBrCreateBrush(HDC hDC, HINSTANCE hInst, DWORD dwRGB)
{
/*
    Creates a colored pattern brush by coloring a
    black and white 8x8 bitmap (back.bmp). White pixels get the RGB value
    and black pixels get white after the application of the ROP2-value
    0xF50225 ( P|~D, P=Pattern, D=Destination).

    Result pattern:
    ........
    *.*.*.*.
    ........     * white
    *.*.*.*.
    ........     . RGB color
    *.*.*.*.
    ........
    *.*.*.*.
*/
    HDC     hdcMem;
    HBRUSH  hbr;
    HBRUSH  hbrOld;
    HBITMAP hbmPat;
    HBITMAP hbmOld;

    hbr    = CreateSolidBrush(dwRGB);
    hdcMem = CreateCompatibleDC(hDC);

    hbmPat = LoadBitmap(hInst,"MyBackInv");

    hbmOld = (HBITMAP)SelectObject(hdcMem, hbmPat);
    hbrOld = (HBRUSH)SelectObject(hdcMem, hbr);
    PatBlt(hdcMem, 0, 0, 8, 8, 0xF50225);

    SelectObject(hdcMem, hbmOld);
    SelectObject(hdcMem, hbrOld);

    DeleteDC(hdcMem);
    DeleteObject(hbr);

    hbr = CreatePatternBrush(hbmPat);
    DeleteObject(hbmPat);

    return hbr;
}
#endif

#ifdef WIN32
#ifndef WIN95
#define WINNT
#endif
#endif

// Dialog* Dialog::pDialogList_   = NULL;
DLGPROC Dialog::pHandler_      = NULL;
int     Dialog::nHandlerCount_ = 0;
HBITMAP Dialog::hBitmap        = NULL;
static  Dialog *pDialog        = NULL;

Dialog::Dialog( int ResId, HWND hParent)
      : Object( IDO_DIALOG )
      , pResourceName_( MAKEINTRESOURCE(ResId) )
      , hParent_( hParent)
      , bClosing_( FALSE )
      , hDlg_( 0 )
{
//// unseren Dialog in die Dialogliste einfuegen

//   pNextDialog_ = pDialogList_;
//   pPrevDialog_ = NULL;
//   pDialogList_ = this;

#ifdef WIN32

   if (nHandlerCount_++ == 0)
       pHandler_ = MakeProcInstance( (DLGPROC) ::DlgProc
                                  , Application::GetInstance());
   hBrush  = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
   HDC hdc = ::GetDC(this->GetHandle());
   HINSTANCE hInst = Application::GetInstance();
   hBrushB = hBrCreateBrush(hdc,hInst,GetSysColor(COLOR_BTNFACE));

#else

   if (nHandlerCount_++ == 0)
   {
      pHandler_ = MakeProcInstance( (DLGPROC) ::DlgProc
                                  , Application::GetInstance());
      hBitmap = LoadBitmap(Application::GetInstance(),"MyBack");
   }
   hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
   if ( hBitmap && (GetSysColor(COLOR_BTNFACE) == RGB(192,192,192)) )
      hBrushB = CreatePatternBrush(hBitmap);
   else
      hBrushB = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

#endif
}

Dialog::~Dialog()
{
#ifdef WIN32

   if (--nHandlerCount_ == 0)
      FreeProcInstance( pHandler_ );
#else

   if (--nHandlerCount_ == 0)
   {
      FreeProcInstance( pHandler_ );
      DeleteObject(hBitmap);
   }

#endif

/*
   if (pNextDialog_ != NULL)
      pNextDialog_->pPrevDialog_ = pPrevDialog_;

   if (pPrevDialog_ != NULL)
      pPrevDialog_->pNextDialog_ = pNextDialog_;

   if (pDialogList_ == this)
       pDialogList_ = pNextDialog_;
*/
   DeleteObject(hBrush);
   DeleteObject(hBrushB);
}

long Dialog::Handler(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   switch( wMsg )
   {
     case WM_INITDIALOG:
     {
#ifdef WINNT
        Ctl3dSubclassDlg(hDlg,CTL3D_ALL);
#endif
        pDialog->SetHandle(hDlg);
        return pDialog->Init();
     }

#ifndef WIN95
     case WM_SYSCOLORCHANGE:
     {
        Ctl3dColorChange();
        return TRUE;
     }
#endif

     case WM_COMMAND:
     {
#ifdef WIN32
        return pDialog->Control( LOWORD(wParam), HIWORD( wParam ) ); 
#else
        return pDialog->Control( wParam, HIWORD( lParam ) );
#endif
     }

#ifdef WIN32
     case WM_CTLCOLORSTATIC:
        return WmCtlColorStatic((HDC)wParam,(HWND)lParam);
     case WM_CTLCOLOREDIT:
     case WM_CTLCOLORBTN:
        return WmCtlColor((HDC)wParam);
     case WM_CTLCOLORDLG:
        return WmCtlColor2((HDC)wParam);
#else
     case WM_CTLCOLOR:
       if ( HIWORD(lParam) == CTLCOLOR_STATIC )
          return WmCtlColorStatic((HDC)wParam, (HWND) LOWORD(lParam));
       else if (
           HIWORD(lParam) == CTLCOLOR_EDIT   ||
           HIWORD(lParam) == CTLCOLOR_BTN
          )
          return WmCtlColor((HDC)wParam);
       else if ( HIWORD(lParam) == CTLCOLOR_DLG )
          return WmCtlColor2((HDC)wParam);
       else
          return DefWindowProc( hDlg, wMsg, wParam, lParam );
#endif

     case WM_VSCROLL:
     case WM_HSCROLL:
     {
#ifdef WIN32
        pDialog->Adjust( (HWND)lParam, LOWORD(wParam), HIWORD( wParam ) );
        UINT idControl = GetWindowLong( (HWND)lParam, GWL_ID );
        return pDialog->Control( idControl, HIWORD( wParam ) ); 
#else
        pDialog->Adjust( (HWND)HIWORD(lParam), wParam, LOWORD( lParam ) );
        UINT idControl = GetWindowWord( (HWND)HIWORD(lParam), GWW_ID);
        return pDialog->Control( idControl, HIWORD( lParam ) );
#endif
     }

     case WMP_VSCROLL:
     {
#ifdef WIN32
        pDialog->AdjustP( (HWND)lParam, LOWORD(wParam), HIWORD( wParam ) );
        UINT idControl = GetWindowLong( (HWND)lParam, GWL_ID );
        return pDialog->Control( idControl, HIWORD( wParam ) ); 
#else
        pDialog->AdjustP( (HWND)HIWORD(lParam), wParam, LOWORD( lParam ) );
        UINT idControl = GetWindowWord( (HWND)HIWORD(lParam), GWW_ID);
        return pDialog->Control( idControl, HIWORD( lParam ) );
#endif
     }

     case WM_PAINT:
     case WM_NCPAINT:
     {
        PAINTSTRUCT ps;
        BeginPaint(hDlg,&ps);
        long retval = pDialog->WmPaint(hDlg,ps);
        EndPaint(hDlg,&ps);
        return retval;
     }

     case WM_SYSCOMMAND:
     {
        return WmSysMenuCommand(LOWORD(wParam));
     }

     case WM_LBUTTONDOWN:
     {
	POINT Where;
	Where.x = LOWORD (lParam) ;
	Where.y = HIWORD (lParam) ;
	return WmLeftMouseDown( Where );
     }

     case WM_RBUTTONDOWN:
     {
	POINT Where;
	Where.x = LOWORD (lParam) ;
	Where.y = HIWORD (lParam) ;
	return WmRightMouseDown( Where );
     }

#ifdef WIN95

     case WM_CONTEXTMENU:
     {
        return WmContextMenu(wParam, lParam);
     }

     case WM_NOTIFY:
     {
        return WmNotify(lParam);
     }

     case WM_SIZE:
     {
        return WmSize(wParam, lParam);
     }

     case MYWM_NOTIFYICON:
     {
        return WmNotifyIcon(wParam, lParam);
     }

#endif

     case WM_MEASUREITEM:
     {
	return WmMeasureItem(wParam, lParam);
     }

     case WM_DRAWITEM:
     {
        LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
        if ( lpdis->CtlType == ODT_BUTTON )
           return DrawOwnerButton(lpdis);
        else
	   return WmDrawItem(wParam, lParam);
     }

     case WM_TIMER:
     {
        return WmTimer(wParam);
     }

     case WM_CLOSE:
     {
        return pDialog->Close();
     }

     case WM_NCDESTROY:
     {
        return pDialog->Free();
     }

     case WM_ACTIVATE:
     {
        return WmActivate(wParam,hDlg);
     }

     default:
     {
        return FALSE;
     }
   }
}

/*
Dialog* Dialog::FindDialog( HWND hDlg )
{
   if (this == NULL)
   {
      return NULL;
   }
   else if (hDlg_ == hDlg)
   {
      return this;
   }
   else if (pNextDialog_ == NULL)
   {
      return pNextDialog_;
   }
   else
   {
      return pNextDialog_->FindDialog( hDlg );
   }
}
*/

void Dialog::Start()
{
}

BOOL Dialog::Init()
{
   return FALSE;
}

#ifdef __BORLANDC__
#pragma argsused
#endif

long Dialog::Control( UINT nControlId, UINT nNotificationCode )
{
   return FALSE; 
}

long Dialog::WmCtlColor(HDC hdc)
{
    SetBkColor(hdc,GetSysColor(COLOR_BTNFACE));
    SetTextColor(hdc,GetSysColor(COLOR_BTNTEXT));
    return ((DWORD)hBrush);
}

long Dialog::WmCtlColor2(HDC hdc)
{
    SetBkColor(hdc,GetSysColor(COLOR_BTNFACE));
    SetTextColor(hdc,GetSysColor(COLOR_BTNTEXT));
    return ((DWORD)hBrushB);
}

long Dialog::WmCtlColorStatic(HDC hdc, HWND Child)
{
    SetBkColor(hdc,GetSysColor(COLOR_BTNFACE));
    SetTextColor(hdc,GetSysColor(COLOR_BTNTEXT));
    return ((DWORD)hBrush);
}

long Dialog::DrawOwnerButton( LPDRAWITEMSTRUCT lpdis )
{
//  handler for BS_OWNERDRAW

    if ( lpdis->CtlType == ODT_BUTTON )
    {
       DrawBitmap(hBrushB,lpdis);  // from USERBTN.DLL
    }
    return TRUE;
}

long Dialog::WmMeasureItem(WPARAM, LPARAM)
{
    return FALSE;
}

long Dialog::WmDrawItem(WPARAM, LPARAM)
{
    return FALSE;
}

long Dialog::WmSysMenuCommand(WPARAM)
{
    return FALSE;
}

LONG Dialog::WmLeftMouseDown(POINT &)
{
   return FALSE;
}

LONG Dialog::WmRightMouseDown(POINT &)
{
   return FALSE;
}

long Dialog::WmContextMenu(WPARAM, LPARAM)
{
    return FALSE;
}

long Dialog::WmNotify(LPARAM)
{
    return FALSE;
}

long Dialog::WmSize(WPARAM, LPARAM)
{
    return FALSE;
}

long Dialog::WmNotifyIcon(WPARAM, LPARAM)
{
    return FALSE;
}

#define HMIN 3

long Dialog::WmPaint(HWND hDlg, PAINTSTRUCT &ps)
{
//  This paint routine draws a series of shades in dialogboxes
//  Controls have to have IDs of 1000, 1001, 1002 ....

    if (IsIconic(hDlg))
    {
        Rectangle(ps.hdc,ps.rcPaint.left-1, ps.rcPaint.top-1,
                         ps.rcPaint.right+1,ps.rcPaint.bottom+1);
        DrawIcon(ps.hdc,2,2,hIcon);
        return FALSE;
    }

    if (GetDlgItem(hDlg,1000) == 0)  // not there --> nothing to do here!!
       return FALSE;

    HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
    HPEN hPen1 = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));
    HPEN hPen2 = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNHIGHLIGHT));

    HWND hShade;     // handle of Control with ID>=1000:
    UINT ID = 1000;  // Control "",1000,"static",SS_SIMPLE |...,x,y,w,h

    while ( (hShade = GetDlgItem(hDlg,ID)) != 0 )
    {
          RECT   rcDlg, rcShade, cliprgn;
          HBRUSH hOldBrush;
          HPEN   hOldPen;
#ifdef WIN32
          POINT  point;
#endif

          GetWindowRect(hDlg,&rcDlg);  // Dialog, relative to screen origin
          int OrigX = rcDlg.left + GetSystemMetrics(SM_CXFRAME);
          int OrigY = rcDlg.top  + GetSystemMetrics(SM_CYFRAME)
                                 + GetSystemMetrics(SM_CYCAPTION);

          GetWindowRect(hShade,&rcShade); // Control: relative to screen origin
          rcShade.left   -= OrigX;        // make relative to Dialog
          rcShade.top    -= OrigY;
          rcShade.right  -= OrigX;
          rcShade.bottom -= OrigY;

          IntersectRect(&cliprgn,&ps.rcPaint,&rcShade);
          HRGN hClipRgn = CreateRectRgnIndirect(&cliprgn);
          SelectClipRgn(ps.hdc,hClipRgn);
                    
          hOldBrush = (HBRUSH)SelectObject( ps.hdc, hBrush );
          hOldPen   = (HPEN)SelectObject( ps.hdc, hPen1 );

          int height = rcShade.bottom - rcShade.top;

          if ( height > HMIN )  // h in dialog resource > 1
             Rectangle(ps.hdc, rcShade.left,  rcShade.top, 
                               rcShade.right, rcShade.bottom);
          else
          {  
#ifndef WIN32
             MoveTo(ps.hdc, rcShade.left,    rcShade.bottom-2);
#else
             MoveToEx(ps.hdc, rcShade.left,  rcShade.bottom-2, &point);
#endif
             LineTo(ps.hdc, rcShade.right,   rcShade.bottom-2);
          }

          SelectObject( ps.hdc, hPen2 );

          if ( height > HMIN )
          {  
#ifndef WIN32
             MoveTo(ps.hdc, rcShade.right-1,   rcShade.top);
#else
             MoveToEx(ps.hdc, rcShade.right-1, rcShade.top, &point);
#endif
             LineTo(ps.hdc, rcShade.right-1,   rcShade.bottom);
          }
#ifndef WIN32
          MoveTo(ps.hdc, rcShade.left,    rcShade.bottom-1);
#else
          MoveToEx(ps.hdc, rcShade.left,  rcShade.bottom-1, &point);
#endif
          LineTo(ps.hdc, rcShade.right,   rcShade.bottom-1);

          SelectObject( ps.hdc, hOldBrush );
          SelectObject( ps.hdc, hOldPen );
          DeleteObject( hClipRgn );

          ID++;  // loop until sequence 1000, 1001, 1002 ... is finished
    }
    DeleteObject( hBrush );
    DeleteObject( hPen1 );
    DeleteObject( hPen2 );

    return FALSE;
}

VOID Dialog::Adjust( HWND hwnd, UINT Code, UINT Pos)
{
   int  Min, Max, Val, LineSize, PageSize;
   UINT idControl;

   GetScrollRange(hwnd,SB_CTL,&Min,&Max);      
   Val = GetScrollPos(hwnd,SB_CTL);
#ifdef WIN32
   idControl = GetWindowLong(hwnd,GWL_ID);
#else
   idControl = GetWindowWord(hwnd,GWW_ID);
#endif
   GetSizes(idControl,LineSize,PageSize);

   switch (Code)
   {
      case SB_BOTTOM:
              Val = Max;
              break;
      case SB_TOP:
              Val = Min;
              break;
      case SB_LINEUP:
              Val -= MIN(LineSize,Val-Min);
              break;
      case SB_LINEDOWN:
              Val += MIN(LineSize,Max-Val);
              break;
      case SB_PAGEUP:
              Val -= MIN(PageSize,Val-Min);
              break;
      case SB_PAGEDOWN:
              Val += MIN(PageSize,Max-Val);
              break;
      case SB_THUMBTRACK:
      case SB_THUMBPOSITION:
              Val = Pos;
              break;
      default:
              break;
   }
   SetScrollPos(hwnd,SB_CTL,Val,TRUE);
}

VOID Dialog::AdjustP( HWND hwnd, UINT Code, UINT Pos)
{
// for RotKnobs

   int  Min, Max, Val, LineSize, PageSize;
   UINT idControl;

   LONG minmax = SendMessage(hwnd,RKM_GETRANGE,0,0L);      
#ifdef WIN32
   short min, max;
   min = LOWORD(minmax);
   max = HIWORD(minmax);
   Min = min;
   Max = max;
#else
   Min = LOWORD(minmax);
   Max = HIWORD(minmax);
#endif
   Val = (int)SendMessage(hwnd,RKM_GETPOS,0,0L);

#ifdef WIN32
   idControl = GetWindowLong(hwnd,GWL_ID);
#else
   idControl = GetWindowWord(hwnd,GWW_ID);
#endif
   GetSizes(idControl,LineSize,PageSize);

   switch (Code)
   {
      case SB_BOTTOM:
              Val = Max;
              break;
      case SB_TOP:
              Val = Min;
              break;
      case SB_LINEDOWN:
              Val -= MIN(LineSize,Val-Min);
              break;
      case SB_LINEUP:
              Val += MIN(LineSize,Max-Val);
              break;
      case SB_PAGEDOWN:
              Val -= MIN(PageSize,Val-Min);
              break;
      case SB_PAGEUP:
              Val += MIN(PageSize,Max-Val);
              break;
      case SB_THUMBTRACK:
      case SB_THUMBPOSITION:
#ifdef WIN32
              Val = (short)Pos;
#else
              Val = Pos;
#endif
              break;
      default:
              break;
   }
   SendMessage(hwnd,RKM_SETPOS,Val,0L);
}

void Dialog::GetSizes(UINT, int &LineSize, int &PageSize)
{
   LineSize = 1;
   PageSize = 10;
}

#ifdef __BORLANDC__
#pragma argsused
#endif

BOOL Dialog::WmTimer(UINT id)
{
   return FALSE;
}

#ifdef __BORLANDC__
#pragma argsused
#endif

BOOL Dialog::WmActivate(UINT wP, HWND hD)
{
   return FALSE;
}

BOOL Dialog::Close( void )
{
   TerminateDialog();
   return TRUE;
}

BOOL Dialog::Free( void )
{
   return TRUE;
}

VOID Dialog::TerminateDialog(VOID)
{
}

VOID Dialog::SetHandle(HWND hDlg)
{
   hDlg_ = hDlg;
#ifndef WIN32
   SetWindowLong(hDlg,DWL_USER,(long)this);
#else
   SetWindowLong(hDlg,GWL_USERDATA,(long)this);
#endif
}

HWND Dialog::GetHandle(void)
{
   return hDlg_;
}

void Dialog::SetIcon(HICON hIc)
{
   hIcon = hIc;
}

/*
long APIENTRY DlgProc( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
   pDialog = Dialog::pDialogList_->FindDialog( hDlg );

   if (pDialog == NULL)
   {
     if (wMsg == WM_INITDIALOG)
     {
        pDialog = (Dialog *)lParam;
        return (pDialog->Handler(hDlg, wMsg, wParam, lParam));
     }
     else
        return FALSE;
   }
   else
     return (pDialog->Handler(hDlg, wMsg, wParam, lParam));
}
*/

long APIENTRY DlgProc( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
     if (wMsg == WM_INITDIALOG)
     {
        pDialog = (Dialog *)lParam;
        return (pDialog->Handler(hDlg, wMsg, wParam, lParam));
     }
     else
     {
#ifndef WIN32
        pDialog = (Dialog *)GetWindowLong(hDlg,DWL_USER);
#else
        pDialog = (Dialog *)GetWindowLong(hDlg,GWL_USERDATA);
#endif
        if (pDialog != NULL)
           return (pDialog->Handler(hDlg, wMsg, wParam, lParam));
        else
           return FALSE;
     }
}


///////////////////////////////////////////////////////////////////////////////
///////////////////// Implementation of class ModalDialog /////////////////////
///////////////////////////////////////////////////////////////////////////////

ModalDialog :: ModalDialog( int ResId, Window *opWin)
             : Dialog( ResId, opWin->GetHandle())
{
}

ModalDialog :: ModalDialog( int ResId, Dialog *opDlg)
             : Dialog( ResId, opDlg->GetHandle())
{
}

ModalDialog :: ~ModalDialog()
{
   if (hDlg_ != NULL)
      TerminateDialog();
}

void ModalDialog::Start()
{
   DialogBoxParam( Application::GetInstance()
                 , pResourceName_
                 , hParent_
                 , pHandler_
                 , (DWORD) this );
}

void ModalDialog::TerminateDialog()
{
   if (!bClosing_)
   {
      bClosing_ = TRUE;
      EndDialog( hDlg_, 0 );
   }
}

///////////////////////////////////////////////////////////////////////////////
//////////////////// Implementation of class ModelessDialog ///////////////////
///////////////////////////////////////////////////////////////////////////////

ModelessDialog :: ModelessDialog( int ResId, Window *opWin )
                : Dialog( ResId, opWin->GetHandle())
{
}

ModelessDialog :: ModelessDialog( int ResId, Dialog *opDlg )
                : Dialog( ResId, opDlg->GetHandle())
{
}

ModelessDialog :: ModelessDialog( int ResId, HWND hDlg )
                : Dialog( ResId, hDlg)
{
}

ModelessDialog :: ~ModelessDialog()
{
}

void ModelessDialog::Start()
{
   CreateDialogParam( Application::GetInstance()
                    , pResourceName_
                    , hParent_
                    , pHandler_
                    , (DWORD) this );
}

void ModelessDialog::TerminateDialog()
{
   if (!bClosing_)
   {
      bClosing_ = TRUE;

      if (hDlg_ != NULL)
      {
         DestroyWindow( hDlg_ );
      }
   }
}

BOOL ModelessDialog::WmActivate(UINT wP, HWND hD)
{
// see Dr. Dobb's Journal, July 1994, page 10
 
   if (wP == WA_INACTIVE)
   {
       Application::SetActiveDlg(NULL);
   }
   else
   {
       Application::SetActiveDlg(hD);
   }
   return FALSE;
}

///////////////////////////////////// EOF /////////////////////////////////////

