////////////////////////////////////////////////////////////////////////////////
//////////////////// Implementation of class CalibrationData //////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

CalibrationData :: CalibrationData(char *filename) : FileBuff(filename)
{
   char  line[80];
   int   retval;
   char  typ[5], lab[5];
   int   leff, ap;
   float a0,a1,a2,a3,j0,jmax,dacmax;

   nr = TellNumberOfLines();
   if (nr > 0)
   {  rec6 = new REC6[nr+1];
      for (int i=0; i<nr; i++)
      {  retval = GetNextLine(line);
         if (retval)
         {  sscanf(line,String(IDS_SSDD7F),
                &typ[0],&lab[0],&leff,&ap,&a0,&a1,&a2,&a3,&j0,&jmax,&dacmax);
            strcpy((rec6+i)->typ,typ);
            strcpy((rec6+i)->lab,String(IDS_BLANK_LB));
            strncpy((rec6+i)->lab,lab,strlen(lab));
            (rec6+i)->leff   = leff;
            (rec6+i)->ap     = ap;
            (rec6+i)->a0     = a0;
            (rec6+i)->a1     = a1;
            (rec6+i)->a2     = a2;
            (rec6+i)->a3     = a3;
            (rec6+i)->j0     = j0;
            (rec6+i)->jmax   = jmax;
            (rec6+i)->dacmax = dacmax;
         }
      }  
      strcpy((rec6+nr)->typ,String(IDS_4PERC));
      strcpy((rec6+nr)->lab,"");
      (rec6+nr)->leff   = 0;
      (rec6+nr)->ap     = 0;
      (rec6+nr)->a0     = (float)0.0;
      (rec6+nr)->a1     = (float)1.0;
      (rec6+nr)->a2     = (float)0.0;
      (rec6+nr)->a3     = (float)0.0;
      (rec6+nr)->j0     = (float)1000.0;
      (rec6+nr)->jmax   = (float)1.0;
      (rec6+nr)->dacmax = (float)1000.0;
   }
}

CalibrationData :: ~CalibrationData(void)
{
   delete []rec6; // [] found with Bounds Checker
}

int CalibrationData :: GetNumberOfLines(void)
{
   return nr;
}

int CalibrationData :: CompareData(int i, char *lab, float leff, float hap)
{
   char label1[10], label2[10];

// get rid of leading or trailing blanks

   sscanf((rec6+i)->lab,String(IDS_STR_FORM),&label1[0]);
   sscanf(lab,String(IDS_STR_FORM),&label2[0]); 
   label1[9] = '\0'; // found with Bounds Checker
   label2[9] = '\0'; // found with Bounds Checker

   if (  (strcmp(label1,label2)        == 0) ||
       ( (strcmp((rec6+i)->lab,String(IDS_4PERC)) == 0) &&
         ((int)(1000.0 * leff + 0.01)  == (rec6+i)->leff) &&
         ((int)(10.0   * hap  + 0.01)  == (rec6+i)->ap) ) )
      return 1;
   return 0;
}

VOID CalibrationData :: GetDac(float bx, int i, long& dac, float& amps)
{
//  Get AMPERE- or DAC-Values from non-linear exitation curve.
//  Parametrisation according to PSI-Magnet-Group (Dave George).

    float bv     = (float)fabs(bx);
    float sig    = (bx >= (float)0.0) ? (float)1.0 : (float)-1.0;
    float dacmax = (rec6+i)->dacmax;
    float sig2   = (dacmax >= (float)0.0) ? (float)1.0 : (float)-1.0;
    float a0     = (rec6+i)->a0;
    float a1     = (rec6+i)->a1;
    float jmax   = (rec6+i)->jmax;
    amps         = (bv - sig * a0) * jmax / a1;  // linear term
    float j0     = (rec6+i)->j0;
    if (amps>j0 && jmax>j0)                      // non-linear contributions ?
    {  float a2  = (rec6+i)->a2;
       float a3  = (rec6+i)->a3;
       for (int j=0; j<100; j++)
       {  float x   = (amps - j0) / (jmax - j0);
          float x2  = x * x;
          float x3  = x2 * x;
          float aj  = a1 / jmax;
          float b   = a0*sig + aj * amps + a2 * x2 + a3 * x3; // non-linear curve
          float db  = bv - b;
          if (db < 0.0001) break;                     // convergence criteria
          float bj = aj + (((float)2.0*a2 + (float)3.0*a3*x)*x)
                                          / (jmax-j0);  // derivative
          float dj = db / bj;
          amps = amps + dj;
       }
    }
    amps = amps * sig * sig2;
    float temp = (amps * float(fabs(dacmax)) / jmax);
    temp += (temp > (float)0.0) ? +(float)0.5 : -(float)0.5;
    dac  = (long) temp;
}

VOID CalibrationData :: GetField(long dac, int i, float& b)
{
//  Get Field-Values from non-linear exitation curve.
//  Parametrisation according to PSI-Magnet-Group (Dave George).

    float a0     = (rec6+i)->a0;
    float a1     = (rec6+i)->a1;
    float dacmax = (rec6+i)->dacmax;
    float jmax   = (rec6+i)->jmax;
    float sig    = (dac > 0) ? (float)1.0 : (float)-1.0;
    float sig2   = (dacmax > (float)0.0) ? (float)1.0 : (float)-1.0;
    float amps   = (float)labs(dac) * jmax / (float)fabs(dacmax);
    b            = a0 * sig + amps * a1 / jmax;
    float j0     = (rec6+i)->j0;
    if (amps>j0 && jmax>j0)                  // non-linear contributions ?
    {  float a2  = (rec6+i)->a2;
       float a3  = (rec6+i)->a3;
       float x   = (amps - j0) / (jmax - j0);
       b        += (a2 + a3 * x) * x * x;
    }
    b            = b * sig * sig2;
}
///////////////////////////////////// EOF //////////////////////////////////////
