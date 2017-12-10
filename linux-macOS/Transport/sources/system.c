/* f77 interface to system routine */

//#include "f2c.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

void gettim_(long *secs)
{
   time_t t;
   t = time(NULL);
   *secs = t;
}

void putcha_(int *c)
{
   putchar(*c);
}

void exit_(int *code)
{
   exit(*code);
}

