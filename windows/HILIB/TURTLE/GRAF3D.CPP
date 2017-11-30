///////////////////////////////////////////////////////////////////////////////
//////////////////////////// Graf3D class implementation //////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"


Graf3D :: Graf3D(BOOL c, DisplayDevice *opD,
                 int gx0, int gy0, int gxlen, int gylen, int r, int g, int b)
        : Object(123)
{
// geometric window

   color = c;
   opDev = opD;
   ixx0  = gx0;
   iyy0  = gy0;
   xdmx  = (float)gxlen;
   ydmx  = (float)gylen;
   ixdmx = gxlen / 2;

// color

   R = r;
   G = g;
   B = b;
}

VOID Graf3D :: Graf3Di(int xmi, int xma, int ymi, int yma,
                       int zmi, int zma, int phi, int tet)
{
// initialize parameters;

   float ph    = float(0.0174533) * phi;
   float th    = float(0.0174533) * tet;
   float cosph = (float)cos(ph);
   float sinph = (float)sin(ph);
   float x1    = float(0.5) * (xmi + xma);
   float y1    = float(0.5) * (ymi + yma);
   float z1    = (float)zmi;
   float a     = (float)sqrt((xma-x1)*(xma-x1) + (yma-y1)*(yma-y1));   

// parallel projection

   float xm    = a;
   float costp = (float)cos(th);
   float sintp = (float)sin(th);
   float yb    = MAX((zma-zmi)*costp+a*sintp,(zma-zmi)*costp-a*sintp);
   float ya    = MIN(a*sintp, -a*sintp);
   float sk    = MIN(ydmx/(yb-ya),xdmx/(float(2.0)*xm));

// integer parameters for Gra3Dp

   ix1 = (long)x1;
   iy1 = (long)y1;
   iz1 = (long)z1;
   iya = (long)ya;
   icost = (int)(8192. * costp);
   isint = (int)(8192. * sintp);
   icosp = (int)(8192. * cosph);
   isinp = (int)(8192. * sinph);
   isk   = (int)(8192. * sk);
}

VOID Graf3D :: Graf3Dp(int ixx, int iyy, int izz, int &nx, int &ny)
{
// transformation of 3D-points into (x/y)-plane

   LONG ix  = ixx - ix1;
   LONG iy  = iyy - iy1;
   LONG iz  = izz - iz1;
   LONG iw  = (iy*icosp-ix*isinp) / 8192;
   LONG ixw = (ix*icosp+iy*isinp) / 8192;
   LONG iyw = (iw*isint+iz*icost) / 8192;
   nx = (int) (ixdmx + (ixw * isk) / 8192 + ixx0);
   ny = (int) (-((iyw - iya) * isk) / 8192 + iyy0);
}

VOID Graf3D :: Graf3Dg(int *x, int *y, int z[DIM][DIM], int m, int n)
{
   Brush *opBrush[10], *opOld;
   POINT Poly[5];

   float a1, a2, val;
   int   ax,ay,bx,by,cx,cy,ddx,ddy;
   int   i, j, k, bnr;
   int   zz1, zz2, zz3, zz4;
   int   u, v, w, a, b;

   int (*ix)[DIM] = new int[DIM][DIM];
   int (*iy)[DIM] = new int[DIM][DIM];

   static UINT inten[10] = {114,130,146,162,178,194,210,226,242,255};

   if (color)
   {  for (int i=0; i<10; i++)
        opBrush[i] = new Brush(RGB((inten[i]*R)/255,
                                   (inten[i]*G)/255,
                                   (inten[i]*B)/255));
      opOld = opDev->SetBrush(opBrush[0]);
   }
   else
   {  for (int i=0; i<10; i++)
        opBrush[i] = new Brush(RGB(inten[i],inten[i],inten[i]));
      opOld = opDev->SetBrush(opBrush[0]);
   }

   Graf3Dp(x[0]  ,y[0]  ,0,ax,ay);
   Graf3Dp(x[m-1],y[0]  ,0,bx,by);
   Graf3Dp(x[m-1],y[n-1],0,cx,cy);
   Graf3Dp(x[0]  ,y[n-1],0,ddx,ddy);

   Poly[0].x = ax;
   Poly[0].y = ay;
   Poly[1].x = bx;
   Poly[1].y = by;
   Poly[2].x = cx;
   Poly[2].y = cy;
   Poly[3].x = ddx;
   Poly[3].y = ddy;
   Poly[4].x = ax;
   Poly[4].y = ay;

   opDev->PutPolygon(Poly,5); // base plate

   for (j=0; j<n; j++)
   {  k = n - j - 1;
      for (i=0; i<m; i++)
      {  u = x[i];
         v = y[k];
         w = z[k][i];
         Graf3Dp(u,v,w,a,b);
         ix[i][k] = a;
         iy[i][k] = b;
      }
   }

   dx = float(x[1] - x[0]);
   dy = float(y[1] - y[0]);

   for (j=1; j<n; j++)
   {  k = n - j;
      for (i=0; i<m-1; i++)
      {
         Poly[0].x = ix[i][k];
         Poly[0].y = iy[i][k];
         Poly[1].x = ix[i+1][k];
         Poly[1].y = iy[i+1][k];
         Poly[2].x = ix[i+1][k-1];
         Poly[2].y = iy[i+1][k-1];
         Poly[3].x = ix[i][k-1];
         Poly[3].y = iy[i][k-1];
         Poly[4].x = ix[i][k];
         Poly[4].y = iy[i][k];

         zz1 = z[k][i];
         zz2 = z[k-1][i];
         zz3 = z[k][i+1];
         zz4 = z[k-1][i+1];
         z1  = float(zz1-zz3);
         z2  = float(zz1-zz2);
         a1  = CosAng();
         z2  = float(zz3-zz4);
         z1  = float(zz2-zz4);
         a2  = CosAng();
         val = (a1 + a2) / float(2.0);      // avarage of 2 cosines
         if (val < (float)0.0) val = (float)0.0;
         bnr = int (10.0 * (val - 0.001));  //  brush number: 0 ... 9
         opDev->SetBrush(opBrush[bnr]);
         opDev->PutPolygon(Poly,5);
      }
   }
   opDev->SetBrush(opOld);
   for (i=0; i<10; i++)
      delete opBrush[i];
   delete[] ix;
   delete[] iy;
}

VOID Graf3D :: SetLight(int phi, int thet)
{
    float fact = (float)0.0174533;

    phi = -phi;
    phi += 180;

    ex = (float)cos(float(thet)*fact) * (float)cos(float(phi)*fact);
    ey = (float)cos(float(thet)*fact) * (float)sin(float(phi)*fact);
    ez = (float)sin(float(thet)*fact);
}

float Graf3D :: CosAng(VOID)
{
//  first vector a

    float ax = dx;
    float ay = (float)0.0;
    float az = z1;

//  second vector b

    float bx = (float)0.0;
    float by = dy;
    float bz = z2;

//  normal vector [a x b]  (unity length)

    float nx = (ay*bz-az*by);
    float ny = (az*bx-ax*bz);
    float nz = (ax*by-ay*bx);
    float normn = (float)sqrt(nx*nx+ny*ny+nz*nz);
    if (normn) normn = float(1.0)/normn; 
    nx *= normn;
    ny *= normn;
    nz *= normn;

//  cosine of angle between vector e and n

    return (nx*ex+ny*ey+nz*ez);
}

VOID Graf3D :: Graf3Dl(int *x, int *y, int z[DIM][DIM], int m, int n)
{
   Brush *opBrush[10], *opOld;
   POINT Poly[5];

   int        top, side, front;
   float      val;

   static UINT inten[10] = {114,130,146,162,178,194,210,226,242,255};

   int i, j, k;
   int ix1, iy1;
   int ix2, iy2;
   int ix3, iy3;
   int ix4, iy4;
   int nx2, ny2;
   int nx3, ny3;
   int nx4, ny4;
   int l;

   if (color)
   {  for (int i=0; i<10; i++)
        opBrush[i] = new Brush(RGB((inten[i]*R)/255,
                                   (inten[i]*G)/255,
                                   (inten[i]*B)/255));
      opOld = opDev->SetBrush(opBrush[0]);
   }
   else
   {  for (int i=0; i<10; i++)
        opBrush[i] = new Brush(RGB(inten[i],inten[i],inten[i]));
      opOld = opDev->SetBrush(opBrush[0]);
   }

// get illumination factor for each face direction

   dx  = (float)1.0; dy  = (float)1.0; z1  = (float)0.0; z2  = (float)0.0;
   val = CosAng();
   if (val < (float)0.0) val = (float)0.0;
   top = int (10.0 * (val - 0.001));  //  brush number: 0 ... 9

   dx  = (float)1.0; dy  = (float)1.0; z1  = (float)0.0; z2  = (float)1000.0;
   val = CosAng();
   if (val < (float)0.0) val = (float)0.0;
   front = int (10.0 * (val - 0.001));  //  brush number: 0 ... 9

   dx  = (float)1.0; dy  = (float)1.0; z1  = (float)1000.0; z2  = (float)0.0;
   val = CosAng();
   if (val < (float)0.0) val = (float)0.0;
   side = int (10.0 * (val - 0.001));  //  brush number: 0 ... 9

// scan box by box

   for (j=0; j<n-1; j++)
   {  k = n - j - 2;
      for (i=0; i<m-1; i++)
      {  l = z[k][i];
         Graf3Dp(x[i],  y[k+1],l,ix1,iy1);
         Graf3Dp(x[i+1],y[k+1],l,ix2,iy2);
         Graf3Dp(x[i+1],y[k],  l,ix3,iy3);
         Graf3Dp(x[i],  y[k],  l,ix4,iy4);
         Graf3Dp(x[i+1],y[k+1],0,nx2,ny2);
         Graf3Dp(x[i+1],y[k],  0,nx3,ny3);
         Graf3Dp(x[i],  y[k],  0,nx4,ny4);

//       draw top rhombus

         opDev->SetBrush(opBrush[top]);
         Poly[0].x = ix1;
         Poly[0].y = iy1;
         Poly[1].x = ix2;
         Poly[1].y = iy2;
         Poly[2].x = ix3;
         Poly[2].y = iy3;
         Poly[3].x = ix4;
         Poly[3].y = iy4;
         Poly[4].x = ix1;
         Poly[4].y = iy1;
         opDev->PutPolygon(Poly,5);

//       draw side wall

         opDev->SetBrush(opBrush[side]);
         Poly[0].x = ix2;
         Poly[0].y = iy2;
         Poly[1].x = nx2;
         Poly[1].y = ny2;
         Poly[2].x = nx3;
         Poly[2].y = ny3;
         Poly[3].x = ix3;
         Poly[3].y = iy3;
         Poly[4].x = ix2;
         Poly[4].y = iy2;
         opDev->PutPolygon(Poly,5);

//       draw front wall

         opDev->SetBrush(opBrush[front]);
         Poly[0].x = ix3;
         Poly[0].y = iy3;
         Poly[1].x = nx3;
         Poly[1].y = ny3;
         Poly[2].x = nx4;
         Poly[2].y = ny4;
         Poly[3].x = ix4;
         Poly[3].y = iy4;
         Poly[4].x = ix3;
         Poly[4].y = iy3;
         opDev->PutPolygon(Poly,5);
      }
   }
   opDev->SetBrush(opOld);
   for (i=0; i<10; i++)
      delete opBrush[i];
}


Graf3D :: ~Graf3D(VOID)
{
}

///////////////////////////////////// EOF /////////////////////////////////////
