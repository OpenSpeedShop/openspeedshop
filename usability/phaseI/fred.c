////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////


/* simpletest.c */
#include <stdio.h>
#include "fred.h"

extern void init();

void fred(int *val)
{
  int i = 0;
  freddy.x = 0;
  int j = 0;
  freddy.y = 0;
  int k = 0;


  for(i=0; i < 100; i++ )
  {
    for( j=0; j < 100; j++ )
    {
      for( k = 0;k < 100000;k++ )
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
