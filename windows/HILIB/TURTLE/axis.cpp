
///////////////////////////////////////////////////////////////////////////////
/////////////////////// Implementation of class Axis //////////////////////////
//////////////// Code written by Stefan Ritt, October 1992 ////////////////////
///////// Modified and adapted for usage with WIN++ by Urs Rohrer (PSI) ///////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

#define GraMove(a,b)   opDev->MoveTo(a,b)
#define GraDraw(a,b)   opDev->LineTo(a,b)
#define GraText(a,b,c) opDev->PosStr(a,b,c)

extern "C"
{
   double power (double x, double y)
   {
	return pow(x,y);
   }
}

///////////////////////////////////////////////////////////////////////////////

Axis :: Axis(DisplayDevice *opdev, int th)
      : Object(120)
{
  TextHeight  = th;
  opDev       = opdev;
}

VOID Axis :: HorzAxis(float Ax1, float Ay1, float Awidth,
                      float Aminor, float Amajor, float Atext, float Alabel,
                      float Axmin, float Axmax, float min, BOOL Logarithmic)
{
  double dx, int_dx, frac_dx, x_act, label_dx, major_dx, x_screen, maxwidth;
  int    tick_base, major_base, n_sig1, n_sig2;
  char   str[80];
  float  base[] = {(float)1.,(float)2.,(float)5.,
                   (float)10.,(float)20.,(float)50.,
                   (float)100.,(float)200.,(float)500.,
                   (float)1000.,(float)2000.,(float)5000.,
                   (float)10000.,(float)20000.,(float)50000.,
                   (float)100000.,(float)200000.,(float)500000.
                  };

  x1           = Ax1;
  y1           = Ay1;
  width        = Awidth;
  minor        = Aminor;
  major        = Amajor;
  text         = Atext;
  label        = Alabel;
  xmin         = Axmin;
  xmax         = Axmax;
  bLogarithmic = Logarithmic;
  MinTickDist  = min;

  if (Axmax <= Axmin)
  {  sprintf(str, "Wrong axis limits: Xmin=%lf Xmax=%lf", Axmin, Axmax);
     DispObjectMsg("%s",str);
     return;
  }

  if (xmin != 0.0 && !bLogarithmic)
     xmin -= (xmax - xmin) / float(1000000.0);

  if (Awidth <= 0)
  {  sprintf(str, "Axis width (%lf) must be positive", Awidth);
     DispObjectMsg("%s",str);
     return;
  }

  if (bLogarithmic && Axmin <= 0)
  {  sprintf(str, "Non-positive logarithmic axis limit: Xmin=%lf", Axmin);
     DispObjectMsg("%s",str);
     return;
  }

  minx = Ax1;
  maxx = Ax1 + Awidth;
  miny = maxy = y1;
  miny = MIN(miny, y1 + minor);
  miny = MIN(miny, y1 + major);
  miny = MIN(miny, y1 + label);
  maxy = MAX(maxy, y1 + minor);
  maxy = MAX(maxy, y1 + major);
  maxy = MAX(maxy, y1 + label);

  if (bLogarithmic)
    dx = power( 10, floor(log(xmin)/LN10-0.0001));
  else
    dx = (xmax - xmin)/ (float) (width/MinTickDist);

  if (bLogarithmic)
  {
    label_dx = dx * 10;
    major_dx = dx * 10;
  }
  else
  {
    frac_dx = modf(log(dx)/LN10, &int_dx);
    if (frac_dx <0) { frac_dx+=1; int_dx -=1; }

    tick_base = frac_dx < LOG2 ? 1 :
		frac_dx < LOG5 ? 2 : 3;
    major_base = tick_base + 1;

    /* rounding up of dx, label_dx */
  
    dx = power(10, int_dx) * base[tick_base];
    major_dx = power(10, int_dx) * base[major_base];
    label_dx = major_dx;
  }

  if (bLogarithmic)
    n_sig1 = 4;
  else
  { /* number of significant digits */

    n_sig1 = (int) (xmin != 0 ? ceil(log(fabs(xmin)/label_dx)/LN10) : 0);
    n_sig2 = (int) (xmax != 0 ? ceil(log(fabs(xmax)/label_dx)/LN10) : 0);
    n_sig1 = MAX(n_sig1, n_sig2);
    n_sig1 = MAX(n_sig1, 4);

    /* determination of maximal width of labels */

    sprintf(str,"%1.*lG", n_sig1, floor(xmin/dx)*dx);
    maxwidth = TextHeight/2 * strlen(str);
    sprintf(str,"%1.*lG", n_sig1, floor(xmax/dx)*dx);
    maxwidth = MAX( (int)maxwidth, (int)(TextHeight/2 * strlen(str)));
    sprintf(str,"%1.*lG", n_sig1, floor(xmax/dx)*dx+label_dx);
    maxwidth = MAX( (int)maxwidth, (int)(TextHeight/2 * strlen(str)));

    /* increasing label_dx, if labels would overlap */

    while (maxwidth > 0.7 * label_dx/(xmax-xmin)*width)
    {
      major_base++;
      if (major_base > 17)
          break;
      label_dx = power(10, int_dx) * base[major_base];
      if (major_base % 3 == 2)
        major_dx = label_dx;
    }
  }

  x_act = floor(xmin/dx)*dx;

  GraMove(int(x1),int(y1));
  GraDraw(int(x1+width),int(y1));

  do
  {
    if (bLogarithmic)
      x_screen = (log(x_act)-log(xmin))/(log(xmax)-log(xmin))*width + x1;
    else
      x_screen = (x_act-xmin)/(xmax-xmin)*width + x1;

    if (x_screen > x1 + width + 0.001) break;

    if (x_screen >= x1)
    { 
      if ( fabs(floor(x_act/major_dx+0.5) - x_act/major_dx) < dx / major_dx / 10.0 )
      {
        if ( fabs(floor(x_act/label_dx+0.5) - x_act/label_dx) < dx / label_dx / 10.0 )
        { 
          /**** label tick mark ****/

          GraMove(int(x_screen),int(y1));
          GraDraw(int(x_screen),int(y1+text));

          /**** label ****/

          if (label != 0)
          {  sprintf(str, "%1.*lG", n_sig1, x_act);
             opDev->SetTextAlignment(TA_CENTER);
             GraText(int(x_screen),int(y1+label),str);
             opDev->SetTextAlignment(TA_LEFT);
          }
        }
        else                     // major tick mark
        {
           GraMove(int(x_screen),int(y1));
           GraDraw(int(x_screen),int(y1+major));
        }
        if (bLogarithmic)
	{  dx *= 10;
           major_dx *= 10;
           label_dx *= 10;
        }
      }
      else                      // minor tick mark
      {  GraMove(int(x_screen),int(y1));
         GraDraw(int(x_screen),int(y1+minor));
      }
    }

    x_act+=dx;

    /* supress 1.23E-17 ... */

    if (fabs(x_act) < dx/100) x_act=0;

  } while(1);
}

VOID Axis :: VertAxis(float Ax1, float Ay1, float Awidth,
                      float Aminor, float Amajor, float Atext, float Alabel,
                      float Aymin, float Aymax, float min, BOOL Logarithmic)
{
  double dy, int_dy, frac_dy, y_act, label_dy, major_dy, y_screen;
  int    tick_base, major_base, n_sig1, n_sig2;
  char   str[80];
  float  base[] = {(float)1.,(float)2.,(float)5.,
                   (float)10.,(float)20.,(float)50.,
                   (float)100.,(float)200.,(float)500.,
                   (float)1000.,(float)2000.,(float)5000.,
                   (float)10000.,(float)20000.,(float)50000.,
                   (float)100000.,(float)200000.,(float)500000.
                  };

  x1           = Ax1;
  y1           = Ay1;
  width        = Awidth;
  minor        = Aminor;
  major        = Amajor;
  text         = Atext;
  label        = Alabel;
  ymin         = Aymin;
  ymax         = Aymax;
  bLogarithmic = Logarithmic;
  MinTickDist  = min;

  if (Aymax <= Aymin)
  { sprintf(str, "Wrong axis limits: Ymin=%lf Ymax=%lf", Aymin, Aymax);
    DispObjectMsg("%s",str);
    return;
  }

  if (ymin != 0.0 && !bLogarithmic)
     ymin -= (ymax - ymin) / float(1000000.0);

  if (bLogarithmic && Aymin <= 0)
  { sprintf(str, "Non-positive logarithmic axis limit: Xmin=%lf", Aymin);
    DispObjectMsg("%s",str);
    return;
  }

  minx = x1;
  maxx = x1;
  miny = y1;
  maxy = y1 + width;
  minx = MIN(minx, x1 + minor);
  minx = MIN(minx, x1 + major);
  minx = MIN(minx, x1 + label);
  maxx = MAX(maxx, x1 + minor);
  maxx = MAX(maxx, x1 + major);
  maxx = MAX(maxx, x1 + label);

  if (bLogarithmic)
    dy = power( 10, floor(log(ymin)/LN10-0.0001));
  else
    dy = (ymax - ymin)/ (float) (width/MinTickDist);

  if (bLogarithmic)
    label_dy = major_dy = dy * 10;
  else
  { frac_dy = modf(log(dy)/LN10, &int_dy);
    if (frac_dy <0) { frac_dy+=1; int_dy -=1; }

    tick_base = frac_dy < LOG2 ? 1 :
                frac_dy < LOG5 ? 2 : 3;
    major_base = tick_base + 1;

    dy = power(10, int_dy) * base[tick_base];
    major_dy = power(10, int_dy) * base[major_base];
    label_dy = major_dy;
  }

  if (bLogarithmic)
    n_sig1 = 4;
  else
  { /* number of significant digits */

    n_sig1 = (int) (ymin != 0 ? ceil(log(fabs(ymin)/label_dy)/LN10) : 0);
    n_sig2 = (int) (ymax != 0 ? ceil(log(fabs(ymax)/label_dy)/LN10) : 0);
    n_sig1 = MAX(n_sig1, n_sig2);
    n_sig1 = MAX(n_sig1, 4);

    /* increasing label_dy, if labels would overlap */

    while (label_dy/(ymax-ymin)*width < 1.5*TextHeight)
    {
      major_base++;
      if (major_base > 17)
          break;
      label_dy = power(10, int_dy) * base[major_base];
      if (major_base % 3 == 2)
        major_dy = label_dy;
    }
  }

  y_act = floor(ymin/dy)*dy;

  GraMove(int(x1),int(y1));
  GraDraw(int(x1),int(y1-width));

  do
  { if (bLogarithmic)
      y_screen = (log(y_act)-log(ymin))/(log(ymax)-log(ymin))*width + y1;
    else
      y_screen = (y_act-ymin)/(ymax-ymin)*width + y1;

    if (y_screen > y1 + width + 0.001) break;

    if (y_screen >= y1)
    { if ( fabs(floor(y_act/major_dy+0.5) - y_act/major_dy) <
           dy / major_dy / 10.0 )
      {  if ( fabs(floor(y_act/label_dy+0.5) - y_act/label_dy) <
           dy / label_dy / 10.0 )
         {
          /**** label tick mark ****/

          GraMove(int(x1),int(2*y1-y_screen));
          GraDraw(int(x1+text),int(2*y1-y_screen));

          /**** label ****/

          if (label != 0)
          {  sprintf(str, "%1.*lG", n_sig1, y_act);
             if (label < 0.0)
                opDev->SetTextAlignment(TA_RIGHT);
             else
                opDev->SetTextAlignment(TA_LEFT);
             GraText(int(x1+label),int(2*y1-TextHeight/2-y_screen),str);
             opDev->SetTextAlignment(TA_TOP);
          }
        }
        else                       // major tick mark
        {  GraMove(int(x1),int(2*y1-y_screen));
           GraDraw(int(x1+major),int(2*y1-y_screen));
        }

        if (bLogarithmic)
        {  dy *= 10;
           major_dy *= 10;
           label_dy *= 10;
        }
      }
      else                         //minor tick mark
      {  GraMove(int(x1),int(2*y1-y_screen));
         GraDraw(int(x1+minor),int(2*y1-y_screen));
      }
    }

    y_act+=dy;

    /* supress 1.23E-17 ... */

    if (fabs(y_act) < dy/100) y_act=0;

  } while(1);

}

Axis :: ~Axis()
{
}

////////////////////////////////// EOF ////////////////////////////////////////
