#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
   extern int work(int);
   int size = 750;

    if(argc != 2) {
        printf("Usage: %s <size>\n", argv[0]);
        printf("No size argument given.   Defaulting to %d.\n", size);
    } else
    {
      size = atoi(argv[1]);
    }

    MPI_Init(&argc,&argv);
    
    work(size);

    MPI_Finalize();

    printf("%s: succesfully competed.\n", argv[0]);
}
