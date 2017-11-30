#include <windows.h>

char BufferName[100];

typedef struct
{
   int percent;
} CB;

CB     *cb;
HANDLE hMem;
BOOL   first = TRUE;

short TURTLEF(LPSTR str);
short PERCENTF(short *per);
void  FINISHF(void);
void  APIENTRY PUTPERCENT(int *per);
short APIENTRY TURTLE(LPSTR str);
short APIENTRY PERCENT(short *per);
void  APIENTRY FINISH(void);
void  APIENTRY MYYIELD(int *tim);
void  APIENTRY MYMESSAGEBOX(LPSTR str, int len);

void  APIENTRY PUTPERCENT(int *per)
{
   // fortran writes "%" in shared memory

   if (first)
   {
      first = FALSE;
      hMem  = CreateFileMapping((HANDLE)0xffffffff,NULL,PAGE_READWRITE,0,
                                  sizeof(CB),BufferName);
      if (hMem)
         cb = (CB *)MapViewOfFile(hMem,FILE_MAP_ALL_ACCESS,0,0,0);
      else
         cb = NULL;
   }
   if (cb)
      cb->percent = *per;
}

short APIENTRY TURTLE(LPSTR str, LPSTR bname)
{   
    short retval;
    strcpy(BufferName,bname);
    retval = TURTLEF(str);
    return retval;
}

short APIENTRY PERCENT(short *per)
{
    return PERCENTF(per);
}

void APIENTRY FINISH(void)
{
    FINISHF();
}

void APIENTRY MYYIELD(int *dummy)
{
   MSG msg;

   while (PeekMessage(&msg, 0, 0, 0, TRUE))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
}

void APIENTRY MYMESSAGEBOX(LPSTR mess, int len)
{
    MessageBox(NULL,mess,"TURLEDLL",MB_OK);
}

INT  APIENTRY LibMain(HANDLE hInst, 
                      DWORD ul_reason_being_called, LPVOID lpReserved)
{
    switch (ul_reason_being_called)
    {
       case DLL_PROCESS_ATTACH:
       case DLL_THREAD_ATTACH:
          break;
       case DLL_PROCESS_DETACH:
       case DLL_THREAD_DETACH:
  	 break;
       default: break;
    }
    
    return 1;

	UNREFERENCED_PARAMETER(hInst);
	UNREFERENCED_PARAMETER(ul_reason_being_called);
	UNREFERENCED_PARAMETER(lpReserved);
}

///////////////////////////////////// EOF /////////////////////////////////////

