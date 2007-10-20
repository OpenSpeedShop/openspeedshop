/*******************************************************************************
** Copyright (c) 2007 The Krell Institue. All Rights Reserved.
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
 * Specification of the offline collector's object blobs.
 *
 */

/** Structure of the blob containing our file objects. */
struct openss_objects {
    string   objname<>;   /**< Name of the object. */
    uint64_t addr_begin;  /** < begin address of object */
    uint64_t addr_end;    /** < end address of object */
};

/** Structure of the blob containing our process info. */
struct openss_expinfo {
    string   collector<>;  /** < Name of the collector. */
    string   hostname<>;   /** < Name of the host. */
    uint32_t pid;          /** < pid for this object. */
    uint64_t tid;	   /** < tid for this object */
};
