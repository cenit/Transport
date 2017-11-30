/*
 * WINDOWS COLORBUT CONTROL - INCLUDE FILE
 *
 */

typedef struct
{
   HBRUSH   FaceBrush;
   HBRUSH   ShadowBrush;
   HBRUSH   HiliteBrush;
   HRGN     LabelRgn;
   HRGN     ShadowRgn;
   HRGN     HiliteRgn;
   COLORREF NormalTextRGB;
   HANDLE   OtherData;
   WPARAM   Flags;
   WPARAM   DrawTextStyle;
   HFONT    LabelFont;
} WNDDATA;
 
/* ColorButton style dialog definitions */
#define    IDVALUE                0x0100
#define    IDTEXT                 0x0101


HANDLE APIENTRY ColorButtonInfo();
#ifndef WIN32
BOOL APIENTRY ColorButtonStyle( HWND, HANDLE, LPFNSTRTOID, LPFNIDTOSTR );
#else
BOOL APIENTRY ColorButtonStyle( HWND, HANDLE, FARPROC, FARPROC );
#endif
UINT APIENTRY ColorButtonFlags( DWORD, LPSTR, UINT );

/*
   Borland Resource Workshop stuff added by Urs Rohrer, PSI, November 1992
   Extracted from RotKnob or AnaMtr by M. Buchheit
*/

#define BMP_TLBOX_0   100
#define BMP_TLBOX_1   101
#define BMP_TLBOX_2   102

#define CRS_DRGDRP_0  100
#define CRS_DRGDRP_1  101
#define CRS_DRGDRP_2  102

#define ST_TITLE        5000
#define ST_DESCR_0      5001
#define ST_DESCR_1      5002
#define ST_DESCR_2      5003

//// window styles
#define RKS_SCALEMASK  0x000FL
#define RKS_DISCRET    0x8000L
#define RKS_AUTOSELECT 0x4000L

// class default values
#define CLASS_VERSION 256*1+0
#define CLASS_NAME  "ColorButton"
#define CLASS_WNDSTYLE (WS_CHILD|WS_VISIBLE|WS_TABSTOP)

