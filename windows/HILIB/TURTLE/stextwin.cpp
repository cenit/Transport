///////////////////////////////////////////////////////////////////////////////
//////////////////// Implementation of class TextWindow //////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

#ifdef WIN32
#ifndef WIN95
#define WINNT
#endif
#endif

WNDPROC TextWindow::lpMyListProc = NULL;
int     TextWindow::counter = 0;        // instance counter

#ifdef __BORLANDC__
RECT TextWindow :: defwindim = {10,20,400,300}; // MSC creates FAR_DATA segment
#endif

TextWindow :: TextWindow( int nCmdShow, LPCSTR szWindowName, RECT rc)
             : Window( nCmdShow, szWindowName, rc)
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
   AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
   AppendMenu(hMenu,MF_STRING,IDM_FONSI,"Font Size\tCtrl+S");

   if (GetSysColor(COLOR_WINDOW) == RGB(255,255,255))  // if ordinary white ...
      opBrush = new Brush(RGB(240,240,240));             // ... then use light gray
   else
      opBrush = new Brush(GetSysColor(COLOR_WINDOW));

#ifdef WINNT
   opFont = new Font("r_ascii");
   opFont->SetFixedPitch(TRUE);
   opFont->SetPointSize(9);
#else
   opFont = new Font("r_ascii",OEM_CHARSET);
   opFont->SetFixedPitch(TRUE);
   char fsistr[10] = "";
   GetPrivateProfileString("Tedi","font_size","6",fsistr,10,"Turtle.ini");
   fontsize = atoi(fsistr);
   opFont->SetPointSize(fontsize);
#endif

   RECT r;
   GetClientRect(this->GetHandle(),&r);
   opList = new ListBox(0,this,-1,-1,r.right+2,r.bottom+2,"");
   opList->SetFont(opFont);

// window subclassing: Get a hook to listbox control's WndProc

   if (counter == 0)
      lpMyListProc = MakeProcInstance((WNDPROC)MyListProc,
                                      Application::GetInstance());
   lpListProc = (WNDPROC) GetWindowLong(opList->GetHandle(),GWL_WNDPROC);
   SetWindowLong(opList->GetHandle(),GWL_WNDPROC,(LONG)lpMyListProc);
   Id = counter++;

   aborted = FALSE;
   closed = FALSE;
   
   Invalidate();
   wait(100);   // wait for filling listbox field with new brush

// engage horizontal scrolling

   FInitListboxExtents(opList->GetHandle());
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
      FFreeListboxExtents(opList->GetHandle());

      if (opFont)
      {  delete opFont;
         opFont = NULL;
      }
      if (opBrush)
      {  delete opBrush;
         opBrush = NULL;
      }
      if (opList)
      {  SetWindowLong(opList->GetHandle(),GWL_WNDPROC,(LONG)lpListProc);
         lpListProc = NULL;
         delete opList;
         opList = NULL;
         if (--counter == 0)
            FreeProcInstance(lpMyListProc);
      }
//      DestroyWindow(GetHandle());  // found by Bounds Checker
      closed = TRUE;
   }
   return FALSE;
}

long TextWindow :: WmPaint( const PAINTSTRUCT &)
{
    if (GetActiveWindow() == this->GetHandle())
       opList->SetFocus();
    return FALSE;
}

long TextWindow :: WmSize(int w, int h)
{
    if (opList && (w>0))
    {
       MoveWindow(opList->GetHandle(),-1,-1,w+2,h+2,TRUE);
       opList->SetFocus();
    }
    return DefaultEvent();
}

long TextWindow :: WmCtlColor(HDC hdc, HWND hChild)
{
    if (opList->GetHandle() == hChild)
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
//    opList->Append(line);   // without horizontal scrolling

//  Update the scroll extent if necessary then add string.

    WAddExtentEntry(opList->GetHandle(), (char *)line);
    SendMessage(opList->GetHandle(), LB_ADDSTRING, 0, (LONG)line);
}

void TextWindow :: ShowFile(LPCSTR FileName)
{
    ToLongDialog *opDlg;
    FILE *fp;
    char buffer[150], line[80];
    long len, nlines = 0;
    int  c, nl = 0, nr = 0;

    if ((fp = fopen(FileName,"r")) != NULL)
    {
       while ((c = fgetc(fp)) != EOF)
       {
          if (c == '\n')
          ++nlines;
       }

       fseek(fp,0L,2);   // put to EOF
       len = ftell(fp);
       fseek(fp,0L,0);   // put to back to BOF

#ifdef WIN32
       if (nlines > 65536L) // File too long -- get first line to display
#else
       if (len > 65536L) // File too long -- get first line to display
#endif
       {
           sprintf(line,"File length=%ld bytes in %ld lines.",len,nlines);
           opDlg = new ToLongDialog(IDD_TOLONG_DIALOG,this,"0",line);
           if (opDlg->GetResult())
           {
               char line[20];
               strcpy(line,opDlg->GetString());
               sscanf(line,"%d",&nr);
           }
           else
           {
               fclose(fp);
               delete opDlg;
               delete this;
               return;
           }
       }

       fseek(fp,0L,0);   // put to back to BOF
       while ( ! feof(fp) )
       {  if (fscanf(fp,"%[^\n]",&buffer[0]))
          {  fgetc(fp);
//             if (nl >= nr) opList->Append(buffer);
             if (nl >= nr) WriteLn(buffer);
             buffer[0] = '\0';
          }
          else            // empty line
          {  fgetc(fp);
//             if (nl >= nr) opList->Append("");
             if (nl >= nr) WriteLn("");
          }
          nl++;
          if ( (nl-nr) == 60 ) wait(50);
       }
       fclose(fp);
    }
}

void TextWindow :: FontSize(void)
{
    char fsistr[40];

    itoa(fontsize,fsistr,10);
    StringDialog *opStDlg = new StringDialog(IDD_FS_DIALOG,this,fsistr);
    if (opStDlg->GetResult())
       strcpy(fsistr,opStDlg->GetString());
    delete opStDlg;

    WritePrivateProfileString("Tedi","font_size",fsistr,"Turtle.ini");
    fontsize = atoi(fsistr);

    opFont->SetPointSize(fontsize);
    opList->SetFont(opFont);

    UpdateWindow(this->GetHandle());
    Invalidate();
}

void TextWindow :: Home(void)
{
    SendMessage(opList->GetHandle(),LB_SETTOPINDEX,0,0L);
}

void TextWindow :: End(void)
{
    DWORD endpos = SendMessage(opList->GetHandle(),LB_GETCOUNT,0,0L) - 1;
    SendMessage(opList->GetHandle(),LB_SETTOPINDEX,(WPARAM)endpos,0L);
}

void TextWindow :: SetFocus(void)
{
    opList->SetFocus();
}

void TextWindow :: ToClipboard(void)
{
    int   i;
    DWORD wlen = 100;
    DWORD cnt = SendMessage(opList->GetHandle(),LB_GETCOUNT,0,0L);
    for (i=0; i<(int)cnt; i++)
    {
        wlen +=
        SendMessage(opList->GetHandle(),LB_GETTEXTLEN,(WPARAM)i,0L) + 2;
    }


    HGLOBAL hMem = GlobalAlloc(GHND,(DWORD)wlen);
    LPSTR pMem = (LPSTR)GlobalLock(hMem);

    for (i=0; i<(int)cnt; i++)
    {
        SendMessage(opList->GetHandle(),LB_GETTEXT,(WPARAM)i,
                    (LPARAM)((LPCSTR) pMem));
        DWORD len =
        SendMessage(opList->GetHandle(),LB_GETTEXTLEN,(WPARAM)i,0L);
        pMem += len;
        *pMem++ = '\r';
        *pMem++ = '\n';
    }

    GlobalUnlock(hMem);

    OpenClipboard(this->GetHandle());
    EmptyClipboard();
    SetClipboardData(CF_TEXT,hMem);
    CloseClipboard();

    MessageBeep( 0 );
}

void TextWindow :: Save( void )
{
    char line[150];
    char FltStr[] = "(*.lis)|*.lis|(*.*)|*.*|";
    char Ext[] = "*.lis";
    char Dir[] = "";
    SaveFileDlg *opF = new SaveFileDlg(this->GetHandle(), Dir, FltStr, Ext);
    if (opF->GetResult())
    {
       FILE *fh = fopen(opF->GetFileName(),"w+");
       if (fh)
       {
          DWORD cnt = SendMessage(opList->GetHandle(),LB_GETCOUNT,0,0L);
          for (int i=0; i<(int)cnt; i++)
          {
              SendMessage(opList->GetHandle(),LB_GETTEXT,(WPARAM)i,
                          (LPARAM)((LPCSTR) line));
              fprintf(fh,"%s\n",line);
          }
          fclose(fh);    
       }
    }
    delete opF;
    opList->SetFocus();
}

void TextWindow :: Print( void )
{
   char line[200]; 
   int fs;

   Font *opFont = new Font("Courier New");

   Printer *opPrt = new Printer;
   opPrt->Start("Output of Data Listing from TURTLE",this);

   fs = opPrt->GetUserHeight() / 60;
   opFont->SetPointSize(fs);
   opPrt->SetFont(opFont);

   int inc = opPrt->GetUserHeight() / 60;

   DWORD lc = SendMessage(opList->GetHandle(),LB_GETCOUNT,0,0L);
   
   int x = opPrt->GetUserWidth()/20;
   int y = inc;
   int index = 0;
   for (UINT i=0; i<lc; i++)
   {
       SendMessage(opList->GetHandle(),LB_GETTEXT,(WPARAM)i,
                   (LPARAM)((LPCSTR) line));
       if ( (index++ >= 55) || (line[0] == 49) )
       {  index = 0;
          y = 0;
          opPrt->NextPage();
          opPrt->SetFont(opFont);
       }
       y += inc;       
       opPrt->PosStr(x,y,line);
   }
   opPrt->End(this);
   delete opPrt;
   delete opFont;
   opList->SetFocus();
}

long TextWindow :: WmSysMenuCommand ( WPARAM MenuItem )
{
   switch (MenuItem)
   {
      case IDM_PASTE: ToClipboard(); return TRUE;
      case IDM_PRINT: Print(); return TRUE;
      case IDM_SAVE:  Save(); return TRUE;
      case SC_CLOSE:  aborted = TRUE; WmClose(); delete this; return TRUE;
      case IDM_HOME:  Home(); return TRUE;
      case IDM_END:   End(); return TRUE;
      case IDM_FONSI: FontSize(); return TRUE;
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
         case 'S': FontSize(); return FALSE;
         case 'Y': aborted = TRUE; WmClose(); delete this; return FALSE;
         default:  return TRUE;
      }
   }
   else
      return TRUE;
}

BOOL TextWindow :: WmNewKeyDown ( WPARAM Key )
{
// suppress listbox control's messagebeep from pressing Ctrl_C etc.

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
// suppress listbox control's messagebeep from pressing Ctrl_C etc.

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

LONG APIENTRY MyListProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
   BOOL retval; // if FALSE, then listbox control's original WndProc is not called

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
      return CallWindowProc(opWin->GetlpListProc(),hWnd,iMessage,wParam,lParam);
   else
      return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
////////////////////// ToLongDialog class implementation //////////////////////
///////////////////////////////////////////////////////////////////////////////

ToLongDialog :: ToLongDialog(int ResID, Window *opWin,
                             LPCSTR text, LPCSTR cap)
              : ModalDialog(ResID,opWin)
{
   caCaption = cap;
   hPar = opWin->GetHandle();
   SetString(text);
   Start();
}

ToLongDialog :: ~ToLongDialog(VOID)
{
    delete opEdit;
    delete opText;
}

BOOL ToLongDialog :: Init(VOID)
{
   CenterDialog(hPar,this->GetHandle());

// transfer text to stringedit

   opEdit = new StringEdit(IDC_EDIT,this);
   opEdit->SetText(caString);
   opEdit->SetCharLimit(6);
   opEdit->SetCrntSelection(0,strlen(caString));
   opEdit->SetFocus();

// transfer text to static control

   opText = new StaticText(IDR_TEXT,this);
   opText->SetText(caCaption);

   return FALSE;
}

long ToLongDialog :: Control( UINT nControlId, UINT )
{ 
    switch (nControlId)
    {   case IDOK:
        case IDYES:
          opEdit->GetText(caString,79);
          caString[6] = '\0';
          TerminateDialog();
          result = TRUE;
          break;
        case IDCANCEL:
        case IDNO:
          TerminateDialog();
          result = FALSE;
          break;
    }
    return FALSE;
}

BOOL ToLongDialog :: GetResult(VOID)
{
    return(result);
}

VOID ToLongDialog :: SetString(LPCSTR cpString)
{
    strcpy(caString,cpString);
}

char *ToLongDialog :: GetString( void )
{
    return caString;
}

/////////////////////////////////// EOF ///////////////////////////////////////
