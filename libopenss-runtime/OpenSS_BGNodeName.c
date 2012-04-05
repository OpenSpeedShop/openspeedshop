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

/* CREDITS: Parts of this work/code is based on source code found in VampirTrace */

/**
 ** VampirTrace
 ** http://www.tu-dresden.de/zih/vampirtrace
 **
 ** Copyright (c) 2005-2012, ZIH, TU Dresden, Federal Republic of Germany
 **
 ** Copyright (c) 1998-2005, Forschungszentrum Juelich, Juelich Supercomputing
 **                          Centre, Federal Republic of Germany
 **
 ** See the file COPYING in the package base directory for details
 **/


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

#if defined(TARGET_OS_BGL) 
#include <stdio.h>
#include <bglpersonality.h>

#include "vt_pform.h"
#include "vt_defs.h"

#define BGL_GROUP_ON_NODEBOARD

static BGLPersonality oss_bgl_personality;

/* platform specific initialization */
void oss_bgl_platform_personality_init() {
  rts_get_personality(&oss_bgl_personality, sizeof(BGLPersonality));
}


/* unique numeric SMP-node identifier */
long oss_bgl_platform_personality_node_id() {
#ifdef BGL_GROUP_ON_NODEBOARD
  return ((oss_bgl_personality.location >> 6) & 0x1fff);
#else
  if ( BGLPersonality_virtualNodeMode(&oss_bgl_personality) )
    return ( BGLPersonality_psetNum(&oss_bgl_personality) *
           BGLPersonality_numNodesInPset(&oss_bgl_personality) +
           BGLPersonality_rankInPset(&oss_bgl_personality)) * 2
           + rts_get_processor_id();
  else
    return BGLPersonality_psetNum(&oss_bgl_personality) *
           BGLPersonality_numNodesInPset(&oss_bgl_personality) +
           BGLPersonality_rankInPset(&oss_bgl_personality);
#endif
}

static void oss_bgl_platform_personality_getLocString(const BGLPersonality *p, char *buf) {
  unsigned row = (p->location >> 15) & 0xf;
  unsigned col = (p->location >> 11) & 0xf;
  unsigned mp  = (p->location >> 10) & 1;
  unsigned nc  = (p->location >> 6) & 0xf;
  unsigned pc  = (p->location >> 1) & 0x1f;
  unsigned asic = p->location & 1;
  const char *asicname = (asic ? "U01" : "U11");
  if (row == 0xff)
    sprintf(buf, "Rxx-Mx-N%x-J%02d-%s", nc, pc, asicname);
  else
    sprintf(buf, "R%x%x-M%d-N%x-J%02d-%s", row, col, mp, nc, pc, asicname);
}

static void oss_bgl_platform_personality_getNodeidString(const BGLPersonality *p, char *buf) {
  unsigned row = (p->location >> 15) & 0xf;
  unsigned col = (p->location >> 11) & 0xf;
  unsigned mp  = (p->location >> 10) & 1;
  unsigned nc  = (p->location >> 6) & 0xf;
  if (row == 0xff)
    sprintf(buf, "Rxx-Mx-N%x", nc);
  else
    sprintf(buf, "R%x%x-M%d-N%x", row, col, mp, nc);
}

/* unique string SMP-node identifier */
char* oss_bgl_platform_personality_node_name() {
#ifdef BGL_GROUP_ON_NODEBOARD
  static char buf[BGLPERSONALITY_MAX_LOCATION];
  bgl_getNodeidString(&oss_bgl_personality, buf);
  return buf;
#else
  static char node[128];
  unsigned x = BGLPersonality_xCoord(&oss_bgl_personality);
  unsigned y = BGLPersonality_yCoord(&oss_bgl_personality);
  unsigned z = BGLPersonality_zCoord(&oss_bgl_personality);

  sprintf(node, "node-%03d-%03d-%03d-%d", x, y, z, rts_get_processor_id());

  /* -- BGL internal location string
 *   static char buf[BGLPERSONALITY_MAX_LOCATION];
 *     BGLPersonality_getLocationString(&oss_bgl_personality, buf);
 *       -- */
  return node;              
#endif
}

/* number of CPUs */
int oss_bgl_platform_personality_num_cpus() {
#ifdef BGL_GROUP_ON_NODEBOARD
  if ( BGLPersonality_virtualNodeMode(&oss_bgl_personality) )
    return 64;
  else
    return 32;
#else
  return 1;
#endif
}

#endif
