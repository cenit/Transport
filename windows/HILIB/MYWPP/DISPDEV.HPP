///////////////////////////////////////////////////////////////////////////////
//////////////////////// Definition of class Device ///////////////////////////
///////////////////////////////////////////////////////////////////////////////

class Device : public Object
{ 
  public:
    virtual HDC GetDC( void ) = 0;
    Device(UINT id);
    ~Device(void);
};

///////////////////////////////////////////////////////////////////////////////
//////////////////// Definition of class DisplayDevice ////////////////////////
///////////////////////////////////////////////////////////////////////////////

class DisplayDevice : public Device
{
     RECT   rc_;
     HFONT  hDfltFont;
     HPEN   hDfltPen;
     HBRUSH hDfltBrush;
     Font   *opFont;
     Pen    *opPen;
     Brush  *opBrush;
   protected:
     VOID SetExtents(RECT rc);
   public:
     DisplayDevice(UINT id);
     ~DisplayDevice(void);
     RECT GetExtents(void);
     int  GetUserWidth(void);
     int  GetUserHeight(void);
     int  GetCharWidth(char);
     int  GetCharHeight(char);
     VOID PutEllipse(int left, int top, int right, int bottom);
     VOID MoveTo(int x, int y);
     VOID LineTo(int x, int y);
     VOID PosStr(int x, int y, char *str);
     VOID SetTextColor(DWORD rgbColor);
     VOID SetBackMode(int mode);
     VOID SetBackColor(DWORD rgbColor);
     VOID PutRectangle(int left, int top, int right, int bottom);
     VOID PutPolygon(LPPOINT pts, int n);
     VOID PutPolyline(LPPOINT pts, int n);
     Brush *SetBrush(Brush *opBrush);
     Pen   *SetPen(Pen *opPen);
     Font  *SetFont(Font *opFont);
     VOID SetTextAlignment(UINT walign);
     VOID CopyBits(POINT Pos, POINT Size, POINT Org, HDC hdcMem);
     VOID StretchBits(POINT DestOrg,POINT DestSize,
                      POINT SrcOrg,POINT SrcSize,HDC hdcMem);
};

//////////////////////////////////// EOF //////////////////////////////////////
