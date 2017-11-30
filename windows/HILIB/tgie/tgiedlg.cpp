///////////////////////////////////////////////////////////////////////////////
//////////////////////// class TgieDialog implementation //////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"
#define TIMER_ID  2    // timer identification
#define TIMER_ID3 3    // timer identification
#define TIMER_ID4 4    // timer identification
#define YBITMAP 16 

#ifdef __BORLANDC__
#define _ltoa ltoa
#endif

///////////////////////////////////////////////////////////////////////////////

char FltStr[] = "(*.*)|*.*|(*.dat)|*.dat|(*.bak)|*.bak|";
char Dir[]    = "";
char Ext[20]  = "*.dat";

//////////////////////////// for Tooltips /////////////////////////////////////

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

BOOL isLarge(HWND hWnd)
{
   RECT rc;
   GetWindowRect(hWnd,&rc);
   if ( ((float)(rc.right-rc.left)) / (float)348.0 > (float)2.2 )
      return TRUE;
   else
      return FALSE;
}

int AdjustForDialogboxSize(int typecode, BOOL update, BOOL Turtle, int ind)
{
   if (!Turtle && (typecode == 17))
	return (ind - 1);
   else if (!Turtle && (typecode == 50))
	return (ind - 1);
   else if (!Turtle && (typecode == 16))
	return (ind - 1);
   else if (update && (typecode == 6))
	return (ind - 2);
   else
	return ind;
}
///////////////////////////////////////////////////////////////////////////////

TgieDialog :: TgieDialog(int ResID, Window *opParent, char *Input)
            : ModalDialog(ResID,opParent)
{
   if (strcmp(Input,"FOR001.DAT") == 0)
   {
      strcpy(SCDIR,"");
      strcpy(for001,Input);    // Input = FOR001.DAT  (Turtle for some time)
   }
   else
   {
      strcpy(SCDIR,Input);     // Input = file directory
      strcpy(for001,SCDIR);    // SCDIR needed for open/save dialog
      strcat(for001,"FOR001.DAT");
   }
   int len = strlen(for001);
   int pos = len - 3;
   strcpy(for001bak,for001);
   strcpy(&for001bak[pos],"bak");  // dat  -->  bak 
   strcpy(for001bk2,for001);
   strcpy(&for001bk2[pos],"bk2");  // dat  -->  bk2

   hPar = opParent->GetHandle();
   ColorButtonWndFn(this->GetHandle(),0,0,0L);  // initialize COLORBUT.DLL
   Start();
}

TgieDialog :: ~TgieDialog(VOID)
{
   delete opListBox;
   delete opTitle;
   delete opData;
   delete opInput;
   delete opBefore;
   delete opAfter;
   delete opUpdate;
   delete opLimits;
   if (nSaved > 0)
   {
	for(int i=0; i<nSaved; i++)
	   delete []SavedLines[i];
	delete []SavedLines;
   }
   DeleteObject(hBitmap);
}

BOOL TgieDialog :: Init(VOID)
{
// Append to System menu

   SystemMenu SystemMenu(this);
   SystemMenu.AppendSep();
   SystemMenu.Append(IDM_ABOUT,"&About this GUI Editor...");

   opListBox = new ListBox(IDC_ELEM,this);
   SendDlgItemMessage(this->GetHandle(),IDC_ELEM,WM_SETFONT,
			(WPARAM)GetStockObject(SYSTEM_FIXED_FONT),FALSE);

   opTitle = new RadioButton(IDC_TIT,this);
   opData  = new RadioButton(IDC_DAT,this);
   opBefore = new RadioButton(IDC_BEFORE,this);
   opAfter  = new RadioButton(IDC_AFTER,this);
   UseTitle = FALSE;
   Before = FALSE;
   opTitle->SetState(0);
   opData->SetState(1);
   opBefore->SetState(0);
   opAfter->SetState(1);

   opInput  = NULL;
   opUpdate = NULL;
   opLimits = NULL;
   nlines   = 0;
   nSaved   = 0;

   if (isLarge(this->GetHandle()))
      hBitmap  = LoadBitmap(Application::GetInstance(),"MyIcons2");
   else
      hBitmap  = LoadBitmap(Application::GetInstance(),"MyIcons");

// color text of buttons

   for (int i=201; i<=204; i++)
      SendDlgItemMessage(this->GetHandle(),i,CRBM_SETTEXTCOLOR, TRUE, RGB(102,102,0));
   for (i=205; i<=211; i++)
      SendDlgItemMessage(this->GetHandle(),i,CRBM_SETTEXTCOLOR, TRUE, RGB(202,0,0));
   SendDlgItemMessage(this->GetHandle(),IDOK,CRBM_SETTEXTCOLOR, TRUE, RGB(202,0,0));

#ifdef WIN95
//   OSVERSIONINFO osvi;
//   osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
//   GetVersionEx(&osvi);
//   if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
//     ;  // Tooltips make program crashing under NT
//   else
     DoCreateDialogTooltip();
#endif

// test if file exists

   int hFile ;
   char mess[256];
   hFile = _lopen(for001,0);
   if (hFile == -1)
   {
	strcpy(mess,"Non-existing path '");
	strcat(mess,for001);
	strcat(mess,"'\ngiven as parameter to TGIE32.EXE.\nThus, FOR001.DAT could not be opened.");
	MessageBox(NULL,mess,"Fatal message issued by TGIE",MB_OK);
	Exit();
   }
   else
   {
	ReadInput(for001);
	strcpy(CurrentFileName,"FOR001.DAT");
	SetTimer(this->GetHandle(),TIMER_ID3,500,NULL); // kill button focus
   }
   return FALSE;
}

long TgieDialog :: WmMeasureItem(WPARAM, LPARAM lParam)
{
   LPMEASUREITEMSTRUCT lpmis; 

   lpmis = (LPMEASUREITEMSTRUCT) lParam; 
 
   lpmis->itemHeight = 14; 
   lpmis->itemWidth  = 14; 

   return TRUE; 
}

long TgieDialog :: WmDrawItem(WPARAM, LPARAM lParam)
{
// Filling in an owner-drawn list box.
// The list box must have the LBS_OWNERDRAWFIXED and LBS_HASSTRINGS styles in addition.

   LPDRAWITEMSTRUCT lpdis; 
   HBITMAP hbmpOld; 
   HDC hdcMem; 
   RECT rcBitmap; 
   TCHAR tchBuffer[256], dummy[156]; 
   TEXTMETRIC tm; 
   int x, y, ipos1, XBITMAP;
//   SIZE size;

   lpdis = (LPDRAWITEMSTRUCT) lParam; 
   if (lpdis->itemID == -1) 
      return TRUE; 

// Draw the bitmap and text for the list box item. Draw a  
// rectangle around the bitmap if it is selected. 

   switch (lpdis->itemAction) 
   { 
	case ODA_SELECT: 
	case ODA_DRAWENTIRE: 

          SendMessage(lpdis->hwndItem, LB_GETTEXT, lpdis->itemID, (LPARAM) tchBuffer); 
          hdcMem  = CreateCompatibleDC(lpdis->hDC); 
          hbmpOld = (HBITMAP)SelectObject(hdcMem, hBitmap); 
          GetTextMetrics(lpdis->hDC, &tm); 
 
//	  Display the bitmap associated with the item ...
 
	  {
//	     from typecode get bitmap section ipos1

	     if (UseTitle)
		typecode = opInput->Title2Typecode(tchBuffer);
	     else
		typecode = opInput->MakeTitle(tchBuffer,dummy,FALSE,FALSE);

	     if ((typecode == 6) && update)
		ipos1 = typecode + 34;
	     else if ((typecode == 6) && !update)
	     {
		if (UseTitle)
		{
		   if (tchBuffer[0] == '1')
			ipos1 = typecode + 4;    // 1-dim slit
		   else
			ipos1 = typecode + 33;   // 2-dim slit
		}
		else
		{
		   char p1[30], p2[30], p3[30], p4[30], p5[30], p6[30];
	           int n = sscanf(tchBuffer,"%s %s %s %s %s %s",p1,p2,p3,p4,p5,p6);
		   if (n <= 5)
		      ipos1 = typecode + 4;    // 1-dim slit
		   else
		      ipos1 = typecode + 33;   // 2-dim slit
		}
	     }
	     else if (typecode >= 50)
	     {
	        if (!Turtle)
	           ipos1 = typecode - 18;
	        else
	           ipos1 = typecode - 15;
	     }
	     else
	        ipos1 = typecode + 4;    

	     int isize;
	     if (!isLarge(this->GetHandle()))
	     {
		isize   = 15;
	        XBITMAP = 22; 
	     }
	     else
	     {
		isize   = 19;
	        XBITMAP = 28; 
	     }
             BitBlt(lpdis->hDC, 
                    lpdis->rcItem.left, lpdis->rcItem.top, 
		    isize, isize,
                    hdcMem, isize*ipos1, 0, SRCCOPY); 
	  } 

//	  Display the text associated with the item. 
 
	  x = XBITMAP;
          y = (lpdis->rcItem.bottom + lpdis->rcItem.top - tm.tmHeight) / 2; 
          TextOut(lpdis->hDC, x-7, y, "   ", 3); 
          TextOut(lpdis->hDC, x, y, tchBuffer, strlen(tchBuffer)); 

//        Is the item selected? 

          if (lpdis->itemState & ODS_SELECTED) 
          { 
//		RECT coordinates to surround bitmap and text. 
 
		rcBitmap.left = lpdis->rcItem.left;
		rcBitmap.top = lpdis->rcItem.top; 
//		GetTextExtentPoint32(hdcMem,tchBuffer,strlen(tchBuffer),&size); // not working for fixed pitch fonts
//		rcBitmap.right = lpdis->rcItem.left + x + size.cx ;
		rcBitmap.right = lpdis->rcItem.left + x + strlen(tchBuffer)*tm.tmAveCharWidth ;
		rcBitmap.bottom = lpdis->rcItem.top + tm.tmHeight; 
 
//		Draw a rectangle around bitmap and text to indicate the selection. 
 
                DrawFocusRect(lpdis->hDC, &rcBitmap); 
          } 
          SelectObject(hdcMem, hbmpOld); 
          DeleteDC(hdcMem); 
          break; 
 
        case ODA_FOCUS: 
 
          break; 
    } 
    return TRUE; 
}

BOOL TgieDialog :: WmTimer(UINT id)
{
    int i;
    int ind[]  = {1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1, 0,0,0,0,0,
		  0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,1, 0,0,0,0,0, 0,0,0,0,0} ;
    int indt[] = {1,1,1,1,1, 1,1,0,1,0, 1,1,1,1,1, 1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1,
		  0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,1, 1,1,1,0,0, 0,0,0,0,1} ;

    if (id == TIMER_ID3) // kill button focus (delayed)
    {
        HWND hp = this->GetHandle();
        HWND ha = GetActiveWindow();
        HWND h1 = GetParent(ha);
        HWND h2 = GetParent(hp);

//      only set focus if on top; otherwise would interfere with display of Info window

        if (h1 == h2)
           opListBox->SetFocus();
    }
    else if (id == TIMER_ID4) // kill button focus befor displaying new window
    {
        KillTimer(this->GetHandle(),id);

	RECT rc = {10, 50, 710, 410};  // x,y,w,h
	if (isLarge(this->GetHandle()))
	{
	   rc.right  = 888;
	   rc.bottom = 512;
	}
        opText = new TextWindow(SW_SHOWNORMAL,"Help Information",rc,hPar);
//        opText = new TextWindow(SW_SHOWNORMAL,"Help Information",rc,this->GetHandle());
        opText->ShowFile("TGIE.TXT");
        opText->Home();
    }
    else if (id == TIMER_ID)
    {
	if (update && opUpdate)
		SendMessage(opUpdate->GetHandle(),WM_SETTEXT,(WPARAM)0,(LPARAM)(LPCSTR)"Update");
	else
		SendMessage(opUpdate->GetHandle(),WM_SETTEXT,(WPARAM)0,(LPARAM)(LPCSTR)"Slit");

	if (Turtle)
		SendMessage(opLimits->GetHandle(),WM_SETTEXT,(WPARAM)0,(LPARAM)(LPCSTR)"Histo-1");
	else
		SendMessage(opLimits->GetHandle(),WM_SETTEXT,(WPARAM)0,(LPARAM)(LPCSTR)"Limits");
	
	for (i=1; i<=60; i++)
	{
	   HWND hwnd;
	   if (Turtle)
	   {
	     if (indt[i-1] == 1)
	     {
		hwnd = GetDlgItem(this->GetHandle(),i+5000);
		if (hwnd != NULL)		
		{
		   SendMessage(hwnd,CRBM_SETTEXTCOLOR, TRUE, RGB(0,0,192));
		   EnableWindow(hwnd,TRUE);
		}
	     }
	     else
	     {
		hwnd = GetDlgItem(this->GetHandle(),i+5000);
		if (hwnd != NULL)		
		{
		   SendMessage(hwnd,CRBM_SETTEXTCOLOR, TRUE, RGB(0,152,0));
		   EnableWindow(hwnd,FALSE);
		}
	     }
	   }
	   else
	   {
	     if (ind[i-1] == 1)
	     {
		hwnd = GetDlgItem(this->GetHandle(),i+5000);
		if (hwnd != NULL)		
		{
		   SendMessage(hwnd,CRBM_SETTEXTCOLOR, TRUE, RGB(0,0,192));
		   EnableWindow(hwnd,TRUE);
		}
	     }
	     else
	     {
		hwnd = GetDlgItem(this->GetHandle(),i+5000);
		if (hwnd != NULL)		
		{
		   SendMessage(hwnd,CRBM_SETTEXTCOLOR, TRUE, RGB(0,152,0));
		   EnableWindow(hwnd,FALSE);
		}
	     }
	   }
	}
	RECT rcDlg;
	GetWindowRect(this->GetHandle(),&rcDlg);  // Dialog, relative to screen origin
	int xx = rcDlg.right - rcDlg.left - 2 * GetSystemMetrics(SM_CXFRAME);
	int yy = rcDlg.bottom - rcDlg.top - 2 * GetSystemMetrics(SM_CYFRAME)
                                          - GetSystemMetrics(SM_CYCAPTION);
	RECT rc = {(3*xx)/4,yy/10,xx,yy};
        InvalidateRect(this->GetHandle(),&rc,TRUE);
	UpdateWindow(this->GetHandle());
        KillTimer(this->GetHandle(),id);
    }
    return FALSE;
}

void TgieDialog :: ReadInput(char *FileName)
{
   int i;

   int nsavel = nlines;
   int topindex = (int)SendMessage(opListBox->GetHandle(),LB_GETTOPINDEX,(WPARAM)0,0);
   SendMessage(opListBox->GetHandle(),LB_RESETCONTENT,(WPARAM)0,0);

   if (opInput)
	delete opInput;
   opInput = new TransInFile(FileName);
   nlines  = opInput->TellNumberOfLinesEx();

   update = opInput->GetUpdateInfo();
   angle  = opInput->GetAngleInfo();

// Make List Box horizontally scrollable (~110 chars)

   SendDlgItemMessage(this->GetHandle(),IDC_ELEM,LB_SETHORIZONTALEXTENT,1000L,0L);

   for (i=0; i<nlines; i++)
   {
     if (UseTitle)
	opListBox->Append(opInput->GetTitle(i));
     else
	opListBox->Append(opInput->GetLine(i,0));
   }


   if (opUpdate)
      delete opUpdate;
   opUpdate  = new PushButton(IDC_SLIT,this);

   if (opInput->IsForTurtle())
   {
	SetWindowText(this->GetHandle(),"Turtle GUI Input Editor");
	Turtle = TRUE;
   }
   else
   {
	SetWindowText(this->GetHandle(),"Transport GUI Input Editor");
	Turtle = FALSE;
   }
   if (opLimits)
      delete opLimits;
   opLimits  = new PushButton(IDC_LIM,this);

   if (nlines > 0)
      SendMessage(GetDlgItem(this->GetHandle(),IDR_TTEXT),WM_SETTEXT,(WPARAM)0,
		         (LPARAM)"Loaded input file is current FOR001.DAT");
   else
      SendMessage(GetDlgItem(this->GetHandle(),IDR_TTEXT),WM_SETTEXT,(WPARAM)0,
		         (LPARAM)"Loaded input file is NONE");

   if (nsavel == nlines)  // probably the same file
      SendMessage(opListBox->GetHandle(),LB_SETTOPINDEX,(WPARAM)topindex,0);
   SetTimer(this->GetHandle(), TIMER_ID, 500, NULL);
}

long TgieDialog :: WmSysMenuCommand(WPARAM MenuItem)
{
//  Handle system menu command events.

    switch (MenuItem)
    {   case IDM_ABOUT:
	   SimpleDialog(IDD_ABOUT_DIALOG,this);
	   return FALSE;
	case SC_CLOSE:
	   Exit();
	   return TRUE;
        default:
	   return FALSE;
    }
}

VOID TgieDialog :: EditMultiLine(int nSel, int *aiSelect)
{
   char buf[1001];
   char line[133];
   char crlf[3];
   crlf[0] = 0xD;
   crlf[1] = 0xA;
   crlf[2] = '\0';

   strcpy(buf,"");
   int index;
   int oldindex = aiSelect[0] - 1;
   for (int i=0; i<nSel; i++)
   {
	index = aiSelect[i]; 
	if (( index - oldindex) != 1)
	{
//	   MessageBox(this->GetHandle(),"Selected line range has to be contiguous",
//					"TGIE: Edit mode error",MB_OK | MB_ICONSTOP);
//	   return;
	}
	strcpy(line,opInput->GetLine(index,0));
	if (strlen(buf) + strlen(line) > 950)
	{
	   MessageBox(this->GetHandle(),"Too many lines selected!",
					"TGIE: Edit mode error",MB_OK | MB_ICONSTOP);
	   return;
	}
	strcat(buf,line);
	strcat(buf,crlf);
	oldindex = index;
   }

   TEditDialog *opTEdit = new TEditDialog(IDD_STRING3_DIALOG,this,buf,1001,TRUE);
   if (opTEdit->GetResult())
   {
	SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)FALSE,MAKELPARAM(-1,0));
	char title[31];
	char *p;
	strcpy(buf,opTEdit->GetString());
	int first = 1;
	int index;
	for (int j=0; j<nSel; j++)
	{
	     if (first == 1)
	     {
	       p = strtok(buf,crlf);
	       first = 0;
	     }		   
	     else
	       p = strtok(NULL,crlf);

	     if (p != NULL)
	       strcpy(line,p);
	     else
	       break;

	     index = aiSelect[j];
	     int typecode = opInput->MakeTitle(line,title,FALSE,FALSE);
	     opInput->SetLine(line,index);
	     opInput->SetTitle(title,index);
	     SendMessage(opListBox->GetHandle(),LB_DELETESTRING,(WPARAM)index,0);
	     if (UseTitle)
		SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
					(WPARAM)index, (LPARAM)title);
	     else
		SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
					(WPARAM)index, (LPARAM)&line[0]);
	     SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)TRUE,MAKELPARAM(index,0));
	}
   }
   delete opTEdit;
}

VOID TgieDialog :: EditLine(int index)
{
	char line[233], title[31];
	strcpy(line,opInput->GetLine(index,0));
	opEdit = new StringDialog(IDD_STRING_DIALOG,this,line,232,TRUE); // fixed pitch font
	if (opEdit->GetResult())
	{
		strcpy(line,opEdit->GetString());
		typecode = opInput->MakeTitle(line,title,FALSE,FALSE);
		opInput->SetLine(line,index);
		opInput->SetTitle(title,index);
		SendMessage(opListBox->GetHandle(),LB_DELETESTRING,(WPARAM)index,0);
		if (UseTitle)
		   SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
					(WPARAM)index, (LPARAM)title);
		else
		   SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
					(WPARAM)index, (LPARAM)&line[0]);
	}
	delete opEdit;
	SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)TRUE,MAKELPARAM(index,0));
}

#ifdef WIN95
long TgieDialog :: WmContextMenu(WPARAM wParam, LPARAM) // if right mouse button pressed ...
{
    if ((HWND)wParam == opListBox->GetHandle())
        SendMessage(this->GetHandle(),WM_COMMAND,(WPARAM)IDC_EDT,(LPARAM)0);
    return FALSE;
}
#endif

long TgieDialog :: Control(UINT nControlId, UINT nNotificationCode)
{
   int retcode = 0;

   switch (nControlId)
   {
      case IDC_EXIT:
	Exit();
	break;
      case IDOK:
	{
	    BOOL ok;
	    unsigned char pbytes[256];

	    GetKeyboardState(&pbytes[0]);

	    if (pbytes[VK_MENU] >= 128) // Alt-key pressed at the same time
	    {
		SaveFileDlg *opF = new SaveFileDlg(this->GetHandle(), SCDIR, FltStr, Ext);
		if (opF->GetResult())
		{
		   char temp[255], Name[100];
		   strcpy(temp,opF->GetFileName());
		   int len = strlen(temp);
		   for (int i=0; i<len; i++)  // get rid of path
		   {
		      int j = len - i - 1;
		      if (temp[j] == '\\')
		      {
			 strcpy(Name,&temp[j+1]);
			 break;
		      }
		   }		   
		   opInput->OverWrite(Name,nlines);
		   strcpy(CurrentFileName,Name);
		   sprintf(temp,"Saved/loaded input file is %s",CurrentFileName);
		   SendMessage(GetDlgItem(this->GetHandle(),IDR_TTEXT),WM_SETTEXT,(WPARAM)0,
		        		 (LPARAM)temp);
		}
		delete opF;
	    }
	    else
	    {
	      if (strcmp(CurrentFileName,"FOR001.DAT") != 0)
	      {
		SimpleDialog oDlg(IDD_QEND_DIALOG,this,"Save Input to FOR001.DAT ?");
		ok = oDlg.GetResult();
	      }
	      else
		ok = TRUE;
	      if (ok)
	      {
		remove(for001bk2);
		rename(for001bak,for001bk2);
		remove(for001bak);
		rename(for001,for001bak);
		opInput->OverWrite(for001,nlines);
		SendMessage(GetDlgItem(this->GetHandle(),IDR_TTEXT),WM_SETTEXT,(WPARAM)0,
			    (LPARAM)"Saved/loaded input file is current FOR001.DAT");
		strcpy(CurrentFileName,"FOR001.DAT");
	      }
	    }
	}
	break;
      case IDC_EDT:
	{ 
	   int nSel;
	   int aiSelect[1000];
	   nSel = (int)SendMessage(opListBox->GetHandle(),LB_GETSELITEMS,
				  (WPARAM)(sizeof(aiSelect)/sizeof(int)),(LPARAM)aiSelect);
	   if (nSel > 1)
	   {
	      EditMultiLine(nSel,aiSelect);
	      break;
	   }
	   else if (nSel == 1)
	   {
	      int index = aiSelect[0];   
	      char line[133], title[31];
	      strcpy(line,opInput->GetLine(index,0));
	      typecode = opInput->MakeTitle(line,title,opInput->IsEquad(index),opInput->IsEbend(index));
	      if (typecode <= 0)
		 EditLine(index);
	      else
	      {
		 if ((typecode <= 30) || ((typecode>=50) && (typecode<=53)) || (typecode == 60))
		 {
		    int ind  = GetNumberOfDataItems(typecode,update,Turtle);
		    int ind2 = AdjustForDialogboxSize(typecode,update,Turtle,ind);
		    opParDialog = new ParDialog(IDD_1_DIALOG+ind2,this,line,title,update,angle,Turtle,opInput->IsEquad(index),opInput->IsEbend(index));
		 }
		 else
		 {
		    sprintf(title,"Typecode %d",typecode);
		    strcpy(line,"");
		    opParDialog = new ParDialog(IDD_INFO_DIALOG,this,line,title,update,angle,Turtle,FALSE,FALSE);
		 }
		 if (opParDialog->GetResult())
		 {
		    strcpy(line,opParDialog->GetString());
		    retcode = 1;
		    if (strlen(line) == 0)
			retcode = 0;
		 }
		 delete opParDialog;
		 if (retcode > 0)
		 {
		     typecode = opInput->MakeTitle(line,title,FALSE,FALSE);
		     opInput->SetLine(line,index);
		     opInput->SetTitle(title,index);
		     SendMessage(opListBox->GetHandle(),LB_DELETESTRING,(WPARAM)index,0);
		     if (UseTitle)
			SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
					(WPARAM)index, (LPARAM)title);
		     else
			SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
					(WPARAM)index, (LPARAM)&line[0]);

		     SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)TRUE,MAKELPARAM(index,0));
		 }
	      }
	   }
	   else
		SimpleDialog oDlg(IDD_INFO_DIALOG,this,"No Element Selected !");
	}
        break;
      case IDC_ELEM:
        switch (nNotificationCode)
        {  case LBN_DBLCLK:
             {
		int index = opListBox->GetCrntInd();   
        	EditLine(index);
             }
	     break;
           default:
	     break;  
        }
	break;
      case IDC_READ:
	{
	    BOOL ok;
	    unsigned char pbytes[256];

	    GetKeyboardState(&pbytes[0]);

	    if (pbytes[VK_CONTROL] >= 128) // Ctrl-key pressed at the same time
	    {
		if (strcmp(CurrentFileName,"FOR001.BAK") != 0)
		{
		   SimpleDialog oDlg(IDD_QEND_DIALOG,this,"Overwrite current Data ?");
		   ok = oDlg.GetResult();
		}
		else
		   ok = TRUE;
		if (ok)
		{
		   ReadInput(for001bak);
	           strcpy(CurrentFileName,"FOR001.BAK");
		   SendMessage(GetDlgItem(this->GetHandle(),IDR_TTEXT),WM_SETTEXT,(WPARAM)0,
		         (LPARAM)"Loaded input file is current FOR001.BAK");
		}
	    }
	    else if (pbytes[VK_MENU] >= 128) // Alt-key pressed at the same time
	    {
		OpenFileDlg *opF = new OpenFileDlg(this->GetHandle(), SCDIR, FltStr, Ext);
		if (opF->GetResult())
		{
		   char temp[255], Name[100];
		   strcpy(temp,opF->GetFileName());
		   int len = strlen(temp);
		   for (int i=0; i<len; i++)  // get rid of path
		   {
		      int j = len - i - 1;
		      if (temp[j] == '\\')
		      {
			 strcpy(Name,&temp[j+1]);
			 break;
		      }
		   }		   
		   if (strcmp(CurrentFileName,Name) != 0)
		   {
			SimpleDialog oDlg(IDD_QEND_DIALOG,this,"Overwrite current Data ?");
			ok = oDlg.GetResult();
		   }
		   else
			ok = TRUE;
		   if (ok)
		   {
			strcpy(CurrentFileName,Name);
			ReadInput(CurrentFileName);
			sprintf(temp,"Loaded input file is %s",CurrentFileName);
			SendMessage(GetDlgItem(this->GetHandle(),IDR_TTEXT),WM_SETTEXT,(WPARAM)0,
		        		 (LPARAM)temp);
		   }
		}
		delete opF;
	    }
	    else
	    {
		if (strcmp(CurrentFileName,"FOR001.DAT") != 0)
		{
		   SimpleDialog oDlg(IDD_QEND_DIALOG,this,"Overwrite current Data ?");
		   ok = oDlg.GetResult();
		}
		else
		   ok = TRUE;
		if (ok)
		{
		   ReadInput(for001);
	           strcpy(CurrentFileName,"FOR001.DAT");
		   SendMessage(GetDlgItem(this->GetHandle(),IDR_TTEXT),WM_SETTEXT,(WPARAM)0,
		         (LPARAM)"Loaded input file is current FOR001.DAT");
		}
	    }
	}
	break;
      case IDC_NEWINPUT:
	{
	   int i;
	   BOOL ok;

	   if (strcmp(CurrentFileName,"NEW") != 0)
	   {
		SimpleDialog oDlg(IDD_QEND_DIALOG,this,"Overwrite current Data ?");
		ok = oDlg.GetResult();
	   }
	   else
		ok = TRUE;
	   if (ok)
	   {
		strcpy(CurrentFileName,"NEW");
		SendMessage(opListBox->GetHandle(),LB_RESETCONTENT,(WPARAM)0,0);
		nlines = opInput->GetNew();

		update = opInput->GetUpdateInfo();
		angle  = opInput->GetAngleInfo();

		SetTimer(this->GetHandle(), TIMER_ID, 500, NULL);

		for (i=0; i<nlines; i++)
		{
		   if (UseTitle)
		      opListBox->Append(opInput->GetTitle(i));
	           else
		      opListBox->Append(opInput->GetLine(i,0));
		}
		SendMessage(GetDlgItem(this->GetHandle(),IDR_TTEXT),WM_SETTEXT,(WPARAM)0,
			    (LPARAM)"Loaded input file is new.");
	   }
	}
	break;
      case IDC_INSERT:
	{
	   int nSel;
	   int aiSelect[1000];
	   nSel = (int)SendMessage(opListBox->GetHandle(),LB_GETSELITEMS,
				  (WPARAM)(sizeof(aiSelect)/sizeof(int)),(LPARAM)aiSelect);
	   int index;
	   if (nSel > 1)
	   {
	      MessageBox(this->GetHandle(),"More than 1 line selected.",
					   "TGIE: Insert mode error",MB_OK | MB_ICONSTOP);
	      break;
	   }
	   if (nSel == 1)
	      index = aiSelect[0];   
	   else
	      index = -1;
	   int nr = (int)SendMessage(opListBox->GetHandle(),LB_GETCOUNT,(WPARAM)0,(LPARAM)0);
	   if ((index >= 0) || (nr == 0))
	   {
		if (nr == 0)  // empty box
		   index = 0;
		else
		{
		   if (!Before)
		      index++; 
		}
		char line[133];
		char title[31];
		strcpy(line,"");
		TEditDialog *opTEdit = new TEditDialog(IDD_STRING2_DIALOG,this,line,1000,TRUE);
		if (opTEdit->GetResult())
		{
		   char *p;
		   char crlf[3];
		   crlf[0] = 0xD;
		   crlf[1] = 0xA;
		   crlf[2] = '\0';
		   char temp[1001];
		   strcpy(temp,opTEdit->GetString());

		   int first = 1;
		   for (;;)
		   {
		     if (first == 1)
		     {
		       p = strtok(temp,crlf);
		       first = 0;
		     }		   
		     else
		       p = strtok(NULL,crlf);
		     if (p)
		       strcpy(line,p);
		     else
		       break;

		     for (int i=nlines; i>index; i--)
		     {
		       opInput->SetTitle(opInput->GetTitle(i-1),i);
		       opInput->SetLine(opInput->GetLine(i-1,0),i);
		     }
		     nlines++;

		     typecode = opInput->MakeTitle(line,title,FALSE,FALSE);
		     if (UseTitle)
			SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
					(WPARAM)index, (LPARAM)title);
		     else
			SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
					(WPARAM)index, (LPARAM)&line[0]);
		     opInput->SetTitle(title,index);
		     opInput->SetLine(line,index);
		     SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)FALSE,MAKELPARAM(-1,0));
		     SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)TRUE,MAKELPARAM(index,0));
		     index++;
		   }
		}
		delete opTEdit;
	   }
	   else
		SimpleDialog oDlg(IDD_INFO_DIALOG,this,"No Element Selected !");
	}
	break;
      case IDC_DAT:
	{
	   int nSel;
	   int aiSelect[1000];
	   nSel = (int)SendMessage(opListBox->GetHandle(),LB_GETSELITEMS,
				  (WPARAM)(sizeof(aiSelect)/sizeof(int)),(LPARAM)aiSelect);
	   int cursorindex;
	   if (nSel > 0)
	      cursorindex = aiSelect[0];
	   else
	      cursorindex = -1;
	   int i;
	   int topindex = (int)SendMessage(opListBox->GetHandle(),LB_GETTOPINDEX,(WPARAM)0,0);
	   SendMessage(opListBox->GetHandle(),LB_RESETCONTENT,(WPARAM)0,0);
	   for (i=0; i<nlines; i++)
		opListBox->Append(opInput->GetLine(i,0));
	   UseTitle = FALSE;
	   opTitle->SetState(0);
	   opData->SetState(1);
	   if (cursorindex != -1)
	     for(int i=0; i<nSel; i++)
	       SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)TRUE,MAKELPARAM(aiSelect[i],0));
	   else
	       SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)FALSE,MAKELPARAM(-1,0));
	   SendMessage(opListBox->GetHandle(),LB_SETTOPINDEX,(WPARAM)topindex,0);
	   update = opInput->GetUpdateInfo();
	   angle  = opInput->GetAngleInfo();
	}
	break;
      case IDC_TIT:
	{
	   int nSel;
	   int aiSelect[1000];
	   nSel = (int)SendMessage(opListBox->GetHandle(),LB_GETSELITEMS,
				  (WPARAM)(sizeof(aiSelect)/sizeof(int)),(LPARAM)aiSelect);
	   int cursorindex;
	   if (nSel > 0)
	      cursorindex = aiSelect[0];
	   else
	      cursorindex = -1;
	   int i;
	   int topindex = (int)SendMessage(opListBox->GetHandle(),LB_GETTOPINDEX,(WPARAM)0,0);
	   SendMessage(opListBox->GetHandle(),LB_RESETCONTENT,(WPARAM)0,0);
	   for (i=0; i<nlines; i++)
//		opListBox->Append(opInput->GetTitle(i));
		opListBox->Append(opInput->GetTitleSpec(i,Turtle));
	   UseTitle = TRUE;
	   opTitle->SetState(1);
	   opData->SetState(0);
	   if (cursorindex != -1)
	   if (cursorindex != -1)
	     for(int i=0; i<nSel; i++)
	       SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)TRUE,MAKELPARAM(aiSelect[i],0));
	   else
	       SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)FALSE,MAKELPARAM(-1,0));
	   SendMessage(opListBox->GetHandle(),LB_SETTOPINDEX,(WPARAM)topindex,0);
	   update = opInput->GetUpdateInfo();
	   angle  = opInput->GetAngleInfo();
	}
	break;
      case IDC_MOVEDOWN:
	{
	   int nSel;
	   int aiSelect[1000];
	   nSel = (int)SendMessage(opListBox->GetHandle(),LB_GETSELITEMS,
				  (WPARAM)(sizeof(aiSelect)/sizeof(int)),(LPARAM)aiSelect);
	   if (nSel == 0)
	   {
		SimpleDialog oDlg(IDD_INFO_DIALOG,this,"No Element Selected !");
		break;
	   }
	   else if (aiSelect[nSel-1] == nlines-1)
	   {
		SimpleDialog oDlg(IDD_INFO_DIALOG,this,"Bottom reached !");
		break;
	   }
	   else
	   {
	     for (int i=0; i<nSel; i++)
	     {
		char temp[133];
		int k = nSel - 1 - i;
		int index = aiSelect[k];
		strcpy(temp,opInput->GetTitle(index));
		opInput->SetTitle(opInput->GetTitle(index+1),index);
		opInput->SetTitle(temp,index+1);
		strcpy(temp,opInput->GetLine(index,0));
		opInput->SetLine(opInput->GetLine(index+1,0),index);
		opInput->SetLine(temp,index+1);
		SendMessage(opListBox->GetHandle(),LB_DELETESTRING,(WPARAM)index,0);
		if (UseTitle)
		   SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
			(WPARAM)index, (LPARAM)opInput->GetTitle(index));
		else
		   SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
			(WPARAM)index, (LPARAM)opInput->GetLine(index,0));
		SendMessage(opListBox->GetHandle(),LB_DELETESTRING,(WPARAM)index+1,0);
		if (UseTitle)
		   SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
			(WPARAM)(index+1), (LPARAM)opInput->GetTitle(index+1));
		else
		   SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
			(WPARAM)(index+1), (LPARAM)opInput->GetLine(index+1,0));
		SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)FALSE,MAKELPARAM(index,0));
		SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)TRUE,MAKELPARAM(index+1,0));
	     }
	   }
	}
	break;
      case IDC_MOVEUP:
	{
	   int nSel;
	   int aiSelect[1000];
	   nSel = (int)SendMessage(opListBox->GetHandle(),LB_GETSELITEMS,
				  (WPARAM)(sizeof(aiSelect)/sizeof(int)),(LPARAM)aiSelect);
	   if (nSel == 0)
	   {
		SimpleDialog oDlg(IDD_INFO_DIALOG,this,"No Element Selected !");
		break;
	   }
	   else if (aiSelect[0] == 0)
	   {
		SimpleDialog oDlg(IDD_INFO_DIALOG,this,"Top reached !");
		break;
	   }
	   else
	   {
	     for (int i=0; i<nSel; i++)
	     {
		char temp[133];
		int index = aiSelect[i];
		strcpy(temp,opInput->GetTitle(index));
		opInput->SetTitle(opInput->GetTitle(index-1),index);
		opInput->SetTitle(temp,index-1);
		strcpy(temp,opInput->GetLine(index,0));
		opInput->SetLine(opInput->GetLine(index-1,0),index);
		opInput->SetLine(temp,index-1);
		SendMessage(opListBox->GetHandle(),LB_DELETESTRING,(WPARAM)index,0);

		if (UseTitle)
		   SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
			(WPARAM)index, (LPARAM)opInput->GetTitle(index));
		else
		   SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
			(WPARAM)index, (LPARAM)opInput->GetLine(index,0));
		SendMessage(opListBox->GetHandle(),LB_DELETESTRING,(WPARAM)index-1,0);

		if (UseTitle)
		   SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
			(WPARAM)(index-1), (LPARAM)opInput->GetTitle(index-1));
		else
		   SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
			(WPARAM)(index-1), (LPARAM)opInput->GetLine(index-1,0));

		SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)FALSE,MAKELPARAM(index,0));
		SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)TRUE,MAKELPARAM(index-1,0));
	     }
	   }
	}
	break;
      case IDC_DELETE:
	{
	   int nSaved2;
	   int aiSelect[1000];
	   nSaved2 = (int)SendMessage(opListBox->GetHandle(),LB_GETSELITEMS,
				      (WPARAM)(sizeof(aiSelect)/sizeof(int)),(LPARAM)aiSelect);
	   if (nSaved2 > 0)
	   {
		if (nSaved > 0)
		{
		   for(int i=0; i<nSaved; i++)
			delete []SavedLines[i];
		   delete []SavedLines;
		}
		nSaved = nSaved2;
		SavedLines = (char **) new long[nSaved];
		for(int i=0; i<nSaved; i++)
	        {
		   int k = nSaved - 1 - i;
		   SavedLines[k] = new char[133];
		   strcpy(SavedLines[k],opInput->GetLine(aiSelect[k],0));
	           SendMessage(opListBox->GetHandle(),LB_DELETESTRING,(WPARAM)aiSelect[k],NULL);
		   for (int j=aiSelect[k]; j<nlines-1; j++)
		   {
		      opInput->SetTitle(opInput->GetTitle(j+1),j);
		      opInput->SetLine(opInput->GetLine(j+1,0),j);
		   }
		   nlines--;
		}
		int index = aiSelect[0];
		if (index > 0)
		   index--;
		if (Before)
		   SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)TRUE,MAKELPARAM(index+1,0));
		else
		   SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)TRUE,MAKELPARAM(index,0));
	   }
	   else
		SimpleDialog oDlg(IDD_INFO_DIALOG,this,"No Element Selected !");
	}
	break;
      case IDC_COPY:
	{
	   int nSaved2;
	   int aiSelect[1000];
	   nSaved2 = (int)SendMessage(opListBox->GetHandle(),LB_GETSELITEMS,
				      (WPARAM)(sizeof(aiSelect)/sizeof(int)),(LPARAM)aiSelect);
	   if (nSaved2 > 0)
	   {
		int index = aiSelect[nSaved2-1]; // last selected line
		if (nSaved > 0)
		{
		   for(int i=0; i<nSaved; i++)
		      delete []SavedLines[i];
		   delete []SavedLines;
		}
		nSaved = nSaved2;
		SavedLines = (char **) new long[nSaved];
		for(int i=0; i<nSaved; i++)
		{
		   SavedLines[i] = new char[133];
		   strcpy(SavedLines[i],opInput->GetLine(aiSelect[i],0));
		}			
		SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)FALSE,MAKELPARAM(-1,0));
		SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)TRUE,MAKELPARAM(index,0));
	   }
	   else
		SimpleDialog oDlg(IDD_INFO_DIALOG,this,"No Element Selected !");
	}
	break;
      case IDC_PASTE:
	{
	   int index;
	   int nSaved2;
	   int aiSelect[1000];
	   nSaved2 = (int)SendMessage(opListBox->GetHandle(),LB_GETSELITEMS,
				      (WPARAM)(sizeof(aiSelect)/sizeof(int)),(LPARAM)aiSelect);
	   if (nSaved2 > 1)
	   {
	      MessageBox(this->GetHandle(),"More than 1 line selected.",
					   "TGIE: Paste mode error",MB_OK | MB_ICONSTOP);
	      break;
	   }
	   else if (nSaved2 == 1)
		index = aiSelect[0];
	   else
		index = -1;
	   int nr = (int)SendMessage(opListBox->GetHandle(),LB_GETCOUNT,(WPARAM)0,(LPARAM)0);
	   if ((index >= 0) || (nr == 0))
	   {
		if (nr == 0)  // empty box
		   index = 0;
		else
		{
		   if (!Before)
		      index++; 
		}
		if (nSaved == 0)
		{
		   SimpleDialog oDlg(IDD_INFO_DIALOG,this,"Paste buffer is empty !");
		   break;		    
		}
		char line[133];
		char title[31];
		for (int j=0; j<nSaved; j++)
		{
		   for (int i=nlines; i>index; i--)
		   {
		      opInput->SetTitle(opInput->GetTitle(i-1),i);
		      opInput->SetLine(opInput->GetLine(i-1,0),i);
		   }
		   nlines++;
		   strcpy(line,SavedLines[j]);
		   typecode = opInput->MakeTitle(line,title,FALSE,FALSE);
		   if (UseTitle)
			SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
					(WPARAM)index, (LPARAM)title);
		   else
			SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
					(WPARAM)index, (LPARAM)&line[0]);
		   opInput->SetTitle(title,index);
		   opInput->SetLine(line,index);
		   index++;
		}
		SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)FALSE,MAKELPARAM(-1,0));
		SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)TRUE,MAKELPARAM(index-1,0));
	   }
	   else
		SimpleDialog oDlg(IDD_INFO_DIALOG,this,"No Element Selected !");
	}
	break;
      case IDC_BEFORE:
	opBefore->SetState(1);
	opAfter->SetState(0);
	Before = TRUE;
	break;
      case IDC_AFTER:
	opBefore->SetState(0);
	opAfter->SetState(1);
	Before = FALSE;
	break;
      case IDC_BEAM:
      case IDC_EDGE:
      case IDC_DRIFT:
      case IDC_BEND:
      case IDC_QUAD:
      case IDC_SLIT:
      case IDC_SHIFT:
      case IDC_ALIGN:
      case IDC_REPEAT:
      case IDC_FIT:
      case IDC_ACCEL:
      case IDC_CORREL:
      case IDC_IO:
      case IDC_MATRIX:
      case IDC_UNITS:
      case IDC_SPECIAL:
      case IDC_SECOND:
      case IDC_SEXT:
      case IDC_SOLO:
      case IDC_ROTAT:
      case IDC_WIEN:
      case IDC_SPACE:
      case IDC_RFCAV:
      case IDC_LDREG:
      case IDC_COMBIN:
      case IDC_OCTO:
      case IDC_SEPA:
      case IDC_SOL2:
      case IDC_JUMP:
      case IDC_DEDX:
      case IDC_LIM:
      case IDC_HIST2X:
      case IDC_HIST2Y:
      case IDC_FLAG:
      case IDC_DECAY:
	{
	   char line[133], title[31];
	   int nSel;
	   int aiSelect[1000];
	   nSel = (int)SendMessage(opListBox->GetHandle(),LB_GETSELITEMS,
				  (WPARAM)(sizeof(aiSelect)/sizeof(int)),(LPARAM)aiSelect);
	   int index;
	   if (nSel > 1)
	   {
	      MessageBox(this->GetHandle(),"More than 1 line selected.",
					   "TGIE: Insert mode error",MB_OK | MB_ICONSTOP);
	      break;
	   }
	   else if (nSel == 1)
	      index = aiSelect[0];
	   else
	      index = -1;

	   int  typecode = nControlId - IDC_BEAM + 1;
	   int  nr = (int)SendMessage(opListBox->GetHandle(),LB_GETCOUNT,(WPARAM)0,(LPARAM)0);

	   if ((index >= 0) || (nr == 0))
	   {
		if (nr == 0)  // empty box
		   index = 0;
		else
		{
		   if (!Before)
		      index++; 
		}
		if ((typecode <= 30) || ((typecode>=50) && (typecode<=53)) || (typecode == 60))
		{
		    strcpy(line,opInput->GetDefaultData(typecode));
		    typecode = opInput->MakeTitle(line,title,FALSE,FALSE);
		    int ind = GetNumberOfDataItems(typecode,update,Turtle);
		    int ind2 = AdjustForDialogboxSize(typecode,update,Turtle,ind);
		    opParDialog = new ParDialog(IDD_1_DIALOG+ind2,this,line,title,update,angle,Turtle,FALSE,FALSE);
		}
		else
		{
		    sprintf(title,"Typecode %d",typecode);
		    strcpy(line,"");
		    opParDialog = new ParDialog(IDD_INFO_DIALOG,this,line,title,update,angle,Turtle,FALSE,FALSE);
		}
		if (opParDialog->GetResult())
		{
		    strcpy(line,opParDialog->GetString());
		    retcode = 1;
		    if (strlen(line) == 0)
			retcode = 0;
		}
		delete opParDialog;
		if (retcode)
		{
		   for (int i=nlines; i>index; i--)
		   {
		      opInput->SetTitle(opInput->GetTitle(i-1),i);
		      opInput->SetLine(opInput->GetLine(i-1,0),i);
		   }
		   typecode = opInput->MakeTitle(line,title,FALSE,FALSE);
		   opInput->SetTitle(title,index);
		   opInput->SetLine(line,index);
		   nlines++;

		   if (UseTitle)
			SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
					(WPARAM)index, (LPARAM)title);
		   else
			SendMessage(opListBox->GetHandle(),LB_INSERTSTRING,
					(WPARAM)index, (LPARAM)&line[0]);
		   SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)FALSE,MAKELPARAM(-1,0));
		   SendMessage(opListBox->GetHandle(),LB_SETSEL,(WPARAM)TRUE,MAKELPARAM(index,0));
		}
	   }
	   else
		SimpleDialog oDlg(IDD_INFO_DIALOG,this,"No Position Selected !");
	}
	break;
      case IDC_INFO:
	{
//	  Display done in WmTimer()
          SetTimer(this->GetHandle(),TIMER_ID4,500,NULL); // kill button focus before display
	}
      default:
	break;
   }
   return FALSE;
}

VOID TgieDialog :: Exit(VOID)
{
#ifdef WIN95
      UnhookWindowsHookEx(hHook);
      DestroyWindow(hWndTT);
#endif
      TerminateDialog();
}

#ifdef WIN95

BOOL TgieDialog :: DoCreateDialogTooltip(void) 
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
//	return FALSE; 

    AddControl(IDOK);
    AddControl(IDC_EXIT);
    AddControl(IDC_ELEM);
    AddControl(IDC_DELETE);
    AddControl(IDC_EDT);
    AddControl(IDC_NEWINPUT);
    AddControl(IDC_INSERT);
    AddControl(IDC_DAT);
    AddControl(IDC_TIT);
    AddControl(IDC_BEFORE);
    AddControl(IDC_AFTER);
    AddControl(IDC_MOVEUP);
    AddControl(IDC_MOVEDOWN);
    AddControl(IDC_COPY);
    AddControl(IDC_PASTE);
    AddControl(IDC_INFO);
    AddControl(IDC_READ);

    AddControl(IDC_BEAM);
    AddControl(IDC_EDGE);
    AddControl(IDC_DRIFT);
    AddControl(IDC_BEND);
    AddControl(IDC_QUAD);
    AddControl(IDC_SLIT);
    AddControl(IDC_SHIFT);
    AddControl(IDC_ALIGN);
    AddControl(IDC_REPEAT);
    AddControl(IDC_FIT);
    AddControl(IDC_ACCEL);
    AddControl(IDC_CORREL);
    AddControl(IDC_IO);
    AddControl(IDC_MATRIX);
    AddControl(IDC_UNITS);
    AddControl(IDC_SPECIAL);
    AddControl(IDC_SECOND);
    AddControl(IDC_SEXT);
    AddControl(IDC_SOLO);
    AddControl(IDC_ROTAT);
    AddControl(IDC_WIEN);
    AddControl(IDC_SPACE);
    AddControl(IDC_RFCAV);
    AddControl(IDC_LDREG);
    AddControl(IDC_COMBIN);
    AddControl(IDC_OCTO);
    AddControl(IDC_SEPA);
    AddControl(IDC_SOL2);
    AddControl(IDC_JUMP);
    AddControl(IDC_DEDX);
    AddControl(IDC_LIM);
    AddControl(IDC_HIST2X);
    AddControl(IDC_HIST2Y);
    AddControl(IDC_FLAG);
    AddControl(IDC_DECAY);
 
    hHook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMsgProc, 
	(HINSTANCE) NULL, GetCurrentThreadId()); 
 
    if (hHook == (HHOOK) NULL) 
	return FALSE; 
 
    return TRUE; 
} 
 
long TgieDialog :: WmNotify(LPARAM lParam) 
{ 
    static CHAR szBuf[128];
 
    if ((((LPNMHDR) lParam)->code) == TTN_NEEDTEXT)
    { 
	LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lParam; 
	int idCtrl = GetDlgCtrlID((HWND)((LPNMHDR)lParam)->idFrom);

//	modify tooltip text dynamically depending on the mode:

	if ( (!update && (idCtrl == IDC_SLIT)) ||
	     ( Turtle && (idCtrl == IDC_LIM) )
	   )
	   LoadString(Application::GetInstance(),idCtrl+10000,szBuf,sizeof(szBuf));
	else
	   LoadString(Application::GetInstance(),idCtrl,szBuf,sizeof(szBuf));
        lpttt->lpszText = szBuf;
    } 
    return 0L; 
} 

#endif 

///////////////////////////////// EOF /////////////////////////////////////////
