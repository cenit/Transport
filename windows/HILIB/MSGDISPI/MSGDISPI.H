///////////////////////////////////////////////////////////
//                                                       //
// MsgDisp - Message Text Display Interface              //
// Copyright (c) Ivan Gerencir, Aug 1993                 //
//                                                       //
///////////////////////////////////////////////////////////

#ifndef WIN32
#include "ntdefs.h"
#endif

#ifdef WIN32
#ifdef __BORLANDC__
#define BCNT
#endif
#endif

#ifndef _MSGDISPI_H_
#define _MSGDISPI_H_

#define APPNAME "DispMsg"

// Used for additional filtering or received messages

#define MAGIC_ID 0xFEDC

// Maximum message string length

#define MAXMSGSTRLEN 256

#ifdef BCNT
void APIENTRY MsgDispS(char FAR *cpBuf);
#elif WIN32
void APIENTRY MsgDispB(char FAR *cpFrmt,...);
#elif __SC__
void APIENTRY MsgDispB(char FAR *cpFrmt,...);
#elif __BORLANDC__
void APIENTRY MsgDispB(char FAR *cpFrmt,...);
#else
void FAR CDECL MsgDispM(char FAR *cpFrmt,...); // Visual C++ (16 bits)
#endif

#endif

// End of File
