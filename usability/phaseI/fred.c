/* simpletest.c */
#include <stdio.h>
#include "fred.h"

extern void init();

void fred(int *val)
{
  int i = 0;
  int j = 0;
  int k = 0;
  freddy.x = 0;
  freddy.y = 0;


  for(i=0; i < 10; i++ )
  {
    for( j=0; j < 10; j++ )
    {
      for( k = 0;k < 10000;k++ )
      {
        small_array[i][j] = (freddy.y * freddy.x)/(j+1)+i;
      }
      if( freddy.y >= 25 )
      {
        break;
      }
      freddy.x += ted(i);  
      small_array[i][j] = freddy.x;
      freddy.y++;
    }
  }

  (*val) = freddy.x;

  return;
}
