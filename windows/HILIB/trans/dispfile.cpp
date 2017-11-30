///////////////////////////////////////////////////////////////////////////////
////////////////////// implementation of class DisplayFile ////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

UINT names[] = { IDS_ZMIN, IDS_ZMAX, IDS_XMPL, IDS_YMPK, IDS_APFAC,
                 IDS_SPEZ, IDS_SPEZ2, IDS_APLAB, IDS_STEER, IDS_DISP,
                 IDS_YDISP, IDS_RAX, IDS_RAY, IDS_MEASX, IDS_MEASY,
                 IDS_MEASD, IDS_CSX, IDS_CSY, IDS_BUNL };

DisplayFile :: DisplayFile(char *filename) : FileBuff(filename)
{
   int i;

   p[0] = (float)0.0;
   p[1] = (float)10.0;
   p[2] = (float)100.0;
   p[3] = (float)100.0;
   for (i=4; i<11; i++)
     p[i] = (float)1.00;
   for (i=11; i<19; i++)
     p[i] = (float)0.00;
}

DisplayFile :: ~DisplayFile(VOID)
{
}

DISPAR *DisplayFile :: GetParameters(void)
{
   char  line[50], item[10];
   float par;

   if (TellNumberOfLines() == 0)
      return NULL;

   while (GetNextLine(line) != 0)
      if (sscanf(line,String(IDS_SF_FORM),&item[0],&par) == 2)
         for (int i=0; i<19; i++)
	 {
           if (strcmp(item,String(names[i]))  == 0)
           {
	      p[i] = par;
///////
//	if (i == 18)
//	{
//	char buffer[100];
//	sprintf(buffer,"%d %s %f", i,String(names[i]),par);
//	MessageBox(NULL,buffer,"DispFile::GetParameters",MB_OK);
//	}
///////
	   }
	 }
   return (&dispar);
}

float DisplayFile :: GetValue(int nr)
{
///////
//	if (nr == 18)
//	{
//	char buffer[100];
//	sprintf(buffer,"%d %f",nr,p[nr]);
//	MessageBox(NULL,buffer,"DispFile::GetValue",MB_OK);
//	}
///////
   return(p[nr]);
}

void DisplayFile :: SetValue(float val, int nr)
{
   p[nr] = val;
}

BOOL DisplayFile :: WriteFile(char *filename)
{
   FILE *fp = fopen(filename,"w+");
   if (fp)
   {  for (int i=0; i<19; i++)
      {
         fprintf(fp,String(IDS_DSP_FORM),String(names[i],1),p[i]);
///////
//        if (i == 18)
//	{
//	char buffer[100];
//	sprintf(buffer,String(IDS_DSP_FORM),String(names[i],1),p[i]);
//	MessageBox(NULL,buffer,"DispFile::WriteFile",MB_OK);
//	}
///////
     }
      fclose(fp);
      return TRUE;
   }
   return FALSE;
}

/////////////////////////////////////// EOF ///////////////////////////////////
