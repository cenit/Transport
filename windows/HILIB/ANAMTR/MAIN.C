/*****************************************************************************

 ----  A N A M T R  -------- MS-Windows Class Library -----------------------
                             Module MAIN: Window Function

==============================================================================

This MS-Windows module contains a dialog control class which provides an
analog meter to display numerical values.

------------------------------------------------------------------------------

This module contains the window function of the dialog class.

------------------------------------------------------------------------------

Copyright 1988, 1989, 1990, 1991, 1992 by Marcellus Buchheit, Germany
Internet mabu@buchheit.de, CompuServe 100020,1723, Fax +49(0)721/356238

Release 1.00 of 91-Dec-30 --- All rights reserved.

*****************************************************************************/
 
// include all common declarations
#include "COMMON.H"

#define PI 3.14159265354

// structure for one scale line
typedef struct
 {POINT ptStart;
  POINT ptStop;
 }
SCLINE; // prefix "scln"


/* ===========================================================================
   WNDDATA                          structure containing the local window data
   ---------------------------------------------------------------------------
   hwDigital .... contains the handle of the digital value child window.
   cxClient ..... is the width of the window client rectangle.
   cyClient ..... is the height of the window client rectangle
   cyScale ...... is the vertical height of the scale in pixels.
   yScalePos .... is the top position of the outermost scale circle.
   cScaleLine ... is the currently set number of scale lines.
   nMinVal ...... contains the currently set scale minimum value.
   nMaxVal ...... contains the currently set scale maximum value.
   hfnt ......... contains the currently set font or 0 if the standard
                  system font is used.
   dftRadius3 ... contains the size of the outest scale circle in pixels.
   dftRadiusSqr3  contains the square of <flRadiusSqr3>.
   cScaleDelta .. is the number of (short) scale lines between names (long)
                  scale lines.
   hmScale ...... is the handle of a movable local memory object which
                  contains the coordinates of the scale. The object contains
                  <cScaleLine> entries of type SCLINE.
   ptPoint ...... is the position the current analog pointer in the scale.
   ptCenter ..... is the center of the analog pointer in the bottom area of
                  the analog meter.
   fInvalid ..... contains the flag "scale contents is invalid and must be
                  recalculated".
   ===========================================================================
*/

typedef struct
 {HWND hwDigital;
  int cxClient,cyClient;
  int cyScale;
  int yScalePos;
  int cScaleLine;
  int nMinVal,nMaxVal;
  HFONT hfnt;
  double dftRadius3,dftRadius3Sqr;
  int cScaleDelta;
  HANDLE hmScale;
  POINT ptPoint;
  POINT ptCenter;
  BYTE fInvalid;
 }
WNDDATA;


/* ------------------------
   common drawing variables
   ------------------------
*/

// GDI tools
static HPEN hpnDraw;
static HPEN hpnRed;
static HPEN hpnFrame;
static HBRUSH hbrBkgr;

// system specific variables
static UINT cxScreenAspect; // factor pixels/mm (*100) horizontal
static UINT cyScreenAspect; // factor pixels/mm (*100) vertical


/*****************************************************************************
A m c I n i t
==============================================================================

This function initializes all global class data. This concerns screen output
constants and other values.

Parameters
 none

Return
 none

*****************************************************************************/

#pragma alloc_text(INIT_TEXT,AmcInit)

VOID APIENTRY AmcInit(VOID)

 {HDC hdc=CreateIC("Display",NULL,NULL,NULL);
  cxScreenAspect=MulDiv(GetDeviceCaps(hdc,HORZRES),100,
                        GetDeviceCaps(hdc,HORZSIZE)
                       );
  cyScreenAspect=MulDiv(GetDeviceCaps(hdc,VERTRES),100,
                        GetDeviceCaps(hdc,VERTSIZE)
                       );
  DeleteDC(hdc);
 } /* AmcInit() */


/*****************************************************************************
C r e a t e D r a w i n g T o o l s
==============================================================================

This function creates the standard drawing tools for the window and sets it
as the default values of <hdc>.

Parameters
 hw   is the handle of the control window.
 hdc  is the display context.
 p .. points to the local window data.

Return
 The function returns TRUE if all tools are created and FALSE if not.

*****************************************************************************/

#ifdef __BORLANDC__
#pragma argsused
#endif

static BOOL CreateDrawingTools(HWND hw, HDC hdc, WNDDATA *p)
{
  hbrBkgr=CreateSolidBrush(RGB(255,255,255));
  // pen color is determined from current set text color
  hpnDraw=CreatePen(PS_SOLID,1,GetTextColor(hdc));
  // pen color is red
  hpnRed=CreatePen(PS_SOLID,1,RGB(255,0,0));
  // pen for frame of analog meter
  hpnFrame=CreatePen(PS_SOLID,1,GetSysColor(COLOR_WINDOWFRAME));
  // select created elements (except memory is too small)
  if (hbrBkgr) SelectObject(hdc,hbrBkgr);
  if (hpnDraw) SelectObject(hdc,hpnDraw);
  // select font if it is not the system font ...
  if (p->hfnt) SelectObject(hdc,p->hfnt); 
  return TRUE;
} /* CreateDrawingTools() */

/*****************************************************************************
D e s t r o y D r a w i n g T o o l s
==============================================================================

This function destroys all created drawing tools. The function is called
after the display context is released.

Parameters
 none

Return
 none

*****************************************************************************/

static VOID DestroyDrawingTools(VOID)
{
  if (hpnDraw)
  {
     DeleteObject(hpnDraw); hpnDraw=0;
  }

  if (hpnRed)
  {
     DeleteObject(hpnRed); hpnRed=0;
  }

  if (hpnFrame)
  {
     DeleteObject(hpnFrame); hpnFrame=0;
  }

  if (hbrBkgr)
  {
     DeleteObject(hbrBkgr); hbrBkgr=0;
  }
}


/*****************************************************************************
S e t P o i n t e r
==============================================================================

This function calculates the position of the analog pointer in the scale
by the current value <nValue>. The position is stored into the WNDDATA
structure.

Parameters
 nValue  contains the new position.
 p ..... points to the local window data.

Return
 none

*****************************************************************************/

VOID SetPointer(int nValue,WNDDATA *p)

 {// set new analog pointer
  double dftAngle=
    3.14159265354/2.*((double)((LONG)nValue-(LONG)p->nMinVal)/
          (double)((LONG)p->nMaxVal-(LONG)p->nMinVal)
          -0.5
         );
  double dftY1;
  // overflow/underflow detection: reduce value
  if (dftAngle<-1.2/4.0*3.14159265354) dftAngle=-1.2/4.0*3.14159265354;
    else if (dftAngle>1.2/4.0*3.14159265354) dftAngle=1.2/4.0*3.14159265354;
  // determine scale position, convert into pixel coordinates
  dftY1=p->dftRadius3*cos(dftAngle);
  p->ptPoint.x=p->cxClient/2+
    (int)sqrt(fabs(p->dftRadius3Sqr-dftY1*dftY1))*(dftAngle<0.0?-1:+1);
  p->ptPoint.y=p->yScalePos+
    MulDiv((int)(p->dftRadius3-dftY1),cyScreenAspect,cxScreenAspect);
 } /* SetPointer() */


/*****************************************************************************
S e t u p W i n d o w C o n t e n t s
==============================================================================

This function calculates the complete contents of the window. The position
and the look of the scale are determined. All data are stored into the
specified WNDDATA structure. 

Parameters
 hw  contains the handle of the control window.
 p   points to the local window data.

Return
 none

*****************************************************************************/

static VOID SetupWindowContents(HWND hw,WNDDATA *p)

 {char szText[128+1];
  RECT rc;
  unsigned char far *psz;
  int iLine,nLine1,nLine2;
  int cxHalf;       // half scale rectangle width
  int yScalePos;    // position above top scale radius
  int cxCharWidth;  // average width of a character in the current sel. font
  int cyCharHeight; // height of a character in the current selected font
  SCLINE *pascln=(SCLINE*)LocalLock(p->hmScale);
  //// determine size of client rectangle and scale
  GetClientRect(hw,&rc);
  p->cxClient=rc.right-rc.left; p->cyClient=rc.bottom-rc.top;
  cxHalf=p->cxClient/2;
   {//// determine size of selected font
    TEXTMETRIC txmtr;
    HDC hdc=GetDC(hw);
    if (p->hfnt) SelectObject(hdc,p->hfnt);
    GetTextMetrics(hdc,&txmtr);
    cxCharWidth=txmtr.tmAveCharWidth; cyCharHeight=txmtr.tmHeight;
    ReleaseDC(hw,hdc);
   } /* block */
  // determine number of longer scale lines from parts of window text
  GetWindowText(hw,szText,sizeof(szText)); psz=(BYTE FAR *)szText;
  // first part of text is unity: start with -1
  for (iLine=-1;*psz;iLine++)
   {// skip characters until next tabulate
    while (*psz && *psz++!=';')
       ;
   } /* for */
  yScalePos=3*cyCharHeight/2;
  // set count for short lines between long scale lines
  p->cScaleDelta=(iLine<=1||iLine>=p->cScaleLine)?
                 1:(p->cScaleLine-1)/(iLine-1);
   {//// calculate scale positions
    double dftRadius1=0.9*(double)cxHalf; // circle 1: 0.9 of half width
    double dftRadius2=(double)cxHalf;     // circle 2: 1.0 of half width
    double dftRadius3=1.1*(double)cxHalf; // circle 3: 1.1 of half width
    // calculates square values
    double dftRadius1Sqr=dftRadius1*dftRadius1;
    double dftRadius2Sqr=dftRadius2*dftRadius2;
    double dftRadius3Sqr=dftRadius3*dftRadius3;
    double dftDelta=0; // angle between two scale points
    double dftAngle;
    SCLINE *pscln1,*pscln2;
    if (p->cScaleLine>=2)
     {// angle between two scale points for 90-degree-scale
      dftDelta=(90.0/180.0*3.14159265354)/(p->cScaleLine-1);
     } /* if */
    //// calculate points for both halfes of scale
    for (dftAngle=p->cScaleLine&1? 0.0:dftDelta/2,
         nLine1=(p->cScaleLine-1)/2,pscln1=pascln+nLine1,
         nLine2=(p->cScaleLine)/2,pscln2=pascln+nLine2;
         nLine1>=0;
         dftAngle+=dftDelta,nLine1--,pscln1--,nLine2++,pscln2++
        )
     {POINT pt1,pt2,pt3;
      double dftCos=cos(dftAngle);
      // determine y float value for radius 1/2/3
      double dftY1=dftCos*dftRadius1;
      double dftY2=dftCos*dftRadius2;
      double dftY3=dftCos*dftRadius3;
      // determine pixel scale points for radius 1/2/3
      pt1.x=(int)sqrt(fabs(dftRadius1Sqr-dftY1*dftY1));
      pt1.y=(int)(dftRadius3-dftY1);
      pt2.x=(int)sqrt(fabs(dftRadius2Sqr-dftY2*dftY2));
      pt2.y=(int)(dftRadius3-dftY2);
      pt3.x=(int)sqrt(fabs(dftRadius3Sqr-dftY3*dftY3));
      pt3.y=(int)(dftRadius3-dftY3);
      //// set scale line values
      // start positions: always radius 1
      // stop positions: radius 2 for short lines, 3 for long lines
      pscln1->ptStart.x=cxHalf-pt1.x;
      pscln1->ptStart.y=yScalePos+
        MulDiv(pt1.y,cyScreenAspect,cxScreenAspect);
      pscln1->ptStop.x=cxHalf-(nLine1%p->cScaleDelta==0? pt3.x:pt2.x);
      pscln1->ptStop.y=yScalePos+
        MulDiv(nLine1%p->cScaleDelta==0? pt3.y:pt2.y,cyScreenAspect,
               cxScreenAspect
              );
      pscln2->ptStart.x=cxHalf+pt1.x;
      pscln2->ptStart.y=pscln1->ptStart.y; // symmetric scale
      pscln2->ptStop.x=cxHalf+(nLine2%p->cScaleDelta==0? pt3.x:pt2.x);
      pscln2->ptStop.y=yScalePos+
        MulDiv(nLine2%p->cScaleDelta==0? pt3.y:pt2.y,cyScreenAspect,
               cxScreenAspect
              );
     } /* for */
    p->yScalePos=yScalePos; // store vertical scale position
    // set origin of analog pointing line
    p->ptCenter.x=cxHalf;
    p->ptCenter.y=yScalePos+
      MulDiv((int)dftRadius3,cyScreenAspect,cxScreenAspect);
    // set height of scale
    p->cyScale=pascln[0].ptStart.y+cyCharHeight;
    p->cyScale=yScalePos+cxHalf/2+cyCharHeight;
    // save radius 3 value and square as window specific data
    p->dftRadius3=dftRadius3; p->dftRadius3Sqr=dftRadius3Sqr;
   } /* block */
   {//// setup digital output
    DWORD ulStyle=GetWindowLong(hw,GWL_STYLE);
    BOOL fDigital=(ulStyle&AMS_DIGITAL)!=0;
    if (fDigital && !p->hwDigital)
     {//// create digital display as child window
#ifdef WIN32
      p->hwDigital=CreateWindow
       ("static","",SS_CENTER|WS_BORDER|WS_VISIBLE|WS_CHILD,
        0,0,0,0,hw,0,(HINSTANCE)GetWindowLong(hw,GWL_HINSTANCE),NULL);
#else
      p->hwDigital=CreateWindow
       ("static","",SS_CENTER|WS_BORDER|WS_VISIBLE|WS_CHILD,
        0,0,0,0,hw,0,GetWindowWord(hw,GWW_HINSTANCE),NULL);
#endif
      // set specific font to child if any is set in the window
      if (p->hfnt) SendMessage(p->hwDigital,WM_SETFONT,(WPARAM)p->hfnt,0);
      // initialize data of digital window
      SetDlgItemInt(hw,0,p->nMinVal,TRUE);
      // clip now child window from client area
      SetWindowLong(hw,GWL_STYLE,ulStyle|WS_CLIPCHILDREN);
     }
    else if (!fDigital && p->hwDigital)
     {//// digital window no longer needed: destroy it
      DestroyWindow(p->hwDigital); p->hwDigital=0;
      // clipping of child window no longer needed
      SetWindowLong(hw,GWL_STYLE,ulStyle&~WS_CLIPCHILDREN);
     } /* if */
    if (p->hwDigital)
     {//// set position and size of digital window
      MoveWindow
       (p->hwDigital,(p->cxClient-6*cxCharWidth)/2,
        p->cyScale+(p->cyClient-p->cyScale-cyCharHeight)/2,
        6*cxCharWidth,cyCharHeight+2,TRUE
       );
     } /* if */
   } /* block */
  SetPointer(p->nMinVal,p); // set pointer to minimum value
  LocalUnlock(p->hmScale);
  p->fInvalid=FALSE; // window contents now valid
 } /* SetupWindowContents() */


/*****************************************************************************
A n a M t r W n d F n
==============================================================================

### window function ###

This function processes any messages received by a window of the window class
"AnalogMeter".

*****************************************************************************/

LONG APIENTRY AnaMtrWndFn(HWND hw, UINT iMsg, WPARAM uP1, LPARAM lP2)
{  
  WNDDATA *p;   // pointer to local window data
  LONG lRet=0;  // return value
  HANDLE hData; // local memory handle of window class data

  if (!hw) return 0L;

#ifdef WIN32
  hData=(HANDLE)GetWindowLong(hw,0); //// pointer to local window data
#else
  hData=(HANDLE)GetWindowWord(hw,0); //// pointer to local window data
#endif
  if (hData) p=(WNDDATA*)LocalLock(hData);

  //// analyse message

  switch (iMsg)
  { case WM_NCCREATE:

      //// create and initialize memory block for window data

      hData=LocalAlloc(LMEM_MOVEABLE|LMEM_ZEROINIT,sizeof(WNDDATA));
      if (!hData) return 0; // not allocated => create-error

      // set handle of allocated memory, init data

#ifdef WIN32
      SetWindowLong(hw,0,(LONG)hData);
#else
      SetWindowWord(hw,0,(WORD)hData);
#endif
      p=(WNDDATA*)LocalLock(hData);

      //// init local window data
      // number of scale lines: determine from windows style

      p->cScaleLine=LOWORD(GetWindowLong(hw,GWL_STYLE)&AMS_LINEMASK);
      p->hmScale=LocalAlloc(LMEM_MOVEABLE,sizeof(SCLINE)*p->cScaleLine);
      if (!p->hmScale) p->cScaleLine=0; // no memory for table => no scale

      // init maximum value (minimum already 0)

      p->nMaxVal=100;
      break; // continue with creation

    case WM_SETFONT:
      //// set new font of window
      // send message to child if any without redrawing
      if (p->hwDigital) SendMessage(p->hwDigital,WM_SETFONT,uP1,0);
      p->hfnt=(HFONT)uP1; // set new font
      p->fInvalid=TRUE; // new scale must be recalculated
      InvalidateRect(hw,NULL,TRUE); // redraw window (later...)
      if (P2LO) UpdateWindow(hw);   // redraw immediately
      goto RETURN; // return 0

    case WM_GETFONT:
      //// return handle of currently set window font
      lRet=(LONG)p->hfnt; // return 0 if SYSTEM font selected
      goto RETURN;  // return p->hfnt

    case AMM_SETRANGE:
      //// set new low and high value of scale range
      p->nMinVal=(int)P2LO; p->nMaxVal=(int)P2HI; goto SETINV;

    case AMM_SETSIZE:
      //// set number of scale points: read from attributes if <uP1> invalid
      p->cScaleLine=(int)((int)uP1<0?
        LOWORD(GetWindowLong(hw,GWL_STYLE)&AMS_LINEMASK):uP1);
      // allocate new buffer in local memory
      if (p->hmScale) LocalFree(p->hmScale);
      p->hmScale=LocalAlloc(LMEM_MOVEABLE,sizeof(SCLINE)*p->cScaleLine);
      if (p->hmScale==0) p->cScaleLine=0; // no memory for table => no scale

    SETINV:
      p->fInvalid=TRUE; // new scale must be recalculated
      InvalidateRect(hw,NULL,TRUE); // redraw window
      goto RETURN; // return 0

    case AMM_GETRANGE:
      //// return last set range values
      lRet=MAKELONG(p->nMinVal,p->nMaxVal); goto RETURN;

    case AMM_GETSIZE:
      //// return number of scale points (from attributes or last setting)
      lRet=MAKELONG(p->cScaleLine,0); goto RETURN;

    case WM_SIZE:
    case WM_SETTEXT:
      p->fInvalid=TRUE; // new scale must be calculated
      InvalidateRect(hw,NULL,TRUE); // redraw window
      break;

    case WM_PAINT:
    {
      PAINTSTRUCT pnt;
      HDC hdc;
      char szText[128+1];
      unsigned char far *psz;
      int i;
      SCLINE *pascln=(SCLINE*)LocalLock(p->hmScale);
      SCLINE *pscln;
      POINT point;
      if (p->fInvalid) SetupWindowContents(hw,p); // update contents
      ////// draw window contents
      hdc=BeginPaint(hw,&pnt); CreateDrawingTools(hw,hdc,p);
      GetWindowText(hw,szText,sizeof(szText));
      SetBkMode(hdc,TRANSPARENT);
      //// draw scale lines: use calculated values in <p->pscln>
      for (i=0,pscln=pascln;i<p->cScaleLine;i++,pscln++)
       {// draw line from start to end point
        POINT point;
        MoveToEx(hdc,pscln->ptStart.x,pscln->ptStart.y,&point);
        LineTo(hdc,pscln->ptStop.x,pscln->ptStop.y);
       } /* for */
      //// draw text of unity into middle of scale
      SetTextAlign(hdc,TA_CENTER|TA_TOP);
      // determine length of unity string
      psz=(BYTE FAR *)szText; while (*psz && *psz!=';') psz++;
      TextOut
       (hdc,p->cxClient/2,(p->cxClient/4+p->cyScale)/2,szText,
        (LPSTR)psz-(LPSTR)szText);
      //// draw text at long scale lines
      SetTextAlign(hdc,TA_CENTER|TA_BOTTOM);
      for (i=0,pscln=pascln;
           *psz && i<p->cScaleLine;
           i+=p->cScaleDelta,pscln+=p->cScaleDelta
          )
       {// determine line of next point line
        unsigned char far *pszStart=++psz; while (*psz && *psz!=';') psz++;
        if (*pszStart && *pszStart!=';')
         {// no empty text: draw it
          TextOut(hdc,pscln->ptStop.x,pscln->ptStop.y,
                  (char *)pszStart,
                  (char *)psz-
                  (char *)pszStart);
         } /* if */   
       } /* for */
      //// set gray cover rectangle, edges invisible except at top
      SelectObject(hdc,hpnFrame);
      SelectObject(hdc,GetStockObject(GRAY_BRUSH));
      Rectangle(hdc,-1,p->cyScale,p->cxClient+1,p->cyClient+1);
      //// draw line of analog pointer (only inside of scale)
      IntersectClipRect(hdc,0,0,p->cxClient,p->cyScale);
      SelectObject(hdc,hpnRed);
      MoveToEx(hdc,p->ptPoint.x,p->ptPoint.y,&point);
      LineTo(hdc,p->ptCenter.x,p->ptCenter.y);
      LocalUnlock(p->hmScale);
      EndPaint(hw,&pnt); DestroyDrawingTools();
      goto RETURN; // return 0
     } /* case */
    case AMM_SETVALUE:
      //// new value set: update line of analog pointer
      if (p->fInvalid)
       {// complete new scale needed: determine positions
        SetupWindowContents(hw,p);
        InvalidateRect(hw,NULL,TRUE); // redraw comletely
       }
      else if (p->ptPoint.x!=0)
       {// remove old pointer line from scale area
        HDC hdc=GetDC(hw);
        HPEN hpnErase;
        RECT rc;
        POINT point;
        CreateDrawingTools(hw,hdc,p);
        SetBkMode(hdc,TRANSPARENT);
        IntersectClipRect(hdc,0,0,p->cxClient,p->cyScale);
//        hpnErase=CreatePen(PS_SOLID,1,GetBkColor(hdc));
        hpnErase=CreatePen(PS_SOLID,1,GetSysColor(COLOR_WINDOW));
        SelectObject(hdc,hpnErase);
        MoveToEx(hdc,p->ptPoint.x,p->ptPoint.y,&point);
        LineTo(hdc,p->ptCenter.x,p->ptCenter.y);
        ReleaseDC(hw,hdc);
        DeleteObject(hpnErase); DestroyDrawingTools();
        // invalidate rectangle which is touched by removed pointer line
        SetRect
         (&rc,min(p->ptPoint.x,p->cxClient/2),p->ptPoint.y,
          max(p->ptPoint.x,p->cxClient/2)+1,p->cyScale
         );
        InvalidateRect(hw,&rc,FALSE); // no background-erasing needed!
       } /* if */
       {// calculate new position of pointer
        RECT rc;
        SetPointer((int)uP1,p);
        SetRect
         (&rc,min(p->ptPoint.x,p->cxClient/2),p->ptPoint.y,
          max(p->ptPoint.x,p->cxClient/2)+1,p->cyScale
         );
        // redraw rectangle of new pointer immediately
        InvalidateRect(hw,&rc,FALSE); // no background erasing needed!
        UpdateWindow(hw);
       } /* if */
      if (p->hwDigital) SetDlgItemInt(hw,0,uP1,TRUE); // update digital value
      goto RETURN; // return 0
    case WM_NCDESTROY:
      //// free allocated local memory for window data
      if (p->hmScale) LocalFree(p->hmScale);
      LocalUnlock(hData); LocalFree(hData); hData=0;
      break; // execute default action
   } /* switch */
  lRet=DefWindowProc(hw,iMsg,uP1,lP2);

  //// return from function: unlock window data memory block

  RETURN:
    if (hData) LocalUnlock(hData);
    return lRet;
}


/* ===================================
   ANAMTR class library: End of MAIN.C
   ===================================
*/
