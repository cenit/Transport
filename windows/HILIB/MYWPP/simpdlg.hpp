///////////////////////////////////////////////////////////////////////////////
///////////////////////// Definition of common function ///////////////////////
///////////////////////////////////////////////////////////////////////////////

void CenterDialog(HWND hPar, HWND hWin);

///////////////////////////////////////////////////////////////////////////////
/////////////////////// Definition of class SimpleDialog //////////////////////
///////////////////////////////////////////////////////////////////////////////

class SimpleDialog : public ModalDialog
{
     LPCSTR Text;
     BOOL result;
     HWND hPar;
   public:
     SimpleDialog( int ResId, Window *opWin, LPCSTR text = NULL);
     SimpleDialog( int ResId, Dialog *opDlg, LPCSTR text = NULL);
     ~SimpleDialog();
     BOOL Init   ( void );
     long Control( UINT nControlId, UINT nNotificationCode );
     BOOL GetResult( void );
};

///////////////////////////////////////////////////////////////////////////////
/////////////////////// Definition of class StringDialog //////////////////////
///////////////////////////////////////////////////////////////////////////////

class StringDialog : public ModalDialog
{
     char caString[233];
     BOOL result;
     HWND hPar;
     StringEdit *opEdit;
     UINT wLen;
     BOOL fixedfont;
   public:
     StringDialog( int ResId, Window *opWin, LPCSTR text, UINT wlen=40 );
     StringDialog( int ResId, Dialog *opDlg, LPCSTR text, UINT wlen=40 );
     StringDialog( int ResId, Dialog *opDlg, LPCSTR text, UINT wlen, BOOL ff);
     ~StringDialog();
     BOOL Init   ( void );
     long Control( UINT nControlId, UINT nNotificationCode );
     BOOL GetResult( void );
     VOID SetString(LPCSTR cpString);
     char *GetString( void );
};

///////////////////////////////////////////////////////////////////////////////
/////////////////////// Definition of class OpenFileDlg ///////////////////////
///////////////////////////////////////////////////////////////////////////////

class OpenFileDlg
{
    BOOL result;
    char FileName[100];
  public:
    OpenFileDlg(HWND hParent, LPCSTR Dir, LPCSTR Fltstr, LPCSTR Ext,
                LPCSTR Title = NULL);
    ~OpenFileDlg(void);
    BOOL GetResult(void);
    char *GetFileName(void);
};

///////////////////////////////////////////////////////////////////////////////
/////////////////////// Definition of class SaveFileDlg ///////////////////////
///////////////////////////////////////////////////////////////////////////////

class SaveFileDlg
{
    BOOL result;
    char FileName[100];
  public:
    SaveFileDlg(HWND hParent, LPCSTR Dir, LPCSTR Fltstr, LPCSTR Ext,
                LPCSTR Title = NULL);
    ~SaveFileDlg(void);
    BOOL GetResult(void);
    char *GetFileName(void);
};

/////////////////////////////////////// EOF ///////////////////////////////////
