#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdlib.h>

// #define OLD_DRAG_AND_DROP 1 
#define DEBUG_OUTPUT_REQUESTED 1

extern unsigned int __internal_debug_setting;
#define DEBUG_ALL                0x11111111
#define DEBUG_OFF                0x00000000
#define DEBUG_PANELCONTAINERS    0x00000001
#define DEBUG_PANELS             0x00000010
#define DEBUG_FRAMES             0x00000100
#define DEBUG_DND                0x00001000
#define DEBUG_MESSAGES           0x00010000
#define DEBUG_PRINTING           0x00100000
#define DEBUG_SAVEAS             0x01000000

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
