///////////////////////////////////////////////////////////////////////////////
///////////////////////////// CAMAC Handler as DLL ////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///   new: multi-drop version (only 2 crates at the moment) U.R. 27-11-1995 ///
///   if n>50, then subtract 50 from n and increment crate address          ///
///////////////////////////////////////////////////////////////////////////////
/// new: SMK offsets via device.lis (AdcChannel & AdcRange) U.R. 20-4-1999  ///
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <process.h>
#ifndef WIN32
#include "ntdefs.h"
#endif
#include "camac.hpp"
#ifdef __BORLANDC__
#define _itoa itoa
#endif

#ifdef WIN32
#ifndef WIN95
#define WINNT
#endif
#endif

/////////////////////////////////// Globals ///////////////////////////////////

char szVersionText[] = "This is version 2.23b compiled by Urs Rohrer (PSI)";

typedef struct
{
  char  DevName[9];
  char  DacSpecBit[2];
  char  DacRoadAddr[3];
  char  DacCamacN[3];
  long  DacLowLimit;
  long  DacUpLimit;
  int   DacType;
  char  AdcSpecBit[2];
  char  AdcRoadAddr[3];
  char  AdcCamacN[3];
  int   AdcChannel;
  int   AdcRange;
  int   AdcType;
  float DScale;
  float Test;
  float Precis;
  float PhysUnit;
  long  DacValue;
  float AdcValue;
  char  Save;
} DEVICE;

#define FLENGTH  6000         // limited file length
#define      nn   100         // limited to 100 devices

#ifdef WIN32
HANDLE	    hMem;
#endif
int         hFile;
long        Length;
char        Buff[FLENGTH];
char        *WorkBuff;
DEVICE      dev[nn];
char        name[nn][9];
int         PageIndex[20], NumberOfPages, ResuniN;
int         nr, nrn;
int         first  = 0;

typedef struct
{
   char   comm[40];
   char   ans[40];
   int    ok;
   int    counter;
   int    busy;
} CB;
CB   *cb    = 0;
HWND hRS232 = 0;
CRITICAL_SECTION crit;

int         status = 0;
char        ans[50];
char        nstr[5];
char        temp[4];
long        d[3];

char        wrtemplate[]  = {"\001A  ,  ,  \002   ,   ,   \003"};
char        rdtemplate[]  = {"\001A  ,  ,  \002\003"};
char        wrcom[30];
char        rdcom[30];

////////////////////////////// local Functions ////////////////////////////////

int far GetSmkDacOffset(LPSTR spec, LPSTR adr, LPSTR nnst)
{
    for (int i=0; i<nr; i++)
    {
	if ((strcmp(dev[i].DacSpecBit,spec) == 0) &&
	    (strcmp(dev[i].DacRoadAddr,adr) == 0) &&
	    (strcmp(dev[i].DacCamacN, nnst) == 0) )
	{
	   return dev[i].AdcChannel;  // alternate usage as SMK-DAC-offset
	}	 
    }
    return 0;
}

float far GetSmkAdcOffset(LPSTR spec, LPSTR adr, LPSTR nnst)
{
    for (int i=0; i<nr; i++)
    {
	if ((strcmp(dev[i].AdcSpecBit,spec) == 0) &&
	    (strcmp(dev[i].AdcRoadAddr,adr) == 0) &&
	    (strcmp(dev[i].AdcCamacN, nnst) == 0) )
	{
	   return ((float)dev[i].AdcRange / (float)1000.0);
	}	 
    }
    return ((float)0.0);
}

int far open(LPSTR filename)
{
    if ((hFile = _lopen(filename, OF_READWRITE)) != -1)
    {   Length = _llseek(hFile,0L,2);
        if (Length > FLENGTH)
           Length = FLENGTH - 1;           // limiter
        _llseek(hFile,0L,0);
        _lread(hFile, Buff, (int)Length);
        *(Buff+Length) ='\0';            // terminate with zero
        _lclose(hFile);
        return (int)Length;
    }
    else
        return -1;
}

int far ReadLn(char **p, char *string)

//  Read line from memory buffer
//  *p is the current pointer in memory, which is updated.

{
    int letter;

    while ( ((letter = **p) != '\n') && (letter != '\0') )
    {    *string++ = **p;
        (*p)++;
    }

    *string = '\0';      // terminate string
    (*p)++;              // jump over CR/LF

    if (letter == '\0')  // signal EOF
        return(0);
    else
        return(1);
}

int far TellNumberOfLines(void)
{
    char line[81];

    char *p = Buff;
    int nl = 0;
    while (ReadLn(&p,line) > 0)
    {  strcpy(line,"");
       nl++;
    }

    return(nl);
}

int far GetNextLine(char *line)
{
   return(ReadLn(&WorkBuff,line));
}

void far ReadPar(void)
{
   char line[81];
   int  j, n;

   char  par1[20], par2[20], par3[20], par4[3], par8[2], par9[3], par10[3];
   char  par17;
   long  par5, par6;
   int   par7, par11, par12, par13;
   float par14, par15, par16;
   float DacMax;

   int count = 0;   // count number of Device lines
   int count2= 0;   // count number of names
   int ind   = 1;   // page counter
   int ic    = 0;   // count entries per page ( <=16 )

   for (int i=0; i<nr; i++)
   { 
     if (GetNextLine(line))
     {  if (line[0] == '*')
        {
           if (ic > 0)
           {
              PageIndex[ind] = count;
              ic = 0;
              if (ind<19) 
              {  ind++;
                 NumberOfPages++;
              }
           }
        }
	else if (line[0] == '-') // comment line (ignore)
	   continue;
        else
        {  
	   par7  = 0;      // otherwise ev. wrong number of combis
           par16 = (float)0.0;    // default if eventually not there
	   par17 = 'Y';    // default if eventually not there
/*
           n = sscanf(line,"%s %s %s %s %ld %ld %d %s %s %s %d %d %d %f %f %f %s",
               &par1[0],
               &par2[0],
               &par3[0],
               &par4[0],
               &par5,
               &par6,
               &par7,
               &par8[0],
               &par9[0],
               &par10[0],
               &par11,
               &par12,
               &par13,
               &par14,
               &par15,
               &par16,
	       &par17);
*/
           char *token; n = 0;
           if ((token = strtok(line," ")) != NULL)
           {strcpy(par1,token); n++;
            if ((token = strtok(NULL," ")) != NULL)
            {strcpy(par2,token); n++;
             if ((token = strtok(NULL," ")) != NULL)
             {strcpy(par3,token); n++;
              if ((token = strtok(NULL," ")) != NULL)
              {strcpy(par4,token); n++;
               if ((token = strtok(NULL," ")) != NULL)
               {par5 = atol(token); n++;
                if ((token = strtok(NULL," ")) != NULL)
                {par6 = atol(token); n++;
                 if ((token = strtok(NULL," ")) != NULL)
                 {par7 = atoi(token); n++;
                  if ((token = strtok(NULL," ")) != NULL)
                  {strcpy(par8,token); n++;
                   if ((token = strtok(NULL," ")) != NULL)
                   {strcpy(par9,token); n++;
                    if ((token = strtok(NULL," ")) != NULL)
                    {strcpy(par10,token); n++;
                     if ((token = strtok(NULL," ")) != NULL)
                     {par11 = atoi(token); n++;
                      if ((token = strtok(NULL," ")) != NULL)
                      {par12 = atoi(token); n++;
                       if ((token = strtok(NULL," ")) != NULL)
                       {par13 = atoi(token); n++;
                        if ((token = strtok(NULL," ")) != NULL)
                        {par14 = (float)atof(token); n++;
                         if ((token = strtok(NULL," ")) != NULL)
                         {par15 = (float)atof(token); n++;
                          if ((token = strtok(NULL," ")) != NULL)
                          {par16 = (float)atof(token); n++;
                           if ((token = strtok(NULL," ")) != NULL)
                           {par17 = (char)token[0]; n++;
                           }
                          }
                         }
                        }
                       }
                      }
                     }
                    }
                   }
                  }
                 }
                }
               }
              }
             }
            }
           }
	   if ( ((n==3) || (n==4)) && (par2[0] == '=') ) // name substitution:
	   {						 // par1 = par3
                if (par3[strlen(par3)-1] == 13) par3[strlen(par3)-1] = '\0';
		for(j=0; j<count; j++)
		{
		   if (strcmp(par3,dev[j].DevName) == 0)
		   {
			strcpy(dev[j].DevName, par1); 
		        break;
		   }
		}
		for(j=0; j<count2; j++)
		{
		   if (strcmp(par3,name[j]) == 0)
		   {
			strcpy(name[j], par1); 
		        break;
		   }
		}
		continue;
	   }
           else if (n>0)
           {  if (strcmp(par1,"RESUNI")==0)
                 ResuniN = atoi(par4);
              else
              {  strcpy(name[count2],&par1[0]);
                 strcpy(&dev[count].DevName[0],&par1[0]);
                 strcpy(&dev[count].DacSpecBit[0],&par2[0]);
                 strcpy(&dev[count].DacRoadAddr[0],&par3[0]);
                 strcpy(&dev[count].DacCamacN[0],&par4[0]);
                 dev[count].DacLowLimit = par5;
                 dev[count].DacUpLimit  = par6;
                 dev[count].DacType     = par7;
                 strcpy(&dev[count].AdcSpecBit[0],&par8[0]);
                 strcpy(&dev[count].AdcRoadAddr[0],&par9[0]);
                 strcpy(&dev[count].AdcCamacN[0],&par10[0]);
                 dev[count].AdcChannel  = par11;
                 dev[count].AdcRange    = par12;
                 dev[count].AdcType     = par13;
                 dev[count].DScale      = par14;
                 DacMax = (par7==1 || par7==4 || par7==10) ? (float)65535.0 : (float)4095.0;
                 DacMax = (par7==5 || par7==8) ? (float)2047.0  : DacMax;
                 dev[count].Test        = DacMax / par14;
                 dev[count].Precis      = par15;
                 dev[count].PhysUnit    = par16;
                 dev[count].Save        = par17;
                 dev[count].DacValue    = 0L;
                 dev[count].AdcValue    = (float)0.0;
                 count++;
                 count2++;
                 ic++;
              }
           }
           else   // empty line -> space
           {  strcpy(&dev[count].DevName[0],"");
              strcpy(&dev[count].DacSpecBit[0],"");
              strcpy(&dev[count].DacRoadAddr[0],"");
              strcpy(&dev[count].DacCamacN[0],"");
              dev[count].DacLowLimit = 0L;
              dev[count].DacUpLimit  = 0L;
              dev[count].DacType     = 0;
              strcpy(&dev[count].AdcSpecBit[0],"");
              strcpy(&dev[count].AdcRoadAddr[0],"");
              strcpy(&dev[count].AdcCamacN[0],"");
              dev[count].AdcChannel  = 0;
              dev[count].AdcRange    = 0;
              dev[count].AdcType     = 0;
              dev[count].DScale      = (float)0.0;
              dev[count].Test        = (float)0.0;
              dev[count].Precis      = (float)0.0;
              dev[count].PhysUnit    = (float)0.0;
              dev[count].DacValue    = 0L;
              dev[count].AdcValue    = (float)0.0;
              dev[count].Save        = 'Y';
              count++;
              ic++;
           }
           if (ic == 16)  // more than 16 entries between '*'s.
           {  ic = 0;
              PageIndex[ind] = count;
              if (ind<19) 
              {  ind++;
                 NumberOfPages++;
              }
           }
        }
     }
   }
   nr  = count;
   nrn = count2;

   if (line[0] != '*')     // if last line was not a '*' ...
   {
      NumberOfPages++;     // ... increment page counter.
      if (par2[0] == '=')  // if last line was a name substitution ...
         NumberOfPages--;  // ... decrement page counter.

   }
   PageIndex[NumberOfPages] = nr;
}

void far DeviceList(void)
{
   for (int i=0; i<20; i++)
     PageIndex[i] = 0;
   NumberOfPages = 0;

   ResuniN = 0;

   WorkBuff = Buff;
   ReadPar();
}

int far GetNumberOfDevices(void)
{
   return nr;
}

void far parcpy(char *dest, char *source)

//  Like strcpy, but null byte not copied.
{
    char c;

    while( (c = *source++) != 0 )
      *dest++ = c;
}

int far CompareLabels(LPSTR devname, char *namelist)
{
//  for comparison with transport labels (only 4 chars)

    char name[20];

    if (strcmp((char *)devname,namelist) == 0)     // skip no chars
       return 1;

    int length = strlen(namelist);
    if (length == 5)
    {  name[0] = namelist[0];
       strcpy(&name[1],&namelist[2]);      // skip second char
       if (strcmp((char *)devname,name) == 0)
          return 1;

       name[0] = namelist[0];
       name[1] = namelist[1];
       strcpy(&name[2],&namelist[3]);      // skip third char
       if (strcmp((char *)devname,name) == 0)
          return 1;
    }

    else if (length == 6)
    {  name[0] = namelist[0];
       strcpy(&name[1],&namelist[3]);      // skip second and third char 
       if (strcmp((char *)devname,name) == 0)
          return 1;

       name[0] = namelist[0];
       name[1] = namelist[2];
       strcpy(&name[2],&namelist[4]);      // skip second and fourth char 
       if (strcmp((char *)devname,name) == 0)
          return 1;
    }

    return 0;                              // not found
}

int far DoCamac(char *command)
{
   if (hRS232)
   {  
      int i = 1000;
      while (cb->ok == -1)
      {
	wait(5);
	i--;
	if (i <= 0)
	   return 0;
      }
      cb->ok = -1;
      strcpy(cb->comm,command);
      SendMessage(hRS232,WM_USER,0,(long)cb);
      while(cb->ok < 0)
         YieldQQ();
      strcpy(ans,cb->ans);
      int ok = cb->ok;
      return ok;
   }
   else
      return 0;
}

int IniKsi2(void)

//  initialize KS3989 for 2nd crate (24 bits, Q-repeat scan, status enabled)
{
    int st1 = WriteCamac("80"," 0","17",60*256L);
    int st2 = WriteCamac("80"," 1","16",     0L);
    int st3 = WriteCamac("80"," 5","17",     5L);
    
    if ( (st1 + st2 + st3) == 24 )
       return(1);
    else
       return(0);
} 

int far SpecCtlCamac(LPSTR nnn, LPSTR a, LPSTR f)

//  execute CAMAC control command
{
    int  j, l;
    char n[4];

    strcpy(n,nnn);
    strcpy(wrcom,rdtemplate);

//  2-crate special

    if ( (n[0]>'4') && (n[1] != '\0') )
    {
	n[0] -= char(5);
        wrcom[1]++;
    }

//  fillin parameters into CAMAC command

    parcpy(&wrcom[2],(char *)n);
    parcpy(&wrcom[5],(char *)a);
    parcpy(&wrcom[8],(char *)f);

//  send command to KS3989

    int q = 1;

    if (hRS232)
    {  
#ifdef WIN32
       EnterCriticalSection(&crit);
#endif
       int i = 1000;
       while (cb->ok == -1)
       {
	 wait(5);
	 i--;
	 if (i <= 0)
	    return 0;
       }
       cb->ok = -1;
       strcpy(cb->comm,wrcom);
       SendMessage(hRS232,WM_USER,0,(long)cb);
       while(cb->ok < 0)
          YieldQQ();
       strcpy(ans,cb->ans);
       l = cb->ok;

       if (l > 0)
       {  status = 0;
          if ( (ans[0]==1) && (ans[1]==wrcom[1]) && (ans[2]==2) )
             for (j=3; j<l-1; j++)
                status = 10 * status + ans[j]-48;
       }
       else
          status = 3;

       q = ( 1^(status & 1) );

#ifdef WIN32
       LeaveCriticalSection(&crit);
#endif
    }
    return q;
}

int far WriteDACwoSemaph(int typ, LPSTR spec, LPSTR ad, LPSTR nnst, long ival)
/*
    execute DAC write command
    without Semaphore for WIN32 (preemptive Multi-Tasking)
    for calls from procedures where semaphore already set
    supported devices:
    0: 12 bit dac (12SW1)
    1: 16 bit dac unsigned (16SW.)
    2: 12 bit combi
    3: DCN32 pointer (auxilliary)
    4: 16 bit signed dac (16SW1A) combi-like
    5: 12 bit combi bipolar
    7: 12 bit signed dac (12SW1A) combi-like
    8: 12 bit bipolar dac (12SWBA) combi-like
    9: SMK (12 bit)
   10: 16 bit signed combi (has a 12 bit ADC)
*/
{
    int  j, l;
    int  idh, idm, idl;
    long jval;
    char nst[4];

    strcpy(nst,nnst);
    strcpy(wrcom,wrtemplate);

//  2-crate special

    if ( (nst[0]>'4') && (nst[1] != '\0') )
    {
	nst[0] -= char(5);
        wrcom[1]++;
    }

//  fillin parameters into CAMAC command

    parcpy(&wrcom[2],(char *)nst);
    parcpy(&wrcom[5],(char *)ad);
    if (spec[0] == '0')    
      parcpy(&wrcom[8],"16");   // spec = 0
    else
      parcpy(&wrcom[8],"17");   // spec = 1
    if (typ == 9)
      parcpy(&wrcom[8],"17");   // SMK
    if (typ == 3)
      parcpy(&wrcom[8],"20");   // DCN32
    if (typ == 10)
    {
       if (spec[0] == '1')    
          parcpy(&wrcom[8],"23");   // WR+SE+UP+SP bits set for setting polarity
       else
          parcpy(&wrcom[8],"16");   // 16 bit value setting
    }

//  prapare data to send

    if (ival < 0)
    {  switch (typ)
       {  case 0:
	  case 7:
            jval = labs(ival) + 4096L;     // 12 bit dac negative
            break;
          case 2:
            jval = labs(ival) + 61440L;    // 12 bit combi negative
            break;
          case 5:
          case 8:
            jval = ival + 4096L + 61440L; // 12 bit bipolar combi negative
            break;
          default:
            jval = labs(ival);
       }
    }
    else
       jval = ival;
    if (typ == 9)
       jval = (long)((float)ival * 4.095) + 32768L;  // SMK 0..1000

    idh = (int)((jval & 0xff0000) >> 16);
    idm = (int)((jval & 0xff00) >> 8);
    idl = (int)(jval & 0xff);
    _itoa(idh,temp,10);
    parcpy(&wrcom[11],temp);
    _itoa(idm,temp,10);
    parcpy(&wrcom[15],temp);
    _itoa(idl,temp,10);
    parcpy(&wrcom[19],temp);

//  send command to KS3989

    if ((l = DoCamac(wrcom)) == 0)
       status = 3;                    // timeout
    else
    {  status = 0;
       if ( (ans[0]==1) && (ans[1]==wrcom[1]) && (ans[2]==2) )
          for (j=3; j<l-1; j++)
             status = 10 * status + ans[j] - 48;
    }
    return(0);
}

////////////////////////////// exportable functions ///////////////////////////

void APIENTRY ShowCamacDLLVersion(HWND hWnd)
{
   MessageBox(hWnd,szVersionText,"About CAMAC(32).DLL for Windows",MB_OK);
}

void APIENTRY YieldQQ(void)
{
/*
   check if any window message is available and send this message to
   the destination application. Call this function to maintain window's
   nonpreemtive multitasking abilities. (By Stefan Ritt)
   This routine can be imported by FORTRAN DLLs.
*/
   MSG msg;

   while (PeekMessage(&msg, 0, 0, 0, TRUE))
   {  TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
}

void APIENTRY wait(int msecs)
{
#ifdef WINNT
   Sleep(msecs);
   YieldQQ();
#else
   long st = (long)GetTickCount();
   do {
         YieldQQ(); YieldQQ(); YieldQQ();

      } while ( (long)GetTickCount() < st+(long)msecs );
#endif
}

int APIENTRY IniDevList(LPSTR filename)
{
   if (first == 0)
   {  if (open(filename) != -1)
      {  first = 1;
         nr = TellNumberOfLines();
         if (nr > nn)
            nr = nn;             // limiter
         DeviceList();
         return(GetNumberOfDevices());
      }
      else
         return -1;
   }
   else
   {  first++;
      return 0;
   }
}


int APIENTRY ReloadDevList(LPSTR filename)
{
   if (open(filename) != -1)
   {
      nr = TellNumberOfLines();
      if (nr > nn)
         nr = nn;             // limiter
      DeviceList();
      return(GetNumberOfDevices());
   }
   else
      return -1;
}

int APIENTRY GetDevName(int pos, LPSTR devname)
{
   if ( (pos > -1) && (pos < nr) )
   {
      strcpy(devname,dev[pos].DevName);
      return 1;
   }
   else
   {
      strcpy(devname,"");
      return 0;
   }
}

int APIENTRY GetDevParam(int pos, LPSTR devname, long &low, long &hi,
                         float &scale, float &prec, float &punit, int &dactyp,
			 char &save, int &adctyp)
{
   if ( (pos > -1) && (pos < nr) )
   {
      strcpy(devname,dev[pos].DevName);
      low    = dev[pos].DacLowLimit;
      hi     = dev[pos].DacUpLimit;
      scale  = dev[pos].DScale;
      prec   = dev[pos].Precis;
      punit  = dev[pos].PhysUnit; 
      dactyp = dev[pos].DacType;
      save   = dev[pos].Save;
      adctyp = dev[pos].AdcType;
      return 1;
   }
   else
   {
      strcpy(devname,"nil");
      low    = 0L;
      hi     = 0L;
      scale  = (float)0.0;
      prec   = (float)0.0;
      punit  = (float)0.0; 
      dactyp = 0;
      save   = 'Y';
      adctyp = 0;

      return 0;
   }
}
int APIENTRY GetDevParam2(int pos, LPSTR devname, LPSTR dacsp, LPSTR dacrd,
			  LPSTR dacn, LPSTR adcsp, LPSTR adcrd, LPSTR adcn,
			  int &chan, int &range)
{
   if ( (pos > -1) && (pos < nr) )
   {
      strcpy(devname,dev[pos].DevName);
      strcpy(dacsp,dev[pos].DacSpecBit);
      strcpy(dacrd,dev[pos].DacRoadAddr);
      strcpy(dacn, dev[pos].DacCamacN);
      strcpy(adcsp,dev[pos].AdcSpecBit);
      strcpy(adcrd,dev[pos].AdcRoadAddr); 
      strcpy(adcn, dev[pos].AdcCamacN);
      chan   = dev[pos].AdcChannel;
      range  = dev[pos].AdcRange;
      return 1;
   }
   else
   {
      strcpy(devname,"nil");
      strcpy(dacsp," ");
      strcpy(dacrd," ");
      strcpy(dacn, " ");
      strcpy(adcsp," ");
      strcpy(adcrd," ");
      strcpy(adcn, " ");
      chan   = 0;
      range  = 0;
      return 0;
   }
}

int APIENTRY SetDac(LPSTR devname, long value)
{
   for (int i=0; i<nr; i++)
      if (CompareLabels(devname,dev[i].DevName))
      {  if (value > dev[i].DacUpLimit)
            value = dev[i].DacUpLimit;
         if (value < dev[i].DacLowLimit)
            value = dev[i].DacLowLimit;
         WriteDAC(dev[i].DacType,dev[i].DacSpecBit,
                  dev[i].DacRoadAddr,dev[i].DacCamacN,value);
         return 1;
      }
   return 0;
}

int APIENTRY SetDacWait(LPSTR devname, long value)
{
   long Status, oldvalue;
   int i, j;

   for (i=0; i<nr; i++)
      if (CompareLabels(devname,dev[i].DevName))
      {  if (value > dev[i].DacUpLimit)
            value = dev[i].DacUpLimit;
         if (value < dev[i].DacLowLimit)
            value = dev[i].DacLowLimit;
         ReadDAC( dev[i].DacType,dev[i].DacSpecBit,
                  dev[i].DacRoadAddr,dev[i].DacCamacN,oldvalue);
         WriteDAC(dev[i].DacType,dev[i].DacSpecBit,
                  dev[i].DacRoadAddr,dev[i].DacCamacN,value);
	 goto check;
      }
   return 0;

check:

// check if polarity has changed (0 has positive sign)
// if polarity changes, power supply will be swiched off and may not be
// switched on again.

   if (
	((oldvalue <  0L) && (value >= 0L)) ||
	((oldvalue >= 0L) && (value  < 0L))    )
   {

//     check if device is a combi

       if (dev[i].DacType != 2 &&
           dev[i].DacType != 4 &&
           dev[i].DacType != 5 &&
           dev[i].DacType != 7 &&
           dev[i].DacType != 8 &&
           dev[i].DacType != 10 )
          return 1;			// device is not a combi(like)

//     check status, if power supply not on try to switch it on

       for (j=0; j<60; j++)  // try up to 30 seconds long
       {
	  wait(500);

	  ReadCamac(dev[i].DacCamacN,dev[i].DacRoadAddr,"02",Status);	     

	  if ( (Status & 0x100) != 0L)
	     break;			// is on now

	  if (dev[i].DacSpecBit[0] == '0')
	     WriteCamac(dev[i].DacCamacN,dev[i].DacRoadAddr,"18",1L);
	  else
	     WriteCamac(dev[i].DacCamacN,dev[i].DacRoadAddr,"19",1L);

       }
   }
   return 1;
}

long APIENTRY ReadAll(LPSTR buffer)
{
   long  value;
   float fval;
   char  temp[20], fstr[20], result[40];
   char  star;

   strcpy(buffer,"*RAL2*\n");
   for (int i=0; i<nr; i++)
   {

      if (dev[i].DevName[0] == 13)
	 continue;
      ReadDAC(dev[i].DacType,dev[i].DacSpecBit,
                 dev[i].DacRoadAddr,dev[i].DacCamacN,value);
      ReadADC(dev[i].AdcType,dev[i].AdcSpecBit,
                 dev[i].AdcRoadAddr,dev[i].AdcCamacN,
                 dev[i].AdcChannel,dev[i].AdcRange,fval);

      if (dev[i].PhysUnit != 0.0)
	 sprintf(temp,"%8.1f",fval*dev[i].PhysUnit);
      else
      {
	 sprintf(temp,"%8.4f",fval);
         int pos = strlen(temp) - 1;
         if (temp[pos] == '0')  // trailing zero
	    temp[pos] = '\0';
      }

      int j = 0;
      int n = strlen(temp);
      for (j=0; j<n; j++)
	if (temp[j] != ' ')  // leading blanks
	   break;
      strcpy(fstr,&temp[j]);

      star = ' ';
      float aux1 = (float) labs(value);
      float aux2 = (float) fabs((double)(fval * dev[i].Test));
      float reldif = (aux1 > (float)1.0)
                     ? (float)fabs((double)((aux1-aux2)/aux1)) : (float)0.0;
      if (reldif > dev[i].Precis)
         star = '*';  // ADC out of range

      if (star == ' ')
         sprintf(result,"%s\t%ld\t%s\n",dev[i].DevName,value,fstr);
      else
      {
         sprintf(result,"%s\t%ld\t%c%s\n",dev[i].DevName,value,star,fstr);
      }
      strcat(buffer,result);
   }
   buffer[strlen(buffer)-1] = '\0'; // get rid of last <CR>
   return (long)nr;
}

long APIENTRY GetDac(LPSTR devname)
{
   long value;

   for (int i=0; i<nr; i++)
      if (CompareLabels(devname,dev[i].DevName))
      {  ReadDAC(dev[i].DacType,dev[i].DacSpecBit,
                 dev[i].DacRoadAddr,dev[i].DacCamacN,value);
         return value;
      }
   return -77777L;
}

long APIENTRY GetAdc(LPSTR devname)
{
// Return float as long !!
// Reason: compatibility between BC and MSC
// If DLL is built with one and application with other compiler, then crash !

   union  { float fval; long lval; } res;

   for (int i=0; i<nr; i++)
      if (CompareLabels(devname,dev[i].DevName))
      {  ReadADC(dev[i].AdcType,dev[i].AdcSpecBit,
                 dev[i].AdcRoadAddr,dev[i].AdcCamacN,
                 dev[i].AdcChannel,dev[i].AdcRange,res.fval);
         return res.lval;
      }
   res.fval = (float)0.999;
   return res.lval;
}

int APIENTRY DelDevList(void)
{
   if (first == 1)
   {  first = 0;
      return 1;
   }
   else
   {  first--;
      return 0;
   }
}

int APIENTRY IniSerialComm(LPSTR DevName)
{
#ifdef WIN95
   char rs232[20] = "RS23232 ";
#else
   char rs232[20] = "RS232 ";
#endif
   strcat(rs232,DevName);
#ifdef WIN95
   hRS232 = FindWindow(NULL,"RS23232");
#else
   hRS232 = FindWindow(NULL,"RS232");
#endif
   if (hRS232 == 0)
   {
      WinExec(rs232,SW_MINIMIZE);
#ifdef WIN95
      hRS232 = FindWindow(NULL,"RS23232");     // start RS23232.EXE
#else
      hRS232 = FindWindow(NULL,"RS232");       // start RS232.EXE
#endif
   }
   wait (100);
   if (hRS232)
   {
#ifndef WIN32
      cb = (CB *) GetWindowLong(hRS232,4); // extra window space !!
#else
      hMem = OpenFileMapping(FILE_MAP_WRITE,FALSE,"CamBuf");
      if (hMem)
         cb = (CB *)MapViewOfFile(hMem,FILE_MAP_ALL_ACCESS,0,0,0);
#endif
      cb->counter++;
      return 1;
   }
   return -2;
}

int APIENTRY CloseSerialComm(void)
{
    if (hRS232)
    {   
#ifdef WIN32
	if (hMem)
	    CloseHandle(hMem);
#endif
        cb->counter--;
        if (cb->counter == 0)
        {
	    wait (100);
            SendMessage(hRS232,WM_CLOSE,0,0L); // close RS232.EXE
            hRS232 = 0;
            cb     = 0;
        }
    }
    return 0;
}

int APIENTRY WriteCamac(LPSTR nnn, LPSTR a, LPSTR f, long data)
//  execute CAMAC write command
{
    int  j, l;
    int  idh, idm, idl;
    char n[4];

#ifdef WIN32
    EnterCriticalSection(&crit);
#endif

    strcpy(n,nnn);
    strcpy(wrcom,wrtemplate);

//  2-crate special

    if ( (n[0]>'4') && (n[1] != '\0') )
    {
	n[0] -= char(5);
        wrcom[1]++;
    }

//  fillin parameters into CAMAC command

    parcpy(&wrcom[2],(char *)n);
    parcpy(&wrcom[5],(char *)a);
    parcpy(&wrcom[8],(char *)f);
    idh = (int)((data & 0xff0000) >> 16);
    idm = (int)((data & 0xff00) >> 8);
    idl = (int)(data & 0xff);
    _itoa(idh,temp,10);
    parcpy(&wrcom[11],temp);
    _itoa(idm,temp,10);
    parcpy(&wrcom[15],temp);
    _itoa(idl,temp,10);
    parcpy(&wrcom[19],temp);

//  send command to KS3989

    if ((l = DoCamac(wrcom)) == 0)
       status = 3;                    // timeout
    else
    {  status = 0;
       if ( (ans[0]==1) && (ans[1]==wrcom[1]) && (ans[2]==2) )
          for (j=3; j<l-1; j++)
             status = 10 * status + ans[j] - 48;
       status &= 15;                  // only 4 bits
    }
#ifdef WIN32
    LeaveCriticalSection(&crit);
#endif
    return(status);
}

int APIENTRY ReadCamac(LPSTR nnn, LPSTR a, LPSTR f, long &data)

//  execute CAMAC read command
{
    int  j, l, ip;
    long temp;
    char n[4];

#ifdef WIN32
    EnterCriticalSection(&crit);
#endif

    strcpy(n,nnn);
    strcpy(rdcom,rdtemplate);

//  2-crate special

    if ( (n[0]>'4') && (n[1] != '\0') )
    {
	n[0] -= char(5);
        rdcom[1]++;
    }

//  fillin parameters into CAMAC command

    parcpy(&rdcom[2],(char *)n);
    parcpy(&rdcom[5],(char *)a);
    parcpy(&rdcom[8],(char *)f);

//  send command to KS3989

    if ((l = DoCamac(rdcom)) == 0)
    {  status = 3;                   // timeout
       data   = 99999L;
    }
    else
    {  data   = 0L;
       status = 0;
       d[0] = 0; d[1] = 0; d[2] = 0;
       if ( (ans[0]==1) && (ans[1]==rdcom[1]) && (ans[2]==2) )
       {  temp = 0L;
          ip   = 0;
          for (j=3; j<l-1; j++)
          {  if (ip < 3)
             {  if (ans[j] == 44)
                {  d[ip++] = temp;
                   temp    = 0;
                }
                else
                   temp = temp * 10 + ans[j]-48;
             }
             else
                status = 10 * status + ans[j]-48; 
          }
          temp = d[2] + (d[1]<<8) + (d[0]<<16);
          data = temp;
       }
    }
#ifdef WIN32
    LeaveCriticalSection(&crit);
#endif
    return(status);
}

int APIENTRY ControlCamac(LPSTR nnn, LPSTR a, LPSTR f)

//  execute CAMAC control command
{
    int  j, l;
    char n[4];

#ifdef WIN32
    EnterCriticalSection(&crit);
#endif

    strcpy(n,nnn);
    strcpy(wrcom,rdtemplate);

//  2-crate special

    if ( (n[0]>'4') && (n[1] != '\0') )
    {
	n[0] -= char(5);
        wrcom[1]++;
    }

//  fillin parameters into CAMAC command

    parcpy(&wrcom[2],(char *)n);
    parcpy(&wrcom[5],(char *)a);
    parcpy(&wrcom[8],(char *)f);

//  send command to KS3989

    if ((l = DoCamac(wrcom)) == 0)
       status = 3;                    // timeout
    else
    {  status = 0;
       if ( (ans[0]==1) && (ans[1]==wrcom[1]) && (ans[2]==2) )
          for (j=3; j<l-1; j++)
             status = 10 * status + ans[j]-48;
    }
#ifdef WIN32
    LeaveCriticalSection(&crit);
#endif
    return(status);
}

int APIENTRY WriteCamacDac(LPSTR devname, LPSTR func, long value)
{
   for (int i=0; i<nr; i++)
      if (CompareLabels(devname,dev[i].DevName))
      {
	 if (dev[i].DacSpecBit[0] == '0')
	    ;
	 else  				  // if special bit set ..
	 {
	     long xfunc = atol(func);
	     xfunc++;
	     _itoa((int)xfunc,func,10);	  // .. then increment func
	 }
         WriteCamac(dev[i].DacCamacN,dev[i].DacRoadAddr,func,value);
         return 1;
      }
   return 0;
}

int APIENTRY ReadCamacDac(LPSTR devname, LPSTR func, long &value)
{
   long val;

   for (int i=0; i<nr; i++)
      if (CompareLabels(devname,dev[i].DevName))
      {
	 if (dev[i].DacSpecBit[0] == '0')
	    ;
	 else  				  // if special bit set ..
	 {
	     long xfunc = atol(func);
	     xfunc++;
	     _itoa((int)xfunc,func,10);	  // .. then increment func
	 }
         ReadCamac(dev[i].DacCamacN,dev[i].DacRoadAddr,func,val);
//         value = val;
         value = val & 0xffff;  /* only 16 bits, CAN-bus has leading bit */
         return 1;
      }
   value = 0L;
   return 0;
}

int APIENTRY getq(void)

//   returns 1 if Q is set or 0 if Q is not set.
{
     return ( 1^(status & 1) );
}

int APIENTRY getx(void)

//   returns 1 if X is set or 0 if X is not set.
{
     return ( 1^((status & 2) >> 1) );
}

int APIENTRY IniKSI(void)

//  initialize KS3989 (24 bits, Q-repeat scan, status enabled
{
    int retval = 1;

    int st1 = WriteCamac("30"," 0","17",60*256L);
    int st2 = WriteCamac("30"," 1","16",     0L);
    int st3 = WriteCamac("30"," 5","17",     5L);

    if ((hFile = _lopen("2ndcrate", OF_READ)) != -1)
    {
	_lread(hFile, Buff, 10);
	_lclose(hFile);
	if (Buff[0] == 'Y')
	   retval = IniKsi2();
    }

    if ( (st1 + st2 + st3) == 24 )
       retval *= 1;
    else
       retval = 0;

    return retval;
} 

int APIENTRY WriteDAC(int typ, LPSTR spec, LPSTR ad, LPSTR nnst, long ival)
/*
    execute DAC write command
    supported devices:
    0: 12 bit dac (12SW1)
    1: 16 bit dac unsigned (16SW.)
    2: 12 bit combi
    3: DCN32 pointer (auxilliary)
    4: 16 bit signed dac (16SW1A) combi-like
    5: 12 bit combi bipolar
    7: 12 bit signed dac (12SW1A) combi-like
    8: 12 bit bipolar dac (12SWBA) combi-like
    9: SMK (12 bit)
   10: 16 bit signed combi (has a 12 bit ADC)
*/
{
#ifdef WIN32
    EnterCriticalSection(&crit);
#endif

//  16SW1A recursion (combination of type 7 and 1)

    if (typ == 4)
    {
       long signum;
       if (ival < 0)
	  signum = 0xffff;
       else
	  signum = 0;
       WriteDACwoSemaph(7,"1",ad,nnst,signum); // write with special == 1
       WriteDACwoSemaph(1,"0",ad,nnst,ival);
#ifdef WIN32
       LeaveCriticalSection(&crit);
#endif
       return(0);
    }

//  16 bit signed combi, recursion (combination of type 10 and 1)

    if (typ == 10)  // new type coded by U.R., June 2005
    {
       long signum;
       if (ival < 0)
	  signum = 0xf000;
       else
	  signum = 0;
       WriteDACwoSemaph(10,"1",ad,nnst,signum); // polarity
       WriteDACwoSemaph(1,"0",ad,nnst,ival);
#ifdef WIN32
       LeaveCriticalSection(&crit);
#endif
       return(0);
    }

    int  j, l;
    int  idh, idm, idl;
    long jval;
    char nst[4];

    strcpy(nst,nnst);
    strcpy(wrcom,wrtemplate);

//  SMK-offset-special

    if (typ == 9)
    {
	ival -= GetSmkDacOffset(spec,ad,nnst);	
    }

//  2-crate special

    if ( (nst[0]>'4') && (nst[1] != '\0') )
    {
	nst[0] -= char(5);
        wrcom[1]++;
    }

//  fillin parameters into CAMAC command

    parcpy(&wrcom[2],(char *)nst);
    parcpy(&wrcom[5],(char *)ad);
    if (spec[0] == '0')    
      parcpy(&wrcom[8],"16");   // spec = 0
    else
      parcpy(&wrcom[8],"17");   // spec = 1
    if (typ == 9)
      parcpy(&wrcom[8],"17");   // SMK
    if (typ == 3)
      parcpy(&wrcom[8],"20");   // DCN32

//  prapare data to send

    if (ival < 0)
    {  switch (typ)
       {  case 0:
	  case 7:
            jval = labs(ival) + 4096L;     // 12 bit dac negative
            break;
          case 2:
            jval = labs(ival) + 61440L;    // 12 bit combi negative
            break;
          case 5:
          case 8:
            jval = ival + 4096L + 61440L; // 12 bit bipolar combi negative
            break;
          default:
            jval = labs(ival);
       }
    }
    else
       jval = ival;
    if (typ == 9)
       jval = (long)((float)ival * 4.095) + 32768L;  // SMK 0..1000

    idh = (int)((jval & 0xff0000) >> 16);
    idm = (int)((jval & 0xff00) >> 8);
    idl = (int)(jval & 0xff);
    _itoa(idh,temp,10);
    parcpy(&wrcom[11],temp);
    _itoa(idm,temp,10);
    parcpy(&wrcom[15],temp);
    _itoa(idl,temp,10);
    parcpy(&wrcom[19],temp);

//  send command to KS3989

    if ((l = DoCamac(wrcom)) == 0)
       status = 3;                    // timeout
    else
    {  status = 0;
       if ( (ans[0]==1) && (ans[1]==wrcom[1]) && (ans[2]==2) )
          for (j=3; j<l-1; j++)
             status = 10 * status + ans[j] - 48;
    }
#ifdef WIN32
    LeaveCriticalSection(&crit);
#endif
    return(0);
}

int APIENTRY ReadDAC(int typ, LPSTR spec, LPSTR ad, LPSTR nnst, long &ival)
/*
    execute DAC read command
    supported devices:
    0: 12 bit dac (12SW1)
    1: 16 bit dac unsigned (16SW.)
    2: 12 bit combi
    3: DCN32 pointer (auxilliary)
    4: 16 bit signed dac (16SW1A) combi-like
    5: 12 bit combi bipolar
    7: 12 bit signed dac (12SW1A) combi-like
    8: 12 bit bipolar dac (12SWBA) combi-like
    9: SMK (12 bit)
   10: 16 bit signed combi (has a 12 bit ADC)
*/
{
//  16SW1A recursion (combination of type 7 and 1)

    if (typ == 4)
    {
       long signum;
       ReadDAC(7,"1",ad,nnst,signum); // read with special == 1
       ReadDAC(1,"0",ad,nnst,ival);
       if ( (signum & 61440L) == 61440L)
          ival = -ival;
       return(0);
    }

//  16 bit signed combi, recursion (combination of type 100 and 1)

    if (typ == 10)
    {
       long signum;
       ReadDAC(100,"1",ad,nnst,signum); // read with special == 1
       ReadDAC(1,"0",ad,nnst,ival);
       if ( (signum & 61440L) == 61440L)
          ival = -ival;
       return(0);
    }

    int  j, l, ip;
    long temp, sign;
    char nst[4];

#ifdef WIN32
    EnterCriticalSection(&crit);
#endif

    strcpy(nst,nnst);
    strcpy(rdcom,rdtemplate);

//  2-crate special

    if ( (nst[0]>'4') && (nst[1] != '\0') )
    {
	nst[0] -= char(5);
        rdcom[1]++;
    }

//  fillin parameters into CAMAC command

    parcpy(&rdcom[2],(char *)nst);
    parcpy(&rdcom[5],(char *)ad);
    if (spec[0] == '0')    
      parcpy(&rdcom[8]," 0");   // spec = 0
    else
      parcpy(&rdcom[8]," 1");   // spec = 1

    if (typ == 100)
    {
      parcpy(&rdcom[8]," 7");   // SE+UP+SP set
    }

//  send command to KS3989

    if ((l = DoCamac(rdcom)) == 0)
    {  status = 3;                      // no Q no X
       ival   = 0L;
    }
    else
    {  status = 0;
       ival   = 0L;
       d[0] = 0; d[1] = 0; d[2] = 0;
       if ( (ans[0]==1) && (ans[1]==rdcom[1]) && (ans[2]==2) )
       {  temp = 0L;
          ip   = 0;
          for (j=3; j<l-1; j++)
          {  if (ip < 3)
             {  if (ans[j] == 44)
                {  d[ip++] = temp;
                   temp    = 0;
                }
                else
                   temp = temp * 10 + ans[j]-48;
             }
             else
                status = 10 * status + ans[j]-48; 
          }
          temp = d[2] + (d[1]<<8) + (d[0]<<16);
          ival = temp;
       }
       if (typ == 100)				// 16 bit signed combi
         goto end;
       if (typ == 1)                           // 16 bit dac unsigned
         ival &= 65535L;
       else
       {
          if ( (ival & 36864L) == 36864L)      // 12 bit dac negative
             sign = -1L;
          else if ( (ival & 61440L) == 61440L) // 12 bit combi negative
             sign = -1L;
          else
             sign = 1L;

          ival &= 4095L;

          if (typ==5 && sign==-1L)             // 12 bit bipolar combi negative
             ival -= 4096L;
          else if (typ==8 && sign==-1L)        // 12SWBA
             ival -= 4096L;
          else          
             ival *= sign;
       }
       if (typ == 9)
       {
         ival = (long)(0.5 + ((float)ival / 4.095));   // SMK 0..1000
	 ival += GetSmkDacOffset(spec,ad,nnst); // offset-special
       }
    }
end:
#ifdef WIN32
    LeaveCriticalSection(&crit);
#endif
    return(0);
}

int APIENTRY ReadADC(int typ, LPSTR spec, LPSTR ad,
                              LPSTR nnst, int chan, int range, float &val)
/*
    execute ADC read command
    supported devices:
    0: 3dv8, 3dv32
    1: 4dv8, 4dv32
    2: 12 bit combi
    3: 12 bit unipolar DCN32 (1V)
    4: 12 bit adc (16SW1A) combi-like
    5: 12 bit bipolar combi
    6: 12 bit unipolar SADC or CSA
    7: 12 bit adc (12SW1A) combi-like
    8: 12 bit bipolar adc (12SWBA) combi-like
    9: SMK (12 bit)
   10: 12 bit signed ADC (has a 16 bit signed DAC)
*/
{
    int  j, l, ip;
    long temp;
    float sign;
    char nst[4];

#ifdef WIN32
    EnterCriticalSection(&crit);
#endif

//  set channel and range for 3dv8, 3dv32, 4dv8 and 4dv32

    if (typ <= 1)
    {  long ival = 2 * range + 4 * chan;
       WriteDACwoSemaph(0,spec,ad,nnst,ival);
       if (typ == 1)
          wait(100);  // was 55
    }

//  set pointer for DCN32

    if (typ == 3)
    {  long ival = 0x3e0 + chan;
       WriteDACwoSemaph(3," 0",ad,nnst,ival);
       *spec = '1';            // read with spec = 1
    }

    strcpy(nst,nnst);
    strcpy(rdcom,rdtemplate);

//  2-crate special

    if ( (nst[0]>'4') && (nst[1] != '\0') )
    {
	nst[0] -= char(5);
        rdcom[1]++;
    }

//  fillin parameters into CAMAC command

    parcpy(&rdcom[2],(char *)nst);

    if (typ == 6)  // SADC
    {
       char astring[5];
       if (chan <= 16)    
       {
          _itoa(chan-1,astring,10);
          parcpy(&rdcom[5],astring);  // A = channel
          parcpy(&rdcom[8]," 0");     // F = 0
       }
       else
       {
          _itoa(chan-17,astring,10);
          parcpy(&rdcom[5],astring);  // A = channel - 16
          parcpy(&rdcom[8]," 1");     // F = 1
       }
    }
    else
    {
       parcpy(&rdcom[5],(char *)ad);
       if (spec[0] == '0')    
         parcpy(&rdcom[8]," 0");   // spec = 0
       else
         parcpy(&rdcom[8]," 1");   // spec = 1
    }
    if ( (typ == 4) || (typ == 7) || (typ == 8) )  // 12SW1A, 12SWBA & 16SW1A
    {
       if (spec[0] == '0')    
         parcpy(&rdcom[8]," 4");   // F = 4, Read Ist-Value
       else
         parcpy(&rdcom[8]," 5");   // F = 5, Read Ist-Value
    }       

    if (typ == 10)
         parcpy(&rdcom[8]," 1");   // F = 0, Read Ist-Value

//  send command to KS3989

    if ((l = DoCamac(rdcom)) == 0)
    {  status = 3;                      // no Q no X
       val   = (float)0.0;
    }
    else
    {
       status = 0;
       val   = (float)0.0;
       temp  = 7777;
       if ( (ans[0]==1) && (ans[1]==rdcom[1]) && (ans[2]==2) )
       {  temp = 0L;
          d[0] = 0; d[1] = 0; d[2] = 0;
          ip   = 0;
          for (j=3; j<l-1; j++)
          {  if (ip < 3)
             {  if (ans[j] == 44)
                {  d[ip++] = temp;
                   temp    = 0;
                }
                else
                   temp = temp * 10 + ans[j]-48;
             }
             else
                status = 10 * status + ans[j]-48; 
          }
          temp = d[2] + (d[1]<<8) + (d[0]<<16);
       }
       long jval = temp;         // save whole length for sign computation

       if (typ == 10)
       {
	  temp = jval & 4095L;
          sign = (float)1.0;
          if ( (jval & 61440L) == 61440L)
             sign = (float)-1.0;
          val = sign * float(temp) * (float)0.0002442;       // 1 / 4095
	  goto end;
       }

       if (typ == 1)
          temp = temp & 65535L;  // 4dv8, 4dv32
       else
          temp = temp & 4095L;   // 3dv3, 3dv32, 12 bit combi, DCN32 or SADC

//  conversion from bcd into float for 3dv8, 3dv32, 4dv8 and 4dv32

       if (typ <= 1)
       {  int i1 = (int)temp & 15;
          int i2 = (int)(temp / 16) & 15;
          int i3 = (int)(temp / 256) & 15;
          val = (float)0.1   * (float)i3 + 
                (float)0.01  * (float)i2 + 
                (float)0.001 * (float)i1;
          if (typ == 1)
          {  int i4 = (int)(temp / 4096) & 15;
             val = (float)0.1 * val + (float)0.1 * (float)i4; 
          }
       }

//     combi 12 bit bipolar

       else if ( (typ == 5) || (typ == 8) )
       {  if ( (jval & 61440L)==61440L )
             temp -= 4096L;
          val = float(temp) * (float)0.0004884;             // 1 / 2047
       }

//     combi 12 bit, SMK, DCN32 or SADC(CSA) 12 bit unipolar

       else
       {  sign = (float)1.0;
          if ( (jval & 61440L)==61440L && typ!=3 )
             sign = (float)-1.0;
          val = sign * float(temp) * (float)0.0002442;       // 1 / 4095

          if (typ == 9)  				// SMK offset special
	     val += GetSmkAdcOffset(spec,ad,nnst);
       }
    }
end:
#ifdef WIN32
    LeaveCriticalSection(&crit);
#endif
    return(0);
}

int RC(void)
{
      if (cb->busy == 0)
      {  cb->busy = 1;
         if (ResuniN != 0)
         {  sprintf(nstr,"%d",ResuniN);   // unit must be in first crate
            for (int i=0; i<30; i++)
            {  
               if (SpecCtlCamac(nstr, " 0", "12"))
                  return 1;
               wait(1000);
            }
            return 0;
         }
         else
            return 1;
      }
      else
      {
//         wait(1000);
         return 0;
      }
}

int APIENTRY ReserveCrate(void)
{
   for (int i=0; i<3; i++)
   {
       if (RC())
           return 1;
   }
   return 0;
}


int APIENTRY FreeCrate(void)
{
   if (cb->busy == 1)
   {
      if (ResuniN != 0)
      {  sprintf(nstr,"%d",ResuniN);  // unit must be in first crate
         ControlCamac(nstr," 0","14");
      }
      cb->busy = 0;
   }
   return 1;
} 

int APIENTRY ReadScaler(LPSTR nst, LPSTR chan, long &value)
{
   return(ReadCamac(nst,chan," 0",value));
}

int APIENTRY ClearScaler(LPSTR nst, LPSTR chan)
{
   return(ControlCamac(nst,chan," 9"));
}

int APIENTRY SwitchOnCombis(void)
{
   int count = 0;

   for (int i=0; i<nr; i++)
   {
      if (dev[i].DacType == 2 ||
          dev[i].DacType == 4 ||
          dev[i].DacType == 5 ||
          dev[i].DacType == 7 ||
          dev[i].DacType == 8 ||
          dev[i].DacType == 10 )
      {
          if (dev[i].Save == 'X')  // exclude from being switched on
	     continue;
	  if (dev[i].DacSpecBit[0] == '0')
             WriteCamac(dev[i].DacCamacN,dev[i].DacRoadAddr,"18",1L);
	  else
             WriteCamac(dev[i].DacCamacN,dev[i].DacRoadAddr,"19",1L);
          count++;
      }
   }
   return count;   
}

int APIENTRY SwitchOnCombi(LPSTR devname)
{
   for (int i=0; i<nr; i++)
      if (CompareLabels(devname,dev[i].DevName))
      {
         if (dev[i].DacType == 2 ||
             dev[i].DacType == 4 ||
             dev[i].DacType == 5 ||
             dev[i].DacType == 7 ||
             dev[i].DacType == 8 ||
             dev[i].DacType == 10 )
	 {
	    if (dev[i].DacSpecBit[0] == '0')
               WriteCamac(dev[i].DacCamacN,dev[i].DacRoadAddr,"18",1L);
	    else
               WriteCamac(dev[i].DacCamacN,dev[i].DacRoadAddr,"19",1L);
	    return 1;
	 }
      }
   return 0;
}

int APIENTRY SwitchOffCombi(LPSTR devname)
{
   for (int i=0; i<nr; i++)
      if (CompareLabels(devname,dev[i].DevName))
      {
         if (dev[i].DacType == 2 ||
             dev[i].DacType == 4 ||
             dev[i].DacType == 5 ||
             dev[i].DacType == 7 ||
             dev[i].DacType == 8 ||
             dev[i].DacType == 10 )
	 {
	    if (dev[i].DacSpecBit[0] == '0')
               WriteCamac(dev[i].DacCamacN,dev[i].DacRoadAddr,"18",2L);
	    else
               WriteCamac(dev[i].DacCamacN,dev[i].DacRoadAddr,"19",2L);
	    return 1;
	 }
      }
   return 0;
}

int APIENTRY GetNumberOfPages(void)
{
   return NumberOfPages;
}

int APIENTRY GetPageIndex(int PageNr)
{
   if ( PageNr == 0)
      return 0;
   else if ( (PageNr > 0) && (PageNr < NumberOfPages + 1) )
      return PageIndex[PageNr];
   else
      return -1;
}

/////////////////////////////// Windows functions /////////////////////////////

#ifdef WIN32
#ifdef __BORLANDC__

#pragma argsused
BOOL WINAPI DllEntryPoint( HINSTANCE hInst,
                           DWORD ul_reason_being_called, LPVOID plvReserved)
{
    switch (ul_reason_being_called)
    {
       case DLL_PROCESS_ATTACH: break;
       case DLL_THREAD_ATTACH: break;
       case DLL_THREAD_DETACH: break;
       case DLL_PROCESS_DETACH: break;
       default: break;
    }
    InitializeCriticalSection(&crit);
    return 1;
}

#else

INT  APIENTRY LibMain(HANDLE hInst, 
                      DWORD ul_reason_being_called, LPVOID lpReserved)
{
    switch (ul_reason_being_called)
    {
       case DLL_PROCESS_ATTACH: break;
       case DLL_THREAD_ATTACH: break;
       case DLL_THREAD_DETACH: break;
       case DLL_PROCESS_DETACH: break;
       default: break;
    }
    InitializeCriticalSection(&crit);
    return 1;

    UNREFERENCED_PARAMETER(lpReserved);
}
#endif
#else

#ifdef __BORLANDC__
#pragma argsused
#endif

int FAR PASCAL LibMain( HANDLE hInstance, UINT wDataSegment,
                                          UINT wHeapSize, LPSTR lpszCmdLine )
{
    if ( wHeapSize != 0 )
        UnlockData( 0 );

    return 1;
}

#ifdef __BORLANDC__
#pragma argsused
#endif

int FAR PASCAL WEP ( int bSystemExit )
{
    return 1;
}

#endif

///////////////////////////////////// EOF /////////////////////////////////////

