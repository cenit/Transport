///////////////////////////////////////////////////////////////////////////////
/////////////////////// Definition of class HplotDialog  //////////////////////
///////////////////////////////////////////////////////////////////////////////

class HplotDialog : public ModalDialog
{
     LPCSTR File;
     BOOL result, Initialized;
     HWND hPar;
     int Index;
     int Nhists, Nh, Ncont;
     int Norays;
     int Phi, Theta;
     StaticText *opText, *opUTI;
     RadioButton *opLin, *opLog;
     RadioButton *opMon, *opCol;
     RadioButton *opIncr, *opNone, *opDecr;
     RadioButton *opP1, *opP2, *opP3, *opP4;
     RadioButton *opC1, *opC2, *opCC, *opCS;
     RadioButton *opL, *opL1, *opL2;
     RadioButton *opS, *opS1, *opS2, *opS3, *opS4;
     CheckBox *opStat;
     StringEdit *opNHist, *opNCont, *opPhi, *opTet;
     ScrollBar *opNumbSB, *opNContSB, *opPhiSB, *opTetSB;
     StringEdit *opX, *opY, *opXZ, *opYZ;
     StringEdit *opHi1, *opZo1, *opPi1;
     StringEdit *opHi3, *opZo3, *opPi3;
     char szNHist[5], szNCont[5], szPhi[5], szTet[5];

   protected:
     BOOL WmTimer(UINT);
     BOOL Init   ( void );
     long Control( UINT nControlId, UINT nNotificationCode );

   public:
     HplotDialog( int ResId, Window *opWin, LPCSTR file, int, int);
     ~HplotDialog();
     BOOL GetResult( void );
};
