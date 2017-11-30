///////////////////////////////////////////////////////////////////////////////
// TTMENU.CPP: Urs Rohrer (PSI), August 1992
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
		"http://people.web.psi.ch/rohrer_u/turtle.htm",
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
	LocalFree((HANDLE)npPal);
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

HANDLE MakeIndexHeader(LPBITMAPINFOHEADER lpInfo)
{
    HANDLE hPalInfo;
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

/////////////////////////////// Globals ///////////////////////////////////////

#ifndef WIN95
PushButton *opExit = NULL;
#endif

///////////////////// CMDLINE passed from BCApplication ///////////////////////

char CMDLINE[80], SCRDIR[80], COLFLAG[10], PRTCOLFLAG[10];
char BUFFERNAME[100];

/////////////////////// prototypes for DLL functions //////////////////////////

extern "C"
{
   short APIENTRY TURTLE(char *,LPSTR);
   short APIENTRY PERCENT(short *);
   void  APIENTRY FINISH(void);
}

#ifdef WIN32

int GetPercent(void)
{
    HANDLE hMem;
    CB     *cb;
    int    val;

//  read shared memory to get "%"

    hMem = OpenFileMapping(FILE_MAP_WRITE,FALSE,BUFFERNAME);
    if (hMem)
    {
       cb = (CB *)MapViewOfFile(hMem,FILE_MAP_ALL_ACCESS,0,0,0);
       if (cb)
       {
          val = cb->percent;
          UnmapViewOfFile(cb);
       }
       CloseHandle(hMem);
       return val;
    }
    else
       return 100;
}

HWND HW;

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
    si.dwXCountChars  = 0;
    si.dwYCountChars  = 0;
    si.dwFillAttribute = BACKGROUND_BLUE |
                         BACKGROUND_GREEN |
                         BACKGROUND_INTENSITY;
    si.dwFlags     =
                     STARTF_USESHOWWINDOW |
                     STARTF_USEPOSITION |
                     STARTF_USEFILLATTRIBUTE |
                     STARTF_USESIZE;
    si.wShowWindow = SW_SHOWDEFAULT;
    si.cbReserved2 = 0;
    si.lpReserved2 = NULL;

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

         UpdateWindow(HW);

    do {
          wait(200);
          GetExitCodeProcess(hw,&dwCode);
       } while (dwCode == STILL_ACTIVE) ;

	 CloseHandle(hw); // found with Bounds Checker
}

#endif

///////////////////////// class MainWindow implementation /////////////////////////

MainWindow :: MainWindow(int nCmdShow, LPCSTR szWindowName, RECT rc)
            : Window( nCmdShow, szWindowName, rc)
{
    hWndToolbar = NULL;

//  Buffername for Percent transfer between Turtle32-DLL and this program

    strcpy(BUFFERNAME,"Percent");
    time_t t;
    srand((unsigned)time(&t));
    int num = rand() % 100000000;
    char temp[20];
    itoa(num,temp,10);
    strcat(BUFFERNAME,temp);

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

//  Get Dir, Color flags (C or nC) from command line

cmdl:

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
       strcat(CMDLINE," C C");           // default
//       DispObjectMsg("Ttmenu32 call error:\n"
//		     "Directory name and\n"
//		     "2 color flags have\n"
//		     "to be passed via\n"
//                   "the command line.\n"
//                   "Defaults are taken.");
	char tempx[512];
	strcpy(tempx,"Abnormal start-up:\nNone of the 3 needed parameters\n");
	strcat(tempx,"passed via the command line.\nDefaults '");
	strcat(tempx,CMDLINE);
	strcat(tempx,"' taken.\n\nHint for proper start-up:\n");
	strcat(tempx,"Do not double-click at the TTMENU32.EXE icon.\n");
	strcat(tempx,"Use shortcut generated during installation instead.\n");
	strcat(tempx,"By default it is in the APPS folder.");
	MessageBox(NULL,tempx,"Warning issued by Turtle Framework",MB_OK);
    }

//  check if all 3 parameters are here, otherwise take defaults

    char *p;
    p = strtok(CMDLINE," ");
    if (p)
    {
       strcpy(SCRDIR,p);
       if (SCRDIR[strlen(SCRDIR)-1] != '\\')
          strcat(SCRDIR,"\\");
    }
    p = strtok(NULL," ");
    if (p)
    {
        strcpy(COLFLAG,p);
	p = strtok(NULL," ");
        if (p)
           strcpy(PRTCOLFLAG,p);
        else
        {
	   strcpy(CMDLINE,"");
	   goto cmdl;
        }
    }
    else
    {
       strcpy(CMDLINE,"");
       goto cmdl;
    }

    AnaMtrWndFn(0,0,0,0L);       // connect to ANAMTR.DLL
    RotKnobWndFn(0,0,0,0L);      // connect to ROTKNOB.DLL

    hWnd = this->GetHandle();         /* window handle                */

    // Load and initialize main menu

    opMenu = new Menu(IDR_MAIN_MENU,this);
    {
      char answer[20];
      GetPrivateProfileString("Editor","Gui","NO",answer,20,"Turtle.ini");
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
    opSysMenu->Append(IDM_ABOUT,"&About this Turtle Framework...");

    // default colors

    SetInternalColors();
    GetDefaultColors("default.col");  // from file if available
    opWin2 = NULL;
#ifdef WIN95
    ApplicationBar();
#endif
    Invalidate();

#ifdef WIN32
    HW = this->GetHandle(); // for UpdateWindow in RunInConsoleWindow
#endif
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
       SendMessage(hWndToolbar,TB_AUTOSIZE,0L,0L);
    return DefaultEvent();
}

#endif

long MainWindow :: WmMenuCommand(WPARAM mid)
{
    switch (mid)
    {   case IDM_COMMAND: EBox(""); break;
        case IDM_A_KEY:   EBox("A"); break;
        case IDM_C_KEY:   EBox("C"); break;
        case IDM_D_KEY:   EBox("D"); break;
        case IDM_E_KEY:   EBox("E"); break;
        case IDM_M_KEY:   EBox("M"); break;
        case IDM_P_KEY:   EBox("P"); break;
        case IDM_T_KEY:   EBox("T"); break;
        default: ExecuteCommand(mid,""); break;
    }
    return FALSE;
}

VOID MainWindow :: ExecuteCommand(UINT mid, char *command)
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
               WritePrivateProfileString("Editor","Gui",par,"Turtle.ini");
	  }
	  break;
	case IDM_GUI:
	  {
	    char command[80];
#ifdef WIN95
	    strcpy(command,"tgie32.exe ");
	    strcat(command,SCRDIR);
#else
	    strcpy(command,"tgie.exe ");
	    strcat(command,SCRDIR);
#endif
	    WinExec(command,SW_SHOW);
	  }
	  break;
	case IDM_EDIT_TEXT:
	  {
	       opMenu->SetCheck(IDM_EDIT_GUI,FALSE);
	       opMenu->SetCheck(IDM_EDIT_TEXT,TRUE);
	       edtgui = FALSE;
	
       char par[4] = "NO";
               WritePrivateProfileString("Editor","Gui",par,"Turtle.ini");
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
		strcat(command,SCRDIR);
		strcat(command,"FOR001.DAT");
#else
		strcpy(command,"editor.exe ");
		strcat(command,SCRDIR);
		strcat(command,"FOR001.DAT");
#endif
	     }
	     WinExec(command,SW_NORMAL);
	  }
	  break;
        case IDM_TEDI: TurtOut(); break;
        case IDM_TURTLE: 
          {
             dpmirun = FALSE;
#ifndef WIN32
             Turtle();
#else
             int rv = Turtle1();
             Turtle2(rv);
#endif
          }
          break;
        case IDM_TURTLED: dpmirun = TRUE; DPMITurtle(); break;
#ifdef WIN32
	case IDM_HPLOT:
	{
	   char cmd[100];
	   strcpy(cmd,"GRATUR32 ");
	   strcat(cmd,SCRDIR);
	   strcat(cmd," 1234");
//	   WinExec(cmd,SW_SHOW);
//         here a replacement for WinExec (with some extra features):
	   {
	     BOOL rv;
	     STARTUPINFO si;
	     PROCESS_INFORMATION pi;
	     char title[40];
	     strcpy(title,"Gratur32 Process Window");
	     memset(&si,0,sizeof(STARTUPINFO));    
	     si.cb = sizeof(STARTUPINFO);
	     si.wShowWindow = SW_SHOWNOACTIVATE;
	     si.lpTitle     = &title[0];
	     si.dwX         = 480;
	     si.dwY         = 100;
	     si.dwXSize     = 800;
	     si.dwYSize     = 420;
	     si.dwFlags = STARTF_USESHOWWINDOW |
	                  STARTF_USEPOSITION |
	                  STARTF_USEFILLATTRIBUTE |
	                  STARTF_USESIZE;
	     si.dwFillAttribute = BACKGROUND_BLUE |
	                          BACKGROUND_GREEN |
	                          BACKGROUND_INTENSITY;
	     rv=CreateProcess(NULL,cmd,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
	     if (!rv)
	     {
		DWORD errorcode = GetLastError();
		char text[100];
		sprintf(text,"error # = %d",errorcode);
		MessageBox(NULL,text,"CreateProcess failed",MB_OK);
	     }
	     CloseHandle(pi.hProcess);
	     CloseHandle(pi.hThread);
	   }
//
//	   get number of histograms and number of rays for HPLOT dialog
//
	   int NH;
	   int NR;
	   {
	     SimpleDialog *opDlg;
	     int ok, Navel;
	     IOFILE *dp;
	     int Numb[100];
	     char filename[100];

	     strcpy(filename,SCRDIR);
	     strcat(filename,"FOR100.DAT");
	     IOFile *fp = new IOFile(filename,ok);
	     if (ok == -2)
	     {  opDlg = new SimpleDialog(IDD_FOR100_DIALOG,this,
                               "MetaFile FOR100.DAT not found.");
	        delete opDlg;
	        delete fp;
	        return;
	     }
	     if (ok == -1)
	     {  opDlg = new SimpleDialog(IDD_INFO_DIALOG,this,
                               "Sorry, error in FOR100.DAT");
	        delete opDlg;
	        delete fp;
	        return;
	     }
	     dp = fp->GetDataPointer();

//	     Compute number of Histograms

	     int Nhist = -1;
	     for (int i=0; i<dp->nhists; i++)
	     {  Navel = dp->mtype[i];
	        if ((Navel == 1) || (Navel == 3))
	        {  Nhist++;
	           Numb[Nhist] = i;
	        }
	        if (dp->mtype[dp->nhists] == 2) Nhist--;
	     }
	     Nhist++;
	     NH = Nhist+dp->loss;
	     NR = dp->norays;
	     delete fp;
	   }
	   strcpy(cmd,SCRDIR);
	   strcat(cmd,"GRATUR.INP");
	   HplotDialog(IDD_HPLOT_DIALOG,this,cmd,NH,NR);
	   break;
	}
#endif
        case IDM_MUSCAT:
        {
#ifdef WIN32
#ifdef WIN95
//
//      2 threads: first start Fortran program and then Muscat dialog:
//
	   char cmd[100];
	   char cmd2[100];
	   strcpy(cmd,"MUSCAT32 ");
	   strcat(cmd,SCRDIR);
	   strcat(cmd," 1234");
//	   WinExec(cmd,SW_SHOW);
//         here a replacement for WinExec (with some extra features):
	   {
	     BOOL rv;
	     STARTUPINFO si;
	     PROCESS_INFORMATION pi;
	     char title[40];
	     strcpy(title,"Muscat Output");
	     memset(&si,0,sizeof(STARTUPINFO));    
	     si.cb = sizeof(STARTUPINFO);
	     si.wShowWindow = SW_SHOWNOACTIVATE;
	     si.lpTitle     = &title[0];
	     si.dwX         = 480;
	     si.dwY         = 100;
	     si.dwXSize     = 800;
	     si.dwYSize     = 420;
	     si.dwFlags = STARTF_USESHOWWINDOW |
	                  STARTF_USEPOSITION |
	                  STARTF_USEFILLATTRIBUTE |
	                  STARTF_USESIZE;
	     si.dwFillAttribute = BACKGROUND_BLUE |
	                          BACKGROUND_GREEN |
	                          BACKGROUND_INTENSITY;
	     rv=CreateProcess(NULL,cmd,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
	     if (!rv)
	     {
		DWORD errorcode = GetLastError();
		char text[100];
		sprintf(text,"error # = %d",errorcode);
		MessageBox(NULL,text,"CreateProcess failed",MB_OK);
	     }
	     CloseHandle(pi.hProcess);
	     CloseHandle(pi.hThread);
	   }
	   strcpy(cmd,SCRDIR);
	   strcat(cmd,"MUSCAT.INP");
	   strcpy(cmd2,SCRDIR);
	   strcat(cmd2,"MUSCAT.PAR");
	   MuscatDialog(IDD_MUSCAT_DIALOG,this,cmd,cmd2);
	   break;
#else
	   char cmd[100];
	   strcpy(cmd,"MUSCAT ");
	   strcat(cmd,SCRDIR);
           RunInConsoleWindow(cmd,"Multiple Scattering Parameters",0,0,632,420); break;
	   break;
#endif
#else
	   WinExec("muscat",SW_SHOW);
	   HANDLE ha, ht = this->GetHandle();

	   do {
		wait(100);
		ha = GetActiveWindow();  // wait until tty-window is active
	      }  while (ha == ht);

	   HANDLE hp = GetNextWindow(ha,GW_HWNDPREV);
	   SetWindowPos(ha,hp,10,10,600,400,SWP_SHOWWINDOW);
	   break;
#endif
        }
        case IDM_COPYF: CopyFile(command); break;
        case IDM_DELFILE: DeleteFile(command); break;
        case IDM_PRINT: PrintFile(command); break;
        case IDM_PRINT_SETUP: PrinterSetup(); break;
	case IDM_HISTO: Histogramms(); break;
        case IDM_STOP: TStop(); break;
	case IDM_DEFAULT_COLORS: SetInternalColors();
                                 if (opWin2) opWin2->NewPenColors();
                                 break;
	case IDM_GET_COLORS: GetColors("*.col"); break;
	case IDM_SAVE_COLORS: SaveColors("*.col"); break;
	case IDM_SCREEN_BACK: ColorMenu(0); break;
	case IDM_AXIS_LINES: ColorMenu(1); break; 
	case IDM_AXIS_TEXT: ColorMenu(2); break;
	case IDM_HISTO_BODY: ColorMenu(3); break;
	case IDM_HISTO_BACK: ColorMenu(4); break;
	case IDM_LEGO_BODY: ColorMenu(5); break;
	case IDM_GRID_LINES: ColorMenu(6); break;
	case IDM_CONT_BACK: ColorMenu(7); break;
	case IDM_CONT_LINE1: ColorMenu(8); break;
	case IDM_CONT_LINE2: ColorMenu(9); break;
        default: break;
    }
}

#ifdef WIN32
///////////////////////////////////////////////////////////////////////////////
////////////////////////  Hook procedure for ColorMenu  ///////////////////////
//////////////////////////////initializes 3D-look /////////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern "C"
{
   HBRUSH hBrush2 = NULL;  // Global

#ifdef __BORLANDC__
#pragma argsused
#endif
 
   long APIENTRY HookProc2(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
   {
       switch( msg )
       {
#ifndef WIN95
          case WM_INITDIALOG:
          {
             Ctl3dSubclassDlg(hDlg,CTL3D_ALL);
             return FALSE;
          }
#endif
          case WM_CTLCOLORDLG:
          case WM_CTLCOLORSTATIC:
          case WM_CTLCOLORSCROLLBAR:
          case WM_CTLCOLORMSGBOX:
//          case WM_CTLCOLORLISTBOX:
          case WM_CTLCOLOREDIT:
          case WM_CTLCOLORBTN:
          {
             SetBkColor((HDC)wParam,GetSysColor(COLOR_BTNFACE));
             SetTextColor((HDC)wParam,RGB(0,0,0));
             return ((DWORD)hBrush2);
          }

          default:
             return FALSE;
       }
   }
}
#endif

VOID MainWindow :: ColorMenu(int index)
{
    CHOOSECOLOR cc;
    COLORREF    clr;
    COLORREF    aclrCust[16];

    union
    {  COLORS c;
       int buffer[3*NUMBER_OF_COLORS];
    } C;

    for (int i=0; i<16; i++)
       aclrCust[i] = RGB(255,255,255);

    C.c = Col;
    clr = RGB(C.buffer[3*index],C.buffer[3*index+1],C.buffer[3*index+2]);

    memset(&cc,0,sizeof(CHOOSECOLOR));

#ifdef WIN32
    FARPROC lpHookProc2 = MakeProcInstance((FARPROC)HookProc2,
                                           Application::GetInstance());
    hBrush2 = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));  // background brush
#endif

    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = this->GetHandle();
    cc.rgbResult = clr;
    cc.lpCustColors = aclrCust;
#ifdef WIN32
    cc.Flags = CC_FULLOPEN | CC_RGBINIT | CC_ENABLEHOOK;
    cc.lpfnHook = (LPOFNHOOKPROC)lpHookProc2;
#else
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
#endif

    if (ChooseColor(&cc))
    {  C.c = Col;
       clr = cc.rgbResult;
       C.buffer[3*index]   = GetRValue(clr);
       C.buffer[3*index+1] = GetGValue(clr);
       C.buffer[3*index+2] = GetBValue(clr);
       Col = C.c;
       if (opWin2) opWin2->NewPenColors();
    }

#ifdef WIN32
    FreeProcInstance(lpHookProc2);
    DeleteObject(hBrush2);
#endif
}

VOID MainWindow :: SetInternalColors()
{
    Col.Screen.r    = 255;  Col.Screen.g    = 255;  Col.Screen.b    = 0;
    Col.AxisLines.r = 0;    Col.AxisLines.g = 0;    Col.AxisLines.b = 0;
    Col.AxisText.r  = 0;    Col.AxisText.g  = 0;    Col.AxisText.b  = 0;
    Col.HistBody.r  = 255;  Col.HistBody.g  = 0;    Col.HistBody.b  = 0;
    Col.HistBack.r  = 0;    Col.HistBack.g  = 255;  Col.HistBack.b  = 0;
    Col.LegoBody.r  = 0;    Col.LegoBody.g  = 255;  Col.LegoBody.b  = 0;
    Col.GridLines.r = 0;    Col.GridLines.g = 0;    Col.GridLines.b = 0;
    Col.ContBack.r  = 0;    Col.ContBack.g  = 0;    Col.ContBack.b  = 0;
    Col.ContLine1.r = 255;  Col.ContLine1.g = 0;    Col.ContLine1.b = 0;
    Col.ContLine2.r = 0;    Col.ContLine2.g = 255;  Col.ContLine2.b = 0;
}

VOID MainWindow :: GetDefaultColors(char *FileName)
{
    FILE  *fp;
    union
    {  COLORS c;
       int buffer[3*NUMBER_OF_COLORS];
    } C;

    if ((fp = fopen(FileName,"r")) != NULL)
    {  
       for (int i=0; i<3*NUMBER_OF_COLORS; i++)
          fscanf(fp,"%d",&C.buffer[i]);
       fclose(fp);
       Col = C.c;
    }
}

VOID MainWindow :: GetColors(char *Ext)
{
    FILE  *fp;
    union
    {  COLORS c;
       int buffer[3*NUMBER_OF_COLORS];
    } C;

    char FltStr[] = "Color files(*.COL)|*.col|All files(*.*)|*.*|";
    char Dir[] = "";
    OpenFileDlg *opF = new OpenFileDlg(this->GetHandle(), SCRDIR, FltStr, Ext);
    if (opF->GetResult())
    {  
       if ((fp = fopen(opF->GetFileName(),"r")) != NULL)
       {  
          for (int i=0; i<3*NUMBER_OF_COLORS; i++)
             fscanf(fp,"%d",&C.buffer[i]);
          fclose(fp);
          Col = C.c;
          if (opWin2) opWin2->NewPenColors();
       }
       else
       {
          SimpleDialog oDg(IDD_QEND_DIALOG,this,"No colors saved yet");
       }
       delete opF;
    }
    else
    {  delete opF;
       return;
    }
}

VOID MainWindow :: SaveColors(char *Ext)
{
    FILE  *fp;
    union
    {  COLORS c;
       int buffer[3*NUMBER_OF_COLORS];
    } C;

    char FltStr[] = "Color files(*.COL)|*.col|All files(*.*)|*.*|";
    char Dir[] = "";
    SaveFileDlg *opF = new SaveFileDlg(this->GetHandle(), SCRDIR, FltStr, Ext);
    if (opF->GetResult())
    {  
       if ((fp = fopen(opF->GetFileName(),"w")) != NULL)
       {
          C.c = Col;
          for (int i=0; i<3*NUMBER_OF_COLORS; i++)
          {  if (i!=0 && i%3==0)
                fprintf(fp,"\n");
             fprintf(fp,"%d ",C.buffer[i]);
          }
          fclose(fp);
       }
    }
    else
    {  delete opF;
       return;
    }
}

VOID MainWindow :: EBox(char *cc)
{
    StringDialog *opStDlg;
    int   i;
    char  command[40] = "", comm[20] = "", par[20] = "";

    UINT  mid[] = {IDM_EXIT,    IDM_TURTLE,     IDM_TURTLED,   IDM_MUSCAT,
                   IDM_EDT,     IDM_COPYF,      IDM_DELFILE,
                   IDM_PRINT,   IDM_PRINT_SETUP,IDM_HISTO,
                   IDM_ABOUT,   IDM_EDT,        IDM_TEDI,
                   0 };

    char *cmp[] = {"EXIT",      "TU",           "TUB",         "MU",
                   "EDIT",      "COPY",         "DEL",
                   "PRINT",     "PS",           "TUG",
                   "A",         "EDT",          "TE",
                   "" };

    opStDlg = new StringDialog(IDD_STRING_DIALOG,this,cc);
    if (opStDlg->GetResult())
    {   strcpy(command,opStDlg->GetString());
        delete opStDlg;
        AnsiUpper(command);
        sscanf(command,"%s %s",&comm[0],&par[0]); // separate off ev. parameter
        i = 0;
        while (strlen(cmp[i]))
        {  if (strcmp(cmp[i],comm) == 0)
           {  ExecuteCommand(mid[i],command);
              break;
           }
           i++;
        }

        if ( strlen(cmp[i]) == 0 )
        {  SimpleDialog oDg(IDD_QEND_DIALOG,this,"Unknown command, Help ?");
           if (oDg.GetResult())
           {    SimpleDialog oDlg(IDD_HELP_DIALOG,this,
                                 "List of all keyboard commands");
           }
        }
    }
    else
       delete opStDlg;
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

long MainWindow :: WmPaint( const PAINTSTRUCT &ps )
{
    DrawBitmap(this, ps.hdc);
    return FALSE;
}

void MainWindow :: DrawBitmap(DisplayDevice *opDev, HDC hdcscr)
{
   HBITMAP hBitmap;
   BITMAP  bm;
   HDC     hdcMem;
   RECT    rc;
   POINT   ptStart, ptSize, ptOrg, ptStretch;

#ifdef WIN95xxx

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
    if (opMenu)
    {  delete opMenu;
       opMenu = NULL;
    }
    if (opSysMenu)
    {  delete opSysMenu;
       opSysMenu = NULL;
    }

#ifndef WIN95
    if (opExit)
    {  delete opExit;
       opExit = NULL;
    }
#endif

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
   char comm[50], Filename[100];
   int n = sscanf(command,"%s %s",&comm[0],&Filename[0]);

   if (n <= 1)
   {  
      char FltStr[] = "(*.dat*)|*.dat*|(*.txt*)|*.txt*|(*.lis*)|*.lis*|(*.*)|*.*|";
      char Ext[] = "*.dat*";
      char Dir[] = "";
      OpenFileDlg *opF = new OpenFileDlg(this->GetHandle(), SCRDIR, FltStr, Ext);
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

#ifndef WIN32

int MainWindow :: Turtle(void)
{
    int retval;
    SimpleDialog *opDlg;

    opMeter = new MeterDialog(IDD_METER_DIALOG,this,
                              GetParent(this->GetHandle()));
    SetTimer(this->GetHandle(), 1, 1000, NULL);

    retval = TURTLE(SCRDIR,BUFFERNAME);

    KillTimer(this->GetHandle(), 1);

    wait(1000);
    DoSound();
    delete opMeter;
    opMeter = 0;

//    this->SetFocus();

    if (retval == 1)
    {  opDlg = new SimpleDialog(IDD_INFO_DIALOG,this,"Error in FOR001.DAT");
       delete opDlg;
    }
    if (retval == 2)
    {  opDlg = new SimpleDialog(IDD_INFO_DIALOG,this,"FOR001.DAT not found");
       delete opDlg;
    }
    return retval;
}

#else

int MainWindow :: Turtle1(void)
{
    int retval;

    opMeter = new MeterDialog(IDD_METER_DIALOG,this,
                              GetParent(this->GetHandle()));
    SetTimer(this->GetHandle(), 1, 1000, NULL);

    retval = TURTLE(SCRDIR,BUFFERNAME);

    return retval;
}

void MainWindow :: Turtle2(int retval)
{
    SimpleDialog *opDlg;

    KillTimer(this->GetHandle(), 1);
    wait(1000);
    DoSound();
    delete opMeter;

    if (retval == 1)
    {  opDlg = new SimpleDialog(IDD_INFO_DIALOG,this,"Error in FOR001.DAT");
       delete opDlg;
    }
    if (retval == 2)
    {  opDlg = new SimpleDialog(IDD_INFO_DIALOG,this,"FOR001.DAT not found");
       delete opDlg;
    }
}

#endif

VOID MainWindow :: DPMITurtle(void)
{
    char PROG[100];
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

#ifndef WIN32
    strcpy(PROG,"TURTEM32");
#else
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
    GetVersionEx(&osvi);
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
    {
//       strcpy(PROG,"Turtle95 ");
       strcpy(PROG,"TurtleNT.pif ");
       strcat(PROG,SCRDIR);
    }
    else
    {
       strcpy(PROG,"TurtleNT ");
       strcat(PROG,SCRDIR);
    }
#endif

#ifndef WIN32

    WinExec(PROG,SW_SHOW);

    HANDLE ha, ht = this->GetHandle();

    do {
          wait(100);
          ha = GetActiveWindow();  // wait until tty-window is active
       }  while (ha == ht);

    HANDLE hp = GetNextWindow(ha,GW_HWNDPREV);
    SetWindowPos(ha,hp,x,y,w,h,SWP_SHOWWINDOW);
    httywin = ha;

//  check every 1/2 second if still open

    SetTimer(this->GetHandle(),1,500,NULL);

#else

    RunInConsoleWindow(PROG," Turtle ",x,y,w,h);
    wait(500);
    char cmd[100];
    strcpy(cmd,SCRDIR);
    strcat(cmd,"STOPTURT");
    remove(cmd);
    DoSound();

#endif
}


VOID MainWindow :: TStop(VOID)
{
    SimpleDialog oDlg(IDD_QEND_DIALOG,this,
                     "Do you really want to stop ?");
    if (oDlg.GetResult())
    {  
       if (dpmirun)
       {
	  char cmd[100];
	  strcpy(cmd,SCRDIR);
	  strcat(cmd,"STOPTURT");
          FILE *fh = fopen(cmd,"w");
          fclose(fh);

#ifndef WIN32
          if (httywin)
             ::SetFocus(httywin);
#endif
       }
       else
          FINISH();
    }
#ifndef WIN32
    else
    {
       if (dpmirun)
         if (httywin)
            ::SetFocus(httywin);
    }
#endif
}

long MainWindow :: WmTimer(UINT id)
{
#ifndef WIN32
    short val;
#endif
    int   perc;

    if (dpmirun)
    {  
       if (!IsWindow(httywin))
       {
          KillTimer(this->GetHandle(),id);
	  char cmd[100];
	  strcpy(cmd,SCRDIR);
	  strcat(cmd,"STOPTURT");
          remove(cmd);
          DoSound();
          httywin = 0;
       }
    }
    else
    {
#ifdef WIN32
       perc = GetPercent();
#else
       perc = (int)PERCENT(&val);
#endif
       opMeter->SetValue(perc);
    }
    return FALSE;
}

VOID MainWindow :: PrinterSetup(VOID)
{
   Printer oPrinter;
   oPrinter.Setup(this);
}

VOID MainWindow :: PrintFile(char *command)
{
    char  buffer[180], comm[10];
    FILE *fp;
    char  FileName[50] = "";

    sscanf(command,"%s %s",&comm[0],&FileName[0]);

    if (strlen(FileName) == 0)  // no parameter given
    {
       char FltStr[] = "(*.dat*)|*.dat*|(*.txt*)|*.txt*|(*.lis*)|*.lis*|(*.*)|*.*|";
       char Ext[] = "*.dat*";
       char Dir[] = "";
       char Title[] = "File to print";
       OpenFileDlg *opF = new OpenFileDlg(this->GetHandle(), SCRDIR,
                                          FltStr, Ext, Title);
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

    opPrt->Start("Output of data file from TURTLE",this);

    int h_print = opPrt->GetUserHeight();
    int inc = h_print / 60;

    int fs;

    fs = h_print / 60;

    Font *opFont = new Font("Courier New");
    opFont->SetPointSize(fs);
    Font *opOldFont = opPrt->SetFont(opFont);

    int x     = opPrt->GetUserWidth()/20;
    int y     = inc;
    int index = 1;

    AnsiLowerBuff(FileName,strlen(FileName));
    BOOL IsFor002 = !strcmp(FileName,"for002.dat");

    if ((fp = fopen(FileName,"r")) != NULL)
    {  while ( ! feof(fp) )
       {  if (fscanf(fp,"%[^\n]",&buffer[0]))
          {  fgetc(fp);
             if (buffer[0] == '1' && IsFor002) // form feed in FOR002.DAT
             {  y = inc; index = 1;
                opPrt->NextPage();
                buffer[0] = ' ';
                SetFont(opOldFont);
                opOldFont = opPrt->SetFont(opFont);
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
    GetProfileString("windows","Device","",info,80);
    AnsiLowerBuff(info,strlen(info));
    if (strstr(info,"c:\\ps810.prn") != NULL)
       WinExec("VPP c:\\ps810.prn",SW_MAXIMIZE);
}

VOID MainWindow :: TurtOut()
{
    FILE *fp;
    SimpleDialog *opDlg;

    char filename[100];
    strcpy(filename,SCRDIR);
    strcat(filename,"FOR002.DAT");
    if ((fp = fopen(filename,"r")) == NULL)
    {  
       opDlg = new SimpleDialog(IDD_INFO_DIALOG,this,"FOR002.DAT not found");
       delete opDlg;
       return;
    }
    else
       fclose(fp);

    int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res

    RECT WinDim = { 0, 0, (n*640)/4, (n*480)/4 };
    opWin = new TextWindow(SW_SHOWNORMAL,"Turtle Output",WinDim);
    opWin->ShowFile(filename);

#ifdef WINNT
    opWin->Home();
    opWin->WaitForKeyDown();
#endif

}

VOID MainWindow :: CopyFile(char *command)
{
    char SourceName[50] = "", DestName[50] = "", comm[10];
    FILE *in, *out;
    SimpleDialog *opDlg2;

    sscanf(command,"%s %s %s",&comm[0],&SourceName[0],&DestName[0]);

    // select source file if not given

    if (strlen(SourceName) == 0)
    {  
       char FltStr[] = "(*.dat*)|*.dat*|(*.txt*)|*.txt*|(*.lis*)|*.lis*|(*.*)|*.*|";
       char Ext[] = "*.dat*";
       char Dir[] = "";
       char Title[] = "Source File";
       OpenFileDlg *opF = new OpenFileDlg(this->GetHandle(), SCRDIR,
                                          FltStr, Ext,Title);
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
       char FltStr[] = "(*.dat*)|*.dat*|(*.txt*)|*.txt*|(*.lis*)|*.lis*|(*.*)|*.*|";
       char Ext[] = "*.dat*";
       char Dir[] = "";
       char Title[] = "Destination File";
       SaveFileDlg *opF = new SaveFileDlg(this->GetHandle(), SCRDIR,
                                          FltStr, Ext, Title);
       if (opF->GetResult())
       {  strcpy(DestName,opF->GetFileName());
          delete opF;
       }
       else
       {  delete opF;
          return;
       }
    }

    // source == dest ?

    if (strcmp(AnsiUpper(SourceName),AnsiUpper(DestName)) == 0)
    {  opDlg2 = new SimpleDialog(IDD_INFO_DIALOG,this,
                                "Source equal destination, brrr !!!");
       delete opDlg2;
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
    {  opDlg2 = new SimpleDialog(IDD_INFO_DIALOG,this,
                                "Cannot open source file");
       delete opDlg2;
       return;
    }
    if ((out = fopen(DestName,"wt")) == NULL)
    {  opDlg2 = new SimpleDialog(IDD_INFO_DIALOG,this,
                               "Cannot open destination file");
       delete opDlg2;
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

VOID MainWindow :: DeleteFile(char *command)
{
    char FileName[50] = "", comm[10];

    int n = sscanf(command,"%s %s",&comm[0],&FileName[0]);

    if (n <= 1)                         // if no parameters given
    {  
       char FltStr[] = "(*.dat*)|*.dat*|(*.txt*)|*.txt*|(*.lis*)|*.lis*|(*.*)|*.*|";
       char Ext[] = "*.dat*";
       char Dir[] = "";
       char Title[] = "File to delete";
       OpenFileDlg *opF = new OpenFileDlg(this->GetHandle(), SCRDIR,
                                          FltStr, Ext, Title);
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

VOID MainWindow :: Histogramms(void)
{
    SimpleDialog *opDlg;
    TugDialog *opTug;
    int ok, Navel;
    IOFILE *dp;
    int Numb[100];
    TUGDATA TugData = {1001,10,45,20,0,60,CONT,FRONT,LIN,INCR};

    char filename[100];
    strcpy(filename,SCRDIR);
    strcat(filename,"FOR100.DAT");
    IOFile *fp = new IOFile(filename,ok);
    if (ok == -2)
    {  opDlg = new SimpleDialog(IDD_FOR100_DIALOG,this,
                               "MetaFile FOR100.DAT not found.");
       delete opDlg;
       delete fp;
       return;
    }
    if (ok == -1)
    {  opDlg = new SimpleDialog(IDD_INFO_DIALOG,this,
                               "Sorry, error in FOR100.DAT");
       delete opDlg;
       delete fp;
       return;
    }
    dp = fp->GetDataPointer();

//  Compute number of Histograms

    int Nhist = -1;
    for (int i=0; i<dp->nhists; i++)
    {  Navel = dp->mtype[i];
       if ((Navel == 1) || (Navel == 3))
       {  Nhist++;
          Numb[Nhist] = i;
       }
       if (dp->mtype[dp->nhists] == 2) Nhist--;
    }
    Nhist++;

    BOOL colorflag = FALSE;
    BOOL printcolorflag = FALSE;
    if (strcmp(COLFLAG,"C") == 0) colorflag = TRUE;
    if (strcmp(PRTCOLFLAG,"C") == 0) printcolorflag = TRUE;

    int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res

    RECT WinDim = {0,0,(n*640)/4,(n*480)/4};
    opWin2 = new GrafWin(SW_SHOWNORMAL,"Turtle Histogram",WinDim,this,
                         colorflag,printcolorflag,dp,&Col);

    while (1)
    {
       opTug = new TugDialog(IDD_TUG_DIALOG,opWin2,
                             Nhist+dp->loss,dp->norays,&TugData);
       if (!opTug->GetResult())
          break;
       delete opTug;
       opTug = 0L;
       opWin2->Erase();

//     normal histograms

       int TugDatanh = TugData.nh;
       BOOL statistics = FALSE; 
       if (TugData.nh >= 1000)  // if +1000, then display statistical info with histogramms
       {
          TugDatanh -= 1000;
          statistics = TRUE;
       }

       if (TugDatanh <= Nhist)
       {  Navel = dp->mtype[Numb[TugDatanh-1]];
          if (Navel == 1)                       // 1-Dim
          {   int K = Numb[TugDatanh-1];
	      if (statistics) K += 1000;
              opWin2->Hist(FALSE,K,TugData);
          }
          else if (Navel == 3)                  // 2-Dim
          {   int K = Numb[TugDatanh-1];
	      if (statistics) K += 1000;
              switch(TugData.plotyp)
              {  case CONT:
                   opWin2->ContPlot(FALSE,K,TugData);
                   break;
                 case LEGO:
                   opWin2->LegoPlot(FALSE,K,TugData);
                   break;
                 case GRID:
                   opWin2->GridPlot(FALSE,K,TugData);
                   break;
                 default:
                   break;
              }
          }
//          opWin2->SetFocus();
       }

//     histograms of particles lost

       else if (TugDatanh <= Nhist + dp->loss)
       {  if (dp->nuny[TugDatanh - Nhist - 1] == 0) // 1-Dim
          {   int K = TugDatanh-Nhist-1;
	      if (statistics) K += 1000;
	      opWin2->Hist(TRUE,K,TugData);
          }
          else                                   // 2-Dim
          {   int K = TugDatanh-Nhist-1;
	      if (statistics) K += 1000;
              switch(TugData.plotyp)
              {  case CONT:
                   opWin2->ContPlot(TRUE,K,TugData);
                   break;
                 case LEGO:
                   opWin2->LegoPlot(TRUE,K,TugData);
                   break;
                 case GRID:
                   opWin2->GridPlot(TRUE,K,TugData);
                   break;
                 default:
                   break;
              }
          }
//          opWin2->SetFocus();
       }

//     end of loop

       if (opWin2->WaitForKeyDown() == 777)
          break;                   // exit in popup menu
       if (TugData.incr == INCR)
       {  if (TugDatanh < Nhist+dp->loss)
             (TugDatanh)++;       // autoincrement histogram counter
          else
             TugDatanh = 1;       // reset to beginning
       }
       else if (TugData.incr == DECR)
       {  if (TugDatanh > 1)
             (TugDatanh)--;       // autodecrement histogram counter
          else
             TugDatanh = Nhist+dp->loss; // reset to end
       }
       TugData.nh = TugDatanh;
       if (statistics)
          TugData.nh += 1000;
    }

    if (opTug) delete opTug;
    delete fp;
    delete opWin2;
    opWin2 = NULL;
}

#ifdef WIN95

static TBBUTTON tbButtons[] =
{
	{ 0, IDM_EDT_INP, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 1, IDM_COPYF,   TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 0, 0,           TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
	{ 2, IDM_TURTLE,  TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 3, IDM_TURTLED, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 4, IDM_MUSCAT,  TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 0, 0,           TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
	{ 5, IDM_PRINT,   TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 6, IDM_TEDI,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        { 7, IDM_HISTO,   TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        { 8, IDM_HPLOT,   TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
	{ 0, 0,           TBSTATE_ENABLED, TBSTYLE_SEP,    {0, 0}, 0, 0},
        { 9, IDM_STOP,    TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0},
        {10, IDM_ABOUT,   TBSTATE_ENABLED, TBSTYLE_BUTTON, {0, 0}, 0, 0}
};

static int nCust[] = {0,1, 3,4,5, 7,8,9,10, 12,13, -1};

static char szTbStrings[] =	"Edt\0Copy\0"
				"Turtle\0TurtleDll\0Muscat\0"
				"Print\0Tedi\0Histo\0Hplot\0"
				"Stop\0About\0" ;
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
    14,
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
#ifndef WIN95
   int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res
   RECT WinDim = {(n*30)/4, (n*30)/4, (n*450)/4, (n*320)/4}; // x,y,w,h
#else
   RECT WinDim = {30, 30, 455, 350}; // x,y,w,h
#endif
   opMain = new MainWindow( nCmdShow, "Turtle Framework", WinDim );
   return( Start() );
}

///////////////////////////////// EOF /////////////////////////////////////////

