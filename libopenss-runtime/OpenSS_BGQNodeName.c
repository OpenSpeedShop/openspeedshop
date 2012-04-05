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


#if defined(TARGET_OS_BGQ) 
#include <stdio.h>
#include <common/bgq_personality.h>
#include <common/bgq_personality_inlines.h>
#include <spi/kernel_interface.h>

#define BGQ_GROUP_ON_NODEBOARD

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

static void oss_bgq_platform_personality_getNodeidString(const _BGQ_Personality_t *p, char *buf) {
   _BGQ_UniversalComponentIdentifier uci;
  uci.UCI = p->Kernel_Config.UniversalComponentIdentifier;

  if ((uci.ComputeCard.Component == _BGQ_UCI_Component_ComputeCard) ||
      (uci.IOCard.Component == _BGQ_UCI_Component_IOCard)) {

    unsigned row = uci.ComputeCard.RackRow;
    unsigned col = uci.ComputeCard.RackColumn;
    unsigned mp  = uci.ComputeCard.Midplane;
    unsigned nc  = uci.ComputeCard.NodeCard;

    if (row == 0xff)
      sprintf(buf, "Rxx-Mx-N%x", nc);
    else
      sprintf(buf, "R%x%x-M%d-N%x", row, col, mp, nc);
  } else {
    sprintf(buf, "R?\?-M?-N?");
  }
}

/* unique string SMP-node identifier */
char* oss_bgq_platform_personality_node_name() {
#ifdef BGQ_GROUP_ON_NODEBOARD
  static char buf[BGQPERSONALITY_MAX_LOCATION];
  bgq_getNodeidString(&oss_bgq_personality, buf);
  return buf;
#else
  static char node[128];
  unsigned x = BGQ_Personality_xCoord(&oss_bgq_personality);
  unsigned y = BGQ_Personality_yCoord(&oss_bgq_personality);
  unsigned z = BGQ_Personality_zCoord(&oss_bgq_personality);

  sprintf(node, "node-%03d-%03d-%03d-%d", x, y, z, Kernel_PhysicalProcessorID());
  return node;
#endif
}

/* number of CPUs */
int oss_bgq_platform_personality_num_cpus() {
#ifdef BGQ_GROUP_ON_NODEBOARD
  return 32 * Kernel_ProcessCount();
#else
  return 1;
#endif
}

#endif

