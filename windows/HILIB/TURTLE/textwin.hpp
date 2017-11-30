///////////////////////////////////////////////////////////////////////////////
///////////////////// definition of class TextWindow //////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern "C"
{
    LONG APIENTRY MyEditProc( HWND hWnd, UINT iMessage,
                              WPARAM wParam, LPARAM lParam);
}

class TextWindow : public Window
{
    static RECT defwindim;
    Font *opFont;
    Brush *opBrush;
    BOOL color, aborted, closed;
    StringEdit *opEdit;
    WNDPROC lpEditProc;
    static WNDPROC lpMyEditProc;
    static int counter;
    int Id;
    int fontsize;

  public:

    TextWindow(int nCmdShow = SW_SHOWNORMAL,
              LPCSTR szWindowName = NULL, RECT rc = defwindim);
    virtual ~TextWindow();
    void WriteLn(LPCSTR line);
    void End(void);
    void FontSize(void);
    void Home(void);
    void Erase(void);
    void SetFocus(void);
    void WaitFor(LPCSTR str);
    char WaitForKeyDown(void);
    void ShowFile(LPCSTR FileName);
    void ShowMemory(Memory *opMem);
    void ToClipboard(void);
    void Save( void );
    void Print( void );
    BOOL WmNewChar( WPARAM Key );
    BOOL WmNewKeyUp( WPARAM Key );
    BOOL WmNewKeyDown( WPARAM Key );
    WNDPROC GetlpEditProc(void) { return lpEditProc; };

  protected:

    virtual long WmPaint( const PAINTSTRUCT &PaintStruct );
    virtual long WmSysMenuCommand ( WPARAM MenuItem ); 
    virtual LONG WmSize(int width, int height);
    virtual LONG WmCtlColor(HDC hdc, HWND hChild);
    virtual BOOL WmDestroy( void );
    virtual BOOL WmClose( void );
};

////////////////////////////////////// EOF ////////////////////////////////////
