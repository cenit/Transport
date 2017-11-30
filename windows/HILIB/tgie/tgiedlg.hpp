///////////////////////////////////////////////////////////////////////////////
////////////////////////// class TgieDialog definition ////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef TGIEDLG_HPP
#define TGIEDLG_HPP

int GetNumberOfDataItems(int typecode, BOOL update, BOOL Turtle);
BOOL isLarge(HWND hWnd);

class TgieDialog : public ModalDialog
{
    TextWindow	  *opText;
    ListBox       *opListBox;
    TransInFile   *opInput;
    StringDialog  *opEdit;
    RadioButton   *opTitle, *opData;
    RadioButton   *opBefore, *opAfter;
    PushButton	  *opUpdate, *opLimits;
    int		  nlines;
    BOOL	  UseTitle, Before, angle, update;
    ParDialog     *opParDialog;
    char	  **SavedLines;
    int		  nSaved;
    HBITMAP       hBitmap;
    int		  typecode;
    BOOL	  Turtle;
    char	  CurrentFileName[132];
    HWND	  hPar;
    char          SCDIR[80], for001[100], for001bak[100], for001bk2[100];

  protected:
    BOOL Init(VOID);
    long Control(UINT, UINT);
    long WmSysMenuCommand(WPARAM );
    long WmDrawItem(WPARAM, LPARAM);
    long WmMeasureItem(WPARAM, LPARAM);
    BOOL WmTimer(UINT);
    VOID Exit(VOID);
    VOID EditLine(int);
    VOID ReadInput(char *);
    VOID EditMultiLine(int, int *);
#ifdef WIN95
    long WmContextMenu(WPARAM, LPARAM);
    long WmNotify(LPARAM);
    BOOL DoCreateDialogTooltip(void);
#endif

  public: 
    TgieDialog(int ResID, Window *, char *);
    ~TgieDialog(VOID);
};

#endif
///////////////////////////////// EOF /////////////////////////////////////////
