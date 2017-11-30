#include <unistd.h>

void mysleep_(long *delay)
{
  usleep((*delay)*1000);
}
