////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Camac Handler Prototypes /////////////////////////
////////////////////////////////////////////////////////////////////////////////

extern "C"
{
  void  APIENTRY ShowCamacDLLVersion(HWND);
  int   APIENTRY IniDevList(LPSTR);
  int   APIENTRY ReloadDevList(LPSTR);
  int   APIENTRY SetDac(LPSTR, long);
  int   APIENTRY SetDacWait(LPSTR, long);
  long  APIENTRY GetDac(LPSTR);
  long  APIENTRY GetAdc(LPSTR);
  int   APIENTRY GetDevName(int, LPSTR);
  int   APIENTRY GetDevParam(int, LPSTR, long &, long &, float &, float &,
                             float &, int &, char &, int &);
  int   APIENTRY GetDevParam2(int, LPSTR, LPSTR, LPSTR, LPSTR, LPSTR, LPSTR,
                              LPSTR, int &, int &);
  int   APIENTRY DelDevList(void);
  int   APIENTRY IniSerialComm(LPSTR);
  int   APIENTRY CloseSerialComm(void);
  int   APIENTRY IniKSI(void);
  int   APIENTRY WriteCamac(LPSTR, LPSTR, LPSTR, long);
  int   APIENTRY ReadCamac(LPSTR, LPSTR, LPSTR, long &);
  int   APIENTRY ControlCamac(LPSTR, LPSTR, LPSTR);
  int   APIENTRY ReadCamacDac(LPSTR, LPSTR, long &);
  int   APIENTRY WriteCamacDac(LPSTR, LPSTR, long );
  int   APIENTRY getq(void);
  int   APIENTRY getx(void);
  int   APIENTRY WriteDAC(int, LPSTR, LPSTR, LPSTR, long);
  int   APIENTRY ReadDAC(int, LPSTR, LPSTR, LPSTR, long &);
  int   APIENTRY ReadADC(int, LPSTR, LPSTR, LPSTR, int, int, float &);
  int   APIENTRY ReadScaler(LPSTR, LPSTR, long &);
  int   APIENTRY ClearScaler(LPSTR, LPSTR);  
  int   APIENTRY ReserveCrate(void);
  int   APIENTRY FreeCrate(void);
  int   APIENTRY SwitchOnCombis(void);
  int   APIENTRY SwitchOnCombi(LPSTR);
  int   APIENTRY SwitchOffCombi(LPSTR);
  int   APIENTRY GetNumberOfPages(void);
  int   APIENTRY GetPageIndex(int);
  void  APIENTRY YieldQQ(void);
  void  APIENTRY wait(int);
  long  APIENTRY ReadAll(LPSTR);
#ifdef WIN32
  int  APIENTRY LibMain(HANDLE hInst, 
                        DWORD ul_reason_being_called, LPVOID lpReserved);
#else
  int FAR PASCAL LibMain( HANDLE hInstance, UINT wDataSegment,
                                            UINT wHeapSize, LPSTR lpszCmdLine );
  int FAR PASCAL WEP ( int bSystemExit );
#endif
}

////////////////////////////////////// EOF /////////////////////////////////////
