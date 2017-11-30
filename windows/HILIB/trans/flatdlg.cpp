///////////////////////////////////////////////////////////////////////////////
/////////////////////// Implementation of class EliDialog   ///////////////////
/////////////////////// Implementation of class EliDialog2  ///////////////////
/////////////////////// Implementation of class EliDialog3  ///////////////////
/////////////////////// Implementation of class InfoDialog  ///////////////////
/////////////////////// Implementation of class SigmaDialog ///////////////////
/////////////////////// Implementation of class TijkDialog  ///////////////////
/////////////////////// Implementation of class AuxWin      ///////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

#ifdef WIN95

extern "C"
{ 
  HWND      hWndTT;  // global
  HWND      hDlg;    // global
  HHOOK     hHook;   // global

  LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam) 
  { 
    MSG *lpmsg; 
 
    lpmsg = (MSG *) lParam; 
    if (nCode < 0 || !(IsChild(hDlg, lpmsg->hwnd))) 
        return (CallNextHookEx(hHook, nCode, wParam, lParam)); 
 
    switch (lpmsg->message)
    { 
        case WM_MOUSEMOVE: 
        case WM_LBUTTONDOWN: 
        case WM_LBUTTONUP: 
        case WM_RBUTTONDOWN: 
        case WM_RBUTTONUP: 
            if (hWndTT != NULL)
            { 
                MSG msg; 
 
                msg.lParam  = lpmsg->lParam; 
                msg.wParam  = lpmsg->wParam; 
                msg.message = lpmsg->message; 
                msg.hwnd    = lpmsg->hwnd; 
                SendMessage(hWndTT, TTM_RELAYEVENT, 0, (LPARAM) (LPMSG) &msg); 
            } 
            break; 
        default: 
            break; 
    } 
    return (CallNextHookEx(hHook, nCode, wParam, lParam)); 
  } 

  BOOL EnumChildProc(HWND hwndCtrl, LPARAM ) 
  { 
    TOOLINFO ti; 
    char szClass[64]; 
 
    GetClassName(hwndCtrl, szClass, sizeof(szClass)); 
    if (lstrcmp(szClass, "STATIC"))
    { 
        ti.cbSize = sizeof(TOOLINFO); 
        ti.uFlags = TTF_IDISHWND; 
        ti.hwnd = hDlg; 
        ti.uId = (UINT) hwndCtrl; 
        ti.hinst = 0; 
        ti.lpszText = LPSTR_TEXTCALLBACK; 
        SendMessage(hWndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti); 
    } 
    return TRUE; 
  } 

  void AddControl(int ID) 
  { 
    TOOLINFO ti; 

    ZeroMemory(&ti,sizeof(ti));
    ti.cbSize = sizeof(ti); 
    ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS; 
    ti.hwnd = hDlg; 
    ti.uId = (UINT) GetDlgItem(hDlg,ID); 
    ti.lpszText = LPSTR_TEXTCALLBACK; 
    SendMessage(hWndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti); 
  } 
}

#endif

///////////////////////////////////////////////////////////////////////////////

EliDialog :: EliDialog(int ResID, Window *opParent,
                       TransEllipseWin *w, TransGrafFile *f, int ypos)
           : ModalDialog(ResID, opParent)
{
    opFile  = f;
    opWin   = w;
    opPar   = opParent;
    wMaxLen = 20;
    y = ypos;
    Start();
}

EliDialog :: ~EliDialog(VOID)
{
    if (opEdit)
	delete opEdit;
}

BOOL EliDialog :: Init(VOID)
{
    HWND hWnd = this->GetHandle();
    SetWindowPos(hWnd,HWND_TOP,0,y,100,100,SWP_NOSIZE);
    opEdit  = new StringEdit(IDC_EDIT,this);
    opEdit->SetText("");
    opEdit->SetFocus();
    opEdit->SetCharLimit(20);
    opEdit->SetCrntSelection(0,0);
    SendDlgItemMessage(this->GetHandle(),IDC_NEXT,CRBM_SETTEXTCOLOR, TRUE, RGB(202,0,0));
    SendDlgItemMessage(this->GetHandle(),IDC_PREV,CRBM_SETTEXTCOLOR, TRUE, RGB(202,0,0));
    SendDlgItemMessage(this->GetHandle(),IDC_CANCEL,CRBM_SETTEXTCOLOR, TRUE, RGB(202,0,0));
    SendDlgItemMessage(this->GetHandle(),IDC_1FRAME,CRBM_SETTEXTCOLOR, TRUE, RGB(202,0,0));
    SendDlgItemMessage(this->GetHandle(),IDC_4FRAME,CRBM_SETTEXTCOLOR, TRUE, RGB(202,0,0));
    SendDlgItemMessage(this->GetHandle(),IDC_PRINT,CRBM_SETTEXTCOLOR, TRUE, RGB(202,0,0));
    SendDlgItemMessage(this->GetHandle(),IDC_CLEAR,CRBM_SETTEXTCOLOR, TRUE, RGB(202,0,0));
    HWND hwnd= GetDlgItem(this->GetHandle(),9998); // IDOK1
    ShowWindow(hwnd,SW_HIDE); // hide ok button

#ifdef WIN95
    DoCreateDialogTooltip();
#endif

    return FALSE;
}

long EliDialog :: Control(UINT nControlId, UINT )
{
    float value;

    switch (nControlId)
    {
	case IDOK1:
	    opEdit->GetText(caString,wMaxLen+1);
	    caString[wMaxLen] = '\0';

            AnsiUpper(caString);

            if (caString[0] == '\0')         // CR pressed
            {  ptr = opFile->GetNextElement();
               DspEllipses();
            }
            else if (strcmp(caString,"-1") == 0)
            {  ptr = opFile->GetPrevElement();
               if (ptr == NULL)
                  ptr = String(IDS_BOF);
               DspEllipses();
            }
            else if (strcmp(caString,"P") == 0)
               opWin->Print();
            else if (strcmp(caString,"*4") == 0)
               opWin->FourFrames();
            else if (strcmp(caString,"*1") == 0)
               opWin->SingleFrame();
            else if (strcmp(caString,"C") == 0)
               opWin->Erase();
            else if (strcmp(caString,String(IDS_QUITS)) == 0)
	    {
#ifdef WIN95
		UnhookWindowsHookEx(hHook);
		DestroyWindow(hWndTT);
#endif
	        SendMessage(opWin->GetHandle(), WM_CLOSE, 0L, 0L);
		MainWindow::UnBusy();
                TerminateDialog();
	    }
            else if (sscanf(caString,"%f",&value) == 1)
            {  ptr = opFile->FindElement(value);
               DspEllipses();
            }
            else
            {  ptr = opFile->FindElement(caString);
               DspEllipses();
            }
	    opEdit->SetText("");
            opEdit->SetFocus();
	    break;
	case IDC_CANCEL:
	    {
#ifdef WIN95
		UnhookWindowsHookEx(hHook);
		DestroyWindow(hWndTT);
#endif
	        SendMessage(opWin->GetHandle(), WM_CLOSE, 0L, 0L);
		MainWindow::UnBusy();
	        TerminateDialog();
	    }
	    break;
        case IDC_NEXT:
            ptr = opFile->GetNextElement();
            DspEllipses();
            opEdit->SetFocus();
            break;
        case IDC_PREV:
            ptr = opFile->GetPrevElement();
            if (ptr == NULL)
               ptr = String(IDS_BOF);
            DspEllipses();
            opEdit->SetFocus();
            break;
        case IDC_1FRAME:
            opWin->SingleFrame();
            opEdit->SetFocus();
            break;
        case IDC_4FRAME:
            opWin->FourFrames();
            opEdit->SetFocus();
            break;
        case IDC_PRINT:
            opWin->Print();
            opEdit->SetFocus();
            break;
        case IDC_CLEAR:
            opWin->Erase();
            opEdit->SetFocus();
            break;
	default:
	    break;
    }
    return FALSE;
}

VOID EliDialog :: DspEllipses(VOID)
{
   float x,xp,r12,y,yp,r34,d,dp;

   if (ptr == 0L)
   {  opFile->Rewind();
      SimpleDialog oDlg(IDD_INFO_DIALOG,opWin,String(IDS_EOF_RF));
   }
   else if (strcmp(ptr,String(IDS_BOF)) == 0)
   {  opFile->Rewind();
      SimpleDialog oDlg(IDD_INFO_DIALOG, opWin,String(IDS_BOF));
   }
   else
   {
       opFile->GetEllipsePar(x,xp,r12,y,yp,r34,d,dp);
       opWin->TransferEllipsePar(x,xp,r12,y,yp,r34,d,dp,ptr);
   }
}

#ifdef WIN95

BOOL EliDialog :: DoCreateDialogTooltip(void) 
{ 
    hDlg = this->GetHandle();

    InitCommonControls(); 
    hWndTT = CreateWindowEx(0, TOOLTIPS_CLASS, (LPSTR) NULL, 
        TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
        CW_USEDEFAULT, hDlg, (HMENU) NULL, Application::GetInstance(), NULL); 
    if (hWndTT == NULL) 
        return FALSE; 
 
//  does not work anymore under Windows 2000 ?????
//    if (!EnumChildWindows(hDlg, (WNDENUMPROC) EnumChildProc, 0)) 
//        return FALSE; 

    AddControl(IDOK1);
    AddControl(IDC_CANCEL);
    AddControl(IDC_NEXT);
    AddControl(IDC_PREV);
    AddControl(IDC_1FRAME);
    AddControl(IDC_4FRAME);
    AddControl(IDC_PRINT);
    AddControl(IDC_CLEAR);
 
    hHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMsgProc, 
        (HINSTANCE) NULL, GetCurrentThreadId()); 
 
    if (hHook == (HHOOK) NULL) 
        return FALSE; 
 
    return TRUE; 
} 
 
long EliDialog :: WmNotify(LPARAM lParam) 
{ 
    static CHAR szBuf[128];
 
    if ((((LPNMHDR) lParam)->code) == TTN_NEEDTEXT)
    { 
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lParam; 
	int idCtrl = GetDlgCtrlID((HWND)((LPNMHDR)lParam)->idFrom);
        LoadString(Application::GetInstance(),idCtrl,szBuf,sizeof(szBuf));
        lpttt->lpszText = szBuf;
    } 
    return 0L; 
} 

#endif 

///////////////////////////////////////////////////////////////////////////////

EliDialog2 :: EliDialog2(int ResID, Window *opParent,
                         TransEllipseWin2 *w, TransGrafFile *f, int ypos)
           : ModalDialog(ResID, opParent)
{
    opFile  = f;
    opWin   = w;
    opPar   = opParent;
    wMaxLen = 20;
    y = ypos;
    Start();
}

EliDialog2 :: ~EliDialog2(VOID)
{
    if (opEdit)
	delete opEdit;
}

BOOL EliDialog2 :: Init(VOID)
{
    HWND hWnd = this->GetHandle();
    SetWindowPos(hWnd,HWND_TOP,0,y,100,100,SWP_NOSIZE);
    opEdit  = new StringEdit(IDC_EDIT2,this);
    opEdit->SetText("");
    opEdit->SetFocus();
    opEdit->SetCharLimit(20);
    opEdit->SetCrntSelection(0,0);
    SendDlgItemMessage(this->GetHandle(),IDC_NEXT2,CRBM_SETTEXTCOLOR, TRUE, RGB(0,102,0));
    SendDlgItemMessage(this->GetHandle(),IDC_PREV2,CRBM_SETTEXTCOLOR, TRUE, RGB(0,102,0));
    SendDlgItemMessage(this->GetHandle(),IDC_CANCEL2,CRBM_SETTEXTCOLOR, TRUE, RGB(0,102,0));
    SendDlgItemMessage(this->GetHandle(),IDC_1FRAME2,CRBM_SETTEXTCOLOR, TRUE, RGB(0,102,0));
    SendDlgItemMessage(this->GetHandle(),IDC_4FRAME2,CRBM_SETTEXTCOLOR, TRUE, RGB(0,102,0));
    SendDlgItemMessage(this->GetHandle(),IDC_PRINT2,CRBM_SETTEXTCOLOR, TRUE, RGB(0,102,0));
    SendDlgItemMessage(this->GetHandle(),IDC_CLEAR2,CRBM_SETTEXTCOLOR, TRUE, RGB(0,102,0));
    HWND hwnd= GetDlgItem(this->GetHandle(),9999); // IDOK2
    ShowWindow(hwnd,SW_HIDE); // hide ok button

#ifdef WIN95
    DoCreateDialogTooltip();
#endif

    return FALSE;
}

long EliDialog2 :: Control(UINT nControlId, UINT )
{
    float value;

    switch (nControlId)
    {
	case IDOK2:
	    opEdit->GetText(caString,wMaxLen+1);
	    caString[wMaxLen] = '\0';

            AnsiUpper(caString);

            if (caString[0] == '\0')         // CR pressed
            {  ptr = opFile->GetNextElement();
               DspEllipses();
            }
            else if (strcmp(caString,"-1") == 0)
            {  ptr = opFile->GetPrevElement();
               if (ptr == NULL)
                  ptr = String(IDS_BOF);
               DspEllipses();
            }
            else if (strcmp(caString,"P") == 0)
               opWin->Print();
            else if (strcmp(caString,"*4") == 0)
               opWin->FourFrames();
            else if (strcmp(caString,"*1") == 0)
               opWin->SingleFrame();
            else if (strcmp(caString,"C") == 0)
               opWin->Erase();
            else if (strcmp(caString,String(IDS_QUITS)) == 0)
	    {
#ifdef WIN95
		UnhookWindowsHookEx(hHook);
		DestroyWindow(hWndTT);
#endif
	        SendMessage(opWin->GetHandle(), WM_CLOSE, 0L, 0L);
		MainWindow::UnBusy2();
                TerminateDialog();
	    }
            else if (sscanf(caString,"%f",&value) == 1)
            {  ptr = opFile->FindElement(value);
               DspEllipses();
            }
            else
            {  ptr = opFile->FindElement(caString);
               DspEllipses();
            }
	    opEdit->SetText("");
            opEdit->SetFocus();
	    break;
	case IDC_CANCEL2:
	    {
#ifdef WIN95
		UnhookWindowsHookEx(hHook);
		DestroyWindow(hWndTT);
#endif
	        SendMessage(opWin->GetHandle(), WM_CLOSE, 0L, 0L);
		MainWindow::UnBusy2();
	        TerminateDialog();
	    }
	    break;
        case IDC_NEXT2:
            ptr = opFile->GetNextElement();
            DspEllipses();
            opEdit->SetFocus();
            break;
        case IDC_PREV2:
            ptr = opFile->GetPrevElement();
            if (ptr == NULL)
               ptr = String(IDS_BOF);
            DspEllipses();
            opEdit->SetFocus();
            break;
        case IDC_1FRAME2:
            opWin->SingleFrame();
            opEdit->SetFocus();
            break;
        case IDC_4FRAME2:
            opWin->FourFrames();
            opEdit->SetFocus();
            break;
        case IDC_PRINT2:
            opWin->Print();
            opEdit->SetFocus();
            break;
        case IDC_CLEAR2:
            opWin->Erase();
            opEdit->SetFocus();
            break;
	default:
	    break;
    }
    return FALSE;
}

VOID EliDialog2 :: DspEllipses(VOID)
{
   float x,xp,r12;

   if (ptr == 0L)
   {  opFile->Rewind();
      SimpleDialog oDlg(IDD_INFO_DIALOG,opWin,String(IDS_EOF_RF));
   }
   else if (strcmp(ptr,String(IDS_BOF)) == 0)
   {  opFile->Rewind();
      SimpleDialog oDlg(IDD_INFO_DIALOG, opWin,String(IDS_BOF));
   }
   else
   {
       opFile->GetEllipsePar2(x,xp,r12);
       opWin->TransferEllipsePar(x,xp,r12,ptr);
   }
}

#ifdef WIN95

BOOL EliDialog2 :: DoCreateDialogTooltip(void) 
{ 
    hDlg = this->GetHandle();

    InitCommonControls(); 
    hWndTT = CreateWindowEx(0, TOOLTIPS_CLASS, (LPSTR) NULL, 
        TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
        CW_USEDEFAULT, hDlg, (HMENU) NULL, Application::GetInstance(), NULL); 
    if (hWndTT == NULL) 
        return FALSE; 
 
//  does not work anymore under Windows 2000 ?????
//    if (!EnumChildWindows(hDlg, (WNDENUMPROC) EnumChildProc, 0)) 
//        return FALSE; 

    AddControl(IDOK2);
    AddControl(IDC_CANCEL2);
    AddControl(IDC_NEXT2);
    AddControl(IDC_PREV2);
    AddControl(IDC_1FRAME2);
    AddControl(IDC_4FRAME2);
    AddControl(IDC_PRINT2);
    AddControl(IDC_CLEAR2);
 
    hHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMsgProc, 
        (HINSTANCE) NULL, GetCurrentThreadId()); 
 
    if (hHook == (HHOOK) NULL) 
        return FALSE; 
 
    return TRUE; 
} 
 
long EliDialog2 :: WmNotify(LPARAM lParam) 
{ 
    static CHAR szBuf[128];
 
    if ((((LPNMHDR) lParam)->code) == TTN_NEEDTEXT)
    { 
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lParam; 
	int idCtrl = GetDlgCtrlID((HWND)((LPNMHDR)lParam)->idFrom);
        LoadString(Application::GetInstance(),idCtrl,szBuf,sizeof(szBuf));
        lpttt->lpszText = szBuf;
    } 
    return 0L; 
} 

#endif 

///////////////////////////////////////////////////////////////////////////////

EliDialog3 :: EliDialog3(int ResID, Window *opParent,
                       TransEllipseWin3 *w, TransGrafFile *f, int ypos)
           : ModalDialog(ResID, opParent)
{
    opFile  = f;
    opWin   = w;
    opPar   = opParent;
    wMaxLen = 20;
    y = ypos;
    Start();
}

EliDialog3 :: ~EliDialog3(VOID)
{
    if (opEdit)
	delete opEdit;
}

BOOL EliDialog3 :: Init(VOID)
{
    HWND hWnd = this->GetHandle();
    SetWindowPos(hWnd,HWND_TOP,0,y,100,100,SWP_NOSIZE);
    opEdit  = new StringEdit(IDC_EDIT3,this);
    opEdit->SetText("");
    opEdit->SetFocus();
    opEdit->SetCharLimit(20);
    opEdit->SetCrntSelection(0,0);
    SendDlgItemMessage(this->GetHandle(),IDC_NEXT3,CRBM_SETTEXTCOLOR, TRUE, RGB(0,0,202));
    SendDlgItemMessage(this->GetHandle(),IDC_PREV3,CRBM_SETTEXTCOLOR, TRUE, RGB(0,0,202));
    SendDlgItemMessage(this->GetHandle(),IDC_CANCEL3,CRBM_SETTEXTCOLOR, TRUE, RGB(0,0,202));
    SendDlgItemMessage(this->GetHandle(),IDC_1FRAME3,CRBM_SETTEXTCOLOR, TRUE, RGB(0,0,202));
    SendDlgItemMessage(this->GetHandle(),IDC_4FRAME3,CRBM_SETTEXTCOLOR, TRUE, RGB(0,0,202));
    SendDlgItemMessage(this->GetHandle(),IDC_PRINT3,CRBM_SETTEXTCOLOR, TRUE, RGB(0,0,202));
    SendDlgItemMessage(this->GetHandle(),IDC_CLEAR3,CRBM_SETTEXTCOLOR, TRUE, RGB(0,0,202));
    HWND hwnd= GetDlgItem(this->GetHandle(),9997); // IDOK3
    ShowWindow(hwnd,SW_HIDE); // hide ok button

#ifdef WIN95
    DoCreateDialogTooltip();
#endif

    return FALSE;
}

long EliDialog3 :: Control(UINT nControlId, UINT )
{
    float value;

    switch (nControlId)
    {
	case IDOK3:
	    opEdit->GetText(caString,wMaxLen+1);
	    caString[wMaxLen] = '\0';

            AnsiUpper(caString);

            if (caString[0] == '\0')         // CR pressed
            {  ptr = opFile->GetNextElement();
               DspEllipses();
            }
            else if (strcmp(caString,"-1") == 0)
            {  ptr = opFile->GetPrevElement();
               if (ptr == NULL)
                  ptr = String(IDS_BOF);
               DspEllipses();
            }
            else if (strcmp(caString,"P") == 0)
               opWin->Print();
            else if (strcmp(caString,"*4") == 0)
               opWin->FourFrames();
            else if (strcmp(caString,"*1") == 0)
               opWin->SingleFrame();
            else if (strcmp(caString,"C") == 0)
               opWin->Erase();
            else if (strcmp(caString,String(IDS_QUITS)) == 0)
	    {
#ifdef WIN95
		UnhookWindowsHookEx(hHook);
		DestroyWindow(hWndTT);
#endif
	        SendMessage(opWin->GetHandle(), WM_CLOSE, 0L, 0L);
		MainWindow::UnBusy3();
                TerminateDialog();
	    }
            else if (sscanf(caString,"%f",&value) == 1)
            {  ptr = opFile->FindElement(value);
               DspEllipses();
            }
            else
            {  ptr = opFile->FindElement(caString);
               DspEllipses();
            }
	    opEdit->SetText("");
            opEdit->SetFocus();
	    break;
	case IDC_CANCEL3:
	    {
#ifdef WIN95
		UnhookWindowsHookEx(hHook);
		DestroyWindow(hWndTT);
#endif
	        SendMessage(opWin->GetHandle(), WM_CLOSE, 0L, 0L);
		MainWindow::UnBusy3();
	        TerminateDialog();
	    }
	    break;
        case IDC_NEXT3:
            ptr = opFile->GetNextElement();
            DspEllipses();
            opEdit->SetFocus();
            break;
        case IDC_PREV3:
            ptr = opFile->GetPrevElement();
            if (ptr == NULL)
               ptr = String(IDS_BOF);
            DspEllipses();
            opEdit->SetFocus();
            break;
        case IDC_1FRAME3:
            opWin->SingleFrame();
            opEdit->SetFocus();
            break;
        case IDC_4FRAME3:
            opWin->FourFrames();
            opEdit->SetFocus();
            break;
        case IDC_PRINT3:
            opWin->Print();
            opEdit->SetFocus();
            break;
        case IDC_CLEAR3:
            opWin->Erase();
            opEdit->SetFocus();
            break;
	default:
	    break;
    }
    return FALSE;
}

VOID EliDialog3 :: DspEllipses(VOID)
{
   float x,xp,r13,y,yp,r24;

   if (ptr == 0L)
   {  opFile->Rewind();
      SimpleDialog oDlg(IDD_INFO_DIALOG,opWin,String(IDS_EOF_RF));
   }
   else if (strcmp(ptr,String(IDS_BOF)) == 0)
   {  opFile->Rewind();
      SimpleDialog oDlg(IDD_INFO_DIALOG, opWin,String(IDS_BOF));
   }
   else
   {
       opFile->GetEllipsePar3(x,y,r13,xp,yp,r24);
       opWin->TransferEllipsePar(x,xp,r13,y,yp,r24,ptr);
   }
}

#ifdef WIN95

BOOL EliDialog3 :: DoCreateDialogTooltip(void) 
{ 
    hDlg = this->GetHandle();

    InitCommonControls(); 
    hWndTT = CreateWindowEx(0, TOOLTIPS_CLASS, (LPSTR) NULL, 
        TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
        CW_USEDEFAULT, hDlg, (HMENU) NULL, Application::GetInstance(), NULL); 
    if (hWndTT == NULL) 
        return FALSE; 
 
//  does not work anymore under Windows 2000 ?????
//    if (!EnumChildWindows(hDlg, (WNDENUMPROC) EnumChildProc, 0)) 
//        return FALSE; 

    AddControl(IDOK3);
    AddControl(IDC_CANCEL3);
    AddControl(IDC_NEXT3);
    AddControl(IDC_PREV3);
    AddControl(IDC_1FRAME3);
    AddControl(IDC_4FRAME3);
    AddControl(IDC_PRINT3);
    AddControl(IDC_CLEAR3);
 
    hHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMsgProc, 
        (HINSTANCE) NULL, GetCurrentThreadId()); 
 
    if (hHook == (HHOOK) NULL) 
        return FALSE; 
 
    return TRUE; 
} 
 
long EliDialog3 :: WmNotify(LPARAM lParam) 
{ 
    static CHAR szBuf[128];
 
    if ((((LPNMHDR) lParam)->code) == TTN_NEEDTEXT)
    { 
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lParam; 
	int idCtrl = GetDlgCtrlID((HWND)((LPNMHDR)lParam)->idFrom);
        LoadString(Application::GetInstance(),idCtrl,szBuf,sizeof(szBuf));
        lpttt->lpszText = szBuf;
    } 
    return 0L; 
} 

#endif 

///////////////////////////////////////////////////////////////////////////////

FILE *Open(TextWindow *opWin, FILE *fpo, char *Name, char *Title, BOOL &first)
{
    char text[80];

    if (!fpo)
    {
       sprintf(text,String(IDS_OPEN_Q),Name);
       SimpleDialog oDlg(IDD_QEND_DIALOG,opWin,text);
       if (oDlg.GetResult())
       {  fpo = fopen(Name,"a");
          opWin->WriteLn(String(IDS_SHOWN_O));
          if (first)
          {  first = FALSE;
             fprintf(fpo,String(IDS_NSN_FORM),Title);
          }
       }
       else
          fpo = NULL;
    }
    else
    {
       sprintf(text,String(IDS_ALREADYO),Name);
       SimpleDialog oDlg(IDD_INFO_DIALOG,opWin,text);
    }
    return fpo;
}

FILE *CloseIt(TextWindow *opWin, FILE *fpo, char *Name)
{
    char text[80];

    if (fpo)
    {
       sprintf(text,String(IDS_CLOSE_Q),Name);
       SimpleDialog oDlg(IDD_QEND_DIALOG,opWin,text);
       if (oDlg.GetResult())
       {
          fclose(fpo);
          fpo = NULL;
          sprintf(text,String(IDS_WRITTEN),Name);
          opWin->WriteLn(text);
       }
    }
    else
    {
       sprintf(text,String(IDS_NOTOPEN),Name);
       SimpleDialog oDlg(IDD_INFO_DIALOG,opWin,text);
    }
    return fpo;
}

///////////////////////////////////////////////////////////////////////////////

InfoDialog :: InfoDialog(int ResID, Window *opParent,
                         TextWindow *w, TransFile *f)
            : ModalDialog(ResID, opParent)
{
    opFile   = f;
    opWin    = w;
    wMaxLen  = 20;
    Start();
}

InfoDialog :: ~InfoDialog(VOID)
{
    if (opEdit)
	delete opEdit;
}

BOOL InfoDialog :: Init(VOID)
{
    opEdit  = new StringEdit(IDC_EDIT,this);
    opEdit->SetText("");
    opEdit->SetFocus();
    opEdit->SetCharLimit(20);
    opEdit->SetCrntSelection(0,0);


    fpo    = NULL;
    first  = TRUE;

#ifdef WIN95
    DoCreateDialogTooltip();
#endif

    return FALSE;
}

long InfoDialog :: Control(UINT nControlId, UINT )
{
    float value;

    switch (nControlId)
    {
	case IDOK:
	    opEdit->GetText(caString,wMaxLen+1);
	    caString[wMaxLen] = '\0';

            AnsiUpper(caString);

            if (caString[0] == '\0')         // CR pressed
            {  ptr = opFile->GetNextElement();
               DspData();
            }
            else if (strcmp(caString,"F") == 0)
               fpo = Open(opWin,fpo,String(IDS_INFO_DAT,1),
                          opFile->GetTitleOfRun(),first);
            else if (strcmp(caString,"C") == 0)
               fpo = CloseIt(opWin,fpo,String(IDS_INFO_DAT,1));
            else if (strcmp(caString,"E") == 0)
               opWin->Erase();
            else if (strcmp(caString,String(IDS_QUITS)) == 0)
            {  if (fpo)
               {
                  fpo = CloseIt(opWin,fpo,String(IDS_INFO_DAT,1));
                  wait(1000);
               }
#ifdef WIN95
		UnhookWindowsHookEx(hHook);
		DestroyWindow(hWndTT);
#endif
                TerminateDialog();
            }
            else if (strcmp(caString,"-1") == 0)
            {  ptr = opFile->GetPrevElement();
               if (ptr == NULL)
                  ptr = String(IDS_BOF);
               DspData();
            }
            else if (sscanf(caString,"%f",&value) == 1)
            {  ptr = opFile->FindElement(value);
               DspData();
            }
            else
            {  ptr = opFile->FindElement(caString);
               DspData();
            }
	    opEdit->SetText("");
            opEdit->SetFocus();
	    break;
	case IDCANCEL:
            if (fpo)
            {
               fpo = CloseIt(opWin,fpo,String(IDS_INFO_DAT,1));
               wait(1000);
            }
#ifdef WIN95
	    UnhookWindowsHookEx(hHook);
	    DestroyWindow(hWndTT);
#endif
	    TerminateDialog();
	    break;
        case IDC_NEXT:
            ptr = opFile->GetNextElement();
            DspData();
            opEdit->SetFocus();
            break;
        case IDC_PREV:
            ptr = opFile->GetPrevElement();
            if (ptr == NULL)
               ptr = String(IDS_BOF);
            DspData();
            opEdit->SetFocus();
            break;
        case IDC_CLOSE:
            fpo = CloseIt(opWin,fpo,String(IDS_INFO_DAT,1));
            opEdit->SetFocus();
            break;
        case IDC_FILE:
            fpo = Open(opWin,fpo,String(IDS_INFO_DAT,1),
                       opFile->GetTitleOfRun(),first);
            opEdit->SetFocus();
            break;
        case IDC_ERASE:
            opWin->Erase();
            opEdit->SetFocus();
            break;
	default:
	    break;
    }
    return FALSE;
}

VOID InfoDialog :: DspData(VOID)
{
   char buffer[1024];

   if (ptr == 0L)
   {  opFile->Rewind();
      opWin->WriteLn(String(IDS_EOF_RF));
   }
   else if (strcmp(ptr,String(IDS_BOF)) == 0)
   {  opFile->Rewind();
      opWin->WriteLn(String(IDS_BOF));
   }
   else
   {
      strcpy(buffer,String(IDS_RN_FORM));
      strcat(buffer,ptr);
      if (fpo)
         fprintf(fpo,String(IDS_NSN_FORM),ptr);

      {  for (int i=0; i<11; i++)
            if ((ptr = opFile->GetBeamInfoData(i)) != 0L)
            {
               strcat(buffer,String(IDS_RN_FORM));
               strcat(buffer,ptr);
               if (fpo)
                  fprintf(fpo,String(IDS_SN_FORM),ptr);
            }
         opWin->WriteLn(buffer);
      }
   }
}

#ifdef WIN95

BOOL InfoDialog :: DoCreateDialogTooltip(void) 
{ 
    hDlg = this->GetHandle();

    InitCommonControls(); 
    hWndTT = CreateWindowEx(0, TOOLTIPS_CLASS, (LPSTR) NULL, 
        TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
        CW_USEDEFAULT, hDlg, (HMENU) NULL, Application::GetInstance(), NULL); 
    if (hWndTT == NULL) 
        return FALSE; 
 
//  does not work anymore under Windows 2000 ?????
//    if (!EnumChildWindows(hDlg, (WNDENUMPROC) EnumChildProc, 0)) 
//        return FALSE; 

    AddControl(IDOK);
    AddControl(IDCANCEL);
    AddControl(IDC_NEXT);
    AddControl(IDC_PREV);
    AddControl(IDC_CLOSE);
    AddControl(IDC_FILE);
    AddControl(IDC_ERASE);
 
    hHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMsgProc, 
        (HINSTANCE) NULL, GetCurrentThreadId()); 
 
    if (hHook == (HHOOK) NULL) 
        return FALSE; 
 
    return TRUE; 
} 
 
long InfoDialog :: WmNotify(LPARAM lParam) 
{ 
    static CHAR szBuf[128];
 
    if ((((LPNMHDR) lParam)->code) == TTN_NEEDTEXT)
    { 
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lParam; 
	int idCtrl = GetDlgCtrlID((HWND)((LPNMHDR)lParam)->idFrom);
        LoadString(Application::GetInstance(),idCtrl,szBuf,sizeof(szBuf));
        lpttt->lpszText = szBuf;
    } 
    return 0L; 
} 

#endif 

///////////////////////////////////////////////////////////////////////////////

SigmaDialog :: SigmaDialog(int ResID, Window *opParent,
                           TextWindow *w, TrOutFile *f)
             : ModalDialog(ResID, opParent)
{
    opFile   = f;
    opWin    = w;
    wMaxLen  = 20;
    Start();
}

SigmaDialog :: ~SigmaDialog(VOID)
{
    if (opEdit)
	delete opEdit;
}

BOOL SigmaDialog :: Init(VOID)
{
    opEdit  = new StringEdit(IDC_EDIT,this);
    opEdit->SetText("");
    opEdit->SetFocus();
    opEdit->SetCharLimit(20);
    opEdit->SetCrntSelection(0,0);

    first  = TRUE;
    fpo    = NULL;

#ifdef WIN95
    DoCreateDialogTooltip();
#endif

    return FALSE;
}

long SigmaDialog :: Control(UINT nControlId, UINT )
{
    float value;

    switch (nControlId)
    {
	case IDOK:
	    opEdit->GetText(caString,wMaxLen+1);
	    caString[wMaxLen] = '\0';

            AnsiUpper(caString);

            if (caString[0] == '\0')         // CR pressed
            {  ptr = opFile->GetNextElement();
               DspData();
            }
            else if (strcmp(caString,"F") == 0)
               fpo = Open(opWin,fpo,String(IDS_SGMA_DAT,1),
                          opFile->GetTitleOfRun(),first);
            else if (strcmp(caString,"C") == 0)
               fpo = CloseIt(opWin,fpo,String(IDS_SGMA_DAT,1));
            else if (strcmp(caString,"E") == 0)
               opWin->Erase();
            else if (strcmp(caString,String(IDS_QUITS)) == 0)
            {  if (fpo)
               {
                  fpo = CloseIt(opWin,fpo,String(IDS_SGMA_DAT,1));
                  wait(1000);
               }
#ifdef WIN95
		UnhookWindowsHookEx(hHook);
		DestroyWindow(hWndTT);
#endif
                TerminateDialog();
            }
            else if (strcmp(caString,"-1") == 0)
            {  ptr = opFile->GetPrevElement();
               if (ptr == NULL)
                  ptr = String(IDS_BOF);
               DspData();
            }
            else if (sscanf(caString,"%f",&value) == 1)
            {  ptr = opFile->FindElement(value);
               DspData();
            }
            else
            {  ptr = opFile->FindElement(caString);
               DspData();
            }
	    opEdit->SetText("");
            opEdit->SetFocus();
	    break;
	case IDCANCEL:
            if (fpo)
            {
               fpo = CloseIt(opWin,fpo,String(IDS_SGMA_DAT,1));
               wait(1000);
            }
#ifdef WIN95
	    UnhookWindowsHookEx(hHook);
	    DestroyWindow(hWndTT);
#endif
	    TerminateDialog();
	    break;
        case IDC_NEXT:
            ptr = opFile->GetNextElement();
            DspData();
            opEdit->SetFocus();
            break;
        case IDC_PREV:
            ptr = opFile->GetPrevElement();
            if (ptr == NULL)
               ptr = String(IDS_BOF);
            DspData();
            opEdit->SetFocus();
            break;
        case IDC_CLOSE:
            fpo = CloseIt(opWin,fpo,String(IDS_SGMA_DAT,1));
            opEdit->SetFocus();
            break;
        case IDC_FILE:
            fpo = Open(opWin,fpo,String(IDS_SGMA_DAT,1),
                       opFile->GetTitleOfRun(),first);
            opEdit->SetFocus();
            break;
        case IDC_ERASE:
            opWin->Erase();
            opEdit->SetFocus();
            break;
	default:
	    break;
    }
    return FALSE;
}

VOID SigmaDialog :: DspData(VOID)
{
   char buffer[2048];
   int i;

   if (ptr == 0L)
   {  opFile->Rewind();
      opWin->WriteLn(String(IDS_EOF_RF));
   }
   else if (strcmp(ptr,String(IDS_BOF)) == 0)
   {  opFile->Rewind();
      opWin->WriteLn(String(IDS_BOF));
   }
   else
   {
      strcpy(buffer,String(IDS_RN_FORM));
      strcat(buffer,ptr);
      if (fpo)
         fprintf(fpo,String(IDS_NSN_FORM),ptr);

      {  for (i=0; i<6; i++)
            if ((ptr = opFile->GetSigmaLine(i)) != 0L)
            {
               strcat(buffer,String(IDS_RN_FORM));
               strcat(buffer,ptr);
               if (fpo)
                  fprintf(fpo,String(IDS_SN_FORM),ptr);
            }
         for (i=0; i<7; i++)
            if ((ptr = opFile->GetMatrixLine(i)) != 0L)
            {
               strcat(buffer,String(IDS_RN_FORM));
               strcat(buffer,ptr);
               if (fpo)
                  fprintf(fpo,String(IDS_SN_FORM),ptr);
            }
         opWin->WriteLn(buffer);
      }
   }
}

#ifdef WIN95

BOOL SigmaDialog :: DoCreateDialogTooltip(void) 
{ 
    hDlg = this->GetHandle();

    InitCommonControls(); 
    hWndTT = CreateWindowEx(0, TOOLTIPS_CLASS, (LPSTR) NULL, 
        TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
        CW_USEDEFAULT, hDlg, (HMENU) NULL, Application::GetInstance(), NULL); 
    if (hWndTT == NULL) 
        return FALSE; 
 
//  does not work anymore under Windows 2000 ?????
//    if (!EnumChildWindows(hDlg, (WNDENUMPROC) EnumChildProc, 0)) 
//        return FALSE; 

    AddControl(IDOK);
    AddControl(IDCANCEL);
    AddControl(IDC_NEXT);
    AddControl(IDC_PREV);
    AddControl(IDC_CLOSE);
    AddControl(IDC_FILE);
    AddControl(IDC_ERASE);
 
    hHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMsgProc, 
        (HINSTANCE) NULL, GetCurrentThreadId()); 
 
    if (hHook == (HHOOK) NULL) 
        return FALSE; 
 
    return TRUE; 
} 
 
long SigmaDialog :: WmNotify(LPARAM lParam) 
{ 
    static CHAR szBuf[128];
 
    if ((((LPNMHDR) lParam)->code) == TTN_NEEDTEXT)
    { 
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lParam; 
	int idCtrl = GetDlgCtrlID((HWND)((LPNMHDR)lParam)->idFrom);
        LoadString(Application::GetInstance(),idCtrl,szBuf,sizeof(szBuf));
        lpttt->lpszText = szBuf;
    } 
    return 0L; 
} 

#endif 

///////////////////////////////////////////////////////////////////////////////

TijkDialog :: TijkDialog(int ResID, Window *opParent,
                         TextWindow *w, TransSecondFile *f)
            : ModalDialog(ResID, opParent)
{
    opFile   = f;
    opWin    = w;
    wMaxLen  = 20;
    Start();
}

TijkDialog :: ~TijkDialog(VOID)
{
    if (opEdit)
	delete opEdit;
}

BOOL TijkDialog :: Init(VOID)
{
    opEdit  = new StringEdit(IDC_EDIT,this);
    opEdit->SetText("");
    opEdit->SetFocus();
    opEdit->SetCharLimit(20);
    opEdit->SetCrntSelection(0,0);

    first  = TRUE;
    fpo    = NULL;

#ifdef WIN95
    DoCreateDialogTooltip();
#endif

    return FALSE;
}

long TijkDialog :: Control(UINT nControlId, UINT )
{
    float value;

    switch (nControlId)
    {
	case IDOK:
	    opEdit->GetText(caString,wMaxLen+1);
	    caString[wMaxLen] = '\0';

            AnsiUpper(caString);

            if (caString[0] == '\0')         // CR pressed
            {  ptr = opFile->GetNextElement();
               DspData();
            }
            else if (strcmp(caString,"F") == 0)
               fpo = Open(opWin,fpo,String(IDS_TIJK_DAT,1),
                          opFile->GetTitleOfRun(),first);
            else if (strcmp(caString,"C") == 0)
               fpo = CloseIt(opWin,fpo,String(IDS_TIJK_DAT,1));
            else if (strcmp(caString,"E") == 0)
               opWin->Erase();
            else if (strcmp(caString,String(IDS_QUITS)) == 0)
            {  if (fpo)
               {
                  fpo = CloseIt(opWin,fpo,String(IDS_TIJK_DAT,1));
                  wait(1000);
               }
#ifdef WIN95
		UnhookWindowsHookEx(hHook);
		DestroyWindow(hWndTT);
#endif
                TerminateDialog();
            }
            else if (strcmp(caString,"-1") == 0)
            {  ptr = opFile->GetPrevElement();
               if (ptr == NULL)
                  ptr = String(IDS_BOF);
               DspData();
            }
            else if (sscanf(caString,"%f",&value) == 1)
            {  ptr = opFile->FindElement(value);
               DspData();
            }
            else
            {  ptr = opFile->FindElement(caString);
               DspData();
            }
	    opEdit->SetText("");
            opEdit->SetFocus();
	    break;
	case IDCANCEL:
            if (fpo)
            {
               fpo = CloseIt(opWin,fpo,String(IDS_TIJK_DAT,1));
               wait(1000);
            }
#ifdef WIN95
	    UnhookWindowsHookEx(hHook);
	    DestroyWindow(hWndTT);
#endif
	    TerminateDialog();
	    break;
        case IDC_NEXT:
            ptr = opFile->GetNextElement();
            DspData();
            opEdit->SetFocus();
            break;
        case IDC_PREV:
            ptr = opFile->GetPrevElement();
            if (ptr == NULL)
               ptr = String(IDS_BOF);
            DspData();
            opEdit->SetFocus();
            break;
        case IDC_CLOSE:
            fpo = CloseIt(opWin,fpo,String(IDS_TIJK_DAT,1));
            opEdit->SetFocus();
            break;
        case IDC_FILE:
            fpo = Open(opWin,fpo,String(IDS_TIJK_DAT,1),
                       opFile->GetTitleOfRun(),first);
            opEdit->SetFocus();
            break;
        case IDC_ERASE:
            opWin->Erase();
            opEdit->SetFocus();
            break;
	default:
	    break;
    }
    return FALSE;
}

VOID TijkDialog :: DspData(VOID)
{
   char buffer[1024];

   if (ptr == 0L)
   {  opFile->Rewind();
      opWin->WriteLn(String(IDS_EOF_RF));
   }
   else if (strcmp(ptr,String(IDS_BOF)) == 0)
   {  opFile->Rewind();
      opWin->WriteLn(String(IDS_BOF));
   }
   else
   {  
      strcpy(buffer,String(IDS_RN_FORM));
      strcat(buffer,ptr);
      if (fpo)
         fprintf(fpo,String(IDS_NSN_FORM),ptr);

      INDVAL *ptr1;
      for (int i=0; i<5; i++)
      {  if ((ptr1 = opFile->GetSortedData(i)) != 0L)
         {  
            strcat(buffer,String(IDS_RN_FORM));
            strcat(buffer,String(IDS_RN_FORM));
            strcat(buffer,ptr1->indexline);
            strcat(buffer,String(IDS_RN_FORM));
            strcat(buffer,ptr1->valueline);
            if (fpo)
            {  fprintf(fpo,String(IDS_NSN_FORM),ptr1->indexline);
               fprintf(fpo,String(IDS_SN_FORM),ptr1->valueline);
            }
         }
      }
      opWin->WriteLn(buffer);
   }
}

#ifdef WIN95

BOOL TijkDialog :: DoCreateDialogTooltip(void) 
{ 
    hDlg = this->GetHandle();

    InitCommonControls(); 
    hWndTT = CreateWindowEx(0, TOOLTIPS_CLASS, (LPSTR) NULL, 
        TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
        CW_USEDEFAULT, hDlg, (HMENU) NULL, Application::GetInstance(), NULL); 
    if (hWndTT == NULL) 
        return FALSE; 
 
//  does not work anymore under Windows 2000 ?????
//    if (!EnumChildWindows(hDlg, (WNDENUMPROC) EnumChildProc, 0)) 
//        return FALSE; 

    AddControl(IDOK);
    AddControl(IDCANCEL);
    AddControl(IDC_NEXT);
    AddControl(IDC_PREV);
    AddControl(IDC_CLOSE);
    AddControl(IDC_FILE);
    AddControl(IDC_ERASE);
 
    hHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMsgProc, 
        (HINSTANCE) NULL, GetCurrentThreadId()); 
 
    if (hHook == (HHOOK) NULL) 
        return FALSE; 
 
    return TRUE; 
} 
 
long TijkDialog :: WmNotify(LPARAM lParam) 
{ 
    static CHAR szBuf[128];
 
    if ((((LPNMHDR) lParam)->code) == TTN_NEEDTEXT)
    { 
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lParam; 
	int idCtrl = GetDlgCtrlID((HWND)((LPNMHDR)lParam)->idFrom);
        LoadString(Application::GetInstance(),idCtrl,szBuf,sizeof(szBuf));
        lpttt->lpszText = szBuf;
    } 
    return 0L; 
} 

#endif 

///////////////////////////////////////////////////////////////////////////////

AuxWin :: AuxWin(int nCmdShow, LPCSTR szWindowName, RECT rc)
        : Window(nCmdShow, szWindowName, rc)
{
   Invalidate();
}

AuxWin :: ~AuxWin(VOID)
{
}

long AuxWin :: WmPaint( const PAINTSTRUCT & )
{
   return FALSE;
}

///////////////////////////////////////////////////////////////////////////////

AuxWin2 :: AuxWin2(int nCmdShow, LPCSTR szWindowName, RECT rc)
        : Window(nCmdShow, szWindowName, rc)
{
   Invalidate();
}

AuxWin2 :: ~AuxWin2(VOID)
{
}

long AuxWin2 :: WmPaint( const PAINTSTRUCT & )
{
   return FALSE;
}

///////////////////////////////////////////////////////////////////////////////

AuxWin3 :: AuxWin3(int nCmdShow, LPCSTR szWindowName, RECT rc)
        : Window(nCmdShow, szWindowName, rc)
{
   Invalidate();
}

AuxWin3 :: ~AuxWin3(VOID)
{
}

long AuxWin3 :: WmPaint( const PAINTSTRUCT & )
{
   return FALSE;
}

/////////////////////////////////// EOF ///////////////////////////////////////

