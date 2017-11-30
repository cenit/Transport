
///////////////////////////////////////////////////////////////////////////////
////////////////////// Implemention of class TransInFile //////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

#define MAXB   255  // same in FileBuff
#define NLINES 1000

TransInFile :: TransInFile(char *filename) : FileBuff(filename)
{
    char text[MAXB+1];
    int  i;

    Turtle = FALSE;
    index = 1;

    line  = (char **) new long[NLINES];
    title = (char **) new long[NLINES];
    for (i=0; i<NLINES; i++)
    {
       line[i]  = new char[133];
       title[i] = new char[31];
    }

    t = (char **) new long[20];
    for (i=0; i<20; i++)
    {
	t[i] = new char[21];
	strcpy(t[i],"");
    }

    strcpy(FileName,filename);
    nlines = TellNumberOfLines();
    if (nlines > NLINES)
       nlines = NLINES -5;

    update = TRUE;
    multscat = FALSE;
    angle  = FALSE;
    equad1 = FALSE;
    ebend1 = FALSE;
    int k  = 0;

    if (nlines > 0)
    {
       int j, i=0;
       while (i < nlines)
       {
         if (GetNextLine(text))
         {
	    int leng = strlen(text);
	    BOOL titlin = FALSE;
	    for (j=0; j<leng; j++)
	    {
		if ( text[j] == ' ')
		   ;
	        else if ((text[j] == '/') || (text[j] == '\'') || (text[j] == '\"'))
		{
		    titlin = TRUE;
		    break;
	        }
		else
		{
		    titlin = FALSE;
		    break;
		}
	    }
	    int pos1 = -1;
	    int pos2 = -1;
	    if (!titlin)
	    {
		for (j=0; j<leng; j++)
		   if (text[j] != ' ')                 // first non blank
		   {
		       pos1 = j;
		       break;
		   }
		for (j=0; j<leng; j++)
		   if (text[j] == '(')                 // begin of comment
		   {
		       pos2 = j;
		       break;
		   }
	    }

	    strcpy(&line[k+1][0],"dummy"); // for MakeTitle

	    if (pos2 == -1)     // no comment on this line
	    {
		strncpy(line[k],text,132);
		line[k][132] = '\0';
		MakeTitle(line[k],title[k],FALSE,FALSE);
	    }
	    else if ((pos2 != -1) && (pos1 == pos2)) // only comment line
	    {
		strncpy(line[k],text,132);
		line[k][132] = '\0';
		MakeTitle(line[k],title[k],FALSE,FALSE);
	    }
	    else	                              // split in 2 lines
	    {
		strncpy(line[k],text,pos2);           // first line
		line[k][pos2+1] = '\0';
		MakeTitle(line[k],title[k],FALSE,FALSE);
		if (k < NLINES-5)	      // if enough space ...
		{
		   k++;
		   strncpy(&line[k][0],"_",1);
		   strncpy(&line[k][1],&text[pos2],131);  // ... second line
		   line[k][132] = '\0';
		   MakeTitle(line[k],title[k],FALSE,FALSE);
		}
	    }
	    i++;
	    k++;
	    index = k;
         }
       }
    }
    nlines = k;
}

int  TransInFile :: TellNumberOfLinesEx()
{
    return nlines;
}

int  TransInFile :: GetNew(void)
{
   int nlines;

   if (!Turtle)
   {
     update = TRUE;
     angle  = FALSE;
     multscat = FALSE;
     strcpy(line[0]," /Test/");
     strcpy(line[1]," 0");
     strcpy(line[2]," 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 /BEAM/ ;");
     strcpy(line[3]," 3.0 1.0 ;");
     strcpy(line[4]," SENTINEL");
     strcpy(line[5]," /*PLOT*/");
     strcpy(line[6]," -1");
     strcpy(line[7]," SENTINEL");
     strcpy(line[8]," SENTINEL");
     nlines = 9;
   }
   else
   {
     update = FALSE;
     angle  = FALSE;
     multscat = FALSE;
     strcpy(line[0]," /Test/");
     strcpy(line[1]," 10000");
     strcpy(line[2]," 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 /BEAM/ ;");
     strcpy(line[3]," 3.0 1.0 ;");
     strcpy(line[4]," 50.0 1.0 -2.0 2.0 0.1 ;");
     strcpy(line[5]," SENTINEL");
     strcpy(line[6]," SENTINEL");
     nlines = 7;
   }
   for (int i=0; i<nlines; i++)
   {
      MakeTitle(line[i],title[i],FALSE,FALSE);
   }
   return nlines;
}

int TransInFile :: Title2Typecode(char *title)
{
   char temp[100], txt[100];

   strcpy(temp, title);
   char *p = strtok(temp," ");
   if (p)
      strcpy(txt,p);
   else
      return -4;

   if (strcmp(txt,"Fragment") == 0)
	return -4;
   else if (strcmp(txt,"COMMENT") == 0)
	return -3;
   else if (strcmp(txt,"SENTINEL") == 0)
	return -2;
   else if (strcmp(txt,"INDICATOR") == 0)
	return -1;
   else if (strcmp(txt,"Number") == 0)
	return -1;
   else if (strcmp(txt,"TITLE") == 0)
	return 0;
   else if (strcmp(txt,"Beam") == 0)
	return 1;
   else if (strcmp(txt,"Add") == 0)
	return 1;
   else if (strcmp(txt,"Multiple") == 0)
	return 1;
   else if (strcmp(txt,"Pole") == 0)
	return 2;
   else if (strcmp(txt,"Drift") == 0)
	return 3;
   else if (strcmp(txt,"Bending") == 0)
	return 4;
   else if (strcmp(txt,"ElstatPrism") == 0)
	return 4;
   else if (strcmp(txt,"Quadrupole") == 0)
	return 5;
   else if (strcmp(txt,"ElstatQuad") == 0)
	return 5;
   else if (strcmp(txt,"1-dim-Slit") == 0)
	return 6;
   else if (strcmp(txt,"2-dim-Slit") == 0)
	return 6;
   else if (strcmp(txt,"Update") == 0)
	return 6;
   else if (strcmp(txt,"Shift") == 0)
	return 7;
   else if (strcmp(txt,"Tolerance") == 0)
	return 8;
   else if (strcmp(txt,"Repeat") == 0)
	return 9;
   else if (strcmp(txt,"Fit") == 0)
	return 10;
   else if (strcmp(txt,"Accelerator") == 0)
	return 11;
   else if (strcmp(txt,"Correlation") == 0)
	return 12;
   else if (strcmp(txt,"I/O") == 0)
	return 13;
   else if (strcmp(txt,"Matrix") == 0)
	return 14;
   else if (strcmp(txt,"Units") == 0)
	return 15;
   else if (strcmp(txt,"Special") == 0)
	return 16;
   else if (strcmp(txt,"Param") == 0)
	return 16;
   else if (strcmp(txt,"Second") == 0)
	return 17;
   else if (strcmp(txt,"Sextupole") == 0)
	return 18;
   else if (strcmp(txt,"Solenoid") == 0)
	return 19;
   else if (strcmp(txt,"Rotation") == 0)
	return 20;
   else if (strcmp(txt,"Wien") == 0)
	return 21;
   else if (strcmp(txt,"Space") == 0)
	return 22;
   else if (strcmp(txt,"RF") == 0)
	return 23;
   else if (strcmp(txt,"Load") == 0)
	return 24;
   else if (strcmp(txt,"Combination") == 0)
	return 25;
   else if (strcmp(txt,"Octupole") == 0)
	return 26;
   else if (strcmp(txt,"Separator") == 0)
	return 27;
   else if (strcmp(txt,"Solenoid2") == 0)
	return 28;
   else if (strcmp(txt,"Jump") == 0)
	return 29;
   else if (strcmp(txt,"dE/dx") == 0)
	return 30;
   else if (strcmp(txt,"Limits") == 0)
	return 50;
   else if (strcmp(txt,"1-dim") == 0)
	return 50;
   else if (strcmp(txt,"x-Axis") == 0)
	return 51;
   else if (strcmp(txt,"y-Axis") == 0)
	return 52;
   else if (strcmp(txt,"Flag") == 0)
	return 53;
   else if (strcmp(txt,"Decay") == 0)
	return 60;
   else
	return -4;
}

int TransInFile :: MakeTitle(char *line, char *title, BOOL equad, BOOL ebend)
{
   char txt[200], tst[200];
   char label[10];
   int  i, n, typecode = 0, par;

   strcpy(title,"????");

   while (*line == ' ') line++;  // remove all leading blanks
   strcpy(txt,line);

   for (i=strlen(txt)-1; i>0; i--)  // remove all trailing blanks
   {
      if (txt[i] == ' ')
	txt[i] = '\0';
      else
	break;
   }

   if (strcmp(txt,"SENTINEL") == 0)
   {
      strcpy(title,"SENTINEL");
      typecode = -2;
      return typecode;
   }
   else if ( (txt[0] == '/') || (txt[0] == '\'') || (txt[0] == '\"') )
   {
      strcpy(title,"TITLE");
      typecode = 0;
      return typecode;
   }
//   else if ( (strlen(txt) == 1) || (strlen(txt) == 2) )
//   {
//      strcpy(title,"INDICATOR");
//      typecode = -1;
//      return typecode;
//   }
   else if (txt[0] == '(')
   {
      strcpy(title,"COMMENT");
      typecode = -3;
      return typecode;
   }
   else if ((txt[0] == '_') && (txt[1] == '('))
   {
      strcpy(title,"COMMENT");
      typecode = -3;
      return typecode;
   }
   else if (strlen(txt) == 0)
   {
      strcpy(title,"");
      typecode = -4;
      return typecode;
   }
   else
   {
      n = sscanf(txt,"%d %s",&par,tst);
      if (n == 1)
      {
	if (index <= 5)  // position in file at the very begining
	{
	   if (par > 0)  // if positive, then "number of rays" --> Turtle
	   {
		strcpy(title,"Number of Rays");
		Turtle = TRUE;
		typecode = -1;
		update = FALSE;
		return typecode;
	   }
	   else  // if <= 0, then indicator --> Transport
	   {
		strcpy(title,"INDICATOR");
		typecode = -1;
		return typecode;
	   }
	}
	else
	{
	   strcpy(title,"INDICATOR");
	   typecode = -1;
	   return typecode;
	}
      }
      n = sscanf(txt,"%s %d",tst,&par);
      if ( (n == 1) || (n==2) )
      {
	char *ptr;
	ptr = strstr(tst,".");
	if (ptr)
	   *ptr = '\0';   // cut off .xxx
	sscanf(tst,"%d",&typecode);
        typecode = abs(typecode);

	if (typecode == 0)  // fragments of typecodes defined on lines before may slip through
	{
	   typecode = -4;
	   return typecode;
	}

        switch (typecode)
        {
	  case 1:
	  {
	     char p1[20], p2[20], p3[20], p4[20], p5[20], p6[20], p7[20], p8[20], p9[20];

	     if (!multscat)
	     {
		int n1 = sscanf(txt,"%s %s %s %s %s %s %s %s %s",p1,p2,p3,p4,p5,p6,p7,p8,p9);
		if (n1 == 9)
		{
		   int n2 = sscanf(p9,"%d",&par);
		   if (n2 == 1)
		     strcpy(title,"Add to Beam");
		   else if (p9[0] == '.')	// because of p9 = ".0" is not converted by sscanf
		     strcpy(title,"Add to Beam");
		   else
		     strcpy(title,"Beam");
	        }
		else
		   strcpy(title,"Beam");
	     }
	     else if (Turtle && multscat && (index > 1) && (index < nlines - 1))
	     {
		int typecode1, typecode2, n1, n2;
		char linet1[200], linet2[200];
		int ind = index;
		strcpy(linet1,GetLine(ind-1,0));
		n1 = sscanf(linet1,"%d %s",&typecode1,tst);
		strcpy(linet2,GetLine(ind+1,0));
		n2 = sscanf(linet2,"%d %s",&typecode2,tst);
		if ((n1 ==2 ) && (n2 == 2) && (typecode1 == 6) && (typecode2 == 3))
		   strcpy(title,"Multiple Scattering");
		else
		   strcpy(title,"Beam");
	     }
	     else
		strcpy(title,"Beam");
	  }
	  break;
	  case 2:  strcpy(title,"Pole Face"); break;
	  case 3:
	  {
	     if (Turtle && multscat && (index > 2))
	     {
		int typecode1, typecode2, n1, n2;
		char linet1[100], linet2[100], titlet[30];
		int ind = index;
		strcpy(linet1,GetLine(ind-2,0));
		n1 = sscanf(linet1,"%d %s",&typecode1,tst);
		strcpy(linet2,GetLine(ind-1,0));
		n2 = sscanf(linet2,"%d %s",&typecode2,tst);

		if ((n1 ==2 ) && (n2 == 2) && (typecode1 == 6) && (typecode2 == 1))
		{
		   strcpy(titlet,"Multiple Scattering");
		   int l = GetLabel(linet2,label);
		   if (l > 0)
		   {
		      strcat(titlet," : ");
		      strcat(titlet,label);
		   }
		   SetTitle(titlet,ind-1);
		}	
	     }
	     strcpy(title,"Drift");
	  }
	  break;
	  case 4:
	  {
	    if (ebend || ebend1)
		strcpy(title,"ElstatPrism");
	    else
		strcpy(title,"Bending Magnet");
	  }
	  break;
	  case 5:
	  {
	     if (equad || equad1)
		strcpy(title,"ElstatQuad");
	     else
		strcpy(title,"Quadrupole");
	  }
	  break;
	  case 6:
	  {
	     if (update)
	        strcpy(title,"Update Rx");
	     else
	     {
		char p1[30], p2[30], p3[30], p4[30], p5[30], p6[30];
	        int n = sscanf(txt,"%s %s %s %s %s %s",p1,p2,p3,p4,p5,p6);
		if (n <= 5)
		   strcpy(title,"1-dim-Slit");
		else
		   strcpy(title,"2-dim-Slit");
	     }
	     break;
	  }
	  case 7:  strcpy(title,"Shift"); break;
	  case 8:  strcpy(title,"Tolerance"); break;
	  case 9:  strcpy(title,"Repeat Code"); break;
	  case 10: strcpy(title,"Fit"); break;
	  case 11: strcpy(title,"Accelerator"); break;
	  case 12: strcpy(title,"Correlation"); break;
	  case 13:
	  {
	     if ( (n==2) && (par == 48) )
		angle = TRUE;
	     strcpy(title,"I/O Code"); break;
	  }
	  case 14: strcpy(title,"Matrix"); break;
	  case 15: strcpy(title,"Units"); break;
	  case 16:
	  {
	     if ( (n==2) && (par == 14) )
		update = FALSE;
	     if ( (n==2) && (par == 165) )
		multscat = TRUE;
	     if ( (n==2) && (par == 96) )
		equad1 = TRUE;
	     if ( (n==2) && (par == 95) )
		equad1 = FALSE;
	     if ( (n==2) && (par == 94) )
		ebend1 = TRUE;
	     if ( (n==2) && (par == 93) )
		ebend1 = FALSE;
	     strcpy(title,"Special Parameter"); break;
	  }
	  case 17: strcpy(title,"Second Order"); break;
	  case 18: strcpy(title,"Sextupole"); break;
	  case 19: strcpy(title,"Solenoid"); break;
	  case 20: strcpy(title,"Rotation"); break;
	  case 21: strcpy(title,"Wien Filter"); break;
	  case 22: strcpy(title,"Space Charge"); break;
	  case 23: strcpy(title,"RF Cavity"); break;
	  case 24: strcpy(title,"Load Register"); break;
	  case 25: strcpy(title,"Combination"); break;
	  case 26: strcpy(title,"Octupole"); break;
	  case 27: strcpy(title,"Separator"); break;
	  case 28: strcpy(title,"Solenoid2"); break;
	  case 29: strcpy(title,"Jump over Collimator"); break;
	  case 30: strcpy(title,"dE/dx Table"); break;
	  case 50:
	  {
	     if (Turtle)
	        strcpy(title,"1-dim Histogram");
	     else
	        strcpy(title,"Limits");
	     break;
	  }
	  case 51: strcpy(title,"x-Axis 2-dim Histogramm"); break;
	  case 52: strcpy(title,"y-Axis 2-dim Histogram"); break;
	  case 53: strcpy(title,"Flag for Histograms"); break;
	  case 60: strcpy(title,"Decay"); break;
	  default: 
	  {
	     strcpy(title,"Fragment");break;
	     typecode = -4;
	  }
        }
        int l = GetLabel(txt,label);
        if (l > 0)
        {
          strcat(title," : ");
          strcat(title,label);
        }
     }
   }
   return typecode;
}

BOOL TransInFile :: GetAngleInfo(void)
{
    return angle;
}

BOOL TransInFile :: GetUpdateInfo(void)
{
    return update;
}

int TransInFile :: GetLabel(char *txt, char *label)
{
    int  i, nr, l;
    char lab[20];

    for (i=strlen(txt)-1; i>=0; i--)
	if (txt[i] == ';')
	{
	   txt[i] = '\0';  // get rid of trailing ";"
	   break;
	}

    nr = sscanf(txt,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
	 t[0],t[1],t[2],t[3],t[4],t[5],t[6],t[7],t[8],t[9],t[10],
	 t[11],t[12],t[13],t[14],t[15],t[16],t[17],t[18],t[19]);

    strcpy(lab,"");
    i = nr-1;
    if ((t[i][0] == '/') || (t[i][0] == '\'') || (t[i][0] == '\"') )   
	strcpy(lab,&t[i][1]);
    for (i=0; i<20; i++)
	if ((lab[i] == '/') || (lab[i] == '\'') || (lab[i] == '\"') )
	{
		lab[i] = '\0';
		break;
	}

    strcpy(label,lab);
    l = strlen(lab);
    return l;
}

char *TransInFile :: GetLine(int nr, int pos)
{
   index = nr;

   if ((int)strlen(line[nr]) > pos)
	return &line[nr][pos];
   else
	return " ";
}

VOID TransInFile :: SetLine(char *txt, int nr)
{
   strcpy(line[nr],txt);
}

char *TransInFile :: GetTitle(int nr)
{
   index = nr;

   return title[nr];
}

char *TransInFile :: GetTitleSpec(int nr, BOOL Turtle)
{
   static char title2[100];
   char label[10];
   char temp[200], tst[200], rest1[200], rest2[200];
   int code, n, typecode;

   index = nr;

   strcpy(temp,line[nr]);
   strcpy(tst,"");
   strcpy(rest1,"");
   strcpy(rest2,"");
   typecode = 0;

   n = sscanf(temp,"%s %s %s",tst,rest1,rest2);
   if ( (n == 2) || (n==3) )
   {
	char *ptr;
	ptr = strstr(tst,".");
	if (ptr)
	   *ptr = '\0';   // cut off .xxx
	sscanf(tst,"%d",&typecode);
        typecode = abs(typecode);
        if (typecode == 16 || typecode == 15 || typecode == 13)
	{
	   ptr = strstr(rest1,";");
	   if (ptr)
		*ptr = '\0'; // cut off ;
	   sscanf(rest1,"%d",&code);
	}
   }
   if (typecode == 13)
   {
        strcpy(title2,GetText13(code,Turtle));
        strcpy(temp,line[nr]);
        int l = GetLabel(temp,label);
        if (l > 0)
        {
          strcat(title2," : ");
          strcat(title2,label);
        }
        return title2;
   }
   else if (typecode == 15)
   {
        strcpy(title2,GetText15(code,Turtle));
        strcpy(temp,line[nr]);
        int l = GetLabel(temp,label);
        if (l > 0)
        {
          strcat(title2," : ");
          strcat(title2,label);
        }
        return title2;
   }
   else if (typecode == 16)
   {
        strcpy(title2,GetText16(code,Turtle));
        strcpy(temp,line[nr]);
        int l = GetLabel(temp,label);
        if (l > 0)
        {
          strcat(title2," : ");
          strcat(title2,label);
        }
        return title2;
   }
   else
   {
        return title[nr];
   }
}

char *TransInFile :: GetText13(int code, BOOL Turtle)
{
   static char title[100];
   char temp[100];

   static struct TransTypeCode13{int code; char *name;} tp13[] = {
		1, "Current beam matrix printing",
		2, "Suppress beam matrix printing",
		3, "Each beam matrix printing",
		4, "Current R1 matrix printing",
		5, "Each R1 matrix printing",
		6, "Suppress R1 matrix printing",
		7, "Suppress Element data printing",
		8, "Resume Element data printing",
		9, "Misalignment summary table printing",
		12,"Beam line coordinate layout printing",
		24,"Current R2 matrix printing",
		40,"Bend restore normal options",
		41,"Bend beta1=beta2=bendangle/2",
		42,"Bend beta1=0, beta2=bendangle",
		43,"Bend beta1=bendangle, beta2=0",
		47,"Bend 2nd parameter means field",
		48,"Bend 2nd parameter means angle",
		0, ""};

   static struct TurtlTypeCode13{int code; char *name;} tt13[] = {
		10,  "Parent aperture observation",
		110, "Charged daughter aperture observation",
		210, "Neutral daughter aperture observation",
		11,  "Parent aperture non-observation",
		111, "Charged daughter non-aperture observation",
		211, "Neutral daughter non-aperture observation",
		0,   ""};

   strcpy(title,"I/O : ");
   strcpy(temp,"unspecified");
   if (Turtle)
   {
      for (int i=0; i<200; i++)
      {
	 if (tt13[i].code == 0) break;
	 if (code == tt13[i].code)
	 {
	    strcpy(temp,tt13[i].name);
	    break;
	 }
      }      
   }
   else
   {
      for (int i=0; i<200; i++)
      {
	 if (tp13[i].code == 0) break;
	 if (code == tp13[i].code)
	 {
	    strcpy(temp,tp13[i].name);
	    break;
	 }
      }      
   }
   strcat(title,temp);   
   return title;
}

char *TransInFile :: GetText15(int code, BOOL Turtle)
{
   static char title[100];
   char temp[100];

   static struct TransTypeCode15{int code; char *name;} tp15[] = {
		1, "Horz & vert transverse dimension",
		2, "Horz & vert divergence dimension",
		3, "Vert transverse beam dim only",
		4, "Vert beam divergence dim only",
		5, "Pulsed beam length dimension",
		6, "Momentum spread dimension",
		7, "Angle (bend,rotation) dimension",
		8, "Longitudinal length dimension",
		9, "Magnetig field dimension",
		10,"Particle mass dimension",
		11,"Beam momentum dimension",
		0, ""};

   static struct TurtlTypeCode15{int code; char *name;} tt15[] = {
		1, "Horz & vert transverse dimension",
		2, "Horz & vert divergence dimension",
		6, "Momentum spread dimension",
		7, "Angle (bend,rotation) dimension",
		8, "Longitudinal length dimension",
		9, "Magnetig field dimension",
		10,"Particle mass dimension",
		11,"Beam momentum dimension",
		12,"Particle energy dimension",
		0,   ""};

   strcpy(title,"Units : ");
   strcpy(temp,"unspecified");
   if (Turtle)
   {
      for (int i=0; i<200; i++)
      {
	 if (tt15[i].code == 0) break;
	 if (code == tt15[i].code)
	 {
	    strcpy(temp,tt15[i].name);
	    break;
	 }
      }      
   }
   else
   {
      for (int i=0; i<200; i++)
      {
	 if (tp15[i].code == 0) break;
	 if (code == tp15[i].code)
	 {
	    strcpy(temp,tp15[i].name);
	    break;
	 }
      }      
   }
   strcat(title,temp);   
   return title;
}

char *TransInFile :: GetText16(int code, BOOL Turtle)
{
   static char title[100];
   char temp[100];

   static struct TransTypeCode16{int code; char *name;} tp16[] = {
		1, "Bend field 2nd deriv",
		3, "Mass (in electron units)",
		4, "Bend horz half-aperture",
		5, "Bend vert half-aperture",
		6, "Cumulative length of system",
		7, "Bend K1 fringing field integral",
		8, "Bend K2 fringing field integral",
		9, "Horz Emit for periodic solution",
		10,"Vert Emit for periodic solution",
		11,"End mark length periodic solution",
		12,"Bend 1/R entrance face",
		13,"Bend 1/R exit face",
		14,"Slits instaed of update",
		15,"Tilt of focal plane",
		16,"x0 of init beam line coord",
		17,"y0 of init beam line coord",
		18,"z0 of init beam line coord",
		19,"theta0 of init beam line coord",
		20,"phi0 of init beam line coord",
		21,"1/R-limit while fitting",
		22,"Max numb of fit iterations",
		25,"Quad I1 fringing field integral",
		26,"Quad I2 fringing field integral",
		27,"Quad I3 fringing field integral",
		29,"Charge (in electron units)",
		31,"Random optimizer on",
		32,"Random optimizer off (def)",
		40,"Extended plate feature for accelerator",
		93,"Elect.static prism off (def)",
		94,"Elect.static prism on",
		95,"Elect.static quad off (def)",
		96,"Elect.static quad on",
		97,"Plasma lens off (def)",
		98,"Plasma lens on",
		0, ""};

   static struct TurtlTypeCode16{int code; char *name;} tt16[] = {
		1,  "Bend field 2nd derivative ",
		3,  "Mass (in electron units)",
		4,  "Bend horz half-aperture",
		5,  "Bend vert half-aperture",
		6,  "Redefine z for histogramming",
		7,  "Bend K1 fringing field integral",
		8,  "Bend K2 fringing field integral",
		9,  "Zmax for decay",
		10, "Energy entries on beam card",
		11, "Define p0 (design momentum)",
		12, "Bend 1/R entrance face",
		13, "Bend 1/R exit face",
		20, "Homegeneous beam distribution",
		21, "Binomial horz beam distribution",
		22, "Binomial vert beam distribution",
		23, "Binomial long beam distribution",
		24, "4Vol beam distribution",
		25, "Quad I1 fringing field integral",
		26, "Quad I2 fringing field integral",
		27, "Quad I3 fringing field integral",
		28, "Quad I4 fringing field integral",
		29, "Charge (in electron units)",
		30, "Seed for random generator",
		31, "Use Halton random generator",
		35, "Quad field random-add amplitude",
		36, "Bend's p0 random spread amplitude",
	        50, "Field scaling factor for SOLO (1)",
	        51, "Field scaling factor for SOLO (2)",
		93, "Elect.static prism off (def)",
		94, "Elect.static prism on",
		95, "Elect.static quad off (def)",
		96, "Elect.static quad on",
		97, "Plasma lens off (def)",
		98, "Plasma lens on",
		100,"Elliptical quad aperture",
		101,"Hyperbolic quad aperture",
		160,"Inverse slit (target)",
		161,"Normal slit (def)",
		162,"Rectangular slit contours",
		163,"Elliptical slit contours (def)",
		165,"Multiple scattering enabled",
		166,"RMS scattering enabled (def)",
		170,"Double sided splitting enabled",
		171,"Normal shifting enabled (def)",
		172,"Single sided splitting enabled",
		177,"Increment for slit dimensions",
		180,"Append beam vector (Pz) on diskfile",
		181,"Append beam vector (Nr) on diskfile",
		190,"Metafile for histogram storage",
		191,"File-output of 3rd order table",
		195,"Suppress entry in loss diagram",
		196,"Inversion of histogram flagging",
		197,"Normal histogram flagging (def)",
		198,"Concatination factor for slits",
		0,  ""};

   strcpy(title,"Param : ");
   strcpy(temp,"unspecified");
   if (Turtle)
   {
      if (code < 0) 
      {
	 strcpy(temp,"Multipole aberration of quad");
	 strcat(title,temp);   
	 return title;
      }
      for (int i=0; i<200; i++)
      {
	 if (tt16[i].code == 0) break;
	 if (code == tt16[i].code)
	 {
	    strcpy(temp,tt16[i].name);
	    break;
	 }
      }      
   }
   else
   {
      for (int i=0; i<200; i++)
      {
	 if (tp16[i].code == 0) break;
	 if (code == tp16[i].code)
	 {
	    strcpy(temp,tp16[i].name);
	    break;
	 }
      }      
   }
   strcat(title,temp);   
   return title;
}

VOID TransInFile :: SetTitle(char *txt, int nr)
{
   strcpy(title[nr],txt);
}

TransInFile :: ~TransInFile(VOID)
{
    int i;

    for (i=0; i<NLINES; i++)
    {
       delete []line[i];
       delete []title[i];
    }
    delete []line;
    delete []title;

    for (i=0; i<20; i++)
       delete []t[i];
    delete []t;
}

char *TransInFile :: GetDefaultData(int typecode)
{
    switch (typecode)
    {
      case  1: strcpy(dline," 1. .1 10. .1 10. 0. 0. 1. /BEAM/ ;"); break;
      case  2: strcpy(dline," 2. 0.0 ;"); break;
      case  3: strcpy(dline," 3. 1.0 /D/ ;"); break;
      case  4: strcpy(dline," 4. 1.0 10.0 0.0 /BEND/ ;"); break;
      case  5: strcpy(dline," 5. 0.5 5.0 100.0 /QUAD/ ;"); break;
      case  6:
	{
	   if (update)
		strcpy(dline," 6. 0. 1. ;");
	   else
		strcpy(dline," 6. 1.0 50.0 3.0 50.0 /SLIT/ ;");
	}
	break;
      case  7: strcpy(dline," 7. 0. 0. 0. 0. 0. 0. ;"); break;
      case  8: strcpy(dline," 8. 0.1 1.0 0.1 1.0 0.1 1.0 15. ;"); break;
      case  9: strcpy(dline," 9. 1.0 ;"); break;
      case 10: strcpy(dline," 10. 1.0 1.0 10.0 0.1 /FIT/ ;"); break;
      case 11: strcpy(dline," 11. 0.01 0.001 ;"); break;
      case 12: strcpy(dline," 12. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. 0. /CORR/ ;"); break;
      case 13: strcpy(dline," 13. 1.0 ;"); break;
      case 14: strcpy(dline," 14. 1.0 0. 0. 0. 0. 0. 1.0 ;"); break;
      case 15: strcpy(dline," 15. 1.0 /MM/ 0.1 ;"); break;
      case 16: strcpy(dline," 16. 4.0 100.0 ;"); break;
      case 17: strcpy(dline," 17. /SECO/ ;"); break;
      case 18: strcpy(dline," 18. 0.5 1.0 100.0 /SEXT/ ;"); break;
      case 19: strcpy(dline," 19. 1.0 10.0 /SOLO/ ;"); break;
      case 20: strcpy(dline," 20. 180.0 ;"); break;
      case 21: strcpy(dline," 21. 1.0 1.0 /WIEN/ ;"); break;
      case 22: strcpy(dline," 22. 0.05 0.2 0.0 0.0 /SPAC/ ;"); break;
      case 23: strcpy(dline," 23. 0.5 200.0 200.0 /CAVI/ ;"); break;
      case 24: strcpy(dline," 24. 1. 1. 1. ;"); break;
      case 25: strcpy(dline," 25. 1. 1. 4. 2. ;"); break;
      case 26: strcpy(dline," 26. 0.2 2.0 50.0 /OCTO/ ;"); break;
      case 27: strcpy(dline," 27. 0.1 0.0 0.0 /SEPA/ ;"); break;
      case 28: strcpy(dline," 28. 0.01 0. 0. 0. 0. 0. 0. 0. ;"); break;
      case 29: strcpy(dline," 29. 0.3 20.0 20.0 25.0 25.0 10.0 300.0 ;"); break;
      case 30: strcpy(dline," 30. 589. 1. 350. 1.27 200. 1.76 100. 2.87 10. 17.8 ;"); break;
      case 50:
	{
	  if (Turtle)
	     strcpy(dline," 50. 1.0 -10.0 10.0 1.0 ;");
	  else
	     strcpy(dline," 50. 1.0 -10.0 10.0 ;");
	}
	break;
      case 51: strcpy(dline,"51. 1.0 -10.0 10.0 1.0 ;"); break;
      case 52: strcpy(dline,"52. 3.0 -10.0 10.0 1.0 ;"); break;
      case 53: strcpy(dline,"53.1 ;"); break;
      case 60: strcpy(dline,"60.0 0.13957 0.10566 0.0 26.0 /DCAY/ ;"); break;
      default: strcpy(dline,""); break;
    }
    return (dline);
}

VOID TransInFile :: OverWrite(char *FileName, int nlines)
{
    char temp[270];

    if (nlines > 0)
    {
       FILE *fp = fopen(FileName,"w+");
       int i = 0;
       while (i<nlines)
       {
	  strcpy(temp,line[i]);
	  if (line[i+1][0] == '_')
	  {
	      strcat(temp,&line[i+1][1]);
	      i++;
	  }
          fprintf(fp,"%s\n",temp);
	  i++;
       }
       fclose(fp);
    }
}

BOOL TransInFile :: IsForTurtle(void)
{
    return Turtle;
}

BOOL TransInFile :: IsEquad(int m)
{
    char temp[200], tst[200], rest1[200], rest2[200];
    int i, n, typecode;
    BOOL equad = FALSE;

    for (i=0; i<m; i++)
    {
      strcpy(temp,line[i]);
      n = sscanf(temp,"%s %s %s",tst,rest1,rest2);
      if ( (n == 2) || (n==3) )
      {
	char *ptr;
	ptr = strstr(tst,".");
	if (ptr)
	   *ptr = '\0';   // cut off .xxx
	sscanf(tst,"%d",&typecode);
        typecode = abs(typecode);
        if (typecode == 16)
	{
	   if (strstr(rest1,"96") != NULL)
		equad = TRUE;
	   if (strstr(rest1,"95") != NULL)
		equad = FALSE;
	}
      }
    }
    return equad;
}

BOOL TransInFile :: IsEbend(int m)
{
    char temp[200], tst[200], rest1[200], rest2[200];
    int i, n, typecode;
    BOOL ebend = FALSE;

    for (i=0; i<m; i++)
    {
      strcpy(temp,line[i]);
      n = sscanf(temp,"%s %s %s",tst,rest1,rest2);
      if ( (n == 2) || (n==3) )
      {
	char *ptr;
	ptr = strstr(tst,".");
	if (ptr)
	   *ptr = '\0';   // cut off .xxx
	sscanf(tst,"%d",&typecode);
        typecode = abs(typecode);
        if (typecode == 16)
	{
	   if (strstr(rest1,"94") != NULL)
		ebend = TRUE;
	   if (strstr(rest1,"93") != NULL)
		ebend = FALSE;
	}
      }
    }
    return ebend;
}
///////////////////////////////////// EOF /////////////////////////////////////

