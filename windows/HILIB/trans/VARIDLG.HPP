///////////////////////////////////////////////////////////////////////////////
////////////////////////// VariDialog class definition ////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef VARIDLG_HPP
#define VARIDLG_HPP

class TransGrafFile;
class BeamInfoDialog;

class VariDialog : public ModalDialog
{
     int result;
     float Field;
     StringEdit *opVari;
     int Posx, Posy, PosWx, PosWy;
     char Zst[30], Titel[20], Label[20], Leff[30], B[30], A[30];
     StaticText *opZst, *opLeff, *opB, *opA;
     CheckBox *opRun, *opRef;
     PushButton *opMore;
     TransGrafFile *opGrafFile;
     BOOL OK;
     BeamInfoDialog *opBID;
   protected:
     long Control(UINT, UINT );
     BOOL Init(VOID);
     VOID TransferParameters(VOID);
   public:
     VariDialog(int ResID, Window *opParent,
                int, int, int, int,
                float, TransGrafFile *);
     int GetResult(VOID);
     float GetField(VOID);
     ~VariDialog(VOID);
};

///////////////////////////////////////////////////////////////////////////////
//////////////////////// BeamInfoDialog class definition //////////////////////
///////////////////////////////////////////////////////////////////////////////

class BeamInfoDialog : public ModelessDialog
{
     ListBox *opList;
     int Posx, Posy;
     int Sizex, Sizey;
     TransGrafFile *opGrafFile;
     BOOL open;
   protected:
     BOOL Init(VOID);
     long WmSysMenuCommand(WPARAM );
   public:
     BeamInfoDialog(int ResID, Dialog *opParent,
                    int, int, int, int, TransGrafFile *);
     BOOL IsOpen(VOID);
     ~BeamInfoDialog(VOID);
};

#endif

///////////////////////////////// EOF /////////////////////////////////////////
