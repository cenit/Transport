///////////////////////////////////////////////////////////////////////////////
//////////////////// TransEllipseWin3 window class definition /////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef TELIWIN3_HPP
#define TELIWIN3_HPP

class TransEllipseWin3 : public Window
{
        DisplayDevice *opDev;
        Font *opFont, *opFont1, *opFont2;
        Font *opFontA, *opFontB, *opFontC, *opFontD, *opOldFont;
        Pen *opOldPen;
        Pen *opBlackPen, *opWhitePen, *opGreenPen, *opRedPen;
        Brush *opBlackBrush, *opWhiteBrush;
        TransGrafFile *opGrafFile;
        HWND hPrntWnd;
        int    h_user, w_user;
        int    h_off, w_off;
        float  magn_fac, f_scal;
        UINT   KeyCode;
        float  x0, z0;
        BOOL   color, colorflag, printcolorflag, colorprint;
        int    counter, NumberOfFrames;
        float  x[4],y[4],r13[4],xp[4],yp[4],r24[4];
        char   infoline[4][80];
        int    x0x[4], y0x[4], x0y[4], y0y[4], len;
        int    xlx[4], ylx[4], xly[4], yly[4];
        float  xscal[4], xpscal[4], yscal[4], ypscal[4];
        float  dummy[500], sind[202], cosd[202];
        char   dim0[5], dim1[5], dim2[5], dim3[5], dim7[5];
        BOOL   closed;
	int    plw;
	int    XOFFSET, YOFFSET, ww;

    protected:
	long WmEraseBackGround(void);
        LONG WmSize(int width, int height);
        long WmSysMenuCommand(WPARAM MenuItem);
	long WmKeyDown(WPARAM Key);
        BOOL Init(VOID);
        long WmPaint( const PAINTSTRUCT & );
        BOOL WmDestroy( void );
	BOOL WmClose( void );
        VOID DrawFrame(VOID);
        VOID DrawEllipses(VOID);
        VOID DrawCaptions(VOID);
        VOID Scaling(float, float &, int &, float &, int &);
	VOID CreatePens(int);
	VOID DestroyPens(VOID);

    public:
        TransEllipseWin3(int, LPCSTR, RECT, Window *,
                        TransGrafFile *, BOOL, BOOL, int);
        ~TransEllipseWin3(VOID);
        VOID GetDimensions(char *, char *, char *, char *, char *);
        VOID TransferEllipsePar(float, float, float, float, 
                                float, float, char *);
        VOID FourFrames(VOID);
        VOID SingleFrame(VOID);
        VOID Dsplay(int);
        VOID Erase(VOID);
        VOID Print(VOID);
        VOID PrintToFile(VOID);
};

#endif

///////////////////////////////// EOF /////////////////////////////////////////
