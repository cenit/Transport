///////////////////////////////////////////////////////////////////////////////
/////////////////////// MeterDialog class implementation //////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "allincs.h"

MeterDialog :: MeterDialog(int ResID, Window *opParent, HWND hPar2)
             : ModelessDialog(ResID,hPar2)
{
    hPar = opParent->GetHandle();
    Start();
}

MeterDialog :: ~MeterDialog(VOID)
{
    delete opMeter;
    TerminateDialog();   // important, otherwise crash
}

BOOL MeterDialog :: Init(VOID)
{
    CenterDialog(hPar,this->GetHandle());

    opMeter = new AmpMeter(IDC_ANALOG,this);
    opMeter->SetText("%;0;;20;;40;;60;;80;;100");
    opMeter->SetRange(0,100);

    return FALSE;
}

VOID MeterDialog :: SetValue(int val)
{
    opMeter->SetValue(val);
}

///////////////////////////////// EOF //////////////////////////////////////
