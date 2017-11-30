///////////////////////////////////////////////////////////////////////////////
///////////////////// Definition of class StaticText //////////////////////////
///////////////////////////////////////////////////////////////////////////////

class StaticText : public Object
{
    int idControl;
    HWND hDlg, hControl;
    BOOL isDialogItem;
  public:
    StaticText(int idC, Dialog *opDialog);
    StaticText(int idC, Window *opParent,
               int x, int y, int w, int h, LPCSTR szText);
    ~StaticText(void);
    void SetText(LPCSTR text);
    void GetText(char *text, UINT len);
    HWND GetHandle(void);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////// Definition of class StringEdit //////////////////////////
///////////////////////////////////////////////////////////////////////////////

class StringEdit : public Object
{
    int idControl;
    HWND hDlg, hControl;
    BOOL isDialogItem;
  public:
    StringEdit(int idC, Dialog *opDialog);
    StringEdit(int idC, Window *opParent,
               int x, int y, int w, int h, LPCSTR szText);
    ~StringEdit(void);
    HWND GetHandle(void);
    void GetText(char *szText, UINT len);
    void SetText(LPCSTR text);
    void SetFocus(void);
    void SetCharLimit(UINT wLim);
    void SetCrntSelection(UINT Low, UINT High);
    void GetCrntSelection(int &Low, int &High);
    void SetSize(int w, int h);
    void SetFont(Font *opFont);
    UINT GetLineCount(void);
    UINT GetSize(void);
    UINT GetTextLine(UINT wInd, char *cpBuf, UINT wLen);
    void ReadFile(LPCSTR FileName);
    HGLOBAL GetEditBuffer(void);
    void WriteFile(LPCSTR FileName);
    void ReadMemory(HLOCAL hMemSrc, int BuffSize);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////// Definition of class PushButton //////////////////////////
///////////////////////////////////////////////////////////////////////////////

class PushButton : public Object
{
    int idControl;
    HWND hDlg, hControl;
    BOOL isDialogItem;
  public:
    PushButton(int idC, Dialog *opDialog);
    PushButton(int idC, Window *opParent,
               int x, int y, int w, int h, LPCSTR szText);
    ~PushButton(void);
    HWND GetHandle(void);
    void GetText(char *szText, UINT len);
    void SetText(LPCSTR text);
    void Enable(void);
    void Disable(void);
    void SetFocus(void);
};

///////////////////////////////////////////////////////////////////////////////
//////////////////////// Definition of class ListBox //////////////////////////
///////////////////////////////////////////////////////////////////////////////

class ListBox : public Object
{
    int idControl;
    HWND hDlg, hControl;
    BOOL isDialogItem;
  public:
    ListBox(int idC, Dialog *opDialog);
    ListBox(int idC, Window *opParent,
               int x, int y, int w, int h, LPCSTR szText);
    ~ListBox(void);
    HWND GetHandle(void);
    void Append(LPCSTR string);
    void SetFont(Font *opFont);
    void SetFocus(void);
    UINT GetCrntInd(void);
    VOID GetCrntItem(LPSTR);
};

///////////////////////////////////////////////////////////////////////////////
////////////////////// Definition of class ScrollBar //////////////////////////
///////////////////////////////////////////////////////////////////////////////

class ScrollBar : public Object
{
   
    int idControl;
    HWND hDlg;
    int PageSize;
    int LineSize;
  public:
    ScrollBar(int idC, Dialog *opDialog);
    ~ScrollBar(void);
    void SetPageSize(int ps) { PageSize = ps; };
    int GetPageSize(void) { return PageSize; };
    void SetLineSize(int ls) { LineSize = ls; };
    int GetLineSize(void) { return LineSize; };
    void SetRange(int low, int hi);
    void SetThumbPos(int pos);
    int GetThumbPos(void);
};

///////////////////////////////////////////////////////////////////////////////
////////////////////// Definition of class RadioButton ////////////////////////
///////////////////////////////////////////////////////////////////////////////

class RadioButton : public Object
{
    int idControl;
    HWND hDlg;
  public:
    RadioButton(int idC, Dialog *opDialog);
    ~RadioButton(void);
    void SetState(UINT wState);
    UINT GetState(void);
};

///////////////////////////////////////////////////////////////////////////////
/////////////////////// Definition of class CheckBox //////////////////////////
///////////////////////////////////////////////////////////////////////////////

class CheckBox : public Object
{
    int idControl;
    HWND hDlg;
  public:
    CheckBox(int idC, Dialog *opDialog);
    ~CheckBox(void);
    void SetState(UINT wState);
    UINT GetState(void);
};

///////////////////////////////////////////////////////////////////////////////
/////////////////////// Definition of class ComboBox //////////////////////////
///////////////////////////////////////////////////////////////////////////////

class ComboBox : public Object
{
    int idControl;
    HWND hDlg;
  public:
    ComboBox(int idC, Dialog *opDialog);
    ~ComboBox( void );
    void SetText(LPCSTR szText);
    void GetText(char *szText, UINT len);
    void SetFocus( void );
    void SetCharLimit (UINT wLim);
    void SetCrntSelection(UINT Low, UINT High);
    void GetCrntSelection(int &start, int &end);
    UINT GetLineCount(void);
    void Append(char *szText);
};

///////////////////////////////////////////////////////////////////////////////
////////////////////// Definition of class AmpMeter ///////////////////////////
///////////////////////////////////////////////////////////////////////////////

class AmpMeter : public Object
{
    int idControl;
    HWND hDlg;
  public:
    AmpMeter(int idC, Dialog *opDialog);
    ~AmpMeter(void);
    void SetText(LPCSTR text);
    void SetValue(int value);
    void SetRange(int low, int hi);
};

///////////////////////////////////////////////////////////////////////////////
/////////////////////// Definition of class RotKnob ///////////////////////////
///////////////////////////////////////////////////////////////////////////////

class RotKnob : public Object
{
    int idControl;
    HWND hDlg, hControl;
  public:
    RotKnob(int idC, Dialog *opDialog);
    ~RotKnob(void);
    void GetText(char *szText, UINT len);
    void SetText(LPCSTR text);
    void SetFocus(void);
    void SetRange(int min, int max);
    void SetPos(int pos);
    int  GetPos( void );
};

///////////////////////////////////////////////////////////////////////////////
///////////////////// Definition of class ColorButton /////////////////////////
///////////////////////////////////////////////////////////////////////////////

class ColorButton : public Object
{
    int idControl;
    HWND hDlg;
  public:
    ColorButton(int idC, Dialog *opDialog);
    ~ColorButton(void);
    void GetText(char *szText, UINT len);
    void SetText(LPCSTR text);
    void Enable(void);
    void Disable(void);
    void SetFocus(void);
    void SetTxtColor(int r, int g, int b);
    void ResetTxtColor( void );
    void SetNewBrushes(Brush *opFace, Brush *opShadow, Brush *opHilite);
    void ResetBrushes( void );
    HWND GetHandle( void );
};
/////////////////////////////////// EOF ///////////////////////////////////////

