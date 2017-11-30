///////////////////////////////////////////////////////////////////////////////
// TPMENU.CPP: Urs Rohrer (PSI), March 1992
// New: (September 1996) Camac-I/O can be done in 2 ways.
// 1.) if 2nd parameter is COM1: or COM2:, then CAMAC(32).DLL is loaded
//     dynamically and I/O is done directly.
// 2.) if 2nd parameter is pc310$10000 or something similar, then I/O is
//     done via a Camac-server program hosted on pc310 and working via
//     TCP/IP-port 10000.
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

#ifdef WIN32
#ifndef WIN95
#define WINNT
#endif
#endif

void WWW1(void)
{
   if ((int)(ShellExecute(NULL,NULL,
		"http://people.web.psi.ch/rohrer_u/trans.htm",
		NULL,NULL,SW_SHOWNORMAL)) <= 32)
   {
	system("WWW.cmd");  // for Bart PE
   }
}

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

#ifdef WIN95

/****************************************************************************
 *									    *
 *  FUNCTION   : MakeDIBPalette(lpInfo)					    *
 *									    *
 *  PURPOSE    : Given a BITMAPINFOHEADER, create a palette based on 
 *		 the color table.
 *		 
 *									    *
 *  RETURNS    : non-zero - handle of a corresponding palette 
 *		 zero - unable to create palette
 *									    *
 ****************************************************************************/

HPALETTE MakeDIBPalette(LPBITMAPINFOHEADER lpInfo, WORD *offBits)
{
    NPLOGPALETTE npPal;
    RGBQUAD far *lpRGB;
    HPALETTE hLogPal; 
    WORD i;

    /* since biClrUsed field was filled during the loading of the DIB,
    ** we know it contains the number of colors in the color table.
    */
    if (lpInfo->biClrUsed)
    {
	npPal = (NPLOGPALETTE)LocalAlloc(LMEM_FIXED, sizeof(LOGPALETTE) + 
				(WORD)lpInfo->biClrUsed * sizeof(PALETTEENTRY));
	if (!npPal)
	    return(FALSE);

	npPal->palVersion = 0x300;
        npPal->palNumEntries = (WORD)lpInfo->biClrUsed;

	/* get pointer to the color table */
	lpRGB = (RGBQUAD FAR *)((LPSTR)lpInfo + lpInfo->biSize);

        /* offset to the bits from start of DIB header */
        *offBits = (WORD)(lpInfo->biSize + 256 * sizeof(RGBQUAD));


	/* copy colors from the color table to the LogPalette structure */
        for (i = 0; i < (WORD)lpInfo->biClrUsed; i++, lpRGB++)
	{
	    npPal->palPalEntry[i].peRed = lpRGB->rgbRed;
	    npPal->palPalEntry[i].peGreen = lpRGB->rgbGreen;
	    npPal->palPalEntry[i].peBlue = lpRGB->rgbBlue;
	    npPal->palPalEntry[i].peFlags = 0;
	}

	hLogPal = CreatePalette((LPLOGPALETTE)npPal);
	LocalFree((NPLOGPALETTE)npPal);
	return(hLogPal);
    }

    /* 24-bit DIB with no color table.  return default palette.  Another
    ** option would be to create a 256 color "rainbow" palette to provide
    ** some good color choices.
    */
    else
	return((HPALETTE)GetStockObject(DEFAULT_PALETTE));
}

/****************************************************************************
 *									    *
 *  FUNCTION   : MakeIndexHeader(lpInfo)				    *
 *									    *
 *  PURPOSE    : Given a BITMAPINFOHEADER, create a new info header 
 *		 using the DIB_PAL_COLORS format.
 *									    *
 *  RETURNS    : non-zero - global handle of a new header
 *		 zero - unable to create new header
 *									    *
 ****************************************************************************/

HGLOBAL MakeIndexHeader(LPBITMAPINFOHEADER lpInfo)
{
    HGLOBAL hPalInfo;
    LPBITMAPINFOHEADER lpPalInfo;
    WORD FAR *lpTable;
    WORD i;

    if (lpInfo->biClrUsed)
    {
	hPalInfo = GlobalAlloc(GMEM_MOVEABLE, lpInfo->biSize +
					lpInfo->biClrUsed * sizeof(WORD));
	if (!hPalInfo)
	    return(NULL);
	lpPalInfo = (LPBITMAPINFOHEADER)GlobalLock(hPalInfo);

	*lpPalInfo = *lpInfo;
	lpTable = (WORD FAR *)((LPSTR)lpPalInfo + lpPalInfo->biSize);

        for (i = 0; i < (WORD)lpInfo->biClrUsed; i++)
	    *lpTable++ = i;

	GlobalUnlock(hPalInfo);
	return(hPalInfo);
    }
    else
	return(NULL);
}
#endif

///////////////////////////////////////////////////////////////////////////////

long GetDacValue(SOCKET sock, char *Name)
{
   char sendbuff[100], recbuff[2048];
   char *ptr;

   sprintf(sendbuff,"RDAC %s",Name);
   TCPSend(sock,sendbuff,recbuff);

   ptr = strtok(recbuff,"=");
   if (ptr)
   {
      ptr = strtok(NULL,"=");
      if (ptr)
         return (atol(ptr));
      else
         return 0L;
   }
   else
      return 0L;
}

int SetDacValue(SOCKET sock, char *Name, long Value)
{
   char sendbuff[100], recbuff[2048];

   sprintf(sendbuff,"WDAC %s %ld",Name,Value);
   TCPSend(sock,sendbuff,recbuff);

   if (strcmp(recbuff,"*WDAC* error") == 0)
      return 0;
   else
      return 1;
}

//////////////////////////////// Global ////////////////////////////////////

#ifndef WIN95
static PushButton *opExit = NULL;
#endif

int errorflag = 0;
int homeit = 1;

// Globals for Ellipses()

struct wpos {
		  int pos1;
		  int pos2;
		  BOOL inuse;
	    } ypos[] = {0,24,false, 28,47,false, 56,69,false};

int jp1 = -1;
int jp2 = -1;
int jp3 = -1;
BOOL busy  = false;
BOOL busy2 = false;
BOOL busy3 = false;

///////////////////// CMDLINE passed from Application ///////////////////////

char CMDLINE[80], SCRDIR[80], PORT[80], COLFLAG[10], PRTCOLFLAG[10];

extern "C"
{
//#ifdef WIN95
//#ifdef __BORLANDC__
   short APIENTRY TRNPRT(short *, LPSTR);
//#else
//   long __stdcall TRANS(int *MODE, char *CMDLINE, int len); // LF90-DLL
//#endif
//#else
//   short APIENTRY TRNPRT(short *, LPSTR);
//#endif
}

///////////////////////////////////////////////////////////////////////////////

#ifdef WIN32

void RunInConsoleWindow(char *PROG, char *name, int x, int y, int w, int h)
{
    DWORD dwCode;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    si.cb          = sizeof( STARTUPINFO );
    si.lpReserved  = NULL;
    si.lpDesktop   = NULL;
    si.lpTitle     = name;
    si.dwX         = x;
    si.dwY         = y;
    si.dwXSize     = w;
    si.dwYSize     = h;
    si.dwXCountChars   = 133;
    si.dwYCountChars   = 999;
    si.dwFillAttribute = BACKGROUND_BLUE |
                         BACKGROUND_GREEN |
                         BACKGROUND_INTENSITY;
    si.dwFlags     =
                     STARTF_USESHOWWINDOW |
                     STARTF_USEPOSITION |
                     STARTF_USEFILLATTRIBUTE |
		     STARTF_USECOUNTCHARS |
                     STARTF_USESIZE;
    si.wShowWindow = SW_SHOWDEFAULT;
    si.cbReserved2 = 0;
    si.lpReserved2 = NULL;
    si.hStdInput   = 0;
    si.hStdOutput  = 0;
    si.hStdError   = 0;
    CreateProcess(
         NULL,
         PROG,
         (LPSECURITY_ATTRIBUTES) NULL,
         (LPSECURITY_ATTRIBUTES) NULL,
         FALSE,
         NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
         (LPVOID) NULL,
         (LPTSTR) NULL,
         &si,
         &pi);

         HANDLE hw = OpenProcess (PROCESS_QUERY_INFORMATION,TRUE,pi.dwProcessId);

    do {
          wait(200);
          GetExitCodeProcess(hw,&dwCode);
       } while (dwCode == STILL_ACTIVE) ;

	 CloseHandle(hw);
}

#endif

///////////////////////////////////////////////////////////////////////////////

#ifndef WIN32

WORD HeapWalk(HANDLE hinst)
{
     LOCALINFO  localinfo;
     LOCALENTRY localentry;

     WORD wHeapMoveable=0;
     WORD wHeapFixed=0;
     WORD wHeapFree=0;
     WORD wUnused=0;
     WORD wSize=0;

     // walk the local heap

     localinfo.dwSize = sizeof (LOCALINFO);
     LocalInfo (&localinfo, hinst);

     localentry.dwSize = sizeof (LOCALENTRY);
     if ( LocalFirst (&localentry, hinst) )
     {
          do
          {
            if (localentry.wFlags & LF_FREE)
                 wHeapFree     += localentry.wSize;

            else if (localentry.wFlags & LF_FIXED)
                 wHeapFixed    += localentry.wSize;

            else if (localentry.wFlags & LF_MOVEABLE)
                 wHeapMoveable += localentry.wSize;
          }
          while (LocalNext (&localentry));
     }

     /*
      *  At this point, heap traversal is done.
      *  However, the heap can grow until the size of DS is 64K (0xFFFF).
      *  Determine how many additional Kbytes the heap can grow.
      *
      */

     wSize   = (WORD) GlobalSize (hinst);
     wUnused = (0xFFFF - wSize);

     return (wUnused + wHeapFree);
}

#endif


///////////////////////// class MainWindow implementation /////////////////////////

MainWindow :: MainWindow(int nCmdShow, LPCSTR szWindowName, RECT rc)
            : Window( nCmdShow, szWindowName, rc)
{

//  Get current disk and directory (use simple DOS functions)

#ifdef __BORLANDC__
    SDISK = getdisk();
    getcurdir(0,SDIR);
#elif __WATCOMC__
    getcwd(SDIR,40);
    SDISK = (int)(SDIR[0]-'A');
#else
    SDISK = _getdrive();
    _getdcwd(0,SDIR,40);
#endif
  
    NullStr[0] = '\0';
    CStr[0]    = 'C';  CStr[1]  = '\0';
    Blank[0]   = ' ';  Blank[1] = '\0';
    nCStr[0]   = 'n';  nCStr[1] = 'C';   nCStr[2] = '\0';

    hWndToolbar = NULL;
    hWnd = this->GetHandle();

    // Load and initialize main menu

    opMenu = new Menu(IDR_MAIN_MENU,this);
    {
      char answer[20];
      GetPrivateProfileString("Editor","Gui","NO",answer,20,"Transprt.ini");
      edtgui = FALSE;
      if (answer[0] == 'Y')
         edtgui = TRUE;
      if (edtgui)
      {
         opMenu->SetCheck(IDM_EDIT_GUI,TRUE);
         opMenu->SetCheck(IDM_EDIT_TEXT,FALSE);
      }
      else
      {
         opMenu->SetCheck(IDM_EDIT_GUI,FALSE);
         opMenu->SetCheck(IDM_EDIT_TEXT,TRUE);
      }
    }
    opSysMenu = new SystemMenu(this);
    opSysMenu->AppendSep();
    opSysMenu->Append(IDM_ABOUT,"&About this Transport Framework...");

#ifdef WIN95
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
    GetVersionEx(&osvi);
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
     dpmirun = FALSE;
    else
     dpmirun = TRUE;
#else
    dpmirun = FALSE;
#endif
    DPMICheck();

//  Assign strings associated with scratch directory and Serial Port
//  for CAMAC communication (passed via command line)
//  Scratch Directory is normally a RAM DISK (e.g F:\)

    if (!strlen(CMDLINE)) 
    {
#ifdef __BORLANDC__
       CMDLINE[0] = char('A' + SDISK);
       strcat(CMDLINE,":\\");
       strcat(CMDLINE,SDIR);
#else
       strcpy(CMDLINE,SDIR);
#endif
       if (CMDLINE[strlen(CMDLINE)-1] != '\\')
          strcat(CMDLINE,"\\");
       if (strchr(SDIR,' ')) // Directory contains Blank(s)
          strcat(CMDLINE,String(IDS_CMD_LINE2));
       else
          strcat(CMDLINE,String(IDS_CMD_LINE));
//       DispObjectMsg(String(IDS_NOPAR),CMDLINE);
	char tempx[512];
	strcpy(tempx,"Abnormal start-up:\nNone of the 4 needed parameters\n");
	strcat(tempx,"passed via the command line.\nDefaults '");
	strcat(tempx,CMDLINE);
	strcat(tempx,"' taken.\n\nHint for proper start-up:\n");
	strcat(tempx,"Do not double-click at the TPMENU32.EXE icon.\n");
	strcat(tempx,"Use shortcut generated during installation instead.\n");
	strcat(tempx,"By default it is in the APPS folder.");
	MessageBox(NULL,tempx,"Warning issued by Tranport Framework",MB_OK);
    }

    char *p;

    p = strchr(CMDLINE,'+'); // if separator equal '+' ..
    if (p)
       Blank[0] = '+';       // ... then replace Blank

    p = strtok(CMDLINE,Blank);
    if (p)
    {
       strcpy(SCRDIR,p);
       if (SCRDIR[strlen(SCRDIR)-1] != '\\')
          strcat(SCRDIR,"\\");
    }
    p = strtok(NULL,Blank);
    if (p)
    {   strcpy(PORT,p);
        p = strtok(NULL,Blank);
        if (p)
        {  strcpy(COLFLAG,p);
           p = strtok(NULL,Blank);
           if (p)
              strcpy(PRTCOLFLAG,p);
           else
           {  strcpy(PRTCOLFLAG,nCStr);
              DispObjectMsg(String(IDS_NOPARP));
           }
        }
        else
        {  strcpy(COLFLAG,CStr);
           strcpy(PRTCOLFLAG,nCStr);
           DispObjectMsg(String(IDS_NOPARC));
        }
    }
    else
    {  strcpy(PORT,"COM1:");
       strcpy(COLFLAG,CStr);
       strcpy(PRTCOLFLAG,nCStr);
       DispObjectMsg(String(IDS_NOPORT));
    }

    char plwstr[10];
    GetPrivateProfileString("Printing","line_width","1",plwstr,10,
			   "Transprt.ini");
    PLW = atoi(plwstr); // print line width

    AnsiUpperBuff( SCRDIR,strlen(SCRDIR));

    strcpy(for000, SCRDIR);
    strcat(for000, String(IDS_FOR000));

    strcpy(for001, SCRDIR);
    strcat(for001, String(IDS_FOR001));

    strcpy(for002, SCRDIR);
    strcat(for002, String(IDS_FOR002));

    strcpy(for004, SCRDIR);
    strcat(for004, String(IDS_FOR004));

    strcpy(save0,  SCRDIR);
    strcat(save0,  String(IDS_SAVE0D));

    strcpy(keep0,  SCRDIR);
    strcat(keep0,  String(IDS_KEEP0D));

    ColorButtonWndFn(this->GetHandle(),0,0,0L);  // initialize COLORBUT.DLL

#ifdef WIN95
    ApplicationBar();
#endif

    Invalidate();
}

long MainWindow :: WmSysMenuCommand(WPARAM MenuItem)
{
//  Handle system menu command events.

    switch (MenuItem)
    {   case IDM_ABOUT:
	{
 	   SimpleDialog(IDD_ABOUT_DIALOG,this);
           SimpleDialog(IDD_ABOUT2_DIALOG,this);
           SimpleDialog oDg(IDD_ABOUT3_DIALOG,this);
           if (!oDg.GetResult()) 
	   {
		WWW1();
	   }
	   else
	   {
		RECT rc, WinDim;

		GetWindowRect(this->GetHandle(),&rc);
		int w = 342;
		int h = 250;
		int x = rc.left + (rc.right-rc.left) / 2 - w / 2;  // center child window
		int y = rc.top + (rc.bottom-rc.top) / 2 - h / 2;
		WinDim.left = x;
		WinDim.top  = y;
		WinDim.right = w;
		WinDim.bottom = h;
		new PopupWindow(SW_SHOWNORMAL,"Legal Disclaimer","myLegalMess",WinDim);
	   }
           return TRUE;
	}

        default:
           return DefaultEvent();
    }
}

#ifndef WIN95

long MainWindow :: WmSize( int width, int )
{
    if (opExit)
       delete opExit;
    opExit = new PushButton((int)IDM_EXIT,this,width-40,0,40,25,"Exit");
    return DefaultEvent();
}

#else

long MainWindow :: WmSize( int, int )
{
    if (hWndToolbar)
       SendMessage(hWndToolbar, TB_AUTOSIZE, 0L, 0L);
    return DefaultEvent();
}

#endif


long MainWindow :: WmMenuCommand( WPARAM mid )
{
    switch (mid)
    {   case IDM_COMMAND: EBox('\0'); break;
        case IDM_A_KEY:   EBox('A'); break;
        case IDM_B_KEY:   EBox('B'); break;
        case IDM_C_KEY:   EBox('C'); break;
        case IDM_D_KEY:   EBox('D'); break;
        case IDM_E_KEY:   EBox('E'); break;
        case IDM_F_KEY:   EBox('F'); break;
        case IDM_G_KEY:   EBox('G'); break;
        case IDM_I_KEY:   EBox('I'); break;
        case IDM_L_KEY:   EBox('L'); break;
        case IDM_O_KEY:   EBox('O'); break;
        case IDM_P_KEY:   EBox('P'); break;
        case IDM_Q_KEY:   EBox('Q'); break;
        case IDM_R_KEY:   EBox('R'); break;
        case IDM_S_KEY:   EBox('S'); break;
        case IDM_T_KEY:   EBox('T'); break;
        case IDM_1_KEY:   EBox('1'); break;
        case IDM_3_KEY:   EBox('3'); break;
        default: ExecuteCommand(mid,NullStr); break;
    }
    return FALSE;
}

VOID MainWindow :: ExecuteCommand(WPARAM mid, LPSTR command)
{
#ifdef __BORLANDC__
    setdisk(SDISK);
    chdir(SDIR);
#elif __WATCOMC__
    chdir(SDIR);
#else
    _chdrive(SDISK);
    _chdir(SDIR);
#endif

    switch (mid)
    {
	case IDM_WWW:
	  WWW1();
	  break;
        case IDM_ABOUT:
          {
 	    SimpleDialog(IDD_ABOUT_DIALOG,this);
            SimpleDialog(IDD_ABOUT2_DIALOG,this);
            SimpleDialog oDg(IDD_ABOUT3_DIALOG,this);
            if (!oDg.GetResult()) 
	    {
		WWW1();
	    }
	    else
	    {
		RECT rc, WinDim;

		GetWindowRect(this->GetHandle(),&rc);
		int w = 342;
		int h = 250;
		int x = rc.left + (rc.right-rc.left) / 2 - w / 2;  // center child window
		int y = rc.top + (rc.bottom-rc.top) / 2 - h / 2;
		WinDim.left = x;
		WinDim.top  = y;
		WinDim.right = w;
		WinDim.bottom = h;
		new PopupWindow(SW_SHOWNORMAL,"Legal Disclaimer","myLegalMess",WinDim);
	    }
          }
	  break;
        case IDM_EXIT: WmClose(); break;
        case IDM_EDT: Edit(command); break;
	case IDM_EDIT_GUI:
	  {
	       opMenu->SetCheck(IDM_EDIT_GUI,TRUE);
	       opMenu->SetCheck(IDM_EDIT_TEXT,FALSE);
	       edtgui = TRUE;
	       char par[4] = "YES";
               WritePrivateProfileString("Editor","Gui",par,"Transprt.ini");
	  }
	  break;
	case IDM_EDIT_TEXT:
	  {
	       opMenu->SetCheck(IDM_EDIT_GUI,FALSE);
	       opMenu->SetCheck(IDM_EDIT_TEXT,TRUE);
	       edtgui = FALSE;
	       char par[4] = "NO";
               WritePrivateProfileString("Editor","Gui",par,"Transprt.ini");
	  }
	  break;
        case IDM_EDT_INP:
	  {
	     char command[80];

	     if (edtgui)
	     {
#ifdef WIN95
		strcpy(command,"tgie32.exe ");
		strcat(command,SCRDIR);
#else
		strcpy(command,"tgie.exe ");
		strcat(command,SCRDIR);
#endif
	     }
	     else
	     {
#ifdef WIN95
		strcpy(command,"editor32.exe ");
		strcat(command,for001);
#else
		strcpy(command,"editor.exe ");
		strcat(command,for001);
#endif
	     }
	     WinExec(command,SW_NORMAL);
	  }
	  break;
        case IDM_BLI:
	  {
	     char PROG[100];
	     strcpy(PROG,"BLI32 ");
	     strcat(PROG,SCRDIR);
             RunInConsoleWindow(PROG," Beam Line Inversion ", 0, 0, 624, 420); break;
	  }
	  break;
	case IDM_GUI:
	  {
	    char command[80];
#ifdef WIN95
	    strcpy(command,"tgie32.exe ");
#else
	    strcpy(command,"tgie.exe ");
#endif
	    strcat(command,SCRDIR);
	    WinExec(command,SW_SHOW);
	  }
	  break;
	case IDM_DISPAR: DisplayParameters();	break;
        case IDM_IFQ:
        {
#ifdef WIN32
#ifdef WIN95
	   char PROG[100];
	   strcpy(PROG,"IFQ32 ");
	   strcat(PROG,SCRDIR);
           RunInConsoleWindow(PROG," Transport Input Editor (Version Q) ", 0, 0, 624, 420); break;
#else
	   char PROG[100];
	   strcpy(PROG,"IFQ ");
	   strcat(PROG,SCRDIR);
           RunInConsoleWindow(PROG," Transport Input Editor (Version Q) ", 0, 0, 624, 420); break;
#endif
#else
	   WinExec("ifq",SW_SHOW);
	   HWND ha, ht = this->GetHandle();

	   do {
		wait(100);
		ha = GetActiveWindow();  // wait until tty-window is active
	      }  while (ha == ht);

	   HWND hp = GetNextWindow(ha,GW_HWNDPREV);
	   SetWindowPos(ha,hp,10,10,600,400,SWP_SHOWWINDOW);
	   break;
#endif
        }
        case IDM_BERO: Bero(); break;
        case IDM_DISPI:
        {
#ifdef WIN32
#ifdef WIN95
	   char PROG[100];
	   strcpy(PROG,"INPUT32 ");
	   strcat(PROG,SCRDIR);
           RunInConsoleWindow(PROG," Transport Input Editor (Version I) ", 0, 0, 624, 420); break;
#else
	   char PROG[100];
	   strcpy(PROG,"INPUT ");
	   strcat(PROG,SCRDIR);
           RunInConsoleWindow(PROG," Transport Input Editor (Version I) ", 0, 0, 624, 420); break;
#endif
#else
	   WinExec("input",SW_SHOW);
	   HWND ha, ht = this->GetHandle();

	   do {
		wait(100);
		ha = GetActiveWindow();  // wait until tty-window is active
	      }  while (ha == ht);

	   HWND hp = GetNextWindow(ha,GW_HWNDPREV);
	   SetWindowPos(ha,hp,10,10,600,400,SWP_SHOWWINDOW);
	   break;
#endif
        }
        case IDM_SIGMA: SigmaOut(); break;
        case IDM_TEDI: homeit=1; TransOut(); break;
        case IDM_TRANS_FORM:
          {
            if (dpmirun)
            {
               DPMITrans(1);
            }
            else
            {
               Transport(1);
            }
	    homeit=1;
	    TransOut();
          }
          break;
        case IDM_TRANS_GRAF:
          {
            if (dpmirun)
            {
		DTM = FALSE;
		DPMITrans(0);
		if (errorflag == 1)
		{
		    DPMITrans(2);
		    homeit=0;
		    TransOut();
		}
            }
            else 
            {
		DTM = FALSE;
		int retval = Transport(0);
		if (retval == 1)
		{
		    RECT rc;

		    GetWindowRect(this->GetHandle(),&rc);
		    int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res
		    int w = (n*160)/4;
		    int h = (n*80)/4;
		    int x = rc.left + (rc.right-rc.left) / 2 - w / 2;  // center child window
		    int y = rc.top + (rc.bottom-rc.top) / 2 - h / 2;
		    RECT WinDim = {x,y,w,h};
		    opWin1 = new TextWindow(SW_SHOWNORMAL,String(IDS_TRANDLL),WinDim);
		    opWin1->WriteLn(String(IDS_RUNNING));
		    opWin1->Home();
		    short mod = 1;
		    TRNPRT(&mod,SCRDIR);
		    homeit = 0;
		    TransOut();
		    delete opWin1;
		}		
            }
          }
          break;
        case IDM_REDRAW:
          {
             DTM = FALSE;
             Redraw();
          }
          break;
        case IDM_SECOND:
          {
             DTM = TRUE;
             SecondOrder(command,FALSE);
          }
          break;
        case IDM_ELLIPSES: Ellipses(); break;
        case IDM_ELLIPSEL: Ellipses_long(); break;
        case IDM_ELLIPSE2: Ellipse2(); break;
        case IDM_SAVER: SaveRun(); break;
        case IDM_QUADS: TransOut(0); break;
        case IDM_FLOOR: TransOut(11); break;
        case IDM_BEAMLINE: TransOut(1); break;
        case IDM_BENDS: TransOut(2); break;
        case IDM_BEAM: TransOut(3); break;
        case IDM_FITS: TransOut(4); break;
        case IDM_SHIFTS: TransOut(5); break;
        case IDM_ELEMENTS: TransOut(6); break;
        case IDM_QDAC: TransOut(7); break;
        case IDM_QAMP: TransOut(8); break;
        case IDM_BDAC: TransOut(9); break;
        case IDM_BAMP: TransOut(10); break;
        case IDM_TIJK: Tijk(); break;
        case IDM_BEAMINFO: BeamInfo(); break;
        case IDM_COPYF: CopyFile(command); break;
        case IDM_QOI: OIFunc(0); break;
        case IDM_BOI: OIFunc(1); break;
        case IDM_LOI: OIFunc(2); break;
        case IDM_DELSCRA: DeleteScratch(); break;
        case IDM_DELFILE: DeleteFile(command); break;
        case IDM_PRINT: PrintFile(command); break;
        case IDM_PRINT_SETUP: PrinterSetup(); break;
	case IDM_PRINT_LW: PLW = GetPrintLineWidth(); break;
        case IDM_GETDACS: InsertDacValues(); break;
	case IDM_FXYDAC: FxyDac(); break;
	case IDM_FXYGAU: FxyGauss(); break;
        case IDM_EXE:
        case IDM_32: dpmirun = TRUE; DPMICheck(); break;
        case IDM_DLL:
        case IDM_16: dpmirun = FALSE; DPMICheck(); break;
        default: break;
    }
}

VOID MainWindow :: EBox(char ch)
{
    StringDialog *opStDlg;
    int   i;
    char  command[40], comm[20], par[20];

    WORD  mid[] = {IDM_EXIT,    IDM_TRANS_GRAF, IDM_TRANS_FORM, IDM_REDRAW,
                   IDM_TIJK,    IDM_EDT,        IDM_COPYF,      IDM_DELFILE,
                   IDM_DELSCRA, IDM_QUADS,      IDM_BEAMLINE,   IDM_BENDS,
                   IDM_BEAM,    IDM_FITS,       IDM_SHIFTS,     IDM_ELEMENTS,
                   IDM_QDAC,    IDM_QAMP,       IDM_BDAC,       IDM_BAMP,
                   IDM_QOI,     IDM_BOI,        IDM_LOI,        IDM_IFQ,
                   IDM_SIGMA,   IDM_TEDI,       IDM_BERO,       IDM_DISPI,
                   IDM_BEAMINFO,IDM_DISPAR,     IDM_PRINT,      IDM_PRINT_SETUP,
                   IDM_ABOUT,   IDM_SECOND,     IDM_ELLIPSES,   IDM_EDT,
                   IDM_SAVER,   IDM_GETDACS,    IDM_32,         IDM_16,
                   IDM_EXE,     IDM_DLL,        IDM_FXYDAC,     IDM_PRINT_LW,
		   0 };
/*
    char *cmp[] = {"EXIT",      "T",            "TFO",          "R",
                   "TIJK",      "EDIT",         "COPY",         "DEL",
                   "DELF",      "OQ",           "OBL",          "OB",
                   "OBE",       "OF",           "OS",           "EL",
                   "OQD",       "OQA",          "OBD",          "OBA",
                   "QOI",       "BOI",          "LOI",          "Q",
                   "SI",        "LI",           "BR",           "I",
                   "BI",        "D",            "PRINT",        "PS",
                   "A",         "DTM",          "E",            "EDT",
                   "SAVE",      "GD",           "32",           "16",
                   "EXE",       "DLL",          "FXY",          "PLW",
		   "" };
*/
//  generate above list with entries from resource file

    char *cmp[45], *p;
    int m;
    for (m=0; m<45; m++)
        cmp[m] = new char[6];
    cmp[44][0] = '\0';
    char comms1[170], comms2[90];
    strcpy(comms1,String(IDS_COMMS1));
    strcpy(comms2,String(IDS_COMMS2));
    strcat(comms1,comms2);
    m = 0;
    if ( (p = strtok(comms1,"|")) != 0 )
    {  strcpy(cmp[m++],p);
       while ( (p = strtok(NULL,"|")) != 0 )
          strcpy(cmp[m++],p);
    }

    char cc[2]; cc[0] = ch; cc[1] = '\0';
    opStDlg = new StringDialog(IDD_STRING_DIALOG,this,cc);
    if (opStDlg->GetResult())
    {
        strcpy(command,opStDlg->GetString());
        delete opStDlg;
        AnsiUpper(command);
	strcpy(comm,"");
        sscanf(command,String(IDS_NX7_FORM),&comm[0],&par[0]); // separate off ev. parameter
        i = 0;
        while (strlen(cmp[i]))
        {
	   if (strcmp(cmp[i],comm) == 0)
           {
	      ExecuteCommand(mid[i],command);
              break;
           }
           i++;
        }

        if ( strlen(cmp[i]) == 0 )
        {
           SimpleDialog oDg(IDD_QEND_DIALOG,this,String(IDS_UNKNOWN));
           if (oDg.GetResult())
           {
              SimpleDialog oDlg(IDD_HELP_DIALOG,this,String(IDS_LISTCOM));
           }
        }
    }
    else
       delete opStDlg;

    for (m=0; m<45; m++)
        delete []cmp[m]; // [] found with Bounds Checker
}

long MainWindow :: WmPaint( const PAINTSTRUCT &ps )
{
    DrawBitmap(this, ps.hdc);
    return FALSE;
}

#ifdef WIN95

void MainWindow :: DrawDIB256(DisplayDevice *opDev, HDC hdcscr)
{
	LPBITMAPINFOHEADER lpInfo, lpHeader;
	HRSRC    hResData;
	HRSRC    hResInfo;
	HGLOBAL  hPalHeader;
	HPALETTE hpal;
	HBITMAP  hDDBitmap, hOldBitmap;
	HDC      hdcMem;
	WORD     offBits;
	POINT    ptStart, ptSize, ptOrg;

	hResInfo = (HRSRC)FindResource(Application::GetInstance(),
		MAKEINTRESOURCE(IDR_APP_BMP2),RT_BITMAP);
	hResData = (HRSRC)LoadResource(Application::GetInstance(),hResInfo);
	lpInfo = (LPBITMAPINFOHEADER) LockResource(hResData);

	hpal = MakeDIBPalette(lpInfo,&offBits);
	if (SelectPalette(hdcscr,hpal,FALSE) == NULL)
		DispObjectMsg("Can't select palette");
	if (RealizePalette(hdcscr) == NULL)
		DispObjectMsg("Can't realize palette");
	hPalHeader = MakeIndexHeader(lpInfo);
	if (hPalHeader == 0)
		DispObjectMsg("Can't create indexed color table");
	lpHeader = (LPBITMAPINFOHEADER) GlobalLock(hPalHeader);

	hdcMem = CreateCompatibleDC(hdcscr);
	hDDBitmap = CreateCompatibleBitmap(hdcscr, 
		(WORD)lpInfo->biWidth, (WORD)lpInfo->biHeight);
	SetDIBits(hdcscr, hDDBitmap, 0, (WORD)lpInfo->biHeight, 
		(LPSTR)lpInfo + offBits, 
		(LPBITMAPINFO)lpHeader, DIB_PAL_COLORS);
	hOldBitmap = (HBITMAP)SelectObject(hdcMem, hDDBitmap);
	ptSize.x  = lpInfo->biWidth;
	ptSize.y  = lpInfo->biHeight;
	ptStart.x = 0;
	ptStart.y = 0;
	for (int i=0; i<5; i++)
	{  for (int j=0; j<4; j++)
	   {
		ptOrg.x = i * lpInfo->biWidth;
		ptOrg.y = j * lpInfo->biHeight;
		opDev->CopyBits(ptOrg,ptSize,ptStart,hdcMem);
	   }
	}
	GlobalUnlock(hPalHeader);
	UnlockResource(hResData);
	FreeResource(hResInfo);
	SelectObject(hdcMem,hOldBitmap);
	DeleteDC(hdcMem);   
}
#endif

void MainWindow :: DrawBitmap(DisplayDevice *opDev, HDC hdcscr)
{
   HBITMAP hBitmap;
   BITMAP  bm;
   HDC     hdcMem;
   RECT    rc;
   POINT   ptStart, ptSize, ptOrg, ptStretch;

#ifdef WIN95xxxx

   UINT nColors = 1 << GetDeviceCaps(hdcscr,PLANES) * 
                          GetDeviceCaps(hdcscr,BITSPIXEL);

   if (nColors <= 16 && nColors > 1)
      hBitmap = LoadBitmap(Application::GetInstance(),
                                MAKEINTRESOURCE(IDR_APP_BMP));
   else
   {
      if (GetDeviceCaps(hdcscr,SIZEPALETTE) == 256)
      {
	  DrawDIB256(opDev,hdcscr);
	  return;
      }
      else
	  hBitmap = LoadBitmap(Application::GetInstance(),
                                MAKEINTRESOURCE(IDR_APP_BMP2));
   }
#else
   hBitmap = LoadBitmap(Application::GetInstance(),
                             MAKEINTRESOURCE(IDR_APP_BMP2));
#endif

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

   for (int i=0; i<5; i++)
   {  for (int j=0; j<4; j++)
      {
         ptOrg.x = i * ptSize.x;
         ptOrg.y = j * ptSize.y;
         opDev->CopyBits(ptOrg,ptStretch,ptStart,hdcMem);
      }
   }

   DeleteDC(hdcMem);   
   DeleteObject(hBitmap);
}

MainWindow :: ~MainWindow(VOID)
{
}

BOOL MainWindow :: WmDestroy(VOID)
{
#ifndef WIN95
    if (opExit) delete opExit;   // found with Bounds Checker
    opExit = NULL;
#endif
    if (opMenu) delete opMenu;   // found with Bounds Checker
    opMenu = NULL;
    if (opSysMenu) delete opSysMenu;
    opSysMenu = NULL;
    PostQuitMessage( 0 );
    return FALSE;
}

BOOL MainWindow :: WmClose(VOID)
{
    SimpleDialog oDlg(IDD_QEND_DIALOG,this);
    if (oDlg.GetResult())
       WmDestroy();
    return FALSE;
}

VOID MainWindow :: Edit(LPCSTR command)
{
   char Command[120];
#ifdef WIN95
   strcpy(Command,"EDITOR32.exe ");
#else
   strcpy(Command,"EDITOR.exe ");
#endif
   char comm[50], Filename[50];
   int n = sscanf(command,String(IDS_NX7_FORM),&comm[0],&Filename[0]);

   if (n <= 1)
   {
      OpenFileDlg *opF = new OpenFileDlg(this->GetHandle(), SCRDIR,
                                   String(IDS_FLTSTR),String(IDS_DATEXT,1));
      if (opF->GetResult())
      {  strcpy(Filename,opF->GetFileName());
         delete opF;
      }
      else
      {  delete opF;
         return;
      }
   }

   strcat(Command,Filename);
   WinExec(Command,SW_NORMAL);
}

int MainWindow :: Transport(int mode)
{
    int retval;
#ifndef BCNT
    retval = Transport0(mode);    
#else
    retval = Transport1(mode);
    Transport2(mode,retval);
#endif
    return retval;
}

#ifndef BCNT

int MainWindow :: Transport0(int mode)
{
    short retval;
    RECT rc;

    GetWindowRect(this->GetHandle(),&rc);
    int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res
    int w = (n*160)/4;
    int h = (n*80)/4;
    int x = rc.left + (rc.right-rc.left) / 2 - w / 2;  // center child window
    int y = rc.top + (rc.bottom-rc.top) / 2 - h / 2;
    RECT WinDim = {x,y,w,h};
    opWin1 = new TextWindow(SW_SHOWNORMAL,String(IDS_TRANDLL),WinDim);
    opWin1->WriteLn(String(IDS_RUNNING));
    opWin1->Home();
//#ifdef WIN95
//#ifdef __BORLANDC__
    short mod = mode;
    retval = TRNPRT(&mod,SCRDIR);
//#else
//    int len = 80L;
//    retval = (short)TRANS(&mode,SCRDIR,len); // LF90-DLL with "hidden" argument
//#endif
//#else
//    short mod = mode;
//    retval = TRNPRT(&mod,SCRDIR);
//#endif
    delete opWin1;

    if (mode == 0 && retval == 0)
    {
       if (!DTM)
          Redraw();
       else
          SecondOrder(NullStr,TRUE);
    }

    if (retval == 1)
    {
       PutInfo(IDS_ERRF1);
    }
    if (retval == 2)
    {
       PutInfo(IDS_F1NOTF);
    }
    return retval;
}
#else

int MainWindow :: Transport1(int mode)
{
    short retval;
    RECT rc;

    GetWindowRect(this->GetHandle(),&rc);
    int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res
    int w = (n*160)/4;
    int h = (n*80)/4;
    int x = rc.left + (rc.right-rc.left) / 2 - w / 2;  // center child window
    int y = rc.top + (rc.bottom-rc.top) / 2 - h / 2;
    RECT WinDim = {x,y,w,h};
    opWin1 = new TextWindow(SW_SHOWNORMAL,String(IDS_TRANDLL),WinDim);
    opWin1->WriteLn(String(IDS_RUNNING));
    opWin1->Home();
//#ifdef WIN95
//#ifdef __BORLANDC__
    short mod = (short)mode;
    retval = TRNPRT(&mod,SCRDIR);
//#else
//    int len = 80L;
//    retval = (short)TRANS(&mode,SCRDIR,len); // LF90-DLL with "hidden" argument
//#endif
//#else
//    short mod = mode;
//    retval = TRNPRT(&mod,SCRDIR);
//#endif
    return retval;
}

void MainWindow :: Transport2(int mode, int retval)
{
    delete opWin1;

    if (mode == 0 && retval == 0)
    {
       if (!DTM)
          Redraw();
       else
          SecondOrder(NullStr,TRUE);
    }

    if (retval == 1)
    {
       PutInfo(IDS_ERRF1);
    }
    if (retval == 2)
    {
       PutInfo(IDS_F1NOTF);
    }
}
#endif

VOID MainWindow :: DPMICheck(VOID)
{
    if (dpmirun)
    {  opMenu->SetCheck(IDM_32,TRUE);
       opMenu->SetCheck(IDM_16,FALSE);
    }
    else
    {  opMenu->SetCheck(IDM_32,FALSE);
       opMenu->SetCheck(IDM_16,TRUE);
    }
}

VOID MainWindow :: DPMITrans(int mode)
{
    char PROG[80];
    RECT rc;

    GetWindowRect(this->GetHandle(),&rc);

    int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res
    int w = (n*152)/4;
    int h = (n*120)/4;
#ifdef WIN32
    w = (2 * w) / 3;
    h = h / 2;
#endif
    int x = rc.left + (rc.right-rc.left) / 2 - w / 2;  // center child window
    int y = rc.top + (rc.bottom-rc.top) / 2 - h / 2;

    remove(String(IDS_ERRORT)); // delete file ERROR.TRA from previous run

#ifndef WIN32
    strcpy(PROG,String(IDS_TRAN32));  // TRANEM32
#else
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
    GetVersionEx(&osvi);
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
//       strcpy(PROG,"Trans95 ");
       strcpy(PROG,"TransNT.pif ");
    else
       strcpy(PROG,"TransNT ");
#endif
    if (mode == 0)
      strcat(PROG,"00 ");
    else if (mode == 2)
      strcat(PROG,"22 ");
    else
      strcat(PROG,"11 ");

    char temp[80];
    strcpy(temp,SCRDIR);
    for (int i=0; i<(int)strlen(temp); i++) // replace Blanks by '$' ..
       if (temp[i] == ' ') temp[i] = '$';   // .. for transnt.exe parameter
    strcat(PROG,temp);

#ifndef WIN32

    WinExec(PROG,SW_SHOW);

    HANDLE ha, ht = this->GetHandle();

    do {
          wait(100);
          ha = GetActiveWindow();  // wait until tty-window is active
       }  while (ha == ht);

    HANDLE hp = GetNextWindow(ha,GW_HWNDPREV);
    SetWindowPos(ha,hp,x,y,w,h,SWP_SHOWWINDOW);

    do {
          wait(200);
          if (GetActiveWindow() == ht) break; // loop until tty disapears
       } while (1) ;
    wait(200);

#else

    RunInConsoleWindow(PROG," Trans ",x,y,w,h);
    wait(500);

#endif

    errorflag = 0;
    FILE *hf = fopen(String(IDS_ERRORT),"r"); // open file ERROR.TRA

    if (!hf)                       // if not existing .. no error
    {
       if (mode == 0)
       {
          if (!DTM)
             Redraw();
          else
             SecondOrder(NullStr,TRUE);
       }
    }
    else
    {
	errorflag = 1;
	fclose(hf);
    }
}

long MainWindow :: WmTimer(UINT )
{
    return FALSE;
}

VOID MainWindow :: TransOut(VOID)
{
    FILE *fp;

    if ((fp = fopen(for002,"r")) == NULL)
    {  
       PutInfo(IDS_F2NOTF);
       return;
    }
    else
    {
       char buf[100];
       char dim1[15],dim2[15],dim3[15],dim4[15],dim5[15],
            dim6[15],dim7[15],dim8[15],dim9[15],dim10[15],dim11[15];
       fread(buf,50,1,fp);
       fclose(fp);
       buf[50] = '\0'; // found with Bounds Checker
       int n = sscanf(buf,String(IDS_11S),
                   dim1,dim2,dim3,dim4,dim5,dim6,dim7,dim8,dim9,dim10,dim11);
       if ( (n == 11) && isalpha(dim1[0]) && isalpha(dim3[0]) )
       {
          PutInfo(IDS_RUNSTF);
          return;
       }
    }

    int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res

    RECT WinDim = { 0, 0, (n*640)/4, (n*480)/4 };
#ifndef WINNT
    STextWindow *opWin =
                   new STextWindow(SW_SHOWNORMAL,String(IDS_TRANOUT),WinDim);

    opWin->ShowFile(for002);
    if (homeit == 1)
       opWin->Home();
    else 
       opWin->End();
#else
    TextWindow *opWin =
               new TextWindow(SW_SHOWNORMAL,String(IDS_TRANOUT),WinDim);
    opWin->ShowFile(for002);
    if (homeit == 1)
       opWin->Home();
    else 
       opWin->End();
    opWin->WaitForKeyDown();
    if (!opWin->IsClosed())
       delete opWin;
#endif
}

VOID MainWindow :: TransOut(int mode)
{
   char buffer[80];
   int  index;
   UINT caption[] = {IDS_QUADS,IDS_BEAML,IDS_BENDM,IDS_IBEAM,IDS_FITC,
                     IDS_SHIFT,IDS_ELEM,IDS_QDAC,IDS_QAMP,IDS_BDAC,IDS_BAMP,
		     IDS_LAYOUT};
   int width[]    = {420,600,590,210,490,600,590,200,160,200,160,600};
   WORD key;

   TransFile *opFile = new TransFile(for002,NULL,for001);
   int nr = opFile->GetNumberOfElements();
   if (nr > 0)
   {  int n = (GetScreenWidth()>800) ? 5 : 4;  // scale with 1.25 for hi res
      RECT WinDim = { (n*20)/4, (n*45)/4, (n*width[mode])/4, (n*350)/4 };
      TextWindow *opWin = 
                 new TextWindow(SW_SHOWNORMAL,String(caption[mode]),WinDim);
      Memory *opMem = new Memory(3000);
      index   = 0;
      while (opFile->Show(mode,buffer))
      {  index++;
         opMem->WriteLn(buffer);
      }
      opWin->ShowMemory(opMem);
      opWin->End();
      if (index == 0)
      {   opWin->WriteLn(String(IDS_NODATAF));
          opWin->WaitForKeyDown();
      }
      else
      {   key = opWin->WaitForKeyDown();
          if (key == 'F' || key == 'f')
             FileOut(opFile,mode,String(caption[mode]));
          else if ((key == 'S' || key == 's') && (mode==7 || mode==9))
          {
             opWin->WriteLn(Blank);
             CamacOut(opFile,mode,opWin);
          }
      }
      opMem->FreeMemory(); // found by Bounds Ckecker
      delete opMem;  // found by Bounds Checker (doesn't free hMem)
      if (!opWin->IsClosed())
         delete opWin;
   }
   else
   {  
      PutInfo(IDS_RUNGTF);
   }
   delete opFile;
}

VOID MainWindow :: CamacOut(TransFile *opFile, int mode, TextWindow *opWin)
{
   char buffer[80], label[10];
   long value;

   typedef int (WINAPI *LPINISERIALCOMM) (LPSTR);
   typedef int (WINAPI *LPCLOSESERIALCOMM) (void);
   typedef int (WINAPI *LPINIDEVLIST) (LPSTR);
   typedef int (WINAPI *LPDELDEVLIST) (void);
   typedef int (WINAPI *LPRESERVECRATE) (void);
   typedef int (WINAPI *LPFREECRATE) (void);
   typedef int (WINAPI *LPINIKSI) (void);
   typedef int (WINAPI *LPSETDAC) (LPSTR, long);
   LPINISERIALCOMM lpIniSerialComm;
   LPCLOSESERIALCOMM lpCloseSerialComm;
   LPINIDEVLIST lpIniDevList;
   LPDELDEVLIST lpDelDevList;
   LPRESERVECRATE lpReserveCrate;
   LPFREECRATE lpFreeCrate;
   LPINIKSI lpIniKSI;
   LPSETDAC lpSetDac;

   opWin->SetFocus();
   BOOL ok = FALSE;
   BOOL direct = (strncmp(PORT,"COM",3) == 0) ? TRUE : FALSE;
   if (direct)
   {   
#ifdef WIN95
      hCamacLib = LoadLibrary("CAMAC32.DLL");
#else
      hCamacLib = LoadLibrary("CAMAC.DLL");
#endif
#ifdef WIN32
      if (hCamacLib <= 0)
      {
         DispObjectMsg("CAMAC(32).DLL not found.");
         return;
      }
#else
      if (hCamacLib <= 32)
      {
         DispObjectMsg("CAMAC(32).DLL not found.");
         return;
      }
#endif
      lpIniSerialComm = (LPINISERIALCOMM)GetProcAddress(hCamacLib,"IniSerialComm");
      lpCloseSerialComm = (LPCLOSESERIALCOMM)GetProcAddress(hCamacLib,"CloseSerialComm");
      lpIniDevList = (LPINIDEVLIST)GetProcAddress(hCamacLib,"IniDevList");
      lpDelDevList = (LPDELDEVLIST)GetProcAddress(hCamacLib,"DelDevList");
      lpReserveCrate = (LPRESERVECRATE)GetProcAddress(hCamacLib,"ReserveCrate");
      lpFreeCrate = (LPFREECRATE)GetProcAddress(hCamacLib,"FreeCrate");
      lpIniKSI = (LPINIKSI)GetProcAddress(hCamacLib,"IniKSI");
      lpSetDac = (LPSETDAC)GetProcAddress(hCamacLib,"SetDac");

      (*lpIniSerialComm)(PORT);
      (*lpIniKSI)();
      if ((*lpIniDevList)(String(IDS_DEVLIS)) > -1)  // file DEVICE.LIS
      {  if (!(*lpReserveCrate)())
         {
            SimpleDialog oDg(IDD_QEND_DIALOG,this,String(IDS_CBUSY));
            if (!oDg.GetResult())
            {  (*lpDelDevList)();
               (*lpCloseSerialComm)();
               FreeLibrary(hCamacLib);
               return;
            }
            opWin->SetFocus();
         }
         ok = TRUE;
      }
   }
   else  // do it via TCP/IP
   {
      char SERVER[20], PORTSTR[20], temp[80], *p;
      strcpy(temp,PORT);
      p = strtok(temp,"$");
      if (p)
         strcpy(SERVER,p);
      else
         return;
      p = strtok(NULL,"$");
      if (p)
         strcpy(PORTSTR,p);
      else
         return;

      int retval = TCPConnect(&sock,SERVER,(int)atol(PORTSTR));
      if (!retval)
         return;

      char recbuff[2048];
      TCPSend(sock,"test TCP",recbuff);
      if (strncmp(recbuff,"*DENY*",6) == 0)
      {
         DispObjectMsg("All server sockets used.");
         return;
      }
      ok = TRUE;
   }

   if (ok)
   {
      opFile->Rewind();
      while (opFile->Show(mode,buffer))
      {  int n = sscanf(buffer,String(IDS_SLD),&label[0],&value);
         if (n == 2)
         {
	    int retval;
	    if (direct)
               retval = (*lpSetDac)(label,value);
	    else
               retval = SetDacValue(sock,label,value);
            if (retval)
               sprintf(buffer,String(IDS_SET_TO),label,value);
            else
                sprintf(buffer,String(IDS_NOT_SET),label);
            opWin->WriteLn(buffer);
         }
      }
      opWin->WaitForKeyDown();
      if (direct)
      {
         (*lpFreeCrate)();
         (*lpDelDevList)();
         (*lpCloseSerialComm)();
         FreeLibrary(hCamacLib);
      }
      else
         TCPClose(sock);
   }
   else
   {
      if (direct)
      {
         PutInfo(IDS_DEVLISF);
         (*lpCloseSerialComm)();
         FreeLibrary(hCamacLib);
      }
   }
}

int  MainWindow :: GetPrintLineWidth(VOID)
{
    char linewidth[40];
    int width;

    itoa(PLW,linewidth,10);
    StringDialog *opStDlg = new StringDialog(IDD_LW_DIALOG,this,linewidth);
    if (opStDlg->GetResult())
       strcpy(linewidth,opStDlg->GetString());
    delete opStDlg;

    WritePrivateProfileString("Printing","line_width",linewidth,
			     "Transprt.ini");
    width = atoi(linewidth);
    return width;
}

VOID MainWindow :: PrinterSetup(VOID)
{
   Printer oPrinter;
   oPrinter.Setup(this);
}

VOID MainWindow :: PrintFile(char *command)
{
    char buffer[180], comm[10];
    FILE *fp;
    char FileName[50];
    FileName[0] = '\0';

    sscanf(command,String(IDS_NX7_FORM),&comm[0],&FileName[0]);

    if (strlen(FileName) == 0)  // no parameter given
    {
       OpenFileDlg *opF = new OpenFileDlg(this->GetHandle(), SCRDIR,
                                          String(IDS_FLTSTR),
                                          String(IDS_DATEXT,1),
                                          String(IDS_PRINTF,2));
       if (opF->GetResult())
       {  strcpy(FileName,opF->GetFileName());
          delete opF;
       }
       else
       {  delete opF;
          return;
       }
    }

    Printer *opPrt = new Printer;

    opPrt->Start(String(IDS_LISTING),this);

    int h_print = opPrt->GetUserHeight();
    int inc = h_print / 60;

    int fs;

    fs = h_print / 60
;

    Font *opFont = new Font(String(IDS_COURIER));  // "Courier New"
    opFont->SetPointSize(fs);
    Font *opOldFont = opPrt->SetFont(opFont);

    int x     = opPrt->GetUserWidth()/20;
    int y     = inc;
    int index = 1;

    AnsiUpperBuff(FileName,strlen(FileName));
    BOOL IsFor002 = !strcmp(FileName,for002);
    if ((fp = fopen(FileName,"r")) != NULL)
    {  while ( ! feof(fp) )
       {  if (fscanf(fp,String(IDS_CR_FORM),&buffer[0]))
          {  fgetc(fp);
             if (buffer[0] == '1' && IsFor002) // form feed in FOR002.DAT
             {  y = inc; index = 1;
                opPrt->NextPage();
                buffer[0] = ' ';
                SetFont(opOldFont);
                opPrt->SetFont(opFont);
             }
             y += inc; index += 1;
             opPrt->PosStr(x,y,buffer);
             buffer[0] = '\0';
          }
          else            // empty line
          {  fgetc(fp);
             y += inc; index += 1;
          }
          if (index >= 55)
          {  y = inc; index = 1;
             opPrt->NextPage();
             SetFont(opOldFont);
             opOldFont = opPrt->SetFont(opFont);
          }
       }
//       if (index != 1) opPrt->NextPage();
       fclose(fp);
    }
    opPrt->End(this);
    delete opPrt;

    SetFont(opOldFont);
    delete opFont;

//  execute VPP if PrinterPort = c:\ps810.prn

    char info[80];
    GetProfileString(String(IDS_WINDOWS),String(IDS_DEVICE,1),NullStr,info,80);
    AnsiLowerBuff(info,strlen(info));
    if (strstr(info,String(IDS_PS810)) != NULL)  // equal "c:\\ps810.prn" ?
       WinExec(String(IDS_VPP),SW_MAXIMIZE);     // exec "VPP.BAT c:\\ps810.prn"
}

VOID MainWindow :: FileOut(TransFile *opFile, int mode, char *caption)
{
   char buffer[80];
   FILE *out;

   if ((out = fopen(String(IDS_OUTPUT),"a")) != NULL)
   {  fprintf(out,"\n");
      fprintf(out,String(IDS_SN_FORM),opFile->GetTitleOfRun());
      fprintf(out,String(IDS_SN_FORM),caption);
      fprintf(out,"\n");
      opFile->Rewind();
      while (opFile->Show(mode,buffer))
         fprintf(out,String(IDS_SN_FORM),buffer);
      fclose(out);
   }
}

VOID MainWindow :: CopyFile(char *command)
{
    char SourceName[50], DestName[50], comm[10];
    FILE *in, *out;
    SourceName[0] = DestName[0] = '\0';

    sscanf(command,String(IDS_3S),&comm[0],&SourceName[0],&DestName[0]);

    // select source file if not given

    if (strlen(SourceName) == 0)
    {  
       OpenFileDlg *opF = new OpenFileDlg(this->GetHandle(), SCRDIR,
                                          String(IDS_FLTSTR),
                                          String(IDS_DATEXT,1),
                                          String(IDS_SOURCE,2));
       if (opF->GetResult())
       {  strcpy(SourceName,opF->GetFileName());
          delete opF;
       }
       else
       {  delete opF;
          return;
       }
    }

    // select destination file if not given

    if (strlen(DestName) == 0)
    {  
       SaveFileDlg *opF = new SaveFileDlg(this->GetHandle(), SCRDIR,
                                          String(IDS_FLTSTR),
                                          String(IDS_DATEXT,1),
                                          String(IDS_DEST,2));
       if (opF->GetResult())
       {
          strcpy(DestName,opF->GetFileName());
          delete opF;
       }
       else
       {  delete opF;
          return;
       }
    }

    // source == dest ?

    if (strcmp(AnsiUpper(SourceName),AnsiUpper(DestName)) == 0)
    {  
       PutInfo(IDS_SREQDES);
       return;
    }

    // backup old destination file if it exists

    char backup[200];
    strcpy(backup,DestName);
    strcat(backup,"2");
    remove(backup);
    rename(DestName,backup);

    // open files

    if ((in = fopen(SourceName,"rt")) == NULL)
    {
       PutInfo(IDS_SOURCEF);
       return;
    }
    if ((out = fopen(DestName,"wt")) == NULL)
    {
       PutInfo(IDS_DESTF);
       return;
    }

    // copy file
    
    int c;
    while (1)
    {   c = fgetc(in);
        if (feof(in))
           break;
        fputc(c,out);
    }
    fclose(in);
    fclose(out);
}

VOID MainWindow :: Tijk(VOID)
{
   TransSecondFile *opFile = new TransSecondFile(for000,keep0,for001);
   int nr = opFile->GetNumberOfElements();
   if (nr > 0)
   {  int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res
      RECT WinDim = {0, (n*24)/4, (n*520)/4, (n*430)/4};
      TextWindow *opWin = new TextWindow(SW_SHOWNORMAL,String(IDS_TIJK),WinDim);
      opWin->DisableClose();
      RECT rc = {0,0,640,1};
      AuxWin *opAux = new AuxWin(SW_HIDE,NullStr,rc);

#ifndef WIN32
      UINT Size = 9 * (HeapWalk(Application::GetInstance()) / 10);
      Memory *opMem = new Memory(Size);
      opWin->ShowMemory(opMem);
#endif

      int dn = IDD_BAR_DIALOG; // next line for hi res with small fonts
      if ((n==5) && (GetSystemMetrics(SM_CYCAPTION)==20)) dn = IDD_BAR2_DIALOG;
      TijkDialog *opTijkDlg = new TijkDialog(dn,opAux,opWin,opFile);
      delete opTijkDlg;
      delete opAux;
      delete opWin;

#ifndef WIN32
      delete opMem;  // found by Bounds Checker (doesn't free hMem)
#endif

   }
   else
   {  
      PutInfo(IDS_NOSECOR);
   }
   delete opFile;
}

VOID MainWindow :: BeamInfo(VOID)
{
   TransFile *opFile = new TransFile(for002,NULL,for001);
   int nr = opFile->GetNumberOfElements();
   if (nr > 0)
   {  int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res

      RECT WinDim = { 0, (n*24)/4, (n*580)/4, (n*430)/4};
      TextWindow *opWin = new TextWindow(SW_SHOWNORMAL,String(IDS_BINFO),WinDim);
      opWin->DisableClose();
      RECT rc = {0,0,640,1};
      AuxWin *opAux = new AuxWin(SW_HIDE,NullStr,rc);

#ifndef WIN32
      UINT Size = 9 * (HeapWalk(Application::GetInstance()) / 10);
      Memory *opMem = new Memory(Size);
      opWin->ShowMemory(opMem);
#endif

      int dn = IDD_BAR_DIALOG; // next line for hi res with small fonts
      if ((n==5) && (GetSystemMetrics(SM_CYCAPTION)==20)) dn = IDD_BAR2_DIALOG;
      InfoDialog *opInfoDlg = new InfoDialog(dn,opAux,opWin,opFile);
      delete opInfoDlg;
      delete opAux;
      delete opWin;

#ifndef WIN32
      delete opMem;  // found by Bounds Checker (doesn't free hMem)
#endif

   }
   else
   {  
      PutInfo(IDS_RUNGTF);
   }
   delete opFile;
}

VOID MainWindow :: SigmaOut(VOID)
{
   FILE *fp;

   if ((fp = fopen(for002,"r")) == NULL)
   {  
       PutInfo(IDS_F2NOTF);
       return;
   }
   else
   {
       char buf[100];
       char dim1[15],dim2[15],dim3[15],dim4[15],dim5[15],
            dim6[15],dim7[15],dim8[15],dim9[15],dim10[15],dim11[15];
       fread(buf,50,1,fp);
       fclose(fp);
       buf[50] = '\0'; // found with Bounds Checker
       int n = sscanf(buf,String(IDS_11S),
                   dim1,dim2,dim3,dim4,dim5,dim6,dim7,dim8,dim9,dim10,dim11);
       if ( (n == 11) && isalpha(dim1[0]) && isalpha(dim3[0]) )
       {
          PutInfo(IDS_RUNSTF);
          return;
       }
   }

   TrOutFile *opFile = new TrOutFile(for002,for001);
   int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res

   RECT WinDim = { 0, (n*24)/4, (n*580)/4, (n*430)/4};
   TextWindow *opWin = new TextWindow(SW_SHOWNORMAL,String(IDS_SIGMAM),WinDim);
   opWin->DisableClose();
   RECT rc = {0,0,640,1};
   AuxWin *opAux = new AuxWin(SW_HIDE,NullStr,rc);

#ifndef WIN32
   UINT Size = 9 * (HeapWalk(Application::GetInstance()) / 10);
   Memory *opMem = new Memory(Size);
   opWin->ShowMemory(opMem);
#endif

   SigmaDialog *opSigmaDlg = new SigmaDialog(IDD_BAR_DIALOG,
                                                opAux,opWin,opFile);
   delete opSigmaDlg;

   delete opAux;
   delete opWin;
   delete opFile;

#ifndef WIN32
      delete opMem;  // found by Bounds Checker (doesn't free hMem)
#endif

}

VOID MainWindow :: OIFunc(int mode)
{
   char buffer[80];
   UINT caption[] = {IDS_QOI, IDS_BOI, IDS_LOI};
   TransInFile *opInput = new TransInFile(for001);
   TransFile  *opOutput = new TransFile(for002,NULL,for001);
   int nr = opOutput->GetNumberOfElements();
   if (nr > 0)
   {  int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res

      RECT WinDim = { (n*80)/4, (n*45)/4, (n*480)/4, (n*350)/4};
      TextWindow *opWin = new TextWindow(SW_SHOWNORMAL,String(caption[mode]),WinDim);

      opInput->GetTransFilePtr(opOutput);
      while (opInput->TransferData(mode,buffer))
         opWin->WriteLn(buffer);
      
      opWin->WriteLn(String(IDS_UPDF1));
      WORD key = opWin->WaitForKeyDown();
      if (key == 'Y' || key == 'y')
      {  
         opWin->WriteLn(Blank);
         opInput->OverWrite();
         opWin->WriteLn(String(IDS_WRNF1));
         wait(1000);
      }
      if (!opWin->IsClosed())
         delete opWin;
   }
   else
   {  
      PutInfo(IDS_RUNGTF);
   }
   delete opOutput;
   delete opInput;
}

VOID MainWindow :: Bero(void)
{
   char buffer[80];
   StringDialog *opStDlg;

   TransInFile *opInput = new TransInFile(for001);
   float OldP = opInput->GetOldP();
   float pscal = opInput->GetPScal();
   sprintf(buffer,String(IDS_BF_FORM),float(33.356) * OldP * pscal);
   opStDlg = new StringDialog(IDD_BERO_DIALOG,this,buffer);
   if (opStDlg->GetResult())
   {
       strcpy(buffer,opStDlg->GetString());
       delete opStDlg;
       float Bero;
       sscanf(buffer,String(IDS_F),&Bero);
       opInput->SetNewP(Bero/(float(33.356)*pscal));

       int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res

       RECT WinDim = { (n*10)/4, (n*10)/4, (n*530)/4, (n*450)/4};
       TextWindow *opWin = new TextWindow(SW_SHOWNORMAL,String(IDS_BERO_SCL),WinDim);

       while (opInput->TransferData(3,buffer))
           opWin->WriteLn(buffer);
      
       opWin->WriteLn(String(IDS_UPDF1));
       WORD key = opWin->WaitForKeyDown();
       if (key == 'Y' || key == 'y')
       {  
          opWin->WriteLn(Blank);
          opInput->OverWrite();
          opWin->WriteLn(String(IDS_WRNF1));
          wait(1000);
       }
       delete opWin;
   }
   else
       delete opStDlg;

   delete opInput;
}

VOID MainWindow :: DeleteFile(char *command)
{
    char FileName[50], comm[10];
    FileName[0] = '\0';

    int n = sscanf(command,String(IDS_NX7_FORM),&comm[0],&FileName[0]);

    if (n <= 1)                         // if no parameters given
    {  
       OpenFileDlg *opF = new OpenFileDlg(this->GetHandle(), SCRDIR,
                                          String(IDS_FLTSTR),
                                          String(IDS_DATEXT,1),
                                          String(IDS_DELFIL,2));
       if (opF->GetResult())
       {  strcpy(FileName,opF->GetFileName());
          delete opF;
       }
       else
       {  delete opF;
          return;
       }
    }
    remove(FileName);
}

VOID MainWindow :: DeleteScratch(VOID)
{
    int i;

    remove(for000);
    remove(for002);

    int pos = strlen(save0) - 5;

    for (i=0; i<10; i++)
    {  save0[pos] = char('0' + i);
       remove(save0);
    }

    pos = strlen(keep0) - 5;

    for (i=0; i<10; i++)
    {  keep0[pos] = char('0' + i);
       remove(keep0);
    }

}

VOID MainWindow :: Redraw(VOID)
{
   int ReRunFlag = 0;

   TransGrafFile  *opGrafFile = new TransGrafFile(for002,save0,for001);
   int nr = opGrafFile->GetNumberOfElements();
   if (nr > 0)
   {  DisplayFile *opDispFile = new DisplayFile(for004);
      DISPAR *dispar = opDispFile->GetParameters();
      if (dispar != NULL)
      {  BOOL colorflag = FALSE;
         BOOL printcolorflag = FALSE;
         if (strcmp(COLFLAG,CStr) == 0) colorflag = TRUE;
         if (strcmp(PRTCOLFLAG,CStr) == 0) printcolorflag = TRUE;
         int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res
         RECT WinDim = {0,0,(n*640)/4,(n*480)/4};
         TransGrafWin *opWin = 
                      new TransGrafWin(SW_SHOWNORMAL,
                      opGrafFile->GetTitleOfRun(),WinDim,
                      this,opGrafFile,0L,dispar,colorflag,printcolorflag,PLW);

         opWin->WaitForKeyDown();
         ReRunFlag = opWin->GetResult();
         delete opWin;
      }  
      else
      {
         PutInfo(IDS_F4NOTF);
      }
      delete opDispFile;
   }
   else if (nr == -1)
   {
      PutInfo(IDS_WRONGD);
   }
   else
   {
      PutInfo(IDS_RUNGTF);
   }
   delete opGrafFile;

   if (ReRunFlag & 2)
   {  SaveRun();
      if (dpmirun)
         DPMITrans(0);
      else
         Transport(0);
   }
   else if (ReRunFlag & 4)
   {  DeleteScratch();
      if (dpmirun)
         DPMITrans(0);
      else
         Transport(0);
   }
}

VOID MainWindow :: DisplayParameters(VOID)
{
   DisplayFile *opDispFile = new DisplayFile(for004);
   opDispFile->GetParameters();

   DisplayDialog oDlg(IDD_DISPAR_DIALOG,this,opDispFile);
   if (oDlg.GetResult())
      opDispFile->WriteFile(for004);

   delete opDispFile;
}

VOID MainWindow :: SecondOrder(char *command, BOOL jump)
{
   static char parlist[40];
   char **Item;
   int  mr, nr, i;
   int ReRunFlag;

   TransGrafFile  *opGrafFile = new TransGrafFile(for002,NULL,for001);
   nr = opGrafFile->GetNumberOfElements();
   if (nr == 0)
   {  
      PutInfo(IDS_RUNGTF);
      delete opGrafFile;
      return;
   }

   TransSecondFile *opSecFile = new TransSecondFile(for000,keep0,for001);
   nr = opSecFile->GetNumberOfElements();
   if (nr == 0)
   {  
      PutInfo(IDS_NOSECOR);
      delete opGrafFile;
      delete opSecFile;
      return;
   }

   DisplayFile *opDispFile = new DisplayFile(for004);
   DISPAR *dispar = opDispFile->GetParameters();
   if (dispar == NULL)
   {  
      PutInfo(IDS_F4NOTF);
      delete opGrafFile;
      delete opSecFile;
      delete opDispFile;
      return;
   }

   if (!jump)
   {
      FileBuff *Dtm = new FileBuff(String(IDS_DTMDAT)); // file "DTM.DAT"
      mr = Dtm->TellNumberOfLines();
      if (mr == 0)
      {
         PutInfo(IDS_DTMNF);
         delete opGrafFile;
         delete opSecFile;
         delete opDispFile;
         delete Dtm;
         return;
      }

      strcpy(parlist,command);

      if (strlen(parlist) <= 3)
      {  Item = (char **) new long[mr];
         char line[80];
         for (i=0; i<mr; i++)
         {  Item[i] = new char[80];
            Dtm->GetNextLine(line);
            strcpy(Item[i],line);
         }
         ListDialog *opList = new ListDialog(IDD_LIST_DIALOG,this,Item, mr);
         int index = opList->GetIndex();
         if (index >= 0)
            strcpy(parlist,Item[index]);
         for (i=0; i<mr; i++)
            delete []Item[i]; // [] found with Bounds Checker
         delete []Item;       // dito
         delete opList;
         delete Dtm;
         if (index < 0)
         {
            PutInfo(IDS_NOITS);
            delete opGrafFile;
            delete opSecFile;
            delete opDispFile;
            return;
         }
      }
   }

   BOOL colorflag = FALSE; 
   if (strcmp(COLFLAG,CStr) == 0) colorflag = TRUE;
   BOOL printcolorflag = FALSE; 
   if (strcmp(PRTCOLFLAG,CStr) == 0) printcolorflag = TRUE;

   opSecFile->GetParLine(parlist);
   int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res
   RECT WinDim = {0,0,(n*640)/4,(n*480)/4};
   TransGrafWin *opWin = 
                new TransGrafWin(SW_SHOWNORMAL,String(IDS_TRANS),WinDim,
                this,opGrafFile,opSecFile,dispar,colorflag,printcolorflag,PLW);
   WORD key = (WORD)opWin->WaitForKeyDown();
   ReRunFlag = opWin->GetResult();
   delete opWin;
   if (key == 999)
   {  
      PutInfo(IDS_ALLZERO);
   }                  
   delete opGrafFile;
   delete opSecFile;
   delete opDispFile;  // found with Bounds Checker

// section for rerunning in a loop

   if (ReRunFlag & 2)
   {  SaveRun();
      if (dpmirun)
         DPMITrans(0);
      else
         Transport(0);
   }
   else if (ReRunFlag & 4)
   {  DeleteScratch();
      if (dpmirun)
         DPMITrans(0);
      else
         Transport(0);
   }
}

VOID MainWindow :: Ellipses(VOID)
{
   char *dim0, *dim1, *dim2, *dim3, *dim7;

   if (busy) return;
   busy = true;
   TransGrafFile  *opFile = new TransGrafFile(for002,NULL,for001);
   dim0 = opFile->GetDimension(0);
   dim1 = opFile->GetDimension(1);
   dim2 = opFile->GetDimension(2);
   dim3 = opFile->GetDimension(3);
   dim7 = opFile->GetDimension(7);
   int nr = opFile->GetNumberOfElements();
   if (nr > 0)
   {  
      for (jp1=0; jp1<3; jp1++) // cascading of up to 3 ellipse displays
      {
	if (!ypos[jp1].inuse)
        {
	  ypos[jp1].inuse = true;
	  break;
	}
      }
      BOOL colorflag = FALSE; 
      if (strcmp(COLFLAG,CStr) == 0) colorflag = TRUE;
      BOOL printcolorflag = FALSE; 
      if (strcmp(PRTCOLFLAG,CStr) == 0) printcolorflag = TRUE;

      int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res
      RECT WinDim = {0,(n*ypos[jp1].pos2)/4,(n*640)/4,(n*453)/4};
      TransEllipseWin *opWin =
                  new TransEllipseWin(SW_SHOWNORMAL,String(IDS_ELI),WinDim,
                              this,opFile,colorflag,printcolorflag,PLW);
      opWin->GetDimensions(dim0,dim1,dim2,dim3,dim7);

      RECT rc = {0,0,640,1};
      AuxWin *opAux = new AuxWin(SW_HIDE,"1",rc);
      int dn = IDD_FLAT_DIALOG; // next line for hi res with small fonts
      if ((n==5) && (GetSystemMetrics(SM_CYCAPTION)==20)) dn = IDD_FLAT2_DIALOG;
      EliDialog *opEliDlg = new EliDialog(dn,opAux,opWin,opFile,ypos[jp1].pos1);
      delete opEliDlg;
      delete opAux;
      delete opWin;
      ypos[jp1].inuse = false;
   }
   else
   {
      PutInfo(IDS_RUNGTF);
   }
   delete opFile;
   busy = false;
}

VOID MainWindow :: Ellipse2(VOID)
{
   char *dim0, *dim1, *dim2, *dim3, *dim7;

   if (busy3) return;
   busy3 = true;
   TransGrafFile  *opFile3 = new TransGrafFile(for002,NULL,for001);
   dim0 = opFile3->GetDimension(0);
   dim1 = opFile3->GetDimension(1);
   dim2 = opFile3->GetDimension(2);
   dim3 = opFile3->GetDimension(3);
   dim7 = opFile3->GetDimension(7);
   int nr = opFile3->GetNumberOfElements();
   if (nr > 0)
   {
      for (jp3=0; jp3<3; jp3++) // cascading of up to 3 ellipse displays
      {
	if (!ypos[jp3].inuse)
        {
	  ypos[jp3].inuse = true;
	  break;
	}
      }
      BOOL colorflag = FALSE; 
      if (strcmp(COLFLAG,CStr) == 0) colorflag = TRUE;
      BOOL printcolorflag = FALSE; 
      if (strcmp(PRTCOLFLAG,CStr) == 0) printcolorflag = TRUE;

      int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res
      RECT WinDim = {0,(n*ypos[jp3].pos2)/4,(n*640)/4,(n*453)/4};
      TransEllipseWin3 *opWin3 =
                  new TransEllipseWin3(SW_SHOWNORMAL,String(IDS_ELI2),WinDim,
                              this,opFile3,colorflag,printcolorflag,PLW);
      opWin3->GetDimensions(dim0,dim1,dim2,dim3,dim7);

      RECT rc = {0,0,640,1};
      AuxWin3 *opAux3 = new AuxWin3(SW_HIDE,"3",rc);
      int dn = IDD_FLAT5_DIALOG; // next line for hi res with small fonts
      if ((n==5) && (GetSystemMetrics(SM_CYCAPTION)==20)) dn = IDD_FLAT6_DIALOG;
      EliDialog3 *opEliDlg3 = new EliDialog3(dn,opAux3,opWin3,opFile3,ypos[jp3].pos1);
      delete opEliDlg3;
      delete opAux3;
      delete opWin3;
      ypos[jp3].inuse = false;
   }
   else
   {
      PutInfo(IDS_RUNGTF);
   }
   delete opFile3;
   busy3 = false;
}

VOID MainWindow :: Ellipses_long(VOID)
{
   char *dim0, *dim1, *dim2, *dim3, *dim7;

   if (busy2) return;
   busy2 = true;
   TransGrafFile  *opFile2 = new TransGrafFile(for002,NULL,for001);
   dim0 = opFile2->GetDimension(4);
   dim1 = opFile2->GetDimension(5);
   dim2 = opFile2->GetDimension(4);
   dim3 = opFile2->GetDimension(5);
   dim7 = opFile2->GetDimension(7);
   int nr = opFile2->GetNumberOfElements();
   if (nr > 0)
   {
      for (jp2=0; jp2<3; jp2++) // cascading of up to 3 ellipse displays
      {
	if (!ypos[jp2].inuse)
        {
	  ypos[jp2].inuse = true;
	  break;
	}
      }
      BOOL colorflag = FALSE; 
      if (strcmp(COLFLAG,CStr) == 0) colorflag = TRUE;
      BOOL printcolorflag = FALSE; 
      if (strcmp(PRTCOLFLAG,CStr) == 0) printcolorflag = TRUE;

      int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res
      RECT WinDim2 = {0,(n*ypos[jp2].pos2)/4,(n*320)/4,(n*453)/4};
      TransEllipseWin2 *opWin2 =
                  new TransEllipseWin2(SW_SHOWNORMAL,String(IDS_ELIL),WinDim2,
                              this,opFile2,colorflag,printcolorflag,PLW);
      opWin2->GetDimensions(dim0,dim1,dim2,dim3,dim7);

      RECT rc = {0,0,640,1};
      AuxWin2 *opAux2 = new AuxWin2(SW_HIDE,"2",rc);
      int dn = IDD_FLAT3_DIALOG; // next line for hi res with small fonts
      if ((n==5) && (GetSystemMetrics(SM_CYCAPTION)==20)) dn = IDD_FLAT4_DIALOG;
      EliDialog2 *opEliDlg2 = new EliDialog2(dn,opAux2,opWin2,opFile2,ypos[jp2].pos1);
      delete opEliDlg2;
      delete opAux2;
      delete opWin2;
      ypos[jp2].inuse = false;
   }
   else
   {
      PutInfo(IDS_RUNGTF);
   }
   delete opFile2;
   busy2 = false;
}

VOID MainWindow :: SaveRun(VOID)
{
   FILE *in, *out, *tst;
   int  i, c;
   char RB[3], WB[3];

   RB[0] = 'r'; RB[1] = 'b'; RB[2] = '\0';
   WB[0] = 'w'; WB[1] = 'b'; WB[2] = '\0';

   int pos = strlen(save0) - 5;

   for (i=0; i<10; i++)
   {  save0[pos] = char('0' + i);
      if ((tst = fopen(save0,RB)) == 0)    // if not yet existing ..
      {  in = fopen(for002,RB);            // .. copy from for002.dat
         if (in)
         {  out = fopen(save0,WB);         // .. to savex.dat (x = 0...9)
            while (1)
            {   c = fgetc(in);
                if (feof(in))
                   break;
                fputc(c,out);
            }
            fclose(in);
            fclose(out);
            break;
         }
      }
      else
         fclose(tst);
   }

   pos = strlen(keep0) - 5;

   for (i=0; i<10; i++)
   {  keep0[pos] = char('0' + i);
      if ((tst = fopen(keep0,RB)) == 0)    // if not yet existing ..
      {  in = fopen(for000,RB);            // .. copy from for000.dat
         if (in)
         {  out = fopen(keep0,WB);         // .. to keepx.dat (x = 0...9)
            while (1)
            {   c = fgetc(in);
                if (feof(in))
                   break;
                fputc(c,out);
            }
            fclose(in);
            fclose(out);
            break;
         }
      }
      else
         fclose(tst);
   }
}

VOID MainWindow :: FxyDac(VOID)
{
      char line[200];
      int q[90]; // 0-29: dacs, 30-59: x-pmdata, 60-89: y-pmdata
      int index[7] = {0,10,20,30,40,60,70};

      FileBuff *For022 = new FileBuff(String(IDS_FOR022)); // file "FOR022.DAT"
      int mr = For022->TellNumberOfLines();
      if (mr <= 6)
      {
         PutInfo(IDS_FOR022NF);
         delete For022;
         return;
      }
      if (mr == 9)
      {
         PutInfo(IDS_FOR022WF);
         delete For022;
         return;
      }
      for (int j=0; j<7; j++)
      {
         For022->GetNextLine(line);
         int i = index[j];
         sscanf(line,String(IDS_10D),&q[i+0],&q[i+1],&q[i+2],&q[i+3],&q[i+4],
                                     &q[i+5],&q[i+6],&q[i+7],&q[i+8],&q[i+9]);
      }
      delete For022;

      q[50] = q[20];  // some more x-pmdata
      q[51] = q[21];
      q[52] = q[22];
      q[53] = q[23];
      q[54] = q[24];

      q[80] = q[25];  // some more y-pmdata
      q[81] = q[26];
      q[82] = q[27];
      q[83] = q[28];
      q[84] = q[29];

      char *qtext = new char[500];
      char *pmtext = new char[500];
      strcpy(qtext,"");
      strcpy(pmtext,"");

      RECT WinDim = {10,5,620,400};
      TextWindow *opWin =
          new TextWindow(SW_SHOWNORMAL,String(IDS_INSXYDAC),WinDim);
      opWin->DisableClose();

      TransInFile *opInput = new TransInFile(for001);
      if (opInput->FxyDac(&q[0],&q[30],&q[60],qtext,pmtext) == 1)
      {
          opWin->WriteLn(String(IDS_TRANSFER));
          opWin->WriteLn(qtext);
          opWin->WriteLn(pmtext);
      }

      opWin->WriteLn(String(IDS_UPDF1));
      WORD key = opWin->WaitForKeyDown();
      if (key == 'Y' || key == 'y')
      {
         opWin->WriteLn(Blank);
         opInput->OverWrite();
         opWin->WriteLn(String(IDS_WRNF1));
         wait(1000);
      }
      delete opWin;
      delete opInput;
      delete qtext;
      delete pmtext;

      if (key == 'Y' || key == 'y')
      {  
         DTM = FALSE;
         if (dpmirun)
            DPMITrans(0);
         else
            Transport(0);
      }
}

VOID MainWindow :: FxyGauss(VOID)
{
      char line[200];
      float q[90]; // 0-29: dacs, 30-59: x-pmdata, 60-89: y-pmdata
      int index[9] = {0,10,20,30,40,50,60,70,80};

      FileBuff *For022 = new FileBuff(String(IDS_FOR022)); // file "FOR022.DAT"
      int mr = For022->TellNumberOfLines();
      if (mr <= 6)
      {
         PutInfo(IDS_FOR022NF);
         delete For022;
         return;
      }
      if (mr < 9)
      {
         PutInfo(IDS_FOR022WF);
         delete For022;
         return;
      }
      for (int j=0; j<9; j++)
      {
         For022->GetNextLine(line);
         int i = index[j];
         sscanf(line,String(IDS_10F),&q[i+0],&q[i+1],&q[i+2],&q[i+3],&q[i+4],
                                     &q[i+5],&q[i+6],&q[i+7],&q[i+8],&q[i+9]);
      }
      delete For022;

      char *qtext = new char[500];
      char *pmtext = new char[500];
      strcpy(qtext,"");
      strcpy(pmtext,"");

      RECT WinDim = {10,5,620,400};
      TextWindow *opWin =
          new TextWindow(SW_SHOWNORMAL,String(IDS_INSXYGAU),WinDim);
      opWin->DisableClose();

      TransInFile *opInput = new TransInFile(for001);
      if (opInput->FxyGauss(&q[0],&q[30],&q[60],qtext,pmtext) == 1)
      {
          opWin->WriteLn(String(IDS_TRANSFER));
          opWin->WriteLn(qtext);
          opWin->WriteLn(pmtext);
      }

      opWin->WriteLn(String(IDS_UPDF1));
      WORD key = opWin->WaitForKeyDown();
      if (key == 'Y' || key == 'y')
      {
         opWin->WriteLn(Blank);
         opInput->OverWrite();
         opWin->WriteLn(String(IDS_WRNF1));
         wait(1000);
      }
      delete opWin;
      delete opInput;
      delete qtext;
      delete pmtext;

      if (key == 'Y' || key == 'y')
      {  
         DTM = FALSE;
         if (dpmirun)
            DPMITrans(0);
         else
            Transport(0);
      }
}

VOID MainWindow :: InsertDacValues(VOID)
{
   char  buffer[80], devname[10];
   int   rv, type;
   float leng, field, hap;
   WORD  key = 0;

   typedef int (WINAPI *LPINISERIALCOMM) (LPSTR);
   typedef int (WINAPI *LPCLOSESERIALCOMM) (void);
   typedef int (WINAPI *LPINIDEVLIST) (LPSTR);
   typedef int (WINAPI *LPDELDEVLIST) (void);
   typedef int (WINAPI *LPRESERVECRATE) (void);
   typedef int (WINAPI *LPFREECRATE) (void);
   typedef int (WINAPI *LPINIKSI) (void);
   typedef long (WINAPI *LPGETDAC) (LPSTR);
   LPINISERIALCOMM lpIniSerialComm;
   LPCLOSESERIALCOMM lpCloseSerialComm;
   LPINIDEVLIST lpIniDevList;
   LPDELDEVLIST lpDelDevList;
   LPRESERVECRATE lpReserveCrate;
   LPFREECRATE lpFreeCrate;
   LPINIKSI lpIniKSI;
   LPGETDAC lpGetDac;

   int mode = 1; // quads only as default
   GetDacDialog *opDlg = new GetDacDialog(IDD_GETDAC_DIALOG,this,mode);
   mode = opDlg->GetResult();
   delete opDlg;
   if (!mode)
      return;
   wait(100); // IniSerial is blocking everithing for about 3 seconds;

   BOOL ok = FALSE;
   BOOL direct = (strncmp(PORT,"COM",3) == 0) ? TRUE : FALSE;
   if (direct)
   {
#ifdef WIN95
      hCamacLib = LoadLibrary("CAMAC32.DLL");
#else
      hCamacLib = LoadLibrary("CAMAC.DLL");
#endif
#ifdef WIN32
      if (hCamacLib <= 0)
      {
         DispObjectMsg("CAMAC(32).DLL not found.");
         return;
      }
#else
      if (hCamacLib <= 32)
      {
         DispObjectMsg("CAMAC(32).DLL not found.");
         return;
      }
#endif
      lpIniSerialComm = (LPINISERIALCOMM)GetProcAddress(hCamacLib,"IniSerialComm");
      lpCloseSerialComm = (LPCLOSESERIALCOMM)GetProcAddress(hCamacLib,"CloseSerialComm");
      lpIniDevList = (LPINIDEVLIST)GetProcAddress(hCamacLib,"IniDevList");
      lpDelDevList = (LPDELDEVLIST)GetProcAddress(hCamacLib,"DelDevList");
      lpReserveCrate = (LPRESERVECRATE)GetProcAddress(hCamacLib,"ReserveCrate");
      lpFreeCrate = (LPFREECRATE)GetProcAddress(hCamacLib,"FreeCrate");
      lpIniKSI = (LPINIKSI)GetProcAddress(hCamacLib,"IniKSI");
      lpGetDac = (LPGETDAC)GetProcAddress(hCamacLib,"GetDac");

      (*lpIniSerialComm)(PORT);
      (*lpIniKSI)();
      if ((*lpIniDevList)(String(IDS_DEVLIS)) > -1)  // file DEVICE.LIS
      {  if (!(*lpReserveCrate)())
         {
            SimpleDialog oDg(IDD_QEND_DIALOG,this,String(IDS_CBUSY));
            if (!oDg.GetResult())
            {  (*lpDelDevList)();
               (*lpCloseSerialComm)();
               FreeLibrary(hCamacLib);
               return;
            }
         }
         ok = TRUE;
      }
   }
   else  // do it via TCP/IP
   {
      char SERVER[20], PORTSTR[20], temp[80], *p;
      strcpy(temp,PORT);
      p = strtok(temp,"$");
      if (p)
         strcpy(SERVER,p);
      else
         return;
      p = strtok(NULL,"$");
      if (p)
         strcpy(PORTSTR,p);
      else
         return;

      int retval = TCPConnect(&sock,SERVER,(int)atol(PORTSTR));
      if (!retval)
         return;

      char recbuff[2048];
      TCPSend(sock,"test TCP",recbuff);
      if (strncmp(recbuff,"*DENY*",6) == 0)
      {
         DispObjectMsg("All server sockets used.");
         return;
      }
      ok = TRUE;
   }

   if (ok)
   {
      TransInFile *opInput = new TransInFile(for001);

      RECT WinDim = {100,5,300,440};
      TextWindow *opWin =
          new TextWindow(SW_SHOWNORMAL,String(IDS_INSERT),WinDim);
      opWin->DisableClose();

      while (opInput->AnalyseInput(mode, type, leng, field, hap, devname) == 1)
      {
	 long value;
	 if (direct)
            value = (*lpGetDac)(devname);
         else
            value = GetDacValue(sock,devname);
         if (value != -77777L)
         {
            rv = opInput->InsertField(type, leng, value, hap, devname, field);
            if (rv == 1)
            {
               sprintf(buffer,String(IDS_KG_FORM),devname, value, field);
               opWin->WriteLn(buffer);
            }
            else if (rv == 0)
            {
               sprintf(buffer,String(IDS_NFCALF),devname);
               opWin->WriteLn(buffer);
            }
         }
         else
         {
            sprintf(buffer,String(IDS_NFDEVL),devname);
            opWin->WriteLn(buffer);
         }
      }
      if (direct)
      {
         (*lpFreeCrate)();
         (*lpDelDevList)();
         (*lpCloseSerialComm)();
         FreeLibrary(hCamacLib);
      }
      else
         TCPClose(sock);

      opWin->WriteLn(String(IDS_UPDF1));
      key = opWin->WaitForKeyDown();
      if (key == 'Y' || key == 'y')
      {
         opWin->WriteLn(Blank);
         opInput->OverWrite();
         opWin->WriteLn(String(IDS_WRNF1));
         wait(1000);
      }
      delete opWin;
      delete opInput;
   }
   else
   {  
      if (direct)
      {
         PutInfo(IDS_DEVLISF);
         (*lpCloseSerialComm)();
         FreeLibrary(hCamacLib);
      }
   }

   if (key == 'Y' || key == 'y')
   {  
      DTM = FALSE;
      if (dpmirun)
         DPMITrans(0);
      else
         Transport(0);
   }
}

#ifdef WIN95

static TBBUTTON tbButtons[] =
{
	{ 0, IDM_EDT_INP,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 1, IDM_COPYF,      TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        { 2, IDM_DISPAR,     TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        { 3, IDM_IFQ,        TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 0, 0,              TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
	{ 4, IDM_TRANS_GRAF, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 5, IDM_REDRAW,     TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 6, IDM_ELLIPSES,   TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        { 7, IDM_SECOND,     TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        { 8, IDM_SAVER,      TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 0, 0,              TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
	{ 9, IDM_BEAMINFO,   TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        {10, IDM_QUADS,      TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        {11, IDM_BENDS,      TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        {12, IDM_QOI,        TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        {13, IDM_BOI,        TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 0, 0,              TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
        {14, IDM_PRINT,      TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        {15, IDM_DELSCRA,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 0, 0,              TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
	{16, IDM_TRANS_FORM, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        {17, IDM_TEDI,       TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        {18, IDM_BEAMLINE,   TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        {19, IDM_BEAM,       TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        {20, IDM_SHIFTS,     TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        {21, IDM_FITS,       TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0}
};

static int nCust[] = {0,1,2,3, 5,6,7,8,9, 11,12,13,14,15, 17,18,
                      20,21,22,23,24,25, -1};

static char szTbStrings[] = "Edt\0Copy\0Dispar\0Ifq\0"
			    "TransGraf\0Redraw\0Ellipse\0Second\0Save\0"
			    "BeamInfo\0Quads\0Bends\0Qoi\0Boi\0"
			    "Print\0DelScra\0"
			    "TransStd\0Tedi\0Beamline\0Beam\0Shifts\0Fits\0";

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
    22,
    Application::GetInstance(),
    IDB_TOOLBAR,
    (LPCTBBUTTON)&tbButtons,
    22,
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

// Next 3 static routines are called from EliDialog, Elidialog2 and EliDialog3 (flatdlg.cpp)
// These Dialogs are hanging on Quit if more than 1 is open at a time.
// So global flags are set false before TerminateDialog();

VOID MainWindow :: UnBusy (void)
{
   busy = false;
   ypos[jp1].inuse = false;
}
VOID MainWindow :: UnBusy2 (void)
{
   busy2 = false;
   ypos[jp2].inuse = false;
}
VOID MainWindow :: UnBusy3 (void)
{
   busy3 = false;
   ypos[jp3].inuse = false;
}

///////////////////////////////////////////////////////////////////////////////

UINT Application :: Run(VOID)
{
   strcpy(CMDLINE,lpszCmdLine);
#ifndef WIN95
   int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res
   RECT WinDim = {(n*30)/4, (n*30)/4, (n*450)/4, (n*320)/4}; // x,y,w,h
#else
   RECT WinDim = {30, 30, 455, 350}; // x,y,w,h
#endif
   opMain = new MainWindow( nCmdShow,String(IDS_TRANS), WinDim );
   return( Start() );
}

///////////////////////////////// EOF /////////////////////////////////////////

