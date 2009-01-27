/*******************************************************************************
** Copyright (c) 2008 William Hachfeld. All Rights Reserved.
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
 * Definition of the OpenSS_[Get|Set]]TLS() functions.
 *
 */

#include "Assert.h"
#include "RuntimeAPI.h"

#include <dlfcn.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>



/** Number of entries in the key/value maps. */
#define MapSize 32

/** Type representing a map of keys to their corresponding values. */
typedef struct {
    uint32_t size;           /**< Actual number of entries in the map. */
    uint32_t keys[MapSize];  /**< Keys. */
    void* values[MapSize];   /**< Corresponding values. */
} Map;



/** Flag indicating if the process is multithreaded.  */
static bool_t is_multithreaded = FALSE;

/** Pointer to pthread_key_create() if present. */
static int (*f_pthread_key_create)(pthread_key_t*, void (*)(void*)) = NULL;

/** Pointer to pthread_key_delete() if present. */
static int (*f_pthread_key_delete)(pthread_key_t) = NULL;

/** Pointer to pthread_getspecific() if present. */
static void* (*f_pthread_getspecific)(pthread_key_t) = NULL;
						     
/** Pointer to pthread_setspecific() if present. */
static int (*f_pthread_setspecific)(pthread_key_t, const void*) = NULL;

/** Key for accessing a thread's TLS map. */
static pthread_key_t map_key;

/** TLS map used when the process isn't actually multithreaded. */
static Map map_for_process;



/**
 * TLS initialization.
 *
 * Performs any one-time initialization of the internal TLS data structures.
 *
 * @note    One-time execution of this initialization is accomplished by
 *          applying the "constructor" attribute to this function. Doing
 *          so causes GCC to force it to be executed upon loading into a
 *          process. This attribute appears to be supported back to GCC
 *          2.95.3 or further.
 *
 * @sa    http://gcc.gnu.org/onlinedocs/gcc-4.3.2/gcc/Function-Attributes.html
 *
 * @ingroup Implementation
 */
static void __attribute__ ((constructor)) initialize()
{
    /* Get pointers to the Pthread functions if present in this process */
    f_pthread_key_create = (int (*)(pthread_key_t*, void (*)(void*)))
	dlsym(RTLD_DEFAULT, "pthread_key_create");
    f_pthread_key_delete = (int (*)(pthread_key_t))
	dlsym(RTLD_DEFAULT, "pthread_key_delete");
    f_pthread_getspecific = (void* (*)(pthread_key_t))
	dlsym(RTLD_DEFAULT, "pthread_getspecific");
    f_pthread_setspecific = (int (*)(pthread_key_t, const void*))
	dlsym(RTLD_DEFAULT, "pthread_setspecific");
    
    /* Decide if this process is multithreaded */
    is_multithreaded = 
	(f_pthread_key_create != NULL) && (f_pthread_key_delete != NULL) &&
	(f_pthread_getspecific != NULL) && (f_pthread_setspecific != NULL);

    /* Is this process multithreaded? */
    if(is_multithreaded) {

	/* Create the key for accessing a thread's TLS map */
	Assert((*f_pthread_key_create)(&map_key, free) == 0);

    }
    else {

	/* Initialize the process-wide TLS map */
	memset(&map_for_process, 0, sizeof(Map));

    }	
}



/**
 * Get the value for a key.
 *
 * Returns a pointer to the value corresponding to the specified key.
 *
 * @param key    Key for which to get the value.
 * @return       Pointer to the current value corresponding to that key.
 *
 * @ingroup Implementation
 */
static void** getValue(uint32_t key)
{
    Map* map = &map_for_process;
    unsigned bucket;

    /* Is this process multithreaded? */
    if(is_multithreaded) {
	
	/** Get this thread's TLS map */
	map = (Map*)(*f_pthread_getspecific)(map_key);

	/** Is this the first time this thread's TLS map was accessed? */
	if(map == NULL) {
	    
	    /* Allocate and initialize a new TLS map */
	    map = (Map*)malloc(sizeof(Map));
	    Assert(map != NULL);
	    memset(map, 0, sizeof(Map));

	    /* Make it this thread's TLS map */
	    Assert((*f_pthread_setspecific)(map_key, map) == 0);
	    
	}
	
    }
    
    /* Check assertions */
    Assert(map != NULL);

    /*
     * Search this thread's TLS map for an existing entry corresponding to the
     * specified key. Use a hash table with a simple linear probe to accelerate
     * the search.
     */
    bucket = key % MapSize;
    while((map->keys[bucket] > 0) && (map->keys[bucket] != key))
	bucket = (bucket + 1) % MapSize;

    /* Is this the first time accessing this key? */
    if(map->keys[bucket] == 0) {

	/* Place this key into the map (with an initially-NULL value) */
	map->keys[bucket] = key;
	map->size++;
	
	/*
	 * Check assertions. There must always be at least one empty (zero)
	 * key in the map or the simple linear probe above would become an
	 * infinite loop.
	 */
	Assert(map->size < (MapSize - 1));
	
    }

    /* Return the value's pointer to the caller */
    return &(map->values[bucket]);
}



/**
 * Get a TLS value.
 *
 * Returns the value in thread-local storage (TLS) corresponding to the
 * specified key.
 *
 * @sa    http://en.wikipedia.org/wiki/Thread-local_storage
 *
 * @param key    Key for which to get the value.
 * @return       Current value corresponding to that key.
 *
 * @ingroup RuntimeAPI
 */
void* OpenSS_GetTLS(uint32_t key)
{
//fprintf(stderr,"OpenSS_GetTLS GET gets key %#X\n",key);
    void** map_value = NULL;
    
    /** Check preconditions */
    Assert(key > 0);

    /** Get a pointer to the value corresponding to this key */
    map_value = getValue(key);
    Assert(map_value != NULL);

    /** Return the current value of this key to the caller */
    return *map_value;
}



/**
 * Set a TLS value.
 *
 * Sets the value in thread-local storage (TLS) corresponding to the
 * specified key.
 *
 * @sa    http://en.wikipedia.org/wiki/Thread-local_storage
 *
 * @param key      Key for which to get the value.
 * @param value    New value corresponding to that key.
 *
 * @ingroup RuntimeAPI
 */
void OpenSS_SetTLS(uint32_t key, void* value)
{
//fprintf(stderr,"OpenSS_SetTLS SET sets key %#X\n",key);
    void** map_value = NULL;
    
    /** Check preconditions */
    Assert(key > 0);

    /** Get a pointer to the value corresponding to this key */
    map_value = getValue(key);
    Assert(map_value != NULL);

    /** Set this key's new value */
    *map_value = value;
}
