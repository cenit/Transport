/*
 * COLORBUT -- Dialog Editor / Custom Controls
 *             Copyright 1991, Professional Development Svcs, Dennis Cook
 *             PDSCNTL
 */

#include <string.h>
#include <windows.h>
#include <custcntl.h>
#ifndef WIN32
#include "ntdefs.h"
#endif
#include "colorbut.h"
#include "clrbuttn.h"

/* Function Prototypes  */

void lstrncpy(LPSTR lpDest, LPSTR lpSrc, int n);
BOOL APIENTRY DrawLabel(HDC hDC, LPSTR lpLabelText, int nCount);   
void ShowButtonDown(HANDLE hWnd);
void ShowButtonUp(HANDLE hWnd);

/* internal ColorButton function prototypes */

BOOL APIENTRY ColorButtonDlgFn( HWND, UINT, WPARAM, LPARAM );
LONG APIENTRY ColorButtonWndFn( HWND, UINT, WPARAM, LPARAM );

/* global static variables */

HANDLE           hLibData;
HANDLE           hLibInstance;
WNDDATA          *psave;

#ifndef WIN32
LPFNSTRTOID      lpfnVerId;
LPFNIDTOSTR      lpfnIdStr;
#endif

/* string for property lists */
#define    IDFNLO                  "lpfnIdFnLo"
#define    IDFNHI                  "lpfnIdFnHi"

/* general custom control definitions */

#ifndef WIN32
#define    ID                        GetWindowWord( hWnd, GWW_ID )
#define    PARENT                    GetWindowWord( hWnd, GWW_HWNDPARENT )
#define    INSTANCE                  GetWindowWord( hWnd, GWW_HINSTANCE )
#else
#define    ID                        GetWindowLong( hWnd, GWL_ID )
#define    PARENT                    (HWND)GetWindowLong( hWnd, GWL_HWNDPARENT )
#define    INSTANCE                  (HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE )
#endif

#define    COLORBUTTONCLASS        "ColorButton"

/* ColorButton specific definitions */

#ifndef WIN32
#define  LABEL_EXTRA_SPACE 12
#define  LABEL_LEN_OFFSET 8
#define  LABEL_TEXT_OFFSET 10
#else
#define  LABEL_EXTRA_SPACE 24
#define  LABEL_LEN_OFFSET 16
#define  LABEL_TEXT_OFFSET 20
#endif

/*
#ifndef WIN32
#define    COLORBUTTON_EXTRA     24
#else
// with Visual C++ 4.0 we have a problem: RegisterClass not working with
// cbWndExtra greater than 40.
// But: with Borland C++ 5,  44 is working.
#ifdef __BORLANDC__
#define    COLORBUTTON_EXTRA     44 
#else
#define    COLORBUTTON_EXTRA     40
#endif
#endif
*/

#define    COLORBUTTON_EXTRA    sizeof(HANDLE)

#define    STRIP_BAD_DT_FLAG   0xFB7F

#define    BUTTONISDOWN_FLAG   0x0001
#define    BUTTONINFOCUS_FLAG  0x0002
#define    USENORMALTEXT_FLAG  0x0004
#define    CAPTUREON_FLAG      0x0008
#define    MKCNTLON_FLAG       0x0010

#define    BUTTONISUP_FLAG     0xFFFE
#define    BUTTONNOFOCUS_FLAG  0xFFFD
#define    USEGRAYEDTEXT_FLAG  0xFFFB
#define    CAPTUREOFF_FLAG     0xFFF7
#define    MKCNTLOFF_FLAG      0xFFEF

/*
#ifndef WIN32
#define    FACEBRUSH             GetWindowWord( hWnd, 0 )
#define    SHADOWBRUSH           GetWindowWord( hWnd, 2 )
#define    HILITEBRUSH           GetWindowWord( hWnd, 4 )
#define    LABELRGN              GetWindowWord( hWnd, 6 )
#define    SHADOWRGN             GetWindowWord( hWnd, 8 )
#define    HILITERGN             GetWindowWord( hWnd, 10 )
#define    NORMALTEXTRGB         GetWindowLong( hWnd, 12 )
#define    OTHERDATA             GetWindowWord( hWnd, 16 )
#define    USENORMALTEXT         (GetWindowWord( hWnd, 18 ) & USENORMALTEXT_FLAG)
#define    BUTTONISDOWN          (GetWindowWord( hWnd, 18 ) & BUTTONISDOWN_FLAG)
#define    BUTTONINFOCUS         (GetWindowWord( hWnd, 18 ) & BUTTONINFOCUS_FLAG)
#define    CAPTUREON             (GetWindowWord( hWnd, 18 ) & CAPTUREON_FLAG)
#define    MKCNTLON              (GetWindowWord( hWnd, 18 ) & MKCNTLON_FLAG)
#define    DRAWTEXTSTYLE         GetWindowWord( hWnd, 20 )
#define    LABELFONT             GetWindowWord( hWnd, 22 )

#define    SET_FACEBRUSH(x)       SetWindowWord( hWnd, 0, x )
#define    SET_SHADOWBRUSH(x)     SetWindowWord( hWnd, 2, x )
#define    SET_HILITEBRUSH(x)     SetWindowWord( hWnd, 4, x )
#define    SET_LABELRGN(x)        SetWindowWord( hWnd, 6, x )
#define    SET_SHADOWRGN(x)       SetWindowWord( hWnd, 8, x )
#define    SET_HILITERGN(x)       SetWindowWord( hWnd, 10, x )
#define    SET_NORMALTEXTRGB(x)   SetWindowLong( hWnd, 12, x )
#define    SET_OTHERDATA(x)       SetWindowWord( hWnd, 16, x )

#define    SET_USENORMALTEXT     SetWindowWord( hWnd, 18, (GetWindowWord ( hWnd, 18 ) | USENORMALTEXT_FLAG) )
#define    SET_USEGRAYEDTEXT     SetWindowWord( hWnd, 18, (GetWindowWord ( hWnd, 18 ) & USEGRAYEDTEXT_FLAG) )
#define    SET_BUTTONISDOWN      SetWindowWord( hWnd, 18, (GetWindowWord ( hWnd, 18 ) | BUTTONISDOWN_FLAG) )
#define    SET_BUTTONISUP        SetWindowWord( hWnd, 18, (GetWindowWord ( hWnd, 18 ) & BUTTONISUP_FLAG) )
#define    SET_BUTTONINFOCUS     SetWindowWord( hWnd, 18, (GetWindowWord ( hWnd, 18 ) | BUTTONINFOCUS_FLAG) )
#define    SET_BUTTONNOFOCUS     SetWindowWord( hWnd, 18, (GetWindowWord ( hWnd, 18 ) & BUTTONNOFOCUS_FLAG) )
#define    SET_CAPTUREON         SetWindowWord( hWnd, 18, (GetWindowWord ( hWnd, 18 ) | CAPTUREON_FLAG) )
#define    SET_CAPTUREOFF        SetWindowWord( hWnd, 18, (GetWindowWord ( hWnd, 18 ) & CAPTUREOFF_FLAG) )
#define    SET_MKCNTLON          SetWindowWord( hWnd, 18, (GetWindowWord ( hWnd, 18 ) | MKCNTLON_FLAG) )
#define    SET_MKCNTLOFF         SetWindowWord( hWnd, 18, (GetWindowWord ( hWnd, 18 ) & MKCNTLOFF_FLAG) )

#define    SET_DRAWTEXTSTYLE(x)  SetWindowWord( hWnd, 20, x )
#define    SET_LABELFONT(x)      SetWindowWord( hWnd, 22, x )

#else

#define    FACEBRUSH             (HBRUSH)GetWindowLong( hWnd, 0 )
#define    SHADOWBRUSH           (HBRUSH)GetWindowLong( hWnd, 4 )
#define    HILITEBRUSH           (HBRUSH)GetWindowLong( hWnd, 8 )
#define    LABELRGN              (HRGN)GetWindowLong( hWnd, 12 )
#define    SHADOWRGN             (HRGN)GetWindowLong( hWnd, 16 )
#define    HILITERGN             (HRGN)GetWindowLong( hWnd, 20 )
#define    NORMALTEXTRGB         GetWindowLong( hWnd, 24 )
#define    OTHERDATA             GetWindowLong( hWnd, 28 )
#define    USENORMALTEXT         (GetWindowLong( hWnd, 32 ) & USENORMALTEXT_FLAG)
#define    BUTTONISDOWN          (GetWindowLong( hWnd, 32 ) & BUTTONISDOWN_FLAG)
#define    BUTTONINFOCUS         (GetWindowLong( hWnd, 32 ) & BUTTONINFOCUS_FLAG)
#define    CAPTUREON             (GetWindowLong( hWnd, 32 ) & CAPTUREON_FLAG)
#define    MKCNTLON              (GetWindowLong( hWnd, 32 ) & MKCNTLON_FLAG)
#define    DRAWTEXTSTYLE         GetWindowLong( hWnd, 36 )
#define    LABELFONT             GetWindowLong( hWnd, 40 )

#define    SET_FACEBRUSH(x)       SetWindowLong( hWnd, 0, (LONG)x )
#define    SET_SHADOWBRUSH(x)     SetWindowLong( hWnd, 4, (LONG)x )
#define    SET_HILITEBRUSH(x)     SetWindowLong( hWnd, 8, (LONG)x )
#define    SET_LABELRGN(x)        SetWindowLong( hWnd, 12, (LONG)x )
#define    SET_SHADOWRGN(x)       SetWindowLong( hWnd, 16, (LONG)x )
#define    SET_HILITERGN(x)       SetWindowLong( hWnd, 20, (LONG)x )
#define    SET_NORMALTEXTRGB(x)   SetWindowLong( hWnd, 24, (LONG)x )
#define    SET_OTHERDATA(x)       SetWindowLong( hWnd, 28, (LONG)x )

#define    SET_USENORMALTEXT     SetWindowLong( hWnd, 32, (GetWindowLong ( hWnd, 32 ) | USENORMALTEXT_FLAG) )
#define    SET_USEGRAYEDTEXT     SetWindowLong( hWnd, 32, (GetWindowLong ( hWnd, 32 ) & USEGRAYEDTEXT_FLAG) )
#define    SET_BUTTONISDOWN      SetWindowLong( hWnd, 32, (GetWindowLong ( hWnd, 32 ) | BUTTONISDOWN_FLAG) )
#define    SET_BUTTONISUP        SetWindowLong( hWnd, 32, (GetWindowLong ( hWnd, 32 ) & BUTTONISUP_FLAG) )
#define    SET_BUTTONINFOCUS     SetWindowLong( hWnd, 32, (GetWindowLong ( hWnd, 32 ) | BUTTONINFOCUS_FLAG) )
#define    SET_BUTTONNOFOCUS     SetWindowLong( hWnd, 32, (GetWindowLong ( hWnd, 32 ) & BUTTONNOFOCUS_FLAG) )
#define    SET_CAPTUREON         SetWindowLong( hWnd, 32, (GetWindowLong ( hWnd, 32 ) | CAPTUREON_FLAG) )
#define    SET_CAPTUREOFF        SetWindowLong( hWnd, 32, (GetWindowLong ( hWnd, 32 ) & CAPTUREOFF_FLAG) )
#define    SET_MKCNTLON          SetWindowLong( hWnd, 32, (GetWindowLong ( hWnd, 32 ) | MKCNTLON_FLAG) )
#define    SET_MKCNTLOFF         SetWindowLong( hWnd, 32, (GetWindowLong ( hWnd, 32 ) & MKCNTLOFF_FLAG) )

#define    SET_DRAWTEXTSTYLE(x)  SetWindowLong( hWnd, 36, (LONG)x )
#define    SET_LABELFONT(x)      SetWindowLong( hWnd, 40, (LONG)x )
#endif
*/
#define    FACEBRUSH        p->FaceBrush
#define    SHADOWBRUSH      p->ShadowBrush
#define    HILITEBRUSH      p->HiliteBrush
#define    LABELRGN         p->LabelRgn
#define    SHADOWRGN        p->ShadowRgn
#define    HILITERGN        p->HiliteRgn
#define    NORMALTEXTRGB    p->NormalTextRGB
#define    OTHERDATA        p->OtherData
#define    USENORMALTEXT    (p->Flags & USENORMALTEXT_FLAG)
#define    BUTTONISDOWN     (p->Flags & BUTTONISDOWN_FLAG)
#define    BUTTONINFOCUS    (p->Flags & BUTTONINFOCUS_FLAG)
#define    CAPTUREON        (p->Flags & CAPTUREON_FLAG)
#define    MKCNTLON         (p->Flags & MKCNTLON_FLAG)
#define    DRAWTEXTSTYLE    p->DrawTextStyle
#define    LABELFONT        p->LabelFont

#define    SET_FACEBRUSH(x)       p->FaceBrush = x
#define    SET_SHADOWBRUSH(x)     p->ShadowBrush = x
#define    SET_HILITEBRUSH(x)     p->HiliteBrush = x
#define    SET_LABELRGN(x)        p->LabelRgn = x
#define    SET_SHADOWRGN(x)       p->ShadowRgn = x
#define    SET_HILITERGN(x)       p->HiliteRgn = x
#define    SET_NORMALTEXTRGB(x)   p->NormalTextRGB = x
#define    SET_OTHERDATA(x)       p->OtherData = x

#define    SET_USENORMALTEXT     p->Flags |= USENORMALTEXT_FLAG
#define    SET_USEGRAYEDTEXT     p->Flags &= USEGRAYEDTEXT_FLAG
#define    SET_BUTTONISDOWN      p->Flags |= BUTTONISDOWN_FLAG
#define    SET_BUTTONISUP        p->Flags &= BUTTONISUP_FLAG
#define    SET_BUTTONINFOCUS     p->Flags |= BUTTONINFOCUS_FLAG
#define    SET_BUTTONNOFOCUS     p->Flags &= BUTTONNOFOCUS_FLAG
#define    SET_CAPTUREON         p->Flags |= CAPTUREON_FLAG
#define    SET_CAPTUREOFF        p->Flags &= CAPTUREOFF_FLAG
#define    SET_MKCNTLON          p->Flags |= MKCNTLON_FLAG
#define    SET_MKCNTLOFF         p->Flags &= MKCNTLOFF_FLAG

#define    SET_DRAWTEXTSTYLE(x)  p->DrawTextStyle = x
#define    SET_LABELFONT(x)      p->LabelFont = x


/* ColorButton specific global variables */

HANDLE   hDrawTextWnd;
HFONT    hDefLabelFont;         
HPEN     hOutlinePen;
HBRUSH   hDefFaceBrush, hDefHiliteBrush, hDefShadowBrush;
COLORREF crDefNormalText;
RECT     rDrawLabel;
FARPROC  lpfnDrawLabel;

/*
   Borland Resource Workshop stuff added by Urs Rohrer, PSI, November 1992
   Extracted from RotKnob or AnaMtr by M. Buchheit
*/

#ifndef WIN32
#if !defined(__CUSTCNTL_H)

/*****************************************************************************

 -------- Read special declarations for Borland's Resource Workshop ---------

*****************************************************************************/

typedef struct
 {UINT    wType;
  UINT    wWidth;
  UINT    wHeight;
  DWORD   dwStyle;
  char    szDescr[CTLDESCR];
  HBITMAP hToolBit;     // bitmap for dialog editor's toolbox
  HCURSOR hDropCurs;    // drag-drop-cursor for dialog editor's toolsbox
 }
RWCTLTYPE;

typedef struct
 {UINT      wVersion;
  UINT      wCtlTypes;
  char      szClass[CTLCLASS];
  char      szTitle[CTLTITLE];
  char      szReserved[10];
  RWCTLTYPE Type[CTLTYPES];
 }
RWCTLINFO;

typedef RWCTLINFO *  PRWCTLINFO;
typedef RWCTLINFO FAR *  LPRWCTLINFO;

#define CTLDATALENGTH 255

typedef struct
 {UINT  wX;
  UINT  wY;
  UINT  wCx;
  UINT  wCy;
  UINT  wId;
  DWORD dwStyle;
  char  szClass[CTLCLASS];
  char  szTitle[CTLTITLE];
  BYTE  CtlDataSize;            // control data size
  BYTE  CtlData[CTLDATALENGTH]; // control data
 } RWCTLSTYLE;

typedef HANDLE (APIENTRY *LPFNINFO)(VOID);
typedef BOOL (APIENTRY *LPFNSTYLE)(HWND,HANDLE,LPFNSTRTOID,LPFNIDTOSTR);
typedef UINT (APIENTRY *LPFNFLAGS)(DWORD,LPSTR,UINT);

typedef struct
 {LPFNINFO  fnRWInfo;       // RW Info function
  LPFNSTYLE fnRWStyle;      // RW Style function
  LPFNFLAGS fnFlags;        // Flags function
  char  szClass[CTLCLASS];
 }
RWCTLCLASS;

typedef struct
 {short       nClasses;
  RWCTLCLASS Classes[1];
 }
CTLCLASSLIST;

#endif
#endif

HANDLE hib;

/**/
/*
 * ColorButtonWndFn( hWnd, wMsg, wParam, lParam ) : LONG
 *
 *        hWnd                    handle to ColorButton window
 *        wMsg                    message number
 *        wParam                  single word parameter
 *        lParam                  double word parameter
 *
 * This function is responsible for processing all the messages
 * which relate to the ColorButton control window.  Note how the
 * code is written to avoid potential problems when re-entrancy
 * ocurrs - this involves the use of extra bytes associated with
 * the window data structure.
 *
 * The LONG value returned by this function is either a value
 * returned by the internal handling of the message or by the
 * default window procedure.
 *
 */

LONG APIENTRY ColorButtonWndFn(HWND   hWnd,
                               UINT   wMsg,
                               WPARAM wParam,
                               LPARAM lParam)
{
   WNDDATA *p;
   HANDLE  hData;
   LONG    lResult;
    
   lResult = TRUE;

   if (!hWnd) return 0L;

#ifdef WIN32
   hData = (HANDLE)GetWindowLong(hWnd,0);
#else
   hData = (HANDLE)GetWindowWord(hWnd,0);
#endif
   if (hData)
   {  p = (WNDDATA *)LocalLock(hData);
      psave = p;
   }

   switch ( wMsg )
   {
      case WM_NCCREATE:
         {
            // Store handle to structure, default brushes and colors

	    hData = LocalAlloc(LMEM_MOVEABLE|LMEM_ZEROINIT,sizeof(WNDDATA));
            if (hData)
            {
#ifdef WIN32
	       SetWindowLong(hWnd,0,(LONG)hData);
#else
	       SetWindowWord(hWnd,0,(WORD)hData);
#endif
            }
         }
         break;

      case WM_CREATE:
         // At this point we need to fill in the default extra bytes and 
         // get the window name from the createstruct
         // In addition to window extra bytes each button control will be
         // allocated local heap space to hold the label text and label 
         // rectangle information.  A handle to this space is put into the
         // window extra bytes.  Format for structure is as follows:
         //   Label rect left int
         //   Label rect top int
         //   Label rect right int
         //   Label rect bottom int
         //   Label text len word
         //   Label text [text len word] bytes
         {
            HANDLE         hOtherData;
            char far       *lpOtherData;
            UINT           wTextLen, wAllocBytes;
            LPCREATESTRUCT lpcsData;

            lpcsData = (LPCREATESTRUCT) lParam;
            if ((lpcsData)->lpszName == NULL)
               wTextLen = 0;
            else
               wTextLen = lstrlen((lpcsData)->lpszName);

            // allocate space for information structure 

            wAllocBytes = wTextLen + LABEL_EXTRA_SPACE;
            hOtherData = LocalAlloc( LHND, wAllocBytes );
            if (hOtherData)
            {  if (wTextLen > 0)
               {
                  // attempt to lock it down 
                  lpOtherData = (char far *)LocalLock((HLOCAL)hOtherData);
                  if (lpOtherData)
                  {
                     // Store button label text info in allocated structure
                     lstrcpy((lpOtherData+LABEL_TEXT_OFFSET),(lpcsData)->lpszName);
                     *( (UINT *) (lpOtherData + LABEL_LEN_OFFSET) ) = wTextLen;
                     LocalUnlock(hOtherData);
                  }
               }
            }

            SET_OTHERDATA(hOtherData);
            SET_FACEBRUSH(hDefFaceBrush);
            SET_SHADOWBRUSH(hDefShadowBrush);
            SET_HILITEBRUSH(hDefHiliteBrush);
            SET_NORMALTEXTRGB(crDefNormalText);
            SET_LABELFONT(hDefLabelFont);
            SET_USENORMALTEXT;
            SET_BUTTONISUP;
            SET_BUTTONNOFOCUS;
            SET_MKCNTLOFF;
            SET_CAPTUREOFF;
            SET_DRAWTEXTSTYLE((DT_CENTER | DT_WORDBREAK));

            SET_HILITERGN(0);
            SET_SHADOWRGN(0);
            SET_LABELRGN(0);
         }
         break;

      case WM_GETDLGCODE:
         {
            if (GetWindowLong(hWnd, GWL_STYLE) & BS_DEFPUSHBUTTON)
               lResult = (LONG) DLGC_DEFPUSHBUTTON;
            else
               lResult = (LONG) DLGC_UNDEFPUSHBUTTON;
         }
         break;

      case WM_SIZE:
         // At this point we need to fill in the default extra bytes and 
         {
            POINT pts[6];
            int iLblHi, iLblWi, iTxtMaxWi, iTxtMaxHi;
            RECT rRect;
            LPRECT lprLabel;
            char far *lpLabelText;
            HDC hDC;
            HRGN hHiliteRgn, hShadowRgn, hLabelRgn;
            HFONT hOldFont;

            hHiliteRgn = (HRGN)HILITERGN;
            hShadowRgn = (HRGN)SHADOWRGN;
            hLabelRgn  = (HRGN)LABELRGN;

            if (hHiliteRgn)
               DeleteObject(hHiliteRgn);
            if (hHiliteRgn)
               DeleteObject(hShadowRgn);
            if (hHiliteRgn)
               DeleteObject(hLabelRgn);

            lprLabel = (LPRECT) LocalLock((HLOCAL)OTHERDATA);

            hDC = GetDC(hWnd);

            hOldFont = (HFONT) SelectObject(hDC, (HGDIOBJ)LABELFONT);
            GetClientRect(hWnd, &rRect);

            // Create the hilite region
            pts[0].x = pts[1].x = pts[1].y = pts[2].y = 1;
            pts[4].x = pts[5].x = pts[3].y = pts[4].y = 3;
            pts[0].y = rRect.bottom - 2;
            pts[2].x = rRect.right - 2;
            pts[3].x = rRect.right - 4;
            pts[5].y = rRect.bottom - 4;
            hHiliteRgn = CreatePolygonRgn((LPPOINT)&pts, 6, WINDING);
            SET_HILITERGN(hHiliteRgn);

            // Create the shadow region
            pts[4].x = pts[5].x = rRect.right - 3;
            pts[3].x = pts[5].y = 2;
            pts[2].x = pts[0].y = 0;
            pts[3].y = pts[4].y = rRect.bottom - 3;
            pts[1].x = pts[0].x = rRect.right - 1;
            pts[1].y = pts[2].y = rRect.bottom - 1;
            hShadowRgn = CreatePolygonRgn((LPPOINT)&pts, 6, WINDING);
            SET_SHADOWRGN(hShadowRgn);

        // max size for label area is:
            (lprLabel)->left   = 6;
            (lprLabel)->top    = 6;
            (lprLabel)->bottom = rRect.bottom - 6;
            (lprLabel)->right  = rRect.right - 6;
        // Now test for minimum height of rectangle
            lpLabelText = (char far *) (lprLabel) + LABEL_TEXT_OFFSET;
            if (!(lpLabelText[0] == 0)) 
               DrawText(hDC, lpLabelText, -1, lprLabel, ((DRAWTEXTSTYLE) | DT_CALCRECT));
        // Make sure label rect does not exceed max size
            iTxtMaxHi = rRect.bottom - 6;          /* - 12; */
            iTxtMaxWi = rRect.right - 12;
            iLblHi    = (lprLabel)->bottom - (lprLabel)->top;
            if (iLblHi > iTxtMaxHi) 
               iLblHi = iTxtMaxHi;
            iLblWi    = (lprLabel)->right - (lprLabel)->left;
            if (iLblWi > iTxtMaxWi) 
               iLblWi = iTxtMaxWi;
        // Reset label rect to center vertically
            (lprLabel)->left   = ((iTxtMaxWi - iLblWi) / 2) + 6;
            (lprLabel)->top    = ((iTxtMaxHi - iLblHi) / 2) + 3;     /* + 6; */
            (lprLabel)->right  = (lprLabel)->left + iLblWi;
            (lprLabel)->bottom = (lprLabel)->top + iLblHi;
            hLabelRgn = CreateRectRgn((lprLabel)->left,
                                      (lprLabel)->top, 
                                      (lprLabel)->right, 
                                      (lprLabel)->bottom);
            SET_LABELRGN(hLabelRgn);
            SelectObject(hDC, (HGDIOBJ)hOldFont);
            LocalUnlock((HLOCAL)OTHERDATA);
            ReleaseDC(hWnd, hDC);
         }
         break;

      case WM_PAINT:
         {
            PAINTSTRUCT ps;
            RECT rRect;
            LPRECT lprLabel;
            char far *lpLabelText;
            UINT wTextLen;
            HDC  hDC;
            HPEN hOldPen;
            HBRUSH hOldBrush;
            HFONT hOldFont;

            hDC = BeginPaint(hWnd, &ps);

            // Before painting anything, notify parent to provide opportunity
            // to set new brush and text colors (like pushbutton, nothing is
            // is done with the return value.                                     
#ifndef WIN32
            SendMessage(PARENT, WM_CTLCOLOR, hDC, MAKELONG(hWnd, CTLCOLOR_BTN));
#else
            SendMessage(PARENT, WM_CTLCOLORBTN, (WPARAM)hDC, (LPARAM)hWnd);
#endif

            lprLabel = (LPRECT) LocalLock((HLOCAL)OTHERDATA);
            GetClientRect(hWnd, &rRect);

            // Set text color and font
            SetTextColor(hDC, NORMALTEXTRGB);
            SetBkMode(hDC, TRANSPARENT);
            hOldFont = SelectObject(hDC, (HGDIOBJ)LABELFONT);

            // Create Button Outline and Fill with Face color
            hOldPen = SelectObject(hDC, (HGDIOBJ)hOutlinePen);
            hOldBrush = SelectObject(hDC, (HGDIOBJ)FACEBRUSH);
            RoundRect(hDC, rRect.left, rRect.top, rRect.right, rRect.bottom, 2, 2);
            // Create button hilite & shadow
            if (BUTTONISDOWN) {
               FillRgn(hDC, HILITERGN, SHADOWBRUSH);
               FillRgn(hDC, SHADOWRGN, FACEBRUSH);
            }
            else {
               FillRgn(hDC, HILITERGN, HILITEBRUSH);
               FillRgn(hDC, SHADOWRGN, SHADOWBRUSH);
            }
            // Display Label text
            lpLabelText = (char far *) (lprLabel) + LABEL_TEXT_OFFSET;
            wTextLen =  (UINT) *((char far *) (lprLabel) + LABEL_LEN_OFFSET);
            // When paint button in down position shift the label rect
            if (BUTTONISDOWN)
               OffsetRect(lprLabel, 2, 2);
            if (!(lpLabelText[0] == 0)) {
               if (USENORMALTEXT) {
                  DrawText(hDC, lpLabelText, -1, lprLabel, DRAWTEXTSTYLE);
               }
               else {
                  HBRUSH hGrayedTextBrush;
                  POINT pt;

                  hGrayedTextBrush = CreateSolidBrush(NORMALTEXTRGB);
                  pt.x = pt.y = 0;
                  ClientToScreen(hWnd, &pt);
                  UnrealizeObject(hGrayedTextBrush);
#ifndef WIN32
                  SetBrushOrg(hDC, pt.x, pt.y);
#else
                  SetBrushOrgEx(hDC, pt.x, pt.y, NULL);
#endif

                  hDrawTextWnd = hWnd;
                  rDrawLabel.left = rDrawLabel.top = 0;
                  rDrawLabel.right = (lprLabel)->right - (lprLabel)->left;
                  rDrawLabel.bottom = (lprLabel)->bottom - (lprLabel)->top;
                  GrayString(hDC, hGrayedTextBrush, (GRAYSTRINGPROC)lpfnDrawLabel, 
                             (DWORD) lpLabelText, wTextLen, 
                             (lprLabel)->left, (lprLabel)->top, 
                             (lprLabel)->right - (lprLabel)->left, 
                             (lprLabel)->bottom - (lprLabel)->top);
                  DeleteObject(hGrayedTextBrush);
               }
            }

            if (BUTTONINFOCUS) {
               rRect.left   =  (lprLabel)->left - 2;
               rRect.top    =  (lprLabel)->top - 0; 
               rRect.right  =  (lprLabel)->right + 2;
               rRect.bottom =  (lprLabel)->bottom + 0;
               DrawFocusRect(hDC, &rRect);
            }

            // After paint button in down position reset label rect value
            if (BUTTONISDOWN)
               OffsetRect(lprLabel, -2, -2);

            SelectObject(hDC, (HGDIOBJ)hOldPen);
            SelectObject(hDC, (HGDIOBJ)hOldBrush);
            SelectObject(hDC, (HGDIOBJ)hOldFont);

            LocalUnlock((HLOCAL)OTHERDATA);
            EndPaint(hWnd, &ps);
         }
         break;

      case WM_SETFOCUS:
         {
            SET_BUTTONINFOCUS;
            InvalidateRect(hWnd, 0, FALSE);
         }
         break;
      case WM_KILLFOCUS:
         {
            SET_BUTTONNOFOCUS;
            InvalidateRect(hWnd, 0, FALSE);
            UpdateWindow(hWnd);
         }
         break;

      case WM_ENABLE:
         {
            if (wParam)   {
               SET_USENORMALTEXT;
            }
            else {
               SET_USEGRAYEDTEXT;
            }
            InvalidateRect(hWnd, 0, FALSE);
            UpdateWindow(hWnd);
         }
         break;

      case WM_KEYDOWN:
         {
            if (wParam == VK_SPACE) {
               // if mouse does not have control
               if (!(MKCNTLON)) {
                  //if the button is shown in the up position
                  if (!(BUTTONISDOWN)) {
                     ShowButtonDown(hWnd);
                  }
               }
            }
         }
         break;

      case WM_LBUTTONDBLCLK:
      case WM_LBUTTONDOWN:
         {
            if (!(CAPTUREON)) {
               SET_CAPTUREON;
               SetCapture(hWnd);
               SET_MKCNTLON;

               if (!(BUTTONISDOWN)) 
                  ShowButtonDown(hWnd);
            }
         }
         break;

      case WM_KEYUP:
         {
            if (wParam == VK_SPACE) {

               // if mouse does not have control
               if (!(MKCNTLON)) {

                  //if the button is shown in the down position
                  if ((BUTTONISDOWN)) {

                     ShowButtonUp(hWnd);

                     // send the parent window a button clicked command message
#ifndef WIN32
                     SendMessage(PARENT,WM_COMMAND,ID,
                                 MAKELONG(hWnd, BN_CLICKED));
#else
                     SendMessage(PARENT,WM_COMMAND,MAKELONG(ID, BN_CLICKED),
                                 (LPARAM)hWnd);
#endif
                  }
               }
            }
         }
         break;

      case WM_LBUTTONUP:
         {
            if (CAPTUREON)
            {  RECT rRect;
               POINT mouWorld;

               GetClientRect(hWnd, &rRect);
               mouWorld.x = LOWORD(lParam);
               mouWorld.y = HIWORD(lParam);
               if (PtInRect(&rRect, mouWorld))
               {
                  // send the parent window a button clicked command message
#ifndef WIN32
                  SendMessage(PARENT,WM_COMMAND,ID,MAKELONG(hWnd, BN_CLICKED));
#else
                  SendMessage(PARENT,WM_COMMAND,MAKELONG(ID, BN_CLICKED),
                              (LPARAM)hWnd);
#endif
                  ShowButtonUp(hWnd);
               }
               SET_MKCNTLOFF;
               SET_CAPTUREOFF;
               ReleaseCapture();
            }
         }
         break;

      case WM_MOUSEACTIVATE:
         {
            SetFocus(hWnd);
         }
         break;

      case WM_MOUSEMOVE:
         {
            RECT rRect;
            POINT mouWorld;

            GetClientRect(hWnd, &rRect);

            // is the cursor in the button window

            mouWorld.x = LOWORD(lParam);
            mouWorld.y = HIWORD(lParam);
            if (PtInRect(&rRect, mouWorld))
            {
               // is the mouse button down
               if (wParam & MK_LBUTTON) {

                  // is the button shown in the up position?
                  if (!(BUTTONISDOWN)) {

                     SET_BUTTONISDOWN;
                     InvalidateRect(hWnd, 0, FALSE);
                     UpdateWindow(hWnd);
                  }
               }
            }
            else {    //cursor outside of the button window

               // is the mouse button down
               if (wParam & MK_LBUTTON) {

                  // is the button shown in the down position 
                  if (BUTTONISDOWN) {

                     SET_BUTTONISUP;
                     InvalidateRect(hWnd, 0, FALSE);
                     UpdateWindow(hWnd);
                  }
               }
            }
         }
         break;

      case WM_SETTEXT:
         {
            UINT wNewTextLen, wCurrSize;
            char far *lpOtherData;
            HANDLE hOtherData;
            RECT rRect;
            UINT wHi, wWi;

            if (lParam == 0)
               wNewTextLen = 0;
            else
               wNewTextLen = lstrlen((LPSTR) lParam);

            wCurrSize =  LocalSize((HLOCAL)OTHERDATA);

            // If the new label string is larger than space currently allocated,
            // then realloc more space
            if ((wNewTextLen + LABEL_EXTRA_SPACE) > wCurrSize) {
               hOtherData = LocalReAlloc((HLOCAL)OTHERDATA, 
                                         (wNewTextLen + LABEL_EXTRA_SPACE),
                                         LHND);
               if (hOtherData) 
                  SET_OTHERDATA(hOtherData);
               else {
                  lResult = FALSE;
                  break;
               }
            }

            lpOtherData = (char far *)LocalLock((HLOCAL)OTHERDATA);
            if (lpOtherData)
            {
               // Store button label text info in allocated structure
               if (wNewTextLen)
                  lstrcpy((lpOtherData + LABEL_TEXT_OFFSET), (LPSTR) lParam);
               else
                  *(lpOtherData + LABEL_TEXT_OFFSET) = 0;

               *( (UINT *) (lpOtherData + LABEL_LEN_OFFSET) ) = wNewTextLen;
               LocalUnlock((HLOCAL)OTHERDATA);

               GetClientRect(hWnd, &rRect);
               wHi = rRect.bottom - rRect.top;
               wWi = rRect.right - rRect.left;
               SendMessage(hWnd,WM_SIZE,SIZENORMAL,MAKELONG(wWi,wHi));

            }
         }
         break;

      case WM_GETTEXTLENGTH:
         {
            char far *lpOtherData;
            UINT wTextLen;

            lpOtherData = (char far *) LocalLock((HLOCAL)OTHERDATA);
            wTextLen = (UINT) *(lpOtherData + LABEL_LEN_OFFSET);
            lResult = (LONG) wTextLen;
            LocalUnlock((HLOCAL)OTHERDATA);
         }
         break;

      case WM_GETTEXT:
         {
            char far *lpOtherData;
            UINT wTextLen;

            lpOtherData = (char far *) LocalLock((HLOCAL)OTHERDATA);
            wTextLen = (int) *(lpOtherData + LABEL_LEN_OFFSET);
            if (wParam < wTextLen)
               wTextLen = wParam;
            if (wTextLen > 0)
               lstrncpy( (LPSTR) lParam, (lpOtherData + LABEL_TEXT_OFFSET), (int) wTextLen);
            lResult = (LONG) wTextLen;
            LocalUnlock((HLOCAL)OTHERDATA);
         }
         break;

      case WM_CLOSE:
      case WM_DESTROY:
         {
            DeleteObject(HILITERGN);
            DeleteObject(SHADOWRGN);
            DeleteObject(LABELRGN);
            LocalFree((HLOCAL)OTHERDATA);

	    LocalUnlock(hData);
	    LocalFree(hData);

            lResult = DefWindowProc( hWnd, wMsg, wParam, lParam );
         }
         break;

      case CRBM_SETFACEBRUSH:
         {
            if (wParam)
               SET_FACEBRUSH((HBRUSH)wParam);
            else
               SET_FACEBRUSH(hDefFaceBrush);
         }
         break;

      case CRBM_SETSHADOWBRUSH:
         {
            if (wParam)
               SET_SHADOWBRUSH((HBRUSH)wParam);
            else
               SET_SHADOWBRUSH(hDefShadowBrush);
         }
         break;

      case CRBM_SETHILITEBRUSH:
         {
            if (wParam)
               SET_HILITEBRUSH((HBRUSH)wParam);
            else
               SET_HILITEBRUSH(hDefHiliteBrush);
         }
         break;

      case CRBM_SETTEXTCOLOR:
         {
            if (wParam)
               SET_NORMALTEXTRGB((COLORREF)lParam);
            else
               SET_NORMALTEXTRGB(crDefNormalText);
         }
         break;

      case CRBM_SETDRAWTEXTSTYLE:
         {
            SET_DRAWTEXTSTYLE(wParam & STRIP_BAD_DT_FLAG);
         }
         break;

      case WM_SETFONT:
      case CRBM_SETLABELFONT:
         {
            RECT rRect;
            UINT wHi, wWi;

            if (wParam)
               SET_LABELFONT((HFONT)wParam);
            else
               SET_LABELFONT(hDefLabelFont);

            GetClientRect(hWnd, &rRect);
            wHi = rRect.bottom - rRect.top;
            wWi = rRect.right - rRect.left;
            SendMessage(hWnd,WM_SIZE,SIZENORMAL,MAKELONG(wWi,wHi));
         }
         break;

      case CRBM_GETFACEBRUSH:
         {
            HBRUSH hTmpBrush;

            hTmpBrush = (HBRUSH) FACEBRUSH;
            if (hTmpBrush == hDefFaceBrush)
               hTmpBrush = 0;
            lResult = MAKELONG (hTmpBrush, 0);
         }
         break;

      case CRBM_GETSHADOWBRUSH:
         {
            HBRUSH hTmpBrush;

            hTmpBrush = (HBRUSH) SHADOWBRUSH;
            if (hTmpBrush == hDefShadowBrush)
               hTmpBrush = 0;
            lResult = MAKELONG (hTmpBrush, 0);
         }
         break;

      case CRBM_GETHILITEBRUSH:
         {
            HBRUSH hTmpBrush;

            hTmpBrush = (HBRUSH) HILITEBRUSH;
            if (hTmpBrush == hDefHiliteBrush)
               hTmpBrush = 0;
            lResult = MAKELONG (hTmpBrush, 0);
         }
         break;

      case CRBM_GETTEXTCOLOR:
         {
            lResult = NORMALTEXTRGB;
         }
         break;

      case CRBM_GETDRAWTEXTSTYLE:
         {
            lResult = MAKELONG (DRAWTEXTSTYLE, 0);
         }
	 break;

      case WM_GETFONT:
      case CRBM_GETLABELFONT:
         {
            HFONT hTmpFont;

            hTmpFont = (HFONT) LABELFONT;
            if (hTmpFont == hDefLabelFont)
               hTmpFont = 0;
            lResult = MAKELONG (hTmpFont, 0);
         }
         break;

      default : /* default window message processing */
         if (hWnd)
            lResult = DefWindowProc( hWnd, wMsg, wParam, lParam );
         break;
    }
    
    /* return final result */
    return( lResult );

}

/**/

void ShowButtonDown(HANDLE hWnd)
{
   WNDDATA *p;

   p = psave;
   SET_BUTTONISDOWN;
   InvalidateRect(hWnd, 0, FALSE);
   UpdateWindow(hWnd);
}

void ShowButtonUp(HANDLE hWnd)
{
   WNDDATA *p;

   p = psave;
   SET_BUTTONISUP;
   InvalidateRect(hWnd, 0, FALSE);
   UpdateWindow(hWnd);
}

BOOL APIENTRY DrawLabel(HDC hDC, LPSTR lpLabelText, int nCount)
{
//   HANDLE hWnd;
   WNDDATA *p;

   p = psave;

//   hWnd = hDrawTextWnd;
   DrawText(hDC, lpLabelText, nCount, &rDrawLabel, DRAWTEXTSTYLE);
   return(TRUE);
}

void lstrncpy(LPSTR lpDest, LPSTR lpSrc, int n)
{
    while (n--)
        if(!(*lpDest++ = *lpSrc++))
            return;
}

/*
   For usage with WIN++ (Version 2.22). New problem (not present with V2.1):
   WM_DESTROY is not sent anymore to each button,
   so GDI resources remain blocked. This may be cured by executing
   FreeObject for each colorbutton before deleting the parent dialogbox.
*/

#ifdef __BORLANDC__
#pragma argsused
#endif

VOID APIENTRY FreeObject(HWND hWnd)
{
   WNDDATA *p;

   p = psave;
//   MessageBox(PARENT,"freed","Object",MB_ICONINFORMATION|MB_OK);
   DeleteObject(HILITERGN);
   DeleteObject(SHADOWRGN);
   DeleteObject(LABELRGN);
   LocalFree((HLOCAL)OTHERDATA);
}

#ifndef WIN32
 
/**/
/*
 * ColorButtonDlgFn( hDlg, wMessage, wParam, lParam ) : BOOL;
 *
 *    hDlg           dialog box handle
 *    wMessage       current window message
 *    wParam         word parameter
 *    lParam         long parameter
 *
 * This function is responsible for processing all the messages that
 * relate to the style dialog box.  This function transfers data 
 * between itself and the ColorButtonStyle using a global data handle.
 *
 * If the user presses the OK button, this data handle is used to pass
 * back a new style data block.  It is left to the calling routine to
 * delete this new block.
 *
 */

BOOL APIENTRY ColorButtonDlgFn(HWND        hDlg,
                               UINT        wMessage,
                               WPARAM      wParam,
                               LPARAM      lParam )
{
   BOOL            bResult;

   /* initialization */
   bResult = TRUE;

   /* process message */
   switch( wMessage )  {

      case WM_INITDIALOG :
         { 
            HANDLE        hCtlStyle;
            LPCTLSTYLE    lpCtlStyle;
            char          szId[  20 ];    /* temp. string to hold id */
   
            /* disable Ok button & save dialog data handle */
            hCtlStyle = hLibData;
            EnableWindow( GetDlgItem(hDlg,IDOK), FALSE );
   
            /* retrieve & display style parameters */
            if ( hCtlStyle ) {
               
               /* add handle to property list */
               SetProp( hDlg, MAKEINTRESOURCE(1), hCtlStyle );
               
               /* update dialog box fields */
               lpCtlStyle = (LPCTLSTYLE)GlobalLock( hCtlStyle );
               SetDlgItemText( hDlg, IDTEXT, lpCtlStyle->szTitle );
   
               /* Set the id value string correctly.
                * Save the pointer to the verify id function from dialog editor
                */
               if ( lpfnIdStr )
               {
                  (*lpfnIdStr)(lpCtlStyle->wId, (LPSTR)szId, sizeof( szId ) );
                  SetDlgItemText( hDlg, IDVALUE, szId );
               }
               else {
                  EnableWindow( GetDlgItem( hDlg, IDVALUE ), FALSE );
               }

               CheckDlgButton(hDlg, 258, (UINT) (LOWORD(lpCtlStyle->dwStyle) & BS_DEFPUSHBUTTON));

               lstrcpy( lpCtlStyle->szClass, COLORBUTTONCLASS );
               SetProp( hDlg, IDFNHI, HIWORD( (DWORD)lpfnVerId ) );
               SetProp( hDlg, IDFNLO, LOWORD( (DWORD)lpfnVerId ) );
               GlobalUnlock( hCtlStyle );
            }
            else
               EndDialog( hDlg, FALSE );
         }
         break;

      case WM_COMMAND :

       /* process sub-message */
         switch( wParam ) {
            case IDCANCEL : /* cancel dialog box */
               RemoveProp( hDlg, MAKEINTRESOURCE(1) );
               RemoveProp( hDlg, IDFNLO );
               RemoveProp( hDlg, IDFNHI );
               EndDialog( hDlg, FALSE );
               break;

            case IDOK :    /* save dialog changes */
               {
                  HANDLE           hCtlStyle;
                  LPCTLSTYLE       lpCtlStyle;
                  LPFNSTRTOID      lpfnId;          /* pointer to the verify id function from dialog editor */
                  char             szId[ 20 ];        /* temp. string to hold the id */
        
                  hCtlStyle = GetProp( hDlg, MAKEINTRESOURCE(1) );
        
                  /* update structure contents */
                  lpCtlStyle = (LPCTLSTYLE)GlobalLock( hCtlStyle );
        
                  szId[ 0 ] = 0;
                  GetDlgItemText( hDlg, IDVALUE, szId, sizeof(szId) );
                  lpfnId = (LPFNSTRTOID)MAKELONG( GetProp( hDlg, IDFNLO ), GetProp( hDlg, IDFNHI ) );
                  if ( lpfnId ) {
                     DWORD        dwResult; /* result ofthe verifyId function */
        
                     dwResult = (*lpfnId)( (LPSTR)szId );
                     if ( !(BOOL)dwResult ) {
                        /* Wrong id */
                        GlobalUnlock( hCtlStyle );
                        break;        
                      }
                      lpCtlStyle->wId = HIWORD( dwResult );
                      if (IsDlgButtonChecked(hDlg, 258))
                         lpCtlStyle->dwStyle |= BS_DEFPUSHBUTTON;
                      else
                         lpCtlStyle->dwStyle |= BS_PUSHBUTTON;

                  }
                  GetDlgItemText( hDlg, IDTEXT, lpCtlStyle->szTitle, sizeof(lpCtlStyle->szTitle) );
                  GlobalUnlock( hCtlStyle );
                  RemoveProp( hDlg, MAKEINTRESOURCE(1) );
                  RemoveProp( hDlg, IDFNLO );
                  RemoveProp( hDlg, IDFNHI );
                  
                  /* end dialog box */
                  hLibData = hCtlStyle;
                  EndDialog( hDlg, TRUE );
               }
               break;

            case IDTEXT : /* control text */
            case IDVALUE : /* control id */
     
               /* enable or disable Ok button */
               if ( HIWORD(lParam) == EN_CHANGE )
                  EnableWindow(GetDlgItem(hDlg,IDOK),
                              (SendMessage(GetDlgItem(hDlg,IDTEXT),WM_GETTEXTLENGTH,0,0L) &&
                               SendMessage(GetDlgItem(hDlg,IDVALUE),WM_GETTEXTLENGTH,0,0L)) ? 
                               TRUE : FALSE);
               break;

            default : /* something else */
               bResult = FALSE;
               break;
         }
         break;

      default :
         bResult = FALSE;
         break;
   }
   /* return final result */
   return( bResult );
}

/**/
/*
 * ColorButtonInfo() : HANDLE
 *
 * This function returns a handle to a global block of memory that
 * contains various information about the kinds of controls the library
 * is capable of supporting.  This data block can, for example, be used
 * by the dialog editor when determining the capabilities of a particular
 * control library.
 *
 * Note that this handle becomes the property of the caller once this
 * function returns.  This implies that the caller must call GlobalFree
 * once it is finished with the data.
 *
 */

HANDLE APIENTRY ColorButtonInfo()
{
   HANDLE       hCtlInfo;
   LPCTLINFO    lpCtlInfo;

   /* allocate space for information structure */
   hCtlInfo = GlobalAlloc( GHND, (DWORD)sizeof(CTLINFO) );
   if ( hCtlInfo ) {

      /* attempt to lock it down */
      lpCtlInfo = (LPCTLINFO)GlobalLock( hCtlInfo );
      if ( lpCtlInfo ) {

         /* define the fixed portion of the structure */
         lpCtlInfo->wVersion = 100;
         lpCtlInfo->wCtlTypes = 1;
         lstrcpy( lpCtlInfo->szClass, COLORBUTTONCLASS );
         lstrcpy( lpCtlInfo->szTitle, "Sample User Control" );

         /* define the variable portion of the structure */
         lpCtlInfo->Type[0].wWidth = 25;
         lpCtlInfo->Type[0].wHeight = 15;
         lpCtlInfo->Type[0].dwStyle = WS_CHILD;
         lstrcpy( lpCtlInfo->Type[0].szDescr, "ColorButton" );

         /* unlock it */
         GlobalUnlock( hCtlInfo );
      }
      else {

         GlobalFree( hCtlInfo );
         hCtlInfo = 0;
      }
   }
   /* return result */
   return( hCtlInfo );

}
 
/**/
/*
 * ColorButtonStyle( hWnd, hCtlStyle, lpfnVeriyId, lpfnGetIdStr ) : BOOL;
 *
 *    hWnd           handle to parent window
 *    hCtlStyle      handle to control style
 *    lpfnVerifyId   pointer to the VerifyId function from Dialog editor
 *    lpfnGetIdStr   pointer to the GetIdStr functionn from Dialog editor
 *
 * This function enables the user to edit the style of a particular
 * control provided.  The current control style information is passed
 * in using a handle to a control style data structure.
 *
 * This function returns this same handle (referencing updated
 * information) if the dialog box is normally closed.  A value of
 * NULL is returned if the user cancelled the operation.
 *
 */

BOOL APIENTRY ColorButtonStyle(HWND        hWnd,
                               HANDLE      hCtlStyle,
                               LPFNSTRTOID lpfnVerifyId,
                               LPFNIDTOSTR lpfnGetIdStr )
{
   FARPROC        lpDlgFn;
   HANDLE        hNewCtlStyle;

   /* initialization */
   hLibData  = hCtlStyle;
   lpfnVerId = lpfnVerifyId;
   lpfnIdStr = lpfnGetIdStr;

   /* display dialog box */
   lpDlgFn = MakeProcInstance( (FARPROC)ColorButtonDlgFn, hLibInstance );
   hNewCtlStyle = ( DialogBox(hLibInstance,"ColorButtonStyle",hWnd,
                    lpDlgFn) ) ? hLibData : 0;
   FreeProcInstance( lpDlgFn );

   /* return updated data block */
   return( hNewCtlStyle );

}

/**/
/*
 * ColorButtonFlags( wFlags, lpszString, wMaxString ) : UINT;
 *
 *    wFlags         class style flags
 *    lpszString     class style string
 *    wMaxString     maximum size of class style string
 *
 * This function translates the class style flags provided into a
 * corresponding text string for output to an RC file.  The general
 * windows flags (contained in the low byte) are not interpreted,
 * only those in the high byte.
 *
 * The value returned by this function is the library instance
 * handle when sucessful, and NULL otherwise.
 *
 */

#ifdef __BORLANDC__
#pragma argsused
#endif

WORD APIENTRY ColorButtonFlags(DWORD       dwFlags,
                               LPSTR       lpszString,
                               UINT        wMaxString )
{

   if (LOWORD(dwFlags) & BS_DEFPUSHBUTTON) {
      lstrcpy(lpszString, "BS_DEFPUSHBUTTON | WS_TABSTOP");
      return( 29 );
   }
   else {
      lstrcpy(lpszString, "WS_TABSTOP");
      return( 10 );
   }
}

/*
   Borland Resource Workshop stuff added by Urs Rohrer, PSI, November 1992
   Extracted from RotKnob or AnaMtr by M. Buchheit
*/

/*****************************************************************************
S p e c i f y I n f o
==============================================================================

### entry function ###

This function returns a handle to an information block which describes the
specified control. Included in the information returned is the suggested size
of the control, default style flags and other useful data.

Parameters
 none

Return
 The handle of a created global data block with the CTLINFO data structure
 is returned.

*****************************************************************************/

#ifdef __BORLANDC__
#pragma argsused
#endif

static VOID SpecifyInfo(LPCTLINFO pCtlInfo, UINT cbTypeStruct)

 {CTLTYPE FAR *pCtlType;
  ////// set general information structure
  pCtlInfo->wCtlTypes=1; // number of control types
  pCtlInfo->wVersion=CLASS_VERSION;
  lstrcpy(pCtlInfo->szClass,CLASS_NAME);
  LoadString(hib,ST_TITLE,pCtlInfo->szTitle,CTLTITLE);
  ////// specify class 0 type
  pCtlType=(CTLTYPE FAR*)((BYTE FAR*)pCtlInfo->Type+0*cbTypeStruct);
  pCtlType->wType=0;
  pCtlType->wWidth=44;
  pCtlType->wHeight=44;
  pCtlType->dwStyle=CLASS_WNDSTYLE|RKS_DISCRET|5;
  LoadString(hib,ST_DESCR_0,pCtlType->szDescr,CTLDESCR);
  ////// specify class 1 type
  // pCtlType=(CTLTYPE FAR*)((BYTE FAR*)(pCtlInfo->Type+1*cbTypeStruct);
  // pCtlType->wType=0;
  // ...
 }

/*****************************************************************************
C o l o r B u t t o n I n f o R W
==============================================================================

### entry function ###

This function returns a handle to an information block which describes the
specified control. Included in the information returned is the suggested size
of the control, default style flags and other useful data.

Parameters
 none

Return
 The handle of a created global data block with the RWCTLINFO data structure
 is returned.

Comments
 This function is the variant of the function ColorButtonInfo() for the Borland
 Resource Workshop.

*****************************************************************************/

HANDLE APIENTRY ColorButtonInfoRW(VOID)

 {HANDLE hgmCtlInfo;
  RWCTLINFO FAR *pCtlInfo;
  UINT i;
  // allocate memory for control information block
  hgmCtlInfo=GlobalAlloc
   (GMEM_MOVEABLE|GMEM_ZEROINIT,(DWORD)sizeof(RWCTLINFO));
  if (!hgmCtlInfo) return 0; // memory too small
  // try to lock
  pCtlInfo=(VOID FAR*)GlobalLock(hgmCtlInfo);
  if (pCtlInfo==NULL)
   {// bad locking: free memory, return without data
    GlobalFree(hgmCtlInfo); return 0;
   } /* if */
  // set class specific information data into allocated buffer
  SpecifyInfo((LPCTLINFO)pCtlInfo,sizeof(RWCTLTYPE));
  // add toolbox bitmap and drag&drop-cursor to allocated types
  for (i=0;i<pCtlInfo->wCtlTypes;i++)
   {pCtlInfo->Type[i].hToolBit=
      LoadBitmap(hib,MAKEINTRESOURCE(BMP_TLBOX_0+i));
    pCtlInfo->Type[i].hDropCurs=
      LoadCursor(hib,MAKEINTRESOURCE(CRS_DRGDRP_0+i));
   } /* for */
  // return set information
  GlobalUnlock(hgmCtlInfo); return hgmCtlInfo; // return handle
 }

/*****************************************************************************
C o l o r B u t t o n F l a g s R W
==============================================================================

### entry function ###

This function converts the specified style flags into a text string that
corresponds to that used in the control header.

Parameters
 ulStyle   class specific control style code.
 pszStyle  points to a buffer which contains the style string after return.
 cchStyle  contains the length of the <pszStyle> buffer.

Return
 The function returns 0 if the text string is created and a non-0-value
 if the function fails.

Comments
 This function is the variant of the function ColorButtonFlags() for the Borland
 Resource Workshop.

*****************************************************************************/

UINT APIENTRY ColorButtonFlagsRW(DWORD ulStyle,LPSTR pszStyle,UINT cchStyle)

 {// call standard function
  ColorButtonFlags((UINT)ulStyle,pszStyle,cchStyle);
  return 0; // return no error
 }


/*****************************************************************************
L i s t C l a s s e s
==============================================================================

### entry function ###

This function is only needed for the Borland's Resource Toolkit. It returns
the basic structure of type CTLCLASSLIST which contains all available classes
and entry functions.

Parameters
 pszAppClass  contains the class name of the dialog editor toolkit.
 uVersion ... contains the version number of the dialog editor toolkit.
 pfnLoad .... points to a function to load project specific resources.
 pfnEdit .... points to a function to start the editor of a project specific
              resource.

Return
 The handle of an allocated and initialized global data block is returned
 which contains the CTLCLASSLIST structure.

*****************************************************************************/

#ifdef __BORLANDC__
#pragma argsused
#endif

HANDLE APIENTRY ListClasses
 (LPSTR pszAppClass,UINT uVersion,FARPROC pfnLoad,FARPROC pfnEdit)

 {HANDLE hgmCtlList;
  CTLCLASSLIST FAR *pCtlList;
  // allocate memory for control list block
  hgmCtlList=GlobalAlloc
   (GMEM_MOVEABLE|GMEM_ZEROINIT,(DWORD)sizeof(CTLCLASSLIST));
  if (!hgmCtlList) return 0; // memory too small
  // try to lock
  pCtlList=(VOID FAR*)GlobalLock(hgmCtlList);
  if (pCtlList==NULL)
   {// bad locking: free memory, return without data
    GlobalFree(hgmCtlList); return 0;
   } /* if */
  ////// set library specific list of available classes
  pCtlList->nClasses=1;
  pCtlList->Classes[0].fnRWInfo=(LPFNINFO)ColorButtonInfoRW;
  pCtlList->Classes[0].fnRWStyle=(LPFNSTYLE)ColorButtonStyle;
  pCtlList->Classes[0].fnFlags=(LPFNFLAGS)ColorButtonFlagsRW;
  lstrcpy(pCtlList->Classes[0].szClass,CLASS_NAME);
  ////// return set information
  GlobalUnlock(hgmCtlList); return hgmCtlList; // return handle
 }

#else

#ifdef __BORLANDC__
#pragma argsused
#endif

BOOL APIENTRY ColorButtonDlgFn(HWND hwDbx, UINT iMsg, WPARAM uP1, LPARAM lP2)
{
   return FALSE;
}

#ifdef __BORLANDC__
#pragma argsused
#endif

BOOL APIENTRY ColorButtonStyle
 (HWND hw,HANDLE hgmCtlStyle, FARPROC lpfnStrToId, FARPROC lpfnIdToStr)
{
   return (BOOL)1;
}

HANDLE APIENTRY ColorButtonInfo(VOID)
{
   return (HANDLE)1;
}

HANDLE APIENTRY ColorButtonInfoRW(VOID)
{
   return (HANDLE)1;
}

#ifdef __BORLANDC__
#pragma argsused
#endif

UINT APIENTRY ColorButtonFlags(DWORD uStyle, LPSTR pszStyle, UINT cchStyle)
{
   return (UINT)1;
}

#ifdef __BORLANDC__
#pragma argsused
#endif

UINT APIENTRY ColorButtonFlagsRW(DWORD ulStyle, LPSTR pszStyle, UINT cchStyle)
{
   return (UINT)1;
}

#ifdef __BORLANDC__
#pragma argsused
#endif

HANDLE APIENTRY ListClasses
 (LPSTR pszAppClass, UINT uVersion, FARPROC pfnLoad, FARPROC pfnEdit)
{
   return (HANDLE)1;
}

#endif

void Initialize(HANDLE hInstance)
{
#ifdef WIN32
   WNDCLASSEX  ClassStruct;
#else
   WNDCLASS    ClassStruct;
#endif

   // register ColorButton window if necessary

   if ( hLibInstance == 0 )
   {
#ifdef WIN32
	 ClassStruct.cbSize =        sizeof(WNDCLASSEX);
#endif
         ClassStruct.lpszClassName = (LPSTR)COLORBUTTONCLASS;
         ClassStruct.hCursor =       LoadCursor( 0, IDC_ARROW );
         ClassStruct.lpszMenuName =  (LPSTR)NULL;
         ClassStruct.style =         CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS|CS_GLOBALCLASS;
         ClassStruct.lpfnWndProc =   (WNDPROC)ColorButtonWndFn;
         ClassStruct.hInstance =     hInstance;
         ClassStruct.hIcon =         0;
#ifdef WIN32
         ClassStruct.hIconSm =       0;
#endif
         ClassStruct.cbWndExtra =    COLORBUTTON_EXTRA;
         ClassStruct.cbClsExtra =    0;
         ClassStruct.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1 );
#ifdef WIN32   
         hLibInstance = ( RegisterClassEx(&ClassStruct) ) ? hInstance : 0;
#else
         hLibInstance = ( RegisterClass(&ClassStruct) ) ? hInstance : 0;
#endif

      // Create default GDI object 

      // First the outline pen

         hOutlinePen     = GetStockObject(BLACK_PEN);

      // Now default text color & font

         crDefNormalText = GetSysColor(COLOR_BTNTEXT);
         hDefLabelFont   = GetStockObject(SYSTEM_FONT);

      // Now default brushes 

         hDefFaceBrush   = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
         hDefShadowBrush = CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
//         hDefHiliteBrush = GetStockObject(WHITE_BRUSH);
         hDefHiliteBrush = CreateSolidBrush(GetSysColor(COLOR_BTNHILIGHT));

         lpfnDrawLabel = MakeProcInstance(DrawLabel, hInstance);

         hib = hInstance;
   }
}

void Detach(void)
{
#ifndef WIN32
    FreeProcInstance(lpfnDrawLabel);
#endif
    DeleteObject(hDefFaceBrush);
    DeleteObject(hDefShadowBrush);
    DeleteObject(hDefHiliteBrush);
    DeleteObject(hDefLabelFont);
    DeleteObject(hOutlinePen);
}

#ifdef WIN32
#ifdef __BORLANDC__
#pragma argsused

BOOL WINAPI DllEntryPoint( HINSTANCE hInst,
                           DWORD ul_reason_being_called, LPVOID plvReserved)
{
    switch (ul_reason_being_called)
    {
       case DLL_PROCESS_ATTACH: Initialize(hInst); break;
       case DLL_THREAD_ATTACH: break;
       case DLL_THREAD_DETACH: break;
       case DLL_PROCESS_DETACH: Detach(); break;
       default: break;
    }
    return 1;
}

#else

INT  APIENTRY LibMain(HANDLE hInst, 
                      DWORD ul_reason_being_called, LPVOID lpReserved)
{
    switch (ul_reason_being_called)
    {
       case DLL_PROCESS_ATTACH: Initialize(hInst); break;
       case DLL_THREAD_ATTACH: break;
       case DLL_THREAD_DETACH: break;
       case DLL_PROCESS_DETACH: Detach(); break;
       default: break;
    }
    return 1;

    UNREFERENCED_PARAMETER(lpReserved);
}
#endif
#else

#ifdef __BORLANDC__
#pragma argsused
#endif

int APIENTRY LibMain( HANDLE hInstance,
                      UINT   wDataSegment,
                      UINT   wHeapSize,
                      LPSTR  lpszCmdLine )
{
   Initialize(hInstance);

   /* return result 1 = success; 0 = fail */

   return( hLibInstance ? 1 : 0 );
}

#ifdef __BORLANDC__
#pragma argsused
#endif

int APIENTRY WEP (int bSystemExit)
{
    Detach();
    return 1;
}

#endif
