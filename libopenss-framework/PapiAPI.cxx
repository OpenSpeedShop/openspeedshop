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

#include <stdlib.h>
#include <string.h>

void hwc_init_papi()
{
    int rval;

    /* Initialize the PAPI library */

    rval = PAPI_library_init(PAPI_VER_CURRENT);

    if (rval != PAPI_VER_CURRENT && rval > 0) {
	fprintf(stderr,"PAPI library version mismatch!\n");
	char error_str[PAPI_MAX_STR_LEN];
	PAPI_perror(rval,error_str,PAPI_MAX_STR_LEN);
	fprintf(stderr,"PAPI_error %d: %s\n",rval,error_str);
	exit(1);
    }

    if (rval < 0) {
	/* test rval to see if something is wrong */
#if 0
	fprintf(stderr,"PAPI rval < 0 !\n");
	char error_str[PAPI_MAX_STR_LEN];
	PAPI_perror(rval,error_str,PAPI_MAX_STR_LEN);
	fprintf(stderr,"PAPI_error %d: %s\n",rval,error_str);
	fprintf(stderr,"SYSTEM error: %s\n", strerror(errno));
#endif
	return;
    }
  hw_info = PAPI_get_hardware_info() ;
  if (hw_info == NULL) {
    fprintf(stderr, "PAPI_get_hardware_info failed\n") ;
    fprintf(stderr, "At line %d in file %s\n", __LINE__, __FILE__) ;
  }
}

#ifdef USE_ALLOC_VALUES
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

void print_papi_events ()
{
/* Scan for all supported native events on this platform */

    int i = PAPI_PRESET_MASK;;
    PAPI_event_info_t info;
    printf("Name                  Code      Description\n");
    do
    {
        int retval = PAPI_get_event_info(i, &info);
        if (retval == PAPI_OK) {
            printf("%-30s 0x%-10x0s\n", info.symbol, info.event_code, info.long_descr);
        }
    } while (PAPI_enum_event(&i, PAPI_PRESET_ENUM_AVAIL) == PAPI_OK);
}

void get_papi_name (int code, char *ecstr)
{
    char EventCodeStr[PAPI_MAX_STR_LEN];
    if (PAPI_event_code_to_name(code, EventCodeStr) != PAPI_OK)
    {
        fprintf(stderr,"get_papi_name: PAPI_event_code_to_name failed.\n");
    }
    strncpy(ecstr,EventCodeStr,sizeof(EventCodeStr));
}

int get_papi_eventcode (char* eventname)
{
    int EventCode = PAPI_NULL;
    char* strptr;
    char ename[PAPI_MAX_STR_LEN];
    strcpy(ename,eventname);
    strptr = ename;
    if (PAPI_event_name_to_code(strptr,&EventCode) != PAPI_OK) {
        fprintf(stderr,"get_papi_eventcode: PAPI_event_name_to_code failed!\n");
        return -1;
    }
    return EventCode;
}

void print_papi_error (int errcode)
{
    char error_str[PAPI_MAX_STR_LEN];
    PAPI_perror(errcode,error_str,PAPI_MAX_STR_LEN);
    fprintf(stderr,"PAPI_error %d: %s\n",errcode,error_str);
}

void get_papi_available_presets()
{
/* Scan for available preset events on this platform */
/* NOTE: For now, exclude derived presets (more than one native event) */

    int available_presets = PAPI_PRESET_ENUM_AVAIL;
    int i = PAPI_PRESET_MASK;
    PAPI_event_info_t info;

/*
    printf("Name\t\tDescription\n");
*/
    do
    {
        int rval = PAPI_get_event_info(i, &info);
        if (rval == PAPI_OK){
	    /* this test will skip derived preset events */
	    if (info.count == 1) {
/*
	        printf("%s\t%s\n", info.symbol, info.short_descr);
*/
	    }
        }
    } while (PAPI_enum_event(&i, available_presets) == PAPI_OK);
}

bool query_papi_event (int event)
{
        int rval = PAPI_query_event(event);
        if (rval != PAPI_OK) {
/*
            fprintf(stderr,"The event %s does not seem to be supported\n",
                get_papi_name(event));
            OpenSS_PAPIerror(rval);
*/
            return false;
        }

        PAPI_event_info_t info;

        rval = PAPI_get_event_info(event,&info);
        if (rval != PAPI_OK) {
/*
	fprintf(stderr,"The event %s does not seem to be supported\n",
		get_papi_name(event));
            OpenSS_PAPIerror(rval);
*/
            return false;
        }

	/* till we get derived presets working, do not allow */
	if (info.count > 1) {
	    return false;
	}

	if (info.count == 0) {
	    return true;
	}
}

papi_available_presets OpenSS_papi_available_presets()
{
/* Scan for available preset events on this platform */
/* NOTE: For now, exclude derived presets (more than one native event) */

    int available_presets = PAPI_PRESET_ENUM_AVAIL;
    int i = PAPI_PRESET_MASK;
    PAPI_event_info_t info;
    papi_available_presets presets_list;
    presets_list.clear();
    std::pair<std::string, std::string> *pe;

    /* make sure papi is started prior to using low-level commands */
    hwc_init_papi();

    do
    {
        int rval = PAPI_get_event_info(i, &info);
        if (rval == PAPI_OK){
	    /* this test will skip derived preset events */
	    if (info.count == 1) {
	        //printf("%s\t%s\n", info.symbol, info.short_descr);
		pe = new std::pair<std::string, std::string>(info.symbol,info.short_descr);	
		presets_list.push_back(*pe);
	    }
        } else {
	    print_papi_error(rval);
	}
    } while (PAPI_enum_event(&i, available_presets) == PAPI_OK);

    return presets_list;
}
