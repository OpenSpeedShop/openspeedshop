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


int small_array[1000][1000];
struct Freddy freddy;

void
init()
{
  int i=0; int j=0;
  for(i=0;i < 1000;i++)
  {
    for(j=0;j < 1000;j++)
    {
      small_array[i][j] = -1;
    }
  }
}

int main(int argc, char **argv)
{
  int i = 1;

  printf("hello, from fred_calls_ted.\n");

  init();

  foo(&i);

  printf("fred_calls_ted is now retired.\n");
}
