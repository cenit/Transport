///////////////////////////////////////////////////////////////////////////////
//////////////////////// TugDialog class implementation ///////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

TugDialog :: TugDialog(int ResID, Window *opParent, int nh, LONG nor,
             TUGDATA *td)
           : ModalDialog(ResID,opParent)
{
    hPar = opParent->GetHandle();
    Initialized = FALSE;
    TugData = td;
    Nhists  = nh;
    Norays  = nor;
    Start();
}

TugDialog :: ~TugDialog(VOID)
{
    delete opText;
    delete opCont;
    delete opLego;
    delete opFront;
    delete opBack;
    delete opLin;
    delete opLog;
    delete opGrid;
    delete opNHist;
    delete opNCont;
    delete opPhi;
    delete opTet;
    delete opNumbSB;
    delete opNContSB;
    delete opPhiSB;
    delete opTetSB;
    delete opIncr;
    delete opNone;
    delete opDecr;
    delete opOK;
    delete opStat;
}

BOOL TugDialog :: Init(VOID)
{
    CenterDialog(hPar, this->GetHandle());

    char Text[80];
    opText = new StaticText(IDC_CAPTION,this);
    sprintf(Text,"All %d histograms refer to %ld rays",Nhists,Norays);    
    opText->SetText(Text);

    opCont  = new RadioButton(IDC_CONT,this);
    opLego  = new RadioButton(IDC_LEGO,this);
    opGrid  = new RadioButton(IDC_GRID,this);
    if (TugData->plotyp == CONT)
    {  opCont->SetState(1);
       opLego->SetState(0);
       opGrid->SetState(0);
    }
    else if (TugData->plotyp == LEGO)
    {  opCont->SetState(0);
       opLego->SetState(1);
       opGrid->SetState(0);
    }
    else
    {  opCont->SetState(0);
       opLego->SetState(0);
       opGrid->SetState(1);
    }

    opIncr  = new RadioButton(IDC_INCR,this);
    opNone  = new RadioButton(IDC_NONE,this);
    opDecr  = new RadioButton(IDC_DECR,this);
    if (TugData->incr == INCR)
    {  opIncr->SetState(1);
       opNone->SetState(0);
       opDecr->SetState(0);
    }
    else if (TugData->incr == NONE)
    {  opIncr->SetState(0);
       opNone->SetState(1);
       opDecr->SetState(0);
    }
    else
    {  opIncr->SetState(0);
       opNone->SetState(0);
       opDecr->SetState(1);
    }

    opFront = new RadioButton(IDC_FRONT,this);
    opBack  = new RadioButton(IDC_BACK,this);
    if (TugData->view == FRONT)
    {  opFront->SetState(1);
       opBack->SetState(0);
    }
    else
    {  opFront->SetState(0);
       opBack->SetState(1);
    }
    opLin = new RadioButton(IDC_LIN,this);
    opLog = new RadioButton(IDC_LOG,this);
    if (TugData->scltyp == LIN)
    {  opLin->SetState(1);
       opLog->SetState(0);
    }
    else
    {  opLin->SetState(0);
       opLog->SetState(1);
    }
    opNHist = new StringEdit(IDC_NUMB, this);
    opNCont = new StringEdit(IDC_NCONT,this);
    opPhi   = new StringEdit(IDC_PHI,  this);
    opTet   = new StringEdit(IDC_THET, this);
    opNHist->SetCharLimit(2);
    opNCont->SetCharLimit(2);
    opPhi->SetCharLimit(2);
    opTet->SetCharLimit(2);

    int tnh = TugData->nh;
    if (tnh >= 1000) 
    {
	statistics = TRUE;
	tnh -= 1000;
    }
    else
	statistics = FALSE;
    sprintf(szNHist,"%d",tnh);

    opNHist->SetText(szNHist);
    sprintf(szNCont,"%d",TugData->nniv);
    opNCont->SetText(szNCont);
    sprintf(szPhi,"%d",TugData->phi);
    opPhi->SetText(szPhi);
    sprintf(szTet,"%d",TugData->tet);
    opTet->SetText(szTet);

    opNumbSB  = new ScrollBar(IDC_NUMB_SC,this);
    opNContSB = new ScrollBar(IDC_NCONT_SC,this);
    opPhiSB   = new ScrollBar(IDC_PHI_SC,this);
    opTetSB   = new ScrollBar(IDC_THET_SC,this);
    opNumbSB->SetRange(1,Nhists);
    opNumbSB->SetPageSize(5);
    opNumbSB->SetThumbPos(tnh);
    opNContSB->SetRange(3,30);
    opNContSB->SetPageSize(5);
    opNContSB->SetThumbPos(TugData->nniv);
    opPhiSB->SetRange(0,90);
    opPhiSB->SetPageSize(5);
    opPhiSB->SetThumbPos(TugData->phi);
    opTetSB->SetRange(0,90);
    opTetSB->SetPageSize(5);
    opTetSB->SetThumbPos(TugData->tet);

    opStat  = new CheckBox(IDC_STAT,this);
    if (statistics)
       opStat->SetState(1);
    else
       opStat->SetState(0);

    opOK = new PushButton(IDOK,this);
    opOK->SetFocus();

    Initialized = TRUE;
    return FALSE;
}

VOID TugDialog :: TransferParameters(VOID)
{
    if (opLin->GetState() == 1)
       TugData->scltyp = LIN;
    else
       TugData->scltyp = LOG;

    if (opFront->GetState() == 1)
       TugData->view = FRONT;
    else
       TugData->view = BACK;

    if (opCont->GetState() == 1)
       TugData->plotyp = CONT;
    else if (opLego->GetState() == 1)
       TugData->plotyp = LEGO;
    else
       TugData->plotyp = GRID;

    if (opIncr->GetState() == 1)
       TugData->incr = INCR;
    else if (opNone->GetState() == 1)
       TugData->incr = NONE;
    else
       TugData->incr = DECR;

    if (opStat->GetState() == 1)
    {
       statistics = TRUE;
       if (TugData->nh < 1000)
          TugData->nh += 1000;
    }
    else
    {
       statistics = FALSE;
       if (TugData->nh >= 1000)
          TugData->nh -= 1000;
    }
}

long TugDialog :: Control( UINT nControlId, UINT )
{ 
    LightDialog *opL;
    int val;

    switch (nControlId)
    {   case IDOK:
          TransferParameters();
          result = TRUE;
          TerminateDialog();
          break;
        case IDCANCEL:
          result = FALSE;
          TerminateDialog();
          break;
        case IDC_LPARAMS:
          opL = new LightDialog(IDD_LIGHT_DIALOG, this,
                                TugData->phil,TugData->tetl);
          if (opL->GetResult())
          {  int phi, tet;
             opL->GetParams(phi,tet);
             TugData->phil = phi;
             TugData->tetl = tet;
          }
          delete opL;
          opNHist->SetFocus();
          break;
        case IDC_NUMB_SC:
          val = (int)opNumbSB->GetThumbPos();
          TugData->nh = val;
          sprintf(szNHist,"%d",TugData->nh);
          opNHist->SetText(szNHist);
          break;
        case IDC_NCONT_SC:
          val = (int)opNContSB->GetThumbPos();
          TugData->nniv = val;
          sprintf(szNCont,"%d",TugData->nniv);
          opNCont->SetText(szNCont);
          break;
        case IDC_PHI_SC:
          val = (int)opPhiSB->GetThumbPos();
          TugData->phi = val;
          sprintf(szPhi,"%d",TugData->phi);
          opPhi->SetText(szPhi);
          break;
        case IDC_THET_SC:
          val = (int)opTetSB->GetThumbPos();
          TugData->tet = val;
          sprintf(szTet,"%d",TugData->tet);
          opTet->SetText(szTet);
          break;
        case IDC_NUMB:
          if (!Initialized) return FALSE;
          opNHist->GetText(szNHist,5);
          if (strlen(szNHist))
          {  if (sscanf(szNHist,"%d",&val) == 1)
             {  if (val < 1)
                  val = 1;
                if (val > Nhists)
                  val = Nhists;
                TugData->nh = val;
                opNumbSB->SetThumbPos(val);
		if (statistics)
        	   TugData->nh += 1000;
             }
          }
          break;
        case IDC_NCONT:
          if (!Initialized) return FALSE;
          opNCont->GetText(szNCont,5);
          if (strlen(szNCont))
          {  if (sscanf(szNCont,"%d",&val) == 1)
             {  if (val < 3)
                  val = 3;
                if (val > 30)
                  val = 30;
                TugData->nniv = val;
                opNContSB->SetThumbPos(val);
             }
          }
          break;
        case IDC_PHI:
          if (!Initialized) return FALSE;
          opPhi->GetText(szPhi,5);
          if (strlen(szPhi))
          {  if (sscanf(szPhi,"%d",&val) == 1)
             {  if (val < 0)
                  val = 0;
                if (val > 90)
                  val = 90;
                TugData->phi = val;
                opPhiSB->SetThumbPos(val);
             }
          }
          break;
        case IDC_THET:
          if (!Initialized) return FALSE;
          opTet->GetText(szTet,5);
          if (strlen(szTet))
          {  if (sscanf(szTet,"%d",&val) == 1)
             {  if (val < 0)
                  val = 0;
                if (val > 90)
                  val = 90;
                TugData->tet = val;
                opTetSB->SetThumbPos(val);
             }
          }
          break;
        default:
          break;
    }
    return FALSE;
}

BOOL TugDialog :: GetResult(VOID)
{
    return(result);
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////// LightDialog class implementation //////////////////////
///////////////////////////////////////////////////////////////////////////////

LightDialog :: LightDialog(int ResID, TugDialog *opParent, int phi, int tet)
             : ModalDialog(ResID,opParent)
{
    Initialized = FALSE;
    hPar = opParent->GetHandle();
    Phi  = phi;
    Tet  = tet;
    Start();
}

LightDialog :: ~LightDialog(VOID)
{
    delete opPhi;
    delete opTet;
    delete opKnob[0];
    delete opKnob[1];
}

BOOL LightDialog :: Init(VOID)
{
    CenterDialog(hPar,this->GetHandle());

    opPhi   = new StringEdit(IDC_PHI,this);
    opTet   = new StringEdit(IDC_THET,this);
    opPhi->SetCharLimit(4);
    opTet->SetCharLimit(2);
    sprintf(szPhi,"%d",Phi);
    opPhi->SetText(szPhi);
    sprintf(szTet,"%d",Tet);
    opTet->SetText(szTet);
    
    opKnob[0] = new RotKnob(IDC_PHI_SC,this);
    KnobMax[0] = 180;
    KnobMin[0] = -180;
    KnobPage[0] = 45;
    KnobVal[0] = Phi;
    opKnob[0]->SetRange(KnobMin[0],KnobMax[0]);
    opKnob[0]->SetPos(KnobVal[0]);

    opKnob[1] = new RotKnob(IDC_THET_SC,this);
    KnobMax[1] = 90;
    KnobMin[1] = 0;
    KnobPage[1] = 10;
    KnobVal[1] = Tet;
    opKnob[1]->SetRange(KnobMin[1],KnobMax[1]);
    opKnob[1]->SetPos(KnobVal[1]);

    opKnob[0]->SetFocus();

    Initialized = TRUE;
    return FALSE;
}

long LightDialog :: Control(UINT nControlId, UINT )
{ 
    int  val;

    switch (nControlId)
    {  case IDOK:
         result = TRUE;
         TerminateDialog();
         break;
       case IDCANCEL:
         result = FALSE;
         TerminateDialog();
         break;
       case IDC_PHI_SC:
         Phi = opKnob[0]->GetPos();
         sprintf(szPhi,"%d",Phi);
         opPhi->SetText(szPhi);
         break;
       case IDC_THET_SC:
         Tet = opKnob[1]->GetPos();
         sprintf(szTet,"%d",Tet);
         opTet->SetText(szTet);
         break;
       case IDC_PHI:
         if (!Initialized) break;
         opPhi->GetText(szPhi,5);
         if (strlen(szPhi))
         {  if (sscanf(szPhi,"%d",&val) == 1)
            {  if (val < KnobMin[0])
                 val = KnobMin[0];
               if (val > KnobMax[0])
                 val = KnobMax[0];
               Phi = val;
               opKnob[0]->SetPos(val);
            }
         }
         break;
       case IDC_THET:
         if (!Initialized) break;
         opTet->GetText(szTet,5);
         if (strlen(szTet))
         {  if (sscanf(szTet,"%d",&val) == 1)
            {  if (val < KnobMin[1])
                 val = KnobMin[1];
               if (val > KnobMax[1])
                 val = KnobMax[1];
               Tet = val;
               opKnob[1]->SetPos(val);
            }
         }
         break;
       default:
         break;
    }
    return FALSE;
}

void LightDialog::GetSizes(UINT nControlId, int &LineSize, int &PageSize)
{
    switch (nControlId)
    {  case IDC_PHI_SC:
         LineSize = 1;
         PageSize = KnobPage[0];
         break;
       case IDC_THET_SC:
         LineSize = 1;
         PageSize = KnobPage[1];
         break;
       default:
         LineSize = 1;
         PageSize = 10;
         break;
    }
}

VOID LightDialog :: GetParams(int &phi, int &tet)
{
    phi = Phi;
    tet = Tet;
}

BOOL LightDialog :: GetResult(VOID)
{
    return(result);
}

///////////////////////////////// EOF /////////////////////////////////////////
