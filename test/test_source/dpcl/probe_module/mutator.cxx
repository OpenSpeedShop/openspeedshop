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

//
// Maximum length of a host name. According to the Linux manual page for the
// gethostname() function, this should be available in a header somewhere. But
// it isn't found on all systems, so define it directly if necessary.
//
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX (256)
#endif


extern char** environ;

static int Count = 0;
static int Count_requested = 0;

static void data_cb(GCBSysType sys, GCBTagType, GCBObjType, GCBMsgType msg)
{
    char* ptr = (char*)msg;
    for(int i = 0; i < sys.msg_size; ++i, ++ptr)
        printf("%c", *ptr);
    printf("\n");
    fflush(stdout);
    Count++;
}

static void output_cb(GCBSysType sys, GCBTagType, GCBObjType, GCBMsgType msg)
{
    char* ptr = (char*)msg;
    for(int i = 0; i < sys.msg_size; ++i, ++ptr)
	printf("%c", *ptr);
    printf("\n");
    fflush(stdout);
}

static void terminate_cb(GCBSysType, GCBTagType, GCBObjType, GCBMsgType)
{  
    printf("Counted %d call%s to sleep()!\n", Count, (Count != 1) ? "s" : "");
    if (Count == Count_requested) {
       // Success!
       printf("SUCCESS\n");
    }
    else {
       printf("FAILS\n");
    }
    // Success!
    Ais_end_main_loop();
}

int main(int argc, char* argv[])
{
    AisStatus retval;
    
    // Initialize DPCL
    Ais_initialize();
 
#if (0)
    // Obtain the local host name from the operating system
    char namebuffer[HOST_NAME_MAX];
    assert(gethostname(namebuffer, sizeof(namebuffer)) == 0);
    printf("blog got namebuffer=%s\n", namebuffer);
    Ais_blog_on(namebuffer, LGL_detail, LGD_daemon, 0,0);

//    Ais_blog_on("hope2.americas.sgi.com", LGL_detail, LGD_daemon, 0,0);

#endif
   
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
    
    // Display usage information when necessary
    if(argc < 4) {
        printf("Usage: %s <count> <hostname> <a.out> <args>\n", argv[0]);
	exit(1);
    }

    // Create the (stopped) program
    Process appProcess;
    retval = appProcess.bcreate(strcmp(argv[2], "localhost") ? argv[2] : NULL,
                                argv[3], (const char**)&(argv[3]),
                                (const char**)environ,
				output_cb, NULL, output_cb, NULL);    
    if(retval.status() != ASC_success) {
	printf("Process::bcreate() failed: %s\n", retval.status_name());
	exit(1);
    }
 
    //
    // Get count of calls to 'mysleep'
    //
    Count_requested = atoi(argv[1]);
    printf("Doing %d calls to sleep\n", Count_requested);

   

    ProbeModule pm;
    pm = ProbeModule("hello.so");
    retval=appProcess.bload_module(&pm);
    if(retval.status() != ASC_success) {
	printf("Process::bload_module() failed: %s\n", retval.status_name());
	exit(1);
    }


    // Get the program object associated with this process
    SourceObj appProgram = appProcess.get_program_object();    

    // Expand all the modules within this program
    for(int m = 0; m < appProgram.child_count(); ++m) {
	SourceObj appModule = appProgram.child(m);	
	retval = appModule.bexpand(appProcess);
	if(retval.status() != ASC_success) {
	    printf("SourceObj::bexpand() failed: %s\n", retval.status_name());
	    exit(1);
	}
    }
    
    //
    // Instrument mutatee to count number of calls to sleep()
    //

    // Find sleep() and its entry point 
    InstPoint sleepPt;

    for(int m = 0; (sleepPt.get_type() == IPT_invalid) &&
	    (m < appProgram.child_count()); ++m) {
	SourceObj appModule = appProgram.child(m);

	for(int f = 0; (sleepPt.get_type() == IPT_invalid) &&
		(f < appModule.child_count()); f++) {
	    SourceObj appFunction = appModule.child(f);
	    
	    char funcName[16384];
	    appFunction.get_demangled_name(funcName, sizeof(funcName));	    
	    if(strcmp(funcName, "mysleep") == 0) {
		
		for(int p = 0; (sleepPt.get_type() == IPT_invalid) &&
			(p < appFunction.exclusive_point_count()); ++p) {
		    InstPoint appPoint = appFunction.exclusive_point(p);

		    if(appPoint.get_type() == IPT_function_entry)
			sleepPt = appFunction.exclusive_point(p);
		    
		}
	    }	    
	}
    }
    if(sleepPt.get_type() == IPT_invalid) {
	printf("Unable to find sleep() or its entry point.\n");
	exit(1);
    }
    
    // Build the probe expression "Ais_send(Ais_msg_handle, "1", 2)"

    //
    // construct call to 'hello(Ais_msg_handle)'
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

    // Insert the probe expression into the process    
    GCBFuncType sleepCallback = data_cb;
    GCBTagType sleepTag = 0;
    ProbeHandle sleepHandle;
    retval = appProcess.binstall_probe(1, &fcall, &sleepPt, &sleepCallback,
				       &sleepTag, &sleepHandle);
    if(retval.status() != ASC_success) {
	printf("Process::binstall_probe() failed: %s\n", retval.status_name());
	exit(1);
    }
    retval = appProcess.bactivate_probe(1, &sleepHandle);
    if(retval.status() != ASC_success) {
	printf("Process::bactivate_probe() failed: %s\n", retval.status_name());
	exit(1);
    }

    // Start execution of mutatee
    retval = appProcess.bstart();
    if(retval.status() != ASC_success) {
        printf("Process::bstart() failed: %s\n", retval.status_name());
        exit(1);
    }


    //
    // Enter the DPCL main event loop
    Ais_main_loop();

    // Success!
    exit(0);
}
