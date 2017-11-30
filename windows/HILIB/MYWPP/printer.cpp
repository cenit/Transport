////////////////////////////////////////////////////////////////////////////////
////////////////////// Implementation of class Printer /////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

#ifdef WINNT
#ifndef WIN95
#define WINNT
#endif
#endif

// local data

static BOOL blPrintAbort;		    /* aborted flag */
static Printer *opPrinter;		    /* current printer */
static Window *opParentWin;         /* parent window to enable after abort */

///////////////////////// printing abort dialog class //////////////////////////

class PrintAbortDialog : public ModelessDialog
{
    StaticText *opName;
    char *cpName;
  protected:
    BOOL Init   ( void );
    long Control( UINT nControlId, UINT nNotificationCode );
  public:
    PrintAbortDialog(Window *opWind, char *cpDocName);
    ~PrintAbortDialog(VOID);
};

static PrintAbortDialog *opPrintAbortDialog;

PrintAbortDialog :: PrintAbortDialog(Window *opWind, char *cpDocName)
                  : ModelessDialog(IDD_PRINT_ABORT_DIALOG,opWind)
{
    cpName = cpDocName;
    Start();
}

PrintAbortDialog :: ~PrintAbortDialog(VOID)
{
    delete opName;
    TerminateDialog();
}

BOOL PrintAbortDialog :: Init(VOID)
{
    opName = new StaticText(IDC_FILE_NAME_TEXT,this);
    opName->SetText(cpName);
    return FALSE;
}

long PrintAbortDialog :: Control( UINT, UINT)
{
    blPrintAbort = TRUE;
    opPrinter->Abort();
    return FALSE;
}

BOOL APIENTRY PrintAbortProc(HANDLE, UINT)
{
    MSG sMsg;

    while (!blPrintAbort && PeekMessage(&sMsg,NULL,NULL,NULL,TRUE))
    {
	if (opPrintAbortDialog && opPrintAbortDialog->GetHandle())
	{
	    if (!IsDialogMessage(opPrintAbortDialog->GetHandle(),&sMsg))
	    {
		TranslateMessage(&sMsg);
		DispatchMessage(&sMsg);
	    }
	}
    }
    return(!blPrintAbort);
}
////////////////////////////////////////////////////////////////////////////////


Printer :: Printer(VOID) : DisplayDevice(IDO_PRINTER)
{
    char caPrintInfo[80];
    LPSTR clpTemp;
    LPSTR clpPrintDriver;
    LPSTR clpPrintPort;

    caPrinterName[0] = '\0';
    caPrinterPort[0] = '\0';
    caPrinterDriver[0] = '\0';
    blError = FALSE;
    opPrintAbortDialog = NULL;

    if (GetProfileString("windows", "Device", (LPSTR) "", caPrintInfo, 80))
    {
	clpTemp = caPrintInfo;
	clpPrintDriver = clpPrintPort = 0;

	while (*clpTemp)
	{
	    if (*clpTemp == ',')
	    {
		*clpTemp++ = 0;

		while (*clpTemp == ' ')
		    clpTemp = AnsiNext(clpTemp);

		if (!clpPrintDriver)
		    clpPrintDriver = clpTemp;
		else
		{
		    clpPrintPort = clpTemp;
		    break;
		}
	    }
	    else
		clpTemp = AnsiNext(clpTemp);
	}

	strncpy(caPrinterName,caPrintInfo,sizeof(caPrinterName));

	if (clpPrintPort)
	    strncpy(caPrinterPort,clpPrintPort,sizeof(caPrinterPort));

	if (clpPrintDriver)
	    strncpy(caPrinterDriver,clpPrintDriver,sizeof(caPrinterDriver));

        char pmodule[32];
        wsprintf (pmodule, "%s.DRV",(LPSTR)caPrinterDriver);
        HMODULE hDriver = LoadLibrary(pmodule);
        if (hDriver < (HMODULE) HINSTANCE_ERROR)
        {
           hdc = CreateDC(caPrinterDriver,caPrinterName,caPrinterPort,NULL);
        }
        else
        {
           LPFNDEVMODE lpfnExtDeviceMode =
                   (LPFNDEVMODE)GetProcAddress(hDriver,(LPSTR)"EXTDEVICEMODE");
           if (lpfnExtDeviceMode)
           {
              int count = lpfnExtDeviceMode(0,hDriver,NULL,caPrinterName,
                                            caPrinterPort,NULL,NULL,0);
              if (count != -1)
              {
                 HGLOBAL hDevMode = GlobalAlloc(GHND, count);
                 if (hDevMode)
                 {
                    LPDEVMODE lpDevMode = (LPDEVMODE)GlobalLock(hDevMode);
                    lpfnExtDeviceMode(0,hDriver,lpDevMode,caPrinterName,
                                      caPrinterPort,NULL,NULL,DM_OUT_BUFFER);
                    hdc = CreateDC(caPrinterDriver,caPrinterName,
                          caPrinterPort,lpDevMode);
                 }
	         else
                 {
                    hdc = 0;
                 }
              }
              else
              {
                  hdc = 0;
              }
           }
           else
           {
               hdc = 0;
           }
        }
        if (hDriver)
           FreeLibrary(hDriver);
    }
    else
    {
       DispObjectMsg("Unable to read printer info in WIN.INI");
       hdc = 0;
    }
}

Printer :: Printer(char *cpName, char *cpDriver, char *cpPort)
         : DisplayDevice(IDO_PRINTER)
{
    strcpy(caPrinterName,cpName);
    strcpy(caPrinterPort,cpPort);
    strcpy(caPrinterDriver,cpDriver);

    blError = FALSE;
    opPrintAbortDialog = NULL;

    hdc = CreateDC(caPrinterDriver,caPrinterName,caPrinterPort,NULL);
}

Printer :: ~Printer(VOID)
{
    opPrintAbortDialog = NULL;
    if (hdc != 0)
       DeleteDC(hdc);
}


BOOL Printer :: Start(char *cpDocName, Window *opWin)
{
    if (hdc != NULL)
    {
	static DOCINFO di = { sizeof(DOCINFO), cpDocName, NULL };

	opPrinter    = this;
        opParentWin  = opWin;
	blPrintAbort = FALSE;
        opPrintAbortDialog = new PrintAbortDialog(opWin,cpDocName);
	opWin->Disable();

	SetAbortProc(hdc,(ABORTPROC)PrintAbortProc);

	if (StartDoc(hdc, &di) > 0)
	{
            RECT rc;

            rc.left   = 0;
            rc.top    = 0;
            rc.right  = GetDeviceCaps(hdc,HORZRES);
            rc.bottom = GetDeviceCaps(hdc,VERTRES);
            SetExtents(rc);

	    if (StartPage(hdc) > 0)
	       return(TRUE);
	}
	else
	{
	    DispObjectMsg("Unable to start printing of %s",cpDocName);
	    blError = TRUE;
	}
    }
    return(FALSE);
}
BOOL Printer :: StartPrintToFile(char *cpDocName, Window *opWin)
{
    if (hdc != NULL)
    {
	static DOCINFO di = { sizeof(DOCINFO), cpDocName, NULL };
	di.lpszOutput="FILE:";

	opPrinter    = this;
        opParentWin  = opWin;
	blPrintAbort = FALSE;
        opPrintAbortDialog = new PrintAbortDialog(opWin,cpDocName);
	opWin->Disable();

	SetAbortProc(hdc,(ABORTPROC)PrintAbortProc);

	if (StartDoc(hdc, &di) > 0)
	{
            RECT rc;

            rc.left   = 0;
            rc.top    = 0;
            rc.right  = GetDeviceCaps(hdc,HORZRES);
            rc.bottom = GetDeviceCaps(hdc,VERTRES);
            SetExtents(rc);

	    if (StartPage(hdc) > 0)
	       return(TRUE);
	}
	else
	{
	    DispObjectMsg("Unable to start printing of %s",cpDocName);
	    blError = TRUE;
	}
    }
    return(FALSE);
}


BOOL Printer :: NextPage(VOID)
{
    if (!blPrintAbort && !blError && (hdc != 0))
    {
	if (EndPage(hdc) > 0)
	{
	    StartPage(hdc);
	    MoveTo(0,0);
	    return(TRUE);
	}
	blError = TRUE;
    }
    return(TRUE);
}


BOOL Printer :: End(Window *opWin)
{
    BOOL blRet = FALSE;

    if (!blPrintAbort && !blError && (hdc != 0))
    {
	EndPage(hdc);
	if (EndDoc(hdc) > 0)
	{
	   blRet = TRUE;
	}
    }

    opWin->Enable();

    if (opPrintAbortDialog)
    {
	delete opPrintAbortDialog;
	opPrintAbortDialog = NULL;
    }

    return(blRet);
}

VOID Printer :: Abort(VOID)
{
    blPrintAbort = TRUE;
}

typedef int (FAR PASCAL *DevProc)(HANDLE,HANDLE,LPSTR,LPSTR);

BOOL Printer :: Setup(Window *opWin)
{
#ifdef WIN32
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

    GetVersionEx (&osvi);
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) // it's Windows 95
    {
       char caName[80];
       strcpy(caName,caPrinterName);

       HANDLE hPrinter;
       OpenPrinter(caName,&hPrinter,NULL);
       PrinterProperties(NULL,hPrinter);   // host window = Desktop
       ClosePrinter(hPrinter);
    }
    else  // it's Windows NT or Windows 3.1x
#endif
    {
       char caDriver[80];
       HMODULE hDriver;

       strcpy(caDriver,caPrinterDriver);

       if (!strchr(caDriver,'.'))
	  strcat(caDriver,".DRV");

       hDriver = LoadLibrary(caDriver);

       if (hDriver > (HMODULE)32)
       {
           DevProc lpDeviceMode;
	   lpDeviceMode = (DevProc)GetProcAddress(hDriver,"DEVICEMODE");

	   if (lpDeviceMode != NULL)
	   {
	       (*lpDeviceMode)(opWin->GetHandle(),hDriver,caPrinterName,
		  caPrinterPort);
	       return(TRUE);
	   }

	   FreeLibrary(hDriver);
       }
       else
	   DispObjectMsg("Unable to load printer driver:  %s  (%x)",
                       caDriver,hDriver);
    }
    return(FALSE);
}

////////////////////////////////// EOF /////////////////////////////////////////
