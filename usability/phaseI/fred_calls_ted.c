/* simpletest.c */
#include <stdio.h>
#include "fred.h"


int small_array[1000][1000];
struct Freddy freddy;

void
init()
{
  int i=0; int j=0;
  for(i=0;i<1000;i++)
  {
    for(j=0;j<1000;j++)
    {
      small_array[i][j] = -1;
    }
  }
}

int main(int argc, char **argv)
{
  int i = 1;

  init();

  foo(&i);
}
