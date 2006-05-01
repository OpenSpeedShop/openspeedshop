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


#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdlib.h>

// #define OLD_DRAG_AND_DROP 1 
#define DEBUG_OUTPUT_REQUESTED 1

extern unsigned int __internal_debug_setting;
#define DEBUG_ALL                0xffff
#define DEBUG_OFF                0x0000
#define DEBUG_PANELCONTAINERS    (1<<1)
#define DEBUG_PANELS             (1<<2)
#define DEBUG_FRAMES             (1<<3)
#define DEBUG_DND                (1<<4)
#define DEBUG_MESSAGES           (1<<5)
#define DEBUG_CLIPS              (1<<6)
#define DEBUG_COMMANDS           (1<<7)
#define DEBUG_CONST_DESTRUCT     (1<<8)

enum DebugKind { MaximumDebugKind = 31 };
#ifdef DEBUG_OUTPUT_REQUESTED
inline bool debug( int debugKind )
{
  bool ret_value = __internal_debug_setting & debugKind;

//  printf("debug(%d) __internal_debug_setting=%d called returning=(%d)\n", debugKind, __internal_debug_setting, ret_value );

  return( ret_value );
}

#define nprintf(kind) if( debug(kind) ) printf

#define dprintf if( __internal_debug_setting ) printf
#else
static void
noop_printf(...)
{
}

#define dprintf noop_printf
#define nprintf(arg) noop_printf
inline bool debug( int arg )
{
  return( 0 );
}
#endif // DEBUG_OUTPUT_REQUESTED

#endif // DEBUG_H
