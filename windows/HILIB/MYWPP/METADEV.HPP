////////////////////////////////////////////////////////////////////////////////
/////////////////////// Definition of class MetafileDevice /////////////////////
////////////////////////////////////////////////////////////////////////////////

class MetafileDevice : public DisplayDevice
{
    HDC hdcmf;
  protected:
    virtual HDC GetDC( void ) { return hdcmf; };
  public:
    MetafileDevice(char *filename,
                   int w = GetScreenWidth(), int h = GetScreenHeight());
    ~MetafileDevice(VOID);
    void SpecialIni(int, int, int, int);
    HMETAFILE Close(VOID);
    HMETAFILE Open(char *filename) { return 0; };
    void WriteClipboard(HWND hWnd, HMETAFILE hMF, int x, int y);
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////// Definition of class MemoryDevice //////////////////////
////////////////////////////////////////////////////////////////////////////////

class MemoryDevice : public DisplayDevice
{
    HDC hdc, hdcMem;
    HBITMAP hBitmap;
  public:
    virtual HDC GetDC( void ) { return hdcMem; };
    MemoryDevice(int w, int h);
    ~MemoryDevice(VOID);
};

//////////////////////////////////// EOF ///////////////////////////////////////
