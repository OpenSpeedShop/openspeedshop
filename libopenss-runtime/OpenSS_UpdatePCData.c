#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RuntimeAPI.h"
			      
/**
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
 * @note    OpenSS_UpdatePCData Returns TRUE if the pc sample buffer is full.
 *          Updates the pc data buffer with passed pc address.
 *          If the address already exists in the buffer and
 *          the count for that address is not maxed, increment
 *          it's count in the count buffer..
 *          Otherwise, add a new entry to the sample buffer
 *          and test  if the buffer is full.
 *          Update the length of the pc and count buffers.
*/
bool_t OpenSS_UpdatePCData(uint64_t pc, OpenSS_PCData *pcdata)
{

    /*
     * Search the passed sample for an existing entry corresponding
     * to this * PC address. Use the hash table and a simple linear probe
     * to accelerate the search.
     */
    unsigned bucket, entry, len;
    bucket = (pc >> 4) % OpenSS_PCHashTableSize;
    while((pcdata->hash_table[bucket] > 0) &&
          (pcdata->buffer.pc[pcdata->hash_table[bucket] - 1] != pc))
    {
        bucket = (bucket + 1) % OpenSS_PCHashTableSize;
    }

    /* Increment count for existing entry if found and not already maxed */
    if((pcdata->hash_table[bucket] > 0) &&
       (pcdata->buffer.pc[pcdata->hash_table[bucket] - 1] == pc) &&
       (pcdata->buffer.count[pcdata->hash_table[bucket] - 1] < UINT8_MAX)) {
        pcdata->buffer.count[pcdata->hash_table[bucket] - 1]++;
        return FALSE;
    }

    /* Otherwise add a new entry for this PC address to the sample */
    entry = pcdata->pc_len;
    pcdata->buffer.pc[entry] = pc;
    pcdata->buffer.count[entry] = 1;
    pcdata->pc_len = pcdata->pc_len + 1;
    pcdata->count_len = pcdata->count_len + 1;

    /* Update the address interval in the data blob's header */
    if(pc < pcdata->addr_begin)
        pcdata->addr_begin = pc;
    if(pc > pcdata->addr_end)
        pcdata->addr_end = pc;

    /* Update the hash table with this new entry */
    pcdata->hash_table[bucket] = entry + 1;

    len = pcdata->pc_len;

    /* Is the sample buffer full? */
    if(len == OpenSS_PCBufferSize) {
	return TRUE;
    }
    return FALSE;
}
