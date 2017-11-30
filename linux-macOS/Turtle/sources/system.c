/* f77 interface to system routine */

//#include "f2c.h"
#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "curses.h"

random_(float *rnd)
{
   long rn;

   rn = rand();
   *rnd = (float)rn / (float)RAND_MAX;
}

seed_(long *seed)
{
   srand(*seed);
}

getsecs_(long *secs)
{
   time_t t;
   t = time(NULL);
   *secs = t;
}

putcha_(int *c)
{
   putchar(*c);
}

getcha_(int *c)
{
   int cc;

   cc = getchar();
   *c = cc;
}

exit_(int *code)
{
   exit(*code);
}

gtext_(int *x, int *y, char *line)
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
