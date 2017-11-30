///////////////////////////////////////////////////////////////////////////////
//////////////////////// definition of class Window ///////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern "C"
{
   LONG APIENTRY WndProc( HWND hWnd, UINT iMessage,
                          WPARAM wParam, LPARAM lParam );
}

class Window : public DisplayDevice
{
private:

   static char szClassName_[14];
   static RECT defwindim;
   HDC hdc_;

protected:

   static HINSTANCE hInstance_;
   static HACCEL hAccel_;
   WPARAM wParam_;
   LPARAM lParam_;
   UINT iMessage_;
   HWND hWnd_;

   BOOL Show( int nCmdShow );
   void Update( void );
   void Invalidate( void );
   void Invalidate2( void );
   LONG DefaultEvent( void );
   virtual LONG WmPaint( const PAINTSTRUCT& PaintStruct );
   virtual BOOL WmDestroy( void );
   virtual BOOL WmClose( void );
   virtual BOOL WmSetCursor( void );
   virtual LONG WmMenuCommand (WPARAM MenuItem);
   virtual LONG WmUser (WPARAM wParam, LPARAM lParam);
   virtual LONG WmEraseBackGround(void);
   virtual LONG WmKeyDown (WPARAM Key);
   virtual LONG WmKeyUp (WPARAM Key);
   virtual LONG WmCharKey (WPARAM Key);
   virtual LONG WmRightMouseDown( POINT &Pos);
   virtual LONG WmLeftMouseDown( POINT &Pos);
   virtual LONG WmRightMouseUp( POINT &Pos);
   virtual LONG WmLeftMouseUp( POINT &Pos);
   virtual LONG WmLeftButtonDblclk( POINT &Pos);
   virtual LONG WmMouseMove( POINT &Pos, WPARAM);
   virtual LONG WmSysMenuCommand (WPARAM MenuItem);
#ifdef WIN95
   virtual LONG WmNotify(LPARAM lParam);
#endif
   virtual LONG WmSize(int nWidth, int nHeight);
   virtual LONG WmTimer(UINT id);
   virtual LONG WmCtlColor(HDC hdc, HWND hChild);
   virtual LONG WmVScroll(UINT Code, UINT Pos);
   virtual LONG WmHScroll(UINT Code, UINT Pos);
   virtual HDC  GetDC( void ) { return hdc_; };

public:
   
   Window( int nCmdShow = SW_SHOWNORMAL
         , LPCSTR szWindowName = NULL
         , RECT rc = defwindim
         , HMENU hMenu = NULL
         , HWND hParent = NULL
         , DWORD dwStyle = WS_OVERLAPPEDWINDOW );
   virtual ~Window();
   LONG WndProcess( UINT iMessage, WPARAM wParam, LPARAM lParam );
   static Window *GetWinPointer( HWND hWnd );
   void SetWinPointer( HWND hWnd );
   static BOOL Register( int nIconName = NULL
                       , LPCSTR pMenuName = NULL );
   HWND GetHandle(void) { return hWnd_; };   
   void Enable( void );
   void Disable( void );
};

//////////////////////////////////// EOF //////////////////////////////////////
