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
 * Declaration and definition of the PC sampling collector's runtime.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"
#include "blobs.h"



/*
 * NOTE: For some reason GCC doesn't like it when the following two macros are
 *       replaced with constant unsigned integers. It complains about the arrays
 *       in the tls structure being "variable-size type declared outside of any
 *       function" even though the size IS constant... Maybe this can be fixed?
 */

/** Number of entries in the sample buffer. */
#define BufferSize 1024

/** Number of entries in the hash table. */
#define HashTableSize (BufferSize + (BufferSize / 4))

/** Thread-local storage. */
#ifdef WDH_PER_THREAD_DATA_COLLECTION
static __thread struct {
#else
static struct {
#endif

    OpenSS_DataHeader header;  /**< Header for following data blob. */
    pcsamp_data data;          /**< Actual data blob. */

    /** Sample buffer. */
    struct {
	uint64_t pc[BufferSize];    /**< Program counter (PC) addresses. */
	uint8_t count[BufferSize];  /**< Sample count at each address. */
    } buffer;    
    
    /** Hash table mapping PC addresses to their sample buffer entries. */
    unsigned hash_table[HashTableSize];
    
} tls;



/**
 * Timer event handler.
 *
 * Called by the timer handler each time a sample is to be taken. Extracts the
 * program counter (PC) address from the signal context and places it into the
 * sample buffer. When the sample buffer is full, it is sent to the framework
 * for storage in the experiment's database.
 *
 * @note    Multple samples at the same PC address within a given sample buffer
 *          are indicated by incrementing a sample count rather than simply
 *          repeating the PC address. This helps compress the data in the common
 *          case where a degree of spatial locality is present. A hash table is
 *          used to accelerate the determination of whether or not an addresss
 *          is already in the sample buffer. This concept is losely based on the
 *          technique employed by Digital/Compaq/HP's DCPI.
 *
 * @sa    http://h30097.www3.hp.com/dcpi/src-tn-1997-016a.html
 * 
 * @param context    Thread context at timer interrupt.
 */
static void pcsampTimerHandler(const ucontext_t* context)
{
    uint64_t pc;
    unsigned bucket, entry;

    /* Obtain the program counter (PC) address from the thread context */
    pc = OpenSS_GetPCFromContext(context);

    /* 
     * Search the sample buffer for an existing entry corresponding to this
     * PC address. Use the hash table and a simple linear probe to accelerate
     * the search.
     */
    bucket = (pc >> 4) % HashTableSize;
    while((tls.hash_table[bucket] > 0) &&
	  (tls.buffer.pc[tls.hash_table[bucket] - 1] != pc))
	bucket = (bucket + 1) % HashTableSize;	

    /* Increment count for existing entry if found and not already maxed */
    if((tls.hash_table[bucket] > 0) &&
       (tls.buffer.pc[tls.hash_table[bucket] - 1] == pc) &&
       (tls.buffer.count[tls.hash_table[bucket] - 1] < UINT8_MAX)) {
	tls.buffer.count[tls.hash_table[bucket] - 1]++;
	return;
    }
    
    /* Otherwise add a new entry for this PC address to the sample buffer */
    entry = tls.data.pc.pc_len;
    tls.buffer.pc[entry] = pc;
    tls.buffer.count[entry] = 1;
    tls.data.pc.pc_len++;
    tls.data.count.count_len++;
    
    /* Update the hash table with this new entry */
    tls.hash_table[bucket] = entry + 1;
    
    /* Update the address interval in the data blob's header */
    if(pc < tls.header.addr_begin)
	tls.header.addr_begin = pc;
    if(pc > tls.header.addr_end)
	tls.header.addr_end = pc;
    
    /* Is the sample buffer full? */
    if(tls.data.pc.pc_len == BufferSize) {

	/* Send these samples */
	tls.header.time_end = OpenSS_GetTime();
	OpenSS_Send(&(tls.header), (xdrproc_t)xdr_pcsamp_data, &(tls.data));

	/* Re-initialize the data blob's header */
	tls.header.time_begin = tls.header.time_end;
	tls.header.time_end = 0;
	tls.header.addr_begin = ~0;
	tls.header.addr_end = 0;
	
	/* Re-initialize the actual data blob */
	tls.data.pc.pc_len = 0;
	tls.data.count.count_len = 0;

	/* Re-initialize the hash table */
	memset(tls.hash_table, 0, sizeof(tls.hash_table));

    }
}




/**
 * Start sampling.
 *
 * Starts program counter (PC) sampling for the thread executing this function.
 * Initializes the appropriate thread-local data structures and then enables the
 * sampling timer.
 *
 * @param arguments    Encoded function arguments.
 */
void pcsamp_start_sampling(const char* arguments)
{
    pcsamp_start_sampling_args args;

    /* Decode the passed function arguments. */
    memset(&args, 0, sizeof(args));
    OpenSS_DecodeParameters(arguments,
			    (xdrproc_t)xdr_pcsamp_start_sampling_args,
			    &args);
    
    /* Initialize the data blob's header */
    tls.header.experiment = args.experiment;
    tls.header.collector = args.collector;
    tls.header.thread = args.thread;    
    tls.header.time_begin = 0;
    tls.header.time_end = 0;
    tls.header.addr_begin = ~0;
    tls.header.addr_end = 0;

    /* Initialize the actual data blob */
    tls.data.interval = (uint64_t)(1000000000) / (uint64_t)(args.sampling_rate);
    tls.data.pc.pc_len = 0;
    tls.data.pc.pc_val = tls.buffer.pc;
    tls.data.count.count_len = 0;
    tls.data.count.count_val = tls.buffer.count;
    
    /* Initialize the hash table */
    memset(tls.hash_table, 0, sizeof(tls.hash_table));

    /* Begin sampling */
    tls.header.time_begin = OpenSS_GetTime();
    OpenSS_Timer(tls.data.interval, pcsampTimerHandler);
}



/**
 * Stop sampling.
 *
 * Stops program counter (PC) sampling for the thread executing this function.
 * Disables the sampling timer and sends any samples remaining in the buffer.
 *
 * @param arguments    Encoded (unused) function arguments.
 */
void pcsamp_stop_sampling(const char* arguments)
{
    /* Stop sampling */
    OpenSS_Timer(0, NULL);
    tls.header.time_end = OpenSS_GetTime();

    /* Send any samples remaining in the sample buffer */
    if(tls.data.pc.pc_len > 0)
	OpenSS_Send(&(tls.header), (xdrproc_t)xdr_pcsamp_data, &(tls.data));
}
