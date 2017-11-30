////////////////////////////////////////////////////////////////////////////////
//////////////////// Implementation of class MetafileDevice ////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

MetafileDevice :: MetafileDevice(char *filename, int w, int h)
                : DisplayDevice(IDO_METADEV)
{
    hdcmf = CreateMetaFile(filename);
    RECT rc = {0,0,w,h};
    SetExtents(rc);
}

MetafileDevice :: ~MetafileDevice(VOID)
{
}

void MetafileDevice :: SpecialIni(int xmin, int xmax, int ymin, int ymax)
{
    SetMapMode(hdcmf, MM_ANISOTROPIC);
#ifdef WIN32
    SetWindowExtEx(hdcmf, (xmax-xmin), (ymax-ymin), NULL);
    SetWindowOrgEx(hdcmf, xmin, ymin, NULL);
#else
    SetWindowExt(hdcmf, (xmax-xmin), (ymax-ymin));
    SetWindowOrg(hdcmf, xmin, ymin);
#endif
}

HMETAFILE MetafileDevice :: Close(VOID)
{
    HMETAFILE h;

    h = CloseMetaFile(hdcmf);
    return h;
}


void MetafileDevice :: WriteClipboard(HWND hWnd, HMETAFILE hMF, int x, int y)
{
     HGLOBAL        hData;
     LPMETAFILEPICT lpMFP;

     hData = GlobalAlloc(GHND, sizeof(METAFILEPICT));
     lpMFP = (LPMETAFILEPICT) GlobalLock(hData);
#ifdef WIN32
     lpMFP->mm   = MM_ANISOTROPIC;
     lpMFP->xExt = x * 50;
     lpMFP->yExt = y * 50;
#else
     lpMFP->mm   = MM_ISOTROPIC;
     lpMFP->xExt = x;
     lpMFP->yExt = y;
#endif
     lpMFP->hMF  = hMF;
     GlobalUnlock(hData);

     OpenClipboard(hWnd);
     EmptyClipboard();
     SetClipboardData(CF_METAFILEPICT, hData);
     CloseClipboard();
     
     MessageBeep (0) ;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////// Implementation of class MemoryDevice //////////////////////
////////////////////////////////////////////////////////////////////////////////

MemoryDevice :: MemoryDevice(int BMWidth, int BMHeight)
              : DisplayDevice(IDO_MEMORYDEV)

{
   hdc      = CreateDC("DISPLAY",NULL,NULL,NULL);
   hdcMem   = CreateCompatibleDC(hdc);
   hBitmap  = CreateCompatibleBitmap(hdc,BMWidth,BMHeight);
   SelectObject(hdcMem,hBitmap);
}

MemoryDevice :: ~MemoryDevice(VOID)
{
   DeleteDC(hdcMem);
   DeleteObject(hBitmap);
   DeleteDC(hdc);
}

//////////////////////////////////// EOF ///////////////////////////////////////


