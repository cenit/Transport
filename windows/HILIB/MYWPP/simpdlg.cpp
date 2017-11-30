///////////////////////////////////////////////////////////////////////////////
////////////////////// common used centering routine //////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

#ifdef WIN32
#ifndef WIN95
#define WINNT
#endif
#endif

void CenterDialog(HWND hPar, HWND hWnd)
{
// get size of parent window

   RECT rcp;
   GetWindowRect(hPar,&rcp);

// get size of dialog box   

   RECT rc;
   GetWindowRect(hWnd,&rc);

// center dialog box in parent window

   int x = rcp.left + ((rcp.right - rcp.left) - (rc.right - rc.left)) / 2;
   if (x <= 0) x = 0;
   int y = rcp.top + ((rcp.bottom - rcp.top) - (rc.bottom - rc.top)) / 2;
   if (y <= 0) y = 0;
   SetWindowPos(hWnd,HWND_TOP,x,y,100,100,SWP_NOSIZE);
}

///////////////////////////////////////////////////////////////////////////////
////////////////////// SimpleDialog class implementation //////////////////////
///////////////////////////////////////////////////////////////////////////////

SimpleDialog :: SimpleDialog(int ResID, Window *opWin, LPCSTR text )
              : ModalDialog(ResID,opWin)
{
   hPar = opWin->GetHandle();
   Text = text;
   Start();
}

SimpleDialog :: SimpleDialog(int ResID, Dialog *opDlg, LPCSTR text )
              : ModalDialog(ResID,opDlg)
{
   hPar = opDlg->GetHandle();
   Text = text;
   Start();
}

SimpleDialog :: ~SimpleDialog(VOID)
{
}

BOOL SimpleDialog :: Init(VOID)
{
   CenterDialog(hPar,this->GetHandle());

// transfer text to static control

   if (Text)
   {  StaticText oText(IDC_TEXT,this);
      oText.SetText(Text);
   }

// set focus to ok or yes button

   HWND h;
   if ( (h = GetDlgItem(this->GetHandle(), IDOK)) != 0 )
      SetFocus( h );
   else if ( (h = GetDlgItem(this->GetHandle(), IDYES)) != 0 )
      SetFocus( h );
   else if ( (h = GetDlgItem(this->GetHandle(), IDC_OKAY)) != 0 )
      SetFocus( h );

   return FALSE;
}

long SimpleDialog :: Control( UINT nControlId, UINT )
{ 
    switch (nControlId)
    {   case IDOK:
        case IDYES:
        case IDC_OKAY:
          TerminateDialog();
          result = TRUE;
          break;
        case IDCANCEL:
        case IDNO:
	case IDC_WWW:
          TerminateDialog();
          result = FALSE;
          break;
    }
    return FALSE;
}


BOOL SimpleDialog :: GetResult(VOID)
{
    return(result);
}

///////////////////////////////////////////////////////////////////////////////
////////////////////// StringDialog class implementation //////////////////////
///////////////////////////////////////////////////////////////////////////////

StringDialog :: StringDialog(int ResID, Window *opWin, LPCSTR text, UINT wl)
              : ModalDialog(ResID,opWin)
{
   wLen = wl;
   hPar = opWin->GetHandle();
   fixedfont = FALSE;
   SetString(text);
   Start();
}

StringDialog :: StringDialog(int ResID, Dialog *opDlg, LPCSTR text, UINT wl)
              : ModalDialog(ResID,opDlg)
{
   wLen = wl;
   hPar = opDlg->GetHandle();
   fixedfont = FALSE;
   SetString(text);
   Start();
}

StringDialog :: StringDialog(int ResID, Dialog *opDlg, LPCSTR text, UINT wl,
			     BOOL ff)
              : ModalDialog(ResID,opDlg)
{
   wLen = wl;
   hPar = opDlg->GetHandle();
   fixedfont = ff;
   SetString(text);
   Start();
}

StringDialog :: ~StringDialog(VOID)
{
    delete opEdit;
}

BOOL StringDialog :: Init(VOID)
{
   CenterDialog(hPar,this->GetHandle());

   if (fixedfont)
       SendDlgItemMessage(this->GetHandle(),IDC_EDIT,WM_SETFONT,
			(WPARAM)GetStockObject(SYSTEM_FIXED_FONT),FALSE);

// transfer text to stringedit

   opEdit = new StringEdit(IDC_EDIT,this);
   opEdit->SetText(caString);
   opEdit->SetCharLimit(wLen);
   opEdit->SetCrntSelection(strlen(caString),strlen(caString));
   opEdit->SetFocus();

   return FALSE;
}

long StringDialog :: Control( UINT nControlId, UINT )
{ 
    switch (nControlId)
    {   case IDOK:
        case IDYES:
          opEdit->GetText(caString,232);
          caString[wLen] = '\0';
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

BOOL StringDialog :: GetResult(VOID)
{
    return(result);
}

VOID StringDialog :: SetString(LPCSTR cpString)
{
    strcpy(caString,cpString);
}

char *StringDialog :: GetString( void )
{
    return caString;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////  Hook procedure for OpenFile and SaveFile  ////////////////
/////////////////// centers dialog and chooses gray background ////////////////
///////////////////////////////////////////////////////////////////////////////

extern "C"
{
   HBRUSH hBrush = 0;  // global
   HWND   hPar   = 0;  // global

#ifdef __BORLANDC__
#pragma argsused
#endif

   long APIENTRY HookProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
   {
       switch( msg )
       {
          case WM_INITDIALOG:
#ifdef WINNT
             Ctl3dSubclassDlg(hDlg,CTL3D_ALL);
#endif
#ifndef WIN95
             CenterDialog(hPar,hDlg); // doesn't work for explorer like Dialog box
#endif
             break; 

#ifdef WIN95
	  case WM_NOTIFY:  // explorer-like Dialog box needs special treatment
	     {
		OFNOTIFY *pNotify = (OFNOTIFY *) lParam;
		switch (pNotify->hdr.code)
		{
		   case CDN_INITDONE:
                      CenterDialog(hPar,GetActiveWindow());
		      break;
		   default:
		      break;
		}
	     }
#endif
#ifdef WIN32
          case WM_CTLCOLORDLG:
          case WM_CTLCOLORSTATIC:
          case WM_CTLCOLORSCROLLBAR:
          case WM_CTLCOLORMSGBOX:
//          case WM_CTLCOLORLISTBOX:
          case WM_CTLCOLOREDIT:
          case WM_CTLCOLORBTN:
#else
          case WM_CTLCOLOR:
             if ( HIWORD(lParam) == CTLCOLOR_DLG       ||
                  HIWORD(lParam) == CTLCOLOR_STATIC    ||
                  HIWORD(lParam) == CTLCOLOR_SCROLLBAR ||
                  HIWORD(lParam) == CTLCOLOR_MSGBOX    ||
//                  HIWORD(lParam) == CTLCOLOR_LISTBOX   ||
                  HIWORD(lParam) == CTLCOLOR_EDIT      ||
                  HIWORD(lParam) == CTLCOLOR_BTN       )
#endif
          {
             SetBkColor((HDC)wParam,GetSysColor(COLOR_BTNFACE));
             SetTextColor((HDC)wParam,GetSysColor(COLOR_BTNTEXT));
             return ((DWORD)hBrush);
          }

          case WM_DRAWITEM:
          {
//             LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
          }
       }
       return FALSE;
   }
}


///////////////////////////////////////////////////////////////////////////////
//////////////////////// OpenFileDlg class implementation /////////////////////
///////////////////////////////////////////////////////////////////////////////

OpenFileDlg :: OpenFileDlg(HWND hParent, LPCSTR Dir, LPCSTR Fltstr,
                           LPCSTR Ext, LPCSTR Title)
{
/////    static OPENFILENAME ofn;  // generates FAR_DATA segment with MSC
    OPENFILENAME ofn;
    char   szDirName[255];
    char   szFile[255];
    char   szFileTitle[255];
    char   szFilter[255];
    char   szTitle[255];

    if (Title != NULL) strcpy(szTitle,Title);
    strcpy(szDirName,Dir);
    strcpy(szFile,Ext);
    strcpy(szFileTitle," ");
    strcpy(szFilter,Fltstr);
    for (int i=0; szFilter[i] != '\0'; i++)
       if (szFilter[i] == '|')
          szFilter[i] = '\0';

    memset(&ofn, 0, sizeof(OPENFILENAME));
    FARPROC lpHookProc = MakeProcInstance((FARPROC)HookProc,
                                          Application::GetInstance());

    hPar   = hParent;                             // parent handle
    hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));  // background brush

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hParent;
    ofn.lpstrFilter = szFilter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.lpstrInitialDir = szDirName;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST |
                OFN_HIDEREADONLY | OFN_ENABLEHOOK;
#ifdef WIN95
    ofn.Flags |= OFN_EXPLORER;
#endif
    if (Title == NULL)
       ofn.lpstrTitle = "Open file";
    else
       ofn.lpstrTitle = szTitle;
#ifdef WIN32
    ofn.lpfnHook = (LPOFNHOOKPROC)lpHookProc;
#else
    ofn.lpfnHook = (unsigned int(CALLBACK *)(unsigned int, unsigned int,
                                             unsigned int, long))lpHookProc;
#endif

    result = GetOpenFileName(&ofn);

    strcpy(FileName,ofn.lpstrFile);

    FreeProcInstance(lpHookProc);
    DeleteObject(hBrush);
}

OpenFileDlg :: ~OpenFileDlg(void)
{
}

BOOL OpenFileDlg :: GetResult(void)
{
    return result;
}

char *OpenFileDlg :: GetFileName(void)
{
    return FileName;
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////// SaveFileDlg class implementation /////////////////////
///////////////////////////////////////////////////////////////////////////////

SaveFileDlg :: SaveFileDlg(HWND hParent, LPCSTR Dir, LPCSTR Fltstr,
                           LPCSTR Ext, LPCSTR Title)
{
/////    static OPENFILENAME ofn;  // generates FAR_DATA segment with MSC
    OPENFILENAME ofn;
    char   szDirName[255];
    char   szFile[255];
    char   szFileTitle[255];
    char   szFilter[255];
    char   szTitle[255];

    if (Title != NULL) strcpy(szTitle,Title);
    strcpy(szDirName,Dir);
    strcpy(szFile,Ext);
    strcpy(szFileTitle," ");
    strcpy(szFilter,Fltstr);
    for (int i=0; szFilter[i] != '\0'; i++)
       if (szFilter[i] == '|')
          szFilter[i] = '\0';

    memset(&ofn, 0, sizeof(OPENFILENAME));
    FARPROC lpHookProc = MakeProcInstance((FARPROC)HookProc,
                                          Application::GetInstance());

    hPar   = hParent;                             // parent handle
    hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));  // background brush

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hParent;
    ofn.lpstrFilter = szFilter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.lpstrInitialDir = szDirName;
    ofn.Flags = OFN_HIDEREADONLY | OFN_ENABLEHOOK;
#ifdef WIN95
    ofn.Flags |= OFN_EXPLORER;
#endif
    if (Title == NULL)
       ofn.lpstrTitle = "Save file";
    else
       ofn.lpstrTitle = szTitle;

#ifdef WIN32
    ofn.lpfnHook = (LPOFNHOOKPROC)lpHookProc;
#else
    ofn.lpfnHook = (unsigned int(CALLBACK *)(unsigned int, unsigned int,
                                             unsigned int, long))lpHookProc;
#endif

    result = GetSaveFileName(&ofn);

    strcpy(FileName,ofn.lpstrFile);

    FreeProcInstance(lpHookProc);
    DeleteObject(hBrush);
}

SaveFileDlg :: ~SaveFileDlg(void)
{
}

BOOL SaveFileDlg :: GetResult(void)
{
    return result;
}

char *SaveFileDlg :: GetFileName(void)
{
    return FileName;
}

///////////////////////////////////// EOF /////////////////////////////////////
