///////////////////////////////////////////////////////////////////////////////
//////////////////////// class TitleOfRun implementation //////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

TitleOfRun :: TitleOfRun(char *filename)
            : Object(33)
{
   char *ptr, temp[100];

   FILE *fin = fopen(filename,"r");
   if (fin)
   {  fscanf(fin,String(IDS_CR_FORM),&caption[0]); // read first line
      fclose(fin);
   }

// get rid of leading and trailing /

   if ((ptr = strstr(caption,"/")) != 0L)
   {  strcpy(temp, ptr + 1);
      if ((ptr = strstr(temp,"/")) != 0L)
         *ptr = '\0';
      strcpy(caption,temp);
   }

// get rid of optinal leading *PLOT*

   if ((ptr = strstr(caption,String(IDS_PLOTST))) != 0L)
   {  strcpy(temp, ptr + 6);
      strcpy(caption,temp);
   }
}

TitleOfRun :: ~TitleOfRun(void)
{
}

char *TitleOfRun :: GetTitleOfRun(void)
{
   return caption;
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////// class TrOutFile implementation ///////////////////////
///////////////////////////////////////////////////////////////////////////////

TrOutFile :: TrOutFile(char *FileName, char *for001)
           : TitleOfRun(for001)
{
    fp = fopen(FileName,"r");

    FindPlotSection();

    char ttl[80];
    strcpy(ttl,String(IDS_TIT));
    for (int m=0; m<17; m++)
    {  titel[m] = new char[4];
       strncpy(titel[m],&ttl[4*m],3);
       titel[m][3] = '\0';
    }
    pos = 0;
}

TrOutFile :: ~TrOutFile(void)
{
    fclose(fp);

    for (int m=0; m<17; m++)
        delete []titel[m]; // [] found with Bounds Checker
}

char *TrOutFile :: FindElement(float z)
{
   if (pos > 999) return 0L;
   rem[pos++] = ftell(fp);
   return SearchElement(z, "");
}

char *TrOutFile :: FindElement(char *Label)
{
   if (pos > 999) return 0L;
   rem[pos++] = ftell(fp);
   return SearchElement((float)0.0, Label);
}

char *TrOutFile :: GetNextElement(void)
{
   if (pos > 999) return 0L;
   rem[pos++] = ftell(fp);
   return SearchElement((float)0.0, "");
}

char *TrOutFile :: GetPrevElement(void)
{
   if (pos <= 0) return 0L;
   pos--;
   fseek(fp,rem[pos],SEEK_SET);
   return SearchElement((float)0.0, "");
}

char *TrOutFile :: SearchElement(float zz, char *Lab)
{
   int   n;
   BOOL  condition;
   char  test1[14];
   char  dim1[20], dim2[20];
   float z, dum1, dum2, dum3;
   char  label[5];
   char  Label[5];
   label[4] = '\0';
   Label[4] = '\0';

   int m;
   char  coord[20], dimx[20];
   float lay1,lay2,lay3;

   strcpy(Label,"    ");
   strncpy(Label,Lab,strlen(Lab)); 
   strcpy(test1,String(IDS_TFORM1));

   while ( GetNextLine() )
   {
       condition = FALSE;
       for (int i=0; i<17; i++)
       {
           if (strncmp(&line[1],titel[i],3) == 0)
           {
               condition = TRUE;
               break;
           }
       }
       if (condition)
       {
           strncpy(label,&line[33],4);  

           do {
                 if ( !GetNextLine() )
                    return NULL;
                 n = sscanf(line,String(IDS_FSFFSF),
                                 &z,dim1,&dum1,&dum2,dim2,&dum3);
                 m = sscanf(line,"%f %s %s %f %f %f %s %f %f %s %f",
                                 &z,dim1,coord,&lay1,&lay2,&lay3,dimx,&dum1,&dum2,dim2,&dum3);
              } while (n != 5 && m != 10);

           if (strlen(Lab) > 0)
              condition = (strcmp(Label,label) == 0) ? TRUE : FALSE;
           else if (zz > 0.0)
              condition = (z >= zz) ? TRUE : FALSE;
           else
              condition = TRUE;

           if ( condition )
           {
               strcpy(sigma1,&line[70]);
               GetNextLine();
               strcpy(sigma2,&line[70]);
               GetNextLine();
               strcpy(sigma3,&line[70]);
               GetNextLine();
               strcpy(sigma4,&line[70]);
               GetNextLine();
               strcpy(sigma5,&line[70]);
               GetNextLine();
               strcpy(sigma6,&line[70]);

               long position = ftell(fp);
               GetNextLine();
               strncpy(test1,&line[1],13);
               if ( strcmp(test1,String(IDS_TFORM1)) == 0 )
               {
                  strcpy(transform1,"      ");
                  strcat(transform1,String(IDS_TFORM1));
                  GetNextLine();
                  strcpy(transform2,&line[9]);
                  GetNextLine();
                  strcpy(transform3,&line[9]);
                  GetNextLine();
                  strcpy(transform4,&line[9]);
                  GetNextLine();
                  strcpy(transform5,&line[9]);
                  GetNextLine();
                  strcpy(transform6,&line[9]);
                  GetNextLine();
                  strcpy(transform7,&line[9]);
               }
               else
               {
                  strcpy(transform1,"");
		  fseek(fp,position,SEEK_SET);
	       }

               AnsiLower(dim1);
               sprintf(line,String(IDS_SIG_FORM),z,dim1,label);
               return line;
           }
       }
   }
   return NULL;     
}

char *TrOutFile :: GetSigmaLine(int i)
{
   switch (i)
   {
      case 0: return sigma1;
      case 1: return sigma2;
      case 2: return sigma3;
      case 3: return sigma4;
      case 4: return sigma5;
      case 5: return sigma6;
      default: return NULL;
   }
}

char *TrOutFile :: GetMatrixLine(int i)
{
   switch (i)
   {
      case 0: return (strlen(transform1)!=0) ? transform1 : NULL;
      case 1: return (strlen(transform1)!=0) ? transform2 : NULL;
      case 2: return (strlen(transform1)!=0) ? transform3 : NULL;
      case 3: return (strlen(transform1)!=0) ? transform4 : NULL;
      case 4: return (strlen(transform1)!=0) ? transform5 : NULL;
      case 5: return (strlen(transform1)!=0) ? transform6 : NULL;
      case 6: return (strlen(transform1)!=0) ? transform7 : NULL;
      default: return NULL;
   }
}

BOOL TrOutFile :: GetNextLine(void)
{
   if (fp)
   {
      while( !feof(fp) )
      {
         if (fscanf(fp,String(IDS_CR_FORM),line))
         {
             fgetc(fp);
             return TRUE;
         }
         else
         {
             strcpy(line," ");
             fgetc(fp);
             return TRUE;
         }
      }
   }
   return FALSE;
}

VOID TrOutFile :: FindPlotSection(void)
{
   char test[7];

   strcpy(test,String(IDS_PLOTST));

   if (fp)
   {
      while( !feof(fp) )
      {
         if (fscanf(fp,String(IDS_CR_FORM),line))
         {
             fgetc(fp);
             strncpy(test,&line[1],6);
             if (strcmp(test,String(IDS_PLOTST)) == 0)
                return;
         }
         else
             fgetc(fp);
      }
   }
}

VOID TrOutFile :: Rewind(void)
{
   fseek(fp,0L,0);  // BOF
   FindPlotSection();
}

///////////////////////////////// EOF /////////////////////////////////////////
