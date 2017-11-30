
///////////////////////////////////////////////////////////////////////////////
//////////////////////// Definition of class EliDialog   //////////////////////
//////////////////////// Definition of class EliDialog2  //////////////////////
//////////////////////// Definition of class EliDialog3  //////////////////////
//////////////////////// Definition of class InfoDialog  //////////////////////
//////////////////////// Definition of class SigmaDialog //////////////////////
//////////////////////// Definition of class TijkDialog  //////////////////////
//////////////////////// Definition of class AuxWin      //////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef FLATDLG_HPP
#define FLATDLG_HPP

#define MAX_STR_LEN 80

class EliDialog : public ModalDialog
{
    char caString[MAX_STR_LEN+1];
    UINT wMaxLen;
    StringEdit *opEdit;
    TransEllipseWin *opWin;
    TransGrafFile *opFile;
    Window *opPar;
    char *ptr;
    int y;
  protected:
    BOOL Init(VOID);
#ifdef WIN95
    long WmNotify(LPARAM);
#endif
    long Control(UINT, UINT);
    VOID DspEllipses(VOID);
  public:
    EliDialog(int ResID, Window *opParent, TransEllipseWin *, TransGrafFile *, int);
    ~EliDialog(VOID);
#ifdef WIN95
    DoCreateDialogTooltip(void);
#endif
};

class EliDialog2 : public ModalDialog
{
    char caString[MAX_STR_LEN+1];
    UINT wMaxLen;
    StringEdit *opEdit;
    TransEllipseWin2 *opWin;
    TransGrafFile *opFile;
    Window *opPar;
    char *ptr;
    int y;
  protected:
    BOOL Init(VOID);
#ifdef WIN95
    long WmNotify(LPARAM);
#endif
    long Control(UINT, UINT);
    VOID DspEllipses(VOID);
  public:
    EliDialog2(int ResID, Window *opParent, TransEllipseWin2 *, TransGrafFile *, int);
    ~EliDialog2(VOID);
#ifdef WIN95
    DoCreateDialogTooltip(void);
#endif
};

class EliDialog3 : public ModalDialog
{
    char caString[MAX_STR_LEN+1];
    UINT wMaxLen;
    StringEdit *opEdit;
    TransEllipseWin3 *opWin;
    TransGrafFile *opFile;
    Window *opPar;
    char *ptr;
    int y;
  protected:
    BOOL Init(VOID);
#ifdef WIN95
    long WmNotify(LPARAM);
#endif
    long Control(UINT, UINT);
    VOID DspEllipses(VOID);
  public:
    EliDialog3(int ResID, Window *opParent, TransEllipseWin3 *, TransGrafFile *, int);
    ~EliDialog3(VOID);
#ifdef WIN95
    DoCreateDialogTooltip(void);
#endif
};

class InfoDialog : public ModalDialog
{
    char caString[MAX_STR_LEN+1];
    UINT wMaxLen;
    StringEdit *opEdit;
    FILE *fpo;
    TextWindow *opWin;
    TransFile *opFile;
    char *ptr;
    BOOL first;
  protected:
    BOOL Init(VOID);
#ifdef WIN95
    long WmNotify(LPARAM);
#endif
    long Control(UINT, UINT);
    VOID DspData(VOID);
  public:
    InfoDialog(int ResID, Window *opParent, TextWindow *, TransFile *);
    ~InfoDialog(VOID);
#ifdef WIN95
    DoCreateDialogTooltip(void);
#endif
};

class SigmaDialog : public ModalDialog
{
    char caString[MAX_STR_LEN+1];
    UINT wMaxLen;
    StringEdit *opEdit;
    FILE *fpo;
    TextWindow *opWin;
    TrOutFile *opFile;
    char *ptr;
    BOOL first;
  protected:
    BOOL Init(VOID);
#ifdef WIN95
    long WmNotify(LPARAM);
#endif
    long Control(UINT, UINT);
    VOID DspData(VOID);
  public:
    SigmaDialog(int ResID, Window *opParent, TextWindow *, TrOutFile *);
    ~SigmaDialog(VOID);
#ifdef WIN95
    DoCreateDialogTooltip(void);
#endif
};

class TijkDialog : public ModalDialog
{
    char caString[MAX_STR_LEN+1];
    UINT wMaxLen;
    StringEdit *opEdit;
    FILE *fpo;
    TextWindow *opWin;
    TransSecondFile *opFile;
    char *ptr;
    BOOL first;
  protected:
    BOOL Init(VOID);
#ifdef WIN95
    long WmNotify(LPARAM);
#endif
    long Control(UINT, UINT);
    VOID DspData(VOID);
  public:
    TijkDialog(int ResID, Window *opParent, TextWindow *, TransSecondFile *);
    ~TijkDialog(VOID);
#ifdef WIN95
    DoCreateDialogTooltip(void);
#endif
};

class AuxWin : public Window
{
   protected:
     long WmPaint( const PAINTSTRUCT &);
   public:
     AuxWin(int, LPCSTR, RECT );
     ~AuxWin(VOID);
};

class AuxWin2 : public Window
{
   protected:
     long WmPaint( const PAINTSTRUCT &);
   public:
     AuxWin2(int, LPCSTR, RECT );
     ~AuxWin2(VOID);
};

class AuxWin3 : public Window
{
   protected:
     long WmPaint( const PAINTSTRUCT &);
   public:
     AuxWin3(int, LPCSTR, RECT );
     ~AuxWin3(VOID);
};

#endif

//////////////////////////////////// EOF //////////////////////////////////////
