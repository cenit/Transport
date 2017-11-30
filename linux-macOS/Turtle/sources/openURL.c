#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  int i, retval;
  char command[80];
  static char *browsers[] = {"netscape", "mozilla", "kfmclient openURL"};

  if (argc != 2)
  {
    printf("Usage: openURL someURL\n");
    printf("Tries to use following browsers: netscape, mozilla or kde-browser.\n");
    return 1;
  }

  for (i=0; i<3; i++)
  {
     strcpy(command,browsers[i]);
     strcat(command," ");
     strcat(command,argv[1]);
     strcat(command," >/dev/null 2>&1");
     retval = system(command);
     if (retval == 0) break;
  }
  return 0;
}
