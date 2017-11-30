/*
 * ROTKNOB.H --- Header File of MS-Windows Dialog Class "RotationKnob"
 *
 * Copyright 1988, 1989, 1990, 1991, 1992 by Marcellus Buchheit, Germany
 * Internet mabu@buchheit.de, CompuServe 100020,1723, Fax +49(0)721/356238
 *
 * Release 1.00 of 91-Dec-30 --- All rights reserved.
 */

//// window styles

#define RKS_SCALEMASK  0x000FL
#define RKS_DISCRET    0x8000L
#define RKS_AUTOSELECT 0x4000L

//// control messages

#define WMP_VSCROLL  (WM_USER+10)
#define RKM_SETRANGE (WM_USER+0)
#define RKM_GETRANGE (WM_USER+1)
#define RKM_SETPOS   (WM_USER+2)
#define RKM_GETPOS   (WM_USER+3)

