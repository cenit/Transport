/* f77 interface to system routine */

//#include "f2c.h"
#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "curses.h"

void random_(float *rnd)
{
   long rn;

   rn = rand();
   *rnd = (float)rn / (float)RAND_MAX;
}

void seed_(long *seed)
{
   srand(*seed);
}

void getsecs_(long *secs)
{
   time_t t;
   t = time(NULL);
   *secs = t;
}

void putcha_(int *c)
{
   putchar(*c);
}

void getcha_(int *c)
{
   int cc;

   cc = getchar();
   *c = cc;
}

void exit_(int *code)
{
   exit(*code);
}

void gtext_(int *x, int *y, char *line)
{
   
   // uses curses routines: initscr, move, addstr and refresh
   // see manual pages "ncurses"
   
   static int first = 1;
   
   if (first)
   {
      initscr();
      first = 0;
   }
   move( *y, *x );
   addstr( line );
   refresh();
}
