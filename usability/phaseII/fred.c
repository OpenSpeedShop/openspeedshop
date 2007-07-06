#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
   extern int work(int);
   int size = 750;
   int i = 0;
   int segment_count  = 1;

    if( argc == 2 ) {

      size = atoi(argv[1]);
      printf("Size argument given using %d.\n", size);

    } else if( argc == 3 ) {

      size = atoi(argv[1]);
      printf("Size argument given using %d.\n", size);
      segment_count = atoi(argv[2]);
      printf("Segment count argument given using %d.\n", segment_count);

    } else if(argc != 2 && argc != 3 )
    {
      printf("Usage: %s <size> <segment_count>\n", argv[0]);
      printf("No size argument given.   Defaulting to %d.\n", size);
    } 
    

if( segment_count == 1 )
{
    work(size);
} else
{
  while( segment_count > 0 )
  {
    for(i=0; i<segment_count;i++)
    {
printf("call work(%d) %d\n", size, i);
      work(size);
    }
    segment_count--;
  }
}

    printf("%s: succesfully completed.\n", argv[0]);
}
