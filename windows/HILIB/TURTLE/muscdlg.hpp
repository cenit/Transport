///////////////////////////////////////////////////////////////////////////////
/////////////////////// Definition of class MuscatDialog //////////////////////
///////////////////////////////////////////////////////////////////////////////

class MuscatDialog : public ModalDialog
{
     LPCSTR File, File2;
     BOOL result;
     HWND hPar;
     int Index;
     StringEdit *opMass;
     StringEdit *opSiga;
     StringEdit *opSige;
     StringEdit *opP0;
     StringEdit *opRho;
     StringEdit *opFrac;
     StringEdit *opLen;
     StringEdit *opZ1;
     StringEdit *opProp1;
     StringEdit *opZ2;
     StringEdit *opProp2;
     StringEdit *opZ3;
     StringEdit *opProp3;
   protected:
     BOOL Init   ( void );
     long Control( UINT nControlId, UINT nNotificationCode );
     BOOL GetResult( void );
     VOID Store(LPCSTR);
     BOOL Loaded(LPCSTR);
     BOOL WmTimer(UINT);
   public:
     MuscatDialog( int ResId, Window *opWin, LPCSTR file, LPCSTR file2);
     ~MuscatDialog();
};
