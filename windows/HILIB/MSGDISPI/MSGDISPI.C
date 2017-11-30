///////////////////////////////////////////////////////////
//                                                       //
// MsgDispI - Message Text Display Interface             //
// Copyright (c) Ivan Gerencir, Aug 1993                 //
//                                                       //
///////////////////////////////////////////////////////////


#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include "msgdispi.h"

UINT MsgDispMessage = 0;

#define FAIL    0
#define SUCCESS 1
#define STREND '\0'

#ifdef __BORLANDC__
#pragma argsused
#endif

#ifndef WIN32

int FAR PASCAL LibMain(HANDLE hInstance, UINT wDataSegment,
                                         UINT wHeapSize, LPSTR lpstCmdLine)
{
    if(MsgDispMessage == 0)
    {
//      Initialize DLL by registering message for communicating with MsgDisp

        if((MsgDispMessage = RegisterWindowMessage(APPNAME)) == 0)
        {
            MessageBox(GetActiveWindow(),
                       "Unable to register Windows Message",
                       "Error in " APPNAME " DLL initialization",
                       MB_OK);
            return(FAIL);
        }
    }
    return(SUCCESS);
}

#else
#ifdef __BORLANDC__
#pragma argsused

BOOL WINAPI DllEntryPoint( HINSTANCE hInst,
                           DWORD ul_reason_being_called, LPVOID plvReserved)
{
    if(MsgDispMessage == 0)
    {
//      Initialize DLL by registering message for communicating with MsgDisp

        if((MsgDispMessage = RegisterWindowMessage(APPNAME)) == 0)
        {
            MessageBox(GetActiveWindow(),
                       "Unable to register Windows Message",
                       "Error in " APPNAME " DLL initialization",
                       MB_OK);
            return(FAIL);
        }
    }
    return 1;
}

#else

INT  APIENTRY LibMain(HANDLE hInst, 
                      DWORD ul_reason_being_called, LPVOID lpReserved)
{
    if(MsgDispMessage == 0)
    {
//      Initialize DLL by registering message for communicating with MsgDisp

        if((MsgDispMessage = RegisterWindowMessage(APPNAME)) == 0)
        {
            MessageBox(GetActiveWindow(),
                       "Unable to register Windows Message",
                       "Error in " APPNAME " DLL initialization",
                       MB_OK);
            return(FAIL);
        }
    }
    return(SUCCESS);
	UNREFERENCED_PARAMETER(hInst);
	UNREFERENCED_PARAMETER(ul_reason_being_called);
	UNREFERENCED_PARAMETER(lpReserved);
}
#endif
#endif

#ifdef BCNT
void APIENTRY MsgDispS(char FAR *cpBuf) // no exportable variable argument list 
#elif WIN32
void APIENTRY MsgDispB(char FAR *cpFrmt,...)
#elif __SC__
void APIENTRY MsgDispB(char FAR *cpFrmt,...)
#elif __BORLANDC__
void APIENTRY MsgDispB(char FAR *cpFrmt,...)
#else
void FAR CDECL MsgDispM(char FAR *cpFrmt,...) // Visual C++ (16 bits)
#endif
{
    ATOM Atom;
    char caBuf[2*MAXMSGSTRLEN];

#ifndef BCNT
    va_list marker;
    va_start(marker,cpFrmt);
    wvsprintf(caBuf,cpFrmt,marker);
#else
    strcpy(caBuf,cpBuf);
#endif

//  Check received string length to maximum allowed

    if(strlen(caBuf) >= MAXMSGSTRLEN)
    {
        MessageBox(GetActiveWindow(),
                   caBuf,
                   "Error in " APPNAME " interface - string too long",
                   MB_OK);
	caBuf[MAXMSGSTRLEN - 1] = STREND;
    }
    
//  Add string to global atom table

    Atom = GlobalAddAtom(caBuf);
    if(Atom == 0)
    {
        MessageBox(GetActiveWindow(),
                   "Error in GlobalAddAtom",
                   APPNAME " DLL MsgDisp",
                   MB_OK);
        return;
    }

//  Send message containing handle of atom.
//  SendMessage does not return until the message is processed

    SendMessage(HWND_BROADCAST, MsgDispMessage, MAGIC_ID, Atom);

//  Now we must delete atom after we are done with it

    GlobalDeleteAtom(Atom);
}

// End of File
