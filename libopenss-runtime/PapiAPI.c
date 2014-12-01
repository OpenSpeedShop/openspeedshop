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
 * utility functions for papi
 *
 */

#include "PapiAPI.h"
#include <dlfcn.h>
#include <fcntl.h>
#include <pthread.h>

/**
 * Method: hwc_init_papi()
 * 
 * xxx.
 *     
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
void hwc_init_papi()
{
    int rval;

    /* Initialize the PAPI library */

    rval = PAPI_library_init(PAPI_VER_CURRENT);

    if (rval != PAPI_VER_CURRENT && rval > 0) {
	fprintf(stderr,"PAPI library version mismatch!\n");
	char error_str[PAPI_MAX_STR_LEN];
#if (PAPI_VERSION_MAJOR(PAPI_VERSION) >= 5)
	PAPI_perror(error_str);
#else
	PAPI_perror(rval,error_str,PAPI_MAX_STR_LEN);
#endif
	fprintf(stderr,"PAPI_error %d: %s\n",rval,error_str);
	exit(1);
    }

    if (rval < 0) {
	/* test rval to see if something is wrong */
	fprintf(stderr,"PAPI rval < 0 !\n");
	char error_str[PAPI_MAX_STR_LEN];
#if (PAPI_VERSION_MAJOR(PAPI_VERSION) >= 5)
	PAPI_perror(error_str);
#else
	PAPI_perror(rval,error_str,PAPI_MAX_STR_LEN);
#endif
	fprintf(stderr,"PAPI_error %d: %s\n",rval,error_str);
	fprintf(stderr,"SYSTEM error: %s\n", strerror(errno));
	return;
    }

    hw_info = PAPI_get_hardware_info() ;
    if (hw_info == NULL) {
	fprintf(stderr, "PAPI_get_hardware_info failed\n") ;
	fprintf(stderr, "At line %d in file %s\n", __LINE__, __FILE__) ;
    }

#if defined(BUILD_TARGETED)
#if defined(HAVE_TARGET_POSIX_THREADS)

    if (PAPI_thread_init(( unsigned long ( * )( void ) ) ( pthread_self )) != PAPI_OK ) {
	fprintf(stderr, "PAPI_thread_init failed\n") ;
    }
#else
#if defined(HAVE_POSIX_THREADS)
    if (PAPI_thread_init(( unsigned long ( * )( void ) ) ( pthread_self )) != PAPI_OK ) {
	fprintf(stderr, "PAPI_thread_init failed\n") ;
    }
#endif
#endif
#else
#if defined(HAVE_POSIX_THREADS)
    if (PAPI_thread_init(( unsigned long ( * )( void ) ) ( pthread_self )) != PAPI_OK ) {
	fprintf(stderr, "PAPI_thread_init failed\n") ;
    }
#endif
#endif




    /* init papi for multiplexing events */
    if (PAPI_multiplex_init() != PAPI_OK) {
        fprintf(stderr, "PAPI_multiplex_init failed\n") ;
    }
}

#ifdef USE_ALLOC_VALUES
/**
 * Method: allocate_test_space()
 * 
 * xxx.
 *     
 * @param   num_tests - xxx.
 * @param   num_events - xxx.
 *
 * @return  long_long **
 *
 * @todo    Error handling.
 *
 */
long_long **allocate_test_space(int num_tests, int num_events)
{
   long_long **values;
   int i;

   values = (long_long **) malloc(num_tests * sizeof(long_long *));
   if (values == NULL)
      exit(1);
   memset(values, 0x0, num_tests * sizeof(long_long *));

   for (i = 0; i < num_tests; i++) {
      values[i] = (long_long *) malloc(num_events * sizeof(long_long));
      if (values[i] == NULL)
         exit(1);
      memset(values[i], 0x00, num_events * sizeof(long_long));
   }
   return (values);
}
#endif

/**
 * Method: print_hw_info()
 * 
 * Print out the hardward information for.
 * the target platform.
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
void print_hw_info()
{

  hw_info = PAPI_get_hardware_info() ;
  if (hw_info == NULL) {
    fprintf(stderr, "PAPI_get_hardware_info failed\n") ;
    fprintf(stderr, "At line %d in file %s\n", __LINE__, __FILE__) ;
  }

  printf("PAPI hardware information\n") ;
  printf("  # of CPUs in SMP node: %d\n", hw_info->ncpu) ;
  printf("  # of SMP nodes:        %d\n", hw_info->nnodes) ;
  printf("  Total CPUs in system:  %d\n", hw_info->totalcpus) ;
  printf("  Vendor:                %d\n", hw_info->vendor) ;
  printf("  Vendor string:         %s\n", hw_info->vendor_string) ;
  printf("  CPU model:             %d\n", hw_info->model) ;
  printf("  CPU model string:      %s\n", hw_info->model_string) ;
  printf("  Revision of CPU:       %9.4f\n", hw_info->revision) ;
  printf("  Cycle time MHz:        %9.4f\n", hw_info->mhz) ;
}

/**
 * Method: get_papi_exe_info()
 * 
 * xxx.
 *     
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
void get_papi_exe_info ()
{
    const PAPI_exe_info_t *exeinfo = NULL;


    if ((exeinfo = PAPI_get_executable_info()) == NULL)
      exit(1);

    printf("Path+Program: %s\n",exeinfo->fullname);
    printf("Program: %s\n",exeinfo->address_info.name);
    printf("Text start: %p, Text end: %p\n",
	exeinfo->address_info.text_start,exeinfo->address_info.text_end);
    printf("Data start: %p, Data end: %p\n",
	exeinfo->address_info.data_start,exeinfo->address_info.data_end);
    printf("Bss start: %p, Bss end: %p\n",
	exeinfo->address_info.bss_start,exeinfo->address_info.bss_end);
}

/**
 * Method: print_papi_events()
 * 
 * Scan for all supported native events on this platform.
 *     
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
void print_papi_events ()
{

    int i = PAPI_PRESET_MASK;;
    PAPI_event_info_t info;
    printf("Name                  Code      Description\n");
    do
    {
        int retval = PAPI_get_event_info(i, &info);
        if (retval == PAPI_OK) {
            //printf("%-30s 0x%-10x0s\n", info.symbol, info.event_code, info.long_descr);
            printf("%-30s %x %-10s\n", info.symbol, info.event_code, info.long_descr);
        }
    } while (PAPI_enum_event(&i, PAPI_PRESET_ENUM_AVAIL) == PAPI_OK);
}

/**
 * Method: get_papi_name()
 * 
 * Get the name of an event represented by a
 * numerical code.
 *     
 * @param   code - xxx.
 * @param   ecstr - xxx.
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
void get_papi_name (int code, char *ecstr)
{
    char EventCodeStr[PAPI_MAX_STR_LEN];
    if (PAPI_event_code_to_name(code, EventCodeStr) != PAPI_OK)
    {
        fprintf(stderr,"get_papi_name: PAPI_event_code_to_name failed.\n");
    }
    strncpy(ecstr,EventCodeStr,sizeof(EventCodeStr));
}

/**
 * Method: get_papi_eventcode()
 * 
 * Return the numerical code represented by a
 * given event string.
 *     
 * @param   eventname - xxx.
 *
 * @return  int EventCode
 *
 * @todo    Error handling.
 *
 */
int get_papi_eventcode (char* eventname)
{
    int EventCode = PAPI_NULL;
    char* strptr;
    char ename[PAPI_MAX_STR_LEN];
    strcpy(ename,eventname);
    strptr = ename;
    if (PAPI_event_name_to_code(strptr,&EventCode) != PAPI_OK) {
        fprintf(stderr,"get_papi_eventcode: PAPI_event_name_to_code failed!\n");
        return;
    }
    return EventCode;
}

/**
 * Method: print_papi_error()
 * 
 * xxx.
 *     
 * @param   errcode - xxx.
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
void print_papi_error (int errcode)
{
    fprintf(stderr,"print_papi_error: ENTERED!\n");
}



