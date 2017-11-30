
void CenterRightDialog(HWND hPar, HWND hWin);

#ifndef DIALOGS_HPP
#define DIALOGS_HPP

///////////////////////////////////////////////////////////////////////////////
///////////////////////// Definition of class ParDialog ///////////////////////
///////////////////////////////////////////////////////////////////////////////

class ParDialog : public ModalDialog
{
     BOOL result;
     HWND hPar;
     StringEdit *opSign, *opVary, *opPar[20], *opLabel;
     StaticText *opDummy, *opType, *opNam[20];
     char line[133];
     char varycodes[40];
     char label[20];
     char fst1[20];
     char sigstr[5];
     int  sign, typecode, nr;
     char *titel;
     char **t;
     BOOL angle, update, Turtle, equad, ebend;
     char delim[2], delim2[2];
     int  nrblanks;

   protected:
     BOOL Init   ( void );
     long Control( UINT nControlId, UINT nNotificationCode );
     long WmLeftMouseDown(POINT &);
     long WmRightMouseDown(POINT &);
     long WmPaint(HWND hDlg, PAINTSTRUCT &ps);

   public:
     TextWindow *opText;
     ParDialog( int ResId, Dialog *opDlg, char *, char *, BOOL, BOOL, BOOL, BOOL, BOOL);
     BOOL GetResult( void );
     char *GetString( void );
     void Fillin(int, BOOL);
     ~ParDialog();
};

///////////////////////////////////////////////////////////////////////////////
//////////////////////// Definition of class TEditDialog //////////////////////
///////////////////////////////////////////////////////////////////////////////

class TEditDialog : public ModalDialog
{
     char caString[1001];
     BOOL result;
     HWND hPar;
     StringEdit *opEdit;
     UINT wLen;
     BOOL fixedfont;
   public:
     TEditDialog( int ResId, Window *opWin, LPCSTR text, UINT wlen=40 );
     TEditDialog( int ResId, Dialog *opDlg, LPCSTR text, UINT wlen=40 );
     TEditDialog( int ResId, Dialog *opDlg, LPCSTR text, UINT wlen, BOOL ff);
     ~TEditDialog();
     BOOL Init   ( void );
     long Control( UINT nControlId, UINT nNotificationCode );
     BOOL GetResult( void );
     VOID SetString(LPCSTR cpString);
     char *GetString( void );
};

#endif

/////////////////////////////////////// EOF ///////////////////////////////////
