#include "allincs.h"

char CMDLINE[80];

//////////////////////// class MainWindow implementation ///////////////////////

MainWindow :: MainWindow(int nCmdShow, LPCSTR szWindowName, RECT rc)
            : Window(nCmdShow, szWindowName, rc)
{
    DoTgie();
}

MainWindow :: ~MainWindow(VOID)
{
}

VOID MainWindow :: DoTgie(VOID)
{
    TgieDialog *opTgie = new TgieDialog(IDD_TGIE_DIALOG,this,CMDLINE);
    delete opTgie;

    PostQuitMessage( 0 );
}

///////////////////////////////////////////////////////////////////////////////

UINT Application :: Run(VOID)
{
   strcpy(CMDLINE,lpszCmdLine);
   int n = (GetScreenWidth()>800) ? 5 : 4; // scale with 1.25 for hi res
   RECT WinDim = {0, 0, (n*328)/4, (n*286)/4}; // x,y,w,h
   opMain = new MainWindow( SW_HIDE, "Tgie", WinDim );
   return( Start() );
}
///////////////////////////////////// EOF /////////////////////////////////////
