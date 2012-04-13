/*******************************************************************************
 * ** Copyright (c) 2012 Krell Institute All Rights Reserved.
 * **
 * ** This library is free software; you can redistribute it and/or modify it under
 * ** the terms of the GNU Lesser General Public License as published by the Free
 * ** Software Foundation; either version 2.1 of the License, or (at your option)
 * ** any later version.
 * **
 * ** This library is distributed in the hope that it will be useful, but WITHOUT
 * ** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * ** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * ** details.
 * **
 * ** You should have received a copy of the GNU Lesser General Public License
 * ** along with this library; if not, write to the Free Software Foundation, Inc.,
 * ** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * *******************************************************************************/


#include <hwi/include/common/uci.h>
#include <spi/include/kernel/process.h>
#include <spi/include/kernel/location.h>
#include <stdio.h>

//#include <common/bgq_personality.h>
//#include <common/bgq_personality_inlines.h>
//#include <spi/kernel_interface.h>

#define BGQ_GROUP_ON_NODEBOARD

#if 0
static _BGQ_Personality_t oss_bgq_personality;

void oss_bgq_platform_personality_init() {

  Kernel_GetPersonality(&oss_bgq_personality, sizeof(_BGQ_Personality_t));

}


 /* Get unique numeric SMP-node identifier */
long oss_bgq_platform_personality_node_id() {
#ifdef BGQ_GROUP_ON_NODEBOARD
  _BGQ_UniversalComponentIdentifier uci;
  uci.UCI = oss_bgq_personality.Kernel_Config.UniversalComponentIdentifier;
  /* use upper part of UCI (upto NodeCard, ignore lower 14bits)
    ** but only use the 13 bits (1FFF) that describe row,col,mp,nc */
  return ((uci.UCI>>14)&0x1FFF);
#else
  return ( BGQ_Personality_psetNum(&oss_bgq_personality) *
           BGQ_Personality_psetSize(&oss_bgq_personality) +
           BGQ_Personality_rankInPset(&oss_bgq_personality)) * Kernel_ProcessCount()
           + Kernel_PhysicalProcessorID();
#endif
}
#endif

static void oss_bgq_platform_personality_getNodeidString(char *buf) {

    Personality_t pers;
    BG_UniversalComponentIdentifier uci;
    Kernel_GetPersonality( &pers, sizeof(pers) );
    char ucStrg[128];
    uci = pers.Kernel_Config.UCI;
    if (bg_uci_toString( uci, ucStrg) < 0) {
        ucStrg[0] = '\0';
    }
    buf = &ucStrg;
    fprintf(stdout, "buf=%s, ucStrg=%s, core ID(%d) Thread ID(%d)\n",
                buf, ucStrg, Kernel_ProcessorCoreID(), Kernel_ProcessorThreadID());


}

/* unique string SMP-node identifier */
char* oss_bgq_platform_personality_node_name() {
  static char buf[128];
  oss_bgq_platform_personality_getNodeidString(buf);
  return buf;
}

/* number of CPUs */
int oss_bgq_platform_personality_num_cpus() {
#ifdef BGQ_GROUP_ON_NODEBOARD
  return 32 * Kernel_ProcessCount();
#else
  return 1;
#endif
}
