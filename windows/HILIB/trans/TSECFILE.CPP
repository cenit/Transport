///////////////////////////////////////////////////////////////////////////////
////////////////////// class TransSecondFile implementation ///////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

TransSecondFile :: TransSecondFile(char *filename, char *savefile, char *for001)
                 : TitleOfRun(for001)
{
    long curpos, len;
    FILE *fhand;

    NumberOfOpenFiles = 0;
    fhand = fopen(filename,"rb");   // open binary file (FORTRAN)
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
          {  fhand = fopen(savefile,"rb");     // .. then also open
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
       nr = (int)(len - sizeof(REC3) - 2) / (sizeof(REC4) + 2);

       if (nr > 0)
       {  GetDimensions();
          if (!CheckDimensions(dim))
             nr = 0;
       }
       else
          nr = 0;
    }

    line   = new REC4;
    info   = new char[40];
    title  = new char[20];
    indval = new INDVAL;
    caption = new char[100];

    char indx[80];
    strcpy(indx,String(IDS_INDEX));
    for (int m=0; m<21; m++)
    {  jk[m] = new char[3];
       strncpy(jk[m],&indx[3*m],2);
       jk[m][2] = '\0';
    }
}

TransSecondFile :: ~TransSecondFile(void)
{
   if (nr > 0)
      for (int j=0; j<11; j++)
        delete []dim[j]; // [] found with Bounds Checker
   for (int i=0; i<NumberOfOpenFiles; i++)
      fclose(fp[i]);
   delete line; 
   delete []info;  // [] found with Bounds Checker
   delete []title;
   delete []caption;
   delete indval;
   for (int m=0; m<21; m++)
       delete []jk[m]; // [] found with Bounds Checker
}

int TransSecondFile :: GetNumberOfElements(void)
{
   return nr;
}

int TransSecondFile :: GetNumberOfOpenFiles(void)
{
   return NumberOfOpenFiles;
}

FILE *TransSecondFile :: GetFilePointer(int fn)
{
   return (fp[fn]);
}

void TransSecondFile :: Rewind(void)
{
   for (int i=0; i<NumberOfOpenFiles; i++)
      fseek(fp[i], (long)sizeof(REC3) + 2, 0);
}

void TransSecondFile :: GetDimensions(void)
{
   REC3 rec3;
   char *p;
   char lab[5];
   lab[4] = '\0';

   fread(&rec3,sizeof(REC3),1,fp[0]);   // read first record of file
   for (int j=0; j<11; j++)
   {  strncpy(lab,&rec3.xdime[j][0],4);
      p = strtok(lab," ");              // cutoff trailing blanks
      dim[j] = new char[5];
      strcpy(dim[j],p);
   }
   adjust(fp[0]);
}

char *TransSecondFile :: FindElement(float z)
{
   return SearchElement(z, "");
}

char *TransSecondFile :: FindElement(char *labi)
{
   return SearchElement((float)0.0, labi);
}

char *TransSecondFile :: GetNextElement(void)
{
   return SearchElement((float)0.0, "");
}

char *TransSecondFile :: SearchElement(float z, char *labi)
{
   char *p;
   BOOL condition;
   char lab[5];
   lab[4] = '\0';

   while (fread(line,sizeof(REC4),1,fp[0]) == 1)
   {  adjust(fp[0]);
      if (strlen(labi) > 0)
      {
         strncpy(lab,line->label,4);
         p = strtok(lab," ");              // cutoff trailing blanks
         condition = (p && strcmp(p,labi) == 0) ? TRUE : FALSE;
      }
      else if (z > (float)0.0)
         condition = (line->along >= z) ? TRUE : FALSE;
      else
         condition = TRUE;
      if (condition)
      {
         GetTitleOfElement((int)line->type,title);
         sprintf(info,String(IDS_FIN_FORM),title,line->along,dim[7],line->label);
         return info;
      }
   }
   return 0L;
}

char *TransSecondFile :: GetPrevElement(void)
{
   long curpos = ftell(fp[0]);
   long newpos = curpos - 2 * ((long)sizeof(REC4) + 2);  // backspace
   if (fseek(fp[0],newpos,0) == 0)
   {  if (fread(line,sizeof(REC4),1,fp[0]) == 1)
      {
         adjust(fp[0]);
         GetTitleOfElement((int)line->type,title);
         sprintf(info,String(IDS_FIN_FORM),title,line->along,dim[7],line->label);
         return info;
      }
   }
   return 0L;
}

INDVAL *TransSecondFile :: GetSortedData(int index)
{
   static int swap[4] = {2,3,0,1};
   float aux, tmax;
   char  buffer[11];
   int   inds;

   strcpy(indval->indexline,"");
   strcpy(indval->valueline,"");

   int rot    = (int)line->totrot;
   float sign = (float)1.0;
   int ii     = index;

// back rotation (only for +-90 and +- 180 degrees)

   if (index < 4)
   {  if (rot == 180 || rot == -180) sign = (float)-1.0;
      if (rot ==  90 && index < 2)   sign = (float)-1.0;
      if (rot == -90 && index > 1)   sign = (float)-1.0;
      if (rot ==  90 || rot ==  -90) ii = swap[index];
   }

// find 6 biggest elements in decending order 

   for (int i=0; i<6; i++)
   {  tmax = (float)0.0;
      for (int j=0; j<21; j++)
      {  aux = line->srmtx[ii][j] * sign;
         if ( aux * aux > tmax * tmax )
         {  tmax = aux;
            inds = j;
         }
      }
      if (tmax != (float)0.0)
      {  sprintf(buffer,String(IDS_IDX_FORM),index+1,jk[inds]);
         strcat(indval->indexline,buffer);
         sprintf(buffer,String(IDS_2ME_FORM),tmax);
         strcat(indval->valueline,buffer);
         line->srmtx[index][inds] = (float)0.0;
      }
   }
   if (strlen(indval->valueline) > 0)
      return indval;
   else
      return 0L;          // zero length strings (all elements zero)
}

VOID TransSecondFile :: GetParLine(char *parline)
{
   char caption[80];
   int  n, nr, inf[4];

   for (n=0; n<4; n++)
     strcpy(&tinf[n][0],String(IDS_T111));

        sscanf(parline,String(IDS_5S_FORM),
               &caption[0],&tinf[0][1],&tinf[1][1],&tinf[2][1],&tinf[3][1]);
   nr = sscanf(parline,String(IDS_S4D_FORM),
                       &caption[0],&inf[0],&inf[1],&inf[2],&inf[3]);
   ncurves = nr - 1;
   {  for (n=0; n<ncurves; n++)
      {  i[n] = inf[n] / 100;
         j[n] = (inf[n] - i[n] * 100) / 10;
         k[n] = inf[n] - i[n] * 100 - j[n] * 10;
         jkind[n] = ((k[n]-1) * k[n]) / 2 + j[n] - 1;
      }
   }
}

char * TransSecondFile :: GetTijkLabel(int n)
{
   return (tinf[n]);
}

int TransSecondFile :: GetMatrixElements(int nrf, int &nn, float &ymax,
                       float z[],float t1[], float t2[], float t3[], float t4[])
{
   int swap[4] = {2,3,0,1};

   Rewind();
   int ind = 0;
   ymax = (float)0.0;
   FILE *fp = GetFilePointer(nrf);
   while (fread(line,sizeof(REC4),1,fp) == 1)
   {
      adjust(fp);
      if ((int)line->type == 20)
         continue;
      z[ind] = line->along;
      int rot    = (int)line->totrot;
      for (int n=0; n<ncurves; n++)
      {
         float sign = (float)1.0;
         int index  = i[n] - 1;
         int ii     = index;
         if (index < 4)
         {  if (rot == 180 || rot == -180) sign = (float)-1.0;
            if (rot ==  90 && index < 2)   sign = (float)-1.0;
            if (rot == -90 && index > 1)   sign = (float)-1.0;
            if (rot ==  90 || rot ==  -90) ii = swap[index];
         }
         int jk = jkind[n];
         float aux = line->srmtx[ii][jk] * sign;
         if (fabs(aux) > ymax)
            ymax = (float)fabs(aux);
         switch (n)
         {  case 0: t1[ind] = aux; break;
            case 1: t2[ind] = aux; break;
            case 2: t3[ind] = aux; break;
            case 3: t4[ind] = aux; break;
            default:               break;
         }
      }
      ind++;
   }
   ymax = (float)1.25 * ymax;
   nn = ncurves;
   return ind;
}

///////////////////////////////// EOF /////////////////////////////////////////
