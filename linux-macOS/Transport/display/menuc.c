#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int menu_(float *x1, float *y1, int *nx, int *ny, char *menu, int *kx, int len)
{
   char *item;

   item = (char *) malloc(len+1);
   strncpy(item,&menu[len],len);
   item[len] = '\0';
   printf("%d %s\n",len, item);
   return (*kx);
}
