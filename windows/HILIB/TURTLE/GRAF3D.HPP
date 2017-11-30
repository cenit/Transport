///////////////////////////////////////////////////////////////////////////////
//////////////////////////// Graf3D class implementation //////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef GRAF3D_HPP
#define GRAF3D_HPP

#define DIM 100

class Graf3D : public Object
{
    DisplayDevice *opDev;
    LONG ixx0, iyy0, ixdmx;
    float xdmx, ydmx;
    LONG ix1, iy1, iz1, iya, icost, isint, icosp, isinp, isk;
    BOOL color;
    float ex, ey, ez;
    float z1, z2, dx, dy;
    UINT R, G, B;
  protected:
    float CosAng(VOID);
  public:
    Graf3D(BOOL, DisplayDevice *, int, int, int, int, int, int, int);
    VOID SetLight(int, int);
    VOID Graf3Di(int, int, int, int, int, int, int, int);
    VOID Graf3Dp(int, int, int, int&, int&);
    VOID Graf3Dg(int *, int *, int[DIM][DIM], int, int);
    VOID Graf3Dl(int *, int *, int[DIM][DIM], int, int);
    ~Graf3D(VOID);
};

#endif

///////////////////////////////////// EOF /////////////////////////////////////
