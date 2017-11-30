///////////////////////////////////////////////////////////////////////////////
///////////////////////// Definition for class Axis ///////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef AXIS_HPP
#define AXIS_HPP

#define LN10 2.302585094
#define LOG2 0.301029996
#define LOG5 0.698970005

class far Axis : public Object
{ DisplayDevice *opDev;
  float x1, y1, width, minor, major, text, label;
  float xmin, ymin, xmax, ymax, minor_step, major_step, MinTickDist;
  float minx, miny, maxx, maxy;
  BOOL  bLogarithmic;
  int   TextHeight;
public:
  Axis(DisplayDevice *, int);
  VOID HorzAxis(float, float, float, float, float, float,
                                            float, float, float, float, BOOL);
  VOID VertAxis(float, float, float, float, float, float,
                                            float,float, float, float, BOOL);
  ~Axis();
};

#endif

///////////////////////////////// EOF /////////////////////////////////////////
