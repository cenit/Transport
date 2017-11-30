////////////////////////////////////////////////////////////////////////////////
//////////////////////// Definition of class Printer ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

extern "C"
{
   BOOL APIENTRY PrintAbortProc(HANDLE, UINT);
}

class Printer : public DisplayDevice
{
    char caPrinterName[40];
    char caPrinterPort[40];
    char caPrinterDriver[40];
    BOOL blError;
    HDC  hdc;
  protected:
    VOID ReleaseDeviceDC(HANDLE hDevC);
  public:
    Printer(VOID);
    Printer(char *caName, char *caDriver, char *caPort);
   ~Printer(VOID);
    HDC  GetDC(VOID) { return hdc; };
    BOOL Start(char *cpDocName, Window *opWin);
    BOOL StartPrintToFile(char *cpDocName, Window *opWin);
    BOOL NextPage(VOID);
    BOOL End(Window *opWin);
    VOID Abort(VOID);
    BOOL Setup(Window *opWin);
};

//////////////////////////////////// EOF ///////////////////////////////////////
