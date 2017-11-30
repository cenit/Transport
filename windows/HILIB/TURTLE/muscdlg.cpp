///////////////////////////////////////////////////////////////////////////////
////////////////////// MuscatDialog class implementation //////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

#define DELAY 100   // timer delay in msec
#define TIMER_ID 2  // timer identification

MuscatDialog :: MuscatDialog(int ResID, Window *opWin, LPCSTR file, LPCSTR file2)
              : ModalDialog(ResID,opWin)
{
   hPar = opWin->GetHandle();
   File = file;
   File2 = file2;
   Start();
}

MuscatDialog :: ~MuscatDialog(VOID)
{
   delete opMass;
   delete opSiga;
   delete opSige;
   delete opP0;
   delete opRho;
   delete opFrac;
   delete opLen;
   delete opZ1;
   delete opProp1;
   delete opZ2;
   delete opProp2;
   delete opZ3;
   delete opProp3;
}

BOOL MuscatDialog :: Init(VOID)
{
   CenterDialog(hPar,this->GetHandle());
   Index   = 0;
   opMass  = new StringEdit(IDC_MASS,  this);
   opSiga  = new StringEdit(IDC_SIGA,  this);
   opSige  = new StringEdit(IDC_SIGE,  this);
   opP0    = new StringEdit(IDC_P0,    this);
   opRho   = new StringEdit(IDC_RHO,   this);
   opFrac  = new StringEdit(IDC_FRAC,  this);
   opLen   = new StringEdit(IDC_LEN,   this);
   opZ1    = new StringEdit(IDC_Z1,    this);
   opProp1 = new StringEdit(IDC_PROP1, this);
   opZ2    = new StringEdit(IDC_Z2,    this);
   opProp2 = new StringEdit(IDC_PROP2, this);
   opZ3    = new StringEdit(IDC_Z3,    this);
   opProp3 = new StringEdit(IDC_PROP3, this);

   if (!Loaded(File2))
   {
     opMass->SetText("0.93827");
     opSiga->SetText("31.0");
     opSige->SetText("-1.0");
     opP0->SetText("1.206");
     opRho->SetText("8.96");
     opFrac->SetText("1.0");
     opLen->SetText("1.0");
     opZ1->SetText("29.0");
     opProp1->SetText("1.0");
     opZ2->SetText("0.0");
     opProp2->SetText("0.0");
     opZ3->SetText("0.0");
     opProp3->SetText("0.0");
   }

   SetTimer(this->GetHandle(), TIMER_ID, DELAY, NULL);

   return FALSE;
}

BOOL MuscatDialog :: WmTimer(UINT id)
{
    KillTimer(this->GetHandle(),id);

    SetFocus(GetDlgItem(this->GetHandle(), IDOK));
    SetActiveWindow(this->GetHandle());

    return FALSE;
}

long MuscatDialog :: Control( UINT nControlId, UINT )
{ 
    switch (nControlId)
    {
        case IDOK:
	{
	  Index++;
	  FILE *fh = fopen(File,"w+");
	  if (fh)
	  {
		char buff1[12],buff2[12],buff3[12],buff4[12],buff5[12];
		char buff6[12],buff7[12],buff8[12],buff9[12],buff10[12];
		char buff11[12],buff12[12],buff13[12];

	        opMass->GetText(buff1,10);
		opSiga->GetText(buff2,10);
		opSige->GetText(buff3,10);
		opP0->GetText(buff4,10);
		opRho->GetText(buff5,10);
		opFrac->GetText(buff6,10);
		opLen->GetText(buff7,10);
		opZ1->GetText(buff8,10);
		opProp1->GetText(buff9,10);
		opZ2->GetText(buff10,10);
		opProp2->GetText(buff11,10);
		opZ3->GetText(buff12,10);
		opProp3->GetText(buff13,10);

		fprintf(fh,"%d\n\r",Index);
		fprintf(fh,"%s,%s,%s\n\r",buff1,buff2,buff3);
		fprintf(fh,"%s,%s,%s\n\r",buff4,buff5,buff6);
		fprintf(fh,"%s,%s,%s,%s,%s,%s,%s\n\r",buff7,buff8,buff9,buff10,buff11,buff12,buff13);
		fclose(fh);    
	  }
          result = TRUE;
          break;
	}

        case IDCANCEL:
	{
	  FILE *fh = fopen(File,"w+");
	  if (fh)
	  {
 		fprintf(fh,"-1\n\r");
 		fprintf(fh,"\n\r");
 		fprintf(fh,"\n\r");
 		fprintf(fh,"\n\r");
 		fclose(fh);    
	  }
	  Store(File2);
          TerminateDialog();
          result = FALSE;
          break;
	}
	case IDH_MASS:
	{
	   SimpleDialog(IDD_MASS_DIALOG,this);
	   break;
	}
	case IDH_SIGA:
	case IDH_SIGE:
	{
	   SimpleDialog(IDD_SIG_DIALOG,this);
	   break;
	}
	case IDH_P0:
	{
	   SimpleDialog(IDD_P0_DIALOG,this);
	   break;
	}
	case IDH_RHO:
	{
	   SimpleDialog(IDD_RHO_DIALOG,this);
	   break;
	}
	case IDH_FRAC:
	{
	   SimpleDialog(IDD_FRAC_DIALOG,this);
	   break;
	}
	case IDH_LEN:
	{
	   SimpleDialog(IDD_LENG_DIALOG,this);
	   break;
	}
	case IDH_Z1:
	case IDH_Z2:
	case IDH_Z3:
	{
	   SimpleDialog(IDD_Z_DIALOG,this);
	   break;
	}
	case IDH_PROP1:
	case IDH_PROP2:
	case IDH_PROP3:
	{
	   SimpleDialog(IDD_PROP_DIALOG,this);
	   break;
	}
    }
    return FALSE;
}

BOOL MuscatDialog :: Loaded(LPCSTR file)
{
    char buff1[12],buff2[12],buff3[12],buff4[12],buff5[12];
    char buff6[12],buff7[12],buff8[12],buff9[12],buff10[12];
    char buff11[12],buff12[12],buff13[12];
    int  i;

    FILE *fh = fopen(file,"r");
    if (fh == 0) return FALSE;

    i=fscanf(fh,"%s %s %s %s %s %s %s %s %s %s %s %s %s",
      &buff1[0],&buff2[0],&buff3[0],&buff4[0],&buff5[0],&buff6[0],
      &buff7[0],&buff8[0],&buff9[0],&buff10[0],&buff11[0],&buff12[0],&buff13[0]);
    if (i != 13) return FALSE;
    fclose(fh);    

    opMass->SetText(buff1);
    opSiga->SetText(buff2);
    opSige->SetText(buff3);
    opP0->SetText(buff4);
    opRho->SetText(buff5);
    opFrac->SetText(buff6);
    opLen->SetText(buff7);
    opZ1->SetText(buff8);
    opProp1->SetText(buff9);
    opZ2->SetText(buff10);
    opProp2->SetText(buff11);
    opZ3->SetText(buff12);
    opProp3->SetText(buff13);

    return TRUE;
}

VOID MuscatDialog :: Store(LPCSTR file)
{
     FILE *fh = fopen(file,"w+");
     if (fh)
     {
	char buff1[12],buff2[12],buff3[12],buff4[12],buff5[12];
	char buff6[12],buff7[12],buff8[12],buff9[12],buff10[12];
	char buff11[12],buff12[12],buff13[12];

        opMass->GetText(buff1,10);
	opSiga->GetText(buff2,10);
	opSige->GetText(buff3,10);
	opP0->GetText(buff4,10);
	opRho->GetText(buff5,10);
	opFrac->GetText(buff6,10);
	opLen->GetText(buff7,10);
	opZ1->GetText(buff8,10);
	opProp1->GetText(buff9,10);
	opZ2->GetText(buff10,10);
	opProp2->GetText(buff11,10);
	opZ3->GetText(buff12,10);
	opProp3->GetText(buff13,10);

	fprintf(fh,"%s %s %s %s %s %s %s %s %s %s %s %s %s",
        buff1,buff2,buff3,buff4,buff5,buff6,buff7,buff8,buff9,buff10,buff11,buff12,buff13);
	fclose(fh);    
     }
}

BOOL MuscatDialog :: GetResult(VOID)
{
    return(result);
}
