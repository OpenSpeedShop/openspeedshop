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

#include <stdio.h>
#include <common/bgp_personality.h>
#include <common/bgp_personality_inlines.h>
#include <spi/kernel_interface.h>

#define BGP_GROUP_ON_NODEBOARD

static _BGP_Personality_t oss_bgp_personality;

void oss_bgp_platform_personality_init() {

  Kernel_GetPersonality(&oss_bgp_personality, sizeof(_BGP_Personality_t));
  //fprintf(stdout, "oss_bgp_personality=0x%llx\n", oss_bgp_personality);

}

 /* Get unique numeric SMP-node identifier */
long oss_bgp_platform_personality_node_id() {
#ifdef BGP_GROUP_ON_NODEBOARD
  _BGP_UniversalComponentIdentifier bgp_uci;
  bgp_uci.UCI = oss_bgp_personality.Kernel_Config.UniversalComponentIdentifier;
  /* use upper part of UCI (upto NodeCard, ignore lower 14bits)
    ** but only use the 13 bits (1FFF) that describe row,col,mp,nc */
  return ((bgp_uci.UCI>>14)&0x1FFF);
#else
  return ( BGP_Personality_psetNum(&oss_bgp_personality) *
           BGP_Personality_psetSize(&oss_bgp_personality) +
           BGP_Personality_rankInPset(&oss_bgp_personality)) * Kernel_ProcessCount()
           + Kernel_PhysicalProcessorID();
#endif
}

static void oss_bgp_platform_personality_getNodeidString(const _BGP_Personality_t *p, char *buf) {
   _BGP_UniversalComponentIdentifier bgp_uci;
  bgp_uci.UCI = p->Kernel_Config.UniversalComponentIdentifier;

  if ((bgp_uci.ComputeCard.Component == _BGP_UCI_Component_ComputeCard) ||
      (bgp_uci.IOCard.Component == _BGP_UCI_Component_IOCard)) {

    unsigned row = bgp_uci.ComputeCard.RackRow;
    unsigned col = bgp_uci.ComputeCard.RackColumn;
    unsigned mp  = bgp_uci.ComputeCard.Midplane;
    unsigned nc  = bgp_uci.ComputeCard.NodeCard;
    //fprintf(stdout, "after 4 stmts, R%x%x-M%d-N%x\n", row, col, mp, nc);

    if (row == 0xff) {
      sprintf(buf, "Rxx-Mx-N%x", nc);
      //fprintf(stdout, "if 0xff, R%x%x-M%d-N%x\n", row, col, mp, nc);
    } else {
      sprintf(buf, "R%x%x-M%d-N%x", row, col, mp, nc);
      // fprintf(stdout, "if not 0xff, R%x%x-M%d-N%x\n", row, col, mp, nc);
    }
  } else {
    sprintf(buf, "R?\?-M?-N?");
    //fprintf(stdout, "else, R?\?-M?-N? \n");
  }
}

/* unique string SMP-node identifier */
char* oss_bgp_platform_personality_node_name() {
#ifdef BGP_GROUP_ON_NODEBOARD
  static char buf[BGPPERSONALITY_MAX_LOCATION];
  oss_bgp_platform_personality_getNodeidString(&oss_bgp_personality, buf);
  //fprintf(stdout, "buf=%s\n", buf);
  return buf;
#else
  static char node[128];
  unsigned x = BGP_Personality_xCoord(&oss_bgp_personality);
  unsigned y = BGP_Personality_yCoord(&oss_bgp_personality);
  unsigned z = BGP_Personality_zCoord(&oss_bgp_personality);

  //fprintf(stdout, "node-%03d-%03d-%03d-%d\n", x, y, z, Kernel_PhysicalProcessorID());
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
