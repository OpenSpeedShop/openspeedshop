
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void mysleep(int seconds)
{
    int i, sum = 1;
     
    for(i = 1; i < seconds * 50000000; ++i)
        sum *= i;
}
 
int main(int argc, char* argv[])
{
    int i, n;
 
    if(argc != 2) {
        printf("Usage: %s <second>\n", argv[0]);
        exit(1);
    }
     
    n = atoi(argv[1]);
     
    for(i = 0; i < n; i++) {
        mysleep(1);
        printf("i = %d\n", i);
    }

    /* Temporary code to help verify mutatee actually runs */
    char fname[256] = "/tmp/mutatee.XXXXXX";
    mktemp(fname);
    FILE* f = fopen(fname, "w");
    fclose(f);
    
}
