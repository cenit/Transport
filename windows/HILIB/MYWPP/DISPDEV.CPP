///////////////////////////////////////////////////////////////////////////////
////////////////////// Implementation of class Device /////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

Device :: Device(UINT id) : Object(id)
{
}

Device :: ~Device()
{
}

///////////////////////////////////////////////////////////////////////////////
//////////////////// Implementation of class DisplayDevice ////////////////////
///////////////////////////////////////////////////////////////////////////////


DisplayDevice::DisplayDevice(UINT id) : Device(id)
{
   hDfltFont  = NULL;
   hDfltPen   = NULL;
   hDfltBrush = NULL;
   opFont     = NULL;
   opPen      = NULL;
   opBrush    = NULL;
}

DisplayDevice::~DisplayDevice(void)
{
}

VOID DisplayDevice::SetExtents(RECT rc)
{
   rc_ = rc;
}

RECT DisplayDevice::GetExtents(void)
{
   RECT rc = rc_;
   return rc;
}

int DisplayDevice::GetUserWidth(void)
{
   return (rc_.right - rc_.left);
}

int DisplayDevice::GetUserHeight(void)
{
   return (rc_.bottom - rc_.top);
}

int DisplayDevice::GetCharWidth(char )
{
   TEXTMETRIC tm;

   GetTextMetrics( GetDC(), &tm);
   return (tm.tmAveCharWidth);
}

int DisplayDevice::GetCharHeight(char )
{
   TEXTMETRIC tm;

   GetTextMetrics( GetDC(), &tm);
   return (tm.tmHeight);
//   return (tm.tmHeight + tm.tmExternalLeading);
}

VOID DisplayDevice::PutEllipse(int left, int top, int right, int bottom)
{
   Ellipse( GetDC(), left, top, right, bottom );
}

VOID DisplayDevice::MoveTo(int x, int y)
{
   POINT point;
   ::MoveToEx( GetDC(), x, y, &point);
}

VOID DisplayDevice::LineTo(int x, int y)
{
   ::LineTo( GetDC(), x, y );
}

VOID DisplayDevice::PosStr(int x, int y, char *str)
{
   TextOut( GetDC(), x, y, str, strlen(str) );
}

VOID DisplayDevice::SetTextColor(DWORD rgbColor)
{
   ::SetTextColor( GetDC(), rgbColor);
}

VOID DisplayDevice::SetBackColor(DWORD rgbColor)
{
   SetBkColor( GetDC(), rgbColor);
}

VOID DisplayDevice::SetBackMode(int mode)
{
   SetBkMode( GetDC(), mode);
}

VOID DisplayDevice::PutRectangle(int left, int top, int right, int bottom)
{
   Rectangle( GetDC(), left, top, right, bottom);
}

VOID DisplayDevice::PutPolygon(LPPOINT pt, int n)
{
   Polygon( GetDC(), pt, n);
}

VOID DisplayDevice::PutPolyline(LPPOINT pt, int n)
{
   Polyline( GetDC(), pt, n);
}

Brush *DisplayDevice::SetBrush(Brush *opNewBrush)
{
   Brush *opTemp = opBrush;
   opBrush = opNewBrush;

   if (opNewBrush == NULL)
   {   if (hDfltBrush) SelectObject(GetDC(),hDfltBrush);
       hDfltBrush = NULL;
   }
   else if (hDfltBrush == NULL)
      hDfltBrush = (HBRUSH)SelectObject(GetDC(),opNewBrush->GetHandle());
   else
      SelectObject(GetDC(),opNewBrush->GetHandle());

   return opTemp;
}

Pen *DisplayDevice::SetPen(Pen *opNewPen)
{
   Pen *opTemp = opPen;
   opPen = opNewPen;

   if (opNewPen == NULL)
   {   if (hDfltPen) SelectObject(GetDC(),hDfltPen);
       hDfltPen = NULL;
   }
   else if (hDfltPen == NULL)
      hDfltPen = (HPEN)SelectObject(GetDC(),opNewPen->GetHandle());
   else
      SelectObject(GetDC(),opNewPen->GetHandle());

   return opTemp;
}

Font *DisplayDevice::SetFont(Font *opNewFont)
{
   Font *opTemp = opFont;
   opFont = opNewFont;

   if (opNewFont == NULL)
   {   if (hDfltFont) SelectObject(GetDC(),hDfltFont);
       hDfltFont = NULL;
   }
   else if (hDfltFont == NULL)
      hDfltFont = (HFONT)SelectObject(GetDC(),opNewFont->GetHandle());
   else
      SelectObject(GetDC(),opNewFont->GetHandle());

   return opTemp;
}

VOID DisplayDevice::SetTextAlignment(UINT walign)
{
   SetTextAlign(GetDC(),walign);
}

VOID DisplayDevice::CopyBits(POINT Pos, POINT Size, POINT Org, HDC hdcMem)
{
   BitBlt( GetDC(),Pos.x,Pos.y,Size.x,Size.y,hdcMem,Org.x,Org.y,SRCCOPY);
}

VOID DisplayDevice::StretchBits(POINT DestOrg, POINT DestSize,
                                POINT SrcOrg,  POINT SrcSize,  HDC hdcMem)
{
   StretchBlt( GetDC(),DestOrg.x,DestOrg.y,DestSize.x,DestSize.y,
               hdcMem,SrcOrg.x,SrcOrg.y,SrcSize.x,SrcSize.y,SRCCOPY);
}

//////////////////////////////////// EOF //////////////////////////////////////
