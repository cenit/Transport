/* f77 interface to system routine */

//#include "f2c.h"
#include "stdlib.h"
#include "time.h"

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

gettim_(long *secs)
{
   time_t t;
   t = time(NULL);
   *secs = t;
}

putcha_(int *c)
{
   putchar(*c);
}

exit_(int *code)
{
   exit(*code);
}

