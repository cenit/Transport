#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


void timer( void(*func)(void), int time )
{
   printf("%d\n", signal ( SIGALRM, (void (*)) func ));
   alarm ( time );
}

void mess(void)
{
   printf("hallo\n");
}

int main()
{
    int i;

    printf("starting\n");
    timer ( mess, 3 );
    for (i=0; i<100; i++)
    { 
        usleep(100000);
        printf("%d\n",i);
    }
    printf("exiting\n");
    return 1;
}


