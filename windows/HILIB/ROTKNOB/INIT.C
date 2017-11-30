/*****************************************************************************

 ----  R O T K N O B ------- MS-Windows Class Library -----------------------
                             Module INIT: Initialization

==============================================================================

This MS-Windows library contains a dialog control class which provides a
rotation knob to enter analog values via keyboard or mouse or to select
between discrete positions.

------------------------------------------------------------------------------

This module contains the initialization and registering of the window class.
The function is called when the DLL is loaded the first time or if in the SLL
the exported register function is called.

------------------------------------------------------------------------------

Copyright 1988, 1989, 1990, 1991, 1992 by Marcellus Buchheit, Germany
Internet mabu@buchheit.de, CompuServe 100020,1723, Fax +49(0)721/356238

Release 1.00 of 91-Dec-30 --- All rights reserved.

*****************************************************************************/
 
// include all common declarations
#include "COMMON.H"

// sole global variable of class
CLASS_DATA rkc;


/*****************************************************************************
R e g i s t e r R o t K n o b
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

BOOL APIENTRY RegisterRotKnob(HANDLE hi, const LPSTR pszClassName)
{
  WNDCLASS wc;

  if (rkc.hi) return TRUE; // class already registered

// set window class structure

  wc.lpszClassName = pszClassName[0] != 0 ? pszClassName
                                          : (LPSTR)CLASS_NAME;
  wc.hInstance     = hi;
  wc.lpfnWndProc   = (WNDPROC)RotKnobWndFn;
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

  rkc.hi=hi;
  RkcInit();
  return TRUE;
}

#ifdef _WINDLL
#ifdef WIN32

#ifdef __BORLANDC__
#pragma argsused

BOOL WINAPI DllEntryPoint( HINSTANCE hinstDll,
                           DWORD fdwRreason, LPVOID plvReserved)
{
   RegisterRotKnob(hinstDll,"");
   return 1;
}

#else

INT  APIENTRY LibMain(HANDLE hiLib, 
                      DWORD ul_reason_being_called, LPVOID lpReserved)
{
   return RegisterRotKnob(hiLib,"");
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
  return RegisterRotKnob(hiLib,"");
}

#endif
#endif
