///////////////////////////////////////////////////////////////////////////////
/////////////////////// VariDialog class implementation ///////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

VariDialog :: VariDialog(int ResID, Window *opParent,
              int px, int py, int pwx, int pwy, float z, TransGrafFile *opf)
            : ModalDialog(ResID,opParent)
{
    Posx  = px;
    Posy  = py;
    PosWx = pwx;
    PosWy = pwy;
    opGrafFile = opf;
    OK = opGrafFile->GetElement(z,Zst,Titel,Label,Leff,B,A);
    Start();
}

VariDialog :: ~VariDialog(VOID)
{
    delete opZst;
    delete opLeff;
    delete opB;
    delete opA;
    delete opVari;
    delete opRun;
    delete opRef;
    delete opMore;
}

BOOL VariDialog :: Init(VOID)
{
    RECT rc;
    GetWindowRect(this->GetHandle(),&rc);
    int w      = rc.right - rc.left;
    int h      = rc.bottom - rc.top;
    int hoff   = 20;               // height of caption bar
    int wmax   = GetScreenWidth();
    int hmax   = GetScreenHeight();

    int xM = Posx + PosWx - w / 2;
    int yM = Posy + PosWy + hoff;
    if (xM < 0) xM = 0; 
    if (xM > wmax - w) xM = wmax - w;
    if (yM > hmax - h) yM = hmax - h;
    SetWindowPos(this->GetHandle(),HWND_TOP,xM,yM,100,100,SWP_NOSIZE);

    char tit[40];
    strcpy(tit,Titel);
    strcat(tit,Label);
    SetWindowText(this->GetHandle(),tit);

    opZst  = new StaticText(IDC_Z,this);
    opLeff = new StaticText(IDC_LEFF,this);
    opB    = new StaticText(IDC_FIELD,this);
    opA    = new StaticText(IDC_HAP,this);
    opZst->SetText(Zst);
    opLeff->SetText(Leff);
    opB->SetText(B);
    opA->SetText(A);

    opRun  = new CheckBox(IDC_NEWRUN,this);
    opRef  = new CheckBox(IDC_NEWREF,this);

    opVari = new StringEdit(IDC_VARI,this);
    opVari->SetText("");
    opVari->SetCharLimit(8);

    opVari->SetFocus();
    opVari->SetCrntSelection(0,8);

    opMore = new PushButton(IDC_MORE,this);

    result = 0;
    Field  = (float)0.0;
    opBID  = NULL;

    return FALSE;
}

VOID VariDialog :: TransferParameters(VOID)
{
    char Buf[20];

    opVari->GetText(Buf,8);
    if (strlen(Buf))
       result |= 1;
    if (opRun->GetState())
       result |= 2;
    if (opRef->GetState())
       result |= 4;

    sscanf(Buf,"%f",&Field);
}

long VariDialog :: Control(UINT nControlId, UINT )
{ 
    switch (nControlId)
    {   case IDOK:
          TransferParameters();
          TerminateDialog();
          break;
        case IDCANCEL:
          TerminateDialog();
          break;
        case IDC_MORE:
          if (OK)
          {  if (opBID)
             {  if (!opBID->IsOpen())
                {   RECT rc;
                    GetWindowRect(this->GetHandle(),&rc);
                    opBID = new BeamInfoDialog(IDD_BINFO_DIALOG,this,
                                          rc.left,rc.top,
                                          rc.right-rc.left,rc.bottom-rc.top,
                                          opGrafFile);
                }
             }
             else
             {   RECT rc;
                 GetWindowRect(this->GetHandle(),&rc);
                 opBID = new BeamInfoDialog(IDD_BINFO_DIALOG,this,
                                          rc.left,rc.top,
                                          rc.right-rc.left,rc.bottom-rc.top,
                                          opGrafFile);
             }
          }
          opVari->SetFocus();
          break;
    }
    return FALSE;
}

int VariDialog :: GetResult(VOID)
{
    return (result);
}

float VariDialog :: GetField(VOID)
{
    return Field;
}

///////////////////////////////////////////////////////////////////////////////
////////////////////// BeamInfoDialog class implementation ////////////////////
///////////////////////////////////////////////////////////////////////////////

BeamInfoDialog :: BeamInfoDialog(int ResID, Dialog *opParent,
                  int px, int py, int sx, int sy, TransGrafFile *opf)
                : ModelessDialog(ResID,opParent)
{
    Posx  = px;
    Posy  = py;
    Sizex = sx;
    Sizey = sy;
    opGrafFile = opf;
    Start();
    open = TRUE;
}

BOOL BeamInfoDialog :: Init(VOID)
{
    RECT rc;
    GetWindowRect(this->GetHandle(),&rc);
    int w      = rc.right - rc.left;
    int h      = rc.bottom - rc.top;
    int wmax   = GetScreenWidth();
    int hmax   = GetScreenHeight();

    int xM = Posx;
    int yM = Posy;
    if (xM > w)
    {  xM -= w;                   // left side
       if (yM + h > hmax)
          yM = yM + Sizey - h;
    }
    else if (xM + Sizex + w < wmax)
    {  xM += Sizex;       // right side
       if (yM + h > hmax)
          yM = yM + Sizey - h;
    }
    else if (yM > h)
       yM -= h;                   // on top
    else
       yM += Sizey;      // on bottom

    SetWindowPos(this->GetHandle(),HWND_TOP,xM,yM,100,100,SWP_NOSIZE);

    opList = new ListBox(IDC_BINFO,this);
    for (int i=0; i<23; i++)
      opList->Append(opGrafFile->GetBeamInfoElem(i));

    SystemMenu oSystemMenu(this);   /* Get system menu	      */
    oSystemMenu.Disable(SC_CLOSE);
    oSystemMenu.Append(IDM_CLOSE,"&Close");

    return FALSE;
}

BOOL BeamInfoDialog :: IsOpen(VOID)
{
    return open;
}

long BeamInfoDialog :: WmSysMenuCommand( WPARAM mid )
{
    switch (mid)
    {  case IDM_CLOSE:
         open = FALSE;
         delete this;
         break;
       default:
         break;
    }
    return FALSE;
}

BeamInfoDialog :: ~BeamInfoDialog(VOID)
{
    delete opList;
    TerminateDialog();  // important, otherwise crash
}

///////////////////////////////// EOF /////////////////////////////////////////
