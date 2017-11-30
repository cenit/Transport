/*****************************************************************************

 ----  R O T K N O B  ------ MS-Windows Class Library -----------------------
                             Module DIALOG: Dialog Editor Communication

==============================================================================

This MS-Windows library contains a dialog control class which provides a
rotation knob to enter analog values via keyboard or mouse or to select
between discrete positions.

------------------------------------------------------------------------------

This module defines the function responsible for displaying the style
dialog box through which the user can define the window style, text and ID.
And this module contains the function responsible for returning a
data structure that describes the control library. This data structure can be
used by other applications when working with the control. Finally a function
converts the binary style flags into text.

------------------------------------------------------------------------------

Copyright 1988, 1989, 1990, 1991, 1992 by Marcellus Buchheit, Germany
Internet mabu@buchheit.de, CompuServe 100020,1723, Fax +49(0)721/356238

Release 1.00 of 91-Dec-30 --- All rights reserved.

*****************************************************************************/

// include all common declarations
#include "COMMON.H"

#ifndef WIN32

static HANDLE hgmStyle; // handle to current set control style memory

// pointers to extern conversation functions between ident name and value
static LPFNSTRTOID lpfnStringToIdent;
static LPFNIDTOSTR lpfnIdentToString;

#if !defined(__CUSTCNTL_H)

/*****************************************************************************

 -------- Read special declarations for Borland's Resource Workshop ---------

*****************************************************************************/

typedef struct
 {UINT    wType;
  UINT    wWidth;
  UINT    wHeight;
  DWORD   dwStyle;
  char    szDescr[CTLDESCR];
  HBITMAP hToolBit;     // bitmap for dialog editor's toolbox
  HCURSOR hDropCurs;    // drag-drop-cursor for dialog editor's toolsbox
 }
RWCTLTYPE;

typedef struct
 {UINT      wVersion;
  UINT      wCtlTypes;
  char      szClass[CTLCLASS];
  char      szTitle[CTLTITLE];
  char      szReserved[10];
  RWCTLTYPE Type[CTLTYPES];
 }
RWCTLINFO;

typedef RWCTLINFO *  PRWCTLINFO;
typedef RWCTLINFO FAR *  LPRWCTLINFO;

#define CTLDATALENGTH 255

typedef struct
 {UINT  wX;
  UINT  wY;
  UINT  wCx;
  UINT  wCy;
  UINT  wId;
  DWORD dwStyle;
  char  szClass[CTLCLASS];
  char  szTitle[CTLTITLE];
  char  CtlDataSize;            // control data size
  char  CtlData[CTLDATALENGTH]; // control data
 } RWCTLSTYLE;

typedef HANDLE (APIENTRY *LPFNINFO)(VOID);
typedef BOOL (APIENTRY *LPFNSTYLE)(HWND,HANDLE,LPFNSTRTOID,LPFNIDTOSTR);
typedef UINT (APIENTRY *LPFNFLAGS)(DWORD,LPSTR,UINT);

typedef struct
 {LPFNINFO  fnRWInfo;       // RW Info function
  LPFNSTYLE fnRWStyle;      // RW Style function
  LPFNFLAGS fnFlags;        // Flags function
  char  szClass[CTLCLASS];
 }
RWCTLCLASS;

typedef struct
 {short       nClasses;
  RWCTLCLASS Classes[1];
 }
CTLCLASSLIST;

#endif

/*****************************************************************************
E r r o r N o M e m
==============================================================================

The error message "Not enough memory" is displayed in a message box.

Parameters
 none

Used Globals
 rkc

Return
 None

*****************************************************************************/

static VOID ErrorNoMem(VOID)

 {char szTitle[50];
  LoadString(rkc.hi,ST_TITLE,szTitle,sizeof(szTitle));
  MessageBox
   (0,"Not enough memory",szTitle,MB_ICONSTOP|MB_SYSTEMMODAL|MB_OK);
 } /* ErrorNoMem() */


/*****************************************************************************
D i s p l a y M e s s a g e
==============================================================================

The string with the specified resource index is displayed in a message box
with the information icon and the OK-button.

Parameters
 hwOwner .. contains the handle of the window which owns the message box.
            The handle may be 0.
 idString   is the index of the control string.

Used Globals
 rkc

Return
 None

*****************************************************************************/

static VOID DisplayMessage(HWND hwOwner,int idString)

 {char szMsg[128];
  char szTitle[50];
  // load string as ressource
  LoadString(rkc.hi,idString,szMsg,sizeof(szMsg));
  LoadString(rkc.hi,ST_TITLE,szTitle,sizeof(szTitle));
  // display message
  MessageBox(hwOwner,szMsg,szTitle,MB_OK|MB_ICONINFORMATION);
 } /* DisplayMessage() */


/*****************************************************************************
G e t S t y l e
==============================================================================

This function initializes the control-specific part of the dialog box with
the current style values in <hgmStyle>. If the data are not readable, an error
message box is displayed.

Parameters
 hwDbx ... is the handle of the dialog box.
 hgmStyle  is the handle of the global memory object with the CTLSTYLE
           structure.

Return
 none

*****************************************************************************/

static VOID GetStyle(HWND hwDbx,HANDLE hgmStyle)

 {LPCTLSTYLE pCtlStyle;
  DWORD ulStyle;
  if (!hgmStyle||(pCtlStyle=(VOID FAR*)GlobalLock(hgmStyle))==NULL)
   {//// style data not available: print warning, return
    DisplayMessage(hwDbx,MSG_GETINF); return;
   } /* if */
  //// transfer general class data into dialog items
  SendDlgItemMessage(hwDbx,IDCLASS,EM_LIMITTEXT,CTLTITLE,0);
  SetDlgItemText(hwDbx,IDCLASS,pCtlStyle->szClass);
  if (lpfnIdentToString)
   {// conversation function available: get string/number
    char szId[20];
    (*lpfnIdentToString)(pCtlStyle->wId,szId,sizeof(szId));
    SetDlgItemText(hwDbx,IDIDVAL,szId);
   }
  else
   {// conversation function not available: get number
    SetDlgItemInt(hwDbx,IDIDVAL,pCtlStyle->wId,TRUE);
   } /* if */
  //// analyse style, convert into dialog items
  ulStyle=pCtlStyle->dwStyle;
  CheckDlgButton(hwDbx,IDDISCRET,(ulStyle&RKS_DISCRET)!=0);
  if (ulStyle&RKS_DISCRET)
   {// discrete: set number of scale lines into field
    SetDlgItemInt(hwDbx,IDSCLLINE,(UINT)(ulStyle&RKS_SCALEMASK),FALSE);
   }
  else
   {// analogue: disable scale number field & its header text
    EnableWindow(GetDlgItem(hwDbx,IDAUTOSEL),FALSE);
    EnableWindow(GetDlgItem(hwDbx,IDSCLLINE),FALSE);
    EnableWindow(GetDlgItem(hwDbx,IDHSCLLINE),FALSE);
   } /* if */
  //// analyse window text, convert into multiple parts
   {char FAR *pszSrc=pCtlStyle->szTitle;
    char ch;
    HANDLE hm;
    char *pszBuf,*pszDest;
    // get unit string into edit field
    while (*pszSrc && *pszSrc!=';') pszSrc++;
    ch=*pszSrc; *pszSrc=0; // terminate string temporarily
    SetDlgItemText(hwDbx,IDTITLE,pCtlStyle->szTitle);
    *pszSrc=ch; // set previous character again
    if (ch!=0) pszSrc++; // to start of scale text
    // get scale text parts into multiline edit field
    hm=LocalAlloc(LMEM_MOVEABLE,260);
    //// get scale text parts from edit field
    if (!hm)
     {// memory small: error & abort
      ErrorNoMem(); goto RETURN;
     };
    pszBuf=LocalLock(hm); pszDest=pszBuf;
    while (*pszSrc!=0)
     {// split text into one or more lines
      while (*pszSrc && *pszSrc!=';') *pszDest++=*pszSrc++;
      if (*pszSrc==';') pszSrc++; // skip separator
      *pszDest++=13; *pszDest++=10; // add line terminator
     } /* while */
    *pszDest=0; // end of text
    SetDlgItemText(hwDbx,IDSCLTEXT,pszBuf);
    LocalUnlock(hm); LocalFree(hm);
   } /* block */
  //// all interpretable style data got
  RETURN:
  GlobalUnlock(hgmStyle);
 } /* GetStyle() */


/*****************************************************************************
S e t S t y l e
==============================================================================

This function sets the selected values in the dialog box into the global
memory block of <hgmStyle>. The function returns TRUE if the data is
stored correctly and FALSE if a specification error was detected. In this
case the focus is set to the wrong input field.

Parameters
 hwDbx ... is the handle of the dialog box.
 hgmStyle  is the handle of the global memory object with the CTLSTYLE
           structure.

Return
 The function returns TRUE if the data is stored correctly and FALSE if not.

*****************************************************************************/

BOOL SetStyle(HWND hwDbx,HANDLE hgmStyle)

 {LPCTLSTYLE pCtlStyle;
  DWORD ulStyle;
  UINT idErrPos;
  UINT idErrMsg;
  if (!hgmStyle||(pCtlStyle=(VOID FAR*)GlobalLock(hgmStyle))==NULL)
   {//// style data not available: print warning, return
    DisplayMessage(hwDbx,MSG_SETINF);
    return TRUE; // information is ignored
   } /* if */
  //// set dialog ident name or value
  idErrPos=IDIDVAL;
  lpfnStringToIdent=(LPFNSTRTOID)MAKELONG
   (GetProp(hwDbx,MAKEINTRESOURCE(2)),GetProp(hwDbx,MAKEINTRESOURCE(3)));
  if (lpfnStringToIdent)
   {//// convert IDIDVAL entry via verify function
    char szId[20];
    DWORD ul;  // result of verify function: lower word is error flag
    szId[0]=0; // string is empty
    GetDlgItemText(hwDbx,IDIDVAL,szId,sizeof(szId));
    ul=(*lpfnStringToIdent)(szId);
    if (!LOWORD(ul)) goto RETERR; // bad ident
    pCtlStyle->wId=HIWORD(ul); // set ident number
   }
  else
   {//// no conversation function available: assume that integer specified
    UINT id;
    BOOL fOk;
    id=GetDlgItemInt(hwDbx,IDIDVAL,&fOk,TRUE);
    if (!fOk) {idErrPos=IDIDVAL; idErrMsg=MSG_NUMVAL; goto ERRMSG;}
    pCtlStyle->wId=id; // set ident number
   } /* if */
  //// set dialog style
  ulStyle=pCtlStyle->dwStyle&~(RKS_DISCRET|RKS_AUTOSELECT|RKS_SCALEMASK);
  if (IsDlgButtonChecked(hwDbx,IDDISCRET))
   {// discrete knob: read & set scale lines
    UINT u;
    BOOL fOk;
    ulStyle|=RKS_DISCRET;
    u=GetDlgItemInt(hwDbx,IDSCLLINE,&fOk,TRUE);
    if (!fOk) {idErrMsg=MSG_NUMVAL; goto ERRMSG;}
    if (u>RKS_SCALEMASK) {idErrMsg=MSG_LINERANGE; goto ERRMSG;}
    ulStyle|=u; // set number of scale values into style
    if (IsDlgButtonChecked(hwDbx,IDAUTOSEL))
     {// discrete knob with auto selection: set flag
      ulStyle|=RKS_AUTOSELECT;
     } /* if */
   } /* if */
  pCtlStyle->dwStyle=ulStyle; // set style into control data block
  //// set window text from unit and scale text
   {char FAR *pszDest=pCtlStyle->szTitle;
    char *pszBuf,*pszSrc;
    HANDLE hm;
    HWND hwItem;
    int cch,cchBuf;
    // determine unit string
    hwItem=GetDlgItem(hwDbx,IDTITLE);
    cch=GetWindowTextLength(hwItem);
    if (cch>=CTLTITLE)
     {// unit string too long
      idErrPos=IDTITLE; idErrMsg=MSG_TITLESTRING;
      goto ERRMSG;
     } /* if */
    GetWindowText(hwItem,pszDest,CTLTITLE);
    pszDest+=cch;
    //// get scale text parts from edit field
    idErrPos=IDSCLTEXT; idErrMsg=MSG_SCALESTRING;
    hm=LocalAlloc(LMEM_MOVEABLE,260);
    if (!hm)
     {// error: memory too small
      ErrorNoMem(); goto RETURN;
     } /* if */
    pszBuf=LocalLock(hm);
    hwItem=GetDlgItem(hwDbx,IDSCLTEXT);
    cchBuf=GetWindowTextLength(hwItem);
    if (cchBuf>260) goto ERRMSG; // memory too small
    GetWindowText(hwItem,pszBuf,260);
    // convert text of edit dialog into scale
    pszSrc=pszBuf;
    while (*pszSrc!=0)
     {// include part separator determine end of part
      *pszDest++=';'; // include part separator
      while (*pszSrc>=' ' && cch<CTLTITLE)
       {// copy character of next block
        *pszDest++=*pszSrc++; cch++;
       } /* while */
      if (*pszSrc) pszSrc+=2; // skip line terminator
     } /* while */
    *pszDest=0; // set string terminator
    LocalUnlock(hm); LocalFree(hm);
   } /* block */
  //// all interpretable style data set
  GlobalUnlock(hgmStyle); return TRUE;
  ////// exception handler //////
  ERRMSG:
    DisplayMessage(hwDbx,idErrMsg);
  RETERR:
    SetFocus(GetDlgItem(hwDbx,idErrPos));
  RETURN:
    GlobalUnlock(hgmStyle);
    return FALSE;
 } /* SetStyle() */


/*****************************************************************************
R o t K n o b D l g F n
===============================================================================

### Function for Modal Dialog Box ###

This function processes any messages received by the DB_STYLE dialog box.

*****************************************************************************/

BOOL APIENTRY RotKnobDlgFn(HWND hwDbx, UINT iMsg, WPARAM uP1, LPARAM lP2)

 {// analyse message
  switch (iMsg)
   {case WM_INITDIALOG:
      // initialize property list (save conversation function for later)
      SetProp(hwDbx,MAKEINTRESOURCE(1),hgmStyle);
      SetProp(hwDbx,MAKEINTRESOURCE(2),OFFSETOF(lpfnStringToIdent));
      SetProp(hwDbx,MAKEINTRESOURCE(3),SELECTOROF(lpfnStringToIdent));
      // initialize fields of dialog box
      GetStyle(hwDbx,hgmStyle);
      return TRUE; // no focus set
    case WM_COMMAND:
      //// process commands
      switch (uP1)
       {case IDDISCRET:
         {BOOL fSet;
          if (P2HI!=BN_CLICKED) break;
          fSet=IsDlgButtonChecked(hwDbx,IDDISCRET);
          EnableWindow(GetDlgItem(hwDbx,IDAUTOSEL),fSet);
          EnableWindow(GetDlgItem(hwDbx,IDSCLLINE),fSet);
          EnableWindow(GetDlgItem(hwDbx,IDHSCLLINE),fSet);
          break;
         } /* case */
        case IDOK:
          //// set selected style data, if error => ignore OK, continue
          if (!SetStyle(hwDbx,GetProp(hwDbx,MAKEINTRESOURCE(1)))) break;
        case IDCANCEL:
          RemoveProp(hwDbx,MAKEINTRESOURCE(1));
          RemoveProp(hwDbx,MAKEINTRESOURCE(2));
          RemoveProp(hwDbx,MAKEINTRESOURCE(3));
          // terminated dialog: return TRUE if OK pressed, otherwise FALSE
          EndDialog(hwDbx,uP1==IDOK);
          break;
       } /* switch */
   } /* switch */
  return FALSE;
 } /* RotKnobDlgFn() */


/*****************************************************************************
R o t K n o b S t y l e
==============================================================================

### entry function ###

This function displays a dialog box, enabling the user to define a new
control style.

Parameters
 hw ......... is the handle of the parent window.
 hCtlStyle .. is the handle to the global memory block with the control style.
 lpfnStrToId  is a pointer to the string-to-value conversation function.
 lpfnIdToStr  is a pointer to the value-to-string conversation function.

Return
 The function returns the handle of the global memory block with the control
 style if the dialog box is normally closed. A value of 0 is returned
 if the user cancelled the operation.

*****************************************************************************/

HANDLE APIENTRY RotKnobStyle
 (HWND hw,HANDLE hgmCtlStyle,LPFNSTRTOID lpfnStrToId,LPFNIDTOSTR lpfnIdToStr)

 {FARPROC lpDbfInst;
  BOOL fResult;
  if (!hgmCtlStyle) return 0; // error
  // set global variables
  lpfnStringToIdent=lpfnStrToId;
  lpfnIdentToString=lpfnIdToStr;
  hgmStyle=hgmCtlStyle;
  // display dialog box
  lpDbfInst=MakeProcInstance(RotKnobDlgFn,rkc.hi);
  fResult=DialogBox(rkc.hi,MAKEINTRESOURCE(DB_STYLE),hw,lpDbfInst);
  FreeProcInstance(lpDbfInst);
  return fResult? hgmStyle:0; // return handle if changed
 } /* RotKnobStyle() */


/*****************************************************************************
S p e c i f y I n f o
==============================================================================

### entry function ###

This function returns a handle to an information block which describes the
specified control. Included in the information returned is the suggested size
of the control, default style flags and other useful data.

Parameters
 none

Return
 The handle of a created global data block with the CTLINFO data structure
 is returned.

*****************************************************************************/

#ifdef __BORLANDC__
#pragma argsused
#endif

static VOID SpecifyInfo(LPCTLINFO pCtlInfo, UINT cbTypeStruct)

 {CTLTYPE FAR *pCtlType;
  ////// set general information structure
  pCtlInfo->wCtlTypes=1; // number of control types
  pCtlInfo->wVersion=CLASS_VERSION;
  lstrcpy(pCtlInfo->szClass,CLASS_NAME);
  LoadString(rkc.hi,ST_TITLE,pCtlInfo->szTitle,CTLTITLE);
  ////// specify class 0 type
  pCtlType=(CTLTYPE FAR*)((char FAR *)pCtlInfo->Type+0*cbTypeStruct);
  pCtlType->wType=0;
  pCtlType->wWidth=44;
  pCtlType->wHeight=44;
  pCtlType->dwStyle=CLASS_WNDSTYLE|RKS_DISCRET|5;
  LoadString(rkc.hi,ST_DESCR_0,pCtlType->szDescr,CTLDESCR);
  ////// specify class 1 type
  // pCtlType=(CTLTYPE FAR*)((char FAR *)(pCtlInfo->Type+1*cbTypeStruct);
  // pCtlType->wType=0;
  // ...
 } /* SpecifyInfo() */


/*****************************************************************************
R o t K n o b I n f o
==============================================================================

### entry function ###

This function returns a handle to an information block which describes the
specified control. Included in the information returned is the suggested size
of the control, default style flags and other useful data.

Parameters
 none

Return
 The handle of a created global data block with the CTLINFO data structure
 is returned.

*****************************************************************************/

HANDLE APIENTRY RotKnobInfo(VOID)

 {HANDLE hgmCtlInfo;
  LPCTLINFO pCtlInfo;
  // allocate memory for control information block
  hgmCtlInfo=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,(DWORD)sizeof(CTLINFO));
  if (!hgmCtlInfo) return 0; // memory too small
  // try to lock
  pCtlInfo=(VOID FAR*)GlobalLock(hgmCtlInfo);
  if (pCtlInfo==NULL)
   {// bad locking: free memory, return without data
    GlobalFree(hgmCtlInfo); return 0;
   } /* if */
  // set class specific information data into allocated buffer
  SpecifyInfo(pCtlInfo,sizeof(CTLTYPE));
  // return set information
  GlobalUnlock(hgmCtlInfo); return hgmCtlInfo; // return handle
 } /* RotKnobInfo() */


/*****************************************************************************
R o t K n o b I n f o R W
==============================================================================

### entry function ###

This function returns a handle to an information block which describes the
specified control. Included in the information returned is the suggested size
of the control, default style flags and other useful data.

Parameters
 none

Return
 The handle of a created global data block with the RWCTLINFO data structure
 is returned.

Comments
 This function is the variant of the function RotKnobInfo() for the Borland
 Resource Workshop.

*****************************************************************************/

HANDLE APIENTRY RotKnobInfoRW(VOID)

 {HANDLE hgmCtlInfo;
  RWCTLINFO FAR *pCtlInfo;
  UINT i;
  // allocate memory for control information block
  hgmCtlInfo=GlobalAlloc
   (GMEM_MOVEABLE|GMEM_ZEROINIT,(DWORD)sizeof(RWCTLINFO));
  if (!hgmCtlInfo) return 0; // memory too small
  // try to lock
  pCtlInfo=(VOID FAR*)GlobalLock(hgmCtlInfo);
  if (pCtlInfo==NULL)
   {// bad locking: free memory, return without data
    GlobalFree(hgmCtlInfo); return 0;
   } /* if */
  // set class specific information data into allocated buffer
  SpecifyInfo((LPCTLINFO)pCtlInfo,sizeof(RWCTLTYPE));
  // add toolbox bitmap and drag&drop-cursor to allocated types
  for (i=0;i<pCtlInfo->wCtlTypes;i++)
   {pCtlInfo->Type[i].hToolBit=
      LoadBitmap(rkc.hi,MAKEINTRESOURCE(BMP_TLBOX_0+i));
    pCtlInfo->Type[i].hDropCurs=
      LoadCursor(rkc.hi,MAKEINTRESOURCE(CRS_DRGDRP_0+i));
   } /* for */
  // return set information
  GlobalUnlock(hgmCtlInfo); return hgmCtlInfo; // return handle
 } /* RotKnobInfoRW() */


/*****************************************************************************
C o p y L i m i t S t r i n g
==============================================================================

This function copies a string from the specified source to the destination
buffer including the terminating 0-character. Never more than the specified
number of bytes (including the trailing 0) are stored into the destination
buffer. If the source string is too long to fit completely into this buffer
it is cut. The trailing 0 is always stored.

Parameters
 pszDest  is the long address of the destination buffer.
 pszSrc   is the long address of the source buffer.
 cbDest   is the length of the destination buffer in range 1..65519.

Return
 The function returns the number of characters in the stored string without
 the trailing 0.

Comment
 The function doesn't support huge pointers.

*****************************************************************************/

static UINT CopyLimitString(LPSTR pszDest, LPSTR pszSrc, UINT cbDest)

 {UINT cbCopy=lstrlen(pszSrc);
  if (cbCopy<cbDest)
   {// standard string copy
    lstrcpy(pszDest,pszSrc);
   }
  else
   {// reduced string copy
    UINT cb;
    cbCopy=cbDest-1; // reserve one byte for trailing 0
    for (cb=cbCopy;cb>0;cb--) *pszDest++=*pszSrc++;
    *pszDest=0; // store trailing 0
   } /* if */
  return cbCopy; // return number stored characters
 } /* CopyLimitString() */


/*****************************************************************************
R o t K n o b F l a g s
==============================================================================

### entry function ###

This function converts the specified style flags into a text string that
corresponds to that used in the control header.

Parameters
 uStyle .. class specific control style code.
 pszStyle  points to a buffer which contains the style string after return.
 cchStyle  contains the length of the <pszStyle> buffer.

Return
 The function returns the number of characters in the created string.

*****************************************************************************/

UINT APIENTRY RotKnobFlags(UINT uStyle, LPSTR pszStyle, UINT cchStyle)

 {char sz[6];
  UINT cch,cchSum=0;
  // process supported styles
  if (uStyle&RKS_DISCRET)
   {// digital option choosed
    cch=CopyLimitString(pszStyle,"RKS_DISCRET",cchStyle);
    pszStyle+=cch; cchStyle-=cch; cchSum+=cch;
   } /* if */
  if (cchSum!=0 && cchStyle>0) {*pszStyle++='|'; cchSum++; cchStyle--;}
  if (uStyle&RKS_AUTOSELECT)
   {// digital option choosed
    cch=CopyLimitString(pszStyle,"RKS_AUTOSELECT",cchStyle);
    pszStyle+=cch; cchStyle-=cch; cchSum+=cch;
   } /* if */
  if (cchSum!=0 && cchStyle>0) {*pszStyle++='|'; cchSum++; cchStyle--;}
  wsprintf(sz,"%d",uStyle&RKS_SCALEMASK);
  cch=CopyLimitString(pszStyle,sz,cchStyle);
  cchSum+=cch;
  return cchSum;
 } /* RotKnobFlags() */


/*****************************************************************************
R o t K n o b F l a g s R W
==============================================================================

### entry function ###

This function converts the specified style flags into a text string that
corresponds to that used in the control header.

Parameters
 ulStyle   class specific control style code.
 pszStyle  points to a buffer which contains the style string after return.
 cchStyle  contains the length of the <pszStyle> buffer.

Return
 The function returns 0 if the text string is created and a non-0-value
 if the function fails.

Comments
 This function is the variant of the function RotKnobFlags() for the Borland
 Resource Workshop.

*****************************************************************************/

UINT APIENTRY RotKnobFlagsRW(DWORD ulStyle, LPSTR pszStyle, UINT cchStyle)

 {// call standard function
  RotKnobFlags((UINT)ulStyle,pszStyle,cchStyle);
  return 0; // return no error
 } /* RotKnobFlagsRW() */


/*****************************************************************************
L i s t C l a s s e s
==============================================================================

### entry function ###

This function is only needed for the Borland's Resource Toolkit. It returns
the basic structure of type CTLCLASSLIST which contains all available classes
and entry functions.

Parameters
 pszAppClass  contains the class name of the dialog editor toolkit.
 uVersion ... contains the version number of the dialog editor toolkit.
 pfnLoad .... points to a function to load project specific resources.
 pfnEdit .... points to a function to start the editor of a project specific
              resource.

Return
 The handle of an allocated and initialized global data block is returned
 which contains the CTLCLASSLIST structure.

*****************************************************************************/

#ifdef __BORLANDC__
#pragma argsused
#endif

HANDLE APIENTRY ListClasses
 (LPSTR pszAppClass, UINT uVersion, FARPROC pfnLoad, FARPROC pfnEdit)

 {HANDLE hgmCtlList;
  CTLCLASSLIST FAR *pCtlList;
  // allocate memory for control list block
  hgmCtlList=GlobalAlloc
   (GMEM_MOVEABLE|GMEM_ZEROINIT,(DWORD)sizeof(CTLCLASSLIST));
  if (!hgmCtlList) return 0; // memory too small
  // try to lock
  pCtlList=(VOID FAR*)GlobalLock(hgmCtlList);
  if (pCtlList==NULL)
   {// bad locking: free memory, return without data
    GlobalFree(hgmCtlList); return 0;
   } /* if */
  ////// set library specific list of available classes
  pCtlList->nClasses=1;
  pCtlList->Classes[0].fnRWInfo=(LPFNINFO)RotKnobInfoRW;
  pCtlList->Classes[0].fnRWStyle=(LPFNSTYLE)RotKnobStyle;
  pCtlList->Classes[0].fnFlags=(LPFNFLAGS)RotKnobFlagsRW;
  lstrcpy(pCtlList->Classes[0].szClass,CLASS_NAME);
  ////// return set information
  GlobalUnlock(hgmCtlList); return hgmCtlList; // return handle
 } /* ListClasses() */

#else

#ifdef __BORLANDC__
#pragma argsused
#endif

BOOL APIENTRY RotKnobDlgFn(HWND hwDbx, UINT iMsg, WPARAM uP1, LPARAM lP2)
{
   return FALSE;
}

#ifdef __BORLANDC__
#pragma argsused
#endif

HANDLE APIENTRY RotKnobStyle
 (HWND hw,HANDLE hgmCtlStyle, FARPROC lpfnStrToId, FARPROC lpfnIdToStr)
{
   return (HANDLE)1;
}

HANDLE APIENTRY RotKnobInfo(VOID)
{
   return (HANDLE)1;
}

HANDLE APIENTRY RotKnobInfoRW(VOID)
{
   return (HANDLE)1;
}

#ifdef __BORLANDC__
#pragma argsused
#endif

UINT APIENTRY RotKnobFlags(UINT uStyle, LPSTR pszStyle, UINT cchStyle)
{
   return (UINT)1;
}

#ifdef __BORLANDC__
#pragma argsused
#endif

UINT APIENTRY RotKnobFlagsRW(DWORD ulStyle, LPSTR pszStyle, UINT cchStyle)
{
   return (UINT)1;
}

#ifdef __BORLANDC__
#pragma argsused
#endif

HANDLE APIENTRY ListClasses
 (LPSTR pszAppClass, UINT uVersion, FARPROC pfnLoad, FARPROC pfnEdit)
{
   return (HANDLE)1;
}

#endif

/* ======================================
   ROTKNOB class library: End of DIALOG.C
   ======================================
*/
