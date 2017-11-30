///////////////////////////////////////////////////////////////////////////////
//////////////////// Implementation of class StaticText ///////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

StaticText :: StaticText(int idC, Dialog *opDialog)
            : Object( IDO_STATIC_TEXT)
{
    hDlg = opDialog->GetHandle();
    idControl = idC;
    isDialogItem = TRUE;
}

StaticText :: StaticText(int idC, Window *opParent, int x, int y,
                         int w, int h, LPCSTR szText)
            : Object( IDO_STRING_EDIT)
{
    HWND hWnd = opParent->GetHandle();
    idControl = idC;

    hControl  = CreateWindow("static",szText,
                             WS_CHILD | WS_VISIBLE | SS_LEFT,
                             x,y,w,h,hWnd,(HMENU)idControl,
                             Application::GetInstance(),NULL);
    isDialogItem = FALSE;
}

StaticText :: ~StaticText( void )
{
}

void StaticText :: SetText(LPCSTR szText)
{
    if (isDialogItem)
       SetDlgItemText(hDlg,idControl,szText);
    else
    {
       SetWindowText( hControl, szText );
    }
}

void StaticText :: GetText(char *szText, UINT len)
{
    if (isDialogItem)
       GetDlgItemText(hDlg,idControl,szText,(int)len);
    else
    {
       GetWindowText( hControl, szText, (int)len );
    }
}

HWND StaticText :: GetHandle(void)
{
    if (isDialogItem)
       return GetDlgItem(hDlg,idControl);
    else
       return hControl;
}

///////////////////////////////////////////////////////////////////////////////
//////////////////// Implementation of class StringEdit ///////////////////////
///////////////////////////////////////////////////////////////////////////////

StringEdit :: StringEdit(int idC, Dialog *opDialog)
            : Object( IDO_STRING_EDIT )
{
    hDlg = opDialog->GetHandle();
    idControl = idC;
    isDialogItem = TRUE;
}

StringEdit :: StringEdit(int idC, Window *opParent, int x, int y,
                         int w, int h, LPCSTR szText)
            : Object( IDO_STRING_EDIT)
{
    HWND hWnd = opParent->GetHandle();
    idControl = idC;

    hControl  = CreateWindow("edit",szText,
                             WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                             WS_BORDER | ES_LEFT | ES_MULTILINE |
                             ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_NOHIDESEL,
                             x,y,w,h,hWnd,(HMENU)idControl,
                             Application::GetInstance(),NULL);
    isDialogItem = FALSE;
}

StringEdit :: ~StringEdit( void )
{
    if (!isDialogItem && hControl)
       DestroyWindow(hControl);
}

HWND StringEdit :: GetHandle(void)
{
    if (isDialogItem)
       return GetDlgItem(hDlg,idControl);
    else
       return hControl;
}

void StringEdit :: SetText(LPCSTR szText)
{
    if (isDialogItem)
       SetDlgItemText(hDlg,idControl,szText);
    else
    {
       SendMessage(hControl,EM_REPLACESEL,0,(LONG)szText);
    }
}

void StringEdit :: GetText(char *szText, UINT len)
{
    if (isDialogItem)
       GetDlgItemText(hDlg,idControl,szText,(int)len);
    else
       GetWindowText(hControl,szText,(int)len);
}

void StringEdit :: SetFocus( void )
{
    if (isDialogItem)
       ::SetFocus( GetDlgItem(hDlg,idControl) );
    else
       ::SetFocus( hControl );
}

void StringEdit :: SetCharLimit (UINT wLim)
{
    if (isDialogItem)
       SendDlgItemMessage(hDlg,idControl,EM_LIMITTEXT,wLim,0L);
}

void StringEdit :: SetCrntSelection(UINT Low, UINT High)
{
#ifdef WIN32
    if (isDialogItem)
       SendDlgItemMessage(hDlg,idControl,EM_SETSEL,(WPARAM)Low,(LPARAM)High);
    else
    {  SendMessage(hControl,EM_SETSEL,(WPARAM)Low,(LPARAM)High);
       SendMessage(hControl,EM_SCROLLCARET,0,0);
    }
#else
    if (isDialogItem)
       SendDlgItemMessage(hDlg,idControl,EM_SETSEL,0,MAKELONG(Low,High));
    else
       SendMessage(hControl,EM_SETSEL,0,MAKELONG(Low,High));
#endif
}

void StringEdit :: GetCrntSelection(int &start, int &end)
{
#ifdef WIN32
    int low, hi;
    if (isDialogItem)
       SendDlgItemMessage(hDlg,idControl,EM_GETSEL,(WPARAM)&low,(LPARAM)&hi);
    else
       SendMessage(hControl,EM_GETSEL,(WPARAM)&low,(LPARAM)&hi);
    start = low;
    end   = hi;
#else
    DWORD val;
    if (isDialogItem)
       val = (DWORD)SendDlgItemMessage(hDlg,idControl,EM_GETSEL,0,0L);
    else
       val = (DWORD)SendMessage(hControl,EM_GETSEL,0,0L);
    start = LOWORD(val);
    end   = HIWORD(val);
#endif
}

void StringEdit :: SetSize(int w, int h)
{
    if (!isDialogItem)
       MoveWindow(hControl, 0, 0, w, h, TRUE);
}

void StringEdit :: SetFont(Font *opFont)
{
   if (!isDialogItem)
      SendMessage(hControl,WM_SETFONT,(WPARAM)opFont->GetHandle(),TRUE);
}

UINT StringEdit :: GetLineCount(void)
{
   if (!isDialogItem)
      return((UINT)SendMessage(hControl,EM_GETLINECOUNT,0,0L));
   else
      return 1;
}

UINT StringEdit :: GetSize(void)
{
#ifndef WIN95
   HLOCAL hMem = (HLOCAL) SendMessage(hControl, EM_GETHANDLE, 0, 0L);
   return (LocalSize(hMem));
#else
   UINT LineNr, size;

   LineNr = (UINT)SendMessage(hControl,EM_GETLINECOUNT,0,0L) - 1;
   size   = (UINT)SendMessage(hControl,EM_LINEINDEX,(WPARAM)LineNr,0L);
   return size;
#endif
}

UINT StringEdit :: GetTextLine(UINT wInd, char *cpBuf, UINT wLen)
{
    UINT wCnt = 0;

    if (hControl)
    {
	*((UINT *) cpBuf) = wLen;
	wCnt = (UINT) SendMessage(hControl,EM_GETLINE,wInd,(LONG) cpBuf);

	if (wCnt < wLen)
	    *(cpBuf + wCnt) = '\0';
	else
	    *(cpBuf + wLen - 1) = '\0';
    }
    return(wCnt);
}

#ifndef WIN95

void StringEdit :: ReadFile(LPCSTR FileName)
{
   if (isDialogItem)
      return;

   HFILE hFile;
   OFSTRUCT ofs;
   HLOCAL hOldMem, hNewMem;
   PSTR pBuf;
   long cbRead, BuffSize;

   hFile = OpenFile( FileName, &ofs , OF_READ ) ;
   if (hFile != HFILE_ERROR)
   {
//    Get FileSize

      long FileLength = _llseek(hFile,0L,2);
#ifndef WIN32
      if (FileLength > 50000)
      {
         SendMessage(hControl,EM_REPLACESEL,0,
            (LONG)"not enough memory in local heap -- file too big\r\n");
         _lclose(hFile);
         return;
      }
#endif
      BuffSize = FileLength + 512;
      _llseek(hFile,0L,0);

//    Get the handle to the old memory and free it

      hOldMem = (HLOCAL) SendMessage(hControl,EM_GETHANDLE, 0, 0L);
      LocalFree(hOldMem);

//    Allocate new memory and read the file into it.

      hNewMem = LocalAlloc(LMEM_MOVEABLE, (UINT)BuffSize);
      if ( hNewMem && ((long)LocalSize(hNewMem) >= BuffSize) )
      {
         pBuf = (PSTR)LocalLock(hNewMem);
         cbRead = _lread(hFile, pBuf, (UINT)BuffSize);
         pBuf[cbRead] = '\0';  // terminating null character

//       Adjust the buffer for the amount actually read in.

         LocalReAlloc(hNewMem, (UINT)(cbRead + 1), 0);
         LocalUnlock(hNewMem);
      }
      else
      {  
         if (hNewMem)
            LocalFree(hNewMem);
         hNewMem = LocalAlloc(LMEM_MOVEABLE, 512);
         pBuf = (PSTR)LocalLock(hNewMem);
         strcpy(pBuf,"not enough memory in local heap -- file too big\r\n");
         LocalUnlock(hNewMem);
      }

//    Set the handle to the new buffer.

      SendMessage(hControl,EM_SETHANDLE, (WPARAM)hNewMem, 0L);

      _lclose(hFile);
   }
}

#else

void StringEdit :: ReadFile(LPCSTR FileName)
{
   if (isDialogItem)
      return;

   HFILE hFile;
   OFSTRUCT ofs;
   char *pBuf;
   long cbRead, BuffSize;

   hFile = OpenFile( FileName, &ofs , OF_READ ) ;
   if (hFile != HFILE_ERROR)
   {
//    Get FileSize

      long FileLength = _llseek(hFile,0L,2);
      BuffSize = FileLength + 512;
      _llseek(hFile,0L,0);

//    Allocate new memory and read the file into it.

      HGLOBAL hMem = GlobalAlloc(LMEM_MOVEABLE, BuffSize+1);
      pBuf = (LPSTR)GlobalLock(hMem);
      if (pBuf)
      {
         cbRead = _lread(hFile, pBuf, (UINT)BuffSize);
         pBuf[cbRead] = '\0';  // terminating null character
         LRESULT r = SendMessage(hControl,WM_SETTEXT,0,(LONG)pBuf);
         if (r == 0)
            SetText("not enough memory -- file too big\r\n");
      }
      else
      {  
         SetText("not enough memory -- file too big\r\n");
      }
      if (hMem)
      {
         GlobalUnlock(hMem);
         GlobalFree(hMem);
      }
      if (hFile)
         _lclose(hFile);
   }
   else
   {  
       SetText("File Error\r\n");
   }
}
#endif

#ifdef WIN95

HGLOBAL StringEdit :: GetEditBuffer(void)
{
   UINT cbWrite, wCnt;
   HGLOBAL hMem;
   LPSTR pBuf;

   cbWrite = GetSize();
   hMem = GlobalAlloc(LMEM_MOVEABLE, cbWrite+512);
   pBuf = (LPSTR)GlobalLock(hMem);

   UINT LineNr = (UINT)SendMessage(hControl,EM_GETLINECOUNT,0,0L) - 1;
   for (UINT i=0; i<LineNr; i++)
   {
	*((UINT *) pBuf) = 200;
	wCnt = (UINT) SendMessage(hControl,EM_GETLINE,i,(LONG)pBuf);
        pBuf = (LPSTR)((long)pBuf + (long)wCnt);
        *pBuf++ = '\r';
        *pBuf++ = '\n';
   }
   *pBuf='\0';
   
   GlobalUnlock(hMem);
   
   return hMem;
}

void StringEdit :: WriteFile(LPCSTR FileName)
{
   if (isDialogItem)
      return;

   OFSTRUCT ofs;

   HGLOBAL hMem = GetEditBuffer();
   LPSTR pBuf   = (LPSTR)GlobalLock(hMem);
   UINT cbWrite = GetSize();

   HFILE hFile  = OpenFile( FileName, &ofs , OF_CREATE ) ;
   _lwrite(hFile, pBuf, cbWrite);
   _lclose(hFile);

   GlobalUnlock(hMem);
   GlobalFree(hMem);
}
#else

void StringEdit :: WriteFile(LPCSTR FileName)
{
   if (isDialogItem)
      return;

   HFILE hFile;
   OFSTRUCT ofs;
   HLOCAL hMem;
   PSTR pBuf, pSave;
   UINT cbWrite;

   hMem = (HLOCAL) SendMessage(hControl, EM_GETHANDLE, 0, 0L);
   cbWrite = LocalSize(hMem);
   pSave = pBuf = (PSTR)LocalLock(hMem);
   for (UINT i=0; i<cbWrite; i++, pBuf++)
       if (!*pBuf)  // find terminating 0 char
           break;
   cbWrite = i;     // real file length
   pBuf = pSave;

   hFile = OpenFile( FileName, &ofs , OF_CREATE ) ;
   _lwrite(hFile, pBuf, cbWrite);
   _lclose(hFile);
   LocalUnlock(hMem);
}
#endif

#ifndef WIN95

void StringEdit :: ReadMemory(HLOCAL hMem, int)
{
   if (isDialogItem)
      return;

// Get the handle to the old memory and free it.

   HLOCAL hOldMem = (HLOCAL) SendMessage(hControl,EM_GETHANDLE, 0, 0L);
   LocalFree(hOldMem);

// Set the handle to the new buffer.

   SendMessage(hControl,EM_SETHANDLE, (WPARAM)hMem, 0L);
}

#else

void StringEdit :: ReadMemory(HLOCAL hMem, int)
{
   if (isDialogItem)
      return;

   PSTR pBuf = (PSTR)LocalLock(hMem);
   long leng = strlen(pBuf);
   if (leng > 50000L) leng = 50000L;
loop:
   LRESULT r = SendMessage(hControl,WM_SETTEXT,0,(LONG)pBuf);
   if (r == 0) // too large to display
   {
       leng -= 2000L;
       *((PSTR)((long)pBuf + leng)) = '\0';
       goto loop;
   }
   LocalUnlock(hMem);
}

#endif

///////////////////////////////////////////////////////////////////////////////
//////////////////// Implementation of class PushButton ///////////////////////
///////////////////////////////////////////////////////////////////////////////

PushButton :: PushButton(int idC, Dialog *opDialog)
            : Object( IDO_PUSH_BUTTON)
{
    hDlg = opDialog->GetHandle();
    idControl = idC;
    isDialogItem = TRUE;
}

PushButton :: PushButton(int idC, Window *opParent, int x, int y,
                         int w, int h, LPCSTR szText)
            : Object( IDO_PUSH_BUTTON)
{
    HWND hWnd = opParent->GetHandle();
    idControl = idC;
    hControl  = CreateWindow("button",szText,
                             WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                             x,y,w,h,hWnd,(HMENU)idControl,
                             Application::GetInstance(),NULL);
    isDialogItem = FALSE;
}

PushButton :: ~PushButton( void )
{
    if (!isDialogItem && hControl)
       DestroyWindow(hControl);
}

void PushButton :: Enable(void)
{
    if (isDialogItem)
       EnableWindow( GetDlgItem(hDlg,idControl), TRUE );
    else
       EnableWindow( hControl, TRUE );
}

void PushButton :: Disable(void)
{
    if (isDialogItem)
       EnableWindow( GetDlgItem(hDlg,idControl), FALSE );
    else
       EnableWindow( hControl, FALSE );
}

HWND PushButton :: GetHandle(void)
{
    if (isDialogItem)
       return GetDlgItem(hDlg,idControl);
    else
       return hControl;
}

void PushButton :: SetFocus( void )
{
    if (isDialogItem)
       ::SetFocus( GetDlgItem(hDlg,idControl) );
    else
       ::SetFocus( hControl );
}

void PushButton :: SetText(LPCSTR szText)
{
    if (isDialogItem)
       SetDlgItemText(hDlg,idControl,szText);
    else
       SetWindowText( hControl, szText );
}

void PushButton :: GetText(char *szText, UINT len)
{
    if (isDialogItem)
       GetDlgItemText(hDlg,idControl,szText,(int)len);
    else
       GetWindowText(hControl,szText,(int)len);
}

///////////////////////////////////////////////////////////////////////////////
//////////////////// Implementation of class ListBox ///////////////////////
///////////////////////////////////////////////////////////////////////////////

ListBox :: ListBox(int idC, Dialog *opDialog)
         : Object( IDO_LIST_BOX)
{
    hDlg = opDialog->GetHandle();
    idControl = idC;
    isDialogItem = TRUE;
}


ListBox :: ListBox(int idC, Window *opParent, int x, int y,
                         int w, int h, LPCSTR szText)
         : Object( IDO_STRING_EDIT)
{
    HWND hWnd = opParent->GetHandle();
    idControl = idC;

    hControl  = CreateWindow("listbox",szText,
                             WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                             WS_BORDER | LBS_NOINTEGRALHEIGHT,
                             x,y,w,h,hWnd,(HMENU)idControl,
                             Application::GetInstance(),NULL);
    isDialogItem = FALSE;
}

ListBox :: ~ListBox( void )
{
}

HWND ListBox :: GetHandle(void)
{
    if (isDialogItem)
       return GetDlgItem(hDlg,idControl);
    else
       return hControl;
}

void ListBox :: Append(LPCSTR szText)
{
    if (isDialogItem)
       SendDlgItemMessage(hDlg, idControl, LB_ADDSTRING, 0, (LONG)szText);
    else
       SendMessage(hControl, LB_ADDSTRING, 0, (LONG)szText);
}

UINT ListBox :: GetCrntInd(void)
{
    if (isDialogItem)
       return (UINT)SendDlgItemMessage(hDlg, idControl, LB_GETCURSEL, 0, 0L);
    else
       return 0;
}

void ListBox :: GetCrntItem(LPSTR szText)
{
    if (isDialogItem)
    {
       WPARAM id = (WPARAM)SendDlgItemMessage(hDlg, idControl, LB_GETCURSEL, 0, 0L);
       SendDlgItemMessage(hDlg, idControl, LB_GETTEXT, id, (LPARAM)szText);
    }
}

void ListBox :: SetFont(Font *opFont)
{
   if (!isDialogItem)
      SendMessage(hControl,WM_SETFONT,(WPARAM)opFont->GetHandle(),TRUE);
}

void ListBox :: SetFocus( void )
{
    if (isDialogItem)
       ::SetFocus( GetDlgItem(hDlg,idControl) );
    else
       ::SetFocus( hControl );
}

///////////////////////////////////////////////////////////////////////////////
//////////////////// Implementation of class ScrollBar ////////////////////////
///////////////////////////////////////////////////////////////////////////////

ScrollBar :: ScrollBar(int idC, Dialog *opDialog)
           : Object( IDO_SCROLL_BAR)
{
    hDlg = opDialog->GetHandle();
    idControl = idC;
    PageSize  = 10;
    LineSize  = 1;
}

ScrollBar :: ~ScrollBar( void )
{
}

void ScrollBar :: SetRange(int low, int hi)
{
   SetScrollRange( GetDlgItem(hDlg,idControl), SB_CTL, low, hi, TRUE);
}

void ScrollBar :: SetThumbPos(int pos)
{
   SetScrollPos( GetDlgItem(hDlg,idControl), SB_CTL, pos, TRUE);
}

int ScrollBar :: GetThumbPos(void)
{
   return GetScrollPos( GetDlgItem(hDlg,idControl), SB_CTL);
}

///////////////////////////////////////////////////////////////////////////////
//////////////////// Implementation of class RadioButton //////////////////////
///////////////////////////////////////////////////////////////////////////////

RadioButton :: RadioButton(int idC, Dialog *opDialog)
            : Object( IDO_RADIO_BUTTON)
{
    hDlg = opDialog->GetHandle();
    idControl = idC;
}

RadioButton :: ~RadioButton( void )
{
}

void RadioButton :: SetState(UINT wState)
{
   SendDlgItemMessage(hDlg,idControl,BM_SETCHECK,(WPARAM)wState,0L);
}

UINT RadioButton :: GetState(void)
{
   return (UINT)SendDlgItemMessage(hDlg,idControl,BM_GETCHECK,0,0L);
}

///////////////////////////////////////////////////////////////////////////////
////////////////////// Implementation of class CheckBox ///////////////////////
///////////////////////////////////////////////////////////////////////////////

CheckBox :: CheckBox(int idC, Dialog *opDialog)
          : Object( IDO_CHECK_BOX)
{
    hDlg = opDialog->GetHandle();
    idControl = idC;
}

CheckBox :: ~CheckBox( void )
{
}

void CheckBox :: SetState(UINT wState)
{
   SendDlgItemMessage(hDlg,idControl,BM_SETCHECK,(WPARAM)wState,0L);
}

UINT CheckBox :: GetState(void)
{
   return (UINT)SendDlgItemMessage(hDlg,idControl,BM_GETCHECK,0,0L);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////// Implementation of class ComboBox ////////////////////////
///////////////////////////////////////////////////////////////////////////////

ComboBox :: ComboBox(int idC, Dialog *opDialog)
          : Object( IDO_COMBO_BOX )
{
    hDlg = opDialog->GetHandle();
    idControl = idC;
}

ComboBox :: ~ComboBox( void )
{
}

void ComboBox :: SetText(LPCSTR szText)
{
    SetDlgItemText(hDlg,idControl,szText);
}

void ComboBox :: GetText(char *szText, UINT len)
{
    GetDlgItemText(hDlg,idControl,szText,(int)len);
}

void ComboBox :: SetFocus( void )
{
    ::SetFocus( GetDlgItem(hDlg,idControl) );
}

void ComboBox :: SetCharLimit (UINT wLim)
{
    SendDlgItemMessage(hDlg,idControl,CB_LIMITTEXT,wLim,0L);
}

void ComboBox :: SetCrntSelection(UINT Low, UINT High)
{
#ifdef WIN32
    SendDlgItemMessage(hDlg,idControl,CB_SETEDITSEL,(WPARAM)Low,(LPARAM)High);
#else
    SendDlgItemMessage(hDlg,idControl,CB_SETEDITSEL,0,MAKELONG(Low,High));
#endif
}

void ComboBox :: GetCrntSelection(int &start, int &end)
{
#ifdef WIN32
    int low, hi;
    SendDlgItemMessage(hDlg,idControl,CB_GETEDITSEL,(WPARAM)&low,(LPARAM)&hi);
    start = low;
    end   = hi;
#else
    DWORD val;
    val = (DWORD)SendDlgItemMessage(hDlg,idControl,CB_GETEDITSEL,0,0L);
    start = LOWORD(val);
    end   = HIWORD(val);
#endif
}

UINT ComboBox :: GetLineCount(void)
{
    return((UINT)SendDlgItemMessage(hDlg,idControl,CB_GETCOUNT,0,0L));
}

void ComboBox :: Append(char *szText)
{
    SendDlgItemMessage(hDlg,idControl,CB_ADDSTRING,0,(LPARAM)szText);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////// Implementation of class AmpMeter ////////////////////////
///////////////////////////////////////////////////////////////////////////////

AmpMeter :: AmpMeter(int idC, Dialog *opDialog)
          : Object( IDO_AMPERE_METER)
{
    hDlg = opDialog->GetHandle();
    idControl = idC;
}

AmpMeter :: ~AmpMeter( void )
{
}

void AmpMeter :: SetText(LPCSTR szText)
{
   SetDlgItemText(hDlg,idControl,szText);
}

void AmpMeter :: SetValue(int Value)
{
   SendDlgItemMessage(hDlg,idControl,AMM_SETVALUE,(WPARAM)Value,0L);
}

void AmpMeter :: SetRange(int low, int hi)
{
   SendDlgItemMessage(hDlg,idControl,AMM_SETRANGE,0,MAKELONG(low,hi));
}

///////////////////////////////////////////////////////////////////////////////
////////////////////// Implementation of class RotKnob ////////////////////////
///////////////////////////////////////////////////////////////////////////////

RotKnob :: RotKnob(int idC, Dialog *opDialog)
         : Object( IDO_ROTKNOB)
{
    hDlg = opDialog->GetHandle();
    idControl = idC;
}

RotKnob :: ~RotKnob( void )
{
}

void RotKnob :: SetFocus( void )
{
    ::SetFocus( GetDlgItem(hDlg,idControl) );
}

void RotKnob :: SetText(LPCSTR szText)
{
    SetDlgItemText(hDlg,idControl,szText);
}

void RotKnob :: GetText(char *szText, UINT len)
{
    GetDlgItemText(hDlg,idControl,szText,(int)len);
}

void RotKnob :: SetRange(int low, int hi)
{
   SendDlgItemMessage(hDlg,idControl,RKM_SETRANGE,0,MAKELONG(low,hi));
}

void RotKnob :: SetPos(int pos)
{
   SendDlgItemMessage(hDlg,idControl,RKM_SETPOS,(WPARAM)pos,0L);
}

int RotKnob :: GetPos( void )
{
   return (int)SendDlgItemMessage(hDlg,idControl,RKM_GETPOS,0,0L);
}

///////////////////////////////////////////////////////////////////////////////
//////////////////// Implementation of class ColorButton //////////////////////
///////////////////////////////////////////////////////////////////////////////

ColorButton :: ColorButton(int idC, Dialog *opDialog)
             : Object( IDO_COLOR_BUTTON)
{
    hDlg = opDialog->GetHandle();
    idControl = idC;
}

ColorButton :: ~ColorButton( void )
{
}

void ColorButton :: Enable(void)
{
    EnableWindow( GetDlgItem(hDlg,idControl), TRUE );
}

void ColorButton :: Disable(void)
{
    EnableWindow( GetDlgItem(hDlg,idControl), FALSE );
}

void ColorButton :: SetFocus( void )
{
    ::SetFocus( GetDlgItem(hDlg,idControl) );
}

void ColorButton :: SetText(LPCSTR szText)
{
    SetDlgItemText(hDlg,idControl,szText);
    InvalidateRect(GetDlgItem(hDlg,idControl),NULL,FALSE);
}

void ColorButton :: GetText(char *szText, UINT len)
{
    GetDlgItemText(hDlg,idControl,szText,(int)len);
}

void ColorButton :: SetTxtColor(int r, int g, int b)
{
    SendDlgItemMessage(hDlg,idControl,CRBM_SETTEXTCOLOR, TRUE, RGB(r,g,b));
}

void ColorButton :: ResetTxtColor( void )
{
    SendDlgItemMessage(hDlg,idControl,CRBM_SETTEXTCOLOR, FALSE, 0L);
    InvalidateRect(GetDlgItem(hDlg,idControl),NULL,FALSE);
}

void ColorButton :: SetNewBrushes(Brush *opFace,Brush *opShadow,Brush *opHilite)
{
    SendDlgItemMessage(hDlg,idControl,CRBM_SETFACEBRUSH,
                      (WPARAM)opFace->GetHandle(), 0L);
    SendDlgItemMessage(hDlg,idControl,CRBM_SETSHADOWBRUSH,
                      (WPARAM)opShadow->GetHandle(), 0L);
    SendDlgItemMessage(hDlg,idControl,CRBM_SETHILITEBRUSH,
                      (WPARAM)opHilite->GetHandle(), 0L);
}

void ColorButton :: ResetBrushes( void )
{
    SendDlgItemMessage(hDlg,idControl,CRBM_SETFACEBRUSH, 0, 0L);
    SendDlgItemMessage(hDlg,idControl,CRBM_SETSHADOWBRUSH, 0, 0L);
    SendDlgItemMessage(hDlg,idControl,CRBM_SETHILITEBRUSH, 0, 0L);
    InvalidateRect(GetDlgItem(hDlg,idControl),NULL,FALSE);
}

HWND ColorButton :: GetHandle(void)
{
    return GetDlgItem(hDlg,idControl);
}

////////////////////////////////// EOF ////////////////////////////////////////

