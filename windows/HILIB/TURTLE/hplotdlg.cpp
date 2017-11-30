///////////////////////////////////////////////////////////////////////////////
////////////////////// HplotDialog class implementation ///////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

#define DELAY 100   // timer delay in msec
#define TIMER_ID 2  // timer identification

HplotDialog :: HplotDialog(int ResID, Window *opWin, LPCSTR file, int nh, int nr )
             : ModalDialog(ResID,opWin)
{
   hPar = opWin->GetHandle();
   Initialized = FALSE;
   File = file;
   Nhists = nh;
   Norays = nr;
   Start();
}

HplotDialog :: ~HplotDialog(VOID)
{
    delete opText;
    delete opUTI;
    delete opLin;
    delete opLog;
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
    delete opCol;
    delete opMon;
    delete opP1;
    delete opP2;
    delete opP3;
    delete opP4;
    delete opC1;
    delete opC2;
    delete opCC;
    delete opCS;
    delete opL;
    delete opL1;
    delete opL2;
    delete opS;
    delete opS1;
    delete opS2;
    delete opS3;
    delete opS4;
    delete opX;
    delete opY;
    delete opXZ;
    delete opYZ;
    delete opHi1;
    delete opZo1;
    delete opPi1;
    delete opHi3;
    delete opZo3;
    delete opPi3;
    delete opStat;
}

BOOL HplotDialog :: Init(VOID)
{
   CenterDialog(hPar,this->GetHandle());

   char Text[80];
   opText = new StaticText(IDC_CAPTION,this);
   sprintf(Text,"All %d histograms refer to %ld rays",Nhists,Norays);    
   opText->SetText(Text);

   opUTI = new StaticText(IDC_UTI,this);

   Nh    = 1;
   Ncont = 10;
   Phi   = 45;
   Theta = 30;

   opC1  = new RadioButton(IDC_C1,this);
   opC2  = new RadioButton(IDC_C2,this);
   opCC  = new RadioButton(IDC_CC,this);
   opCS  = new RadioButton(IDC_CS,this);
   opL   = new RadioButton(IDC_L, this);
   opL1  = new RadioButton(IDC_L1,this);
   opL2  = new RadioButton(IDC_L2,this);
   opS   = new RadioButton(IDC_S, this);
   opS1  = new RadioButton(IDC_S1,this);
   opS2  = new RadioButton(IDC_S2,this);
   opS3  = new RadioButton(IDC_S3,this);
   opS4  = new RadioButton(IDC_S4,this);
   opC1->SetState(1);

   opP1  = new RadioButton(IDC_PR1,this);
   opP2  = new RadioButton(IDC_PR2,this);
   opP3  = new RadioButton(IDC_PR3,this);
   opP4  = new RadioButton(IDC_PR4,this);
   opP1->SetState(1);

   opIncr  = new RadioButton(IDC_INCR,this);
   opNone  = new RadioButton(IDC_NONE,this);
   opDecr  = new RadioButton(IDC_DECR,this);
   opIncr->SetState(1);

   opLin = new RadioButton(IDC_LIN,this);
   opLog = new RadioButton(IDC_LOG,this);
   opLin->SetState(1);

   opMon = new RadioButton(IDC_MON,this);
   opCol = new RadioButton(IDC_COL,this);
   opMon->SetState(1);

   opStat = new CheckBox(IDC_STAT,this);
   opStat->SetState(1);

   opNHist = new StringEdit(IDC_NUMB, this);
   opNCont = new StringEdit(IDC_NCONT,this);
   opPhi   = new StringEdit(IDC_PHI,  this);
   opTet   = new StringEdit(IDC_THET, this);
   opNHist->SetCharLimit(2);
   opNCont->SetCharLimit(2);
   opPhi->SetCharLimit(2);
   opTet->SetCharLimit(2);
   sprintf(szNHist,"%d", Nh);
   opNHist->SetText(szNHist);
   sprintf(szNCont,"%d", Ncont);
   opNCont->SetText(szNCont);
   sprintf(szPhi,"%d", Phi);
   opPhi->SetText(szPhi);
   sprintf(szTet,"%d", Theta);
   opTet->SetText(szTet);

   opNumbSB  = new ScrollBar(IDC_NUMB_SC,this);
   opNContSB = new ScrollBar(IDC_NCONT_SC,this);
   opPhiSB   = new ScrollBar(IDC_PHI_SC,this);
   opTetSB   = new ScrollBar(IDC_THET_SC,this);
   opNumbSB->SetRange(1,Nhists);
   opNumbSB->SetPageSize(5);
   opNumbSB->SetThumbPos(Nh);
   opNContSB->SetRange(3,30);
   opNContSB->SetPageSize(5);
   opNContSB->SetThumbPos( Ncont );
   opPhiSB->SetRange(0,90);
   opPhiSB->SetPageSize(5);
   opPhiSB->SetThumbPos( Phi );
   opTetSB->SetRange(0,90);
   opTetSB->SetPageSize(5);
   opTetSB->SetThumbPos( Theta );

   opX   = new StringEdit(IDC_X,  this);
   opX->SetCharLimit(5);
   opY   = new StringEdit(IDC_Y,  this);
   opY->SetCharLimit(5);
   opXZ  = new StringEdit(IDC_XZ, this);
   opXZ->SetCharLimit(2);
   opYZ  = new StringEdit(IDC_YZ, this);
   opYZ->SetCharLimit(2);
   opHi1 = new StringEdit(IDC_H1, this);
   opHi1->SetCharLimit(1);
   opHi3 = new StringEdit(IDC_H3, this);
   opHi3->SetCharLimit(1);
   opZo1 = new StringEdit(IDC_ZO1, this);
   opZo1->SetCharLimit(1);
   opZo3 = new StringEdit(IDC_ZO3, this);
   opZo3->SetCharLimit(1);
   opPi1 = new StringEdit(IDC_P1, this);
   opPi1->SetCharLimit(1);
   opPi3 = new StringEdit(IDC_P3, this);
   opPi3->SetCharLimit(1);

   Index   = 0;

   SetTimer(this->GetHandle(), TIMER_ID+1, DELAY, NULL);

   Initialized = TRUE;
   return FALSE;
}

BOOL HplotDialog :: WmTimer(UINT id)
{

    KillTimer(this->GetHandle(),id);

    if (id == TIMER_ID+1)
    {
	SetFocus(GetDlgItem(this->GetHandle(), IDOK));
	SetActiveWindow(this->GetHandle());
    }
    else
    {
//	delayed resetting of Edit-box content to lower/upper limits

	sprintf(szNHist,"%d",Nh);
	opNHist->SetText(szNHist);
	sprintf(szNCont,"%d",Ncont);
	opNCont->SetText(szNCont);
	sprintf(szPhi,"%d",Phi);
	opPhi->SetText(szPhi);
	sprintf(szTet,"%d",Theta);
	opTet->SetText(szTet);
    }

    return FALSE;
}

long HplotDialog :: Control( UINT nControlId, UINT )
{ 
    char text[30];
    int  val, iadd;
    char txt1[10], txt2[10], txt3[10];

    switch (nControlId)
    {   case IDOK:
        case IDYES:
        case IDC_OKAY:
	case IDC_ERA:
	{
	  Index++;

	  FILE *fh = fopen(File,"w+");
	  if (fh)
	  {
 		fprintf(fh,"%d   \n",Index);

	        opX->GetText(txt1,6);
	        opY->GetText(txt2,6);
 		fprintf(fh,"%s,%s\n",txt1,txt2);

	        opXZ->GetText(txt1,3);
	        opYZ->GetText(txt2,3);
 		fprintf(fh,"%s,%s\n",txt1,txt2);

	        opHi1->GetText(txt1,2);
	        opZo1->GetText(txt2,2);
	        opPi1->GetText(txt3,2);
 		fprintf(fh,"%s,%s,%s\n",txt1,txt2,txt3);

	        opHi3->GetText(txt1,2);
	        opZo3->GetText(txt2,2);
	        opPi3->GetText(txt3,2);
 		fprintf(fh,"%s,%s,%s\n",txt1,txt2,txt3);

		iadd = 0;
		if (opStat->GetState() == 1)      // statitical info
		   iadd = 1000;
		if (nControlId == IDC_ERA)
 		  fprintf(fh,"%d\n",-(Nh+iadd));    // erase
		else
 		  fprintf(fh,"%d\n",Nh+iadd);

		if (opLin->GetState() == 1)
		   strcpy(text,"LIN");
		else
		   strcpy(text,"LOG");
 		fprintf(fh,"%s\n",text);

		if (opC1->GetState() == 1)
 		   fprintf(fh,"C1\n");
		else if (opC2->GetState() == 1)
 		   fprintf(fh,"C2\n");
		else if (opCC->GetState() == 1)
 		   fprintf(fh,"CC\n");
		else if (opCS->GetState() == 1)
 		   fprintf(fh,"CS\n");
		else if (opL->GetState() == 1)
 		   fprintf(fh,"L \n");
		else if (opL1->GetState() == 1)
 		   fprintf(fh,"L1\n");
		else if (opL2->GetState() == 1)
 		   fprintf(fh,"L2\n");
		else if (opS->GetState() == 1)
 		   fprintf(fh,"S \n");
		else if (opS1->GetState() == 1)
 		   fprintf(fh,"S1\n");
		else if (opS2->GetState() == 1)
 		   fprintf(fh,"S2\n");
		else if (opS3->GetState() == 1)
 		   fprintf(fh,"S3\n");
		else if (opS4->GetState() == 1)
 		   fprintf(fh,"S4\n");

		if (opMon->GetState() == 1)
 		   fprintf(fh,"M\n");
		else
 		   fprintf(fh,"C\n");

 		fprintf(fh,"%d\n",Ncont);
 		fprintf(fh,"%d,%d\n",Phi,Theta);

 		fclose(fh);    
	  }

//	  auto increment/decrement

	  if (opIncr->GetState() == 1)
	    Nh++;
	  else if (opNone->GetState() == 1)
	    ;
	  else
	    Nh--;
          if (Nh > Nhists)
	    Nh = 1;
	  else if (Nh < 1)
	    Nh = Nhists;
	  sprintf(szNHist,"%d", Nh);
	  opNHist->SetText(szNHist);
	  opNumbSB->SetThumbPos(Nh);

          result = TRUE;
          break;
	}
        case IDCANCEL:
        case IDNO:
	{
	  FILE *fh = fopen(File,"w+");
	  if (fh)
	  {
 		fprintf(fh,"-1   \n");
 		fprintf(fh,"\n");
 		fprintf(fh,"\n");
 		fprintf(fh,"\n");
 		fclose(fh);    
	  }
          TerminateDialog();
          result = FALSE;
          break;
	}
	case IDC_PR:
	case IDC_MET:
	{
	  Index++;

	  FILE *fh = fopen(File,"w+");
	  if (fh)
	  {
		if (nControlId == IDC_MET)
		   strcpy(text,"m ");
		else
		{
		   if (opP1->GetState() == 1)
		      strcpy(text,"p1");
		   else if (opP2->GetState() == 1)
		      strcpy(text,"p2");
		   else if (opP3->GetState() == 1)
		      strcpy(text,"p3");
		   else if (opP4->GetState() == 1)
		      strcpy(text,"p4");
		}
 		fprintf(fh,"%d,%s\n",Index,text);
 		fprintf(fh,"\n");
 		fprintf(fh,"\n");
 		fprintf(fh,"\n");
 		fclose(fh);    
	  }
	  break;
	}

        case IDC_NUMB_SC:
	{
          val = (int)opNumbSB->GetThumbPos();
          sprintf(szNHist,"%d",val);
          opNHist->SetText(szNHist);
	  Nh = val;
          break;
	}
        case IDC_NCONT_SC:
	{
          val = (int)opNContSB->GetThumbPos();
	  Ncont = val;
          sprintf(szNCont,"%d",val);
          opNCont->SetText(szNCont);
          break;
	}
        case IDC_PHI_SC:
	{
          val = (int)opPhiSB->GetThumbPos();
	  Phi = val;
          sprintf(szPhi,"%d",val);
          opPhi->SetText(szPhi);
          break;
	}
        case IDC_THET_SC:
	{
          val = (int)opTetSB->GetThumbPos();
	  Theta = val;
          sprintf(szTet,"%d",val);
          opTet->SetText(szTet);
          break;
	}
        case IDC_NUMB:
	{
          if (!Initialized) return FALSE;
          opNHist->GetText(szNHist,5);
          if (strlen(szNHist))
          {  if (sscanf(szNHist,"%d",&val) == 1)
             {  if (val < 1)
		{
                  val = 1;
                  SetTimer(this->GetHandle(), TIMER_ID, DELAY, NULL);
		}
                if (val > Nhists)
		{
                  val = Nhists;
                  SetTimer(this->GetHandle(), TIMER_ID, DELAY, NULL);
		}
		Nh = val;
                opNumbSB->SetThumbPos(val);
             }
          }
          break;
	}
        case IDC_NCONT:
	{
          if (!Initialized) return FALSE;
          opNCont->GetText(szNCont,5);
          if (strlen(szNCont))
          {  if (sscanf(szNCont,"%d",&val) == 1)
             {  if (val < 3)
		{
                  val = 3;
                  SetTimer(this->GetHandle(), TIMER_ID, DELAY, NULL);
		}
                if (val > 30)
		{
                  val = 30;
                  SetTimer(this->GetHandle(), TIMER_ID, DELAY, NULL);
		}
		Ncont = val;
                opNContSB->SetThumbPos(val);
             }
          }
          break;
	}
        case IDC_PHI:
	{
          if (!Initialized) return FALSE;
          opPhi->GetText(szPhi,5);
          if (strlen(szPhi))
          {  if (sscanf(szPhi,"%d",&val) == 1)
             {  if (val < 0)
		{
                  val = 0;
                  SetTimer(this->GetHandle(), TIMER_ID, DELAY, NULL);
		}
                if (val > 90)
		{
                  val = 90;
                  SetTimer(this->GetHandle(), TIMER_ID, DELAY, NULL);
		}
		Phi = val;
                opPhiSB->SetThumbPos(val);
             }
          }
          break;
	}
        case IDC_THET:
	{
          if (!Initialized) return FALSE;
          opTet->GetText(szTet,5);
          if (strlen(szTet))
          {  if (sscanf(szTet,"%d",&val) == 1)
             {  if (val < 0)
		{
                  val = 0;
                  SetTimer(this->GetHandle(), TIMER_ID, DELAY, NULL);
		}
                if (val > 90)
		{
                  val = 90;
                  SetTimer(this->GetHandle(), TIMER_ID, DELAY, NULL);
		}
		Theta = val;
                opTetSB->SetThumbPos(val);
             }
          }
          break;
	}
        case IDC_C1:
	  opUTI->SetText("Contour Plot (monochrome)");
          break;
        case IDC_C2:
	  opUTI->SetText("Contour Plot (color)");
          break;
        case IDC_CC:
	  opUTI->SetText("Color Plot");
          break;
        case IDC_CS:
	  opUTI->SetText("Scatter Plot (monochrome)");
          break;
        case IDC_L:
	  opUTI->SetText("Lego Plot (monochrome)");
          break;
        case IDC_L1:
	  opUTI->SetText("Color Lego Plot (mode 1)");
          break;
        case IDC_L2:
	  opUTI->SetText("Color Lego Plot (mode 2)");
          break;
        case IDC_S:
	  opUTI->SetText("Surface Plot (monochrome)");
          break;
        case IDC_S1:
	  opUTI->SetText("Color Surface Plot (mode 1)");
          break;
        case IDC_S2:
	  opUTI->SetText("Color Surface Plot (mode 2)");
          break;
        case IDC_S3:
	  opUTI->SetText("Surface Plot with contours on top");
          break;
        case IDC_S4:
	  opUTI->SetText("Surface Plot with Gouraud shading");
          break;
    }
    return FALSE;
}


BOOL HplotDialog :: GetResult(VOID)
{
    return(result);
}
