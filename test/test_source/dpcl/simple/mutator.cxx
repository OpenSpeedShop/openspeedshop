
#include <assert.h>
#include <stdio.h>
#include <netinet/in.h>

#include "dpcl.h"
#include "version.h"

//
// Maximum length of a host name. According to the Linux manual page for the
// gethostname() function, this should be available in a header somewhere. But
// it isn't found on all systems, so define it directly if necessary.
//
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX (256)
#endif

extern char** environ;

static void output_cb(GCBSysType sys, GCBTagType, GCBObjType, GCBMsgType msg)
{
    char* ptr = (char*)msg;
    for(int i = 0; i < sys.msg_size; ++i, ++ptr)
        fputc(*ptr, stdout);
}

static void terminate_cb(GCBSysType, GCBTagType, GCBObjType, GCBMsgType)
{
    Ais_end_main_loop();
}

int main(int argc, char* argv[])
{
    AisStatus retval;
        unsigned char v[4];
        int iv=htonl(DPCLVersion);
        memcpy(v,&iv,sizeof(int));
        printf("\tusing DPCLVersion = %x.%x.%x.%x\n"
                ,v[0],v[1],v[2],v[3]);
    assert(argc > 1);

    Ais_initialize();
#if (1)
    // Obtain the local host name from the operating system
    char namebuffer[HOST_NAME_MAX];
    assert(gethostname(namebuffer, sizeof(namebuffer)) == 0);
//    printf("blog got namebuffer=%s\n", namebuffer);
    Ais_blog_on(namebuffer, LGL_detail, LGD_daemon, 0,0);

//    Ais_blog_on("hope2.americas.sgi.com", LGL_detail, LGD_daemon, NULL, 0);
#endif

    GCBFuncType old_terminate_cb;
    GCBTagType old_terminate_tag;
    retval = Ais_override_default_callback(AIS_PROC_TERMINATE_MSG,
					   terminate_cb, NULL,
					   &old_terminate_cb,
					   &old_terminate_tag);
    if(retval.status() != ASC_success) {
	printf("Ais_override_default_callback() failed: %s\n",
	       retval.status_name());
        printf("FAILS\n");
	return 1;
    } else {
	printf("Ais_override_default_callback() passes: %s\n", retval.status_name());
    }
    
    Process mutatee;
    retval = mutatee.bcreate(NULL, argv[1],
			     (const char**)&(argv[1]),
			     (const char**)environ,
			     output_cb, NULL, output_cb, NULL);
    if(retval.status() != ASC_success) {
	printf("Process::bcreate() failed: %s\n", retval.status_name());
        printf("FAILS\n");
	return 1;
    } else {
	printf("Process::bcreate() passes: %s\n", retval.status_name());
    }
    
    retval = mutatee.bstart();
    if(retval.status() != ASC_success) {
	printf("Process::bstart() failed: %s\n", retval.status_name());
        printf("FAILS\n");
	return 1;
    } else {
	printf("Process::bstart() passes: %s\n", retval.status_name());
    }
    
    Ais_main_loop();
    
    printf("SUCCESS\n");

    return 0;
}
