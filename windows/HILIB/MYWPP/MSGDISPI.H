///////////////////////////////////////////////////////////
//                                                       //
// MsgDisp - Message Text Display Interface              //
// Copyright (c) Ivan Gerencir, Aug 1993                 //
// Language: Borland C++ 3.1 & Application Frameworks    //
// OS: Windows 3.1                                       //
//                                                       //
///////////////////////////////////////////////////////////

// File: msgdispi.h; Listing 7

#ifdef WIN32
#ifdef __BORLANDC__
#define BCNT
#endif
#endif

#ifndef _MSGDISPI_H_
#define _MSGDISPI_H_

/////////////////////////////////////////////////////////////////////
///////////////// Message Display Interface routines ////////////////
/////////////////////////////////////////////////////////////////////

extern "C"
{
#ifdef BCNT
  void APIENTRY MsgDispS(char FAR *Text);
#define MsgDispx MsgDispS
#elif WIN32
  void APIENTRY MsgDispB(char FAR *Text,...);
#define MsgDisp MsgDispB
#elif __SC__
  void APIENTRY MsgDispB(char FAR *Text,...);
#define MsgDisp MsgDispB
#elif __BORLANDC__
  void APIENTRY MsgDispB(char FAR *Text,...);
#define MsgDisp MsgDispB
#else
  void FAR CDECL MsgDispM(char FAR *Text,...);  // Visual C++ (16-bit)
#define MsgDisp MsgDispM
#endif
}

#endif

// End of File
