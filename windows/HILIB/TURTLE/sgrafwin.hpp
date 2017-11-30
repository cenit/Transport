///////////////////////////////////////////////////////////////////////////////
////////////////////// GrafWin window class definition ////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef GRAFWIN_HPP
#define GRAFWIN_HPP

#define NUMBER_OF_COLORS 10

typedef struct
{
   int r;
   int g;
   int b;
} Rgb;

typedef struct
{ 
   Rgb Screen;
   Rgb AxisLines;
   Rgb AxisText;
   Rgb HistBody;
   Rgb HistBack;
   Rgb LegoBody;
   Rgb GridLines;
   Rgb ContBack;
   Rgb ContLine1;
   Rgb ContLine2;
} COLORS;

class GrafWin : public Window
{
        Window *opPar;
        BOOL closed;
        DisplayDevice *opDev;
        Font *opFont, *opFont1, *opFont2, *opOldFont;
        int OldPosx, OldPosy, OldPosMx, OldPosMy;
        Pen *opOldPen, *opPen1, *opPen2, *opPen3, *opPen4;
        Menu *opPopupMenu;
        HMENU hSubMenu;
        HWND hPrntWnd;
        int  h_user, w_user, H_USER, W_USER, h_char, H_CHAR;
        UINT   KeyCode;
        char   *Tstring;
        BOOL   color;
        BOOL   clipb;
	BOOL   toprint ;
        IOFILE *dp;
        BOOL Histo, Cont, Lego, Grid;
        int ix[200], iy[200], np, N, Nunit, Ny, Nunity, Nhist;
        int Phi, Tet, Nniv, PhiL, TetL;
        float hu, wu, wh, w0, hh, h0;
        SCLTYP scltyp;
        VIEWDI View;
        int x1old, y1old, x2old, y2old;
        float Xmin, Xmax, Ymin, Ymax, Xstep, Ystep;
        float Xmom, Xmom2, Xsum;
        BOOL statistics;
        BOOL colorflag, printcolorflag, colorprint;
        COLORS *Col;
	BOOL pon;
	int  Phi0, Tet0, x0, y0, signumx, signumy;
	int  u1, v1, u2, v2, u3, v3, u4, v4;
	HCURSOR hArrow, hUpDown, hLeftRight, hCursor;
	float zcoord;
        char zlabel[5];
	DisplayDevice *opSaved;
	MemoryDevice *opMem;
	int BMWidth, BMHeight;

    protected:
	long WmEraseBackGround(void);
        BOOL WmSetCursor(VOID);
        LONG WmSize(int width, int height);
        BOOL Init(VOID);
        long WmPaint( const PAINTSTRUCT &);
        long WmSysMenuCommand(WPARAM);
        long WmMenuCommand(WPARAM);
        BOOL WmClose(void);
        BOOL WmDestroy(void);
        long WmKeyDown(WPARAM);
        long WmRightMouseDown(POINT &);
        long WmLeftMouseDown(POINT &);
        long WmLeftMouseUp(POINT &);
        long WmMouseMove(POINT &, WPARAM);
        VOID DrawFrame(VOID);
        VOID DrawFrame2(VOID);
        VOID DrawHist(VOID);
        VOID DrawCont(VOID);
        VOID DrawLego(VOID);
        VOID DrawGrid(VOID);
        VOID Caption(char *, int, float &, BOOL dim=TRUE);
        VOID Timds(VOID);
        VOID Contour(int *, int, int *, int, int[DIM][DIM], int, int *, int);
        VOID TriCont(int, int, int, int, int, int *, int, int, int, int);
	VOID CheckMenu(SCLTYP);
	VOID CheckMenu(BOOL,BOOL,BOOL);
        VOID DrawHistTitle(char *, int, int, int, SCLTYP, float, char *);
        int Ellips(float *iplot, float, int, float, float, int, float,
		         float *, float *, float *, float *, float *, float *, float *);
	int Mean(float *iplot,  float, int, float, float *, float *, float *);
    public:
        GrafWin(int, LPCSTR, RECT, Window *, BOOL, BOOL, IOFILE *,
                COLORS *);
        ~GrafWin(VOID);
        VOID Hist(BOOL,int,TUGDATA);
        VOID ContPlot(BOOL,int,TUGDATA);
        VOID LegoPlot(BOOL,int,TUGDATA);
        VOID GridPlot(BOOL,int,TUGDATA);
        VOID Erase(VOID);
        VOID ToClipboard(VOID);
        VOID Print(VOID);
        VOID PrintToFile(VOID);
        UINT GetKey(VOID);
        UINT WaitForKeyDown(VOID);
        VOID NewPenColors(VOID);
};

#endif

///////////////////////////////// EOF /////////////////////////////////////////
