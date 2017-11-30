///////////////////////////////////////////////////////////////////////////////
//////////////////////// implementation of class Font /////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

Font :: Font(LPCSTR FontName) : Object(IDO_FONT)
{
   strcpy(Name,FontName);
   memset(&lf,0,sizeof(LOGFONT));
   strcpy(lf.lfFaceName,Name);
   hfont = CreateFontIndirect(&lf);
}

Font :: Font(LPCSTR FontName, int flag, int flag2 ) : Object(IDO_FONT)
{
   strcpy(Name,FontName);
   memset(&lf,0,sizeof(LOGFONT));
   if (flag == 0)
      lf.lfWeight =         400 ;
   else
      lf.lfWeight =         700 ;
   strcpy(lf.lfFaceName,Name);
   
   hfont = CreateFontIndirect(&lf);
}

Font :: Font(LPCSTR FontName, BYTE CharSet) : Object(IDO_FONT)
{
   strcpy(Name,FontName);
   memset(&lf,0,sizeof(LOGFONT));
   lf.lfCharSet = CharSet ;
   strcpy(lf.lfFaceName,Name);
   hfont = CreateFontIndirect(&lf);
}

Font :: Font(void) : Object(IDO_FONT)
{
   memset(&lf,0,sizeof(LOGFONT));

   lf.lfHeight =         9 ;
   lf.lfWidth =          0 ;
   lf.lfEscapement =     0 ;
   lf.lfOrientation =    0 ;
   lf.lfWeight =         0 ;
   lf.lfItalic =         0 ;
   lf.lfUnderline =      0 ;
   lf.lfStrikeOut =      0 ;
   lf.lfCharSet =        OEM_CHARSET ;
   lf.lfOutPrecision =   OUT_DEFAULT_PRECIS ;
   lf.lfClipPrecision =  CLIP_DEFAULT_PRECIS ;
   lf.lfQuality =        DEFAULT_QUALITY ;
   lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN ;
   lstrcpy(lf.lfFaceName, "FixedSys" ) ;

   hfont = CreateFontIndirect(&lf);
}

Font :: ~Font(void)
{
   DeleteObject(hfont);
}

VOID Font :: SetPointSize(UINT size, DisplayDevice *opDev)
{
    int wLogiY = GetDeviceCaps(opDev->GetDC(),(int)LOGPIXELSY);
    if (wLogiY > 0)
#ifndef WIN32
       lf.lfHeight = (size * wLogiY) / 72;
#else
       lf.lfHeight = size;    // 17-8-94 (WINNT 3.5 beta)
#endif
    else
       lf.lfHeight = size;
    lf.lfWidth = 0;

    DeleteObject(hfont);
    hfont = CreateFontIndirect(&lf);
}

void Font :: SetFixedPitch(BOOL blState)
{
    lf.lfPitchAndFamily = 0xf0;
    lf.lfPitchAndFamily |= blState ? FIXED_PITCH : VARIABLE_PITCH;

    DeleteObject(hfont);
    hfont = CreateFontIndirect(&lf);
}

void Font :: SetEscapement(int tenthofdeg)
{
    lf.lfEscapement = tenthofdeg ;
#ifdef WIN32
    lf.lfOrientation = tenthofdeg ;  // probably a bug in NT !!!!
#endif
    DeleteObject(hfont);
    hfont = CreateFontIndirect(&lf);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////// implementation of class Pen /////////////////////////
///////////////////////////////////////////////////////////////////////////////

Pen :: Pen(UINT wType, UINT wWidth, DWORD rgbColor) : Object(IDO_PEN)
{
   hPen = CreatePen(wType,wWidth,rgbColor);
}

Pen :: Pen(LPLOGPEN lplgpn) : Object(IDO_PEN)
{
   hPen = CreatePenIndirect(lplgpn);
}

Pen :: ~Pen(void)
{
   DeleteObject(hPen);
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////// implementation of class Brush /////////////////////////
///////////////////////////////////////////////////////////////////////////////

Brush :: Brush(DWORD rgbColor) : Object(IDO_BRUSH)
{
   hBrush = CreateSolidBrush(rgbColor);
}

Brush :: ~Brush(void)
{
   DeleteObject(hBrush);
}

/////////////////////////////////// EOF ///////////////////////////////////////
