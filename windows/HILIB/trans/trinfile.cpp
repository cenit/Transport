///////////////////////////////////////////////////////////////////////////////
////////////////////// Implemention of class TransInFile //////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

void AddLabel(char *buffer, char *label)
{
    if (strlen(label))
    {  strcat(buffer," /");
       strcat(buffer,label);
       strcat(buffer,"/ ;");
    }
    else
       strcat(buffer," ;");
}

VOID TrimFloat(char *fn)
{
   int i, len = strlen(fn);
   for (i=len-1; i>0; i--)
   {  if (fn[i] == ' ')
         fn[i] = '0';     // blank -> 0
      if (fn[i] == '.')
      {  fn[i+1] = '\0';  // xxx. no zero after the point
         break;
      }
      if (fn[i] != '0')
      {  fn[i+1] = '\0';  // cut trailing zeroes
         break;
      }
   }
   if (fn[0] == '-' && strlen(fn) > 9)
     fn[9] = '\0';        // 7 digits + dot + minus
   else if (strlen(fn) > 8)
     fn[8] = '\0';        // 7 digits + dot
}

void PutTypeCode(char *buffer, float type)
{
    char temp[80];

    strcpy(buffer,"");
    sprintf(temp,String(IDS_6_3F),type);
    TrimFloat(temp);
    strcat(buffer,temp);
}

void AddParameter(char *buffer, float par)
{
    char temp[80];

    strcat(buffer," ");
    sprintf(temp,String(IDS_10_5F),par);
    TrimFloat(temp);
    strcat(buffer,temp);
}

void AddSpecial(char *buffer, char *format, float par)
{
    char temp[80];

    strcat(buffer," ");
    sprintf(temp,format,par);
    TrimFloat(temp);
    strcat(buffer,temp);
}

///////////////////////////////////////////////////////////////////////////////

#define MAXB 255  // same in FileBuff

TransInFile :: TransInFile(char *filename) : FileBuff(filename)
{
    char text[MAXB+1];

    strcpy(FileName,filename);
    nlines = TellNumberOfLines();
    if (nlines > 0)
    {  line = (char **) new long[nlines];
       for (int i=0; i<nlines; i++)
         if (GetNextLine(text))
         {  line[i] = new char[133];
            strncpy(line[i],text,132);
	    line[i][133] = '\0';
         }
    }
    index    = 0;
    linesout = 0;
    first    = TRUE;
    opCalQ   = NULL;
    opCalB   = NULL;
    g2       = (float)10.0;
    nindex   = (float)0.0;
    anin     = FALSE;
    fac      = (float)1.0;
    pscal    = (float)1.0;
    equad    = FALSE;

    f[0] = &TransInFile::TransferQuads;
    f[1] = &TransInFile::TransferBeam;
    f[2] = &TransInFile::TransferLength;
    f[3] = &TransInFile::TransferBero;
    f[4] = &TransInFile::ErrorMess;
    f[5] = &TransInFile::ErrorMess;
}

TransInFile :: ~TransInFile(VOID)
{
    if (nlines > 0)
    {  for (int i=0; i<nlines; i++)
          delete []line[i]; // [] found with Bounds Checker
       delete []line;       // dito
    }
    if (opCalQ) delete opCalQ;
    if (opCalB) delete opCalB;
}

VOID TransInFile :: GetTransFilePtr(TransFile *p)
{
    opOFile = p;
}

int TransInFile :: TransferQuads(char *info)
{
    char  *p, buffer[80];
    char  title[80], olab[80], dim1[80], dim2[80], dim3[80], semi[80];
    char  lab[80], ilab[80];

    float oleng = (float)0.0, ofield = (float)0.0, ohap = (float)0.0;
    float ileng = (float)0.0, ifield = (float)0.0, ihap = (float)0.0;
    float type  = (float)0.0; // initialisations found by Bounds Checker
    int   n, t;

    if (opOFile->NextQuad(buffer) == 1)
    {
       sscanf(buffer,String(IDS_SSFSFSFS),
           &title[0],&olab[0],&oleng,&dim1[0],&ofield,&dim2[0],&ohap,&dim3[0]);
       while(index < nlines)
       {  
          n = sscanf(line[index],String(IDS_4FSS),
                     &type,&ileng,&ifield,&ihap,&lab[0],&semi[0]);
          if (n>0)
             t = (int)type;
          else
             t = 0;
          if (t == 19)     // Solenoid
             n = sscanf(line[index],String(IDS_3FSS),
                     &type,&ileng,&ifield,&lab[0],&semi[0]);
          if ( ((t == 5 || t == 18) && n >= 5) ||
                         ( t == 19  && n >= 4) )
          {
             p = strtok(&lab[1],"/");
             if (p) strcpy(ilab,p);
             else strcpy(ilab,"");

             PutTypeCode(line[index],type);
             AddParameter(line[index],ileng);
             AddParameter(line[index],ofield);
             if (t != 19)                      // not for Solenoid
                AddSpecial(line[index],String(IDS_8_3F),ihap);
             AddLabel(line[index],ilab);

             if (strcmp(ilab,olab) == 0)
             {
                strcpy(info,String(IDS_F1LFT));
                strcat(info,ilab);
                sprintf(buffer,String(IDS_F2RGT),ofield,dim2);
                strcat(info,buffer);
             }
             else
             {
                strcpy(info,String(IDS_F1MM));
                strcat(info,ilab);
                strcat(info,String(IDS_LMM));
                strcat(info,olab);
                strcat(info,String(IDS_F2MM));
             }
             index++;
             return 1;
          }
          index++;
       }
    } 
    return 0;
}

int TransInFile :: TransferBeam(char *info)
{
    char  buffer[80], coord[80], dim[80], lab[80];
    float value, type, p[16], temp;
    int   n, i, t, len;

    while ( !linesout && (opOFile->InitialBeam2(buffer) == 1) )
    {  n = sscanf(buffer,String(IDS_SFS),&coord[0],&value,&dim[0]);
       if (n >= 2)
       {
          if      (strcmp(coord,String(IDS_x))    == 0)
             x = value;
          else if (strcmp(coord,String(IDS_xp))   == 0)
             xp = value;
          else if (strcmp(coord,String(IDS_r12))  == 0)
             r12 = value;
          else if (strcmp(coord,String(IDS_y))    == 0)
             y = value;
          else if (strcmp(coord,String(IDS_yp))   == 0)
             yp = value;
          else if (strcmp(coord,String(IDS_r34))  == 0)
             r34 = value;
          else if (strcmp(coord,String(IDS_R16))  == 0)
             R16 = value;
          else if (strcmp(coord,String(IDS_R26))  == 0)
             R26 = value;
          else if (strcmp(coord,String(IDS_p))    == 0)
             pp = value;
          else if (strcmp(coord,String(IDS_dpp))  == 0)
             dp = value;
          else
             continue;

          strcpy(info,buffer);
          return 1;
       }
    }

    linesout = 10;
    while (index < MIN(20,nlines))       // only first 20 lines
    {
       n = sscanf(line[index],String(IDS_F),&type);
       if (n == 0)
          t = 0;
       else
          t = int(type);
       n = 0;
       if (t == 1)        // Beam
       {  n = sscanf(line[index],String(IDS_8FS),
                  &p[0],&p[1],&p[2],&p[3],&p[4],&p[5],&p[6],&p[7],&lab[0]);
          p[1] = x;
          p[2] = xp;
          p[3] = y;
          p[4] = yp;
          p[6] = dp;
          p[7] = pp; 
          len  = 8;
       }
       else if (t == 12)  // Corr
       {  n = sscanf(line[index],String(IDS_16FS),
                  &p[0],&p[1],&p[2],&p[3],&p[4],&p[5],&p[6],&p[7],&p[8],
                  &p[9],&p[10],&p[11],&p[12],&p[13],&p[14],&p[15],&lab[0]);
          p[1] = r12;
          p[6] = r34;
          len  = 16;
       }
       else if (t == 14)  // Matrix
       {  n = sscanf(line[index],String(IDS_8FS),
                  &p[0],&p[1],&p[2],&p[3],&p[4],&p[5],&p[6],&p[7],&lab[0]);
          if (p[7] == 1.0)
          {  p[6] = R16;
             len = 8;
          }
          else if (p[7] == 2.0)
          {  p[6] = R26;
             len = 8;
          }
          else
             n = 0;
       }
       if (n > 0)
       {  
          PutTypeCode(line[index],p[0]);
          for (i=1; i<len; i++)
              AddSpecial(line[index],String(IDS_12_5F),p[i]);
          n = sscanf(lab,String(IDS_F),&temp);      // is lab == 0.0 ? 
          if (n==1 && temp==0.0 && t==14)  // 2nd order continuation of type 14
             strcat(line[index],lab);
          else if (strcmp(lab,";") != 0)
          {
             strcat(line[index]," ");
             strcat(line[index],lab);
             strcat(line[index]," ;");
          }   
          else
             strcat(line[index]," ;");

          strcpy(info,line[index]);
          index++;
          return 1;
       }
       index++;
    }
    return 0;
}

int TransInFile :: TransferLength(char *info)
{
    float length, type, p[5];
    char  *ptr, buffer[80], label[80], ilab[80], lab[80], title[80];
    int   n, i, t, len;
    
    while (opOFile->NextElement(buffer) == 1)
    {  n = sscanf(buffer,String(IDS_SSF),&title[0],&label[0],&length);
       if (n == 3)
       {  if ( (strcmp(title,String(IDS_T_DRIFT)) == 0) ||
               (strcmp(title,String(IDS_T_BEND))  == 0) ||
               (strcmp(title,String(IDS_T_QUAD))  == 0) ||
               (strcmp(title,String(IDS_T_SEXT))  == 0) ||
               (strcmp(title,String(IDS_T_SOLO))  == 0) )
          {   while (index < nlines)
              {
                 n = sscanf(line[index],String(IDS_F),&type);
                 if (n == 1)
                    t = (int)type;
                 else
                    t = 0;
                 if (t == 3)
                 {  sscanf(line[index],String(IDS_2FS),&p[0],&p[1],&lab[0]);
                    if (lab[0] == ';') {index++; continue;} // no label
                    len = 2;
                    break;
                 }
                 else if (t == 4)
                 {  sscanf(line[index],String(IDS_4FS),
                           &p[0],&p[1],&p[2],&p[3],&lab[0]);
                    if (lab[0] == ';') {index++; continue;} // no label
                    len = 4;
                    break;
                 }
                 else if (t == 5)
                 {  sscanf(line[index],String(IDS_4FS),
                           &p[0],&p[1],&p[2],&p[3],&lab[0]);
                    if (lab[0] == ';') {index++; continue;} // no label
                    len = 4;
                    break;
                 }
                 else if (t == 18)
                 {  sscanf(line[index],String(IDS_4FS),
                           &p[0],&p[1],&p[2],&p[3],&lab[0]);
                    if (lab[0] == ';') {index++; continue;} // no label
                    len = 4;
                    break;
                 }
                 else if (t == 19)
                 {  sscanf(line[index],String(IDS_3FS),
                           &p[0],&p[1],&p[2],&lab[0]);
                    if (lab[0] == ';') {index++; continue;} // no label
                    len = 3;
                    break;
                 }
                 else
                 {  index++;
                    continue;
                 }
              }
              ptr = strtok(&lab[1],"/");
              if (ptr) strcpy(ilab,ptr); // label without /
              else strcpy(ilab,"");
              if (strcmp(ilab,label) == 0)
              {  if (length != p[1])
                 {
                    linesout++;
                    p[1] = length;           // modify length in for001.dat
                    PutTypeCode(line[index],p[0]);
                    for (i=1; i<len; i++)
                       AddSpecial(line[index],String(IDS_12_5F),p[i]);
                    AddLabel(line[index],label);

                    strcpy(info,String(IDS_LMIL));
                    strcat(info,line[index]);
                    index++;
                    return 1;
                 }
                 else
                    index++;
              }
              else
              {
                 strcpy(info,ilab);
                 strcat(info,String(IDS_LMM2));
                 strcat(info,label);
                 index++;
                 return 1;
              }
          }
       }
       else
          continue;
    }
    if (linesout == 0)
    {  linesout = 1;
       strcpy(info,String(IDS_NOMLF));
       return 1;
    }
    return 0;
}

void TransInFile :: SetNewP(float p)
{
    NewP = p;
}

float TransInFile :: GetOldP(void)
{
    float type, p[9]; // [9] was [8], found by Bounds Checker (at compile time)
    char  lab[8], dummy[8];
    int   m, n, t;

    while (index < nlines)
    {
       n = sscanf(line[index],String(IDS_F),&type);
       if (n == 1)
          t = (int)type;
       else
          t = 0;
       if (t == 1)
       {  n = sscanf(line[index],String(IDS_8FSS),
                     &p[0],&p[1],&p[2],&p[3],&p[4],&p[5],&p[6],&p[7],&lab[0],&dummy[0]);
          m = sscanf(lab,String(IDS_F),&p[8]); // check if numeric
          if ( (n < 9) || (m == 1) )
          {  index++;
             continue;
          }
          index = 0;
          return p[7];
       }
       else if (t == 15)
       {  n = sscanf(line[index],String(IDS_2FLFL),
                     &p[0],&p[1],&lab[0],&p[2],&dummy[0]);
          if ( (n == 5) && (int(p[1]) == 11) )
             pscal = p[2];
          index++;
          continue;
       }
       else
       {  index++;
          continue;
       }
    }
    index = 0;
    return (float)0.0;
}

float TransInFile :: GetPScal(void)
{
    return pscal;
}

int TransInFile :: TransferBero(char *info)
{
    float type, p[9]; // [9] was [8], found by Bounds Checker (at compile time)
    float oldv;
    char  *ptr, label[5], lab[8], dummy[8], title[11];
    int   m, n, i, t, len;
    
    p[2] = (float)0.0; // was uninitialized, Bounds Checker

    if (index >= nlines)
       return 0;

    while (index < nlines)
    {
       n = sscanf(line[index],String(IDS_F),&type);
       if (n == 1)
          t = (int)type;
       else
          t = 0;
       if (t == 1)
       {
          n = sscanf(line[index],String(IDS_8FSS),
                     &p[0],&p[1],&p[2],&p[3],&p[4],&p[5],&p[6],&p[7],&lab[0],&dummy[0]);
          m = sscanf(lab,String(IDS_F),&p[8]); // check if numeric
          if ( (n < 9) || (m == 1) )
          {  index++;
             continue;
          }
          n = 9;
          len = 8;
          OldP = p[7];
          p[7] = NewP;
          fac  = NewP / OldP;
          break;
       }
       else if ( (t == 4) && !anin)
       {  n = sscanf(line[index],String(IDS_4FS),
                     &p[0],&p[1],&p[2],&p[3],&lab[0]);
          len = 4;
          strcpy(title,String(IDS_T_BEND));
          oldv = p[2];
          p[2] *= fac;
          break;
       }
       else if ( (t == 5) && !equad)
       {  n = sscanf(line[index],String(IDS_4FS),
                     &p[0],&p[1],&p[2],&p[3],&lab[0]);
          len = 4;
          strcpy(title,String(IDS_T_QUAD));
          oldv = p[2];
          p[2] *= fac;
          break;
       }
       else if (t == 13)
       {  n = sscanf(line[index],String(IDS_3FS),
                     &p[0],&p[1],&p[2],&lab[0]);
          if ((int)p[1] == 47)
             anin = FALSE;  // normal (field)
          else if ((int)p[1] == 48)
             anin = TRUE;   // angle instead of field
          index++;
          continue;
       }
       else if (t == 16)
       {  n = sscanf(line[index],String(IDS_3FS),
                     &p[0],&p[1],&p[2],&lab[0]);
          if ((int)p[1] == 96)      // equad
             equad = TRUE;
          else if ((int)p[1] == 95) // normal quad
             equad = FALSE;
          index++;
          continue;
       }
       else if (t == 18)
       {  n = sscanf(line[index],String(IDS_4FS),
                     &p[0],&p[1],&p[2],&p[3],&lab[0]);
          len = 4;
          strcpy(title,String(IDS_T_SEXT));
          oldv = p[2];
          p[2] *= fac;
          break;
       }
       else if (t == 19)
       {  n = sscanf(line[index],String(IDS_3FS),
                     &p[0],&p[1],&p[2],&lab[0]);
          len = 3;
          strcpy(title,String(IDS_T_SOLO));
          oldv = p[2];
          p[2] *= fac;
          break;
       }
       else
       {  index++;
          continue;
       }
   }

   if (n == (len+1))
   {
      ptr = strtok(&lab[1],"/");
      if (ptr) strcpy(label,ptr); // label without /
      else strcpy(label,"");

      linesout++;
      PutTypeCode(line[index],p[0]);
      for (i=1; i<len; i++)
          AddSpecial(line[index],String(IDS_12_5F),p[i]);
      AddLabel(line[index],label);

      if (n < 7)
         sprintf(info,String(IDS_BERO_FRM),title,label,oldv,p[2]);
      else
         strcpy(info,line[index]);

      index++;
      return 1;
   }
   else
      return 0;
}

int TransInFile :: ErrorMess(char *buffer)
{
   DispObjectMsg(String(IDS_NYIMPL));
   strcpy(buffer,"");
   return 0;
}

int TransInFile :: TransferData(int nr, char *buffer)
{
    return( (this->*f[nr]) (buffer) );
}

int TransInFile :: AnalyseInput(int mode, int &t, float &leng, float &field,
                                                  float &hap, char *label)
{
    char  *p;
    char  lab[7], semi[7];
    float type, ind, value;


    if (first)
    {  opCalQ = new CalibrationData(String(IDS_QCAL));
       opCalB = new CalibrationData(String(IDS_BCAL));
       nrQ    = opCalQ->GetNumberOfLines();
       if (!nrQ && (mode&7))
       {
          PutInfo(IDS_QUADSNF);
          return -1;
       }
       nrB    = opCalB->GetNumberOfLines();
       if (!nrB && (mode&24))
       {
          PutInfo(IDS_BENDSNF);
          return -1;
       }
       first  = FALSE;
    }

    while(index < nlines)
    {  if (strstr(line[index],"SEN"))   // finished afer first occurrence of SEN
          return 0;
       int n = sscanf(line[index],String(IDS_F),&type);
       if (n == 1)
          t = (int)type;
       else
          t = 0;

       if (t==5 && (mode&1)==1)                  // Quad
          sscanf(line[index],String(IDS_4FSS),
                     &type,&leng,&field,&hap,&lab[0],&semi[0]);
       else if (t==4 && (mode&8)==8 && !anin)    // Bend
       {  sscanf(line[index],String(IDS_4FSS),
                     &type,&leng,&field,&nindex,&lab[0],&semi[0]);
          hap = g2;
       }
       else if (t==18 && (mode&2)==2)            // Sextupoles
          sscanf(line[index],String(IDS_4FSS),
                     &type,&leng,&field,&hap,&lab[0],&semi[0]);
       else if (t==19 && (mode&4)==4)            // Solenoid
       {  sscanf(line[index],String(IDS_3FSS),
                     &type,&leng,&field,&lab[0],&semi[0]);
          hap = g2;
       }
       else if (t==21 && (mode&16)==16)          // Wien-Filter
       {  sscanf(line[index],String(IDS_3FSS),
                     &type,&leng,&field,&lab[0],&semi[0]);
          hap = g2;
       }
       else if (t == 13)
       {  sscanf(line[index],String(IDS_3F),&type,&ind,&value);
          if ((int)ind == 47)
             anin = FALSE;  // normal (field)
          else if ((int)ind == 48)
             anin = TRUE;   // angle instead of field
          index++;
          continue;
       }
       else if (t == 16)
       {  sscanf(line[index],String(IDS_3F),&type,&ind,&value);
          if ((int)ind == 5)
             g2 = value;
          index++;
          continue;
       }
       else
       {  index++;
          continue;
       }
       if (lab[0] == ';')                     // no label given ..
          strcpy(label,String(IDS_BLANK_LB)); // .. then make it blank
       else if (lab[1] == '*')        // first char of label equal '*' ...
       {  index++;                    // .. then ignore this element
          continue;
       }
       else
       {  p = strtok(&lab[1],"/");
          if (p) strcpy(label,p);
          else strcpy(label,"");
       }
       index++;
       return 1;
    }
    return 0;
}

int TransInFile :: InsertField(int type, float leng, long dac,
                                         float hap, char *label, float &field)
{
    int   i;

    if (type==5 || type==18 || type==19)
    {  int ind = nrQ;                // default: not in QUADS.CAL
       for (i=0; i<nrQ; i++)
          if (opCalQ->CompareData(i,label,leng,hap))
          {  ind = i;
             break;
          }
          opCalQ->GetField(dac,ind,field);
       if (ind == nrQ)
          return 0;
    }
    else if (type==4 || type==21)
    {  int ind = nrB;                // default: not in BENDS.CAL
       for (i=0; i<nrB; i++)
          if (opCalB->CompareData(i,label,leng,hap))
          {  ind = i;
             break;
          }
          opCalB->GetField(dac,ind,field);
       if (ind == nrB)
          return 0;
    }

    PutTypeCode(line[index-1],(float)type);
    AddParameter(line[index-1],leng);
    AddParameter(line[index-1],field);
    if (type == 5 || type == 18) 
       AddSpecial(line[index-1],String(IDS_8_3F),hap);
    else if (type == 4)                       
       AddSpecial(line[index-1],String(IDS_8_4F),hap);
    AddLabel(line[index-1],label);

    return 1;
}

int TransInFile :: FxyGauss(float q[], float x[], float y[], char *qtext, char *pmtext)
{
    char  *p, temp[30];
    char  label[100], lab[100], semi[100];
    float type;
    float leng,field,hap;
    float indic1, indic2, fitval, fitacc;
    int   t;
    char  crlf[3];

    crlf[0] = 0x0d; crlf[1] = 0x0a; crlf[2] = '\0';

    int mfx = 0;
    int mfy = 0;
    int mq  = 0;
    int n;

    while(index < nlines)
    {
       if (strstr(line[index],"SEN"))   // finished afer first occurrence of SEN
          return 1;

       n = sscanf(line[index],String(IDS_F),&type);
       if (n == 1)
          t = (int)type;
       else
          t = 0;

       if ((t==5) && (mq<30))           // Quad
       {
          n = sscanf(line[index],String(IDS_4FSS),
                     &type,&leng,&field,&hap,&lab[0],&semi[0]);
	  if (n < 6)
	     goto jump1;
          p = strtok(&lab[1],"/");
          if (p) strcpy(label,p);
          else strcpy(label,"");

	  field = q[mq] / (float)1000.0 ;
          mq++;
	  sprintf(temp,String(IDS_DAC_FORM),label,field);
	  strcat(qtext,temp);
          if (mq%4 == 0) strcat(qtext,crlf);

          strcpy(line[index],"");
          PutTypeCode(line[index],(float)type);
          AddParameter(line[index],leng);
          AddParameter(line[index],field);
          AddSpecial(line[index],String(IDS_8_3F),hap);
          AddLabel(line[index],label);
       }
jump1:
       if ((t==10) || (t==-10))   // Fit
       {
          n = sscanf(line[index],String(IDS_5FSS),
                     &type,&indic1,&indic2,&fitval,&fitacc,&lab[0],&semi[0]);
	  if (n < 7)
	     goto jump2;
          p = strtok(&lab[1],"/");
          if (p) strcpy(label,p);
          else strcpy(label,"");

          int i1 = (int)indic1;
          int i2 = (int)indic2;
          if ((i1==1) && (i2==1) && (mfx<30)) // x-constraint
          {
             if (x[mfx] < float(0.00001)) fitacc = float(999.9);
             fitval = (float)fabs(x[mfx]);
             mfx++;
	     sprintf(temp,String(IDS_PMD_FORM),label,fitval);

             goto newline;
          }
          if ((i1==3) && (i2==3) && (mfy<30)) // y-constraint
          {
             if (y[mfy] < float(0.00001)) fitacc = float(999.9);
             fitval = (float)fabs(y[mfy]);
             mfy++;
	     sprintf(temp,String(IDS_PMD_FORM),label,fitval);
newline:
	     strcat(pmtext,temp);
             if ((mfx+mfy)%5 == 0) strcat(pmtext,crlf);

             strcpy(line[index],"");
             PutTypeCode(line[index],(float)type);
             AddSpecial(line[index],String(IDS_3_1F),indic1);
             AddSpecial(line[index],String(IDS_3_1F),indic2);
             AddParameter(line[index],fitval);
             AddParameter(line[index],fitacc);
             AddLabel(line[index],label);
          }
       }
jump2:
       index++;
    }
    return 0;
}

int TransInFile :: FxyDac(int q[], int x[], int y[], char *qtext, char *pmtext)
{
    char  *p, temp[30];
    char  label[100], lab[100], semi[100];
    float type;
    float leng,field,hap;
    float indic1, indic2, fitval, fitacc;
    int   t;
    char  crlf[3];

    crlf[0] = 0x0d; crlf[1] = 0x0a; crlf[2] = '\0';

    opCalQ = new CalibrationData(String(IDS_QCAL));
    nrQ    = opCalQ->GetNumberOfLines();
    if (!nrQ)
    {
       PutInfo(IDS_QUADSNF);
       return 0;
    }

    int mfx = 0;
    int mfy = 0;
    int mq  = 0;
    int n;

    while(index < nlines)
    {
       if (strstr(line[index],"SEN"))   // finished afer first occurrence of SEN
          return 1;

       n = sscanf(line[index],String(IDS_F),&type);
       if (n == 1)
          t = (int)type;
       else
          t = 0;

       if ((t==5) && (mq<30))           // Quad
       {
          n = sscanf(line[index],String(IDS_4FSS),
                     &type,&leng,&field,&hap,&lab[0],&semi[0]);
	  if (n < 6)
	     goto jump1;
          p = strtok(&lab[1],"/");
          if (p) strcpy(label,p);
          else strcpy(label,"");

          int ind = nrQ;                // default: not in QUADS.CAL
          for (int i=0; i<nrQ; i++)
          {
             if (opCalQ->CompareData(i,label,leng,hap))
             {
                ind = i;
                break;
             }
          }
          if (ind < nrQ) // if found in QUADS.CAL
          {
             opCalQ->GetField(q[mq],ind,field); // convert DAC to field
             mq++;
	     sprintf(temp,String(IDS_DAC_FORM),label,field);
	     strcat(qtext,temp);
             if (mq%4 == 0) strcat(qtext,crlf);

             strcpy(line[index],"");
             PutTypeCode(line[index],(float)type);
             AddParameter(line[index],leng);
             AddParameter(line[index],field);
             AddSpecial(line[index],String(IDS_8_3F),hap);
             AddLabel(line[index],label);
          }
       }
jump1:
       if ((t==10) || (t==-10))   // Fit
       {
          n = sscanf(line[index],String(IDS_5FSS),
                     &type,&indic1,&indic2,&fitval,&fitacc,&lab[0],&semi[0]);
	  if (n < 7)
	     goto jump2;
          p = strtok(&lab[1],"/");
          if (p) strcpy(label,p);
          else strcpy(label,"");

          int i1 = (int)indic1;
          int i2 = (int)indic2;
          if ((i1==1) && (i2==1) && (mfx<30)) // x-constraint
          {
             fitval = float(x[mfx]) / float(10.0);
             if (fitval < float(0.01)) fitacc = float(1000.0);
             mfx++;
	     sprintf(temp,String(IDS_PMD_FORM),label,fitval);

             goto newline;
          }
          if ((i1==3) && (i2==3) && (mfy<30)) // y-constraint
          {
             fitval = float(y[mfy]) / float(10.0);
             if (fitval < float(0.01)) fitacc = float(1000.0);
             mfy++;
	     sprintf(temp,String(IDS_PMD_FORM),label,fitval);
newline:
	     strcat(pmtext,temp);
             if ((mfx+mfy)%5 == 0) strcat(pmtext,crlf);

             strcpy(line[index],"");
             PutTypeCode(line[index],(float)type);
             AddSpecial(line[index],String(IDS_3_1F),indic1);
             AddSpecial(line[index],String(IDS_3_1F),indic2);
             AddParameter(line[index],fitval);
             AddParameter(line[index],fitacc);
             AddLabel(line[index],label);
          }
       }
jump2:
       index++;
    }
    return 0;
}

int TransInFile :: ModifyInput(int typ, float par1, float par2,
                                                    float par3, char *olab)
{
    char  *p;
    char  semi[10];
    char  lab[10], ilab[10];
    float ileng, ifield, ihap;
    float type = (float)0.0; // initialization found with Bounds Checker
    int   n;
    
    while(index < nlines)
    {  
       n = sscanf(line[index],String(IDS_4FSS),
                  &type,&ileng,&ifield,&ihap,&lab[0],&semi[0]);
       int t = (int)type;
       if (t == 19)     // Solenoid
       {  n = sscanf(line[index],String(IDS_3FSS),
                     &type,&ileng,&ifield,&lab[0],&semi[0]);
          ihap = par3; // dummy adjustment
       }
       if ( ((t == 4 || t == 5 || t == 18) && n >= 5) ||
                      ( t == 19  && n >= 4) )
       {  
             p = strtok(&lab[1],"/");
             if (p) strcpy(ilab,p);
             else strcpy(ilab,"");

             BOOL comp1 = (fabs(ileng - par1) < 0.000001);
             BOOL comp2 = (fabs(ihap  - par3) < 0.000001);
             if ((strcmp(ilab,olab)) == 0 && (t == typ) && comp1 && comp2)
             {
                PutTypeCode(line[index],type);
                AddParameter(line[index],ileng);
                AddParameter(line[index],par2);   // update field
                if (t != 19)                      // not for Solenoid
                   AddSpecial(line[index],String(IDS_8_3F),ihap);
                AddLabel(line[index],ilab);

                return 1;
            }
       }
       index++;
    }
    return 0;
}

VOID TransInFile :: OverWrite(VOID)
{
    FILE *fp = fopen(FileName,"w+");
    for (int i=0; i<nlines; i++)
       fprintf(fp,"%s\n",line[i]);
    fclose(fp);
}

///////////////////////////////////// EOF /////////////////////////////////////

