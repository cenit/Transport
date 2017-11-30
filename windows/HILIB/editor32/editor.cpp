///////////////////////////////////////////////////////////////////////////////
// Text Editor for Windows 3.1 and Windows NT by Urs Rohrer (PSI), 1-MAY-93  //
// New version for Windows95 by Urs Rohrer (PSI), November 1995              //
// EM_GETHANDLE and EM_SETHANDLE do not work under Windows95                 //
// new function GetEditBuffer() (slow) helps to solve the problem            //
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

//////////////////////////// Globals //////////////////////////////////////////

#ifndef WIN95
PushButton *opQuit  = NULL;
PushButton *opSave  = NULL;
PushButton *opCut   = NULL;
PushButton *opCopy  = NULL;
PushButton *opPaste = NULL;
PushButton *opFind  = NULL;
PushButton *opNext  = NULL;
PushButton *opRepl  = NULL;
PushButton *opBegin = NULL;
PushButton *opEnd   = NULL;
#endif

WNDPROC MainWindow::lpMyEditProc = NULL;

char FltStr[] = "(*.*)|*.*|(*.tex)|*.tex|(*.txt)|*.txt|(*.lis)|*.lis|";
char Dir[]    = "";
char Ext[20]  = "*.*";
char CMDLINE[80];

#ifdef WIN95
#define BH 28
#define SH 20
#else
#define BH 30
#define SH 25
#endif

#ifndef WIN32
#define Sleep wait
#endif


#ifndef WIN32
#define KE_RELEASE 0x8000
#ifdef __BORLANDC__
#define _asm asm
#endif

///////////////////////////////////////////////////////////////////////////////

extern "C"
{
// For Windows 3.1:
// Get address of undocumented windows function.
// From A. Schulman's Undocumented Windows, p.490
// Keybd_Event1(): Simulates pressing keys.
// Motivation: 
// In Windows simultaneous pressing shift and arrow_keys pastes text.
// This feature may be exploited for simulating the corresponding EDT
// keypad commands. Computer has to hold down shift key !!!
// had to rename Keybd_Event to Keybd_Event1 because of Symantec C++

   WNDPROC GetProc(LPCSTR szFunction, LPCSTR szModule, int nOrdinal)
   {
      HMODULE hModule;
      WNDPROC lpfnByName;
      WNDPROC lpfnByOrd;

      if ((hModule = GetModuleHandle(szModule)) == NULL)
         if ((hModule = LoadLibrary (szModule)) == NULL)
            return 0;
      if ((lpfnByName = (WNDPROC)GetProcAddress(hModule, szFunction)) == NULL)
         return 0;
      if ((lpfnByOrd = (WNDPROC)GetProcAddress(hModule, (LPCSTR)(LONG)nOrdinal)) == NULL)
         return 0;
      if (lpfnByName != lpfnByOrd)
         return 0;
      return lpfnByName;
   }

   void (FAR PASCAL *Keybd_Event1) (void); // protoyupe of this undocumented func.

// simulate keeping pressed down shift key

   void SetShift(void)
   {
     if((Keybd_Event1 = 
           (void(pascal *)(void))GetProc("KEYBD_EVENT","USER",289))!=0)
     {
        _asm mov ax, VK_SHIFT;
        _asm xor bx,bx;
        Keybd_Event1();
     }
   }

// simulate releasing pressed down shift key

   void ReleaseShift(void)
   {
     if((Keybd_Event1 = 
           (void(pascal *)(void))GetProc("KEYBD_EVENT","USER",289))!=0)
     {
        _asm mov ax, VK_SHIFT + KE_RELEASE;
        _asm xor bx,bx;
        Keybd_Event1();
     }
   }
}

#else

// For WIN32: There is a documented function.
// but ReleaseShift is not working under Windows 2000 ?????

   void SetShift(void)
   {
      OSVERSIONINFO osvi;
      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      GetVersionEx(&osvi);
      if ((osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
          (osvi.dwMajorVersion == 5))
         ;
      else
         keybd_event(VK_SHIFT, 0, KEYEVENTF_EXTENDEDKEY | 0, 0L);
   }

   void ReleaseShift(void)
   {
      OSVERSIONINFO osvi;
      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      GetVersionEx(&osvi);
      if ((osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
          (osvi.dwMajorVersion == 5))
         ;
      else
         keybd_event(VK_SHIFT, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0L);
   }

#endif

///////////////////////////////////////////////////////////////////////////////

void MyYield(void)
{
   MSG msg;

   while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
}

void wait(int msecs)
{
   long st;

   for (st = GetTickCount(); (long)GetTickCount() < st+(long)msecs; MyYield())
      ;
}

//////////////////////// class MainWindow implementation ///////////////////////

MainWindow :: MainWindow(int nCmdShow, LPCSTR szWindowName, RECT rc)
            : Window(nCmdShow, szWindowName, rc)
{

#ifndef WIN32

extern char **__argv;

#endif

#ifdef WIN32
#ifdef __BORLANDC__

extern char **__argv;

#endif
#endif

   opEdit   = 0;
#ifdef WIN95
   hWndStatusBar = 0;
   hWndToolbar   = 0;
   static int aWidths[5];
#else
   opLineNr = 0;
#endif

   opMenu = new Menu(IDR_MAIN_MENU,this);
   opSysMenu = new SystemMenu(this);
   opSysMenu->AppendSep();
   opSysMenu->Append(IDM_ABOUT,"&About Editor32...");

   if (GetSysColor(COLOR_WINDOW) == RGB(255,255,255))  // if ordinary white ...
      opBrush = new Brush(RGB(240,240,240));             // ... then use light gray
   else
      opBrush = new Brush(GetSysColor(COLOR_WINDOW));
   opBrush2 = new Brush(GetSysColor(COLOR_BTNFACE));

   opFont = new Font("System");
   opFont->SetFixedPitch(TRUE);

   RECT r;
   GetClientRect(this->GetHandle(),&r);
#ifdef WIN95
   int n = (GetScreenWidth()>800) ? +1 : -4;
   opEdit = new StringEdit(0,this,-1,BH,r.right+2,r.bottom-BH-SH+n,"");
#else
   opEdit = new StringEdit(0,this,-1,BH,r.right+2,r.bottom-BH-SH+1,"");
#endif
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

#ifdef WIN95
   hWndStatusBar = CreateStatusWindow(
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|CCS_BOTTOM|SBARS_SIZEGRIP,
		" ",
		this->GetHandle(),
		2);
   aWidths[0] = 100;
   aWidths[1] = 135;
   aWidths[2] = 170;
   aWidths[3] = 205;
   aWidths[4] = -1;
   SendMessage(hWndStatusBar,SB_SETPARTS,5,(LPARAM)aWidths);
   SendMessage(hWndStatusBar,SB_SETTEXT,0|SBT_NOBORDERS,(LPARAM)"  Key lock status:");
   SendMessage(hWndStatusBar,SB_SETTEXT,4|SBT_NOBORDERS,(LPARAM)" ");
#else
   opLineNr = new StaticText(0,this,0,r.bottom-SH+2,r.right+2,SH-2,
		"Cursor at line # 1");
#endif
   LineNumber = 1;
   BeginningOfCurrentLine = 0;
   strcpy(UndeleteBuff,"");
   strcpy(WordBuff,"");
   StartMark = -1;

// window subclassing: Get a hook to edit control's WndProc

   lpMyEditProc = MakeProcInstance((WNDPROC)MyEditProc,
                                      Application::GetInstance());
   lpEditProc = (WNDPROC) GetWindowLong(opEdit->GetHandle(),GWL_WNDPROC);
   SetWindowLong(opEdit->GetHandle(),GWL_WNDPROC,(LONG)lpMyEditProc);

   closed  = FALSE;
   done    = TRUE;
   wCase   = 0;
   wQuery  = 1;
   strcpy(FindStr,"");
   strcpy(ReplaceStr,"");

   keypad = FALSE;
   GoldPressed = FALSE;
   Forward = TRUE;
   MouseDown = FALSE;

// special for Notebooks

   GoldKey = TRUE;
   int hFile;
   char Buff[10];
   char FileName[80];
   strcpy(FileName,__argv[0]);
   int l1 = strlen(FileName);
   int l2 = strlen("editor32.exe");
   FileName[l1-l2] = '\0'; // Remaining is the directory where editor32 is.
   strcat(FileName,"goldkey");  // File goldkey has to be in the same dir.
   if ((hFile = _lopen(FileName, OF_READ)) != -1)
   {
	_lread(hFile, Buff, 10);
	_lclose(hFile);
	if (Buff[0] == 'N')   // if NO, then SetNumLock not active
	   GoldKey = FALSE;
   }
   SaveOldNumLockStatus();
   SetNumLock();
   
#ifdef WIN95
   SetStatusButtons();
   ApplicationBar();
#endif

   Invalidate();

   if (strlen(CMDLINE))
   {  wait (100);
      AnsiLower(CMDLINE);
      char text[100] = "Editor32 - ";
      strcat(text,CMDLINE);
      SetWindowText(this->GetHandle(),text);
      opEdit->ReadFile(CMDLINE);
      DisplayLineNumber();
   }
   else
   {
      opEdit->SetCrntSelection((UINT)0,(UINT)-1);
      SendMessage(opEdit->GetHandle(),EM_REPLACESEL,0,(long)(LPSTR)"");
   }
}

MainWindow :: ~MainWindow()
{
}

MainWindow :: WmClose( void )
{
   if (!closed)
   {
#ifndef WIN95
      if (opQuit)
      {  delete opQuit;
         opQuit  = NULL;
      }
      if (opSave)
      {  delete opSave;
         opSave  = NULL;
      }
      if (opCut)
      {  delete opCut;
         opCut   = NULL;
      }
      if (opCopy)
      {  delete opCopy;
         opCopy  = NULL;
      }
      if (opPaste)
      {  delete opPaste;
         opPaste = NULL;
      }
      if (opFind)
      {  delete opFind;
         opFind  = NULL;
      }
      if (opNext)
      {  delete opNext;
         opNext  = NULL;
      }
      if (opRepl)
      {  delete opRepl;
         opRepl  = NULL;
      }
      if (opBegin)
      {  delete opBegin;
         opBegin = NULL;
      }
      if (opEnd)
      {  delete opEnd;
         opEnd   = NULL;
      }
#endif
      if (opMenu)
         delete opMenu;
      if (opSysMenu)
         delete opSysMenu;
      if (opFont)
      {  delete opFont;
         opFont = NULL;
      }
      if (opBrush)
      {  delete opBrush;
         opBrush = NULL;
      }
      if (opBrush2)
      {  delete opBrush2;
         opBrush2 = NULL;
      }
      if (opEdit)
      {  SetWindowLong(opEdit->GetHandle(),GWL_WNDPROC,(LONG)lpEditProc);
         lpEditProc = NULL;
         delete opEdit;
         opEdit = NULL;
         FreeProcInstance(lpMyEditProc);
      }
#ifdef WIN95
      if (hWndStatusBar)
	 DestroyWindow(hWndStatusBar);
#else
      if (opLineNr)
         delete opLineNr;
#endif

      if (StartMark != -1)
         ReleaseShift();

      ResetNumLock();
      WmDestroy();
      closed = TRUE;
   }
   return FALSE;
}

BOOL MainWindow::WmDestroy( void )
{
    PostQuitMessage(0);
    return FALSE;
}

long MainWindow :: WmSysMenuCommand(WPARAM MenuItem)
{
//  Handle system menu command events.

    switch (MenuItem)
    {   case IDM_ABOUT:
	{
 	   SimpleDialog(IDD_ABOUT_DIALOG,this);
           return TRUE;
	}
	case WM_CLOSE:
	{
	   WmClose();
           return TRUE;
	}

        default:
           return DefaultEvent();
    }
}

long MainWindow :: WmPaint( const PAINTSTRUCT &ps)
{
    RECT rc;

    if (GetActiveWindow() == this->GetHandle() && opEdit)
       opEdit->SetFocus();

//    FillRect(ps.hdc,&ps.rcPaint,opBrush2->GetHandle());
    rc.left   = ps.rcPaint.left ;
    rc.top    = ps.rcPaint.top ;
    rc.right  = ps.rcPaint.right ;
    rc.bottom = BH;
    FillRect(ps.hdc,&rc,opBrush2->GetHandle());

    return FALSE;
}

long MainWindow :: WmSize(int w, int h)
{
    if (opEdit && (w>0))
    {  
#ifdef WIN95
       int n = (GetScreenWidth()>800) ? +1 : -4;
       MoveWindow(opEdit->GetHandle(),-1,BH,w+2,h-BH-SH+n,TRUE);
#else
       MoveWindow(opEdit->GetHandle(),-1,BH,w+2,h-BH-SH+1,TRUE);
#endif
       opEdit->SetFocus();
    }

#ifdef WIN95
    if (hWndStatusBar && (w>0))
    {
	RECT rc;
	GetWindowRect(hWndStatusBar,&rc);
	int cy = rc.bottom - rc.top;
	MoveWindow(hWndStatusBar,0,h-cy,w,cy,TRUE);
    }
#else
    if (opLineNr && (w>0))
	MoveWindow(opLineNr->GetHandle(),0,h-SH+2,w+2,SH+2,TRUE);
#endif

#ifdef WIN95
    if (hWndToolbar)
       SendMessage(hWndToolbar, TB_AUTOSIZE, 0L, 0L);
#else
    int bw = w/10;
    if (opQuit) delete opQuit;
    opQuit = new PushButton((int)IDM_EXIT,this,3,3,bw-3,24,"Quit");

    if (opSave) delete opSave;
    opSave = new PushButton((int)IDM_SAVE,this,1*bw+3,3,bw-3,24,"Save");

    if (opCut) delete opCut;
    opCut = new PushButton((int)IDM_CUT,this,2*bw+3,3,bw-3,24,"Cut");

    if (opCopy) delete opCopy;
    opCopy = new PushButton((int)IDM_COPY,this,3*bw+3,3,bw-3,24,"Copy");

    if (opPaste) delete opPaste;
    opPaste = new PushButton((int)IDM_PASTE,this,4*bw+3,3,bw-3,24,"Paste");

    if (opFind) delete opFind;
    opFind = new PushButton((int)IDM_FIND,this,5*bw+3,3,bw-3,24,"Find");

    if (opNext) delete opNext;
    opNext = new PushButton((int)IDM_FIND_NEXT,this,6*bw+3,3,bw-3,24,"Next");

    if (opRepl) delete opRepl;
    opRepl = new PushButton((int)IDM_REPLACE,this,7*bw+3,3,bw-3,24,"Replace");

    if (opBegin) delete opBegin;
    opBegin = new PushButton((int)IDM_BEGIN,this,8*bw+3,3,bw-3,24,"BOF");

    if (opEnd) delete opEnd;
    opEnd = new PushButton((int)IDM_END,this,9*bw+3,3,bw-3,24,"EOF");
#endif

    return DefaultEvent();
}

long MainWindow :: WmCtlColor(HDC hdc, HWND hChild)
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
    {
       SetBkColor(hdc,GetSysColor(COLOR_BTNFACE));
       ::SetTextColor(hdc,GetSysColor(COLOR_BTNTEXT));
       return ((DWORD)opBrush2->GetHandle());
    }
}

long MainWindow :: WmTimer(UINT id)
{
//  perform delayed actions

    int i, nLine;

    if (id == 1)       // ======> induced by WmNewLeftMouseDown()
    {  KillTimer(this->GetHandle(),id);
       DisplayLineNumber();
    }
    else if (id == 2)  // ======> induced by Adjust()
    {
       KillTimer(this->GetHandle(),id);
       switch(SaveCode)
       {
          case SB_TOP:
            LineNumber = 1;
            break;
          case SB_BOTTOM:
            LineNumber  = 1 +
                 (int)SendMessage(opEdit->GetHandle(),EM_GETLINECOUNT,0,0L);
            break;
          case SB_LINEUP:
            if (LineNumber > 1)
               LineNumber--;
            break;
          case SB_LINEDOWN:
            nLine = (int)SendMessage(opEdit->GetHandle(),EM_GETLINECOUNT,0,0L);
            if (nLine > LineNumber)
               LineNumber++;
            break;
          case SB_PAGEUP:
          case SB_PAGEDOWN:
          case SB_THUMBTRACK:
          case SB_THUMBPOSITION:
            i = OffsetFromTop + (int)
                  SendMessage(opEdit->GetHandle(),EM_GETFIRSTVISIBLELINE,0,0L);
            if (i < (int)SendMessage(opEdit->GetHandle(),EM_GETLINECOUNT,0,0L))
               LineNumber = i + 1;
            break;
          default:
            break;
       }
       GotoLine(LineNumber);
       done = TRUE;
    }
    else if (id == 3)  // ======> induced by WmNewKeyDown
    {
       KillTimer(this->GetHandle(),id);
       DisplayLineNumber();
    }
    else
    {
    }
    return FALSE;
}

void MainWindow :: Home(void)
{
    opEdit->SetCrntSelection(0,0);
    DisplayLineNumber(); 
    opEdit->SetFocus();
}

void MainWindow :: End(void)
{
    UINT size = opEdit->GetSize();
    opEdit->SetCrntSelection(size,size);
    DisplayLineNumber(); 
    opEdit->SetFocus();
}

void MainWindow :: Print( void )
{
   char line[200]; 
   int fs;

   Font *opFont = new Font("Courier New");

   Printer *opPrt = new Printer;
   opPrt->Start("Listing",this);

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
}

void MainWindow :: Save( void )
{
    SaveFileDlg *opF = new SaveFileDlg(this->GetHandle(), Dir, FltStr, Ext);
    if (opF->GetResult())
    {  opEdit->WriteFile(opF->GetFileName());
       char *ptr = strchr(opF->GetFileName(),'.');  // store new extension
       if (ptr)
       {  strcpy(Ext,"*");
          strcat(Ext,ptr);
       }
    }
    delete opF;
    opEdit->SetFocus();
}

void MainWindow :: Open( void )
{
    OpenFileDlg *opF = new OpenFileDlg(this->GetHandle(), Dir, FltStr, Ext);
    if (opF->GetResult())
    {
       opEdit->ReadFile(opF->GetFileName());
       strcpy(CMDLINE,opF->GetFileName());          // display Filename
       AnsiLower(CMDLINE);
       char text[100] = "Editor32 - ";
       strcat(text,CMDLINE);
       SetWindowText(this->GetHandle(),text);
       char *ptr = strchr(opF->GetFileName(),'.');  // store new extension
       if (ptr)
       {  strcpy(Ext,"*");
          strcat(Ext,ptr);
       }
    }
    delete opF;
    opEdit->SetFocus();
}

void MainWindow :: BackFile(LPCSTR FileName)
{
   char OldFile[80];

   strcpy(OldFile,FileName);
   char *ptr = strchr(OldFile,'.');
   if (ptr)
     *ptr = '\0';
   strcat(OldFile,".bak");

   remove(OldFile);
   rename(FileName,OldFile);
}

void MainWindow :: DisplayLineNumber(void)
{
    int  start, end;
    char line[50];

    opEdit->GetCrntSelection(start,end);
    LineNumber = 1 +
    (int)SendMessage(opEdit->GetHandle(),EM_LINEFROMCHAR,(UINT)end,0L);
    sprintf(line,"Cursor at line # %d",LineNumber);
#ifdef WIN95
    SendMessage(hWndStatusBar,SB_SETTEXT,4|SBT_NOBORDERS,(LPARAM)(LPCSTR)line);
#else
    opLineNr->SetText(line);
#endif

    BeginningOfCurrentLine = 
    (int)SendMessage(opEdit->GetHandle(),EM_LINEINDEX,(UINT)(LineNumber-1),0L);

    *(WORD *) &LineBuff[0] = sizeof(LineBuff)-1;
    WORD cbText = (WORD) SendMessage(opEdit->GetHandle(),EM_GETLINE,
                                     LineNumber-1,(LONG)(LPSTR)LineBuff);
    LineBuff[cbText] = '\0';
    EndOfCurrentLine = BeginningOfCurrentLine + strlen(LineBuff);

    if (StartMark != -1)
       opEdit->SetCrntSelection(StartMark,end);
}

void MainWindow :: GotoLine(UINT LineNr)
{
    LineNr--;
    UINT cLines = (UINT)SendMessage(opEdit->GetHandle(),EM_GETLINECOUNT,0,0L);
    if (cLines > LineNr)
    {  UINT index =
          (UINT)SendMessage(opEdit->GetHandle(),EM_LINEINDEX,(WPARAM)LineNr,0L);
       opEdit->SetCrntSelection(index,index);
       DisplayLineNumber();
    }       
    else
    {
       SimpleDialog oDlg(IDD_INFO_DIALOG,this,"Line # not existing");
       opEdit->SetFocus();
    }
}
void MainWindow :: DeleteToEndOfLine( void )
{
    int start,end;

    DisplayLineNumber();
    opEdit->GetCrntSelection(start,end);
    opEdit->SetCrntSelection(start,EndOfCurrentLine);
    SendMessage(opEdit->GetHandle(),EM_REPLACESEL,0,(LONG)(LPSTR)"");
}
void MainWindow :: DeleteLine( void )
{
    int start,end;

    DisplayLineNumber();
    opEdit->GetCrntSelection(start,end);
    strcpy(UndeleteBuff,&LineBuff[start-BeginningOfCurrentLine]);
    opEdit->SetCrntSelection(start,EndOfCurrentLine+2);
    SendMessage(opEdit->GetHandle(),EM_REPLACESEL,0,(LONG)(LPSTR)"");
}

void MainWindow :: UndeleteLine( void )
{
    char crlf[3] = "\015\012";
    strcat(UndeleteBuff,crlf);
    SendMessage(opEdit->GetHandle(),EM_REPLACESEL,0,(LONG)(LPSTR)UndeleteBuff);
    DisplayLineNumber();
}

void MainWindow :: DeleteWord( void )
{
    HGLOBAL hMem;
    LPSTR  pStart, pSave;
    int    start, end;

    opEdit->GetCrntSelection(start,end);

    hMem   = opEdit->GetEditBuffer();
    pStart = (LPSTR)GlobalLock(hMem);

    pStart += (long)start;

    pSave  = pStart;

    if (*pStart != ' ')
    {   for (int i=0; i<150; pStart++,i++)
           if (*pStart == ' ' || *pStart == 10)
           {  opEdit->SetCrntSelection(start,start+i+1);
              strncpy(WordBuff,pSave,i+1);
              WordBuff[i+1] = '\0';
              SendMessage(opEdit->GetHandle(),EM_REPLACESEL,0,(LONG)(LPSTR)"");
              break;
           }
    }
    else
    {   for (int i=0; i<150; pStart++,i++)
           if (*pStart != ' ' && *pStart != 10)
           {  opEdit->SetCrntSelection(start,start+i);
              strncpy(WordBuff,pSave,i);
              WordBuff[i] = '\0';
              SendMessage(opEdit->GetHandle(),EM_REPLACESEL,0,(LONG)(LPSTR)"");
              break;
           }
    }
    GlobalUnlock(hMem);
    GlobalFree(hMem);
}

void MainWindow :: UndeleteWord( void )
{
    SendMessage(opEdit->GetHandle(),EM_REPLACESEL,0,(LONG)(LPSTR)WordBuff);
    DisplayLineNumber();
}

void MainWindow :: StepWord( void )
{
    HGLOBAL hMem;
    LPSTR  pStart;
    int    i, start, end, pos;

    opEdit->GetCrntSelection(start,end);

    hMem   = opEdit->GetEditBuffer();
    pStart = (LPSTR)GlobalLock(hMem);
    pStart += (long)start;

    if (Forward)
    {
       for (i=0; i<150; pStart++,i++)
          if ( (*pStart      != ' ' && *pStart      != 13) && 
               (*(pStart+1L) == ' ' || *(pStart+1L) == 13) )
             break;
       pos = start+i+1;
    }
    else
    {
       for (i=0; i<150; pStart--,i++)
          if ( (*pStart      != ' ' && *pStart      != 13) && 
               (*(pStart-1L) == ' ' || *(pStart-1L) == 13) )
             break;
       pos = start-i-1;
       if (pos < 0) pos = 0;
    }
    opEdit->SetCrntSelection(pos,pos);
    DisplayLineNumber();

    GlobalUnlock(hMem);
    GlobalFree(hMem);
}

void MainWindow :: Specins( void )
{
    StringDialog *opDlg = new StringDialog(IDD_SPECINS_DIALOG,this,"");
    if (opDlg->GetResult())
    {  char string[20];
       UINT Ascii, Dummy;
       strcpy(string,opDlg->GetString());
       delete opDlg;
       UINT result = sscanf(string,"%d %d",&Ascii,&Dummy);
       if (result == 1)      // is it one legal number ?
       {  char Oem[2]="";
          char Ansi[2]="";
          Oem[0] = (char)Ascii;
          OemToAnsiBuff(Oem,Ansi,1);
          SendMessage(opEdit->GetHandle(),EM_REPLACESEL,0,(LONG)(LPSTR)Ansi);
       }
       opEdit->SetFocus();
       return;
    }
    else
    {  delete opDlg;
       opEdit->SetFocus();
    }
}

void MainWindow :: ExecCommand( void )
{
    StringDialog *opDlg = new StringDialog(IDD_COMMAND_DIALOG,this,"");
    if (opDlg->GetResult())
    {  char command[20];
       strcpy(command,opDlg->GetString());
       delete opDlg;
       AnsiLower(command);
       if (strcmp(command,"quit") == 0)       
       {  WmClose();
       }
       else if (strcmp(command,"exit") == 0)       
       {
          BackFile(CMDLINE);
          opEdit->WriteFile(CMDLINE);
          WmClose();
       }
       else if (strcmp(command,"save") == 0)       
       {
          BackFile(CMDLINE);
          opEdit->WriteFile(CMDLINE);
          opEdit->SetFocus();
       }
       else
       {  UINT LineNr, Dummy;
          UINT result = sscanf(command,"%d %d", &LineNr, &Dummy);
          if (result == 1)      // is it one legal number ?
             GotoLine(LineNr);
          opEdit->SetFocus();
       }
       return;
    }
    else
    {  delete opDlg;
       opEdit->SetFocus();
    }
}

void MainWindow :: Find(BOOL wCase)
{
    HGLOBAL hMem;
    LPSTR  pBuf, ptr;
    LPSTR  pStart;
    int    start, end;
    char   compstr[80];
    long   offset, leng;

    hMem   = opEdit->GetEditBuffer();
    pStart = (LPSTR)GlobalLock(hMem);

    strcpy(compstr,FindStr);
    leng   = strlen(compstr);

    if (!wCase)   // Case insensitive
    {
       AnsiLower(pStart);
       AnsiLower(compstr);
    }

    opEdit->GetCrntSelection(start,end);
    pBuf = pStart + start + 1;
    if ((ptr = strstr(pBuf,compstr)) != 0)
    {
       offset = (long)(ptr - pStart);
       opEdit->SetCrntSelection((UINT)offset, (UINT)(offset + leng));
       DisplayLineNumber();
    }
    else
    {  
       SimpleDialog oDlg(IDD_INFO_DIALOG,this,"String not found");
    }

    GlobalUnlock(hMem);
    GlobalFree(hMem);
}

void MainWindow :: Replace(BOOL wCase, BOOL wQuery, BOOL wReplaceAll)
{
    HGLOBAL hMem;
    LPSTR  pBuf, ptr;
    LPSTR  pStart;
    int    start, end;
    char   compstr[80];
    long   offset, leng;
    int    result;
    QueryDialog *opDlg;

    hMem   = opEdit->GetEditBuffer();
    pStart = (LPSTR)GlobalLock(hMem);

    strcpy(compstr,FindStr);
    leng   = strlen(compstr);

    if (!wCase)
    {
       AnsiLower(compstr);
       AnsiLower(pStart);
    }

    if (!wReplaceAll)
    {
       opEdit->GetCrntSelection(start,end);
       pBuf = pStart + start + 1;
       if ((ptr = strstr(pBuf,compstr)) != 0)
       {  offset = (long)(ptr - pStart);
          opEdit->SetCrntSelection((UINT)offset, (UINT)(offset + leng));
          DisplayLineNumber();
          result = 0;
          if (wQuery)
          {  opDlg = new QueryDialog(IDD_QUERY_DIALOG,this);
             while ( 1 )
             {   result = opDlg->GetResult();
                 if (result != -1)
                    break;
                 MyYield();
                 Sleep(100);
             }
             delete opDlg;
             if (result == 2) goto l2;
          }
          if (result == 0) SendMessage(opEdit->GetHandle(),EM_REPLACESEL,0,
                                       (LPARAM)(LPCSTR)ReplaceStr);
       }
       else
       {  
           SimpleDialog oDlg(IDD_INFO_DIALOG,this,"String not found");
       }
    }
    else
    {
       int counter = 0;
       if (wQuery)
          opDlg = new QueryDialog(IDD_QUERY_DIALOG,this);
       while (1)
       {  opEdit->GetCrntSelection(start,end);
          pBuf = pStart + start + 1;
          ptr = strstr(pBuf,compstr);
          if (ptr == NULL) break;
          offset = (long)(ptr - pStart);

          opEdit->SetCrntSelection((UINT)offset, (UINT)(offset + leng));

          DisplayLineNumber();
          result = 0;
          if (wQuery)
          {  while ( 1 )
             {   result = opDlg->GetResult();
                 if (result != -1)
                    break;
                 MyYield();
                 Sleep(100);
             }
             if (result == 2) goto l1;
          }
          if (result == 0)
          {  SendMessage(opEdit->GetHandle(),EM_REPLACESEL,0,
                                       (LPARAM)(LPCSTR)ReplaceStr);
             if (leng != (long)strlen(ReplaceStr))    // if length changes ...
             {
		GlobalUnlock(hMem);                   // ... get new start of mem
	        GlobalFree(hMem);
                hMem   = opEdit->GetEditBuffer();
                pStart = (LPSTR)GlobalLock(hMem);
                if (!wCase) AnsiLower(pStart);
             }
             counter++;
          }
          if (wQuery)
             opDlg->Reset();
       }
l1:
       if (wQuery)
          delete opDlg;
       char text[50];
       sprintf(text,"total %d replacements",counter);
       SimpleDialog oDlg(IDD_INFO_DIALOG,this,text);
    }
l2:
    GlobalUnlock(hMem);
    GlobalFree(hMem);
}

long MainWindow :: WmMenuCommand ( WPARAM MenuItem )
{
   switch (MenuItem)
   {  
      case IDM_PASTE:
        {
           SendMessage(opEdit->GetHandle(),WM_PASTE,0,0L);
           DisplayLineNumber();
           opEdit->SetFocus();
           return TRUE;
        }
      case IDM_COPY:
        {
           SendMessage(opEdit->GetHandle(),WM_COPY,0,0L);
           DisplayLineNumber();
           opEdit->SetFocus();
           return TRUE;
        }
      case IDM_CUT:
        {
           SendMessage(opEdit->GetHandle(),WM_CUT,0,0L);
           DisplayLineNumber();
           opEdit->SetFocus();
           StartMark = -1;
           ReleaseShift();
           return TRUE;
        }
      case IDM_PRINT: Print(); return TRUE;
      case IDM_PRINT_SETUP:
        {
           Printer *opPrt = new Printer;
           opPrt->Setup(this);
           delete opPrt;
           return TRUE;
        }
      case IDM_NEW:
        {
           opEdit->SetCrntSelection((UINT)0,(UINT)-1);
           SendMessage(opEdit->GetHandle(),EM_REPLACESEL,0,(long)(LPSTR)"");
           strcpy(CMDLINE,"");
           SetWindowText(this->GetHandle(),"Editor32");
           opEdit->SetFocus();
           DisplayLineNumber();
           return TRUE;
        }
      case IDM_SAVE:
        {
           BackFile(CMDLINE);
           opEdit->WriteFile(CMDLINE);
           opEdit->SetFocus();
           return TRUE;
        }
      case IDM_SAVE_AS: Save(); return TRUE;
      case IDM_SAVE_EXIT:
        {  
           BackFile(CMDLINE);
           opEdit->WriteFile(CMDLINE);
           WmClose();
           return TRUE;
        }
      case IDM_OPEN: Open(); DisplayLineNumber(); return TRUE;
      case IDM_EXIT: WmClose(); return TRUE;
      case IDM_ABOUT:
        { 
           SimpleDialog oDlg(IDD_ABOUT_DIALOG,this);
           opEdit->SetFocus();
           return TRUE;
        }
      case IDM_BEGIN: Home();  return TRUE;
      case IDM_END:   End(); return TRUE;
      case IDM_SELECT:
        {
           opEdit->SetCrntSelection((UINT)0,(UINT)-1);
           return TRUE;
        }
      case IDM_FIND:
        {  FindDialog *opDlg = new FindDialog(IDD_FIND_DIALOG,this,
                                              FindStr,wCase);
           if (opDlg->GetResult())
           {
              strcpy(FindStr,opDlg->GetString());
              wCase = opDlg->GetCase();
              delete opDlg;
              Find(wCase);
           }
           else
              delete opDlg;
        }
        opEdit->SetFocus();
        return TRUE;
      case IDM_FIND_NEXT:
        Find(wCase);
        opEdit->SetFocus();
        return TRUE;
      case IDM_REPLACE:
        {  ReplaceDialog *opDlg = new ReplaceDialog(IDD_REPLACE_DIALOG,this,
                                              FindStr,ReplaceStr,wCase,wQuery);
           if (opDlg->GetResult())
           {
              strcpy(FindStr,opDlg->GetFindString());
              strcpy(ReplaceStr,opDlg->GetReplaceString());
              wCase = opDlg->GetCase();
              wQuery = opDlg->GetQuery();
              wReplaceAll = opDlg->GetReplaceAll();
              delete opDlg;
              Replace(wCase,wQuery,wReplaceAll);
           }
           else
              delete opDlg;
        }
        opEdit->SetFocus();
        return TRUE;
      default:        return DefaultEvent();
   }
}

/////////////////////////// subclassing methods ///////////////////////////////

VOID MainWindow :: SetStatusButtons(VOID)
{
   BYTE KeyState[256];

   GetKeyboardState((LPBYTE) &KeyState);

   if ( (KeyState[VK_CAPITAL] & 1) == 1)
      SendMessage(hWndStatusBar,SB_SETTEXT, 1,(LPARAM)"CAP");
   else
      SendMessage(hWndStatusBar,SB_SETTEXT, 1|SBT_POPOUT,(LPARAM)"CAP");

   if ( (KeyState[144] & 1) == 1)
      SendMessage(hWndStatusBar,SB_SETTEXT, 2,(LPARAM)"NUM");
   else
      SendMessage(hWndStatusBar,SB_SETTEXT, 2|SBT_POPOUT,(LPARAM)"NUM");

   if ( (KeyState[145] & 1) == 1)
      SendMessage(hWndStatusBar,SB_SETTEXT, 3,(LPARAM)"SCRL");
   else   
      SendMessage(hWndStatusBar,SB_SETTEXT, 3|SBT_POPOUT,(LPARAM)"SCRL");
}

VOID MainWindow :: SetNumLock(VOID)
{
    if (GoldKey)
    {
       BYTE KeyState[256];
       GetKeyboardState((LPBYTE) &KeyState);
       KeyState[144] = 1;
       SetKeyboardState((LPBYTE) &KeyState);
    }
}

VOID MainWindow :: ResetNumLock(VOID)
{
    if (GoldKey)
    {
       BYTE KeyState[256];
       GetKeyboardState((LPBYTE) &KeyState);
       KeyState[144] = OldNumLockStatus;
       SetKeyboardState((LPBYTE) &KeyState);
    }
}

VOID MainWindow :: SaveOldNumLockStatus(void)
{
    if (GoldKey)
    {
       BYTE KeyState[256];
       GetKeyboardState((LPBYTE) &KeyState);
       OldNumLockStatus = KeyState[144];
    }
}

BOOL MainWindow :: WmNewKeyDown ( WPARAM Key, LPARAM Scan )
{
// DispObjectMsg("%d %ld %ld",Key,Scan&0x0100,Scan&0x00ff);
   if (GetKeyState(VK_CONTROL) < 0)  // control key pressed
   {  switch (Key)
      {
         case 'C':
         case 'Y': WmClose(); return FALSE;
         case 'Z': ExecCommand(); return FALSE;
         default:  return TRUE;
      }
   }
   else if ((Scan & 0x0100) == 256)
   {
      switch (Scan & 0x00ff)
      {
         case 69: // NumLock : GoldKey
            SetNumLock();
#ifdef WIN95
	    SetStatusButtons();
#endif
            GoldPressed = TRUE;
            return FALSE;    // swallow
         case 28:            // Num Enter
            if (GoldPressed)
            {  GoldPressed = FALSE;
               SendMessage(this->GetHandle(),WM_COMMAND,IDM_REPLACE,0L);
            }
            else
            {
            }
            return FALSE;    // swallow
         case 53:            // Num /
            return FALSE;    // swallow
         case 71:            // home
         case 72:            // arrow up
         case 73:            // page up
         case 75:            // arrow left
         case 77:            // arrow right
         case 79:            // end
         case 80:            // arrow down
         case 81:            // page down
	    SetTimer(this->GetHandle(),3,1,NULL);  // ---> action see WmTimer()
//          DisplayLineNumber();  // must be delayed
            return TRUE;
	 case 83:	     // Delete  
            SendMessage(this->GetHandle(),WM_COMMAND,IDM_CUT,0L);
            return FALSE;    // swallow
         default:
            return TRUE;     // transfer to edit
      }
   }
   else if ((Scan & 0x0100) == 0)
   {  HWND hWnd = opEdit->GetHandle();
      switch (Scan & 0x00ff)
      {
	 case 58:    // CapsLock
         case 70:    // ScrollLock
#ifdef WIN95
	    SetStatusButtons();
#endif
            return FALSE;    // swallow
         case 14:  // Backspace
         case 28:  // Enter
            DisplayLineNumber();
            return TRUE;
         case 82:  // Num 0 : Home cursor and step one line forward or backward
            if (GoldPressed)
            {  GoldPressed = FALSE;
               SendMessage(hWnd,WM_KEYDOWN,0xD,0x001C0001L);  // CR key
               SendMessage(hWnd,WM_CHAR,0xD,0x001C0001L);
               SendMessage(hWnd,WM_KEYUP,0xD,0xC01C0001L);
               SendMessage(hWnd,WM_KEYDOWN,0x26,0x01480001L); // Up key
               SendMessage(hWnd,WM_KEYUP,0x26,0xC1480001L);
            }
            else
            {  SendMessage(hWnd,WM_KEYDOWN,0x24,0x01470001L);  // Home key
               SendMessage(hWnd,WM_KEYUP,0x24,0xC1470001L);
               if (Forward)
               {  SendMessage(hWnd,WM_KEYDOWN,0x28,0x01500001L); // Down key
                  SendMessage(hWnd,WM_KEYUP,0x28,0xC1500001L);
               }
               else
               {  SendMessage(hWnd,WM_KEYDOWN,0x26,0x01480001L); // Up key
                  SendMessage(hWnd,WM_KEYUP,0x26,0xC1480001L);
               }
            }
            return FALSE;    // swallow
         case 79:            // Num 1
            if (GoldPressed)   // delete to EOL
            {  GoldPressed = FALSE;
            }
            else
               StepWord();
            return FALSE;    // swallow
         case 80:            // Num 2
            if (GoldPressed)   // delete to EOL
            {  GoldPressed = FALSE;
               DeleteToEndOfLine();
            }
            else
            {  if (Forward)    // go to EOL
               {  SendMessage(hWnd,WM_KEYDOWN,0x23,0x014F0001L);
                  SendMessage(hWnd,WM_KEYUP,0x23,0xC14F0001L);
               }
               else            // go to BOL
               {  SendMessage(hWnd,WM_KEYDOWN,0x24,0x01470001L); // Home
                  SendMessage(hWnd,WM_KEYUP,0x24,0xC1470001L);
               }
            }
            return FALSE;    // swallow
         case 81:            // Num 3
            if (GoldPressed)
            {  GoldPressed = FALSE;
               Specins();
            }
            else
            {
            }            
            return FALSE;    // swallow
         case 75:            // Num 4
            if (GoldPressed)
            {  GoldPressed = FALSE;
               End();
            }
            else
               Forward = TRUE;  // direction is forward
            return FALSE;    // swallow
         case 76:            // Num 5
            if (GoldPressed)
            {  GoldPressed = FALSE;
               Home();
            }
            else
               Forward = FALSE;  // direction is backward
            return FALSE;    // swallow
         case 77:            // Num 6
            if (GoldPressed)
            {  GoldPressed = FALSE;
               SendMessage(this->GetHandle(),WM_COMMAND,IDM_PASTE,0L);
            }
            else
            {
               SendMessage(this->GetHandle(),WM_COMMAND,IDM_CUT,0L);
            }
            return FALSE;    // swallow
         case 71:            // Num 7
            if (GoldPressed)
            {  GoldPressed = FALSE;
               ExecCommand();
            }
            else
	    {  if (Forward)
                  End(); // EOF
               else
                  Home();  // BOF
	       opEdit->SetFocus();
            }
            return FALSE;    // swallow
         case 72:            // Num 8
            return FALSE;    // swallow
         case 73:            // Num 9
            return FALSE;    // swallow
         case 55: // Num * : Find and Find next
            if (GoldPressed)
            {  GoldPressed = FALSE;
               SendMessage(this->GetHandle(),WM_COMMAND,IDM_FIND,0L);
            }
            else
               SendMessage(this->GetHandle(),WM_COMMAND,IDM_FIND_NEXT,0L);
            return FALSE;    // swallow
         case 78:            // Num +
            if (GoldPressed)
            {  GoldPressed = FALSE;
               UndeleteWord();
            }
            else
            {  DeleteWord();
            }
            return FALSE;    // swallow
         case 74:            // Num -
            if (GoldPressed)
            {  GoldPressed = FALSE;
               UndeleteLine();
            }
            else
            {  DeleteLine();
            }
            return FALSE;    // swallow
         case 83:            // Num .
            if (GoldPressed)
            {  GoldPressed = FALSE;
               int start,end;
               opEdit->GetCrntSelection(start,end);
               opEdit->SetCrntSelection(end,end);
               StartMark = -1;
               ReleaseShift();
            }
            else
            {
               int start,end;
               opEdit->GetCrntSelection(start,end);
               StartMark = end;
               SetShift();
            }
            return FALSE;    // swallow
         default:
            return TRUE;     // transfer to edit
      }
   }
   else
      return TRUE;     // transfer to edit
}

BOOL MainWindow :: WmNewKeyUp ( WPARAM Key, LPARAM Scan )
{
// suppress edit control's messagebeep from pressing Ctrl_C etc.

   if (GetKeyState(VK_CONTROL) < 0)  // control key pressed
   {  switch (Key)
      {
         case 'C':
         case 'Z':
         case 'Y': return FALSE;
         default:  return TRUE;
      }
   }
   else if ((Scan & 0x0100) == 256)
   {
      switch (Scan & 0x00ff)
      {
         case 69:   	     // NumLock
         case 28:            // Num Enter
         case 53:            // Num /
            return FALSE;    // swallow
         case 71:            // home
         case 72:            // arrow up
         case 73:            // page up
         case 75:            // arrow left
         case 77:            // arrow right
         case 79:            // end
         case 80:            // arrow down
         case 81:            // page down
            {
               if (StartMark != -1)  // keep selection on when key released
               {  int  start, end;
                  opEdit->GetCrntSelection(start,end);
                  opEdit->SetCrntSelection(StartMark,end);
               }
            }
            return TRUE;
         default:
            return TRUE;     // transfer to edit
      }
   }
   else if ((Scan & 0x0100) == 0)
   {
      switch (Scan & 0x00ff)
      {
	 case 58:    // CapsLock
         case 70:    // ScrollLock
         case 82:    // Num 0
         case 79:    // Num 1
         case 80:    // Num 2
         case 81:    // Num 3
         case 75:    // Num 4
         case 76:    // Num 5
         case 77:    // Num 6
         case 71:    // Num 7
         case 72:    // Num 8
         case 73:    // Num 9
         case 55:    // Num *
         case 78:    // Num +
         case 74:    // Num -
         case 83:    // Num .
           return FALSE;   // swallow
         default:
           return TRUE;    // transfer to edit
      }
   }
   else
      return TRUE;    // transfer to edit
}

BOOL MainWindow :: WmNewChar ( WPARAM Key, LPARAM Scan)
{
// suppress edit control's messagebeep from pressing Ctrl_C etc.

   if (GetKeyState(VK_CONTROL) < 0)  // control key pressed
   {  switch (Key + 64)
      {
         case 'C':
         case 'Z':
         case 'Y': return FALSE;
         default:  return TRUE;
      }
   }
   else if ((Scan & 0x0100) == 256)
   {
      switch (Scan & 0x00ff)
      {
         case 69:    // NumLock
         case 28:    // Num Enter
         case 53:    // Num /
           return FALSE;   // swallow
         default:
           return TRUE;    // transfer to edit
      }
   }
   else if ((Scan & 0x0100) == 0)
   {
      switch (Scan & 0x00ff)
      {
	 case 58:    // CapsLock
         case 70:    // ScrollLock
         case 82:    // Num 0
         case 79:    // Num 1
         case 80:    // Num 2
         case 81:    // Num 3
         case 75:    // Num 4
         case 76:    // Num 5
         case 77:    // Num 6
         case 71:    // Num 7
         case 72:    // Num 8
         case 73:    // Num 9
         case 55:    // Num *
         case 78:    // Num +
         case 74:    // Num -
         case 83:    // Num .
           return FALSE;   // swallow
         default:
           return TRUE;    // transfer to edit
      }
   }
   else
      return TRUE;    // transfer to edit
}

BOOL MainWindow :: WmNewLeftMouseDown ( void )
{
    MouseDown = TRUE;
    SetTimer(this->GetHandle(),1,50,NULL);  // ---> action see WmTimer()
    return TRUE;    // transfer to edit
}

BOOL MainWindow :: WmNewLeftMouseUp ( void )
{
    MouseDown = FALSE;
    return TRUE;    // transfer to edit
}

BOOL MainWindow :: WmNewMouseMove ( void )
{
    if (MouseDown)
       DisplayLineNumber();
    return TRUE;    // transfer to edit
}

BOOL MainWindow :: WmNewPaint ( void )
{
    if (MouseDown)
       DisplayLineNumber();
    return TRUE;    // transfer to edit
}

BOOL MainWindow :: Adjust(UINT Code)
{
    while (!done)   // wait until previous action comleted
      MyYield();
    done = FALSE;   // block ---> freed in WmTimer()
    OffsetFromTop = LineNumber - 1 -
              (int)SendMessage(opEdit->GetHandle(),EM_GETFIRSTVISIBLELINE,0,0L);
    SaveCode = Code;
    SetTimer(this->GetHandle(),2,50,NULL);  // ---> action see WmTimer()
    return TRUE;    // transfer to edit
}

LONG APIENTRY MyEditProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
   BOOL retval; // if FALSE, then edit control's original WndProc is not called

// Get stored object pointer to parent window (done with Window::SetWinPointer).

   MainWindow *opWin = (MainWindow *) GetWindowLong(GetParent(hWnd),0);

   if (opWin)
   {
     switch (iMessage)
     {
        case WM_CHAR:
          retval = opWin->WmNewChar(LOWORD(wParam), HIWORD(lParam)); break;
        case WM_KEYUP:
          retval = opWin->WmNewKeyUp(LOWORD(wParam), HIWORD(lParam)); break;
        case WM_KEYDOWN:
          retval = opWin->WmNewKeyDown(LOWORD(wParam), HIWORD(lParam)); break;
        case WM_LBUTTONDOWN:
          retval = opWin->WmNewLeftMouseDown(); break;
        case WM_LBUTTONUP:
          retval = opWin->WmNewLeftMouseUp(); break;
        case WM_MOUSEMOVE:
          retval = opWin->WmNewMouseMove(); break;
        case WM_PAINT:
          retval = opWin->WmNewPaint(); break;
        case WM_VSCROLL:
#ifdef WIN32
          retval=opWin->Adjust(LOWORD(wParam)); break;
#else
          retval=opWin->Adjust(wParam); break;
#endif
          default: retval = TRUE; break;
     }

     if (retval)
        return CallWindowProc(opWin->GetlpEditProc(),hWnd,iMessage,wParam,lParam);
     else
        return FALSE;
   }
   else
      return FALSE;
}

#ifdef WIN95

static TBBUTTON tbButtons[] =
{
	{ 0, IDM_NEW,        TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 1, IDM_OPEN,       TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        { 2, IDM_SAVE,       TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 0, 0,              TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
        { 3, IDM_CUT,        TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 4, IDM_COPY,       TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 5, IDM_PASTE,      TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 0, 0,              TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
	{ 6, IDM_PRINT,      TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 0, 0,              TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
        { 7, IDM_FIND,       TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        { 8, IDM_FIND_NEXT,  TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 9, IDM_REPLACE,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 0, 0,              TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
        {10, IDM_BEGIN,      TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        {11, IDM_END,        TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        {12, IDM_EXIT,       TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 0, 0,              TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
        {13, IDM_ABOUT,      TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0}
};

static int nCust[] = {0,1,2, 4,5,6, 8, 10,11,12, 14,15,16, 18, -1};

static char szTbStrings[] = "New\0Open\0Save\0"
			    "Cut\0Copy\0Paste\0"
			    "Print\0"
			    "Find\0FindNext\0Replace\0"
			    "Home\0EOF\0Quit\0"
			    "About\0";

LPSTR GetString(int iString)
{
   int i, cb;
   LPSTR pString;

   pString = szTbStrings;
   for (i=0; i<iString; i++)
   {
	cb = lstrlen(pString);
	pString += (cb+1);
   }
   return pString;
}

void MainWindow :: ApplicationBar(void)
{
  hWndToolbar = CreateToolbarEx( 
    this->GetHandle(),
    WS_CHILD | WS_BORDER | WS_VISIBLE | TBSTYLE_TOOLTIPS | CCS_ADJUSTABLE,
    ID_TOOLBAR,
    14,
    Application::GetInstance(),
    IDB_TOOLBAR,
    (LPCTBBUTTON)&tbButtons,
    19,
    16,16,
    16,16,
    sizeof(TBBUTTON)
    );
}

LONG MainWindow :: WmNotify (LPARAM lParam)
{
    static CHAR szBuf[128];

    switch (((LPNMHDR)lParam)->code)
    {
	case TTN_NEEDTEXT:
	{
	  LPTOOLTIPTEXT lpToolTipText = (LPTOOLTIPTEXT)lParam;
	  LoadString(Application::GetInstance(),lpToolTipText->hdr.idFrom,
	             szBuf,sizeof(szBuf));
          lpToolTipText->lpszText = szBuf;
	  return 0L;
	}

	case TBN_GETBUTTONINFO:
	{
	  LPTBNOTIFY ptbn = (LPTBNOTIFY)lParam;
	  int iButton = nCust[ptbn->iItem];
	  if (iButton != -1)
	  {
	    lstrcpy(ptbn->pszText, GetString(ptbn->iItem));
	    memcpy(&ptbn->tbButton,&tbButtons[iButton],sizeof(TBBUTTON));
	    return TRUE;
	  }
	  else
	    return FALSE;
	}

	case TBN_QUERYDELETE:
	  return TRUE;

	case TBN_QUERYINSERT:
	  return TRUE;

	case TBN_CUSTHELP:
	  return 0L;

	case TBN_TOOLBARCHANGE:
	  SendMessage(hWndToolbar, TB_AUTOSIZE, 0L, 0L);
	  return 0L;

	default:
	  return TRUE;
    }
}

LONG MainWindow :: WmLeftButtonDblclk (POINT &)
{
    SendMessage(hWndToolbar,TB_CUSTOMIZE, 0L, 0L);
    return 0L;
}

#endif

///////////////////////////////////////////////////////////////////////////////

UINT Application :: Run(VOID)
{
   strcpy(CMDLINE,lpszCmdLine);
   int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res
   RECT WinDim = {0, 0, (n*640)/4, (n*480)/4}; // x,y,w,h
   opMain = new MainWindow( nCmdShow, "Editor32", WinDim );
   return( Start() );
}
/////////////////////////////////// EOF ///////////////////////////////////////
