/*
 * ANAMTR.H --- Header File of MS-Windows Dialog Class "AnalogMeter"
 *
 * Copyright 1988, 1989, 1990, 1991, 1992 by Marcellus Buchheit, Germany
 * Internet mabu@buchheit.de, CompuServe 100020,1723, Fax +49(0)721/356238
 *
 * Release 1.00 of 91-Dec-30 --- All rights reserved.
 */

#ifndef WIN32
#include "ntdefs.h"
#endif

//// window styles
#define AMS_LINEMASK 0x007FL
#define AMS_DIGITAL  0x8000L

#ifndef RC_INVOKED

//// control messages
#define AMM_SETVALUE (WM_USER)
#define AMM_SETRANGE (WM_USER+1)
#define AMM_SETSIZE  (WM_USER+2)
#define AMM_GETRANGE (WM_USER+3)
#define AMM_GETSIZE  (WM_USER+4)

//// notification messages
// (none)

//// =====================================================
//// additional information for the static linking version
//// =====================================================

// function for registering the class name
BOOL APIENTRY RegisterAnaMtr(HANDLE hi,const LPSTR pszClassName);

//
// following functions must be exported in the DEF file of the application:
//   AnaMtrWndFn
//

#endif
