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
 * Specification of the performance data header.
 *
 */

/* Performance data header. - jeg leave for comparator */

/**
 * Performance data header.
 *
 * Header which is prepended to all performance data sent between a collector's
 * runtime and the framework. Contains enough information to allow the framework
 * to store and index the data within the proper experiment database.
 */
struct OpenSS_DataHeader {

    int experiment;  /**< Identifier of experiment to contain the data. */
    int collector;   /**< Identifier of collector gathering data. */
    int thread;      /**< Identifier of gathered data's thread. */
    
    uint64_t time_begin;  /**< Beginning of gathered data's time interval. */
    uint64_t time_end;    /**< End of gathered data's time interval. */
    
    uint64_t addr_begin;  /**< Beginning of gathered data's address range. */
    uint64_t addr_end;    /**< End of gathered data's address range. */
    
};
