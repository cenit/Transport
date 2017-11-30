///////////////////////////////////////////////////////////////////////////////
////////////////////// class TransGrafFile implementation /////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

TransGrafFile :: TransGrafFile(char *filename, char *savefile, char *for001)
               : TransFile(filename, savefile, for001)
{
    hap    = (float)10.0;
    totrot = (float)0.0;
    sn     = (float)0.0;
    cs     = (float)1.0;
    prevtype = 0;
    nexttype = 0;
    CMS[0]   = 'c';
    CMS[1]   = 'm';
    CMS[2]   = '\0';
}

TransGrafFile :: ~TransGrafFile(void)
{
}

float TransGrafFile :: splint(float x[], float y[], float yp[], int n, float z)
{
/*
   Original Fortran Library Routine by E. Seiler (SIN), March 1971
   Translated from Fortran-4 to C by Urs Rohrer (PSI), April 1992

   n     number of given points (at least 2)
   x[]   given arguments (must be sorted in ascending order)
   y[]   given function values
   yp[]  given first derivatives of function
   z     given position at which function has to be interpolated
*/

// catch abnormal conditions

   if (n<2)
     return (float)0.0;
   if (z<x[0] || z>x[n-1])
     return (float)0.0;

// get nearest points through bisection

   int left    = 0;
   int right   = n-1;
   while (right-left > 1)
   {  int half = (right + left) / 2;
      if (z < x[half])
        right  = half;
      else if (z > x[half])
        left   = half;
      else
      { left   = half;
        right  = half + 1;
      }
   }

// compute interpolated value through cubic spline

   float x1  = x[left];
   float x2  = x[right];
   float y1  = y[left];
   float y2  = y[right];
   float y1p = yp[left];
   float y2p = yp[right];

   float dx  = x2 - x1;
   float dy  = y2 - y1;
   float u   = (z - x1) / dx;
   float u2  = u  * u;
   float u3  = u2 * u;
   float dy2 = (float)-2.0 * dy;
   float yp2 = y2p + (float)2.0 * y1p;
   float dy3 = (float)3.0 * dy;
   float yp3 = y1p + y2p;
   float yb2 = dy2 + dx * yp3;
   float yb4 = dy3 - dx * yp2;

   return (y1 + u * dx * y1p + u2 * yb4 + u3 * yb2);
}

float TransGrafFile :: Fac2(char *dim5, char *dim6)
{
   float fac;
   char CM[3], MM[3], PM[3], PC[3], PC2[3], PM2[3];

   CM[0]   = 'c';    CM[1]   = 'm';    CM[2]   = '\0';
   MM[0]   = 'm';    MM[1]   = 'm';    MM[2]   = '\0';
   PM[0]   = 'p';    PM[1]   = 'm';    PM[2]   = '\0';
   PC[0]   = 'p';    PC[1]   = 'c';    PC[2]   = '\0';
   PM2[0]  = '%';    PM2[1]  = 'o';    PM2[2]  = '\0';
   PC2[0]  = '%';    PC2[1]  = ' ';    PC2[2]  = '\0';


   if ((strcmp(dim5,CM) == 0) && (strcmp(dim6,PM)  == 0))  fac = (float)0.1;
   if ((strcmp(dim5,CM) == 0) && (strcmp(dim6,PM2) == 0)) fac = (float)0.1;
   if ((strcmp(dim5,MM) == 0) && (strcmp(dim6,PC)  == 0))  fac = (float)10.0;
   if ((strcmp(dim5,MM) == 0) && (strcmp(dim6,PC2) == 0)) fac = (float)10.0;
   if ((strcmp(dim5,MM) == 0) && (strcmp(dim6,PM)  == 0))  fac = (float)1.0;
   if ((strcmp(dim5,MM) == 0) && (strcmp(dim6,PM2) == 0)) fac = (float)1.0;
   if ((strcmp(dim5,CM) == 0) && (strcmp(dim6,PC)  == 0))  fac = (float)1.0;
   if ((strcmp(dim5,CM) == 0) && (strcmp(dim6,PC2) == 0)) fac = (float)1.0;

///////
//	{
//	char buffer[100];
//	sprintf(buffer,"%f %s %s ", fac,dim5,dim6);
//	MessageBox(NULL,buffer,"Diagnostics",MB_OK);
//	}
///////

   return fac;
}

int TransGrafFile :: GetEnveloppe(int nrf, float zmin, float zmax, int np,
                                  float x[], float y[], float p[], float py[], float dl[])
{
   REC2 rec2;

   int nr = GetNumberOfElements();
   if (nr == 0) return 0;
   float fac = (float)1.0;
   if (strcmp(GetDimension(0),CMS) == 0) fac = (float)0.1;
   if (strcmp(GetDimension(7),CMS) == 0) fac = fac * (float)0.01;
   int type;
   int index      = -1;
   int prevtype   = 1;
   float prevleng = (float)-99.0;
   float d, dp, dy, dyp;
   float s11, s22, s21, s33, s44, s43, r12x, r34y;
   float sn2, cs2, tqx, tqy, tqxp, tqyp;
   float fac2 = Fac2(GetDimension(4),GetDimension(5));

   float dz    = (zmax - zmin) / (float) (np - 1);
   float *qz   = new float[nr];
   float *qx   = new float[nr];
   float *qxp  = new float[nr];
   float *qy   = new float[nr];
   float *qyp  = new float[nr];
   float *qd   = new float[nr];
   float *qdp  = new float[nr];
   float *qdy  = new float[nr];
   float *qdyp = new float[nr];
   float *qdl  = new float[nr];
   float *qdlp = new float[nr];

   FILE *fp = GetFilePointer(nrf);
   while (fread(&rec2,sizeof(REC2),1,fp) == 1)
   {  
      adjust(fp);
      type = (int)rec2.type;
      switch (type)
      {  case 1:
         case 2:
         case 3:
         case 4:
         case 5:
         case 11:
         case 12:
         case 14:
         case 18:
         case 19:
         case 21:
         case 30:
         { index++;
           if (type == 12 && prevtype == 1)  index--;  // beam special
           if (type == 14 && prevtype == 1)  index--;  // beam special
           if (type == 14 && prevtype == 12) index--;  // beam special
           qz[index]  = rec2.along;
           if (rec2.along-prevleng < (float)0.0009 && 
                                           index > 0)  // zero length ->
              qz[index] += (float)0.0001;              // add a little bit

//	   envelope (backrotated)

//           qx[index]   = rec2.siun[0];
//           qxp[index]  = rec2.siun[1] * rec2.r12 * fac;
//           qy[index]   = rec2.siun[2];
//           qyp[index]  = rec2.siun[3] * rec2.r34 * fac;

	   s11  = rec2.siun[0] * rec2.siun[0];
	   s22  = rec2.siun[1] * rec2.siun[1];
	   s21  = rec2.siun[0] * rec2.siun[1] * rec2.r12;
	   s33  = rec2.siun[2] * rec2.siun[2];
	   s44  = rec2.siun[3] * rec2.siun[3];
	   s43  = rec2.siun[2] * rec2.siun[3] * rec2.r34;
	   sn2  = sn * sn;
	   cs2  = cs * cs;
           tqx  = (float)sqrt(s11 * cs2 + s33 * sn2);
           tqy  = (float)sqrt(s11 * sn2 + s33 * cs2);
           tqxp = (float)sqrt(s22 * cs2 + s44 * sn2);
           tqyp = (float)sqrt(s22 * sn2 + s44 * cs2); 
	   r12x = (s21 * cs2 + s43 * sn2) / ( tqx * tqxp);
	   r34y = (s21 * sn2 + s43 * cs2) / ( tqy * tqyp);
           qx[index]   = tqx;
           qy[index]   = tqy;
	   qxp[index]  = tqxp * r12x * fac;
	   qyp[index]  = tqyp * r34y * fac;

//	   bunch length (not rotated)

	   qdl[index]  = rec2.siun[4];
	   qdlp[index] = rec2.siun[5] * rec2.r56 * fac2 ;

//         dispersion trajectory (backrotated & normalized with R66 [because of acceleration])

           d   = rec2.rtmx[2] / rec2.rtmx[12];
           dp  = rec2.rtmx[5] / rec2.rtmx[12] * fac;
           dy  = rec2.rtmx[8] / rec2.rtmx[12];
           dyp = rec2.rtmx[11] / rec2.rtmx[12] * fac;
           qd[index]   = d  * cs - dy  * sn;
           qdy[index]  = d  * sn + dy  * cs;
           qdp[index]  = dp * cs - dyp * sn;
           qdyp[index] = dp * sn + dyp * cs;

           prevtype    = type;
           prevleng    = qz[index];
           break;
         }
         case 20:    // rotation angle
         { totrot    += rec2.data[0];
           float arg  = totrot * (float)3.14159 / (float)180.0;
           cs         = (float)cos(arg);
           sn         = (float)sin(arg);
           break;
         }
         default:
           prevtype = type;
           break;
      }
      if (index >= nr-1) break;
   }
   Rewind();
   totrot = (float)0.0;
   cs     = (float)1.0;
   sn     = (float)0.0;
   index++;

// equidistant interpolation

   int indmax = np;
   for (int i=0; i<=np; i++)
   {   float z = zmin + dz * (float) i;
       if (z > qz[index-1])
       {  indmax = i - 1;
          break;
       }
       x[i]  = splint(qz,qx,qxp,index,z);
       y[i]  = splint(qz,qy,qyp,index,z);
       p[i]  = splint(qz,qd,qdp,index,z);
       py[i] = splint(qz,qdy,qdyp,index,z);
       dl[i] = splint(qz,qdl,qdlp,index,z);
   }

   delete []qz; // [] found with Bounds Checker
   delete []qx;
   delete []qxp;
   delete []qy;
   delete []qyp;
   delete []qd;
   delete []qdp;
   delete []qdy;
   delete []qdyp;
   delete []qdl;
   delete []qdlp;
   return indmax;
}

int TransGrafFile :: GetRays(int nrf, float zmin, float zmax, int np,
                                      float x[], float y[])
{
   REC2 rec2;

   int nr = GetNumberOfElements();
   if (nr == 0) return 0;
   float fac = (float)1.0;
   if (strcmp(GetDimension(0),CMS) == 0) fac = (float)0.1;
   if (strcmp(GetDimension(7),CMS) == 0) fac = fac * (float)0.01;
   int type;
   int index      = -1;
   int prevtype   = 1;
   float prevleng = (float)-99.0;
   float xx, xxp, yy, yyp;

   float dz    = (zmax - zmin) / (float) (np - 1);
   float *qz   = new float[nr];
   float *qx   = new float[nr];
   float *qxp  = new float[nr];
   float *qy   = new float[nr];
   float *qyp  = new float[nr];

   FILE *fp = GetFilePointer(nrf);

   while (fread(&rec2,sizeof(REC2),1,fp) == 1)
   {  
      adjust(fp);
      type = (int)rec2.type;
      switch (type)
      {  case 1:
         case 2:
         case 3:
         case 4:
         case 5:
         case 7:
         case 11:
         case 12:
         case 14:
         case 18:
         case 19:
         case 21:
	 case 30:
         { index++;
           if (type == 12 && prevtype == 1)  index--;  // beam special
           if (type == 14 && prevtype == 1)  index--;  // beam special
           if (type == 14 && prevtype == 12) index--;  // beam special
           qz[index]  = rec2.along;
           if (rec2.along-prevleng < (float)0.0009 && 
                                           index > 0)  // zero length ->
              qz[index] += (float)0.0001;              // add a little bit
           xx  = rec2.cenun[0];
           xxp = rec2.cenun[1] * fac;
           yy  = rec2.cenun[2];
           yyp = rec2.cenun[3] * fac;
           qx[index]  = xx  * cs - yy  * sn;
           qy[index]  = xx  * sn + yy  * cs;
           qxp[index] = xxp * cs - yyp * sn;
           qyp[index] = xxp * sn + yyp * cs;

           prevtype    = type;
           prevleng    = qz[index];
           break;
         }
         case 20:    // rotation angle
         { totrot    += rec2.data[0];
           float arg  = totrot * (float)3.14159 / (float)180.0;
           cs         = (float)cos(arg);
           sn         = (float)sin(arg);
           break;
         }
         default:
           prevtype = type;
           break;
      }
      if (index >= nr-1) break;
   }
   Rewind();
   totrot = (float)0.0;
   cs     = (float)1.0;
   sn     = (float)0.0;
   index++;

// equidistant interpolation

   int indmax = np;
   for (int i=0; i<=np; i++)
   {   float z = zmin + dz * (float) i;
       if (z > qz[index-1])
       {  indmax = i - 1;
          break;
       }
       x[i]  = splint(qz,qx,qxp,index,z);
       y[i]  = splint(qz,qy,qyp,index,z);
   }

   delete []qz;
   delete []qx;
   delete []qxp;
   delete []qy;
   delete []qyp;

   return indmax;
}

int TransGrafFile :: GetCSTrajectories(int nrf, float zmin, float zmax, int np,
                             float cx[], float sx[], float cy[], float sy[])
{
   REC2 rec2;

   int nr = GetNumberOfElements();
   if (nr == 0) return 0;
   float fac = (float)1.0;
   if (strcmp(GetDimension(0),CMS) == 0) fac = (float)0.1;
   if (strcmp(GetDimension(7),CMS) == 0) fac = fac * (float)0.01;
   int type;
   int index      = -1;
   int prevtype   = 1;
   float prevleng = (float)-99.0;
   float cxx, cxxp, cyy, cyyp;
   float sxx, sxxp, syy, syyp;
   float xx, xxp, yy, yyp;

   float dz    = (zmax - zmin) / (float) (np - 1);
   float *qz    = new float[nr];
   float *qcx   = new float[nr];
   float *qcxp  = new float[nr];
   float *qcy   = new float[nr];
   float *qcyp  = new float[nr];
   float *qsx   = new float[nr];
   float *qsxp  = new float[nr];
   float *qsy   = new float[nr];
   float *qsyp  = new float[nr];

   FILE *fp = GetFilePointer(nrf);

   while (fread(&rec2,sizeof(REC2),1,fp) == 1)
   {  
      adjust(fp);
      type = (int)rec2.type;
      switch (type)
      {  case 1:
         case 2:
         case 3:
         case 4:
         case 5:
         case 11:
         case 12:
         case 14:
         case 18:
         case 19:
         case 21:
	 case 30:
         { index++;
           if (type == 12 && prevtype == 1)  index--;  // beam special
           if (type == 14 && prevtype == 1)  index--;  // beam special
           if (type == 14 && prevtype == 12) index--;  // beam special
           qz[index]  = rec2.along;
           if (rec2.along-prevleng < (float)0.0009 &&
                                           index > 0)  // zero length ->
              qz[index] += (float)0.0001;              // add a little bit

//           if (rec2.along==0.0 && (type==1 || type==12 || type==14)) (5-12-2005, Schwenke)
           if (rec2.along>=0.0 && (type==1 || type==12 || type==14))
           {  xx   = rec2.siun[0];
              xxp  = rec2.siun[1];
              yy   = rec2.siun[2];
              yyp  = rec2.siun[3]; 
           }

           cxx  = rec2.rtmx[0];
           cyy  = rec2.rtmx[6];
           qcx[index]  = (cxx  * cs - cyy  * sn) * xx;
           qcy[index]  = (cxx  * sn + cyy  * cs) * yy;

           sxx  = rec2.rtmx[1];
           syy  = rec2.rtmx[7];
           qsx[index]  = (sxx  * cs - syy  * sn) * xxp;
           qsy[index]  = (sxx  * sn + syy  * cs) * yyp;

           cxxp = rec2.rtmx[3];
           cyyp = rec2.rtmx[9];
           qcxp[index] = (cxxp * cs - cyyp * sn) * xx * fac;
           qcyp[index] = (cxxp * sn + cyyp * cs) * yy * fac;

           sxxp = rec2.rtmx[4];
           syyp = rec2.rtmx[10];
           qsxp[index] = (sxxp * cs - syyp * sn) * xxp * fac;
           qsyp[index] = (sxxp * sn + syyp * cs) * yyp * fac;

           prevtype    = type;
           prevleng    = qz[index];
           break;
         }
         case 20:    // rotation angle
         { totrot    += rec2.data[0];
           float arg  = totrot * (float)3.14159 / (float)180.0;
           cs         = (float)cos(arg);
           sn         = (float)sin(arg);
           break;
         }
         default:
           prevtype = type;
           break;
      }
      if (index >= nr-1) break;
   }
   Rewind();
   totrot = (float)0.0;
   cs     = (float)1.0;
   sn     = (float)0.0;
   index++;

// equidistant interpolation

   int indmax = np;
   for (int i=0; i<=np; i++)
   {   float z = zmin + dz * (float) i;
       if (z > qz[index-1])
       {  indmax = i - 1;
          break;
       }
       cx[i]  = (float)fabs(splint(qz,qcx,qcxp,index,z));
       cy[i]  = (float)fabs(splint(qz,qcy,qcyp,index,z));
       sx[i]  = (float)fabs(splint(qz,qsx,qsxp,index,z));
       sy[i]  = (float)fabs(splint(qz,qsy,qsyp,index,z));
   }

   delete []qz;
   delete []qcx;
   delete []qcxp;
   delete []qcy;
   delete []qcyp;
   delete []qsx;
   delete []qsxp;
   delete []qsy;
   delete []qsyp;

   return indmax;
}

int TransGrafFile :: GetNextDrift(float& z, char *label)
{
   REC2 rec2;

   int nr = GetNumberOfElements();
   if (nr == 0) return 0;

   FILE *fp = GetFilePointer();

   while (fread(&rec2,sizeof(REC2),1,fp) == 1)
   {  
      adjust(fp);
      if (rec2.type == 3)
      {  z = rec2.along;
         strncpy(label,&rec2.label[0],4);
         if (strcmp(label,String(IDS_BLANK_LB)) == 0)
            continue;
         return 1;
      }
   }
   return 0;
}

int TransGrafFile :: GetNextSlit(float& z, float& x, float& y, char *label)
{
   REC2 rec2;

   int nr = GetNumberOfElements();
   if (nr == 0) return 0;

   FILE *fp = GetFilePointer();

   while (fread(&rec2,sizeof(REC2),1,fp) == 1)
   {  
      adjust(fp);
      if (rec2.type == 6)
      {  int icon1 = (int)rec2.data[0];
         int icon2 = (int)rec2.data[2];
         if (icon1 != 1 && icon1 != 3) continue;
         x = y = (float)-99.0;
         if (icon1 == 1)
            x = rec2.data[1];
         if (icon1 == 3)
            y = rec2.data[1];
         if (icon2 == 1)
            x = rec2.data[3];
         if (icon2 == 3)
            y = rec2.data[3];
         z = rec2.along;
         strncpy(label,&rec2.label[0],4);
         return 1;
      }
   }
   return 0;
}

int TransGrafFile :: GetNextMagnet(int& index, float& z, float& l, float& B,
                                   float& a, char *label, int& acctype)
{
   REC2 rec2;
   int  type, arg;

   acctype = 0;
   int nr = GetNumberOfElements();
   if (nr == 0) return 0;

   FILE *fp = GetFilePointer();

   while (fread(&rec2,sizeof(REC2),1,fp) == 1)
   {  
      adjust(fp);
      type = (int)rec2.type;
      if (type == 16)
      {  switch ((int)rec2.data[0])
         {  case 5:  hap = rec2.data[1]; break;
            default: break;
         }
      }
      if (type == 20)
      {
         totrot    += rec2.data[0];
         arg  = int(totrot);
      }
      if (type == 4  || type == 5  || type == 11 ||
          type == 18 || type == 19 || type == 21)
      {  index = 0;
         if (type == 4 || type == 21) index = 1;  // BEND and WIEN
         z = rec2.along;
         l = rec2.data[0];
         B = rec2.data[1];
         if (type == 5 || type == 18)
            a = rec2.data[2];
         else
         {
            a = hap;
	    if (arg == 90 || arg == -90 || arg == 270)
	    a = float(-1.0) * a;
         }
         strncpy(label,&rec2.label[0],4);
         if (type == 11)                    // accelerator special
         {
            long curpos = ftell(fp);        // remember current position
            while (fread(&rec2,sizeof(REC2),1,fp) == 1) // search next type 11 on next records
	    {
	        adjust(fp);
	        nexttype = (int)rec2.type;
	        if (nexttype == 11) break;   // SCINT (typecode 30) might be present in between
		if (nexttype == 3)  break;   // drift as terminator between sections
	    }
            fseek(fp,curpos,0);             // reset to old position
            if (prevtype == 11 && nexttype == 11)
               acctype = 2; // intermediate
            else if (prevtype != 11)
               acctype = 1; // first
            else
	    {
                acctype = 3; // last
		nexttype = 0 ;
		type = 0 ;
	    }
         }
         prevtype = type;
         return 1;
      }
   }
   totrot = (float)0.0;
   prevtype = type;
   return 0;
}

int TransGrafFile :: GetNextFit(int& index, float& z, float& val)
{
   REC5 rec5;
   int  type;

   int nr = GetNumberOfElements();
   if (nr == 0) return 0;

   FILE *fp = GetFilePointer();

   while (fread(&rec5,sizeof(REC5),1,fp) == 1)
   {  
      adjust(fp);
      type = (int)rec5.type;
      if (type == 10)
      {  if ((rec5.f1 == 1.0 && rec5.f2 == 1.0) || 
             (rec5.f1 == 3.0 && rec5.f2 == 3.0) ||
             (rec5.f1 == 5.0 && rec5.f2 == 5.0) )
         {  index = 1 + (int)(rec5.f2 / 2.0);
	    if (index == 3) index = 5; // BUNL
            val   = rec5.con;
            z     = rec5.z;
            return 1;
         }
         else if (rec5.f1 == 0.0 && (rec5.f2 == 1.0 || rec5.f2 == 3.0))
         {  index = 3 + (int)(rec5.f2 / 2.0);
            val   = rec5.con;
            z     = rec5.z;
            return 1;
         }
         else
           continue;
      }
   }
   return 0;
}

//////////////////////////////////// EOF //////////////////////////////////////
