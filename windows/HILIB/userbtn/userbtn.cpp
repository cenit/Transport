////////////////////////////////////////////////////////////////////////////////
/////////////////////// DLL which Draws UserButtons ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#ifndef WIN32
#include "ntdefs.h"
#endif
#include "userbtn.hpp"

HINSTANCE hib;

void APIENTRY DrawBitmap(HBRUSH hBrushB, LPDRAWITEMSTRUCT lpdis)
{
/*
    Sources: M. Buchheit: Windows Programmierbuch, p.263
             SDK 3.1: Programmers's Reference, Volume 3, p. 122

    Instead of bitmaps ICONS are drawn, because the body of icons
    can be made transparent. So the buttons have always the basic
    color COLOR_BTNFACE.
*/
    HICON   hIcon;
    UINT    offset;
    HRGN    hRgn;
    HBRUSH  hBrush;
    RECT    rc;

    switch (lpdis->itemAction)
    {
       case ODA_DRAWENTIRE: 
         offset = (lpdis->itemState & ODS_FOCUS) ? 5000 : 1000;
         break;
       case ODA_SELECT:
         offset = (lpdis->itemState & ODS_SELECTED) ? 3000 : 1000;
         break;
       case ODA_FOCUS:
         offset = 5000;
         break;
       default: 
         offset = 0;
         break;
    }

#ifdef WIN32
    if ((hIcon = (HICON)LoadImage(hib,MAKEINTRESOURCE(offset+lpdis->CtlID),
				  IMAGE_ICON,64,64,0)) != 0)
#else
    if ((hIcon = LoadIcon(hib,MAKEINTRESOURCE(offset+lpdis->CtlID))) != 0)
#endif
    {
       FillRect(lpdis->hDC,&lpdis->rcItem,hBrushB);
       rc.top    = 0;
       rc.left   = 0;
       if (lpdis->CtlID == 999)
       {
         rc.right  = 64;
         rc.bottom = 32;
       }
       else
       {
         rc.right  = 63;
         rc.bottom = 39;
       }
       hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
       FillRect(lpdis->hDC,&rc,hBrush);
       DeleteObject(hBrush);

       hRgn = CreateRectRgn(0,0,rc.right,rc.bottom);
       SelectClipRgn(lpdis->hDC,hRgn);
#ifdef WIN32
       DrawIconEx(lpdis->hDC,lpdis->rcItem.left,lpdis->rcItem.top,hIcon,
                  64,64,0,NULL,DI_NORMAL);
#else
       DrawIcon(lpdis->hDC,lpdis->rcItem.left,lpdis->rcItem.top,hIcon);
#endif
       DeleteObject(hRgn);
    }
}

#ifdef WIN32
#ifdef __BORLANDC__
#pragma argsused

BOOL WINAPI DllEntryPoint( HINSTANCE hInst,
                           DWORD ul_reason_being_called, LPVOID plvReserved)
{
    hib = hInst;
    return 1;
}

#else

INT  APIENTRY LibMain(HINSTANCE hInst, 
                      DWORD ul_reason_being_called, LPVOID lpReserved)
{
    hib = hInst;

    return 1;
	UNREFERENCED_PARAMETER(hInst);
	UNREFERENCED_PARAMETER(ul_reason_being_called);
	UNREFERENCED_PARAMETER(lpReserved);
}
#endif
#else

#ifdef __BORLANDC__
#pragma argsused
#endif

int FAR PASCAL LibMain( HANDLE hInstance, UINT wDataSegment,
                                          UINT wHeapSize, LPSTR lpszCmdLine )
{
    if ( wHeapSize != 0 )
        UnlockData( 0 );

    hib = hInstance;

    return 1;
}

#ifdef __BORLANDC__
#pragma argsused
#endif

int FAR PASCAL WEP ( int bSystemExit )
{
    return 1;
}

#endif
///////////////////////////////////// EOF /////////////////////////////////////

