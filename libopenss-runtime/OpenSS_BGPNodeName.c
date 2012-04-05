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

#if defined(TARGET_OS_BGP) 
#include <stdio.h>
#include <common/bgp_personality.h>
#include <common/bgp_personality_inlines.h>
#include <spi/kernel_interface.h>

#define BGP_GROUP_ON_NODEBOARD

static _BGP_Personality_t oss_bgp_personality;

void oss_bgp_platform_personality_init() {

  Kernel_GetPersonality(&oss_bgp_personality, sizeof(_BGP_Personality_t));

}


 /* Get unique numeric SMP-node identifier */
long oss_bgp_platform_personality_node_id() {
#ifdef BGP_GROUP_ON_NODEBOARD
  _BGP_UniversalComponentIdentifier uci;
  uci.UCI = oss_bgp_personality.Kernel_Config.UniversalComponentIdentifier;
  /* use upper part of UCI (upto NodeCard, ignore lower 14bits)
    ** but only use the 13 bits (1FFF) that describe row,col,mp,nc */
  return ((uci.UCI>>14)&0x1FFF);
#else
  return ( BGP_Personality_psetNum(&oss_bgp_personality) *
           BGP_Personality_psetSize(&oss_bgp_personality) +
           BGP_Personality_rankInPset(&oss_bgp_personality)) * Kernel_ProcessCount()
           + Kernel_PhysicalProcessorID();
#endif
}

static void oss_bgp_platform_personality_getNodeidString(const _BGP_Personality_t *p, char *buf) {
   _BGP_UniversalComponentIdentifier uci;
  uci.UCI = p->Kernel_Config.UniversalComponentIdentifier;

  if ((uci.ComputeCard.Component == _BGP_UCI_Component_ComputeCard) ||
      (uci.IOCard.Component == _BGP_UCI_Component_IOCard)) {

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
char* oss_bgp_platform_personality_node_name() {
#ifdef BGP_GROUP_ON_NODEBOARD
  static char buf[BGPPERSONALITY_MAX_LOCATION];
  bgp_getNodeidString(&oss_bgp_personality, buf);
  return buf;
#else
  static char node[128];
  unsigned x = BGP_Personality_xCoord(&oss_bgp_personality);
  unsigned y = BGP_Personality_yCoord(&oss_bgp_personality);
  unsigned z = BGP_Personality_zCoord(&oss_bgp_personality);

  sprintf(node, "node-%03d-%03d-%03d-%d", x, y, z, Kernel_PhysicalProcessorID());
  return node;
#endif
}

/* number of CPUs */
int oss_bgp_platform_personality_num_cpus() {
#ifdef BGP_GROUP_ON_NODEBOARD
  return 32 * Kernel_ProcessCount();
#else
  return 1;
#endif
}

#endif

