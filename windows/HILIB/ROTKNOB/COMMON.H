/*****************************************************************************

 ----  R O T K N O B ------- MS-Windows Class Library -----------------------
                             Header COMMON.H: Private Common Declarations

==============================================================================

This MS-Windows library contains a dialog control class which provides a
rotation knob to enter analog values via keyboard or mouse or to select
between discrete positions.

------------------------------------------------------------------------------

Copyright 1988, 1989, 1990, 1991, 1992 by Marcellus Buchheit, Germany
Internet mabu@buchheit.de, CompuServe 100020,1723, Fax +49(0)721/356238

Release 1.00 of 91-Dec-30 --- All rights reserved.

*****************************************************************************/
 
// class default values
#define CLASS_VERSION 256*1+0
#define CLASS_NAME  "RotationKnob"
#define CLASS_WNDSTYLE (WS_CHILD|WS_VISIBLE|WS_TABSTOP)

// set master control variable
#define _WINDLL

// define/undefine non-debugging option name
#define NDEBUG
// #define _WINDOWS
#include <WINDOWS.H>

// resource declarations
#define BMP_TLBOX_0     100
#define BMP_TLBOX_1     101
#define BMP_TLBOX_2     102
// (further up to BMP_TLBOX_11 if needed)
#define CRS_DRGDRP_0    100
#define CRS_DRGDRP_1    101
#define CRS_DRGDRP_2    102
// (further up to CRS_DRGDRP_11 if needed)

#define DB_STYLE        800

#define IDNONE           0
#define IDNONE2          3
#define IDNONE3          4
#define IDNONE4          5
#define IDNONE5          6
#define IDCLASS         3000
#define IDIDVAL         3001
#define IDTITLE         3002
#define IDDISCRET       3010
#define IDSCLTEXT       3011
#define IDHSCLLINE      3012
#define IDSCLLINE       3013
#define IDAUTOSEL       3014

#define ST_TITLE        5000
#define ST_DESCR_0      5001
#define ST_DESCR_1      5002
#define ST_DESCR_2      5003
// (up to ST_DECSCR_11 if needed)

#define MSG_GETINF      6000
#define MSG_SETINF      6001
#define MSG_NUMVAL      6002
#define MSG_LINERANGE   6003
#define MSG_TITLESTRING 6004
#define MSG_SCALESTRING 6005

#ifndef RC_INVOKED
////
//// the following parts are only loaded for the C segments
////

#include <CUSTCNTL.H>
#include "ROTKNOB.H"

// ANSI-C headers
#include <STDLIB.H>
#include <STDIO.H>
#include <MATH.H>

// parameter & conversation macros
#define P2LO LOWORD(lP2)
#define P2HI HIWORD(lP2)
#define MAKEP(sel,off) ((void far*)MAKELONG(off,sel))
// #define SELECTOROF(p) (HIWORD((LONG)(p)))
// #define OFFSETOF(p) (LOWORD((LONG)(p)))
#define LONGADDR(p) (LONG)(LPSTR)(p)
#define NEARADDR(p) ((BYTE NEAR*)(LONG)(p))

// default class style definition
#ifdef _WINDLL
#define CS_STDCLASS (CS_GLOBALCLASS|CS_DBLCLKS)
#else
#define CS_STDCLASS 0
#endif

/* ===========================================================================
   CLASS_DATA                             complete global data of dialog class
   ---------------------------------------------------------------------------
   hi ....... contains the instance of the library (DLL) or application (SLL).
   ===========================================================================
*/
typedef struct
 {HANDLE hi;
 }
CLASS_DATA;

// global variable of class
extern CLASS_DATA rkc;

// global functions (between modules)
VOID APIENTRY RkcInit(VOID);

// standard entry functions
LONG APIENTRY RotKnobWndFn(HWND hw, UINT iMsg, WPARAM uP1, LPARAM lP2);

#endif /* ndef RC_INVOKED */
