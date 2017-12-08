#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void timestamp_(long *dummy)
{
   struct stat stbuf;

   stat("./FOR022.DAT",&stbuf);
   printf(" FOR022.DAT has been fetched from ACS on %s\n",ctime(&stbuf.st_ctime));
}

void sleep_(long *delay)
{
  sleep((unsigned)*delay);
}
