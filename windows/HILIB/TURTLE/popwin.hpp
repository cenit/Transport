///////////////////////////////////////////////////////////////////////////////
///////////////////// definition of class PopupWindow /////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern "C"
{
    LRESULT CALLBACK MyOKProc( HWND hWnd, UINT iMessage,
                            WPARAM wParam, LPARAM lParam);
}

class PopupWindow : public Window
{
    static RECT defwindim;
    HINSTANCE hInst;
    HBRUSH hBrush;
    LPCSTR bmname;
    HWND hOK;

  public:
    PopupWindow(int nCmdShow, LPCSTR szWindowName, LPCSTR szBitmapName, RECT rc);
    virtual ~PopupWindow();

  protected:
    void DrawBitmap(DisplayDevice *opDev, HDC hdcscr);
    virtual long WmPaint( const PAINTSTRUCT &PaintStruct );
    virtual long WmSize(int width, int height);
    virtual BOOL WmDestroy( void );
    virtual BOOL WmClose( void );
    virtual long WmMenuCommand(WPARAM mid);
    virtual long WmEraseBackGround(void);
};

////////////////////////////////////// EOF ////////////////////////////////////
