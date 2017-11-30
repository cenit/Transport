////////////////////////////////////////////////////////////////////////////////
///////////////////// Prototypes for Drawing UserButtons ///////////////////////
////////////////////////////////////////////////////////////////////////////////

extern "C"
{
  void APIENTRY DrawBitmap(HBRUSH hBrushB, LPDRAWITEMSTRUCT lpdis);

#ifdef WIN32
  int APIENTRY LibMain(HINSTANCE hInst,
                         DWORD ui_reason_being_called, LPVOID lpReserved);
#else
  int FAR PASCAL LibMain( HANDLE hInstance, UINT wDataSegment,
                                            UINT wHeapSize, LPSTR lpszCmdLine);
  int FAR PASCAL WEP ( int bSystemExit );
#endif
}
////////////////////////////////////// EOF /////////////////////////////////////

