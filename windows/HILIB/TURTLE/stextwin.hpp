///////////////////////////////////////////////////////////////////////////////
//////////////////// definition of class TextWindow ///////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern "C"
{
    LONG APIENTRY MyListProc( HWND hWnd, UINT iMessage,
                              WPARAM wParam, LPARAM lParam);
}

class TextWindow : public Window
{
    static RECT defwindim;
    Font *opFont;
    Brush *opBrush;
    BOOL color, aborted, closed;
    ListBox *opList;
    WNDPROC lpListProc;
    static WNDPROC lpMyListProc;
    static int counter;
    int Id;
    int fontsize;

  public:

    TextWindow(int nCmdShow = SW_SHOWNORMAL,
               LPCSTR szWindowName = NULL, RECT rc = defwindim);
    virtual ~TextWindow();
    void WriteLn(LPCSTR line);
    void ShowFile(LPCSTR FileName);
    void SetFocus(void);
    void FontSize(void);
    void Home(void);
    void End(void);
    void Save(void);
    void Print(void);
    void ToClipboard(void);
    BOOL WmNewChar( WPARAM Key );
    BOOL WmNewKeyUp( WPARAM Key );
    BOOL WmNewKeyDown( WPARAM Key );
    WNDPROC GetlpListProc(void) { return lpListProc; };

  protected:

    virtual long WmPaint( const PAINTSTRUCT &PaintStruct );
    virtual long WmSysMenuCommand ( WPARAM MenuItem ); 
    virtual LONG WmSize(int width, int height);
    virtual LONG WmCtlColor(HDC hdc, HWND hChild);
    virtual BOOL WmDestroy( void );
    virtual BOOL WmClose( void );
};

///////////////////////////////////////////////////////////////////////////////
/////////////////////// Definition of class ToLongDialog //////////////////////
///////////////////////////////////////////////////////////////////////////////

class ToLongDialog : public ModalDialog
{
     char caString[80];
     LPCSTR caCaption;
     BOOL result;
     HWND hPar;
     StringEdit *opEdit;
     StaticText *opText;
   public:
     ToLongDialog( int ResId, Window *opWin, LPCSTR text, LPCSTR caption);
     ~ToLongDialog();
     BOOL Init   ( void );
     long Control( UINT nControlId, UINT nNotificationCode );
     BOOL GetResult( void );
     VOID SetString(LPCSTR cpString);
     char *GetString( void );
};

////////////////////////////////////// EOF ////////////////////////////////////
