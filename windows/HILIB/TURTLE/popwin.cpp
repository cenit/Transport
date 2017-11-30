///////////////////////////////////////////////////////////////////////////////
///////////////////// Implementation of class PopupWindow /////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

#define BW 60 // Button width
#define BH 30 // Button height
#define BO 40 // Button offset from bottom
#define ID 123
static PushButton *opOK = NULL;
HBRUSH hBrCreateBrush(HDC hDC, HINSTANCE hInst, DWORD dwRGB); // defined in ..\mywpp\dialog.cpp
BOOL ok = FALSE;

// for subclassing:

WNDPROC fnOldOK;
LRESULT CALLBACK MyOKProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

////////////////////////////////////////////////////////////////////////////////////////////

PopupWindow :: PopupWindow( int nCmdShow, LPCSTR szWindowName, LPCSTR szBitmapName, RECT rc)
             : Window( nCmdShow, szWindowName, rc)
{
   bmname = szBitmapName;
   hInst  = Application::GetInstance();
   hBrush = 0;
   int w  = GetUserWidth();
   int h  = GetUserHeight();
   opOK   = new PushButton(ID,this,(w-BW)/2,h-BO,BW,BH,"OK");
   hOK    = GetDlgItem(this->GetHandle(),ID);
   SetFocus(hOK);
   fnOldOK = (WNDPROC) SetWindowLong(hOK,GWL_WNDPROC,(LPARAM)MyOKProc); // subclassing
   Invalidate();
}

BOOL PopupWindow::WmDestroy( void )
{
   if (opOK) delete opOK;   // found with Bounds Checker
   opOK = NULL;
   return FALSE;
}

BOOL PopupWindow::WmClose( void )
{
   DestroyWindow(GetHandle());
   return FALSE;
}

PopupWindow :: ~PopupWindow()
{
   PopupWindow::WmClose();
}

long PopupWindow :: WmPaint(const PAINTSTRUCT &ps)
{
// Background as special Brush

   if (!hBrush)
      hBrush = hBrCreateBrush(ps.hdc,hInst,GetSysColor(COLOR_BTNFACE));
   SelectObject(ps.hdc,hBrush);
   int w = GetUserWidth();
   int h = GetUserHeight();
   PutRectangle(-1,-1,w+1,h+1);
   DeleteObject(hBrush);

// Legal Message as Bitmap

   DrawBitmap(this, ps.hdc);

   SetFocus(hOK);
   return FALSE;
}

long PopupWindow :: WmMenuCommand(WPARAM mid)
{
   switch (mid)
   {
      case ID: WmClose(); break;
   }
   return FALSE;
}

long PopupWindow :: WmEraseBackGround(void)
{
   return FALSE; // tell Windows to not erase background (white flickering)
}

void PopupWindow :: DrawBitmap(DisplayDevice *opDev, HDC hdcscr)
{
   HBITMAP hBitmap;
   BITMAP  bm;
   HDC     hdcMem;
   RECT    rc;
   POINT   ptStart, ptSize, ptOrg, ptStretch;

   hBitmap = LoadBitmap(Application::GetInstance(),bmname);
   GetObject(hBitmap, sizeof(BITMAP), (LPSTR) &bm);
   hdcMem = CreateCompatibleDC(hdcscr);
   SelectObject(hdcMem, hBitmap);
   SetMapMode(hdcMem, GetMapMode(hdcscr));

   ptSize.x = bm.bmWidth;
   ptSize.y = bm.bmHeight;
   DPtoLP(hdcscr,&ptSize,1);
   ptOrg.x = 0;
   ptOrg.y = 0;
   DPtoLP (hdcMem, &ptOrg, 1);
   rc = GetExtents();
   ptStretch.x = rc.right-rc.left;
   ptStretch.y = rc.bottom-rc.top;
   ptStart.x = 0;
   ptStart.y = 0;

   ptOrg.x = (GetUserWidth()-ptSize.x)/2;
   if (ptOrg.x < 0) ptOrg.x = 0;
   ptOrg.y = (GetUserHeight()-BO-ptSize.y)/2;
   if (ptOrg.y < 0) ptOrg.y = 0;

   opDev->CopyBits(ptOrg,ptStretch,ptStart,hdcMem);
   DeleteDC(hdcMem);   
   DeleteObject(hBitmap);
}

long PopupWindow :: WmSize(int w, int h)
{
// OK button in window

   if (opOK)
   {
      delete opOK;
      opOK = new PushButton(ID,this,(w-BW)/2,h-BO,BW,BH,"OK");
      hOK  = GetDlgItem(this->GetHandle(),ID);
      fnOldOK = (WNDPROC) SetWindowLong(hOK,GWL_WNDPROC,(LPARAM)MyOKProc); // subclassing
      SetFocus(hOK);
   }
   return DefaultEvent();
}

///////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK MyOKProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
   if (iMsg == WM_COMMAND)  // includes also pressing <CR>
   {
      SendMessage(GetParent(hWnd),WM_CLOSE,0,0L);
   }
   return CallWindowProc(fnOldOK,hWnd,iMsg,wParam,lParam);
}

/////////////////////////////////// EOF ///////////////////////////////////////
