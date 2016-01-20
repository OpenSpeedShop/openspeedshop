/*******************************************************************************
 * ** Copyright (c) 2012-2016 Krell Institute All Rights Reserved.
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

#if defined(RUNTIME_PLATFORM_BGL) 
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
