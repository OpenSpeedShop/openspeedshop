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

char* oss_bgq_platform_personality_node_name() {

    static char ucStrg[128];
    Personality_t pers;
    BG_UniversalComponentIdentifier uci;
    Kernel_GetPersonality( &pers, sizeof(pers) );
    uci = pers.Kernel_Config.UCI;
    if (bg_uci_toString( uci, ucStrg) < 0) {
        ucStrg[0] = '\0';
    }
    //fprintf(stdout, "oss_bgq_platform_personality_getNodeidString, ucStrg=%s, core ID(%d) Thread ID(%d)\n",
    //            ucStrg, Kernel_ProcessorCoreID(), Kernel_ProcessorThreadID());
    //fprintf(stderr, "oss_bgq_platform_personality_getNodeidString, ucStrg=%s, core ID(%d) Thread ID(%d)\n",
    //            ucStrg, Kernel_ProcessorCoreID(), Kernel_ProcessorThreadID());
    return ucStrg;


}

