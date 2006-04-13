////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <inttypes.h>

bool GLOBAL_bool;

int8_t GLOBAL_int8_t;
int16_t GLOBAL_int16_t;
int32_t GLOBAL_int32_t;
int64_t GLOBAL_int64_t;

uint8_t GLOBAL_uint8_t;
uint16_t GLOBAL_uint16_t;
uint32_t GLOBAL_uint32_t;
uint64_t GLOBAL_uint64_t;

char GLOBAL_char;
int GLOBAL_int;
long GLOBAL_long;
long long GLOBAL_long_long;

float GLOBAL_float;
double GLOBAL_double;

char* GLOBAL_char_ptr;
char GLOBAL_char_array[4];

char& GLOBAL_char_ref = GLOBAL_char;

enum { Red = 0, Green = 1, Blue = 2 } GLOBAL_enum;
struct { int x; int y; } GLOBAL_struct;
union { int x; double y; } GLOBAL_union;

typedef int Type1;
typedef Type1 Type2;
Type2 GLOBAL_nested_type;

int main(int argc, char* argv[])
{
}
