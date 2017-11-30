///////////////////////////////////////////////////////////////////////////////
////////////////////////// Definition of class Dialog /////////////////////////
///////////////////////////////////////////////////////////////////////////////

extern "C"
{
   long APIENTRY DlgProc (HWND hWnd, UINT wMsg,
                          WPARAM wParam, LPARAM lParam);
}

class Dialog : public Object
{

//    Dialog* pNextDialog_;
//    Dialog* pPrevDialog_;

    static  int nHandlerCount_;
    HBRUSH  hBrush, hBrushB;
    HICON   hIcon;
    static  HBITMAP hBitmap;

  protected:
    BOOL Close( void );
    BOOL Free( void );
    virtual void Start();
    virtual void TerminateDialog();
    VOID SetHandle(HWND hDlg);
    LPCSTR pResourceName_;
    HWND     hParent_;
    static DLGPROC pHandler_;
    BOOL   bClosing_;
    HWND hDlg_;

  public:
    Dialog( int ResId, HWND hParent);
    virtual ~Dialog();
    virtual BOOL Init( void );
    virtual long Control( UINT nControlId, UINT nNotificationCode );
    virtual long WmCtlColor(HDC hdc);
    virtual long WmCtlColor2(HDC hdc);
    virtual long WmCtlColorStatic(HDC hdc, HWND child);
    virtual long DrawOwnerButton(LPDRAWITEMSTRUCT lpdis);
    virtual long WmDrawItem(WPARAM wParam, LPARAM lParam);
    virtual long WmMeasureItem(WPARAM wParam, LPARAM lParam);
    virtual long WmPaint(HWND hDlg, PAINTSTRUCT &ps);
    virtual long WmSysMenuCommand( WPARAM MenuItem );
    virtual long WmContextMenu( WPARAM wParam, LPARAM lParam);
    virtual long WmRightMouseDown( POINT &Pos);
    virtual long WmLeftMouseDown( POINT &Pos);
    virtual long WmNotify( LPARAM lParam);
    virtual long WmSize(WPARAM wParam, LPARAM lParam);
    virtual long WmNotifyIcon(WPARAM wParam, LPARAM lParam);
    virtual BOOL WmTimer(UINT id);
    virtual BOOL WmActivate(UINT wP, HWND hD);
    virtual VOID Adjust( HWND hwnd, UINT Code, UINT Pos);
    virtual VOID AdjustP( HWND hwnd, UINT Code, UINT Pos);
    virtual VOID GetSizes( UINT nControlId, int &LineSize, int &PageSize);
    HWND GetHandle( void );
    void SetIcon(HICON hIc);
    long Handler( HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam );

//    Dialog* FindDialog( HWND hDlg );
//    static  Dialog* pDialogList_;

};

///////////////////////////////////////////////////////////////////////////////
/////////////////////// Definition of class ModalDialog ///////////////////////
///////////////////////////////////////////////////////////////////////////////

class ModalDialog  : public Dialog
{
   public:
     ModalDialog( int ResId, Window *opWin);
     ModalDialog( int ResId, Dialog *opDlg);
     virtual ~ModalDialog();
     virtual void Start();

   protected:
     void TerminateDialog();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////// Definition of class ModelessDialog //////////////////////
///////////////////////////////////////////////////////////////////////////////

class ModelessDialog  : public Dialog
{
   public:
     ModelessDialog( int ResId, Window *opWin );
     ModelessDialog( int ResId, Dialog *opWin );
     ModelessDialog( int ResId, HWND hDlg );
     virtual ~ModelessDialog();
     virtual void Start();
     virtual BOOL WmActivate(UINT wP, HWND hD);

   protected:
     void TerminateDialog();
};

//////////////////////////////////// EOF //////////////////////////////////////
