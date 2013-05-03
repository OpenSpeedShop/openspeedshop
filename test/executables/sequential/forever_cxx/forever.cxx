/* A program that never finishes */
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

using namespace std;
void frank()
{
  int i, j;
  int t = 0;

  for(i=0; i<500; i++)
  {
    for(j=0; j<500; j++)
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

main()
{
  int i;
  int number = 10000;
  cout << getpid() << endl;

  for(i=0;i<number;i++)
    {
      foo();
      frank();
      bill();
    }
}
