/** @file
 *
 * Storage for the parse results of a single OpenSpeedShop command.
 *
 */

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

ostringstream out_stream;

//*************************************************************

typedef struct {
    int arg_count;
    char **arg_strings;
    char *identifier;
} arglist_t;

//*************************************************************

char *cluster_list[] = {
    NULL,
    "cluster_1",
    "cluster_1,cluster_2,cluster_3"
};

char *host_list_1[] = {
    NULL,
    "host_1"
};

char *host_list[] = {
    NULL,
    "host_1",
    "host_1,host_2,host_3",
    "128.064.032.008",
    "128.064.032.008,032.002.016.128",
    "host_1,128.064.032.008,host_2,032.004.016.128"
};

char *file_list_1[] = {
    NULL,
    "file_1"
};

char *file_list[] = {
    NULL,
    "file_1",
    "file_1,file_2,file_3"
};

char *pid_list[] = {
    NULL,
    "888",
    "888,999,777",
    "888:999",
    "777,888:999,666"
};

char *thread_list[] = {
    NULL,
    "123",
    "123,456,789",
    "123:456",
    "123,456:789,987"
};

char *rank_list[] = {
    NULL,
    "100",
    "100,200,500",
    "100:200",
    "100,200:500,800"
};

#define TARGET_MAX 6

arglist_t target_arg[TARGET_MAX] = {
    3, cluster_list,"-c",
    6, host_list,"-h",
    3, file_list,"-f",
    5, pid_list,"-p",
    5, thread_list,"-t",
    5, rank_list,"-r"
};

//*************************************************************

char *exptype_list[] = {
    NULL,
    "pcsamp",
    "usertime",
    "mpi",
    "fpe",
    "hwc",
    "io",
    "pcsamp,usertime,mpi,io"
};

#define EXPTYPE_MAX 1
arglist_t exptype_arg[EXPTYPE_MAX] = {
    8,exptype_list,NULL
};

//*************************************************************


char *exp_id_list[] = {
    NULL,
    "6"
};

#define EXPID_MAX 1
arglist_t exp_id_arg[EXPTYPE_MAX] = {
    2,exp_id_list,"-x"
};

//*************************************************************


char *all_list[] = {
    NULL,
    "all"
};

#define ALL_MAX 1
arglist_t all_arg[ALL_MAX] = {
    2,all_list,NULL
};

//*************************************************************


char *focus_list[] = {
    NULL,
    "focus"
};

#define FOCUS_MAX 1
arglist_t focus_arg[FOCUS_MAX] = {
    2,focus_list,NULL
};

//*************************************************************


char *kill_list[] = {
    NULL,
    "kill"
};

#define KILL_MAX 1
arglist_t kill_arg[KILL_MAX] = {
    2,kill_list,NULL
};

//*************************************************************


char *copy_list[] = {
    NULL,
    "copy"
};

#define COPY_MAX 1
arglist_t copy_arg[COPY_MAX] = {
    2,copy_list,NULL
};

//*************************************************************


#define FILE_MAX 1
arglist_t file_arg[FILE_MAX] = {
    3,file_list,"-f"
};

#define FILE_1_MAX 1
arglist_t file_1_arg[FILE_1_MAX] = {
    2,file_list_1,"-f"
};

//*************************************************************


#define HOST_1_MAX 1
arglist_t host_1_arg[HOST_1_MAX] = {
    2,host_list_1,"-h"
};

//*************************************************************


#define CLUSTER_MAX 1
arglist_t cluster_arg[CLUSTER_MAX] = {
    3,cluster_list,"-c"
};

//*************************************************************


char *gui_list[] = {
    NULL,
    "-gui"
};

#define GUI_MAX 1
arglist_t gui_arg[GUI_MAX] = {
    2,gui_list,NULL
};

//*************************************************************


char *view_list[] = {
    NULL,
    "vtop100",
    "vtop50",
    "vtop10",
    "vexcltime",
    "vio",
    "vfpe",
    "vhwc"
};

#define VIEW_MAX 1
arglist_t view_arg[VIEW_MAX] = {
    8,view_list,NULL
};

//*************************************************************


char *metric_list[] = {
    NULL,
    "pcsamp::view1",
    "pcsamp::view1,pcsamp::view2,exptype2::view2",
    "exptype3"
};

#define METRIC_MAX 1
arglist_t metric_arg[METRIC_MAX] = {
    4,metric_list,NULL
};

//*************************************************************


char *lineno_list[] = {
    NULL,
    "27",
    "33:88"
};

#define LINENO_MAX 1
arglist_t lineno_arg[LINENO_MAX] = {
    3,lineno_list,"-l"
};

//*************************************************************


char *address_list[] = {
    NULL,
    "0x33567",
    "func1",
};

#define ADDRESS_MAX 1
arglist_t address_arg[ADDRESS_MAX] = {
    3,address_list,NULL
};

//*************************************************************


char *xx_list[] = {
    NULL,
    "6"
};

#define xx_MAX 1
arglist_t xx[xx_MAX] = {
    2,xx_list,"-x"
};

//*************************************************************

/**
 * Function: print_arg
 * 
 * Print an optionally dashed argument.
 *     
 * @param   dash    	dash arg.
 * @param   p_arg    	rest of the argument
 *
 * @return  void.
 *
 */
void
print_arg(char *dash, char* p_arg)
{
    out_stream << " ";
    if (dash && p_arg)
    	out_stream << dash << " ";
    if (p_arg)
    	out_stream << p_arg;
}

/**
 * Function: one_level
 * 
 * Single loop.
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
void
one_level(char *cmd_name,
    	    int start_1,int max_1,arglist_t *p_arglist_1
	    )
{
    int i1,i2;
    
    // 
    for (i1=0; i1<max_1; ++i1) {
    	for (i2=start_1; i2<p_arglist_1[i1].arg_count; ++i2) {
	    char *p_arg_1 = p_arglist_1[i1].arg_strings[i2];

	    	    	    out_stream << cmd_name;

	    	    	    if (p_arg_1)
			    	print_arg(p_arglist_1[i1].identifier,p_arg_1);

		    	    out_stream << endl;
	}
    }
}

/**
 * Function: two_level
 * 
 * double loop.
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
void
two_level(char *cmd_name,
    	    int start_1,int max_1,arglist_t *p_arglist_1,
    	    int start_2,int max_2,arglist_t *p_arglist_2
	    )
{
    int i1,i2,j1,j2;
    
    // 
    for (i1=0; i1<max_1; ++i1) {
    	for (i2=start_1; i2<p_arglist_1[i1].arg_count; ++i2) {
	    char *p_arg_1 = p_arglist_1[i1].arg_strings[i2];
    	    for (j1=0; j1<max_2; ++j1) {
	    	for (j2=start_2; j2<p_arglist_2[j1].arg_count; j2++) {
		    char *p_arg_2 = p_arglist_2[j1].arg_strings[j2];

	    	    	    out_stream << cmd_name;

	    	    	    if (p_arg_1)
			    	print_arg(p_arglist_1[i1].identifier,p_arg_1);
	    	    	    if (p_arg_2)
	    	    	    print_arg(p_arglist_2[j1].identifier,p_arg_2);

		    	    out_stream << endl;
	    	}
	    }
	}
    }
}


/**
 * Function: three_level
 * 
 * triple loop.
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
void
three_level(char *cmd_name,
    	    int start_1,int max_1,arglist_t *p_arglist_1,
    	    int start_2,int max_2,arglist_t *p_arglist_2,
    	    int start_3,int max_3,arglist_t *p_arglist_3
	    )
{
    int i1,i2,j1,j2,k1,k2;
    
    // 
    for (i1=0; i1<max_1; ++i1) {
    	for (i2=start_1; i2<p_arglist_1[i1].arg_count; ++i2) {
	    char *p_arg_1 = p_arglist_1[i1].arg_strings[i2];
    	    for (j1=0; j1<max_2; ++j1) {
	    	for (j2=start_2; j2<p_arglist_2[j1].arg_count; j2++) {
		    char *p_arg_2 = p_arglist_2[j1].arg_strings[j2];
		    for (k1=0; k1<max_3; ++k1) {
		    	 for (k2=start_3; k2<p_arglist_3[k1].arg_count; k2++) {
			    char *p_arg_3 = p_arglist_3[k1].arg_strings[k2];

	    	    	    out_stream << cmd_name;

	    	    	    if (p_arg_1)
			    	print_arg(p_arglist_1[i1].identifier,p_arg_1);
	    	    	    if (p_arg_2)
			    	print_arg(p_arglist_2[j1].identifier,p_arg_2);
	    	    	    if (p_arg_3)
			    	print_arg(p_arglist_3[k1].identifier,p_arg_3);

		    	    out_stream << endl;
			}
		    }
	    	}
	    }
	}
    }
}

/**
 * Function: three_level
 * 
 * triple loop.
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
void
five_level(char *cmd_name,
    	    int start_1,int max_1,arglist_t *p_arglist_1,char *arg_wrap_1,
    	    int start_2,int max_2,arglist_t *p_arglist_2,char *arg_wrap_2,
    	    int start_3,int max_3,arglist_t *p_arglist_3,char *arg_wrap_3,
    	    int start_4,int max_4,arglist_t *p_arglist_4,char *arg_wrap_4,
    	    int start_5,int max_5,arglist_t *p_arglist_5,char *arg_wrap_5
	    )
{
    int i1,i2,j1,j2,k1,k2,l1,l2,m1,m2;
    
    // 
    for (i1=0; i1<max_1; ++i1) {
    	for (i2=start_1; i2<p_arglist_1[i1].arg_count; ++i2) {
	    char *p_arg_1 = p_arglist_1[i1].arg_strings[i2];
    	    for (j1=0; j1<max_2; ++j1) {
	    	for (j2=start_2; j2<p_arglist_2[j1].arg_count; j2++) {
		    char *p_arg_2 = p_arglist_2[j1].arg_strings[j2];
		    for (k1=0; k1<max_3; ++k1) {
		    	 for (k2=start_3; k2<p_arglist_3[k1].arg_count; k2++) {
			    char *p_arg_3 = p_arglist_3[k1].arg_strings[k2];
		    	    for (l1=0; l1<max_4; ++l1) {
		    	    	for (l2=start_4; l2<p_arglist_4[l1].arg_count; l2++) {
			    	    char *p_arg_4 = p_arglist_4[l1].arg_strings[l2];
		    	    	    for (m1=0; m1<max_5; ++m1) {
		    	    	    	for (m2=start_5; m2<p_arglist_5[m1].arg_count; m2++) {
			    	    	    char *p_arg_5 = p_arglist_5[m1].arg_strings[m2];

	    	    	    	    	    out_stream << cmd_name;

	    	    	    	    	    if (p_arg_1)
			    	    	    	print_arg(p_arglist_1[i1].identifier,p_arg_1);
	    	    	    	    	    if (p_arg_2)
			    	    	    	print_arg(p_arglist_2[j1].identifier,p_arg_2);
	    	    	    	    	    if (p_arg_3)
			    	    	    	print_arg(p_arglist_3[k1].identifier,p_arg_3);
	    	    	    	    	    if (p_arg_4) {
					    	if (arg_wrap_4)
						    out_stream << " " << arg_wrap_4[0] << " ";
			    	    	    	print_arg(p_arglist_4[l1].identifier,p_arg_4);
					    	if (arg_wrap_4)
						    out_stream << " " << arg_wrap_4[1] << " ";
					    }
	    	    	    	    	    if (p_arg_5)
			    	    	    	print_arg(p_arglist_5[m1].identifier,p_arg_5);

		    	    	    	    out_stream << endl;
					}
				    }
				}
			    }
			}
		    }
	    	}
	    }
	}
    }
}

/**
 * Function: open_output
 * 
 * triple loop.
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
fstream *
open_output(char *name)
{

    fstream *p_os = new fstream(name,ios_base::out);
    if (!p_os) {
    	cerr << "cannot open " << name << endl;
	exit(1);
    }
    
    return p_os;
}

/**
 * Function: dunp_close_output
 * 
 * cleanup.
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
void
dunp_close_output(fstream *p_os)
{
    *p_os << out_stream.str();
    p_os->close();
    out_stream.str("");
}

/**
 * Function: main
 * 
 * dump command line permutations for regression test.
 *     
 *
 * @return  int.
 *
 */
int 
main()
{
    fstream *p_os = NULL;

    // EXPATTACH
    p_os = open_output("expattach.input");
    
    three_level("expattach",
    	    	0,EXPID_MAX,exp_id_arg,
    	    	0,TARGET_MAX,target_arg,
    	    	0,EXPTYPE_MAX,exptype_arg
    	    	);

    dunp_close_output(p_os);

    // EXPCLOSE
    p_os = open_output("expclose.input");

    if (!p_os) {
    	cerr << "cannot open expattach.input" <<  endl;
    }

    two_level("expclose",
    	    	0,FOCUS_MAX,focus_arg,
    	    	0,KILL_MAX,kill_arg
    	    	);
    two_level("expclose",
    	    	0,EXPID_MAX,exp_id_arg,
    	    	0,KILL_MAX,kill_arg
    	    	);
    two_level("expclose",
    	    	0,ALL_MAX,all_arg,
    	    	0,KILL_MAX,kill_arg
    	    	);

    dunp_close_output(p_os);

    // EXPCREATE
    p_os = open_output("expcreate.input");

    two_level("expcreate",
    	    	0,TARGET_MAX,target_arg,
    	    	0,EXPTYPE_MAX,exptype_arg
    	    	);

    dunp_close_output(p_os);

    // EXPDETACH
    p_os = open_output("expdetach.input");
    three_level("expdetach",
    	    	0,EXPID_MAX,exp_id_arg,
    	    	0,TARGET_MAX,target_arg,
    	    	0,EXPTYPE_MAX,exptype_arg
    	    	);

    dunp_close_output(p_os);

    // EXPDISABLE
    p_os = open_output("expdisable.input");
    one_level("expdisable",0,EXPID_MAX,exp_id_arg);
    one_level("expdisable",0,ALL_MAX,all_arg);

    dunp_close_output(p_os);

    // EXPENABLE
    p_os = open_output("expenable.input");
    one_level("expenable",0,EXPID_MAX,exp_id_arg);
    one_level("expenable",0,ALL_MAX,all_arg);

    dunp_close_output(p_os);

    // FOCUS
    p_os = open_output("expfocus.input");
    one_level("expfocus",0,EXPID_MAX,exp_id_arg);

    dunp_close_output(p_os);

    // EXPGO
    p_os = open_output("expgo.input");
    one_level("expgo",0,EXPID_MAX,exp_id_arg);
    one_level("expgo",0,ALL_MAX,all_arg);

    dunp_close_output(p_os);

    // EXPPAUSE
    p_os = open_output("exppause.input");
    one_level("exppause",0,EXPID_MAX,exp_id_arg);
    one_level("exppause",0,ALL_MAX,all_arg);

    dunp_close_output(p_os);

    // EXPRESTORE
    p_os = open_output("exprestore.input");
    one_level("exprestore",1,FILE_1_MAX,file_1_arg);

    dunp_close_output(p_os);

    // EXPSAVE
    p_os = open_output("expsave.input");
    three_level("expsave",
    	    	0,EXPID_MAX,exp_id_arg,
    	    	0,COPY_MAX,copy_arg,
    	    	1,FILE_1_MAX,file_1_arg
    	    	);

    dunp_close_output(p_os);

    // EXPVIEW
    	    	/* NOT DONE YET! */

    // EXPVIEW
    p_os = open_output("expview.input");
    five_level ("expview",
    	    	0,EXPID_MAX,exp_id_arg,NULL,
    	    	0,GUI_MAX,gui_arg,NULL,
    	    	1,VIEW_MAX,view_arg,NULL,
    	    	0,METRIC_MAX,metric_arg,"()",
    	    	0,TARGET_MAX,target_arg,NULL
    	    	);

    dunp_close_output(p_os);

    // LISTBREAKS
    p_os = open_output("listbreaks.input");
    one_level("listbreaks",
    	    	0,EXPID_MAX,exp_id_arg
    	    	);
    one_level("listbreaks",
    	    	0,ALL_MAX,all_arg
    	    	);

    dunp_close_output(p_os);

    // LISTEXP
    p_os = open_output("listexp.input");
    out_stream << "listexp" << endl;

    dunp_close_output(p_os);

    // LISTHOSTS
    p_os = open_output("listhosts.input");
    one_level("listhosts",0,EXPID_MAX,exp_id_arg);
    one_level("listhosts",0,ALL_MAX,all_arg);
    one_level("listhosts",0,CLUSTER_MAX,cluster_arg);

    dunp_close_output(p_os);

    // LISTOBJ
    p_os = open_output("listobj.input");
    two_level("listobj",0,EXPID_MAX,exp_id_arg,0,TARGET_MAX,target_arg);

    dunp_close_output(p_os);

    // LISTPIDS
    p_os = open_output("listpids.input");
    three_level("listpids",0,ALL_MAX,all_arg,0,HOST_1_MAX,host_1_arg,1,FILE_1_MAX,file_1_arg);
    three_level("listpids",0,EXPID_MAX,exp_id_arg,0,HOST_1_MAX,host_1_arg,1,FILE_1_MAX,file_1_arg);

    dunp_close_output(p_os);

    // LISTMETRICS
    p_os = open_output("listmetrics.input");
    one_level("listmetrics",0,EXPID_MAX,exp_id_arg);
    one_level("listmetrics",0,ALL_MAX,all_arg);
    one_level("listmetrics",0,EXPTYPE_MAX,exptype_arg);

    dunp_close_output(p_os);

    // LISTPARAMS
    p_os = open_output("listparams.input");
    one_level("listparams",0,EXPID_MAX,exp_id_arg);
    one_level("listparams",0,ALL_MAX,all_arg);
    one_level("listparams",0,EXPTYPE_MAX,exptype_arg);

    dunp_close_output(p_os);

    // LISTSRC
    p_os = open_output("listsrc.input");
    three_level("listsrc",0,EXPID_MAX,exp_id_arg,0,TARGET_MAX,target_arg,0,LINENO_MAX,lineno_arg);

    dunp_close_output(p_os);

    // LISTRANKS
    p_os = open_output("listranks.input");
    two_level("listranks",0,EXPID_MAX,exp_id_arg,0,TARGET_MAX,target_arg);
    two_level("listranks",0,ALL_MAX,all_arg,0,TARGET_MAX,target_arg);

    dunp_close_output(p_os);

    // LISTSTATUS
    p_os = open_output("liststatus.input");
    one_level("liststatus",0,EXPID_MAX,exp_id_arg);
    one_level("liststatus",0,ALL_MAX,all_arg);

    dunp_close_output(p_os);

    // LISTTHREADS
    p_os = open_output("listthreads.input");
    two_level("listthreads",0,EXPID_MAX,exp_id_arg,0,TARGET_MAX,target_arg);
    two_level("listthreads",0,ALL_MAX,all_arg,0,TARGET_MAX,target_arg);

    dunp_close_output(p_os);

    // LISTTYPES
    p_os = open_output("listtypes.input");
    one_level("listtypes",0,EXPID_MAX,exp_id_arg);
    one_level("listtypes",0,ALL_MAX,all_arg);

    dunp_close_output(p_os);

    // LISTVIEWS
    p_os = open_output("listviews.input");
    one_level("listviews",0,EXPID_MAX,exp_id_arg);
    one_level("listviews",0,ALL_MAX,all_arg);
    one_level("listviews",0,EXPTYPE_MAX,exptype_arg);

    dunp_close_output(p_os);

    // CLEARBREAK
    p_os = open_output("clearbreak.input");
    out_stream << "clearbreak 9" << endl;

    dunp_close_output(p_os);

    // EXIT
    p_os = open_output("exit.input");
    out_stream << "exit" << endl;

    dunp_close_output(p_os);

    // HELP

    p_os = open_output("help.input");
    out_stream << "help expcreate" << endl;

    dunp_close_output(p_os);

    // HISTORY
    p_os = open_output("history.input");
    one_level("history",0,FILE_1_MAX,file_1_arg);

    dunp_close_output(p_os);

    // LOG
    p_os = open_output("log.input");
    one_level("log",0,FILE_1_MAX,file_1_arg);

    dunp_close_output(p_os);

    // OPENGUI
    p_os = open_output("opengui.input");
    out_stream << "opengui" << endl;

    dunp_close_output(p_os);

    // PLAYBACK
    p_os = open_output("playback.input");
    one_level("playback",
    	    	1,FILE_1_MAX,file_1_arg
    	    	);

    dunp_close_output(p_os);

    // RECORD
    p_os = open_output("record.input");
    one_level("record",
    	    	0,FILE_1_MAX,file_1_arg
    	    	);

    dunp_close_output(p_os);

    // SETBREAK
    p_os = open_output("setbreak.input");
    three_level("setbreak",
    	    	0,EXPID_MAX,exp_id_arg,
    	    	0,TARGET_MAX,target_arg,
    	    	1,LINENO_MAX,address_arg
    	    	);
    dunp_close_output(p_os);


}
