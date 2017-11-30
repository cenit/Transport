/*****************************************************************************

 ----  R O T K N O B  ------ MS-Windows Class Library -----------------------
                             Module MAIN: Window Function

==============================================================================

This MS-Windows module contains a dialog control class which provides a
rotation knob to enter analog values via keyboard or mouse.

------------------------------------------------------------------------------

This module contains the window function of the dialog class.

------------------------------------------------------------------------------

Copyright 1988, 1989, 1990, 1991, 1992 by Marcellus Buchheit, Germany
Internet mabu@buchheit.de, CompuServe 100020,1723, Fax +49(0)721/356238

Release 1.00 of 91-Dec-30 --- All rights reserved.

Modified by Urs C. Rohrer, PSI, CH-5232 Villigen, Switzerland, November 1992

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
   rcKnob ......... is the rectangle of the knob (without scale)
   hrgnKnob ....... is the handle of the elliptic knob region.
   ptCenter ....... is the pointer of the center of the knob rectangle.
   cScaleLine ..... contains the currently set number of scale lines.
   dftRadius1 ..... is the size of the innermost scale radius in pixels.
   dftRadius2 ..... is the size of the outermost scale radius in pixels.
   nMinVal ........ contains the currently set scale minimum value.
   nMaxVal ........ contains the currently set scale maximum value.
   hfnt ........... contains the currently set font or 0 if the standard
                    system font is used.
   nSelValue ...... contains the current selected scale value.
   nTrackValue .... contains the current scale value during the tracking.
   nStartValue .... contains the scale value before the tracking was started.
   dftPointerAngle  contains the last valid angle during tracking.
   fbStatus ....... contains the following status flags:
                    SF_RESIZE ... is the flag "resize dimension of scale"
                    SF_TRACK .... is the flag "during tracking" 
                    SF_VALPOINTER is the flag "pointer valid during tracking"
   ===========================================================================
*/

typedef struct
 {RECT rcKnob;
  HRGN hrgnKnob;
  POINT ptCenter;
  int  cScaleLine;
  double dftRadius1;
  double dftRadius2;
  int  nMinVal,nMaxVal;
  HFONT hfnt;
  int  nSelValue;
  int  nTrackValue;
  int  nStartValue;
  double dftPointerAngle;
  BYTE fbStatus;
 }
WNDDATA;

#define SF_RESIZE 1
#define SF_TRACK  2
#define SF_VALPOINTER 4

/* ------------------------
   common drawing variables
   ------------------------
*/

// GDI tools
static HBRUSH hbrWhite;   // used for displaying knob shade
static HBRUSH hbrGrey;    // used for displaying knob body
static HBRUSH hbrDGrey;   // used for displaying knob shade
static HBRUSH hbrRed;     // used for displaying focus
static HPEN   hpnWhite;   // used for displaying knob shade
static HPEN   hpnGrey;    // used for displaying knob body
static HPEN   hpnDGrey;   // used for displaying knob shade
static HPEN   hpnBlack;   // used for normal drawing
static HPEN   hpnBlck2;   // used for normal drawing

// system specific variables
static UINT cxScreenAspect; // factor pixels/mm (*100) horizontal
static UINT cyScreenAspect; // factor pixels/mm (*100) vertical

/*****************************************************************************
R k c I n i t
==============================================================================

This function initializes all global class data. This contains screen output
constants and other values.

Parameters
 none

Return
 none

*****************************************************************************/

#pragma alloc_text(INIT_TEXT,RkcInit)

VOID APIENTRY RkcInit(VOID)

 {HDC hdc=CreateIC("Display",NULL,NULL,NULL);
  cxScreenAspect=MulDiv(GetDeviceCaps(hdc,HORZRES),100,
                        GetDeviceCaps(hdc,HORZSIZE)
                       );
  cyScreenAspect=MulDiv(GetDeviceCaps(hdc,VERTRES),100,
                        GetDeviceCaps(hdc,VERTSIZE)
                       );
  DeleteDC(hdc);
 } /* RkcInit() */


/*****************************************************************************
S e t N e w V a l u e
==============================================================================

This function checks the specified value if it is within the range and sends
it in dependence of the setting of the window style RKS_DISCRET to the parent
window. If the option RKS_AUTOSELECT is set, the rotation position is
updated. During tracking, the value is only sent to the parent if RKS_DISCRET
is not set.

Parameters
 p ....... points to the WNDDATA structure of the window.
 hw ...... contains the handle of the window.
 dftAngle  contains the new angle value (in range -Pi..+Pi).

Return
 TRUE is returned if the angle value is valid and FALSE if the angle is
 out of range.

*****************************************************************************/

static BOOL SetNewValue(WNDDATA *p,HWND hw,double dftAngle)

 {int nValue;
  double dftValue;
  BOOL fTrack=p->fbStatus&SF_TRACK;
  if (!fTrack||p->fbStatus&SF_VALPOINTER)
   {//// determine new value from specified angle
    if (fabs(dftAngle)>0.85*PI)
     {// out of min..max tolerance: ignore if not during tracking
      if (!fTrack) return FALSE;
      // during tracking => reduce to min/max angle
      dftAngle=dftAngle>=0? +0.85*PI:-0.85*PI;
     } /* if */
    // determine scale value from angle (round up/down)
    dftValue=dftAngle/(3.0/2.0*PI)*(double)(p->nMaxVal-p->nMinVal)
            +(double)(p->nMinVal+p->nMaxVal)/2.0;
    // round value up/down before converting to integer
    nValue=(int)(dftValue+(dftValue>=0? +0.5:-0.5));
    // reduce new position to value within iMin..iMax
    if (nValue<p->nMinVal) nValue=p->nMinVal;
    else if (nValue>p->nMaxVal) nValue=p->nMaxVal;
   }
  else
   {// out of rotation knob interior during tracking: set start value again
    nValue=p->nStartValue;
   } /* if */
  if (fTrack)
   {// check overwinding, if so => ignore new position & abort
    int nHalf=(p->nMinVal+p->nMaxVal)/2;
    if (nValue>nHalf && p->nTrackValue==p->nMinVal ||
        nValue<nHalf && p->nTrackValue==p->nMaxVal
       )
      return FALSE;
   } /* if */
  if (!fTrack || dftAngle!=p->dftPointerAngle)
   {// invalidate contents of rotation knob
    InvalidateRect(hw,&p->rcKnob,FALSE);
    if (fTrack) p->dftPointerAngle=dftAngle;
   } /* if */
  //// send WM_COMMAND or WMP_VSCROLL message to parent if new value valid
  if (!fTrack||nValue!=p->nTrackValue)
   {// send message with new position to parent window
    DWORD ulStyle=GetWindowLong(hw,GWL_STYLE);
    p->nTrackValue=nValue; // save new value
    if (ulStyle&RKS_DISCRET)
     {// send WM_COMMAND message with new command value to parent
      if (ulStyle&RKS_AUTOSELECT && !fTrack)
       {// set current position as selected position
        p->nSelValue=nValue;
       } /* if */
      UpdateWindow(hw); // redraw knob contents before sending message
      if (!fTrack||!(ulStyle&RKS_AUTOSELECT))
#ifdef WIN32
        SendMessage
        (GetParent(hw),WM_COMMAND,MAKELONG(GetDlgCtrlID(hw),nValue),(LPARAM)hw);
#else
        SendMessage
        (GetParent(hw),WM_COMMAND,GetDlgCtrlID(hw),MAKELONG(hw,nValue));
#endif
     }
    else
    { BOOL ft = fTrack ? SB_THUMBTRACK : SB_THUMBPOSITION;
      // send WMP_VSCROLL message with new position value to parent
      UpdateWindow(hw); // redraw knob contents before sending message
#ifdef WIN32
      SendMessage(GetParent(hw),WMP_VSCROLL,MAKELONG(ft,nValue),(LPARAM)hw);
#else
      SendMessage(GetParent(hw),WMP_VSCROLL,ft,MAKELONG(nValue,hw));
#endif
    } /* if */
   } /* if */
  UpdateWindow(hw); // redraw knob if not already redrawn
  return TRUE; // valid angle value
 } /* SetNewValue() */


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
  //// determine colors

  hbrWhite=CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));
  hbrGrey=CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
  hbrDGrey=CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
  hbrRed=CreateSolidBrush(RGB(255,0,0));
  hpnWhite=CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNHIGHLIGHT));
  hpnGrey=CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNFACE));
  hpnDGrey=CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));
  hpnBlack=CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNTEXT));
  hpnBlck2=CreatePen(PS_SOLID,1,RGB(0,0,0));
  // select created elements (except memory is too small)
  if (hbrWhite) SelectObject(hdc,hbrWhite);
  if (hpnBlack) SelectObject(hdc,hpnBlack);
  // select font if it is not the system font ...
  if (p->hfnt) SelectObject(hdc,p->hfnt); 
  return TRUE;
}


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
  if (hbrWhite) { DeleteObject(hbrWhite); hbrWhite=0; }
  if (hbrGrey)  { DeleteObject(hbrGrey);  hbrGrey=0; }
  if (hbrDGrey) { DeleteObject(hbrDGrey); hbrDGrey=0; }
  if (hbrRed)   { DeleteObject(hbrRed);   hbrRed=0; }
  if (hpnWhite) { DeleteObject(hpnWhite); hpnWhite=0; }
  if (hpnDGrey) { DeleteObject(hpnDGrey); hpnDGrey=0; }
  if (hpnGrey)  { DeleteObject(hpnGrey);  hpnGrey=0; }
  if (hpnBlack) { DeleteObject(hpnBlack); hpnBlack=0; }
  if (hpnBlck2) { DeleteObject(hpnBlck2); hpnBlck2=0; }
}


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

 {int iRadius;
  int cyCharHeight;
  RECT rc; // client area of window
   {//// determine height of selected font
    TEXTMETRIC txmtr;
    HDC hdc=GetDC(hw);
    if (p->hfnt) SelectObject(hdc,p->hfnt);
    GetTextMetrics(hdc,&txmtr);
    cyCharHeight=txmtr.tmHeight;
    ReleaseDC(hw,hdc);
   } /* block */
  //// determine size of client rectangle and scale
  GetClientRect(hw,&rc);
  p->ptCenter.x=rc.right/2;
  p->ptCenter.y=rc.bottom/2;
  if (rc.bottom < rc.right)                           // wider than high
    iRadius=((10*rc.bottom)/14-2*cyCharHeight)/2;     // radius of circle
  else                                                // higher than wide
    iRadius=((10*rc.right)/14-2*cyCharHeight)/2;      // radius of circle
  if (iRadius<cyCharHeight/2) iRadius=cyCharHeight/2; // minimum size
  // reduce radius by maximum aspect factor
  if (cxScreenAspect>cyScreenAspect)
   {iRadius=MulDiv(iRadius,cyScreenAspect,cxScreenAspect);
   }
  else
   {iRadius=MulDiv(iRadius,cxScreenAspect,cyScreenAspect);
   } /* if */
  // calculate size of knob ellipse
  p->rcKnob.left=p->ptCenter.x-iRadius;
  p->rcKnob.right=p->ptCenter.x+iRadius;
  iRadius=MulDiv(iRadius,cyScreenAspect,cxScreenAspect);
  p->rcKnob.top=p->ptCenter.y-iRadius;
  p->rcKnob.bottom=p->ptCenter.y+iRadius;
  if (p->hrgnKnob) DeleteObject(p->hrgnKnob);
  p->hrgnKnob=CreateEllipticRgnIndirect(&p->rcKnob);
  // determine scale size: outer & inner radius
  p->dftRadius1=(double)((p->rcKnob.right-p->rcKnob.left)/2+6);
  // length of scale line: half the radius
  p->dftRadius2=1.3*p->dftRadius1;
//  p->fbStatus!=~SF_RESIZE; // window contents now valid
  p->fbStatus=FALSE; // window contents now valid
 } /* SetupWindowContents() */


/*****************************************************************************
R o t K n o b W n d F n
==============================================================================

### window function ###

This function processes any messages received by a window of the window class
"RotationKnob".

*****************************************************************************/

LONG APIENTRY RotKnobWndFn(HWND hw, UINT iMsg, WPARAM uP1, LPARAM lP2)
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
      SetWindowWord(hw,0,hData);
#endif
      p=(WNDDATA*)LocalLock(hData);

      //// init local window data (all values are already 0!)

      { DWORD ulStyle=GetWindowLong(hw,GWL_STYLE);
        // determine maximum value or set default ...
        p->nMaxVal=ulStyle&RKS_DISCRET? (int)(ulStyle&RKS_SCALEMASK)-1:100;
      } /* block */
      break; // continue with creation

    case WM_SETFONT:
      //// set new font of window
      p->hfnt=(HFONT)uP1; // set new font
      p->fbStatus|=SF_RESIZE;       // new size of scale needed
      InvalidateRect(hw,NULL,TRUE); // redraw window (later...)
      if (P2LO) UpdateWindow(hw);   // redraw immediately
      goto RETURN; // return 0;

    case WM_GETFONT:
      //// return handle of set window font
      lRet=(LONG)p->hfnt; // 0 if SYSTEM font selected
      goto RETURN;  // return p->hfnt

    case WM_GETDLGCODE:
      // if focus: window wants direction keys
      if (hw==GetFocus()) lRet=DLGC_WANTARROWS;
      goto RETURN; // consumed

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
      //// redraw inside of knob (gray if active focus)
      InvalidateRect(hw,NULL,FALSE);
      break; // not consumed

    case WM_SIZE:
    case WM_SETTEXT:
      //// redraw full window
      p->fbStatus|=SF_RESIZE;
      InvalidateRect(hw,NULL,FALSE); UpdateWindow(hw);
      break; // not consumed

    case WM_PAINT:
    { PAINTSTRUCT pnt;
      HDC hdc;
      int dd;
      if (p->fbStatus&SF_RESIZE) SetupWindowContents(hw,p);
      hdc=BeginPaint(hw,&pnt);
      CreateDrawingTools(hw,hdc,p);
      if (!EqualRect(&pnt.rcPaint,&p->rcKnob))
       {//// calculate & redraw scale of knob
        int cch;
        char far *pszText;
        char far *psz;
        HANDLE hmText;
        //// draw shaded rectangle
        {
            POINT point;
            SelectObject(hdc,hpnDGrey);
            SelectObject(hdc,hbrGrey);

            Rectangle(hdc,0,0,2*p->ptCenter.x+1,2*p->ptCenter.y+1);

            SelectObject(hdc,hpnWhite);

            MoveToEx(hdc, 2*p->ptCenter.x-1, 2*p->ptCenter.y-1, &point);
            LineTo  (hdc, 2*p->ptCenter.x-1, 0                        );
            MoveToEx(hdc, 2*p->ptCenter.x-1, 2*p->ptCenter.y-1, &point);
            LineTo  (hdc, 0                , 2*p->ptCenter.y-1        );

            SelectObject(hdc,hpnBlack);
        }
        SelectObject(hdc,hbrWhite);
        SetTextColor(hdc,GetSysColor(COLOR_BTNTEXT));
        SetBkMode(hdc,TRANSPARENT);
        cch=GetWindowTextLength(hw);
        hmText=LocalAlloc(LMEM_MOVEABLE,cch+1);
        pszText=LocalLock(hmText); GetWindowText(hw,pszText,cch+1);
        // determine number of scale points
        for (psz=pszText,p->cScaleLine=0;*psz;psz++)
         {if (*psz==';') p->cScaleLine++;
         } /* for */
        //// draw title text of knob (at bottom)
         {RECT rc;
          for (psz=pszText;*psz && *psz!=';';psz++) // determine length
             ;
          GetClientRect(hw,&rc);
          rc.bottom-=4; // reduce bottom position
          DrawText
           (hdc,pszText,psz-pszText,&rc,DT_SINGLELINE|DT_BOTTOM|DT_CENTER);
         } /* if */
          SetTextAlign(hdc,TA_CENTER|TA_BOTTOM);
        //// calculate and draw scale lines and text
         dd = (int)(p->dftRadius1/2.);
         Arc(hdc,
         p->rcKnob.left-5,p->rcKnob.top-5,p->rcKnob.right+5,p->rcKnob.bottom+5,
         p->ptCenter.x+dd,p->ptCenter.y+dd,p->ptCenter.x-dd,p->ptCenter.y+dd+1);
         {double dftRadius1Sqr=p->dftRadius1*p->dftRadius1;
          double dftRadius2Sqr=p->dftRadius2*p->dftRadius2;
          double dftAngle,dftDelta;
          int i;
          POINT point;
          if (p->cScaleLine>=2)
           {// angle between two scale points for 270-degree-scale
            dftDelta=3.0/2.0*PI/(p->cScaleLine-1); // 3/2 PI is 270 degree
           } /* if */
          // calculate scale points for right half
          for (i=0,dftAngle=-3.0*PI/4.0;
               i<p->cScaleLine;i++,dftAngle+=dftDelta)
           {double dftCos=cos(dftAngle);
            double dftY=dftCos*p->dftRadius1;
            POINT ptStart,ptStop;
            ptStart.x=p->ptCenter.x+
              (int)sqrt(fabs(dftRadius1Sqr-dftY*dftY))*(dftAngle<0.0?-1:+1);
            ptStart.y=p->ptCenter.y-
              MulDiv((int)dftY,cyScreenAspect,cxScreenAspect);
            dftY=dftCos*p->dftRadius2;
            ptStop.x=p->ptCenter.x+
              (int)sqrt(fabs(dftRadius2Sqr-dftY*dftY))*(dftAngle<0.0?-1:+1);
            ptStop.y=p->ptCenter.y-
              MulDiv((int)dftY,cyScreenAspect,cxScreenAspect);
            MoveToEx(hdc,ptStart.x,ptStart.y,&point);
            LineTo(hdc,ptStop.x,ptStop.y);
            if (*psz)
             {// print text to scale point
              for (pszText=++psz;*psz && *psz!=';';psz++)
                 ;
              if (*pszText!=';' && *pszText!=0)
               {// no empty text: draw it
                SetTextAlign
                 (hdc,2*i>=p->cScaleLine? TA_BASELINE|TA_LEFT:
                  2*i==p->cScaleLine-1? TA_CENTER|TA_BOTTOM:
                  TA_BASELINE|TA_RIGHT
                 );
                TextOut(hdc,ptStop.x,ptStop.y,pszText,psz-pszText);
               } /* if */   
             } /* if */
           } /* for */
         } /* block */
        // delete temporarily allocated memory
        LocalUnlock(hmText); LocalFree(hmText);
        //// draw shades
        {HRGN hshade, hring, hE1, hE2, hT;
         POINT triangle[4];
         SelectObject(hdc,hbrWhite);
         //// draw filled circlular ring, gives white rim
         hring = CreateRectRgn(0,0,0,0);
         hE1 = CreateEllipticRgn(p->rcKnob.left-3,p->rcKnob.top-3,
                                 p->rcKnob.right+3,p->rcKnob.bottom+3);
         hE2 = CreateEllipticRgn(p->rcKnob.left,p->rcKnob.top,
                                 p->rcKnob.right,p->rcKnob.bottom);
         CombineRgn(hring,hE1,hE2,RGN_DIFF);
         hshade = CreateRectRgn(0,0,0,0);
         triangle[0].x = p->rcKnob.right+3; triangle[0].y = p->rcKnob.top   -3;
         triangle[1].x = p->rcKnob.left -3; triangle[1].y = p->rcKnob.top   -3;
         triangle[2].x = p->rcKnob.left -3; triangle[2].y = p->rcKnob.bottom+3;
         triangle[3].x = p->rcKnob.right+3; triangle[3].y = p->rcKnob.top   -3;
         hT = CreatePolygonRgn(triangle,4,WINDING);
         CombineRgn(hshade,hring,hT,RGN_AND);
         PaintRgn(hdc,hshade);
         DeleteObject(hshade); DeleteObject(hring), DeleteObject(hT);
         DeleteObject(hE1); DeleteObject(hE2);
         // draw dark grey shade
         SelectObject(hdc,hbrDGrey);
         SelectObject(hdc,hpnDGrey);
         hring = CreateRectRgn(0,0,0,0);
         hE1 = CreateEllipticRgn(p->rcKnob.left-3,p->rcKnob.top-3,
                                 p->rcKnob.right+3,p->rcKnob.bottom+3);
         hE2 = CreateEllipticRgn(p->rcKnob.left,p->rcKnob.top,
                                 p->rcKnob.right,p->rcKnob.bottom);
         
         CombineRgn(hring,hE1,hE2,RGN_DIFF);
         hshade = CreateRectRgn(0,0,0,0);
         triangle[0].x = p->rcKnob.right+3; triangle[0].y = p->rcKnob.top   -3;
         triangle[1].x = p->rcKnob.right+3; triangle[1].y = p->rcKnob.bottom+3;
         triangle[2].x = p->rcKnob.left -3; triangle[2].y = p->rcKnob.bottom+3;
         triangle[3].x = p->rcKnob.right+3; triangle[3].y = p->rcKnob.top   -3;
         hT = CreatePolygonRgn(triangle,4,WINDING);
         CombineRgn(hshade,hring,hT,RGN_AND);
         PaintRgn(hdc,hshade);
         DeleteObject(hshade); DeleteObject(hring), DeleteObject(hT);
         DeleteObject(hE1); DeleteObject(hE2);
         SelectObject(hdc,hpnBlck2);
         Arc(hdc,
         p->rcKnob.left-3,p->rcKnob.top-3,p->rcKnob.right+3,p->rcKnob.bottom+3,
         p->ptCenter.x,p->ptCenter.y+10,p->ptCenter.x,p->ptCenter.y+10);

         SelectObject(hdc,hpnBlack);
        } /* block */
       } /* if */
      //// draw knob itself (ellipse+pointer line)
       {HRGN hring, hE1, hE2;
        //// draw ring, avoids flickering when turnig knob
        SelectObject(hdc,hbrGrey);
        SelectObject(hdc,hpnGrey);
        hring = CreateRectRgn(0,0,0,0);
        hE1 = CreateEllipticRgn(p->rcKnob.left,p->rcKnob.top,
                                p->rcKnob.right+1,p->rcKnob.bottom+1);
        hE2 = CreateEllipticRgn(p->ptCenter.x-3,p->ptCenter.y-3,
                                p->ptCenter.x+3,p->ptCenter.y+3);
        CombineRgn(hring,hE1,hE2,RGN_DIFF);
        PaintRgn(hdc,hring);
        //// draw focus dot
        DeleteObject(hring); DeleteObject(hE1); DeleteObject(hE2);
        SelectObject(hdc,hw==GetFocus() ? hpnBlack : hpnGrey);
        SelectObject(hdc,hw==GetFocus() ? hbrRed : hbrGrey);
        Ellipse(hdc,
           p->ptCenter.x-4,p->ptCenter.y-4,p->ptCenter.x+4,p->ptCenter.y+4);
       }
       {//// repaint interior of knob (pointer line)
        POINT ptStop, pnt;
        double dftY;
        double dftAngle;
        SelectObject(hdc,p->hrgnKnob);
        if ((p->fbStatus&(SF_TRACK|SF_VALPOINTER))==(SF_TRACK|SF_VALPOINTER))
         {// use temporary line angle during tracking
          dftAngle=p->dftPointerAngle;
         }
        else
         {// calculate angle from current (set) position
          dftAngle=PI*(3.0/2.0*(double)(p->nSelValue-p->nMinVal)/
                      (double)(p->nMaxVal-p->nMinVal)
                      -0.75
                     );
         } /* if */
        p->dftRadius1 *= 0.8;
        dftY=p->dftRadius1*cos(dftAngle);
        // calcuate end of scale line from <dftAngle>
        ptStop.x=
          (int)sqrt(fabs
            (p->dftRadius1*p->dftRadius1-dftY*dftY))*(dftAngle<0.0?-1:+1);
        ptStop.y=
          -MulDiv((int)dftY,cyScreenAspect,cxScreenAspect);
        p->dftRadius1 /= 0.8;
        SelectObject(hdc,hpnBlack);
        MoveToEx(hdc,p->ptCenter.x+ptStop.x/2,p->ptCenter.y+ptStop.y/2,&pnt);
        LineTo(hdc,p->ptCenter.x+ptStop.x,p->ptCenter.y+ptStop.y);
       } /* block */
      // end of drawing
      EndPaint(hw,&pnt); DestroyDrawingTools();
      goto RETURN; // return 0
     } /* case */

    case WM_LBUTTONDOWN:
     {//// control knob via mouse
      int cx=(int)P2LO-p->ptCenter.x;
      int cy=MulDiv(p->ptCenter.y-(int)P2HI,cxScreenAspect,cyScreenAspect);
      SetFocus(hw); // focus to knob
      if (PtInRegion(p->hrgnKnob,P2LO,P2HI))
       {//// position inside knob: start knob tracking
        p->fbStatus|=SF_TRACK; p->fbStatus&=~SF_VALPOINTER; // set flags
        p->nStartValue=p->nSelValue; // set tracking start value
        SetCapture(hw); // capture mouse input
        p->dftPointerAngle=atan2((double)cx,(double)cy);
        p->fbStatus|=SF_VALPOINTER; // set flag "valid line"
        // set tracking start position of line
        SetNewValue(p,hw,p->dftPointerAngle);
       }
      else
       {//// position outside knob: new knob setting if within scale
        double dftDelta=sqrt(fabs((double)((LONG)cx*(LONG)cx+
                                           (LONG)cy*(LONG)cy)));
        double dftAngle;
        // check position, out-of-scale => exit
        if (dftDelta<p->dftRadius1||dftDelta>p->dftRadius2) break;
        // determine angle from mouse position
        dftAngle=atan2((double)cx,(double)cy);
        // calculate new position from <dftAngle>, set to parent if valid
        SetNewValue(p,hw,dftAngle);
       } /* if */
      break;
     } /* case */

    case WM_MOUSEMOVE:
    case WM_LBUTTONUP:
      if (!(p->fbStatus&SF_TRACK)) break; // only valid during tracking
      if (iMsg==WM_LBUTTONUP)
       {// end of tracking mode: send last position
        p->fbStatus&=~SF_TRACK; // end of tracking
        ReleaseCapture(); // end of mouse capturing
       } /* if */
      //// during tracking: analyse position of mouse cursor
      if (PtInRegion(p->hrgnKnob,P2LO,P2HI))
       {// mouse position within knob: determine new angle of line
        int cx=(int)P2LO-p->ptCenter.x;
        int cy=MulDiv(p->ptCenter.y-(int)P2HI,cxScreenAspect,cyScreenAspect);
        double dftAngle=atan2((double)cx,(double)cy);
        p->fbStatus|=SF_VALPOINTER; // set flag "valid line"
        // determine validy of new position, track it if valid
        SetNewValue(p,hw,dftAngle);
       } /* if */
      else
       {// outside knob: reset position before tracking
        p->fbStatus&=~SF_VALPOINTER; // reset flag "valid line"
        SetNewValue(p,hw,0);
       } /* if */
      goto RETURN; // return 0

    case WM_KEYDOWN:
      //// handle knob via keyboard
      if (GetWindowLong(hw,GWL_STYLE)&RKS_DISCRET)
       {// convert keyboard input to new position and send as command
        int nNewValue=p->nSelValue;
        switch (uP1)
         {case VK_DOWN:
          case VK_LEFT:
            // set to previous command
            if (nNewValue>p->nMinVal) nNewValue--;
            break;
          case VK_UP:
          case VK_RIGHT:
            // set to next command
            if (nNewValue<p->nMaxVal) nNewValue++;
            break;
          case VK_HOME:
            // set to first command
            nNewValue=p->nMinVal;
            break;
          case VK_END:
            // set to last command
            nNewValue=p->nMaxVal;
            break;
         } /* switch */
        if (nNewValue!=p->nSelValue)
         {// update window, send new position as command message to parent
          if (GetWindowLong(hw,GWL_STYLE)&RKS_AUTOSELECT)
           {// set new value, update screen
            p->nSelValue=nNewValue;
            InvalidateRect(hw,&p->rcKnob,FALSE); UpdateWindow(hw);
           } /* if */
          // send new value to parent window
#ifdef WIN32
          SendMessage(GetParent(hw),
           WM_COMMAND,MAKELONG(GetDlgCtrlID(hw),nNewValue),(LPARAM)hw);
#else
          SendMessage(GetParent(hw),
           WM_COMMAND,GetDlgCtrlID(hw),MAKELONG(hw,nNewValue));
#endif
         } /* if */
       }
      else
       {// convert keyboard input to scroll command and send as notification
        int iCmd;
        switch (uP1)
         {case VK_LEFT:
          case VK_DOWN:
            iCmd=SB_LINEDOWN; break;
          case VK_RIGHT:
          case VK_UP:
            iCmd=SB_LINEUP; break;
          case VK_NEXT:
            iCmd=SB_PAGEDOWN; break;
          case VK_PRIOR:
            iCmd=SB_PAGEUP; break;
          case VK_HOME:
            iCmd=SB_TOP; break;
          case VK_END:
            iCmd=SB_BOTTOM; break;
          default:
            goto DEFPROC; // not consumed
         } /* switch */
#ifdef WIN32
        SendMessage(GetParent(hw),WMP_VSCROLL,MAKELONG(iCmd,0),(LPARAM)hw);
#else
        SendMessage(GetParent(hw),WMP_VSCROLL,iCmd,MAKELONG(0,hw));
#endif
       } /* if */
      goto RETURN; // return 0, key is consumed

    case RKM_SETRANGE:
      //// set new value in new range
#ifdef WIN32
      p->nMinVal=(short)P2LO; p->nMaxVal=(short)P2HI;
#else
      p->nMinVal=P2LO; p->nMaxVal=P2HI;
#endif
      p->nSelValue=uP1;
      // repaint full window
      InvalidateRect(hw,NULL,TRUE); UpdateWindow(hw);
      goto RETURN; // return 0

    case RKM_GETRANGE:
      //// return current selected range
#ifdef WIN32
      lRet=MAKELONG((short)p->nMinVal,(short)p->nMaxVal);
#else
      lRet=MAKELONG(p->nMinVal,p->nMaxVal);
#endif
      goto RETURN;

    case RKM_SETPOS:
      //// set new knob position
      p->nSelValue=uP1;
      // repaint knob rectangle
      InvalidateRect(hw,&p->rcKnob,FALSE); UpdateWindow(hw);
      goto RETURN; // return 0

    case RKM_GETPOS:
      //// return current knob position
      lRet=p->nSelValue;
      goto RETURN;

    case WM_NCDESTROY:
      //// free allocated local memory for window data
      if (p->hrgnKnob)
       {// destroy elliptic knob region
        DeleteObject(p->hrgnKnob); p->hrgnKnob=0;
       } /* if */
      LocalUnlock(hData); LocalFree(hData); hData=0;
      break; // execute default action
   } /* switch */

  DEFPROC:
    lRet=DefWindowProc(hw,iMsg,uP1,lP2);

  //// return from function: unlock window data memory block

  RETURN:
    if (hData) LocalUnlock(hData);
    return lRet;
} /* RotKnobWndFn() */


/* ====================================
   ROTKNOB class library: End of MAIN.C
   ====================================
*/
