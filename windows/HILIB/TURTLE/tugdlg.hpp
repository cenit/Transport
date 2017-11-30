///////////////////////////////////////////////////////////////////////////////
///////////////////////// TugDialog class definition /////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef TUGDLG_HPP
#define TUGDLG_HPP

enum SCLTYP {LIN,LOG};
enum PLOTYP {CONT,LEGO,GRID};
enum VIEWDI {FRONT,BACK};
enum INCTYP {INCR,NONE,DECR};

typedef struct
{
   int nh;
   int nniv;
   int phi;
   int tet;
   int phil;
   int tetl;
   PLOTYP plotyp;
   VIEWDI view;   
   SCLTYP scltyp;
   INCTYP incr;
} TUGDATA;

class TugDialog : public ModalDialog
{
     HWND hPar;
     BOOL result, Initialized, statistics;
     StaticText *opText;
     int Nhists;
     LONG Norays;
     TUGDATA *TugData;
     PushButton *opOK;
     RadioButton *opCont, *opLego, *opGrid;
     RadioButton *opFront, *opBack;
     RadioButton *opLin, *opLog;
     RadioButton *opIncr, *opNone, *opDecr;
     CheckBox *opStat;
     StringEdit *opNHist, *opNCont, *opPhi, *opTet;
     ScrollBar *opNumbSB, *opNContSB, *opPhiSB, *opTetSB;
     char szNHist[5], szNCont[5], szPhi[5], szTet[5];
     int xP, yP;
   protected:
     BOOL Init( VOID );
     long Control( UINT nControlId, UINT nNotificationCode );
     VOID TransferParameters(VOID);
   public:
     TugDialog(int ResId, Window *opParent, int, LONG, TUGDATA *);
     ~TugDialog(VOID);
     BOOL GetResult(VOID);
};

///////////////////////////////////////////////////////////////////////////////
//////////////////////// LightDialog class definition /////////////////////////
///////////////////////////////////////////////////////////////////////////////

class LightDialog : public ModalDialog
{
     HWND hPar;
     BOOL result, Initialized;
     StringEdit *opPhi, *opTet;
     char szPhi[6], szTet[6];
     int Phi, Tet;
     RotKnob *opKnob[2];
     int KnobVal[2], KnobMax[2], KnobMin[2], KnobPage[2];
     int xPos, yPos;
   public:
     LightDialog(int ResId, TugDialog *opParent, int Phi, int Theta);
     ~LightDialog( VOID );
     BOOL Init( VOID );
     long Control( UINT nControlId, UINT nNotificationCode );
     void GetSizes(UINT nControlId, int &LineSize, int &PageSize);
     VOID GetParams( int &phi, int &theta );
     BOOL GetResult( VOID );
};

#endif
///////////////////////////////// EOF /////////////////////////////////////////
