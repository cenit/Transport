///////////////////////////////////////////////////////////////////////////////
////////////////////// DisplayDialog class implementation /////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

DisplayDialog :: DisplayDialog(int ResID, Window *opParent, DisplayFile *p)
               : ModalDialog(ResID, opParent)
{
    opDispFile = p;
    hPar       = opParent->GetHandle();
    Start();
}

DisplayDialog :: ~DisplayDialog(VOID)
{
    int i;

    for (i=0; i<5; i++)
       delete opParams[i];   
    for (i=0; i<15; i++)
    {  delete opOnButton[i];
       delete opOffButton[i];
    }
}

BOOL DisplayDialog :: Init(VOID)
{
    CenterDialog(hPar, this->GetHandle());

    int  i;
    char field[8];  // [8] found with Bounds Checker

    for (i=0; i<5; i++)
    {  opParams[i] = new StringEdit(IDC_ZMIN+i,this);
       opParams[i]->SetCharLimit(6);
       sprintf(field,"%7.2f",opDispFile->GetValue(i));
       opParams[i]->SetText(field);
    }

    for (i=0; i<14; i++)
    {  
       opOnButton[i]  = new RadioButton(IDC_SPEZ_ON+i, this);
       opOffButton[i] = new RadioButton(IDC_SPEZ_OFF+i,this);
       if (opDispFile->GetValue(i+5))
          opOnButton[i]->SetState(1);
       else
          opOffButton[i]->SetState(1);
///////
//	if (i == 13)
//	{
//	char buffer[100];
//	sprintf(buffer,"%d %f ",i,opDispFile->GetValue(i+5));
//	MessageBox(NULL,buffer,"DisplayDialog::Init",MB_OK);
//	}
///////
    }
    Specials2();

    opParams[0]->SetFocus();
    opParams[0]->SetCrntSelection(0,7);
    return FALSE;
}

void DisplayDialog :: Specials2()
{
    opOnButton[14]  = new RadioButton(IDC_SPEZ_ON+14, this);
    opOffButton[14] = new RadioButton(IDC_SPEZ_OFF+14,this);

    int par1 = (int)opDispFile->GetValue(11); // rax
    int par2 = (int)opDispFile->GetValue(12); // ray
    int par3 = (int)opDispFile->GetValue(16); // csx
    int par4 = (int)opDispFile->GetValue(17); // csy
    if (par3 != 0 || par4 != 0)
    {
          opOffButton[14]->SetState(1);
          opOnButton[14]->SetState(0);
	  return;
    }
    if (par1 > 1 || par2 > 1)
    {
          opOffButton[14]->SetState(0);
          opOnButton[14]->SetState(1);
    }
    else if (par1 == 0 && par2 == 0)
    {
          opOffButton[14]->SetState(0);
          opOnButton[14]->SetState(1);
    }
    else
    {
          opOffButton[14]->SetState(1);
          opOnButton[14]->SetState(0);
    }
}

VOID DisplayDialog :: TransferParameters(VOID)
{
    int   i;
    char  field[8]; // [8] found with Bounds Checker
    float val;

    for (i=0; i<5; i++)
    {  opParams[i]->GetText(field,8);  // bug up to 02-May-02: 8 was a 7
       sscanf(field,"%f",&val);
       opDispFile->SetValue(val,i);
    }

    for (i=0; i<15; i++)
    {  if (opOnButton[i]->GetState() == 1)
         opDispFile->SetValue((float)1.0,i+5);
       else
         opDispFile->SetValue((float)0.0,i+5);
///////
//	if (i == 13)
//	{
//	char buffer[100];
//	sprintf(buffer,"%d %f ",i,opDispFile->GetValue(i+5));
//	MessageBox(NULL,buffer,"DisplayDialog::TransferParameters",MB_OK);
//	}
///////
    }
    Specials();
}


void DisplayDialog :: Specials()
{
    if (opOnButton[14]->GetState() == 1 && opOnButton[6]->GetState() == 1)
    {
         opDispFile->SetValue((float)2.0,11);
    }    
    if (opOnButton[14]->GetState() == 1 && opOnButton[7]->GetState() == 1)
    {
         opDispFile->SetValue((float)2.0,12);
    }    
    if (opOnButton[14]->GetState() == 0 && opOnButton[6]->GetState() == 1)
    {
         opDispFile->SetValue((float)1.0,11);
    }    
    if (opOnButton[14]->GetState() == 0 && opOnButton[7]->GetState() == 1)
    {
         opDispFile->SetValue((float)1.0,12);
    }    
    if (opOnButton[14]->GetState() == 1 && opOnButton[6]->GetState() == 0)
    {
         opDispFile->SetValue((float)0.0,11);
    }    
    if (opOnButton[14]->GetState() == 1 && opOnButton[7]->GetState() == 0)
    {
         opDispFile->SetValue((float)0.0,12);
    }    
}

long DisplayDialog :: Control(UINT nControlId, UINT )
{ 
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

	case IDC_CSX_OFF:
	case IDC_CSY_OFF:
	  if (opOffButton[11]->GetState() == 1 && opOffButton[12]->GetState() == 1)
	  {
	    opOffButton[14]->SetState(0);
	    opOnButton[14]->SetState(1);  // ENVEL on if CSX off and CSY off 
	  }
	  break;
	case IDC_CSX_ON:
	case IDC_CSY_ON:
	  {
	    opOffButton[14]->SetState(1);  // ENVEL off
	    opOnButton[14]->SetState(0);
	    opOffButton[4]->SetState(1);  // DISP off
	    opOnButton[4]->SetState(0);
	    opOffButton[5]->SetState(1);  // YDISP off
	    opOnButton[5]->SetState(0);
	    opOffButton[6]->SetState(1);  // RAX off
	    opOnButton[6]->SetState(0);
	    opOffButton[7]->SetState(1);   // RAY off
	    opOnButton[7]->SetState(0);
	    opOffButton[13]->SetState(1);   // BUNL off
	    opOnButton[13]->SetState(0);
	  }
	  break;
	case IDC_RAX_ON:
	case IDC_RAY_ON:
	  {
	    opOffButton[11]->SetState(1);  // CSX off
	    opOnButton[11]->SetState(0);
	    opOffButton[12]->SetState(1);  // CSY off
	    opOnButton[12]->SetState(0);
	    opOffButton[13]->SetState(1);   // BUNL off
	    opOnButton[13]->SetState(0);
	  }
	  break;
	case IDC_RAX_OFF:
	case IDC_RAY_OFF:
        case IDC_ENVEL_OFF:
	  if (opOffButton[11]->GetState() == 1 && opOffButton[12]->GetState() == 1 &&
	      opOffButton[6]->GetState() == 1 && opOffButton[7]->GetState() == 1)
	  {
	    opOffButton[14]->SetState(0);
	    opOnButton[14]->SetState(1);  // ENVEL on if RAX,RAY,CSX & CSY off
	  }
	  break;
        case IDC_ENVEL_ON:
	case IDC_DISP_ON:
	case IDC_YDISP_ON:
	  {
	    opOffButton[11]->SetState(1);  // CSX off
	    opOffButton[12]->SetState(1);
	    opOnButton[11]->SetState(0);   // CSY off
	    opOnButton[12]->SetState(0);
	  }
	  break;
	case IDC_BUNL_ON:
	  {
	    opOffButton[13]->SetState(0);  // BUNL on
	    opOnButton[13]->SetState(1);
	    opOffButton[14]->SetState(0);  // ENVEL on
	    opOnButton[14]->SetState(1);
	    opOffButton[11]->SetState(1);  // CSX off
	    opOnButton[11]->SetState(0);
	    opOffButton[12]->SetState(1);  // CSY off
	    opOnButton[12]->SetState(0);
	    opOffButton[6]->SetState(1);   // RAX off
	    opOnButton[6]->SetState(0);
	    opOffButton[7]->SetState(1);   // RAY off
	    opOnButton[7]->SetState(0);
	  }
	  break;
	case IDC_BUNL_OFF:
	  {
	    opOffButton[13]->SetState(1);   // BUNL off
	    opOnButton[13]->SetState(0);
	  }
	  break;
    }
    return FALSE;
}

BOOL DisplayDialog :: GetResult(VOID)
{
    return (result);
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////// GetDacDialog class implementation /////////////////////
///////////////////////////////////////////////////////////////////////////////

GetDacDialog :: GetDacDialog(int ResID, Window *opParent, int set)
              : ModalDialog(ResID, opParent)
{
    settings = set;
    hPar     = opParent->GetHandle();
    Start();
}


GetDacDialog :: ~GetDacDialog(VOID)
{
    int i;

    for (i=0; i<5; i++)
    {  delete opYesButton[i];
       delete opNoButton[i];
    }
}

BOOL GetDacDialog :: Init(VOID)
{
    CenterDialog(hPar, this->GetHandle());

    int index = 1;
    for (int i=0; i<5; i++)
    {  opYesButton[i] = new RadioButton(IDC_QUAD_YES+i,this);
       opNoButton[i]  = new RadioButton(IDC_QUAD_NO+i, this);
       if ((settings&index) == index)
          opYesButton[i]->SetState(1);
       else
          opNoButton[i]->SetState(1);
       index *= 2;
    }
    return FALSE;
}

VOID GetDacDialog :: TransferParameters(VOID)
{
    result = 0;
    int index  = 1;
    for (int i=0; i<5; i++)
    {  if (opYesButton[i]->GetState() == 1)
          result += index;
       index *= 2;
    }
}

long GetDacDialog :: Control(UINT nControlId, UINT )
{ 
    switch (nControlId)
    {   case IDOK:
          TransferParameters();
          TerminateDialog();
          break;
        case IDCANCEL:
          result = 0;
          TerminateDialog();
          break;
    }
    return FALSE;
}

int GetDacDialog :: GetResult(VOID)
{
    return (result);
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////// ListDialog class implementation ///////////////////////
///////////////////////////////////////////////////////////////////////////////

ListDialog :: ListDialog(int ResID, Window *opWin, char **StrList, UINT len)
            : ModalDialog(ResID,opWin)
{
   hPar         = opWin->GetHandle();
   caStringList = StrList;
   ListLength   = len;
   Item         = -1;
   opOkButton   = NULL;

   Start();
}

ListDialog :: ~ListDialog(VOID)
{
    delete opList;
    delete opOkButton;
}

BOOL ListDialog :: Init(VOID)
{
   CenterDialog(hPar,this->GetHandle());

   opList = new ListBox(IDC_LIST_BOX,this);
   opOkButton = new PushButton(IDOK,this);

   for (int i=0; i<(int)ListLength; i++)
      opList->Append(caStringList[i]);

   opOkButton->Disable();

   return FALSE;
}

long ListDialog :: Control( UINT nControlId, UINT nNotificationCode)
{ 
    switch (nControlId)
    {   
        case IDOK:
          Item = opList->GetCrntInd();   
          TerminateDialog();
          break;
        case IDCANCEL:
          Item = -1;
          TerminateDialog();
          break;
        case IDC_LIST_BOX:
          switch (nNotificationCode)
          {  case LBN_DBLCLK:
               Item = opList->GetCrntInd();   
               TerminateDialog();
               break;
             case LBN_SELCHANGE:
               opOkButton->Enable();
               break;
             default:
               break;  
          }
        default:
          break;
    }
    return FALSE;
}

int ListDialog :: GetIndex(VOID)
{
    return(Item);
}

///////////////////////////////// EOF /////////////////////////////////////////
