
#include <stdio.h>
#include <stdlib.h>
#include <dpcl.h>

//
// Maximum length of a host name. According to the Linux manual page for the
// gethostname() function, this should be available in a header somewhere. But
// it isn't found on all systems, so define it directly if necessary.
//
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX (256)
#endif

void msg_cb(GCBSysType sys, GCBTagType tag, GCBObjType obj, GCBMsgType msg);

 
static void terminate_cb(GCBSysType, GCBTagType, GCBObjType, GCBMsgType)
{
    Ais_end_main_loop();
}

int

main(int argc, char *argv[])

{

    AisStatus retval;

    Process P;

    AisStatus sts;

    int c;

    int value;

    ProbeExp pcount;

    ProbeExp pcount2;

    ProbeExp init_func;

    ProbeExp begin_func;

    ProbeExp data_func;

    ProbeExp end_func;

    ProbeExp exit_begin_func;

    ProbeExp exit_data_func;

    ProbeExp exit_end_func;


    ProbeModule load_1;

    float period;

    float interval;

    Phase phase1;

    const int bufsize = 256;

    char buffer[bufsize];

    char *name;

 

                             // initialize DPCL environment

    Ais_initialize();

#if (1)
    // Obtain the local host name from the operating system
    char namebuffer[HOST_NAME_MAX];
    assert(gethostname(namebuffer, sizeof(namebuffer)) == 0);
//    printf("blog got namebuffer=%s\n", namebuffer);
    Ais_blog_on(namebuffer, LGL_detail, LGD_daemon, 0,0);

//        Ais_blog_on("hope2.americas.sgi.com", LGL_detail, LGD_daemon, 0,0);
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
    } else {
       printf("Ais_override_default_callback() succeeded: %s\n",
               retval.status_name());
    }

                             // construct a valid Process object

//    printf("debug info: argv[1]=%s, argv[2]=%d\n", argv[1], atoi(argv[2]) );
    P = Process(argv[1], atoi(argv[2]));

                             // connect to the target application

    sts = P.bconnect();

    if (sts.status() != ASC_success) exit(1);


                             // load probe module

    load_1 = ProbeModule("probe_module.so");

    sts = P.bload_module(&load_1);

    if (sts.status() != ASC_success) exit(1);

                             // look for all the phase related functions

    for (c = 0;c < load_1.get_count(); c++) 

    {

        name = load_1.get_name(c, buffer, bufsize);

        if (strcmp("init_func", name) == 0)

        {

            init_func = load_1.get_reference(c);

        }

        else if (strcmp("begin_func", name) == 0)

        {

            begin_func = load_1.get_reference(c);

        }

        else if (strcmp("data_func", name) == 0)

        {

            data_func = load_1.get_reference(c);

        }

        else if (strcmp("end_func", name) == 0)

        {

            end_func = load_1.get_reference(c);

        }

    }

                             // create a phase object with a large period

                             // to effectively suspend its execution

    period = 999.0;

    try 

    {

        phase1 = Phase(period,

                       begin_func, (GCBFuncType)msg_cb, (GCBTagType)1,

                       data_func, (GCBFuncType)msg_cb, (GCBTagType)2,

                       end_func, (GCBFuncType)msg_cb, (GCBTagType)3);

    }

    catch (AisStatus excp) 

    {

        printf("new Phase failed with status =%s\n", excp.status_name());
        exit(1);

    }

                             // add the phase to target application

    sts = P.badd_phase(phase1,

                       init_func, (GCBFuncType)msg_cb, (GCBTagType)4);

    if (sts.status() != ASC_success) {
        printf("P.badd_phase (init_func) failed with status =%d\n", sts.status());
        exit(1);
    }

                             // set the phase's exit functions

    sts = P.bset_phase_exit(phase1,

                            exit_begin_func, (GCBFuncType)msg_cb, (GCBTagType)6,

                            exit_data_func, (GCBFuncType)msg_cb, (GCBTagType)7,

                            exit_end_func, (GCBFuncType)msg_cb, (GCBTagType)8);

    if (sts.status() != ASC_success) {
        printf("P.bset_phase_exit (exit_begin, exit_data_func, exit_end_func) failed with status =%d\n", sts.status());
        exit(1);
    }

    value = 0;

    pcount = P.balloc_mem(int32_type(), &value, phase1, sts);

    if (sts.status() != ASC_success) {
      printf("P.balloc_mem (pcount-int32_type()) failed with status =%d\n", sts.status());
      exit(1);
    }

    value = 100;

    pcount2 = P.balloc_mem(int32_type(), &value, phase1, sts);

    if (sts.status() != ASC_success) {
      printf("P.balloc_mem (pcount2-int32_type()) failed with status =%d\n", sts.status());
      exit(1);
    }

                             // set the phase to its true execution interval

                             // to resume its normal operation

    period = 0.1;

    sts = P.bset_phase_period(phase1, period);

    if (sts.status() != ASC_success) {
      printf("P.bset_phase_period (period) failed with status =%d\n", sts.status());
      exit(1);
    }
                             // query the phase's execution interval

    interval = P.get_phase_period(phase1, sts);

    sts = P.battach();

    if (sts.status() != ASC_success) {
      printf("P.battach() failed with status =%d\n", sts.status());
      exit(1);
    }

    sts = P.bresume();

    if (sts.status() != ASC_success) {
      printf("P.bresume() failed with status =%d\n", sts.status());
      exit(1);
    }

    Ais_main_loop();

    // Success!
    printf("SUCCESS!\n");
    exit(0);

}

 

void

msg_cb(GCBSysType sys, GCBTagType tag, GCBTagType obj, GCBMsgType msg)

{

    static int count = 0;

 

    count++;

    char *chp = (char *)msg;

//    printf("msg_cb received the msg(%d)=\"", count);

    for (int i = 0; i < sys.msg_size; ++i)

    {

//        printf("%c", chp[i]);

    }

//    printf("\"\n");

}
 
