///////////////////////////////////////////////////////////////////////////////
/////////////////////// FindDialog class implementation ///////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

FindDialog :: FindDialog(int ResID, Window *opWin, LPCSTR text, BOOL c, UINT wl)
            : ModalDialog(ResID,opWin)
{
   wLen = wl;
   hPar = opWin->GetHandle();
   SetString(text);
   wCase = c;
   Start();
}

FindDialog :: ~FindDialog(VOID)
{
    delete opCase;
    delete opEdit;
}

BOOL FindDialog :: Init(VOID)
{
   CenterDialog(hPar,this->GetHandle());

   opCase = new CheckBox(IDC_MATCH_CASE,this);
   opCase->SetState(wCase);

// transfer text to Findedit

   opEdit = new StringEdit(IDC_FIND_EDIT,this);
   opEdit->SetText(caString);
   opEdit->SetCharLimit(wLen);
   opEdit->SetCrntSelection(0,strlen(caString));
   opEdit->SetFocus();

   return FALSE;
}

long FindDialog :: Control( UINT nControlId, UINT )
{ 
    switch (nControlId)
    {   case IDOK:
          opEdit->GetText(caString,79);
          caString[wLen] = '\0';
          wCase = opCase->GetState();
          TerminateDialog();
          result = TRUE;
          break;
        case IDCANCEL:
          TerminateDialog();
          result = FALSE;
          break;
       case IDC_MATCH_CASE:
          opCase->SetState((opCase->GetState()) ? 0 : 1);
          opEdit->SetFocus();
          break;
    }
    return FALSE;
}

BOOL FindDialog :: GetResult(VOID)
{
    return(result);
}

VOID FindDialog :: SetString(LPCSTR cpString)
{
    strcpy(caString,cpString);
}

BOOL FindDialog :: GetCase( void )
{
    return wCase;
}

char *FindDialog :: GetString( void )
{
    return caString;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////// ReplaceDialog class implementation //////////////////////
///////////////////////////////////////////////////////////////////////////////

ReplaceDialog :: ReplaceDialog(int ResID, Window *opWin,
                 LPCSTR text1, LPCSTR text2,
                 BOOL c, BOOL q, UINT wl1, UINT wl2)
               : ModalDialog(ResID,opWin)
{
   wLenFind = wl1;
   wLenReplace = wl2;
   hPar = opWin->GetHandle();
   strcpy(caFind,text1);
   strcpy(caReplace,text2);
   wCase = c;
   wQuery = q;
   Start();
}

ReplaceDialog :: ~ReplaceDialog(VOID)
{
    delete opCase;
    delete opQuery;
    delete opFind;
    delete opReplace;
}

BOOL ReplaceDialog :: Init(VOID)
{
   CenterDialog(hPar,this->GetHandle());

   opCase = new CheckBox(IDC_MATCH_CASE,this);
   opCase->SetState(wCase);
   opQuery = new CheckBox(IDC_QUERY,this);
   opQuery->SetState(wQuery);

   opFind = new StringEdit(IDC_FIND_EDIT,this);
   opFind->SetText(caFind);
   opFind->SetCharLimit(wLenFind);
   opFind->SetCrntSelection(0,strlen(caFind));
   opFind->SetFocus();
   opReplace = new StringEdit(IDC_REPLACE_EDIT,this);
   opReplace->SetText(caReplace);
   opReplace->SetCharLimit(wLenReplace);
   opReplace->SetCrntSelection(0,strlen(caReplace));

   return FALSE;
}

long ReplaceDialog :: Control( UINT nControlId, UINT )
{ 
    switch (nControlId)
    {   case IDOK:
          wReplaceAll = 0;
          goto Label1;
        case IDC_REPLACE_ALL:
          wReplaceAll = 1;
Label1:   opFind->GetText(caFind,79);
          caFind[wLenFind] = '\0';
          opReplace->GetText(caReplace,79);
          caReplace[wLenReplace] = '\0';
          wCase = opCase->GetState();
          wQuery = opQuery->GetState();
          TerminateDialog();
          result = TRUE;
          break;
        case IDCANCEL:
          TerminateDialog();
          result = FALSE;
          break;
       case IDC_MATCH_CASE:
          opCase->SetState((opCase->GetState()) ? 0 : 1);
          opFind->SetFocus();
          break;
       case IDC_QUERY:
          opQuery->SetState((opQuery->GetState()) ? 0 : 1);
          opFind->SetFocus();
          break;
    }
    return FALSE;
}

BOOL ReplaceDialog :: GetResult(VOID)
{
    return(result);
}

BOOL ReplaceDialog :: GetCase( void )
{
    return wCase;
}

BOOL ReplaceDialog :: GetQuery( void )
{
    return wQuery;
}

BOOL ReplaceDialog :: GetReplaceAll( void )
{
    return wReplaceAll;
}

char *ReplaceDialog :: GetFindString( void )
{
    return caFind;
}

char *ReplaceDialog :: GetReplaceString( void )
{
    return caReplace;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////// QueryDialog class implementation //////////////////////
///////////////////////////////////////////////////////////////////////////////

QueryDialog :: QueryDialog(int ResID, Window *opParent)
             : ModelessDialog(ResID,opParent)
{
    hPar = opParent->GetHandle();
    result = -1;
    Start();
}

QueryDialog :: ~QueryDialog(VOID)
{
    TerminateDialog();
}

BOOL QueryDialog :: Init(VOID)
{
    CenterDialog(hPar,this->GetHandle());
    ::SetFocus(GetDlgItem(this->GetHandle(),IDYES));
    return FALSE;
}

long QueryDialog :: Control(UINT nControlId, UINT)
{ 
    switch (nControlId)
    {   case IDYES: result = 0; break;
        case IDNO: result = 1; break;
        case IDCANCEL: result = 2; break;
    }
    return FALSE;
}

int QueryDialog :: GetResult(VOID)
{
    return(result);
}

void QueryDialog :: Reset(VOID)
{
    result = -1;
}

///////////////////////////////////// EOF /////////////////////////////////////
