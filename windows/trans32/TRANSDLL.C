#include <windows.h>
#include <stdio.h>
#include <setjmp.h>

/*
   Attention:
   ==========
   setjmp and longjmp don't work with Digital FORTRAN 
*/

jmp_buf env;
jmp_buf buffer;

short APIENTRY TRNPRT(short *mode, LPSTR str);
int   APIENTRY MYSETJMP(jmp_buf buf);
void  APIENTRY MYLONGJMP(jmp_buf buf, int *retval);
void  APIENTRY MYYIELD(int *tim);
short TRANS(short *mode, LPSTR str);

short APIENTRY TRNPRT(short *mode, LPSTR str)
{
    short retval;

//    int jval = setjmp(env);     // save registers
//    if (jval == 0)
//    {
       retval = TRANS(mode,str); // FORTRAN routine
//       longjmp(env,1);           // go to restore registers
//    }
    return retval;
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

int APIENTRY MYSETJMP(jmp_buf buf)
{
   int retval = setjmp(buf);
//   char temp[20];
//   sprintf(temp,"%ld",retval);
//   MessageBox(NULL,temp,"hallo",MB_OK);
   return retval;
}

void  APIENTRY MYLONGJMP(jmp_buf buf, int *retval)
{
//   char temp[20];
//   sprintf(temp,"%ld",buf);
//   MessageBox(NULL,temp,"hallo2",MB_OK);
   longjmp(buf,*retval);
//   MessageBox(NULL,temp,"hallo2",MB_OK);
}

INT  APIENTRY LibMain(HANDLE hInst, 
                      DWORD ul_reason_being_called, LPVOID lpReserved)
{
    return 1;
	UNREFERENCED_PARAMETER(hInst);
	UNREFERENCED_PARAMETER(ul_reason_being_called);
	UNREFERENCED_PARAMETER(lpReserved);
}

///////////////////////////////////// EOF /////////////////////////////////////
