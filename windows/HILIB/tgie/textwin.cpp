///////////////////////////////////////////////////////////////////////////////
///////////////////// Implementation of class TextWindow //////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

void YieldQQ(void)
{
/*
   check if any window message is available and send this message to
   the destination application. Call this function to maintain window's
   nonpreemtive multitasking abilities. (By Stefan Ritt)
   This routine can be imported by FORTRAN DLLs.
*/
   MSG msg;

   while (PeekMessage(&msg, 0, 0, 0, TRUE))
   {  TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
}

void wait(int msecs)
{
#ifndef WIN32
   long st = (long)GetTickCount();
   do {
         YieldQQ(); YieldQQ(); YieldQQ();

      } while ( (long)GetTickCount() < st+(long)msecs );
#else
   Sleep(msecs);
   YieldQQ();
#endif
}

///////////////////////////////////////////////////////////////////////////////

WNDPROC TextWindow::lpMyEditProc = NULL;
int     TextWindow::counter = 0;        // instance counter

#ifdef __BORLANDC__
RECT TextWindow :: defwindim = {10,20,400,300}; //MSC creates FAR_DATA segment
#endif

TextWindow :: TextWindow( int nCmdShow, LPCSTR szWindowName, RECT rc, HWND hw)
           : Window( nCmdShow, szWindowName, rc, NULL, hw)
{
// add items to system menu

   HMENU hMenu = GetSystemMenu(GetHandle(),FALSE);
   AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
   AppendMenu(hMenu,MF_STRING,IDM_SAVE, "to File\tCtlr+F");
   AppendMenu(hMenu,MF_STRING,IDM_PRINT,"to Printer\tCtrl+P");
   AppendMenu(hMenu,MF_STRING,IDM_PASTE,"to Clipboard\tCtrl+C");
   AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
   AppendMenu(hMenu,MF_STRING,IDM_HOME,"Home\tCtrl+H");
   AppendMenu(hMenu,MF_STRING,IDM_END,"End\tCtrl+E");

   if (GetSysColor(COLOR_WINDOW) == RGB(255,255,255))  // if ordinary white ...
      opBrush = new Brush(RGB(0,255,255));             // ... then use cyan
   else
      opBrush = new Brush(GetSysColor(COLOR_WINDOW));

   opFont = new Font("System");
   opFont->SetFixedPitch(TRUE);

   RECT r;
   GetClientRect(this->GetHandle(),&r);
   opEdit = new StringEdit(0,this,-1,-1,r.right,r.bottom,"");
   opEdit->SetFont(opFont);

#ifdef WIN32
#ifdef WIN95
   SendMessage(opEdit->GetHandle(),EM_LIMITTEXT,(WPARAM)0,0L);
#else
   SendMessage(opEdit->GetHandle(),EM_LIMITTEXT,(WPARAM)5000000L,0L);
#endif
#else
   SendMessage(opEdit->GetHandle(),EM_LIMITTEXT,(WPARAM)0,0L);
#endif

// window subclassing: Get a hook to edit control's WndProc

   if (counter == 0)
      lpMyEditProc = MakeProcInstance((WNDPROC)MyEditProc,
                                      Application::GetInstance());
   lpEditProc = (WNDPROC) GetWindowLong(opEdit->GetHandle(),GWL_WNDPROC);
   SetWindowLong(opEdit->GetHandle(),GWL_WNDPROC,(LONG)lpMyEditProc);
   Id = counter++;

   aborted = FALSE;
   closed = FALSE;
   
   Invalidate();
   wait(100);   // wait for filling edit field with new brush
}

BOOL TextWindow::WmDestroy( void )
{
    return FALSE;
}

TextWindow :: ~TextWindow()
{
    WmClose();
}

TextWindow :: WmClose( void )
{
   if (!closed)
   {
      if (opFont)
      {  delete opFont;
         opFont = NULL;
      }
      if (opBrush)
      {  delete opBrush;
         opBrush = NULL;
      }
      if (opEdit)
      {  SetWindowLong(opEdit->GetHandle(),GWL_WNDPROC,(LONG)lpEditProc);
         lpEditProc = NULL;
         delete opEdit;
         opEdit = NULL;
         if (--counter == 0)
            FreeProcInstance(lpMyEditProc);
      }
//      DestroyWindow(GetHandle()); // found with Bounds Checker
      closed = TRUE;
   }
   return FALSE;
}

BOOL TextWindow :: IsClosed(void)
{
   return closed;
}

long TextWindow :: WmPaint( const PAINTSTRUCT &)
{
    if (GetActiveWindow() == this->GetHandle())
       opEdit->SetFocus();
    return FALSE;
}

long TextWindow :: WmSize(int w, int h)
{
    if (opEdit && (w>0))
    {  opEdit->SetSize(w,h);
       opEdit->SetFocus();
    }
    return DefaultEvent();
}

long TextWindow :: WmCtlColor(HDC hdc, HWND hChild)
{
    if (opEdit->GetHandle() == hChild)
    {
       if (GetSysColor(COLOR_WINDOW) == RGB(255,255,255))  // if ordinary white ...
          SetBkColor(hdc,RGB(0,255,255));                  // ... then use cyan
       else
          SetBkColor(hdc,GetSysColor(COLOR_WINDOW));
       return ((DWORD)opBrush->GetHandle());
    }
    else
       return ((DWORD)opBrush->GetHandle());
}

void TextWindow :: WriteLn(LPCSTR line)
{
    char crlf[3]; crlf[0] = 0x0d; crlf[1] = 0x0a; crlf[2] = '\0';
    char text[100];

    strcpy(text,line);
    strcat(text,crlf);

    opEdit->SetText(text);
}

void TextWindow :: Home(void)
{
    opEdit->SetCrntSelection(0,0);
}

void TextWindow :: End(void)
{
    UINT size = opEdit->GetSize();
    opEdit->SetCrntSelection(size,size);
}

void TextWindow :: SetFocus(void)
{
    opEdit->SetFocus();
}

void TextWindow :: WaitFor(LPCSTR str)
{
    char lline[100]; 
    BOOL nok = TRUE;

    while ( nok && !aborted)
    {
       opEdit->GetTextLine(opEdit->GetLineCount()-1,lline,100);
       nok = strcmp(lline,str);
       wait(55);
    }
}

void TextWindow :: ShowFile(LPCSTR FileName)
{
    opEdit->ReadFile(FileName);
}

void TextWindow :: ShowMemory(Memory *opMem)
{
    HLOCAL pMem = opMem->GetHandle();
    UINT Size = opMem->GetSize();
    opEdit->ReadMemory(pMem,Size+1);
}

void TextWindow :: ToClipboard(void)
{
    opEdit->SetCrntSelection((UINT)0,(UINT)-1);
    SendMessage(opEdit->GetHandle(),WM_COPY,0,0L);
    opEdit->SetFocus();
    End();
    MessageBeep( 0 );
}

void TextWindow :: Print( void )
{
   char line[200]; 
   int fs;

   Font *opFont = new Font("Courier New");
   Printer *opPrt = new Printer;

   fs = opPrt->GetUserHeight() / 60;
   opFont->SetPointSize(fs,opPrt);  // opPrt must be set here
   opPrt->SetFont(opFont);

   opPrt->Start("Listing",this);

   int inc = opPrt->GetUserHeight() / 60;
   UINT lc = opEdit->GetLineCount();
   
   int x = opPrt->GetUserWidth()/20;
   int y = inc;
   int index = 0;
   for (UINT i=0; i<lc; i++)
   {
       if (index++ >= 55)
       {  index = 0;
          y = 0;
          opPrt->NextPage();
          opPrt->SetFont(opFont);
       }
       y += inc;       
       opEdit->GetTextLine(i,line,200);
       opPrt->PosStr(x,y,line);
   }
   opPrt->End(this);
   delete opPrt;
   delete opFont;
   opEdit->SetFocus();
   End();
}

void TextWindow :: Save( void )
{
    char FltStr[] = "(*.lis)|*.lis|(*.*)|*.*|";
    char Ext[] = "*.lis";
    char Dir[] = "";
    SaveFileDlg *opF = new SaveFileDlg(this->GetHandle(), Dir, FltStr, Ext);
    if (opF->GetResult())
       opEdit->WriteFile(opF->GetFileName());
    delete opF;
    opEdit->SetFocus();
    End();
}

long TextWindow :: WmSysMenuCommand ( WPARAM MenuItem )
{
   switch (MenuItem)
   {  case IDM_PASTE: ToClipboard(); return TRUE;
      case IDM_PRINT: Print(); return TRUE;
      case IDM_SAVE:  Save(); return TRUE;
      case SC_CLOSE:  aborted = TRUE; WmClose(); delete this; return TRUE;
      case IDM_HOME:  Home(); return TRUE;
      case IDM_END:   End(); return TRUE;
      default:        return DefaultEvent();
   }
}

/////////////////////////// subclassing methods ///////////////////////////////

BOOL TextWindow :: WmNewKeyUp ( WPARAM Key )
{
   if (GetKeyState(VK_CONTROL) < 0)  // control key pressed
   {  switch (Key)
      {
         case 'C': ToClipboard(); return FALSE;
         case 'P': Print(); return FALSE;
         case 'F': Save(); return FALSE;
         case 'H': Home(); return FALSE;
         case 'E': End(); return FALSE;
         case 'Y': aborted = TRUE; WmClose(); delete this; return FALSE;
         default:  return TRUE;
      }
   }
   else
      return TRUE;
}

BOOL TextWindow :: WmNewKeyDown ( WPARAM Key )
{
// suppress edit control's messagebeep from pressing Ctrl_C etc.

   if (GetKeyState(VK_CONTROL) < 0)  // control key pressed
   {  switch (Key)
      {
         case 'C':
         case 'P':
         case 'F':
         case 'H':
         case 'Y':
         case 'E': return FALSE;
         default:  return TRUE;
      }
   }
   else
      return TRUE;
}

BOOL TextWindow :: WmNewChar ( WPARAM Key )
{
// suppress edit control's messagebeep from pressing Ctrl_C etc.

   if (GetKeyState(VK_CONTROL) < 0)  // control key pressed
   {  switch (Key + 64)
      {
         case 'C':
         case 'P':
         case 'F':
         case 'H':
         case 'Y':
         case 'E': return FALSE;
         default:  return TRUE;
      }
   }
   else
      return TRUE;
}

LONG APIENTRY MyEditProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
   BOOL retval; // if FALSE, then edit control's original WndProc is not called

// Get stored object pointer to parent window (done with Window::SetWinPointer).

   TextWindow *opWin = (TextWindow *) GetWindowLong(GetParent(hWnd),0);

   switch (iMessage)
   {
      case WM_CHAR: retval = opWin->WmNewChar(LOWORD(wParam)); break;
      case WM_KEYUP: retval = opWin->WmNewKeyUp(LOWORD(wParam)); break;
      case WM_KEYDOWN: retval = opWin->WmNewKeyDown(LOWORD(wParam)); break;
      default: retval = TRUE; break;
   }

   if (retval)
      return CallWindowProc(opWin->GetlpEditProc(),hWnd,iMessage,wParam,lParam);
   else
      return FALSE;
}

/////////////////////////////////// EOF ///////////////////////////////////////
