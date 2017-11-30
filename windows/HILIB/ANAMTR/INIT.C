/*****************************************************************************

 ----  A N A M T R  -------- MS-Windows Class Library -----------------------
                             Module INIT: Initialization

==============================================================================

This MS-Windows library contains a dialog control class which provides an
analog meter to display numerical values.

------------------------------------------------------------------------------

This module contains the initialization and registering of the window class.
The function is called when the DLL is loaded at first time or if in the SLL
the exported register function is called.

------------------------------------------------------------------------------

Copyright 1988, 1989, 1990, 1991, 1992 by Marcellus Buchheit, Germany
Internet mabu@buchheit.de, CompuServe 100020,1723, Fax +49(0)721/356238

Release 1.00 of 91-Dec-30 --- All rights reserved.

*****************************************************************************/
 
// include all common declarations
#include "COMMON.H"

// sole global variable of class
CLASS_DATA amc;


/*****************************************************************************
R e g i s t e r A n a M t r
==============================================================================

### SLL entry function ###

This function registers the window class of the dialog control.

Parameters
 hi .......... is the handle of the module instance which possesses the
               window class.
 pszClassName  is the class name of the control class. If the string is
               empty, the default name in CLASS_NAME is used.

Return
 TRUE is returned if the window class was registered and FALSE if not.

*****************************************************************************/

BOOL APIENTRY RegisterAnaMtr(HANDLE hi, const LPSTR pszClassName)
{
  WNDCLASS wc;

  if (amc.hi) return TRUE; // class already registered

// set window class structure

  wc.lpszClassName = pszClassName[0] != 0 ? pszClassName
                                          : (LPSTR)CLASS_NAME;
  wc.hInstance     = hi;
  wc.lpfnWndProc   = (WNDPROC)AnaMtrWndFn;
  wc.style         = CS_STDCLASS|CS_HREDRAW|CS_VREDRAW;
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wc.hCursor       = LoadCursor(0,IDC_ARROW);
  wc.hIcon         = 0;
  wc.lpszMenuName  = NULL;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = sizeof(HANDLE);
 
// register window, return status

  if (!RegisterClass(&wc)) return FALSE; // bad operation

// registering was successful: set instance, init dialog control

  amc.hi=hi;
  AmcInit();
  return TRUE;
}

#ifdef _WINDLL
#ifdef WIN32

#ifdef __BORLANDC__
#pragma argsused

BOOL WINAPI DllEntryPoint( HINSTANCE hinstDll,
                           DWORD fdwRreason, LPVOID plvReserved)
{
   RegisterAnaMtr(hinstDll,"");
   return 1;
}

#else

INT  APIENTRY LibMain(HANDLE hiLib, 
                      DWORD ul_reason_being_called, LPVOID lpReserved)
{
   return RegisterAnaMtr(hiLib,"");
	UNREFERENCED_PARAMETER(ul_reason_being_called);
	UNREFERENCED_PARAMETER(lpReserved);
}
#endif

#else

#ifdef __BORLANDC__
#pragma argsused
#endif

int FAR PASCAL WEP(int idExit)
{
   return 1; // nothing to do here
}

#ifdef __BORLANDC__
#pragma argsused
#endif

int FAR PASCAL LibMain(HANDLE hiLib, UINT usDS, UINT cbHeap, LPSTR ulReserved)
{
  return RegisterAnaMtr(hiLib,"");
}
#endif
#endif
