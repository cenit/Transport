///////////////////////////////////////////////////////////////////////////////
//////////////////////// Definition of class FindDialog ///////////////////////
///////////////////////////////////////////////////////////////////////////////

class FindDialog : public ModalDialog
{
     char caString[80];
     BOOL result;
     HWND hPar;
     StringEdit *opEdit;
     UINT wLen;
     CheckBox *opCase;
     BOOL wCase;
   public:
     FindDialog( int ResId, Window *opWin, LPCSTR text, BOOL c, UINT wlen=40 );
     ~FindDialog();
     BOOL Init   ( void );
     long Control( UINT nControlId, UINT nNotificationCode );
     BOOL GetResult( void );
     BOOL GetCase( void );
     VOID SetString(LPCSTR cpString);
     char *GetString( void );
};

///////////////////////////////////////////////////////////////////////////////
/////////////////////// Definition of class ReplaceDialog /////////////////////
///////////////////////////////////////////////////////////////////////////////

class ReplaceDialog : public ModalDialog
{
     char caFind[80], caReplace[80];
     BOOL result;
     HWND hPar;
     StringEdit *opFind, *opReplace;
     UINT wLenFind, wLenReplace;
     CheckBox *opCase;
     CheckBox *opQuery;
     BOOL wCase, wQuery, wReplaceAll;
   public:
     ReplaceDialog( int ResId, Window *opWin,
                    LPCSTR text1, LPCSTR text2,
                    BOOL c, BOOL q, UINT wlen1=40, UINT wlen2=40 );
     ~ReplaceDialog();
     BOOL Init( void );
     long Control( UINT nControlId, UINT nNotificationCode );
     BOOL GetResult( void );
     BOOL GetCase( void );
     BOOL GetQuery( void );
     BOOL GetReplaceAll( void );
     char *GetFindString( void );
     char *GetReplaceString( void );
};

///////////////////////////////////////////////////////////////////////////////
////////////////////////// QueryDialog class definition ///////////////////////
///////////////////////////////////////////////////////////////////////////////

class QueryDialog : public ModelessDialog
{
     HWND hPar;
     int result;
   protected:
     long Control(UINT, UINT);
     BOOL Init(VOID);
   public:
     QueryDialog(int ResID, Window *opParent);
     ~QueryDialog(VOID);
     int GetResult(VOID);
     void Reset(VOID);
};
////////////////////////////////////// EOF ////////////////////////////////////
