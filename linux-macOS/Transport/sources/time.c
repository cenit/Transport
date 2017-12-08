//#include "f2c.h"
#include "stdlib.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void exit_(int *code)
{
   exit(*code);
}

void gettim_(char *tim)
{
   time_t t;
   char *ts;
   char *p;

   strcpy(tim,"        ");
   t = time(NULL);
   ts = ctime(&t);
   p = strtok(ts," ");
   p = strtok(NULL," ");
   p = strtok(NULL," ");
   p = strtok(NULL," ");
   if (p)
      strncpy(tim,p,strlen(p));
}

void getdat_(char *dat)
{
   time_t t;
   char *ts;
   char *p;
   char month[4], day[3], year[5], tim[9], d[10];

   t = time(NULL);
   ts = ctime(&t);
   p = strtok(ts," ");
   p = strtok(NULL," ");
   if (p)
      strcpy(month,p);
   p = strtok(NULL," ");
   if (p)
      strcpy(day,p);
   p  = strtok(NULL," ");
   if (p)
      strcpy(tim,p);
   p = strtok(NULL," ");
   if (p)
      strcpy(year,p);
   year[4] = '\0';

   sprintf(d,"%s-%s-%s",day,month,&year[2]);
   strcpy(dat,"         ");
   strncpy(dat,d,strlen(d));
}


