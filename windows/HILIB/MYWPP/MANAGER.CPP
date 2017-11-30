#include "allincs.h"

int GetScreenWidth(void)
{
    return GetSystemMetrics(SM_CXSCREEN);
}
int GetScreenHeight(void)
{
    return GetSystemMetrics(SM_CYSCREEN);
}

/*

// does not work under Windows 95 !!!!!!!!!!!

#ifndef WIN32
#ifndef __BORLANDC__
#ifndef __SC__

int AllowAnotherInstance(HINSTANCE hInstance)
{ 

//  Function that modifies a module table to trick the loader
//  into letting a second instance of a multiple data segment program run.
//  By Matt Pietrek, MS-System Journal, Jan./Feb. 1994

    HMODULE hModuleSel;
    LPSTR moduleName, fileName;
    BYTE cbModuleName;
    
    hModuleSel = SELECTOROF(    // Convert the HINSTANCE to an HMODULE
        GlobalLock(GetModuleHandle((LPSTR)MAKELP(0,hInstance))));

    if ( hModuleSel == 0 )      // Make sure we succeeded.
        return 0;

    // Make pointers to the resident names table and the OFSTRUCT
    moduleName = (LPSTR)MAKELP( hModuleSel, *(WORD FAR *)MAKELP(hModuleSel, 0x26));
    fileName = (LPSTR)MAKELP( hModuleSel, *(WORD FAR *)MAKELP(hModuleSel, 0x0A));

    // Get the module name length, and advance to the actual string
    cbModuleName = *moduleName++;   // First byte is a length byte
    
    // Convert the first uppercase letter of the modulename to lowercase
    while ( cbModuleName )
    {
        if ( isupper(*moduleName) )
        {
            *moduleName = tolower(*moduleName); break;
        }
        cbModuleName--; moduleName++;
    }
    
    if ( cbModuleName == 0 )    // Make sure we succeeded
        return 0;
    
    // Position to the end of the filename. First byte is a length byte
    fileName += *fileName - 1;

    // Find the first uppercase letter in the filename. Convert to lowercase
    while ( TRUE )
    {
        // Stop when we come to a directory separator or colon
        if ( (*fileName=='\\') || (*fileName=='/') || (*fileName==':') )
            return 0;
        
        if ( isupper(*fileName) )
        {
            *fileName = tolower(*fileName); break;
        }
        fileName--;
    }   
    return 1;
}
#endif
#endif
#endif
*/

///////////////////////////////////////////////////////////////////////////////
///////////////////// implementation of class Object //////////////////////////
///////////////////////////////////////////////////////////////////////////////

Object :: Object(UINT id)
{
  Id = id;
}

Object :: ~Object(void)
{
}

VOID Object :: DispObjectMsg(LPCSTR cpFrmt,...)
{
    char caBuf[1024], caTitle[50];

#ifdef __WATCOMC__    // replacing bad macros with those from Visual C/C++
    typedef char *	va_listx;
#define _INTSIZEOF(n)	( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) ) 
#define va_startx(ap,v)	( ap = (va_listx)&v + _INTSIZEOF(v) )
    va_listx marker;
    va_startx(marker,cpFrmt);
    wvsprintf(caBuf,cpFrmt,(char **)marker);
#else
    va_list marker;
    va_start(marker,cpFrmt);
    wvsprintf(caBuf,cpFrmt,marker);
#endif

    switch (Id)
    {  case IDO_APPLICATION: strcpy(caTitle,"application"); break;
       case IDO_WINDOW: strcpy(caTitle,"window"); break;
       case IDO_METADEV: strcpy(caTitle,"metadev"); break;
       case IDO_DIALOG: strcpy(caTitle,"dialog"); break;
       case IDO_STATIC_TEXT: strcpy(caTitle,"static text"); break;
       case IDO_STRING_EDIT: strcpy(caTitle,"string edit"); break;
       case IDO_PUSH_BUTTON: strcpy(caTitle,"pushbutton"); break;
       case IDO_LIST_BOX: strcpy(caTitle,"listbox"); break;
       case IDO_RADIO_BUTTON: strcpy(caTitle,"radiobutton"); break;
       case IDO_CHECK_BOX: strcpy(caTitle,"checkbox"); break;
       case IDO_SCROLL_BAR: strcpy(caTitle,"scrollbar"); break;
       case IDO_COMBO_BOX: strcpy(caTitle,"combobox"); break;
       case IDO_AMPERE_METER: strcpy(caTitle,"ampmeter"); break;
       case IDO_ROTKNOB: strcpy(caTitle,"rotknob"); break;
       case IDO_COLOR_BUTTON: strcpy(caTitle,"colorbutton"); break;
       case IDO_BRUSH: strcpy(caTitle,"brush"); break;
       case IDO_PEN: strcpy(caTitle,"pen"); break;
       case IDO_FONT: strcpy(caTitle,"font"); break;
       case IDO_PRINTER: strcpy(caTitle,"printer"); break;
       case IDO_MEMORY: strcpy(caTitle,"memory"); break;
       case IDO_MEMORYDEV: strcpy(caTitle,"memory device"); break;
       case IDO_MENU: strcpy(caTitle,"menu"); break;
       default: strcpy(caTitle,"unknown"); break;
    }
    MessageBox(NULL,caBuf,caTitle,MB_OK);
}

///////////////////////////////////////////////////////////////////////////////
/////////////////// implementation of class Application ///////////////////////
///////////////////////////////////////////////////////////////////////////////

Window *Application::opMain = NULL;
HINSTANCE Application::hInstance = NULL;
HINSTANCE Application::hPrevInstance = NULL;
HACCEL Application::hAccelTable = NULL;
HWND   Application::ghActiveDlg = NULL;

Application :: Application( HINSTANCE hInstance_
                          , HINSTANCE hPrevInstance_
                          , LPSTR lpszCmdLine_
                          , int nCmdShow_ ) : Object(IDO_APPLICATION)
{
/* fixed with BC 4.5
#ifdef WIN32
#ifdef __BORLANDC__
   char *p = strchr(lpszCmdLine_,' ');
   if (p)  // strip off first parameter == program name (bug in BC++ 32bit)
   {
      strcpy(lpszCmdLine,(char *)(p+1));      
      strcpy(lpszCmdLine_,lpszCmdLine);
   }   
#endif
#endif
*/
   strcpy(lpszCmdLine,lpszCmdLine_);
   hInstance = hInstance_;
   hPrevInstance = hPrevInstance_;
   nCmdShow = nCmdShow_;
   hInstance = hInstance_;
   hPrevInstance = hPrevInstance_;

   if (!hPrevInstance)
      Window::Register(IDR_APP_ICON, NULL );
   hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAIN_ACCEL));
}

Application :: ~Application(void)
{
   delete opMain;
   opMain = NULL;
}

UINT Application :: Start()
{
   MSG msg;

   while( GetMessage( &msg, NULL, 0, 0 ) )
   {
      if ( (ghActiveDlg == NULL) || !IsDialogMessage(ghActiveDlg,&msg) )
      {
         if (!TranslateAccelerator(msg.hwnd,hAccelTable,&msg))
         {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
         }
      }
   }
   return msg.wParam;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// WinMain ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int PASCAL WinMain( HINSTANCE hInstance
                  , HINSTANCE hPrevInstance
                  , LPSTR lpszCmdLine
                  , int nCmdShow )
{
// Usage of 3d-controls:
// For WIN32 each dialogbox is subclassed individually with Ctl3dSubclassDlg()
// via the message WM_INITDIALOG in files dialog.cpp (Handler()) and simpdlg.cpp
// (HookProc()). Otherwise WM_CTLCOLORxxxx messages do not get through and
// therefore the private brush will not be set. This seems to be a bug in
// CTL3D32.DLL. For CTL3DV2 (WIN16) this is not the case, because with
// Ctl3dAutoSubclass() everything works fine, private brushes are set via the
// messages WM_CTLCOLOR if desired.

#ifndef WIN95
    Ctl3dRegister(hInstance);
#endif
#ifndef WIN32
    Ctl3dAutoSubclass(hInstance);
#endif

    Application *opApp = new Application( hInstance
                                        , hPrevInstance
                                        , lpszCmdLine
                                        , nCmdShow );

/*
#ifndef WIN32
#ifndef __BORLANDC__
#ifndef __SC__
    AllowAnotherInstance(hInstance); // only for Visual C/C++ (16 bit)
#endif
#endif
#endif
*/

    int retval = opApp->Run();

#ifndef WIN95
    Ctl3dUnregister(hInstance);
#endif
 
    delete opApp;
    return retval;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////// Implementation of class Memory //////////////////////
///////////////////////////////////////////////////////////////////////////////

Memory :: Memory(UINT size) : Object(IDO_MEMORY)
{
    if ((hMem = LocalAlloc(LMEM_MOVEABLE, size)) != 0)
    {  pBuf = (PSTR)LocalLock(hMem);
       *pBuf = '\0';
    }
    Size = size;
    ReallocSize = 19 * (Size / 20);
    UnitSize = Size;
    BuffLen = 0;
}

Memory :: ~Memory(void)
{
}

void Memory :: FreeMemory(void)
{
    while (LocalFlags(hMem) & LMEM_LOCKCOUNT) // found with Bounds Checker
	LocalUnlock(hMem);
    LocalFree(hMem);
}

void Memory :: WriteLn(LPCSTR line)
{
    char crlf[3]; crlf[0] = 0x0d; crlf[1] = 0x0a; crlf[2] = '\0';

    if (BuffLen > ReallocSize)
    {  if (LocalReAlloc(hMem,Size+UnitSize,LMEM_MOVEABLE) != 0)
       {  Size += UnitSize;
          ReallocSize = 19 * (Size / 20);
       }
       else if (LocalReAlloc(hMem,Size+UnitSize/2,LMEM_MOVEABLE) != 0)
       {  Size += UnitSize/2;
          ReallocSize = 19 * (Size / 20);
       }
       else
          return;
    }
    if (hMem)
    {  pBuf = (PSTR)LocalLock(hMem);
       if (pBuf)
       {  pBuf += BuffLen;
          strcpy(pBuf,line);
          pBuf += strlen(line);
          strcat(pBuf,crlf);
          pBuf += 2;
          BuffLen += strlen(line) + 2;
       }
       LocalUnlock(hMem);
    }
}

HLOCAL Memory :: Reset(void)
{
   return (LocalReAlloc(hMem,Size,LMEM_MOVEABLE));
}

void Memory :: Clear(void)
{
   BuffLen = 0;   
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////// Implementation of class Menu ////////////////////////
////////////////////////////////////////////////////////////////////////////////

Menu :: Menu(int ResId, Window *opWin)
      : Object(IDO_MENU)
{
    StandAlone = (opWin == NULL) ? TRUE : FALSE;

    if (ResId)
    {
       hMenu = LoadMenu(Application::GetInstance(),MAKEINTRESOURCE(ResId));
       if (opWin)
          SetMenu(opWin->GetHandle(),hMenu);
    }
    else
       hMenu = GetSystemMenu(opWin->GetHandle(),FALSE);

}

Menu :: Menu(int ResId, Dialog *opDlg)
      : Object(IDO_MENU)
{
    StandAlone = (opDlg == NULL) ? TRUE : FALSE;

    if (ResId)
    {
       hMenu = LoadMenu(Application::GetInstance(),MAKEINTRESOURCE(ResId));
       if (opDlg)
          SetMenu(opDlg->GetHandle(),hMenu);
    }
    else
       hMenu = GetSystemMenu(opDlg->GetHandle(),FALSE);

}

Menu :: ~Menu( void )
{
    if (hMenu && StandAlone)
       DestroyMenu(hMenu);
}

void Menu :: SetCheck(UINT wID, BOOL blState)
{
    if (blState)
       CheckMenuItem(hMenu,wID,MF_CHECKED);
    else
       CheckMenuItem(hMenu,wID,MF_UNCHECKED);
}

void Menu :: Enable(UINT wID)
{
    EnableMenuItem(hMenu,wID,MF_ENABLED);
}

void Menu :: Disable(UINT wID)
{
    EnableMenuItem(hMenu,wID,MF_DISABLED | MF_GRAYED);
}

void Menu :: AppendSep(void)
{
    AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
}

void Menu :: Append(UINT wID, LPCSTR cpItem)
{
    AppendMenu(hMenu,MF_STRING,wID,cpItem);
}

SystemMenu :: SystemMenu(Window *opWin)
            : Menu(0,opWin)
{
}

SystemMenu :: SystemMenu(Dialog *opDlg)
            : Menu(0,opDlg)
{
}

SystemMenu :: ~SystemMenu(void)
{
}
////////////////////////////////////// EOF ////////////////////////////////////
