///////////////////////////////////////////////////////////////////////////////
//////////////////////// class TransFile implementation ///////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"


VOID GetTitleOfElement(int type, char *title)
{
   switch (type)
   {  case 1:  strcpy(title,String(IDS_BEAM_T));   break;
      case 2:  strcpy(title,String(IDS_ROTAT_T));  break;
      case 3:  strcpy(title,String(IDS_DRIFT_T));  break;
      case 4:  strcpy(title,String(IDS_BEND_T));   break;
      case 5:  strcpy(title,String(IDS_QUAD_T));   break;
      case 6:  strcpy(title,String(IDS_SLIT_T));   break;
      case 7:  strcpy(title,String(IDS_SHIFT_T));  break;
      case 11: strcpy(title,String(IDS_ACC_T));    break;
      case 12: strcpy(title,String(IDS_CORR_T));   break;
      case 14: strcpy(title,String(IDS_MATRIX_T)); break;
      case 18: strcpy(title,String(IDS_SEXT_T));   break;
      case 19: strcpy(title,String(IDS_SOLO_T));   break;
      case 20: strcpy(title,String(IDS_ZRO_T));    break;
      case 21: strcpy(title,String(IDS_WIEN_T));   break;
      case 22: strcpy(title,String(IDS_SPCH_T));   break;
      case 23: strcpy(title,String(IDS_RFCAV_T));  break;
      case 30: strcpy(title,String(IDS_SCINT_T));  break;
      case 102: strcpy(title,String(IDS_FRINGE_T)); break;
      case 104: strcpy(title,String(IDS_EPRI_T));  break;
      case 105: strcpy(title,String(IDS_EQUAD_T)); break;
      default: strcpy(title,""); break;
   }
}

BOOL CheckDimensions(char *dim[11])
{
   char S[40], D[40];

   strcpy(S,String(IDS_UNIT_SRC));
   strcpy(D,String(IDS_UNIT_DES));

   for (int i=0; D[i] != '\0'; i++)  // replace '~' --> '\0'
       if (D[i] == '~')
           D[i] = '\0';

   if (strncmp(dim[0], &S[0],2)  == 0) strncpy(dim[0], &D[0],3);  // MM  -> mm
   if (strncmp(dim[0], &S[3],2)  == 0) strncpy(dim[0], &D[3],3);  // CM  -> cm
   if (strncmp(dim[1], &S[6],2)  == 0) strncpy(dim[1], &D[6],5);  // MR  -> mrad
   if (strncmp(dim[2], &S[0],2)  == 0) strncpy(dim[2], &D[0],3);  // MM  -> mm
   if (strncmp(dim[2], &S[3],2)  == 0) strncpy(dim[2], &D[3],3);  // CM  -> cm
   if (strncmp(dim[3], &S[6],2)  == 0) strncpy(dim[3], &D[6],5);  // MR  -> mrad
   if (strncmp(dim[4], &S[0],2)  == 0) strncpy(dim[4], &D[0],3);  // MM  -> mm
   if (strncmp(dim[4], &S[3],2)  == 0) strncpy(dim[4], &D[3],3);  // CM  -> cm
   if (strncmp(dim[5], &S[11],2) == 0) strncpy(dim[5], &D[11],2); // PC  -> %
   if (strncmp(dim[5], &S[14],2) == 0) strncpy(dim[5], &D[14],3); // PM  -> %o
   if (strncmp(dim[6], &S[17],3) == 0) strncpy(dim[6], &D[17],4); // DEG -> Deg
   if (strncmp(dim[7], &S[21],1) == 0) strncpy(dim[7], &D[21],2); // M   -> m
   if (strncmp(dim[8], &S[23],2) == 0) strncpy(dim[8], &D[23],3); // KG  -> kG
   if (strncmp(dim[10],&S[26],3) == 0) strncpy(dim[10],&D[26],4); // GEV -> GeV
   if (strncmp(dim[10],&S[30],3) == 0) strncpy(dim[10],&D[30],4); // MEV -> MeV

   if (dim[0][1] != 'm' || dim[2][1] != 'm')   // written by PLOTFL ?
      return FALSE;                            // no!
   else
      return TRUE;
}

VOID PutInfo(UINT wID)
{
   char buf[256];

   LoadString(Application::GetInstance(),wID,buf,255);
   SimpleDialog oDlg(IDD_INFO_DIALOG,Application::GetMainWindowPtr(),buf);
}

char *String(UINT wID, UINT n)
{
   static char szBuffer[3][128];

   if (n==2)  // Bounds Checker
      LoadString(Application::GetInstance(),wID,szBuffer[n],127);
   else
      LoadString(Application::GetInstance(),wID,szBuffer[n],255);
   return szBuffer[n];
}

///////////////////////////////////////////////////////////////////////////////

/*
   Special for reading FOR000.DAT and FOR002.DAT:
   ----------------------------------------------
   CR,LF cannot be included into the different records (REC1 to REC5) written
   by FORTRAN because Windows NT reads only multiples of 4 bytes. If we would
   do so, the file pointer would be positioned 2 bytes after the end of the
   record just read. So we make all recordlengths multiples of 4 bytes and jump
   over CR,LF after having read the record with fseek.
   For Windows 3.1 (16 bits) all this would not be necessary, but it works
   there too.
*/

void adjust(FILE *fp) // jump over CR,LF
{
   fseek(fp, 2L, 1);  // advance file pointer by 2 bytes from current position
}

///////////////////////////////////////////////////////////////////////////////

TransFile :: TransFile(char *filename, char *savefile, char *for001)
           : TitleOfRun(for001)
{
    long len, curpos;
    FILE *fhand;

    Blank[0]   = ' ';  Blank[1] = '\0';
    NullStr[0] = '\0';
    RB[0]      = 'r';  RB[1] = 'b';  RB[2] = '\0';

    for (int j=0; j<11; j++)
       dim[j] = new char[5];  // new position of this found by Bounds Checker

    NumberOfOpenFiles = 0;
    fhand = fopen(filename,RB);   // open binary file (FORTRAN)
    if (fhand)
    {  fp[NumberOfOpenFiles] = fhand;
       NumberOfOpenFiles++;
    } 
    else
       fp[0] = 0;

    if (savefile != NULL)
    {  int pos = strlen(savefile) - 5;
       for (int i=0; i<10; i++)
       {  savefile[pos] = char('0' + i);
          {  fhand = fopen(savefile,RB);
             if (fhand)
             {  fp[NumberOfOpenFiles] = fhand;
                NumberOfOpenFiles++;
             } 
          }
       }
    }

    nr = 0;
    if (fp[0])
    {
       curpos = ftell(fp[0]);   /* save current position */
       fseek(fp[0], 0L, 2);     /* put file pointer to EOF */
       len = ftell(fp[0]);      /* = length of file */
       fseek(fp[0], curpos, 0); /* put back at saved position */
       nr = (int)(len - sizeof(REC1) - 2) / (sizeof(REC2) + 2);
       if (nr > 0)
       {  GetDimensions();
          if (!CheckDimensions(dim))
             nr = -1;
       }
       else
          nr = 0;
    }

    info      = new char[80];
    g2        = (float)10.0;
    k1        = (float)0.5;
    k2        = (float)0.0;
    r1inv     = (float)0.0;
    r2inv     = (float)0.0;
    prevtype  = 0;
    linesout  = 0;
    flag      = 0;
    oldchisqr = (float)-0.1;
    totleng   = (float)0.0;
    totrot    = (float)0.0;

    first     = TRUE;
    equad     = FALSE;  // default to Magn. QUAD
    ebend     = FALSE;  // default to Magn. BEND

    f[0]  = &TransFile::NextQuad;
    f[1]  = &TransFile::NextBeamLine;
    f[2]  = &TransFile::NextBend;
    f[3]  = &TransFile::InitialBeam;
    f[4]  = &TransFile::NextFitCondition;
    f[5]  = &TransFile::NextShift;
    f[6]  = &TransFile::NextElement;
    f[7]  = &TransFile::NextQuadDac;
    f[8]  = &TransFile::NextQuadAmp;
    f[9]  = &TransFile::NextBendDac;
    f[10] = &TransFile::NextBendAmp;
    f[11] = &TransFile::NextLayout;
    f[12] = &TransFile::ErrorMess;

    flag14 = FALSE;

    caption = new char[100];
}

TransFile :: ~TransFile(void)
{
   for (int j=0; j<11; j++)
      delete []dim[j]; // [] found with Bounds Checker
   for (int i=0; i<NumberOfOpenFiles; i++)
      fclose(fp[i]);
   if (info) delete []info;       // found with Bounds Checker
   if (caption) delete []caption; // dito
}

char *TransFile :: GetDimension(int ndim)
{
   return dim[ndim];
}

int TransFile :: GetNumberOfOpenFiles(void)
{
   return NumberOfOpenFiles;
}

FILE *TransFile :: GetFilePointer(int fn)
{
   return (fp[fn]);
}

int TransFile :: GetNumberOfElements(void)
{
   return nr;
}

void TransFile :: Rewind(void)
{
    for (int i=0; i<NumberOfOpenFiles; i++)
       fseek(fp[i], (long)sizeof(REC1) + 2, 0);
    totleng = (float)0.0;
}

void TransFile :: GetDimensions(void)
{
   REC1 rec1;
   char *p;
   char lab[15];
   lab[4] = '\0';

   fread(&rec1,sizeof(REC1),1,fp[0]);   // read first record of file
   for (int j=0; j<11; j++)
   {  strncpy(lab,&rec1.xdime[j][0],4);
      p = strtok(lab,Blank);            // cutoff trailing blanks
//      dim[j] = new char[5]; // old position, Bounds Checker didn't like
      if (p)
      {  *(p+4) = '\0';           // limit
         strcpy(dim[j],p);
      }
   }
   adjust(fp[0]);
}

int TransFile :: NextQuad(char *buffer)
{
   REC2  rec2;
   float hap;
   char  lab[5];
   lab[4] = '\0';

   while (fread(&rec2,sizeof(REC2),1,fp[0]) == 1)
   {  
      adjust(fp[0]);
      if (rec2.type == 16 && rec2.data[0] == 96.0) // it is an Elec. QUAD
       equad = TRUE;
      if (rec2.type == 16 && rec2.data[0] == 95.0) // it is an Magn. QUAD
       equad = FALSE;
      if (rec2.type == 16 && (int)rec2.data[0] == 5)  // for Solenoid
      {  g2 = rec2.data[1];
         continue;
      }
      if (rec2.type == 5 || rec2.type == 18 || rec2.type == 19)
      {  GetTitleOfElement((int)rec2.type,title);
         strncpy(lab,&rec2.label[0],4);
         if (rec2.type == 19)           // Solenoid
            hap = g2;
         else
            hap = rec2.data[2];
	 if (equad)
	 {
           GetTitleOfElement(105,title);
           sprintf(buffer,String(IDS_QO_FORM),
                  title,
                  lab,
                  rec2.data[0],dim[7],
                  rec2.data[1],"kV/cm",
                  hap,dim[0]);
         }
         else
           sprintf(buffer,String(IDS_QO_FORM),
                  title,
                  lab,
                  rec2.data[0],dim[7],
                  rec2.data[1],dim[8],
                  hap,dim[0]);
         return 1;
      }
   }
   return 0;
}

int TransFile :: NextBend(char *buffer)
{
   REC2 rec2;
   char lab[5];
   lab[4] = '\0';

   while (fread(&rec2,sizeof(REC2),1,fp[0]) == 1)
   {  
      adjust(fp[0]);
      if (rec2.type == 16 && rec2.data[0] == 94.0) // it is an Elec. PRISM
       ebend = TRUE;
      if (rec2.type == 16 && rec2.data[0] == 93.0) // it is an Magn. BEND
       ebend = FALSE;
      if (rec2.type == 16)
      {  switch ((int)rec2.data[0])
         {  case 5:  g2    = rec2.data[1]; break;
            case 7:  k1    = rec2.data[1]; break;
            case 8:  k2    = rec2.data[1]; break;
            case 12: r1inv = rec2.data[1]; break;
            case 13: r2inv = rec2.data[1]; break;
            default: break;
         }
         prevtype = 0L;
      }
      if ( (rec2.type == 2 && rec2.data[0] != 1000.) || rec2.type == 4 )
      {  GetTitleOfElement((int)rec2.type,title);
         strncpy(lab,&rec2.label[0],4);
         if (rec2.type == 4)
         {
	   if (ebend)
	    sprintf(buffer,
                    String(IDS_BE_FORM),
                    lab,
                    rec2.data[0],dim[7],
                    g2,dim[2],
                    rec2.data[3],dim[6],
                    rec2.data[2],
		    rec2.data[1]);
	   else
	    sprintf(buffer,
                    String(IDS_BA_FORM),
                    title,
                    lab,
                    rec2.data[0],dim[7],
                    g2,dim[2],
                    rec2.data[1],dim[8],
                    rec2.data[2],
                    rec2.data[3],dim[6]);
           prevtype = 4L;
         }
         else
         {  float rinv = r1inv;
            if (prevtype == 4)
               rinv = r2inv;
            sprintf(buffer,
                    String(IDS_BK_FORM),
                    title,
                    lab,
                    rec2.data[0],dim[6],
                    k1, k2, rinv);
            prevtype = 2L;
         }
         return 1;
      }
   }
   return 0;
}

int TransFile :: NextBeamLine(char *buffer)
{
   REC2 rec2;
   char lab[5];
   lab[4] = '\0';
   static int arg = 0;

   while (fread(&rec2,sizeof(REC2),1,fp[0]) == 1)
   {
      adjust(fp[0]);
      if (rec2.type == 16 && rec2.data[0] == 96.0) // it is an Elec. QUAD
         equad = TRUE;
      if (rec2.type == 16 && rec2.data[0] == 95.0) // it is an Magn. QUAD
         equad = FALSE;
      if (rec2.type == 16 && rec2.data[0] == 94.0) // it is an Elec. PRISM
         ebend = TRUE;
      if (rec2.type == 16 && rec2.data[0] == 93.0) // it is an Magn. BEND
         ebend = FALSE;
      if (rec2.type == 2 && rec2.data[0] == 1000.0)
         rec2.type = 102; // Quad fringe field
      if (rec2.type == 4 && ebend)
         rec2.type = 104; // electrostatic prism
      if (rec2.type == 5 && equad)
         rec2.type = 105; // electrostatic quad
      if (rec2.type == 20)
      {  totrot += rec2.data[0];
         arg = (int)totrot;
      } 
      GetTitleOfElement((int)rec2.type,title);
      if (strlen(title) > 0)  // choice of elements with title
      {  strncpy(lab,&rec2.label[0],4);
         if (arg == 90 || arg == -90 || arg == -270)
             sprintf(buffer,
                 String(IDS_BL_FORM),
                 title,
                 lab,
                 rec2.along,   dim[7],
                 rec2.siun[2], dim[2],
                 rec2.siun[0], dim[0],
                -rec2.rtmx[8], dim[2], dim[5],
                 rec2.rtmx[2], dim[0], dim[5]);
	 else
             sprintf(buffer,
                 String(IDS_BL_FORM),
                 title,
                 lab,
                 rec2.along,   dim[7],
                 rec2.siun[0], dim[0],
                 rec2.siun[2], dim[2],
                 rec2.rtmx[2], dim[0], dim[5],
                 rec2.rtmx[8], dim[2], dim[5]);
         return 1;
      }
   }
   return 0;
}

int TransFile :: NextLayout(char *buffer)
{
   REC2 rec2;
   char lab[5];
   lab[4] = '\0';
   static int arg = 0;
   if (first)
   {
      sprintf(buffer,
"                     l         x         y         z          theta       phi");
      first = FALSE;
      return 1;
   }
   while (fread(&rec2,sizeof(REC2),1,fp[0]) == 1)
   {
      adjust(fp[0]);
      if (rec2.type == 16 && rec2.data[0] == 96.0) // it is an Elec. QUAD
         equad = TRUE;
      if (rec2.type == 16 && rec2.data[0] == 95.0) // it is an Magn. QUAD
         equad = FALSE;
      if (rec2.type == 16 && rec2.data[0] == 94.0) // it is an Elec. PRISM
         ebend = TRUE;
      if (rec2.type == 16 && rec2.data[0] == 93.0) // it is an Magn. BEND
         ebend = FALSE;
      if (rec2.type == 2 && rec2.data[0] == 1000.0)
         rec2.type = 102; // Quad fringe field
      if (rec2.type == 4 && ebend)
         rec2.type = 104; // electrostatic prism
      if (rec2.type == 5 && equad)
         rec2.type = 105; // electrostatic quad
      if (rec2.type == 20)
      {  totrot += rec2.data[0];
         arg = (int)totrot;
      } 
      GetTitleOfElement((int)rec2.type,title);
      if (strlen(title) > 0)  // choice of elements with title
      {  strncpy(lab,&rec2.label[0],4);
         if (rec2.layout[3] >= 7777.)
            return 0;
         else
         {
            sprintf(buffer,
                 String(IDS_LAY_FORM),
                 title,
                 lab,
		 rec2.along, dim[7],
                 rec2.layout[0],
                 rec2.layout[1],
                 rec2.layout[2], dim[7],
                 rec2.layout[3],
                 rec2.layout[4], dim[6]);
            return 1;
	 }
      }
   }
   return 0;
}

int TransFile :: InitialBeam2(char *buffer)
{
   flag14 = TRUE;
   return InitialBeam(buffer);
}

int TransFile :: InitialBeam(char *buffer)
{
   REC2 rec2;

   if (linesout == 0)
   {  while (fread(&rec2,sizeof(REC2),1,fp[0]) == 1)
      {  
         adjust(fp[0]);
         if ( (rec2.type == 1 || rec2.type == 12 || rec2.type == 14) &&
               rec2.along == 0.0)
         {
	    if (rec2.type == 1)
	    {
	       p0     = rec2.data[0];
	       dpp    = rec2.siun[5];

               x0     = rec2.siun[0];
               xp0    = rec2.siun[1];
               r12    = 0.0;
	       epsx   = x0 * xp0;
	       alphax = (float) 0.0;
	       betax  = x0 * x0 / epsx;

	       y0     = rec2.siun[2];
	       yp0    = rec2.siun[3];
	       r34    = 0.0;
	       epsy   = y0 * yp0;
	       alphay = (float) 0.0;
	       betay  = y0 * y0 / epsy;
	    }
	    if (rec2.type == 12)
	    {
               r12    = rec2.r12;
	       epsx   = x0 * xp0 * (float)sqrt(fabs(1.0-r12*r12));
	       alphax = (float) -tan(asin(r12));
	       betax  = x0 * x0 / epsx;

	       r34    = rec2.r34;
	       epsy   = y0 * yp0 * (float)sqrt(fabs(1.0-r34*r34));
	       alphay = (float) -tan(asin(r34));
	       betay  = y0 * y0 / epsy;
	    }
	    if ((rec2.type == 14) && flag14)
	    {
               r16   = rec2.rtmx[2];
               r26   = rec2.rtmx[5];
	    }
	    if (rec2.type == 14)
	    {
               r16   = rec2.rtmx[2];
               r26   = rec2.rtmx[5];
	    }
         }
      }   
      strcpy(buffer,String(IDS_0_M)); 
      linesout++;
      return 1;
   }
   if (linesout < 17)
   {
      switch (linesout)
      {
         case 1:  sprintf(buffer,String(IDS_BE1_FORM),x0,dim[0]); break;
         case 2:  sprintf(buffer,String(IDS_BE2_FORM),xp0,dim[1]); break;
         case 3:  sprintf(buffer,String(IDS_BE3_FORM),r12); break;
         case 4:  sprintf(buffer,String(IDS_BE4_FORM),epsx,dim[0],dim[1]); break;
         case 5:  sprintf(buffer,String(IDS_BE5_FORM),y0,dim[2]); break;
         case 6:  sprintf(buffer,String(IDS_BE6_FORM),yp0,dim[3]); break;
         case 7:  sprintf(buffer,String(IDS_BE7_FORM),r34); break;
         case 8:  sprintf(buffer,String(IDS_BE8_FORM),epsy,dim[2],dim[3]); break;
         case 9:  sprintf(buffer,String(IDS_BE9_FORM),r16,dim[0],dim[5]); break;
         case 10: sprintf(buffer,String(IDS_BEA_FORM),r26,dim[1],dim[5]); break;
         case 11: sprintf(buffer,String(IDS_BEB_FORM),p0,dim[10]); break;
         case 12: sprintf(buffer,String(IDS_BEC_FORM),dpp,dim[5]); break;
	 case 13: sprintf(buffer,String(IDS_BED_FORM),alphax); break;
	 case 14: sprintf(buffer,String(IDS_BEE_FORM),betax,dim[7]); break;
	 case 15: sprintf(buffer,String(IDS_BEF_FORM),alphay); break;
	 case 16: sprintf(buffer,String(IDS_BEG_FORM),betay,dim[7]); break;
      }
      linesout++;
      return 1;
   }
   linesout = 0;
   return 0;
}

int TransFile :: NextFitCondition(char *buffer)
{
   int  i;
   REC5 rec5;
   char temp[50];
   char lab[5];
   lab[4] = '\0';

   if (flag == 0)
   {  while (fread(&rec5,sizeof(REC5),1,fp[0]) == 1)
      {
         adjust(fp[0]);
         if (rec5.type == 10)
         {  if (rec5.chisqr != oldchisqr)
            {   strncpy(labs,&rec5.label[0],4); labs[4] = '\0';
                z     = rec5.z;
                f1    = rec5.f1;
                f2    = rec5.f2;
                con   = rec5.con;
                delta = rec5.delta;
                val   = rec5.val;
                nl    = (int)rec5.nl;
                for (i=0; i<nl; i++)
                {  labv[i] = new char[5];
                   strcpy(labv[i],String(IDS_BLANK_LB));
                   strncpy(labv[i],&rec5.labs[i][0],4);
                }
                flag  = 1;
                oldchisqr = rec5.chisqr;
                strcpy(buffer,NullStr);
                sprintf(temp,String(IDS_CHISQ),oldchisqr);
                strcat(buffer,temp);
                if (rec5.failed != 0L) strcat(buffer,String(IDS_FAILED));
                return 1;
            }
            else
            {   strncpy(lab,&rec5.label[0],4);
                sprintf(buffer,
                       String(IDS_FIT_FORM),
                       lab,rec5.z,rec5.f1,rec5.f2,rec5.con,rec5.delta,rec5.val);
                return 1;
            }
         }
      }
   }

   if (flag == 1)
   {  flag = 2;
      strcpy(buffer,String(IDS_VAR));
      for (i=0; i<nl; i++)
      {  strcat(buffer,labv[i]);
         strcat(buffer,", ");
         delete []labv[i]; // [] found with Bounds Checker
      }
      buffer[strlen(buffer)-2] = '\0';
      return 1;
   }

   if (flag == 2)
   {  flag = 0;
      sprintf(buffer,
              String(IDS_FIT_FORM),
              labs,z,f1,f2,con,delta,val);
      return 1;
   }
   return 0;
}

int TransFile :: NextShift(char *buffer)
{
   REC2 rec2;
   int  icon1, icon2;
   char lab[5];
   lab[4] = '\0';

   while (fread(&rec2,sizeof(REC2),1,fp[0]) == 1)
   {  
      adjust(fp[0]);
      if (rec2.type == 6 || rec2.type == 7)   // Shift or Slit
      {  GetTitleOfElement((int)rec2.type,title);
         strncpy(lab,&rec2.label[0],4);
         if (rec2.type == 6)
         {  icon1 = int(rec2.data[0]);
            icon2 = int(rec2.data[2]);
	    if (icon1 == 0)
	    {
		break; // it is an update e.g. 6. 0. 2. ;
	    }
            else if (icon2 > 0)  // 2-dimensional
               sprintf(buffer,
                       String(IDS_S2D_FORM),
                       title,lab,
                       rec2.along, dim[7],
                       icon1,
                       rec2.data[1],dim[icon1-1],
                       icon2,
                       rec2.data[3],dim[icon2-1]);
            else           // one-dimensional
               sprintf(buffer,
                       String(IDS_S1D_FORM),
                       title,lab,
                       rec2.along, dim[7],
                       icon1,
                       rec2.data[1],dim[icon1-1]);
            return 1;
         }
         else
         {  sprintf(buffer,
                    String(IDS_AXS_FORM),
                    title, lab,
                    rec2.along, dim[7],
                    rec2.data[0],dim[0],
                    rec2.data[1],dim[1],
                    rec2.data[2],dim[2],
                    rec2.data[3],dim[3]);
            return 1;
         }
      }
   }
   return 0;
}

int TransFile :: NextElement(char *buffer)
{
   REC2 rec2;
   float null  = (float)0.0;
   char lab[5];
   lab[4] = '\0';

   while (fread(&rec2,sizeof(REC2),1,fp[0]) == 1)
   {  
      adjust(fp[0]);
      if (rec2.type == 2 && rec2.data[0] == 1000.0)
         rec2.type = 102; // Quad fringe field
      if (rec2.type == 4 && ebend)
         rec2.type = 104; // electrostatic prism
      if (rec2.type == 5 && equad)
         rec2.type = 105; // electrostatic quad
      GetTitleOfElement((int)rec2.type,title);
      strncpy(lab,&rec2.label[0],4);
      switch (rec2.type)
      {  
         case 1:
         { sprintf(buffer,
                   String(IDS_NX1_FORM),
                   title,lab,rec2.data[0],dim[10]);
           prevtype = 1L;
           break;
         }
         case 2:
         { float rinv = r1inv;
           if (prevtype == 4)
              rinv = r2inv;
           sprintf(buffer,
                   String(IDS_NX2_FORM),
                   title, lab,
                   rec2.data[0],dim[6],
                   k1, k2, rinv);
           prevtype = 2L;
           break;
         }
         case 3:
         { sprintf(buffer,
                   String(IDS_NX1_FORM),
                   title,lab,rec2.data[0],dim[7]);
           totleng += rec2.data[0];
           prevtype = 3L;
           break;
         }
         case 4:
         case 104:
         {
	   if (ebend) 
	   {
	     sprintf(buffer,
                   String(IDS_NXE_FORM),
                   lab,
                   rec2.data[0],dim[7],
                   g2,dim[2],
                   rec2.data[3],dim[6],
                   rec2.data[2],
		   rec2.data[1]);
	   }
	   else
	   {
	     sprintf(buffer,
                   String(IDS_NX4_FORM),
                   title, lab,
                   rec2.data[0],dim[7],
                   g2,dim[2],
                   rec2.data[1],dim[8],
                   rec2.data[2],
                   rec2.data[3],dim[6]);
	   }
           prevtype = 4L;
           totleng += rec2.data[0];
           break;
         }
         case 5:
         case 105:
	 {
	   if (equad)
           { sprintf(buffer,
                   String(IDS_NX5E_FORM),
                   title,lab,
                   rec2.data[0],dim[7],
                   rec2.data[2],dim[0],
                   rec2.data[1]);
           }
	   else
           { sprintf(buffer,
                   String(IDS_NX5_FORM),
                   title,lab,
                   rec2.data[0],dim[7],
                   rec2.data[2],dim[0],
                   rec2.data[1],dim[8]);
           }
           prevtype = 5L;
           totleng += rec2.data[0];
           break;
	 }
         case 18:
         { sprintf(buffer,
                   String(IDS_NX5_FORM),
                   title,lab,
                   rec2.data[0],dim[7],
                   rec2.data[2],dim[0],
                   rec2.data[1],dim[8]);
           totleng += rec2.data[0];
           prevtype = 18L;
           break;
         }
         case 19:
         case 21:
         { sprintf(buffer,
                   String(IDS_NX5_FORM),
                   title,lab,
                   rec2.data[0],dim[7],
                   g2,dim[0],   // aperture given by 16. 5. g/2 ;
                   rec2.data[1],dim[8]);
           totleng += rec2.data[0];
           prevtype = 19L;
           break;
         }
         case 6:
         { int icon1 = (int)rec2.data[0];
           int icon2 = (int)rec2.data[2];
           if (icon1 == 0)
	   {
	      sprintf(buffer,"*UPDATE*   %s     R%d",lab,(int)rec2.data[1]);
	      break;
	   }
           else if (icon2 > 0)  // 2-dimensional
              sprintf(buffer,
                      String(IDS_N62_FORM),
                      title,lab,
                      null,dim[7],
                      icon1,
                      rec2.data[1],dim[icon1-1],
                      icon2,
                      rec2.data[3],dim[icon2-1]);
           else           // one-dimensional
              sprintf(buffer,
                      String(IDS_N61_FORM),
                      title,lab,
                      null,dim[7],
                      icon1,
                      rec2.data[1],dim[icon1-1]);
           prevtype = 6L;
           break;
         }
         case 7:
         case 23:
         { sprintf(buffer,
                   String(IDS_NX7_FORM),
                   title,lab);
           prevtype = 7L;
           break;
         }
         case 11:
         { sprintf(buffer,
                   String(IDS_N11_FORM),
                   title,lab,
                   rec2.data[0],dim[7],
                   rec2.data[2],dim[10],
                   rec2.data[1],dim[10]);
           totleng += rec2.data[0];
           prevtype = 11L;
           break;
         }
         case 14:
         { sprintf(buffer,
                   String(IDS_NX1_FORM),
                   title,lab,rec2.data[0],dim[7]);
           totleng += rec2.data[0];
           prevtype = 14L;
           break;
         }
         case 16:
         {  switch ((int)rec2.data[0])
            {  case 5:  g2    = rec2.data[1]; break;
               case 7:  k1    = rec2.data[1]; break;
               case 8:  k2    = rec2.data[1]; break;
               case 12: r1inv = rec2.data[1]; break;
               case 13: r2inv = rec2.data[1]; break;
	       case 96: equad = TRUE;  break;
	       case 95: equad = FALSE; break;
	       case 94: ebend = TRUE;  break;
	       case 93: ebend = FALSE; break;
               default: break;
            }
           prevtype = 16L;
           continue;  // no output
         }
         case 20:
         { sprintf(buffer,
                   String(IDS_NX1_FORM),
                   title,lab,rec2.data[0],dim[6]);
           prevtype = 20L;
           break;
         }
         case 22:
         { sprintf(buffer,
                   String(IDS_SPC_FORM),
                   title,lab,
                   rec2.data[0],dim[7],
                   rec2.data[1],
                   rec2.data[2],
                   rec2.data[3]);
           prevtype = 22L;
           break;
	 }
         case 30:
           prevtype = 30L;
           continue;  // no output
         case 102:
         { sprintf(buffer,
                   String(IDS_NX2_FORM),
                   title, lab,
                   null,dim[6],
                   rec2.data[1],
                   rec2.data[2],
                   rec2.data[3]);
           prevtype = 102L;
           break;
         }
         default:
           prevtype = 0L;
           continue;  // no output
      }
      return 1;
   }

// fileend reached -- add line with beamline length

   if (totleng > 0.0)
   {  sprintf(buffer,
              String(IDS_TLG_FORM),
              totleng,dim[7]);
      totleng = (float)0.0;
      return 1;
   }
   else
      return 0;
}

int TransFile :: NextQuadData(int flag, CalibrationData *opCal, char *buffer)
{
   REC2  rec2;
   char  buff[20];
   int   i;
   long  dac;
   float hap, amp;
   char  lab[5];
   lab[4] = '\0';

   int nrl = opCal->GetNumberOfLines();
   while (fread(&rec2,sizeof(REC2),1,fp[0]) == 1)
   {  
      adjust(fp[0]);
      if (rec2.type == 16 && (int)rec2.data[0] == 5)  // for Solenoid
      {  g2 = rec2.data[1];
         continue;
      }
      if (rec2.type == 5 || rec2.type == 18 || rec2.type == 19)
      {  strncpy(lab,&rec2.label[0],4);
         if (rec2.type == 19)           // Solenoid
            hap = g2;
         else
            hap = rec2.data[2];
         int index = nrl;                // default: not in QUADS.CAL
         for (i=0; i<nrl; i++)
            if (opCal->CompareData(i,lab,rec2.data[0],hap))
            {  index = i;
               break;
            }
         opCal->GetDac(rec2.data[1],index,dac,amp);
         strcpy(buffer,lab);
         strcat(buffer,"  ");
         if (flag == 0)
         {  if (index == nrl)
	       sprintf(buff,"not available");
	    else
	       sprintf(buff,String(IDS_7LD_FORM),dac);
            strcat(buffer,buff);
         }
         else
         {  if (index == nrl)
               sprintf(buff,"not available");
            else
               sprintf(buff,String(IDS_71A_FORM),amp);
            strcat(buffer,buff);
         }
         return 1;
      }
   }
   return 0;
}

int TransFile :: NextBendData(int flag, CalibrationData *opCal, char *buffer)
{
   REC2  rec2;
   char  buff[20];
   int   i, index;
   long  dac;
   float amp;
   char  lab[5];
   lab[4] = '\0';

   int nrl = opCal->GetNumberOfLines();
   while (fread(&rec2,sizeof(REC2),1,fp[0]) == 1)
   {  
      adjust(fp[0]);
      if (rec2.type == 16 && (int)rec2.data[0] == 5)
      {  g2 = rec2.data[1];
         continue;
      }
      if (rec2.type == 4)
      {  strncpy(lab,&rec2.label[0],4);
         index = nrl;                // default: not in BENDS.CAL
         for (i=0; i<nrl; i++)
            if (opCal->CompareData(i,lab,rec2.data[0],g2))
            {  index = i;
               break;
            }
         opCal->GetDac(rec2.data[1],index,dac,amp);
         strcpy(buffer,lab);
         strcat(buffer,"  ");
         if (flag == 0)
         {  if (index == nrl)
               sprintf(buff,"not available");
	    else
	       sprintf(buff,String(IDS_7LD_FORM),dac);
            strcat(buffer,buff);
         }
         else
         {  if (index == nrl)
               sprintf(buff,"not available");
            else
               sprintf(buff,String(IDS_71A_FORM),amp);
            strcat(buffer,buff);
         }
         return 1;
      }
   }
   return 0;
}

int TransFile :: NextQuadDac(char *buffer)
{
   static CalibrationData *opCalData;

   if (first)
   {  opCalData = new CalibrationData(String(IDS_QCAL));
      if (!opCalData->GetNumberOfLines())
      {
         PutInfo(IDS_QUADSNF);
         return 0;
      }
      first = FALSE;
   }

   while (NextQuadData(0,opCalData,buffer) == 1)
     return 1;

   delete opCalData;
   first = TRUE;
   return 0;
}

int TransFile :: NextQuadAmp(char *buffer)
{
   static CalibrationData *opCalData;

   if (first)
   {  opCalData = new CalibrationData(String(IDS_QCAL));
      if (!opCalData->GetNumberOfLines())
      {
         PutInfo(IDS_QUADSNF);
         return 0;
      }
      first = FALSE;
   }

   while (NextQuadData(1,opCalData,buffer) == 1)
     return 1;

   delete opCalData;
   first = TRUE;
   return 0;
}

int TransFile :: NextBendDac(char *buffer)
{
   static CalibrationData *opCalData;

   if (first)
   {  opCalData = new CalibrationData(String(IDS_BCAL));
      if (!opCalData->GetNumberOfLines())
      {
         PutInfo(IDS_BENDSNF);
         return 0;
      }
      first = FALSE;
   }

   while (NextBendData(0,opCalData,buffer) == 1)
     return 1;

   delete opCalData;
   first = TRUE;
   return 0;
}

int TransFile :: NextBendAmp(char *buffer)
{
   static CalibrationData *opCalData;

   if (first)
   {  opCalData = new CalibrationData(String(IDS_BCAL));
      if (!opCalData->GetNumberOfLines())
      {
         PutInfo(IDS_BENDSNF);
         return 0;
      }
      first = FALSE;
   }

   while (NextBendData(1,opCalData,buffer) == 1)
     return 1;

   delete opCalData;
   first = TRUE;
   return 0;
}

int TransFile :: ErrorMess(char *buffer)
{
   DispObjectMsg(String(IDS_NYIMPL));
   strcpy(buffer,NullStr);
   return 0;
}

int TransFile::Show(int nr, char *buffer)
{
    return( (this->*f[nr]) (buffer) );
}

VOID TransFile :: SaveParams(REC2 *rec2)
{
    x0    = rec2->cenun[0];
    xp0   = rec2->cenun[1];
    y0    = rec2->cenun[2];
    yp0   = rec2->cenun[3];
    xmax  = rec2->siun[0];
    xpmax = rec2->siun[1];
    ymax  = rec2->siun[2];
    ypmax = rec2->siun[3];
    dl    = rec2->siun[4];
    dpp   = rec2->siun[5];
    r12   = rec2->r12;
    r34   = rec2->r34;
    r56   = rec2->r56;
    r13   = rec2->r13;
    r24   = rec2->r24;
    cx    = rec2->rtmx[0];
    sx    = rec2->rtmx[1];
    dx    = rec2->rtmx[2];
    cpx   = rec2->rtmx[3];
    spx   = rec2->rtmx[4];
    dpx   = rec2->rtmx[5];
    cy    = rec2->rtmx[6];
    sy    = rec2->rtmx[7];
    dy    = rec2->rtmx[8];
    cpy   = rec2->rtmx[9];
    spy   = rec2->rtmx[10];
    dpy   = rec2->rtmx[11];
}

void TransFile :: GetEllipsePar(float &x, float &xp, float &xcorr,
                                float &y, float &yp, float &ycorr,
                                float &d, float &dp)
{
    x     = xmax;
    xp    = xpmax;
    xcorr = r12;
    y     = ymax;
    yp    = ypmax;
    ycorr = r34;
    d     = dx  * dpp;
    dp    = dpx * dpp;
}

void TransFile :: GetEllipsePar2(float &x, float &y, float &corr)
{
    x     = dl;
    y     = dpp;
    corr  = r56;
}

void TransFile :: GetEllipsePar3(float &x, float &y, float &corr1,
                                float &xp, float &yp, float &corr2)
{
    x     = xmax;
    y     = ymax;
    corr1 = r13;
    xp    = xpmax;
    yp    = ypmax;
    corr2 = r24;
}

char *TransFile :: FindElement(float z)
{
   return SearchElement(z, NullStr);
}

char *TransFile :: FindElement(char *labi)
{
   return SearchElement((float)0.0, labi);
}

char *TransFile :: GetNextElement(void)
{
   return SearchElement((float)0.0, NullStr);
}

char *TransFile :: SearchElement(float z, char *labi)
{
   char *p;
   REC2 rec2;
   BOOL condition;
   char lab[5];
   lab[4] = '\0';

   while (fread(&rec2,sizeof(REC2),1,fp[0]) == 1)
   {  adjust(fp[0]);
      if (rec2.type == 16 && rec2.data[0] == 96.0) // it is an Elec. QUAD
         equad = TRUE;
      if (rec2.type == 16 && rec2.data[0] == 95.0) // it is an Magn. QUAD
         equad = FALSE;
      if (rec2.type == 16 && rec2.data[0] == 94.0) // it is an Elec. PRISM
         ebend = TRUE;
      if (rec2.type == 16 && rec2.data[0] == 93.0) // it is an Magn. BEND
         ebend = FALSE;
      if (strlen(labi) > 0)
      {
         strncpy(lab,rec2.label,4);
         p = strtok(lab,Blank);              // cutoff trailing blanks
         condition = (p && strcmp(p,labi) == 0) ? TRUE : FALSE;
      }
      else if (z > 0.0)
         condition = (rec2.along >= z) ? TRUE : FALSE;
      else
         condition = TRUE;
      
      if (condition)
      {
         if (rec2.type == 2 && rec2.data[0] == 1000.0)
            rec2.type = 102; // Quad fringe field
         if (rec2.type == 4 && ebend)
            rec2.type = 104; // electrostatic prism
         if (rec2.type == 5 && equad)
            rec2.type = 105; // electrostatic quad
         GetTitleOfElement((int)rec2.type,title);
         if (strlen(title) == 0L) continue;
         strncpy(lab,&rec2.label[0],4);
         SaveParams(&rec2);
         sprintf(info,String(IDS_TIT_FORM),title,rec2.along,dim[7],lab);
         return info;
      }
   }
   return 0L;
}

char *TransFile :: GetPrevElement(void)
{
   REC2 rec2;
   char lab[5];
   lab[4] = '\0';

   long curpos = ftell(fp[0]);
   long newpos = curpos - 2 * ((long)sizeof(REC2) + 2);  // backspace
   Rewind();

   while (fread(&rec2,sizeof(REC2),1,fp[0]) == 1)
   {  
      adjust(fp[0]);
      if (rec2.type == 16 && rec2.data[0] == 96.0) // it is an Elec. QUAD
         equad = TRUE;
      if (rec2.type == 16 && rec2.data[0] == 95.0) // it is an Magn. QUAD
         equad = FALSE;
      if (rec2.type == 16 && rec2.data[0] == 94.0) // it is an Elec. PRISM
         ebend = TRUE;
      if (rec2.type == 16 && rec2.data[0] == 93.0) // it is an Magn. BEND
         ebend = FALSE;
      if (ftell(fp[0]) == newpos)
	 break;
   }

   if (fseek(fp[0],newpos,0) == 0)
   {  while (fread(&rec2,sizeof(REC2),1,fp[0]) == 1)
      {  
         adjust(fp[0]);
         if (rec2.type == 2 && rec2.data[0] == 1000.0)
            rec2.type = 102; // Quad fringe field
         if (rec2.type == 4 && ebend)
            rec2.type = 104; // electrostatic prism
         if (rec2.type == 5 && equad)
            rec2.type = 105; // electrostatic quad
         GetTitleOfElement((int)rec2.type,title);
         if (strlen(title) == 0L)
         {  curpos = ftell(fp[0]);
            newpos = curpos - 2 * ((long)sizeof(REC2) + 2);  // backspace
            if (newpos < 0) return 0L;
            if (fseek(fp[0],newpos,0) == 0)
               continue;
         }
         strncpy(lab,&rec2.label[0],4);
         SaveParams(&rec2);
         sprintf(info,String(IDS_TIT_FORM),title,rec2.along,dim[7],lab);
         return info;
      }
   }
   return 0L;
}

BOOL TransFile :: GetElement(float z, char *zst, char *tit, char *labi,
                             char *l, char *B, char *a)
{
   REC2 rec2;
   char lab[5];
   lab[4] = '\0';

   while (fread(&rec2,sizeof(REC2),1,fp[0]) == 1)
   {  
      adjust(fp[0]);
      if (rec2.type == 16 && rec2.data[0] == 96.0) // it is an Elec. QUAD
         equad = TRUE;
      if (rec2.type == 16 && rec2.data[0] == 95.0) // it is an Magn. QUAD
         equad = FALSE;
      if (rec2.type == 16 && rec2.data[0] == 94.0) // it is an Elec. PRISM
         ebend = TRUE;
      if (rec2.type == 16 && rec2.data[0] == 93.0) // it is an Magn. BEND
         ebend = FALSE;
      if (rec2.along >= z)
      {  if (rec2.type == 2 && rec2.data[0] == 1000.0)
            rec2.type = 102; // Quad fringe field
         if (rec2.type == 4 && ebend)
            rec2.type = 104; // electrostatic prism
         if (rec2.type == 5 && equad)
            rec2.type = 105; // electrostatic quad
         GetTitleOfElement((int)rec2.type,title);
         if (strlen(title) == 0L) continue;

         strncpy(lab,&rec2.label[0],4);
         SaveParams(&rec2);

         strcpy(ilabel,lab);

         fpar1 = rec2.data[0];
         fpar2 = rec2.data[1];
         fpar3 = rec2.data[2];
         itype = int(rec2.type);

         sprintf(tit,  String(IDS_STR_FORM), title);
         sprintf(zst,  String(IDS_ZEX_FORM), rec2.along,   dim[7]);
         sprintf(labi, String(IDS_STR_FORM), lab);
         sprintf(l,    String(IDS_LEX_FORM), rec2.data[0], dim[7]);

         if (rec2.type == 4  ||
             rec2.type == 5  ||
             rec2.type == 18 ||
             rec2.type == 19 ||
             rec2.type == 21 )
             sprintf(B, String(IDS_BEX_FORM), rec2.data[1], dim[8]);
         else 
             sprintf(B,NullStr);

         if (rec2.type == 5  ||
             rec2.type == 18 ||
             rec2.type == 21 )
             sprintf(a, String(IDS_AEX_FORM), rec2.data[2], dim[0]);
         else if (rec2.type == 4)
             sprintf(a, String(IDS_NEX_FORM), rec2.data[2]);
         else
             sprintf(a,NullStr);

         Rewind();
         return TRUE;
      }
   }

// z not found

   sprintf(tit,  "-");
   sprintf(zst,  String(IDS_ZGTHZMAX));
   sprintf(labi, String(IDS_LABEMPTY));

   sprintf(l,    String(IDS_LUNKNOWN));
   sprintf(B,    String(IDS_BUNKNOWN));
   sprintf(a,    String(IDS_AUNKNOWN));

   strcpy(ilabel,String(IDS_BLANK_LB));
   fpar1 = (float)0.0;
   fpar2 = (float)0.0;
   fpar3 = (float)0.0;
   itype = 0;

   Rewind();
   return FALSE;
}

VOID TransFile :: GetParameters(int &t, float &p1, float &p2,
                                                   float &p3, char *l)
{
   t  = itype;
   p1 = fpar1;
   p2 = fpar2;
   p3 = fpar3;
   for (int i=0; i<4; i++)
     if (ilabel[i] == ' ')
     {  ilabel[i] = '\0';
        break;
     }
   strcpy(l,ilabel);
}

char *TransFile :: GetBeamInfoData(int index)
{
   switch(index)
   {  case 0:
        strcpy(info,  String(IDS_SIGMA));
        return info;
      case 1:
        sprintf(info, String(IDS_CENTERS),
                      x0,dim[0],xp0,dim[1],y0,dim[2],yp0,dim[3]);
        return info;
      case 2:
        sprintf(info, String(IDS_HPARS),
                      xmax,dim[0],xpmax,dim[1],r12);
        return info;
      case 3:
        sprintf(info, String(IDS_VPARS),
                      ymax,dim[2],ypmax,dim[3],r34);
        return info;
      case 4:
        sprintf(info, String(IDS_1_TWISS),dpp,dim[5]);
        return info;
      case 5:
        sprintf(info, String(IDS_2_TWISS));
        return info;
      case 6:
	epsx  = xmax * xpmax * (float)sqrt(fabs(1.0-r12*r12));
	betax = (xmax * xmax) / epsx;
	epsy  = ymax * ypmax * (float)sqrt(fabs(1.0-r34*r34));
	betay = (ymax * ymax) / epsy;
        sprintf(info, String(IDS_3_TWISS),
                      (float) -tan(asin(r12)), betax, dim[7],
                      (float) -tan(asin(r34)), betay, dim[7]  );
        return info;
      case 7:
        strcpy(info,  String(IDS_TFORM1));
        return info;
      case 8: 
        strcpy(info,  String(IDS_HORZVERT));
        return info;
      case 9: 
        sprintf(info, String(IDS_R1_FORM),
                      cx,sx,dx,cy,sy,dy);
        return info;
      case 10:
        sprintf(info, String(IDS_R1_FORM),
                      cpx,spx,dpx,cpy,spy,dpy);
        return info;
      default:
        return 0L;
   }
}

char *TransFile :: GetBeamInfoElem(int index)
{
   switch(index)
   {  case 0:
        sprintf(info,String(IDS_x_FORM),  xmax,dim[0]);
        return info;
      case 1:
        sprintf(info,String(IDS_xp_FORM), xpmax,dim[1]);
        return info;
      case 2:
        sprintf(info,String(IDS_r12_FORM),r12);
        return info;
      case 3:
        sprintf(info,String(IDS_y_FORM),  ymax,dim[0]);
        return info;
      case 4:
        sprintf(info,String(IDS_yp_FORM), ypmax,dim[1]);
        return info;
      case 5:
        sprintf(info,String(IDS_r34_FORM),r34);
        return info;
      case 6:
        sprintf(info,String(IDS_dpp_FORM),dpp,dim[5]);
        return info;

      case 7:
        sprintf(info,String(IDS_x0_FORM), x0,dim[0]);
        return info;
      case 8:
        sprintf(info,String(IDS_x0p_FORM),xp0,dim[1]);
        return info;
      case 9:
        sprintf(info,String(IDS_y0_FORM), y0,dim[0]);
        return info;
      case 10:
        sprintf(info,String(IDS_y0p_FORM),yp0,dim[1]);
        return info;


      case 11:
        sprintf(info,String(IDS_R11_FORM),cx);
        return info;
      case 12:
        sprintf(info,String(IDS_R12_FORM),sx,dim[0],dim[1]);
        return info;
      case 13:
        sprintf(info,String(IDS_R16_FORM),dx,dim[0],dim[5]);
        return info;
      case 14:
        sprintf(info,String(IDS_R21_FORM),cpx,dim[1],dim[0]);
        return info;
      case 15:
        sprintf(info,String(IDS_R22_FORM),spx);
        return info;
      case 16:
        sprintf(info,String(IDS_R26_FORM),dpx,dim[1],dim[5]);
        return info;

      case 17:
        sprintf(info,String(IDS_R33_FORM),cy);
        return info;
      case 18:
        sprintf(info,String(IDS_R34_FORM),sy,dim[2],dim[3]);
        return info;
      case 19:
        sprintf(info,String(IDS_R36_FORM),dy,dim[2],dim[5]);
        return info;
      case 20:
        sprintf(info,String(IDS_R43_FORM),cpy,dim[3],dim[2]);
        return info;
      case 21:
        sprintf(info,String(IDS_R44_FORM),spy);
        return info;
      case 22:
        sprintf(info,String(IDS_R46_FORM),dpy,dim[3],dim[5]);
        return info;

      default:
        return (Blank);
   }   
}
///////////////////////////////// EOF /////////////////////////////////////////
