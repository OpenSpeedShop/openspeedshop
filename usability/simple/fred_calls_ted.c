/* simpletest.c */
#include <stdio.h>
#include "fred.h"


int small_array[1000][1000];
struct Freddy freddy;

int main(int argc, char **argv)
{
  int i = 1;

  foo(&i);

  printf("Set a breakpoint at this line!\n");

  printf("The final value of small_array[9,9]=%d\n", small_array[9][9]);
}
