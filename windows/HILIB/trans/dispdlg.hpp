///////////////////////////////////////////////////////////////////////////////
///////////////////////// DisplayDialog class definition //////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef DISPDLG_HPP
#define DISPDLG_HPP

class DisplayDialog : public ModalDialog
{
     BOOL result;
     DisplayFile  *opDispFile;
     StringEdit *opParams[5];
     RadioButton *opOnButton[15];
     RadioButton *opOffButton[15];
     HWND hPar;
   protected:
     long Control(UINT, UINT );
     BOOL Init(VOID);
     VOID TransferParameters(VOID);
     void Specials(void);
     void Specials2(void);
   public:
     DisplayDialog(int ResID, Window *opParent, DisplayFile *);
     ~DisplayDialog(VOID);
     BOOL GetResult(VOID);
};

///////////////////////////////////////////////////////////////////////////////
/////////////////////////  GetDacDialog class definition //////////////////////
///////////////////////////////////////////////////////////////////////////////

class GetDacDialog : public ModalDialog
{
     int settings, result;
     RadioButton *opYesButton[5];
     RadioButton *opNoButton[5];
     HWND hPar;
   protected:
     long Control(UINT, UINT );
     BOOL Init(VOID);
     VOID TransferParameters(VOID);
   public:
     GetDacDialog(int ResID, Window *opParent, int);
     ~GetDacDialog(VOID);
     int GetResult(VOID);
};

///////////////////////////////////////////////////////////////////////////////
/////////////////////// Definition of class ListDialog ////////////////////////
///////////////////////////////////////////////////////////////////////////////

class ListDialog : public ModalDialog
{
     PushButton *opOkButton;
     HWND hPar;
     ListBox *opList;
     char **caStringList;
     int Item;
     UINT ListLength;
   public:
     ListDialog( int ResId, Window *opWin, char **list, UINT len);
     ~ListDialog();
     BOOL Init   ( void );
     long Control( UINT nControlId, UINT nNotificationCode );
     int GetIndex( void );
};

#endif
///////////////////////////////// EOF /////////////////////////////////////////
