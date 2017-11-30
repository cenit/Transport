///////////////////////////////////////////////////////////////////////////////
////////////////////////// Some General Definitions ///////////////////////////
///////////////////////////////////////////////////////////////////////////////

int GetScreenWidth(void);
int GetScreenHeight(void);

///////////////////////////////////////////////////////////////////////////////
////////////////////////// definition of class Object /////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define IDO_APPLICATION 0
#define IDO_WINDOW  1
#define IDO_METADEV 2
#define IDO_DIALOG 3
#define IDO_STATIC_TEXT 4
#define IDO_STRING_EDIT 5
#define IDO_PUSH_BUTTON 6
#define IDO_LIST_BOX 7
#define IDO_RADIO_BUTTON 8
#define IDO_CHECK_BOX 9
#define IDO_SCROLL_BAR 10
#define IDO_COMBO_BOX 11
#define IDO_AMPERE_METER 12
#define IDO_ROTKNOB 13
#define IDO_COLOR_BUTTON 14
#define IDO_BRUSH 15
#define IDO_PEN 16
#define IDO_FONT 17
#define IDO_PRINTER 18
#define IDO_MEMORY 19
#define IDO_MENU 20
#define IDO_MEMORYDEV 21

class Object
{
    UINT Id;
  public:    
    Object(UINT id);
    virtual ~Object(void);  // virtual was missing, found by WATCOM C/C++
    VOID DispObjectMsg(LPCSTR cpFrmt,...);
};

///////////////////////////////////////////////////////////////////////////////
//////////////////////// definition of class Application //////////////////////
///////////////////////////////////////////////////////////////////////////////

class Dialog;
class Window;
class DisplayDevice;

class Application : public Object
{
    static HINSTANCE hInstance;
    static HINSTANCE hPrevInstance;
    static HACCEL hAccelTable;
    static HWND   ghActiveDlg;
    char lpszCmdLine[90];
    int nCmdShow;
    static Window *opMain;
  public:
    Application( HINSTANCE hInstance_
               , HINSTANCE hPrevInstance_
               , LPSTR lpszCmdLine_
               , int nCmdShow_ );
    ~Application(void);
    virtual UINT Run(void);
    static DisplayDevice *GetMainWindow(void) {return (DisplayDevice *)opMain;};
    static Window *GetMainWindowPtr(void) {return opMain;};
    static HINSTANCE GetInstance( void ) { return hInstance; }
    static HINSTANCE GetPrevInstance( void ) { return hPrevInstance; }
    static void   SetActiveDlg(HWND hDlg) { ghActiveDlg = hDlg; }
    UINT Start( void );
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////// Definition of class Memory //////////////////////////
///////////////////////////////////////////////////////////////////////////////

class Memory : public Object
{
     HLOCAL hMem;
     UINT BuffLen, Size, UnitSize, ReallocSize;
     PSTR pBuf;
   public:
     Memory(UINT size);
     ~Memory(void);
     void FreeMemory(void);
     HLOCAL GetHandle(void) { return hMem; };
     UINT GetSize(void) { return BuffLen; };
     void WriteLn(LPCSTR line);
     HLOCAL Reset(void);
     void Clear(void);
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Definition of class Menu //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Menu : public Object
{
     HMENU hMenu;
     BOOL  StandAlone;
   public:
     Menu(int ResId, Window *opWin);
     Menu(int ResId, Dialog *opDlg);
     ~Menu(void);
     void SetCheck(UINT wID, BOOL blState);
     void Enable(UINT wID);
     void Disable(UINT wID);
     void AppendSep(void);
     void Append(UINT wID, LPCSTR cpItem);
     HMENU GetHandle(void) { return hMenu; };
};

class SystemMenu : public Menu
{
   public:
     SystemMenu(Window *opWin);
     SystemMenu(Dialog *opDlg);
     ~SystemMenu(void);
};

////////////////////////////////////// EOF ////////////////////////////////////
