/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

/** @file
 *
 * Definition of the runtime library's API.
 *
 */

#ifndef _OpenSpeedShop_Framework_RuntimeAPI_
#define _OpenSpeedShop_Framework_RuntimeAPI_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#include <rpc/rpc.h>



/**
 * Performance data header.
 *
 * Header which is prepended to all performance data sent between a collector's
 * runtime and the framework. Contains enough information to allow the framework
 * to store and index the data within the proper experiment database.
 */
typedef struct {

    uint32_t experiment;  /**< Identifier of experiment containing data. */
    uint32_t thread;      /**< Identifier of gathered data's thread. */
    uint32_t collector;   /**< Identifier of collector gathering data. */
    
    uint64_t time_begin;  /**< Beginning of gathered data's time interval. */
    uint64_t time_end;    /**< End of gathered data's time interval. */
    
    uint64_t addr_begin;  /**< Beginning of gathered data's address range. */
    uint64_t addr_end;    /**< End of gathered data's address range. */
    
} OpenSS_DataHeader;



int OpenSS_DecodeParameters(const char*, const xdrproc_t, void*);
int OpenSS_Send(const OpenSS_DataHeader*, const xdrproc_t, const void*);



#endif
