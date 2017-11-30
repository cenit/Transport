///////////////////////////////////////////////////////////////////////////////
/////////////////////// Definition of class MainWindow //////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef TPMENU_HPP
#define TPMENU_HPP

#ifdef WIN32
#ifdef __BORLANDC__
#define BCNT
#endif
#endif

extern "C"
{
   void wait(int msecs);
}

class MainWindow : public Window
{
        TextWindow *opWin1;
        Menu *opMenu;
	SystemMenu *opSysMenu;
        HWND  hWnd;              // Window handle
        int   w_user, h_user, w_full, w_bitmap, h_bitmap;
        int   SDISK;
        char  SDIR[80];
        FILE  *fpo;
        char  for000[150], for001[150], for002[150], for004[150]; 
	char  save0[150], keep0[150];
        BOOL  dpmirun;
        HWND  httywin;
        int   MODE;
        BOOL  DTM;
        char  NullStr[1], CStr[2], Blank[2], nCStr[3];
	HWND  hWndToolbar;
	SOCKET sock;
	HINSTANCE hCamacLib;
	int   PLW;
	BOOL  edtgui;

    protected:
	long  WmMenuCommand( WPARAM );
 	long  WmPaint( const PAINTSTRUCT & );
        long  WmSize( int width, int height );
        BOOL  WmDestroy( void );
        BOOL  WmClose( void );
        long  WmTimer(UINT id);
	long  WmSysMenuCommand( WPARAM );
#ifdef WIN95
	LONG  WmNotify(LPARAM);
	LONG  WmLeftButtonDblclk(POINT &);
#endif

    public:
	MainWindow(int nCmdShow, LPCSTR szWindow, RECT rc );
	~MainWindow(VOID);
        VOID  ExecuteCommand(UINT, char *);
        VOID  DrawDIB256(DisplayDevice *, HDC );
        VOID  DrawBitmap(DisplayDevice *, HDC );
        VOID  Edit(LPCSTR);
        VOID  EBox(char);
        BOOL  EndDialog(VOID);
        int   Transport(int);
	static VOID  UnBusy(void);
	static VOID  UnBusy2(void);
	static VOID  UnBusy3(void);
#ifndef BCNT
        int   Transport0(int);
#else
        int   Transport1(int);
        VOID  Transport2(int, int);
#endif
        VOID  TransOut(void);
        VOID  TransOut(int);
        VOID  FileOut(TransFile *, int, char *);
        VOID  CamacOut(TransFile *, int, TextWindow *);
        VOID  Tijk(VOID);
        VOID  BeamInfo(VOID);
        VOID  SigmaOut(VOID);
        VOID  CopyFile(char *);
        VOID  PrintFile(char *);
        VOID  PrinterSetup(VOID);
        VOID  OIFunc(int);
        VOID  DeleteScratch(VOID);
        VOID  DeleteFile(char *);
        VOID  Redraw(VOID);
        VOID  SecondOrder(char *, BOOL);
        VOID  DisplayParameters(VOID);
        VOID  Ellipses(VOID);
        VOID  Ellipse2(VOID);
        VOID  Ellipses_long(VOID);
        VOID  SaveRun(VOID);
        VOID  InsertDacValues(VOID);
	VOID  FxyDac(VOID);
	VOID  FxyGauss(VOID);
        VOID  Update(VOID);
        VOID  DPMICheck(VOID);
        VOID  DPMITrans(int);
        VOID  Bero(VOID);
	int   GetPrintLineWidth(VOID);
#ifdef WIN95
	VOID  ApplicationBar(VOID);
#endif
};

#endif

///////////////////////////////// EOF /////////////////////////////////////////

