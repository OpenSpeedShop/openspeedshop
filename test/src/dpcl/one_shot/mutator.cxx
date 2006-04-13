////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////


#include "dpcl.h"
#include <stdlib.h>
#include <string.h>


extern char** environ;


static void output_cb(GCBSysType sys, GCBTagType, GCBObjType, GCBMsgType msg)
{
    char* ptr = (char*)msg;
    for(int i = 0; i < sys.msg_size; ++i, ++ptr)
          printf("%c", *ptr);
	  printf("\n");
	  fflush(stdout);
}

static void execute_cb(GCBSysType sys, GCBTagType tag, GCBObjType obj, GCBMsgType msg)
{
    Process *p = (Process *)obj;
    AisStatus *sts = (AisStatus *)msg;
    printf("task %d execute is completed w/status %d\n", p->get_task(),
	     (int)*sts);
    if((int)*sts != ASC_success) {
        printf("FAILED\n");
        exit(1);
    }
    else {
        printf("SUCCESS\n");
    }
    //
    // Success!
    //
    Ais_end_main_loop();
}

static void data_cb(GCBSysType sys, GCBTagType, GCBObjType obj, GCBMsgType msg)
{  
    Process *p = (Process *)obj;
    char* ptr = (char*)msg;
    for(int i = 0; i < sys.msg_size; ++i, ++ptr)
          printf("%c", *ptr);
	  printf("\n");
	  fflush(stdout);
}

static void terminate_cb(GCBSysType, GCBTagType, GCBObjType, GCBMsgType)
{  
   Ais_end_main_loop();
}

int main(int argc, char* argv[])
{
    AisStatus retval;
    
    // Initialize DPCL
    Ais_initialize();


    // Specify our process termination handler
    GCBFuncType old_terminate_cb;
    GCBTagType old_terminate_tag;
    retval = Ais_override_default_callback(AIS_PROC_TERMINATE_MSG,
					   terminate_cb, NULL,
					   &old_terminate_cb,
					   &old_terminate_tag);
    if(retval.status() != ASC_success) {
	printf("Ais_override_default_callback() failed: %s\n",
	       retval.status_name());
	exit(1);
    }
    

    if(argc < 3) {
        printf("Usage: %s <hostname> <a.out> <args>\n", argv[0]);
        exit(1);
    }

    // Create the (stopped) program
    Process appProcess;
    retval = appProcess.bcreate(strcmp(argv[1], "localhost") ? argv[1] : NULL,
               argv[2], (const char**)&(argv[2]), 
               (const char**)environ,
               output_cb, NULL, output_cb, NULL);
    if(retval.status() != ASC_success) {
        printf("Process::bcreate() failed: %s\n", retval.status_name());
        exit(1);
    }

    //
    // load the probe module to be executed 'one shot'
    //
    ProbeModule pm;
    pm = ProbeModule("hello.so");
    retval=appProcess.bload_module(&pm);
    if(retval.status() != ASC_success) {
	printf("Process::bload_module() failed: %s\n", retval.status_name());
	exit(1);
    }
    //
    // construct call to 'hello(Ais_msg_handle)'
    //
    //
    ProbeExp fcall;
    ProbeExp fref,parms[2],sor;
    char buffer[512],*name;
    int x,count,dtype;
    count=pm.get_count();
    for(x=0;x<count;x++)
    {
       name=pm.get_name(x,buffer,512);
       if(strcmp(name,"hello")==0)
       {
          fref = pm.get_reference(x);
          parms[0] = Ais_msg_handle;
          parms[1] = ProbeExp(4);
          fcall = fref.call(2,parms);
       }
    }
    //
    // Execute the probe immediately, regardless of what the mutatee is doing
    //
    // Insert the probe expression into the process    
    //
    GCBFuncType executeCallBack = execute_cb;
    GCBFuncType dataCallBack = data_cb;
    retval = appProcess.execute(fcall, dataCallBack, (GCBTagType)1,
		        executeCallBack, (GCBTagType)2);
    if(retval.status() != ASC_success) {
	printf("Process::bexecute of one-shot probe() failed: %s\n",
		       	retval.status_name());
	exit(1);
    }
    else {
	printf("Process::bexecute of one-shot probe() succeeded: %s\n",
		       	retval.status_name());
    }

    // Enter the DPCL main event loop
     Ais_main_loop();
    //
    exit(18);
}
    

