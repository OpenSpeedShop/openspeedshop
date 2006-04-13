
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int i, n;

    if(argc != 2) {
	printf("Usage: %s <second>\n", argv[0]);
	exit(1);
    }

    n = atoi(argv[1]);

    for(i = 0; i < n; i++) {
	sleep(1);
	printf("i = %d\n", i);
    }		

}
