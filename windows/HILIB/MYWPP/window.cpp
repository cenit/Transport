///////////////////////////////////////////////////////////////////////////////
//////////////////// implementation of class Window ///////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"


HINSTANCE Window::hInstance_     = NULL;
char   Window::szClassName_[] = "My Window";
#ifdef __BORLANDC__
RECT   Window::defwindim = {CW_USEDEFAULT,0,CW_USEDEFAULT,0};
#endif
// previous statement generates FAR_DATA segment with MSC

Window::Window( int nCmdShow, LPCSTR szWindowName, RECT rc
              , HMENU hMenu, HWND hParent, DWORD dwStyle )
       :DisplayDevice(IDO_WINDOW)
{
   hWnd_ =  CreateWindow( szClassName_
                        , szWindowName != NULL
                          ? szWindowName
                          : szClassName_
                        , dwStyle
                        , rc.left
                        , rc.top
                        , rc.right
                        , rc.bottom
                        , hParent
                        , hMenu
                        , Application::GetInstance()
                        , (LPSTR)this );
   Show (nCmdShow);
   Update();
}

Window::~Window()
{
   if (hWnd_ != NULL)
      DestroyWindow( hWnd_ );
}

BOOL Window::Register( int nIconName, LPCSTR pMenuName )
{
   WNDCLASS wndclass;

   wndclass.style         = CS_HREDRAW | CS_VREDRAW;

   wndclass.lpfnWndProc   = (WNDPROC) ::WndProc;
   wndclass.cbClsExtra    = 0;
   wndclass.cbWndExtra    = sizeof( Window * ) + sizeof(long);
   wndclass.hInstance     = hInstance_
                          = Application::GetInstance();
   if (nIconName == NULL)
      wndclass.hIcon      = 0;
   else
      wndclass.hIcon       = LoadIcon(hInstance_, MAKEINTRESOURCE(nIconName));
   wndclass.hCursor        = LoadCursor( NULL, IDC_ARROW );
   wndclass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
   wndclass.lpszMenuName   = pMenuName;
   wndclass.lpszClassName  = szClassName_;

   return(!RegisterClass( &wndclass ));
}

LONG Window::WndProcess( UINT iMessage, WPARAM wParam, LPARAM lParam )
{
  HWND hChild;

  wParam_ = wParam;
  lParam_ = lParam;
  iMessage_ = iMessage;

  switch( iMessage )
  {
    case WM_USER:
     return WmUser( wParam, lParam);

    case WM_ERASEBKGND:
     return WmEraseBackGround();
   
#ifdef WIN32
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORLISTBOX:
      hChild = (HWND)lParam;
#else
    case WM_CTLCOLOR:
      if (HIWORD(lParam) != CTLCOLOR_EDIT   &&
          HIWORD(lParam) != CTLCOLOR_STATIC &&
          HIWORD(lParam) != CTLCOLOR_LISTBOX )
         return DefWindowProc( hWnd_, iMessage, wParam, lParam );
      else
         hChild = LOWORD(lParam);
#endif
      return WmCtlColor((HDC)wParam,hChild);

    case WM_COMMAND:
      return WmMenuCommand(LOWORD(wParam));

#ifdef WIN95
    case WM_NOTIFY:
      return WmNotify(lParam);
#endif

    case WM_KEYDOWN:
      return WmKeyDown(LOWORD(wParam));

    case WM_KEYUP:
      return WmKeyUp(LOWORD(wParam));

    case WM_CHAR:
      return WmCharKey(LOWORD(wParam));

    case WM_SYSCOMMAND:
      return WmSysMenuCommand(LOWORD(wParam));

    case WM_VSCROLL:
#ifdef WIN32
      return WmVScroll(LOWORD(wParam), HIWORD(wParam));
#else
      return WmVScroll(wParam, LOWORD(lParam));
#endif

    case WM_HSCROLL:
#ifdef WIN32
      return WmHScroll(LOWORD(wParam), HIWORD(wParam));
#else
      return WmHScroll(wParam, LOWORD(lParam));
#endif

    case WM_PAINT:
    {
      RECT rc;
      GetClientRect( hWnd_, (LPRECT) &rc );
      SetExtents(rc);

      PAINTSTRUCT ps;
      BeginPaint( hWnd_, &ps );
      hdc_ = ps.hdc;
      LONG ReturnValue = WmPaint( ps );
      EndPaint( hWnd_, &ps );

      return ReturnValue;
    }

    case WM_SETCURSOR:
    {
       HWND hwParent = GetParent(hWnd_);
       if (hwParent && SendMessage(hwParent,WM_SETCURSOR,wParam,lParam))
          return TRUE;
       if (wParam == (WPARAM)hWnd_)
       {
          if (LOWORD(lParam) == HTCLIENT)
             return WmSetCursor();
          else
             return DefWindowProc( hWnd_, iMessage, wParam, lParam );
       }
    }

    case WM_MOUSEMOVE:
    {
      POINT Where;
      Where.x = LOWORD (lParam) ;
      Where.y = HIWORD (lParam) ;
      return WmMouseMove( Where , wParam);
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

    case WM_LBUTTONUP:
    {
      POINT Where;
      Where.x = LOWORD (lParam) ;
      Where.y = HIWORD (lParam) ;
      return WmLeftMouseUp( Where );
    }

    case WM_RBUTTONUP:
    {
      POINT Where;
      Where.x = LOWORD (lParam) ;
      Where.y = HIWORD (lParam) ;
      return WmRightMouseUp( Where );
    }

#ifdef WIN95
    case WM_LBUTTONDBLCLK:
    {
      POINT Where;
      Where.x = LOWORD (lParam) ;
      Where.y = HIWORD (lParam) ;
      return WmLeftButtonDblclk( Where );
    }
#endif

    case WM_SIZE:
      return WmSize(LOWORD(lParam), HIWORD(lParam));

    case WM_TIMER:
      return WmTimer(wParam);

    case WM_DESTROY:
      return WmDestroy();

    case WM_CLOSE:
      return WmClose();

    default:
      return DefWindowProc( hWnd_, iMessage, wParam, lParam );
  }
}

void Window::Enable(void)
{
   EnableWindow( hWnd_, TRUE );
}

void Window::Disable(void)
{
   EnableWindow( hWnd_, FALSE );
}

void Window::SetWinPointer( HWND hWnd )
{
   hWnd_ = hWnd;
   SetWindowLong( hWnd, 0, (LONG) this );
}

Window *Window::GetWinPointer( HWND hWnd )
{
   return (Window *)GetWindowLong( hWnd, 0 );
}

#ifdef __BORLANDC__
#pragma argsused
#endif

long Window::WmVScroll(UINT Code, UINT Pos)
{
   return FALSE;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
long Window::WmHScroll(UINT Code, UINT Pos)
{
   return FALSE;
}

long Window::WmPaint( const PAINTSTRUCT & )
{
   return DefWindowProc( hWnd_, WM_PAINT, wParam_, lParam_);
}

BOOL Window::WmDestroy( void )
{
   return FALSE;
}

BOOL Window::WmClose( void )
{
   return FALSE;
}

BOOL Window::WmSetCursor( void )
{
   SetCursor(LoadCursor(NULL,IDC_ARROW));
   return FALSE;
}

LONG Window::WmCtlColor(HDC , HWND)
{
   return DefWindowProc( hWnd_, iMessage_, wParam_, lParam_);
}

LONG Window::WmCharKey (WPARAM)
{
   return DefWindowProc( hWnd_, WM_CHAR, wParam_, lParam_);
}

LONG Window::WmKeyUp (WPARAM)
{
   return DefWindowProc( hWnd_, WM_KEYUP, wParam_, lParam_);
}

LONG Window::WmKeyDown (WPARAM)
{
   return DefWindowProc( hWnd_, WM_KEYDOWN, wParam_, lParam_);
}

LONG Window::WmMenuCommand (WPARAM)
{
   return DefWindowProc( hWnd_, WM_COMMAND, wParam_, lParam_);
}

#ifdef WIN95
LONG Window::WmNotify (LPARAM)
{
   return DefWindowProc( hWnd_, WM_NOTIFY, wParam_, lParam_);
}
#endif

LONG Window::WmSysMenuCommand (WPARAM)
{
   return DefWindowProc( hWnd_, iMessage_, wParam_, lParam_);
}

LONG Window::WmMouseMove(POINT &, WPARAM)
{
   return DefWindowProc( hWnd_, WM_MOUSEMOVE, wParam_, lParam_);
}

LONG Window::WmLeftMouseDown(POINT &)
{
   return DefWindowProc( hWnd_, WM_LBUTTONDOWN, wParam_, lParam_);
}

LONG Window::WmRightMouseDown(POINT &)
{
   return DefWindowProc( hWnd_, WM_RBUTTONDOWN, wParam_, lParam_);
}

LONG Window::WmLeftMouseUp(POINT &)
{
   return DefWindowProc( hWnd_, WM_LBUTTONUP, wParam_, lParam_);
}

LONG Window::WmRightMouseUp(POINT &)
{
   return DefWindowProc( hWnd_, WM_RBUTTONUP, wParam_, lParam_);
}

LONG Window::WmLeftButtonDblclk(POINT &)
{
   return DefWindowProc( hWnd_, WM_LBUTTONDBLCLK, wParam_, lParam_);
}

LONG Window::WmSize(int, int)
{
   return DefWindowProc( hWnd_, WM_RBUTTONDOWN, wParam_, lParam_);
}

LONG Window::WmTimer(UINT )
{
   return DefWindowProc( hWnd_, WM_TIMER, wParam_, lParam_);
}

LONG Window::WmUser(WPARAM wParam_, LPARAM lParam_)
{
   return DefWindowProc( hWnd_, WM_USER, wParam_, lParam_);
}

LONG Window::WmEraseBackGround(void)
{
   return DefWindowProc( hWnd_, WM_ERASEBKGND, wParam_, lParam_);
}

void Window::Invalidate( void )
{
   InvalidateRect( hWnd_, NULL, TRUE );
}

void Window::Invalidate2( void )
{
   InvalidateRect( hWnd_, NULL, FALSE );
}

void  Window::Update( void )
{
   UpdateWindow( hWnd_ );
}

BOOL Window::Show(int nCmdShow)
{
   return ShowWindow( hWnd_, nCmdShow );
}

LONG Window::DefaultEvent( void )
{
   return DefWindowProc( hWnd_, iMessage_, wParam_, lParam_);
}

LONG APIENTRY WndProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
     Window *pWindow = Window::GetWinPointer( hWnd );

     if (pWindow == NULL)
     {
        if ((iMessage == WM_CREATE) || (iMessage == WM_NCCREATE))
        {
          LPCREATESTRUCT lpcs;
          lpcs = (LPCREATESTRUCT) lParam;
          pWindow = (Window *) lpcs->lpCreateParams;
          pWindow->SetWinPointer( hWnd );

          return pWindow->WndProcess( iMessage, wParam, lParam );
        }
        else
          return DefWindowProc( hWnd, iMessage, wParam, lParam );
     }
     else
        return pWindow->WndProcess( iMessage, wParam, lParam );
}
/////////////////////////////////// EOF ///////////////////////////////////////
