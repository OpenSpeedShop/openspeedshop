#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdlib.h>

// #define OLD_DRAG_AND_DROP 1 
#define DEBUG_OUTPUT_REQUESTED 1

extern unsigned int __internal_debug_setting;
#define DEBUG_ALL                0xffffffff
#define DEBUG_OFF                0x00000000
#define DEBUG_PANELCONTAINERS    (1<<1)
#define DEBUG_PANELS             (1<<2)
#define DEBUG_FRAMES             (1<<3)
#define DEBUG_DND                (1<<4)
#define DEBUG_MESSAGES           (1<<5)
#define DEBUG_PRINTING           (1<<6)
#define DEBUG_SAVEAS             (1<<7)

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
