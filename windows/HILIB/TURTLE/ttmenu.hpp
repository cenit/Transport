///////////////////////////////////////////////////////////////////////////////
//////////////////////// Definition of class Ttmenu ///////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef TTMENU_HPP
#define TTMENU_HPP

#ifdef WIN32
#ifdef __BORLANDC__
#define BCNT
#endif
#endif

typedef struct
{
   int percent;
} CB;

extern "C"
{
   VOID DoSound(VOID);
   void wait(int msecs);
}

class MainWindow : public Window
{
        Menu *opMenu;
        SystemMenu *opSysMenu;
        MeterDialog *opMeter;
        TextWindow  *opWin;
        GrafWin  *opWin2;

        HWND  hWnd;
        int   w_user, h_user, w_full, w_bitmap, h_bitmap;
        int   SDISK;
        char  SDIR[80];
        FILE  *fpo;
        BOOL  dpmirun;
        HWND  httywin;
        COLORS Col;
	HWND  hWndToolbar;
	BOOL  edtgui;

    protected:
	long  WmMenuCommand(WPARAM);
	long  WmSysMenuCommand(WPARAM);
 	long  WmPaint( const PAINTSTRUCT &);
        long  WmTimer(UINT);
        long  WmSize(int, int);
        BOOL  WmClose(void);
        BOOL  WmDestroy(void);
	LONG  WmNotify(LPARAM);
#ifdef WIN95
	LONG  WmLeftButtonDblclk(POINT &);
#endif
    public:
	MainWindow(int,LPCSTR,RECT);
	~MainWindow(VOID);
        VOID  ExecuteCommand(UINT, char *);
        VOID  DrawBitmap(DisplayDevice *, HDC);
        VOID  DrawDIB256(DisplayDevice *, HDC);
        VOID  Edit(LPCSTR);
        VOID  EBox(char *);
        BOOL  EndDialog(VOID);
#ifndef WIN32
        int   Turtle(void);
#else
        int   Turtle1(void);
#endif
        VOID  Turtle2(int);
        VOID  DPMITurtle(void);
        VOID  TStop(void);
        VOID  CopyFile(char *);
        VOID  PrintFile(char *);
        VOID  PrinterSetup(VOID);
        VOID  DeleteScratch(VOID);
        VOID  DeleteFile(char *);
        VOID  Update(VOID);
        VOID  TurtOut(VOID);
	VOID  Histogramms(VOID);
        VOID  ColorMenu(int);
        VOID  GetColors(char *);
        VOID  SaveColors(char *);
	VOID  GetDefaultColors(char *);
	VOID  SetInternalColors(VOID);
#ifdef WIN95
	VOID  ApplicationBar(VOID);
#endif
};

#endif

///////////////////////////////// EOF /////////////////////////////////////////
