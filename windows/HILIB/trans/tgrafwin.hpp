///////////////////////////////////////////////////////////////////////////////
////////////////////// TransGrafWin window class definition////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef TGRAFWIN_HPP
#define TGRAFWIN_HPP

class MainWindow;

class TransGrafWin : public Window
{
        DisplayDevice *opDev;
        Font *opFont, *opFont1, *opFont2, *opOldFont;
        Font *opFontX, *opFont3;
        Brush *opBlackBrush;
        Pen *opOldPen;
        Pen *opWhiteDottedPen, *opBlackDottedPen, *opRedDottedPen;
        Pen *opBlackPen, *opWhitePen, *opGreenPen, *opMagentaPen;
        Pen *opCyanPen, *opBluePen, *opRedPen, *opYellowPen;

	Menu *opPopupMenu;
        HMENU hSubMenu;
        MainWindow *opPar;
        TransGrafFile *opGrafFile;
        TransSecondFile *opSecFile;
        HWND hPrntWnd;
        int    h_user, w_user;
        int    h_off, w_off;
        float  magn_fac, f_scal;
        char   *Tstring;
        UINT   KeyCode;
        int    x0, z0;
        DISPAR *dispar;
        BOOL   color, colorflag, printcolorflag, colorprint, DisplayFlag;
        int    fs1, fs2, ch, cw;
        int    Result;
        BOOL   clipb;
        BOOL   closed;
	int    plw;
	int    XOFFSET, YOFFSET;
	BOOL   dcomp;
	float  apfac;

    protected:
	long WmEraseBackGround(void);
        BOOL Init(VOID);
        long WmPaint( const PAINTSTRUCT &);
        long WmSysMenuCommand(WPARAM);
        long WmMenuCommand(WPARAM);
        BOOL WmClose(void);
        BOOL WmDestroy(void);
        VOID Print(VOID);
        VOID PrintToFile(VOID);
        VOID ToClipboard(VOID);
        long WmKeyDown(WPARAM);
        long WmRightMouseDown(POINT &);
        long WmLeftMouseDown(POINT &);
        VOID DrawFrame(VOID);
        VOID DrawEnvelope(int);
        VOID DrawDrifts(VOID);
        VOID DrawMagnets(VOID);
        VOID DrawSlits(VOID);
        VOID DrawFits(VOID);
        VOID DrawRays(VOID);
        VOID DrawCSTrajectories(int);
        VOID DrawSecondOrder(VOID);
	VOID CreatePens(int);
	VOID DestroyPens(VOID);

    public:
        TransGrafWin(int, LPCSTR, RECT, MainWindow *, TransGrafFile *,
                     TransSecondFile *, DISPAR *, BOOL, BOOL, int);
        ~TransGrafWin(VOID);
        int GetResult(VOID);
        VOID Dsplay(int);
        UINT WaitForKeyDown(VOID);
};

#endif

///////////////////////////////// EOF /////////////////////////////////////////
