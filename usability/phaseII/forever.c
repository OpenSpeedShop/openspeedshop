/* A program that never finishes */
#include <stdio.h>

void frank()
{
  int i, j;
  int t = 0;

  for(i=0; i<1000; i++)
  {
    for(j=0; j<1000; j++)
    {
      t += i*j;
    }
  }
}

void foo()
{
  int i, j;
  int t = 0;

  for(i=0; i<1000; i++)
  {
    for(j=0; j<1000; j++)
    {
      t += i*j;
    }
  }
}

void bill()
{
  int i, j, k;
  int t = 0;

  for(i=0; i<100; i++)
  {
    for(j=0; j<100; j++)
    {
      for(k=0; k<1000; k++)
      {
        t += i*j*k;
      }
    }
  }
}

main(int argc, char **argv)
{
  int i;
  int number = 100;
  char input[1024];

  for(i=0;i<argc;i++)
  {
    printf("argv[%d]=%s\n", i, argv[i]);
  }
  
  printf("Started a program that never finishes.... pid=%d\n", getpid() );
  while( number > 0 )
  {
    for(i=0;i<number;i++)
    {
      foo();
      frank();
      bill();
//      sleep(1);
      printf(".");
      fflush(stdout);
    }
    input[0] = '\0';
    printf("\nGive me a number: ");
    scanf("%s", input);
    number = atoi(input);
    printf("Input was %d\n", number);
  }
  printf("End of life for the forever program....\n");
}
