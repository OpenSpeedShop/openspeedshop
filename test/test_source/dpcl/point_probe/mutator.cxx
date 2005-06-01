
#include "dpcl.h"
#include <stdlib.h>
#include <string.h>

static int Count = 0;
static int Count_requested = 0;

extern char** environ;

static void data_cb(GCBSysType, GCBTagType, GCBObjType, GCBMsgType)
{
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
    Ais_end_main_loop();
}

int main(int argc, char* argv[])
{
    AisStatus retval;
    
    // Initialize DPCL
    Ais_initialize();

#if (1)
//        Ais_blog_on("hope2.americas.sgi.com", LGL_detail, LGD_daemon, 0,0);
        Ais_blog_on("localhost", LGL_detail, LGD_daemon, 0,0);
#endif

    
    // Specify our process termination handler
    GCBFuncType old_terminate_cb;
    GCBTagType old_terminate_tag;
    printf("Ais_override called\n");
    retval = Ais_override_default_callback(AIS_PROC_TERMINATE_MSG,
					   terminate_cb, NULL,
					   &old_terminate_cb,
					   &old_terminate_tag);
    if(retval.status() != ASC_success) {
	printf("Ais_override_default_callback() failed: %s\n",
	       retval.status_name());
	exit(1);
    }
    
#ifdef TEST_CREATE_PROCESS

    // Display usage information when necessary
    if(argc < 4) {
        printf("Usage: %s <count> <hostname> <a.out> <args>\n", argv[0]);
	exit(1);
    }

    // Create the (stopped) program
    Process appProcess;
    printf("bcreate called\n");
    retval = appProcess.bcreate(strcmp(argv[2], "localhost") ? argv[2] : NULL,
                                argv[3], (const char**)&(argv[3]), 
                                (const char**)environ,
				output_cb, NULL, output_cb, NULL);    
    if(retval.status() != ASC_success) {
	printf("Process::bcreate() failed: %s\n", retval.status_name());
	exit(1);
    }
    
#else

    // Display usage information when necessary
    // `
    printf("arg count %d\n", argc);
    if(argc < 3) {
	printf("Usage: %s <count> <hostname> <pid>\n", argv[0]);
	exit(1);
    }
    
    // Attach to the running program
    //
    int pid = atoi(argv[3]);
    printf("Pid is %d\n", pid);
    Process appProcess(strcmp(argv[2], "localhost") ? argv[2] : NULL, atoi(argv[3]));
    retval = appProcess.bconnect();
    if(retval.status() != ASC_success) {
	printf("Process::bconnect() failed: %s\n", retval.status_name());
	exit(1);
    }
    printf("mutator battaches  %s pid %s\n", argv[2], argv[3]);
    retval = appProcess.battach();
    if(retval.status() != ASC_success) {
	printf("Process::battach() failed: %s\n", retval.status_name());
	exit(1);
    }
    
#endif

    //
    // Get count of calls to 'mysleep'
    //
    Count_requested = atoi(argv[1]);
    printf("Doing %d calls to sleep\n", Count_requested);

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
    ProbeExp paramsExp[3];
    paramsExp[0] = Ais_msg_handle;
    paramsExp[1] = ProbeExp("1");
    paramsExp[2] = ProbeExp(2);
    ProbeExp sleepExp = Ais_send.call(3, paramsExp);
    
    // Insert the probe expression into the process    
    //
    GCBFuncType cbs[] = {data_cb};
    GCBTagType tags[] = {(GCBTagType)0};
    ProbeHandle sleepHandle;
    retval = appProcess.binstall_probe(1, &sleepExp, &sleepPt, cbs,
				       tags, &sleepHandle);

    if(retval.status() != ASC_success) {
	printf("Process::binstall_probe() failed: %s\n", retval.status_name());
	exit(1);
    }
    retval = appProcess.bactivate_probe(1, &sleepHandle);
    if(retval.status() != ASC_success) {
	printf("Process::bactivate_probe() failed: %s\n", retval.status_name());
	exit(1);
    }

    //
    // Let the mutatee run until completion
    // 
    
#ifdef TEST_CREATE_PROCESS

#if (1)
    // Start execution of mutatee
    retval = appProcess.bstart();
    if(retval.status() != ASC_success) {
	printf("Process::bstart() failed: %s\n", retval.status_name());
	exit(1);
    }
#endif

#else

    // Resume execution of mutatee
    retval = appProcess.bresume();
    if(retval.status() != ASC_success) {
	printf("Process::bresume() failed: %s\n", retval.status_name());
	exit(1);
    }

#endif
    
    // Enter the DPCL main event loop
    Ais_main_loop();

    exit(0);
}
