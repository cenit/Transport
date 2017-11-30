///////////////////////////////////////////////////////////////////////////////
///////////////////// Implementation of class TextWindow //////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

WNDPROC TextWindow::lpMyEditProc = NULL;
int     TextWindow::counter = 0;        // instance counter

#ifdef __BORLANDC__
RECT TextWindow :: defwindim = {10,20,400,300}; // MSC creates FAR_DATA segment
#endif

TextWindow :: TextWindow( int nCmdShow, LPCSTR szWindowName, RECT rc,
              BOOL sml)
            : Window( nCmdShow, szWindowName, rc)
{
// add items to system menu

   hMenu = GetSystemMenu(GetHandle(),FALSE);
   AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
   AppendMenu(hMenu,MF_STRING,IDM_SAVE, String(IDS_FIL_MENU));
   AppendMenu(hMenu,MF_STRING,IDM_PRINT,String(IDS_PRT_MENU));
   AppendMenu(hMenu,MF_STRING,IDM_PASTE,String(IDS_CBD_MENU));
   AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
   AppendMenu(hMenu,MF_STRING,IDM_HOME, String(IDS_HOM_MENU));
   AppendMenu(hMenu,MF_STRING,IDM_END,  String(IDS_END_MENU));

   if (GetSysColor(COLOR_WINDOW) == RGB(255,255,255))  // if ordinary white ...
      opBrush = new Brush(RGB(240,240,240));             // ... then use light gray
   else
      opBrush = new Brush(GetSysColor(COLOR_WINDOW));

   if (sml)
   {
      opFont = new Font(String(IDS_R_ASCII),OEM_CHARSET);
      opFont->SetFixedPitch(TRUE);
      opFont->SetPointSize(6);
   }
   else
   {  opFont = new Font(String(IDS_SYSTEM));
      opFont->SetFixedPitch(TRUE);
   }

   RECT r;
   GetClientRect(this->GetHandle(),&r);
   opEdit = new StringEdit(0,this,-1,-1,r.right+2,r.bottom+2,"");
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

void TextWindow :: DisableClose(void)
{
    EnableMenuItem(hMenu, SC_CLOSE, MF_GRAYED);
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
    {
       MoveWindow(opEdit->GetHandle(),-1,-1,w+2,h+2,TRUE);
       opEdit->SetFocus();
    }
    return DefaultEvent();
}

long TextWindow :: WmCtlColor(HDC hdc, HWND hChild)
{
    if (opEdit->GetHandle() == hChild)
    {
       if (GetSysColor(COLOR_WINDOW) == RGB(255,255,255))  // if ordinary white ...
          SetBkColor(hdc,RGB(240,240,240));                  // ... then use light gray
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
    char text[1024];

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

void TextWindow :: Erase(void)
{
    opEdit->SetCrntSelection(0,(UINT)-1);
    SendMessage(opEdit->GetHandle(),EM_REPLACESEL,0,(long)(LPSTR)"");
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

char TextWindow :: WaitForKeyDown(void)
{
    char lline[100]; 
    UINT len = 0;

    while ( len==0 && !aborted)
    {
       opEdit->GetTextLine(opEdit->GetLineCount()-1,lline,100);
       len = strlen(lline);
       wait(55);
    }
    return lline[0];
}

BOOL TextWindow :: IsClosed(void)
{
   return closed;
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
    opEdit->SetCrntSelection(0,(UINT)-1);
    SendMessage(opEdit->GetHandle(),WM_COPY,0,0L);
    opEdit->SetFocus();
    End();
    MessageBeep( 0 );
}

void TextWindow :: Print( void )
{
   char line[200]; 
   int fs;

   Font *opFont = new Font(String(IDS_COURIER));

   Printer *opPrt = new Printer;
   opPrt->Start(String(IDS_LISTING),this);

   fs = opPrt->GetUserHeight() / 60;

   opFont->SetPointSize(fs);
   opPrt->SetFont(opFont);

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
    char Dir[] = "";
    SaveFileDlg *opF = new SaveFileDlg(this->GetHandle(), Dir,
                                                          String(IDS_FL2STR),
                                                          String(IDS_LISEXT,1));
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
