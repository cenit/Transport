///////////////////////////////////////////////////////////////////////////////
/////////////////////// Definition of class MainWindow ////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern "C"
{
    LONG APIENTRY MyEditProc( HWND hWnd, UINT iMessage,
                              WPARAM wParam, LPARAM lParam);
}

class MainWindow : public Window
{
    Font *opFont;
    Brush *opBrush, *opBrush2;
    Menu *opMenu, *opSysMenu;
    BOOL color, aborted, closed;
    StringEdit *opEdit;
    StaticText *opLineNr;
    int LineNumber;
    WNDPROC lpEditProc;
    static WNDPROC lpMyEditProc;
    char FindStr[80];
    char ReplaceStr[80];
    BOOL wCase, wQuery, wReplaceAll;
    BOOL keypad, Forward, GoldPressed;
    char LineBuff[150], UndeleteBuff[150];
    char WordBuff[150];
    int  BeginningOfCurrentLine, EndOfCurrentLine;
    int  StartMark;
    UINT SaveCode;
    int  OffsetFromTop;
    BOOL done;
    BOOL MouseDown;
    HWND hWndToolbar;
    HWND hWndStatusBar;
    BOOL GoldKey;
    int  OldNumLockStatus;

  public:

    MainWindow(int nCmdShow, LPCSTR szWindowName, RECT rc);
    virtual ~MainWindow();
    void End(void);
    void Home(void);
    void Save( void );
    void Open( void );
    void BackFile(LPCSTR FileName);
    void Print( void );
    void DisplayLineNumber( void );
    void Find( BOOL );
    void Replace( BOOL, BOOL, BOOL);
    void DeleteLine(void);
    void DeleteToEndOfLine(void);
    void UndeleteLine(void);
    void DeleteWord(void);
    void UndeleteWord(void);
    void StepWord(void);
    void Specins(void);
    void ExecCommand(void);
    void GotoLine(UINT LineNr);
    void SetNumLock(void);
    void SaveOldNumLockStatus(void);
    void SetStatusButtons(void);
    void ResetNumLock(void);
    BOOL WmNewChar( WPARAM Key, LPARAM Scan );
    BOOL WmNewKeyUp( WPARAM Key, LPARAM Scan );
    BOOL WmNewKeyDown( WPARAM Key, LPARAM Scan );
    BOOL WmNewLeftMouseDown( void );
    BOOL WmNewLeftMouseUp( void );
    BOOL WmNewMouseMove( void );
    BOOL WmNewPaint( void );
    BOOL Adjust(UINT);
    WNDPROC GetlpEditProc(void) { return lpEditProc; };
#ifdef WIN95
    VOID ApplicationBar(void);
#endif

  protected:

    virtual long WmPaint( const PAINTSTRUCT &PaintStruct );
    virtual long WmMenuCommand ( WPARAM MenuItem ); 
    virtual long WmSysMenuCommand ( WPARAM MenuItem ); 
    virtual LONG WmSize(int width, int height);
    virtual LONG WmLeftButtonDblclk(POINT &);
    virtual LONG WmNotify(LPARAM);
    virtual LONG WmCtlColor(HDC hdc, HWND hChild);
    virtual LONG WmTimer(UINT id);
    virtual BOOL WmDestroy( void );
    virtual BOOL WmClose( void );
};

////////////////////////////////////// EOF ////////////////////////////////////

