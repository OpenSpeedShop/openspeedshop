/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2015 Krell Institute  All Rights Reserved.
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
  void Openss_Basic_Initialization ();
  void Openss_Basic_Termination();

//#define DEBUG_CLI 1

// This must be first include due to warnings regarding redefinition
// of certain definitions (POSIX_*)
#include "SS_Input_Manager.hxx"

#include "ToolAPI.hxx"

#include "SS_Timings.hxx"

#include <ltdl.h>
#include <pthread.h>
#include <Python.h>
#include <sstream>

#include <dlfcn.h>
// Get the full path name for the python dynamic library
#ifdef HAVE_CONFIG_H
#include "config.h" 
#endif

using namespace OpenSpeedShop::Framework;

// Global links to some windows.
CMDWID gui_window = 0;
CMDWID tli_window = 0;
CMDWID command_line_window = 0;
CMDWID Embedded_WindowID = 0;
OpenSpeedShop_Start_Modes actualCLIStartMode = SM_Unknown;

#define PTMAX 10
pthread_t phandle[PTMAX];

static bool Trying_to_terminate;
static bool Watcher_Active;
static bool need_gui;
static bool need_tli;
static bool need_command_line;
static OpenSpeedShop_Start_Modes oss_start_mode;

static bool executable_encountered;
static bool collector_encountered;
static bool read_stdin_file;

#if BUILD_CLI_TIMING
 /** Performance data Timings class instantiation handle  */
SS_Timings *cli_timing_handle ;
#endif


extern void pcli_load_messages(void);
extern void Internal_Info_Dump (CMDWID issuedbywindow);
extern void User_Info_Dump (CMDWID issuedbywindow);

/**
 * Method: Process_Command_Line()
 * 
 * 
 *     
 * @param   argc
 * @param   argv
 *
 * @return  void
 *
 * @todo    .
 *
 */
static void
Process_Command_Line (int argc, char **argv)
{
  bool found_f_option = false;
  bool found_t_option = false;
  bool found_r_option = false;
  bool found_h_option = false;
  bool found_c_option = false;
  bool found_p_option = false;
  bool found_tli = false;
  bool found_gui = false;
  bool found_batch = false;
  bool found_online = false;
  bool found_database = false;
  int i;

  // Default to offline start mode and offline
  oss_start_mode = SM_Offline;
  bool found_offline = true;

#if DEBUG_CLI
  std::cerr << "Enter Process_Command_Line, argc=" << argc << std::endl;
#endif
 /* Check the command line flags: */
  for ( i=1; i<argc; i++ ) {

    if (argv[i] == NULL) {
#if DEBUG_CLI
     std::cerr << "Process_Command_Line, continue, argv[i]=" << argv[i] << " i=" << i << std::endl;
#endif
      continue;
    }

#if DEBUG_CLI
  std::cerr << "Process_Command_Line, argv[i]=" << argv[i] << " i=" << i << std::endl;
#endif

   // Look for an indication of which input control window to open.
   // (I really do intend to allow both gui and tli windows at the same time!)
   //

    if (!strcasecmp( argv[i], "-cli")) {

#if DEBUG_CLI
      std::cerr << "Process_Command_Line, Parsing openss args, FOUND -cli: option" << std::endl;
#endif

      found_tli = true;
      need_tli = true;
      if (!found_gui) need_gui = false;
      continue;
    } else if (!strcasecmp( argv[i], "-gui")) {

#if DEBUG_CLI
      std::cerr << "Process_Command_Line, Parsing openss args, FOUND -gui: option" << std::endl;
#endif

      found_gui = true;
      need_gui = true;
      if (!found_tli) need_tli = false;
      continue;

    } else if (!strcasecmp( argv[i], "-batch")) {

#if DEBUG_CLI
      std::cerr << "Process_Command_Line, Parsing openss args, FOUND -batch: option" << std::endl;
#endif

      found_batch = true;
      found_online = false;
      found_offline = false;
      oss_start_mode = SM_Batch;
      read_stdin_file = (stdin && !isatty(fileno(stdin)));
      continue;

    } else if (!strcasecmp( argv[i], "-offline")) {

#if DEBUG_CLI
      std::cerr << "Process_Command_Line, Parsing openss args, FOUND -offline: option" << std::endl;
#endif
      found_offline = true;
      found_online = false;
      found_batch = false;
      oss_start_mode = SM_Offline;
//      read_stdin_file = (stdin && !isatty(fileno(stdin)));

    } else if (!strcasecmp( argv[i], "-online")) {

#if DEBUG_CLI
      std::cerr << "Process_Command_Line, Parsing openss args, FOUND -online: option" << std::endl;
#endif
      found_offline = false;
      found_online = true;
      found_batch = false;
      oss_start_mode = SM_Online;

    }


   /* Look for an executable description. */
    if (!strcasecmp( argv[i], "-c") ||
        !strcasecmp( argv[i], "-h") ||
        !strcasecmp( argv[i], "-f") ||
        !strcasecmp( argv[i], "-p") ||
        !strcasecmp( argv[i], "-t") ||
        !strcasecmp( argv[i], "-r")) {
#if DEBUG_CLI
     std::cerr << "Process_Command_Line, executable encountered? options" << std::endl;
#endif
      executable_encountered = true;
      need_command_line = true;

     // if the next argv is not another "-" option, skip it.
      if (!strcasecmp( argv[i], "-f") ){ 
         found_f_option = true;

#if DEBUG_CLI
         std::cerr << "Process_Command_Line, FOUND -f option" << std::endl;
#endif

      } else if (!strcasecmp( argv[i], "-p") ){ 
         found_p_option = true;
      } else if (!strcasecmp( argv[i], "-t") ){ 
         found_t_option = true;
      } else if (!strcasecmp( argv[i], "-h") ){ 
         found_h_option = true;
      } else if (!strcasecmp( argv[i], "-r") ){ 
         found_r_option = true;
      } else if (!strcasecmp( argv[i], "-c") ){ 
         found_c_option = true;
      } else {

#if DEBUG_CLI
         std::cerr << "Process_Command_Line, Parsing openss args, DID not FIND -f, -p, -t, -h, -r or -c options" << std::endl;
#endif

      }
#if DEBUG_CLI
      std::cerr << "Process_Command_Line, Parsing openss args, checking for .openss clause, found_f_option=" 
                << found_f_option << std::endl;
      std::cerr << "Process_Command_Line, Parsing openss args, checking for .openss clause, i+1=" 
                << (i+1) << " argc=" << argc << std::endl;
      std::cerr << "Process_Command_Line, Parsing openss args, checking for .openss clause, argv[i+1]=" 
                << argv[i+1] << " i=" << i << std::endl;
      std::cerr << "Process_Command_Line, Parsing openss args, checking for .openss clause, strcasecmp(argv[i+1], .openss)=" 
                << strcasecmp( argv[i+1], ".openss") << " i=" << i << std::endl;
#endif
      // If the f option was found, before skipping next argument, see if it is a database file
      if (found_f_option && 
          ((i+1)<argc) && 
          (argv[i+1] != NULL) && 
          strstr( argv[i+1], ".openss") &&
          !strstr( argv[i+1], "-f ")) {
#if DEBUG_CLI
        std::cerr << "Process_Command_Line, Parsing openss args, FOUND .openss: clause" << std::endl;
#endif
       found_database = true;


      }

      if (((i+1)<argc) &&
          (argv[i+1] != NULL) &&
          (*(argv[i+1]) != '-' )) i++;
      continue;
    } else if ( argv[i] != NULL && *(argv[i]) != '-' ) {

      collector_encountered = true;
      need_command_line = true;
#if DEBUG_CLI
      std::cerr << "Process_Command_Line, Parsing openss args, collector encountered, need command line" << std::endl;
#endif
      continue;

    }
  }

#if DEBUG_CLI
    std::cerr << "Process_Command_Line, starting checks, need_tli=" << need_tli << std::endl;
    std::cerr << "Process_Command_Line, starting_checks, need_gui=" << need_gui << std::endl;
    std::cerr << "Process_Command_Line, starting_checks, need_tli=" << need_tli << std::endl;
    std::cerr << "Process_Command_Line, starting_checks, found_batch=" << found_batch << std::endl;
    std::cerr << "Process_Command_Line, starting_checks, found_offline=" << found_offline << std::endl;
    std::cerr << "Process_Command_Line, starting_checks, found_online=" << found_online << std::endl;
    std::cerr << "Process_Command_Line, starting_checks, found_tli=" << found_tli << std::endl;
    std::cerr << "Process_Command_Line, starting_checks, found_gui=" << found_gui << std::endl;
    std::cerr << "Process_Command_Line, starting_checks, oss_start_mode=" << oss_start_mode << std::endl;
    std::cerr << "Process_Command_Line, starting_checks, found_f_option=" << found_f_option << std::endl;
    std::cerr << "Process_Command_Line, starting_checks, found_p_option=" << found_p_option << std::endl;
    std::cerr << "Process_Command_Line, starting_checks, found_t_option=" << found_t_option << std::endl;
    std::cerr << "Process_Command_Line, starting_checks, found_r_option=" << found_r_option << std::endl;
    std::cerr << "Process_Command_Line, starting_checks, found_h_option=" << found_h_option << std::endl;
    std::cerr << "Process_Command_Line, starting_checks, found_c_option=" << found_c_option << std::endl;
    std::cerr << "Process_Command_Line, starting_checks, found_database=" << found_database << std::endl;
#endif

  // If we find a dynamic option then we have to run as online, so force the mode to online
  if (found_p_option || 
      found_t_option || 
      found_h_option || 
      found_r_option || 
      found_c_option) {

    if (found_offline) {
      // Warn user that we are overriding the setting of 
      // offline mode and reassigning to online mode
        std::cerr << "WARNING: Found dynamic/online arguments (-c,-h,-p,-r,or -t) present with -offline specified.  Switching to online mode." << std::endl;
    }

    if (found_gui) {
#if DEBUG_CLI
      std::cerr << "Process_Command_Line, Parsing openss args, found_gui case1, setting need_gui=TRUE" << std::endl;
#endif
      need_gui = true;
      need_tli = false;
    } else if (found_tli) {
      need_gui = false;
      need_tli = true;
    } else {
      need_gui = false;
      need_tli = false;
    }

    found_offline = false;
    found_online = true;
    found_batch = false;
    oss_start_mode = SM_Online;

  } else if (found_f_option && 
             !found_tli && 
             !found_online && 
             !found_batch && 
             !found_database && 
             !found_gui) {
    // Assume default of offline when f option 
    // is found and no online explicit option
    need_gui = false;
    need_tli = false;
    found_offline = true;
    found_online = false;
    found_batch = false;
    oss_start_mode = SM_Offline;

#if DEBUG_CLI
    std::cerr << "Process_Command_Line, Parsing openss args, found_offline - not gui not cli" << std::endl;
#endif

  } else if (found_f_option && 
             found_online && 
             !found_tli && 
             !found_online && 
             !found_batch && 
             !found_database && 
             !found_gui) {

    // Run the online command as an immediate command 
    // if the f_option is present and the -online option 
    // is present with no -gui present
    need_gui = false;
    need_tli = false;
    found_online = true;
    found_offline = false;
    found_batch = false;
    oss_start_mode = SM_Online;

#if DEBUG_CLI
    std::cerr << "Process_Command_Line, Parsing openss args, found_online - not gui not cli" << std::endl;
#endif

  } else if (!found_online && 
             !found_offline && 
             !found_batch && 
             !found_database && 
             !found_gui && 
             !found_tli) {
   // If not specified by the user, default to -gui only mode.
#if DEBUG_CLI
      std::cerr << "Process_Command_Line, Parsing openss args, not found all case, setting need_gui=TRUE" << std::endl;
#endif
    need_gui = true;
    need_tli = false;

#if DEBUG_CLI
    std::cerr << "Process_Command_Line, Parsing openss args, found_nothing - gui by default" << std::endl;
#endif

  } else if (found_online && 
             !found_offline && 
             !found_batch && 
             !found_tli && 
             found_gui ) {
    need_gui = true;
    need_tli = false;

#if DEBUG_CLI
    std::cerr << "Process_Command_Line, Parsing openss args, found_online - gui not cli" << std::endl;
#endif

  } else if (found_online && 
             found_tli && 
             !found_offline && 
             !found_batch && 
             !found_gui) {
    need_gui = false;
    need_tli = true;

#if DEBUG_CLI
    std::cerr << "Process_Command_Line, Parsing openss args, found_offline - not gui" << std::endl;
#endif

  } else if (found_offline && 
             !found_f_option && 
             !found_tli && 
             !found_online && 
             !found_batch) {
    need_gui = true;
    need_tli = false;

#if DEBUG_CLI
    std::cerr << "Process_Command_Line, Parsing openss args, found_offline, case1 - gui not cli" << std::endl;
    std::cerr << "Process_Command_Line, Parsing openss args, found_offline, case1 - setting need_gui=TRUE" << std::endl;
#endif

  } else if (found_offline && 
             !found_f_option &&
             !found_tli && 
             !found_online && 
             !found_batch) {
    need_gui = true;
    need_tli = false;

#if DEBUG_CLI
    std::cerr << "Process_Command_Line, Parsing openss args, found_offline, case2 - gui not cli" << std::endl;
    std::cerr << "Process_Command_Line, Parsing openss args, found_offline, case2 - setting need_gui=TRUE" << std::endl;
#endif
  } else if ( (found_offline || found_online) &&
               found_f_option &&
               found_database && 
               found_tli && 
               !found_gui && 
               !found_batch) {
    need_gui = false;
    need_tli = true;
#if DEBUG_CLI
    std::cerr << "Process_Command_Line, Parsing openss args, found_offline, case4 - cli database restore" << std::endl;
    std::cerr << "Process_Command_Line, Parsing openss args, found_offline, case4 - setting need_gui=FALSE" << std::endl;
#endif

  } else if ( (found_offline || found_online) &&
               found_f_option &&
               found_database && 
               !found_tli && 
               !found_batch) {
    need_gui = true;
    need_tli = false;

#if DEBUG_CLI
    std::cerr << "Process_Command_Line, Parsing openss args, found_offline, case3 - gui database restore" << std::endl;
    std::cerr << "Process_Command_Line, Parsing openss args, found_offline, case3 - setting need_gui=TRUE" << std::endl;
#endif


  } else {

#if DEBUG_CLI
    std::cerr << "Process_Command_Line, Parsing openss args, FELL THROUGH" << std::endl;
#endif
  }


#if DEBUG_CLI
    std::cerr << "EXIT Process_Command_Line, need_gui=" << need_gui << std::endl;
    std::cerr << "EXIT Process_Command_Line, need_tli=" << need_tli << std::endl;
    std::cerr << "EXIT Process_Command_Line, found_batch=" << found_batch << std::endl;
    std::cerr << "EXIT Process_Command_Line, found_offline=" << found_offline << std::endl;
    std::cerr << "EXIT Process_Command_Line, found_online=" << found_online << std::endl;
    std::cerr << "EXIT Process_Command_Line, found_tli=" << found_tli << std::endl;
    std::cerr << "EXIT Process_Command_Line, found_gui=" << found_gui << std::endl;
    std::cerr << "EXIT Process_Command_Line, oss_start_mode=" << oss_start_mode << std::endl;
    std::cerr << "EXIT Process_Command_Line, found_f_option=" << found_f_option << std::endl;
    std::cerr << "EXIT Process_Command_Line, found_p_option=" << found_p_option << std::endl;
    std::cerr << "EXIT Process_Command_Line, found_t_option=" << found_t_option << std::endl;
    std::cerr << "EXIT Process_Command_Line, found_r_option=" << found_r_option << std::endl;
    std::cerr << "EXIT Process_Command_Line, found_h_option=" << found_h_option << std::endl;
    std::cerr << "EXIT Process_Command_Line, found_c_option=" << found_c_option << std::endl;
#endif

    // Define the start mode found for use in decisions to be made later in the cli
    actualCLIStartMode = oss_start_mode;

}

extern "C" void initPY_Input ();
/**
 * Method: Initial_Python()
 * 
 * 
 *     
 * @param   void
 *
 * @return  void
 *
 * @todo    .
 *
 */
static void
Initial_Python ()
{
    void *handle;
    const char* initialization_filename = "init.py";
    std::string python_dso_name = PYTHON_FP_LIB_NAME;
 
#if DEBUG_CLI
    std::cerr << "ENTERING Initial_Python" << std::endl;
#endif

    // Insure the libltdl user-defined library search path has been set
    Assert(lt_dlgetsearchpath() != NULL);
    
    // Iterate over individual directories in that search path
    std::string path = lt_dlgetsearchpath();
    for(std::string::size_type 
	    i = path.find_first_not_of(':', 0), next = path.find(':', i);
	i != std::string::npos;
	i = path.find_first_not_of(':', next), next = path.find(':', i)) {
	
	// Extract this directory
	Path directory = 
	    path.substr(i, (next == std::string::npos) ? next : next - i);
	
	// Assemble the candidate and check if it is a file
	Path candidate = directory + Path(initialization_filename);
	if(candidate.isFile()) {
	    
#if DEBUG_CLI
           std::cerr << "IN Initial_Python, python_dso_name.c_str()=" << python_dso_name.c_str() << std::endl;
#endif
            handle = dlopen(python_dso_name.c_str(), RTLD_LAZY | RTLD_GLOBAL);
            if (!handle) {
               fprintf (stderr, "openss error: python dynamic shared object failed to load: %s\n", dlerror());
               // exit(1);
            }

 
	    // Initialize Python
	    Py_Initialize();
	    
	    // Load the Py_Input module into Python
	    initPY_Input();

            // Initialize the port used by the DPCL daemons
            std::string dpcl_port = "DpcldListenerPort = '" + 
		Experiment::getDpcldListenerPort() + "'";
            PyRun_SimpleString(dpcl_port.c_str());

	    // Initialize the installation directory
	    char buffer[PATH_MAX];
	    memset(buffer, 0, sizeof(buffer));
	    std::ostringstream proc_entry;
	    proc_entry << "/proc/" << getpid() << "/exe";
	    readlink(proc_entry.str().c_str(), buffer, sizeof(buffer) - 1);
	    Path install_dir = Path(buffer).removeLast().removeLast();
	    PyRun_SimpleString((std::string("OpenssInstallDir = '") +
				install_dir + std::string("'")).c_str());

	    // Load the initialization file into Python
	    FILE* fp = fopen(candidate.c_str(), "r");
	    Assert(fp != NULL);

#if DEBUG_CLI
            std::cerr << "executing init.py, candidate.c_str()=" << candidate.c_str()
	              << " candidate.getBaseName().c_str()=" << candidate.getBaseName().c_str() << std::endl;
#endif

	    PyRun_SimpleFile(fp, candidate.getBaseName().c_str());
	    Assert(fclose(fp) == 0);

	    // Return successfully to the caller
	    return;
	    
	}	
    }

    // Failed to find the initialization file if we get this far
    std::cerr << "Unable to locate \"" << initialization_filename
	      << "\" necessary to initialize python." << std::endl;
    exit(EXIT_FAILURE);
}

bool Start_COMMAND_LINE_Mode (CMDWID my_window, 
                              int argc, 
                              char ** argv, 
                              bool need_cli, 
                              bool need_gui, 
                              OpenSpeedShop_Start_Modes oss_start_mode);
void SS_Direct_stdin_Input (void *attachtowindow);
void SS_Watcher ();

extern "C"
{
/**
 * Method: usage()
 * 
 * 
 *     
 * @param   void
 *
 * @return  void
 *
 * @todo    .
 *
 */
  void usage()
  {
    printf("openss, version - prepreprepreRelease 0.01\n");
    printf("usage: openss [-f executable] [-h host] [-x experiment_name]\n");
    printf("              [-x experiment_name]\n");
    printf("              [-p process_ID [-r thread_rank] ]\n");
    printf("              [-a \"command line args\"]\n\n");
    printf("  -f : executable being measured\n");
    printf("  -h : host to locate target executable or process\n");
    printf("  -x : experiement name\n");
    printf("      o pssamp   - Actual CPU time at the source line, machine instruction,\n");
    printf("                   and function levels by sampling the program counter at\n");
    printf("                   10 or 1-millisecond intervals. See “PC Sampling\n");
    printf("                   Wizard.\n");
    printf("\n");
    printf("      o usertime - Inclusive and exclusive CPU time for each function by\n");
    printf("                   sampling the callstack at 30-millisecond intervals.\n");
    printf("                   See   User Time Wizard.\n");
    printf("\n");
    printf("      o fpe      - All floating-point exceptions, with the exception type\n");
    printf("                   and the call stack at the time of the exception. See\n");
    printf("                   FPE Tracing Wizard.\n");
    printf("\n");
    printf("      o hwc/hwctime\n");
    printf("                 - Counts at the source line, machine instruction, and\n");
    printf("                   function levels of various hardware events, including:\n");
    printf("                   clock cycles, graduated instructions, primary\n");
    printf("                   instruction cache misses, secondary instruction cache\n");
    printf("                   misses, primary data cache misses, secondary data\n");
    printf("                   cache misses, translation lookaside buffer (TLB)\n");
    printf("                   misses, and graduated floating-point instructions.\n");
    printf("                   See HW Counter Wizard.\n");
    printf("\n");
    printf("      o io/iot   - Traces the following I/O system calls: read, readv,\n");
    printf("                   write, writev, open, close, dup, pipe, creat.\n");
    printf("                   See IO Wizard.\n");
    printf("\n");
    printf("      o mpi/mpit/mpiotf\n");
    printf("                 - Traces calls to various MPI routines.\n");
    printf("                   See MPI Wizard.\n");
    printf("  -p : pid of target process\n");
    printf("  -r : rank of \n");
    printf("  -a : quoted command line arguments to be passed to executable.\n");
    printf("\n");
    printf("Examples:\n");
    printf("  To start Open|Speedshop simply type:\n");
    printf("  $ openss\n");
    printf("\n\n");
    printf("  To load an executable into Open|Speedshop\n");
    printf("  for eventual capturing of data:\n");
    printf("  $ openss -f a.out\n");
    printf("\n\n");
    printf("  To attach to a running executable:\n");
    printf("  $ openss -p 123456\n");
    printf("\n\n");
    printf("  To load an executable to run a simple timing (pcSampling) experiment:\n");
    printf("  $ openss -f a.out -x pcsamp\n");
  }

/**
 * Method: cli_terminate()
 * 
 * 
 *     
 * @param   void
 *
 * @return  void
 *
 * @todo    .
 *
 */
  void
  cli_terminate ()
  {
    if (tli_window != 0) {

     // Before we do anything else -
     // Stop async read loop for xterm window
      //pthread_cancel (phandle[1]);
      pthread_kill (phandle[1], SIGUSR1);
      usleep (10000);
    }

   // Close Python
#ifdef PYTHON_GC_ON
    Py_Finalize ();
#endif

   // Close allocated input windows.
    if (gui_window != 0) {

      CMDWID w = gui_window;
      extern void killTheGUI();
      killTheGUI();
      pthread_cancel (phandle[2]);
      gui_window = 0;
      Window_Termination(w);
    }

    if (tli_window != 0) {

     // Stop async read loop for xterm window
     //  pthread_cancel (phandle[1]);

      CMDWID w = tli_window;
      tli_window = 0;
      Window_Termination(w);
    }

    if (command_line_window != 0) {
      CMDWID w = command_line_window;
      command_line_window = 0;
      Window_Termination(w);
    } 

    // If the timing of CLI events is enabled then process 
    // the gathered performance data

#if BUILD_CLI_TIMING
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {

      cli_timing_handle->processTimingEventEnd( SS_Timings::cliAllStart, 
                                                SS_Timings::cliAllCount, 
                                                SS_Timings::cliAllMax, 
                                                SS_Timings::cliAllMin, 
                                                SS_Timings::cliAllTotal, 
                                                SS_Timings::cliAllEnd);

      // Call performance data output routine to print report
      cli_timing_handle->CLIPerformanceStatistics();
    }
#endif

  }

/**
 * Method: catch_signal()
 * 
 * 
 *     
 * @param   sig
 * @param   error_num
 *
 * @return  void
 *
 * @todo    .
 *
 */
  static void
  catch_signal (int sig, int error_num)
  {

    static bool processing_signal = false;

    if (sig == SIGQUIT) {
      // abort on user signal for CNTRL-\ 
      abort();
    } else if (sig != SIGINT) {

     // If not a user initiated signal, try a normal shutdown.
      if (Trying_to_terminate) {
        std::cerr << "Error during termination - " << sig << " " <<  error_num <<  std::endl;
        abort();
      }

      Trying_to_terminate = true;
     // the folowing lines are for debugging
    	std::cerr << "catch_signal " << sig << std::endl;
    	// Internal_Info_Dump(1);
    	// User_Info_Dump(1);

      try {
        cli_terminate ();
        Openss_Basic_Termination();
      }
      catch(const Exception& error) {
        std::cerr << "catch secondary exception: " << error.getDescription() << std::endl;
      }
    }
    exit (1);
  }

/**
 * Method: cli_init()
 * 
 * 
 *     
 * @param   argc
 * @param   argv
 *
 * @return  int
 *
 * @todo    .
 *
 */
  int
  cli_init(int argc, char **argv)
  {
    try {

#if DEBUG_CLI
      std::cerr << "ENTER cli_init" << std::endl;
#endif

#if BUILD_CLI_TIMING
      cli_timing_handle = new SS_Timings();
      cli_timing_handle->in_expCreate(false);
      cli_timing_handle->in_expAttach(false);

    // Gather performance information on the cli's basic initialization
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::cliBasicInitStart] = Time::Now();
    }
#endif

#if DEBUG_CLI
      std::cerr << "IN cli_init, calling Openss_Basic_Initialization()" << std::endl;
#endif
     // Basic Initialization
      Openss_Basic_Initialization();

      if (!OPENSS_LIMIT_SIGNAL_CATCHING) {
       // Optionally, set up to catch bad errors
        SET_SIGNAL (SIGILL, catch_signal);
        SET_SIGNAL (SIGFPE, catch_signal);
        SET_SIGNAL (SIGBUS, catch_signal);
        SET_SIGNAL (SIGSEGV, catch_signal);
        SET_SIGNAL (SIGSYS, catch_signal);
        // SET_SIGNAL (SIGPIPE, catch_signal);
        // SET_SIGNAL (SIGCLD, catch_signal);
      }

     // Always catch user signals.
      SET_SIGNAL (SIGINT, catch_signal); // CNTRL-C
      SET_SIGNAL (SIGQUIT, catch_signal); // CNTRL-\ 

     // Process the execution time arguments for openss.
      int i;
      ArgStruct *argStruct = new ArgStruct(argc, (const char **) argv);
      pid_t my_pid = getpid();
      char HostName[MAXHOSTNAMELEN+1];

      if (gethostname ( &HostName[0], MAXHOSTNAMELEN)) {
        std::cerr << "ERROR: can not retrieve host name\n";
        abort ();
      }
      
      // Sanity check that the command enum is not broken.
      // See oss_cmd_enum definition in SS_Parse_Result.hxx.

      for (i=1;i<CMD_MAX;++i) {

      	if (i != OpenSpeedShop::cli::cmd_desc[i].ndx) {
        	    std::cerr << "ERROR: cmd_desc array out of synch with oss_cmd_enum \n";
        	    std::cerr << "       See oss_cmd_enum definition in SS_Parse_Result.hxx \n";
        	    abort ();
	}
      }

      read_stdin_file = (stdin && !isatty(fileno(stdin)));
      executable_encountered = false;
      collector_encountered = false;

#if BUILD_CLI_TIMING
    // Process the performance information on the cli's basic initialization
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->processTimingEventEnd( SS_Timings::cliBasicInitStart,
                                                   SS_Timings::cliBasicInitCount,
                                                   SS_Timings::cliBasicInitMax,
                                                   SS_Timings::cliBasicInitMin,
                                                   SS_Timings::cliBasicInitTotal,
                                                   SS_Timings::cliBasicInitEnd);
    }
#endif

#if BUILD_CLI_TIMING
    // Gather performance information on the cli's command line and python initialization
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::cliCmdLinePythonStart] = Time::Now();
    }
#endif

#if DEBUG_CLI
      std::cerr << "IN cli_init, calling Process_Command_Line()" << std::endl;
#endif

      Process_Command_Line (argc, argv);

#if DEBUG_CLI
      std::cerr << "IN cli_init, calling pcli_load_messages()" << std::endl;
#endif

     // Load in pcli messages into message czar
      pcli_load_messages();

#if DEBUG_CLI
      std::cerr << "IN cli_init, calling Initial_Python()" << std::endl;
#endif

     // Open the Python interpreter.
      Initial_Python ();

#if BUILD_CLI_TIMING
    // Process the performance information on the cli's command line and python initialization
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->processTimingEventEnd( SS_Timings::cliCmdLinePythonStart,
                                                   SS_Timings::cliCmdLinePythonCount,
                                                   SS_Timings::cliCmdLinePythonMax,
                                                   SS_Timings::cliCmdLinePythonMin,
                                                   SS_Timings::cliCmdLinePythonTotal,
                                                   SS_Timings::cliCmdLinePythonEnd);
    } 
#endif

#if BUILD_CLI_TIMING
    // Gather performance information on the cli's window initialization
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::cliWindowInitStart] = Time::Now();
    }
#endif

#if DEBUG_CLI
      std::cerr << "In cli_init, Checking need_command_line=" << need_command_line <<  " read_stdin_file=" << read_stdin_file << std::endl;
#endif

     // Create the input windows that we will need.
      if (need_command_line || read_stdin_file) {
        command_line_window = Default_Window ("COMMAND_LINE",
        	    	    	    	    	    &HostName[0],
					      my_pid,
					      0,
					      false);
      }

      if (need_tli) {
        tli_window = TLI_Window ("TLI",&HostName[0],my_pid,0,true);
      }

#if DEBUG_CLI
      std::cerr << "In cli_init, Checking need_gui=" << need_gui <<  " my_pid=" << my_pid << std::endl;
#endif
      if (need_gui) {
        gui_window = GUI_Window ("GUI",&HostName[0],my_pid,0,true);
#if DEBUG_CLI
        std::cerr << "In cli_init, We DO need_gui=" << need_gui <<  " gui_window=" << gui_window << std::endl;
#endif
      }

#if DEBUG_CLI
      std::cerr << "In cli_init, Checking command_line_window=" << command_line_window <<  std::endl;
#endif

     // Complete set up for each input window.
      if (command_line_window != 0) {
#if DEBUG_CLI
        std::cerr << "Calling Start_Command_Line_Mode need_gui=" <<  need_gui << " need_tli=" << need_tli << std::endl;
#endif
       // Move the command line options to an input control window.
        if ( !Start_COMMAND_LINE_Mode( command_line_window, 
                                       argc, 
                                       argv, 
                                       need_tli,
                                       need_gui,
                                       oss_start_mode) ) {
          return -1;
        }

      } else if (oss_start_mode == SM_Batch && (argc <= 2) && !read_stdin_file) {

        std::cerr << "Missing command line arguments\n";
        return -1;

      }

#if DEBUG_CLI
        std::cerr << "In cli_init, Checking need_tli=" <<  need_tli << std::endl;
#endif

      if (need_tli) {

#if DEBUG_CLI
        std::cerr << "In cli_init, Calling pthread_create for SS_Direct_stdin_Input" <<  std::endl;
#endif

       // Start up the Text Line Interface to read from the keyboard.
        int stat = pthread_create(&phandle[1], 
        	    	    	    	0, 
				(void   *(*)(void *))SS_Direct_stdin_Input,
				(void   *)tli_window);

#if DEBUG_CLI
        std::cerr << "In cli_init, After Calling pthread_create for SS_Direct_stdin_Input" << phandle[1] << std::endl;
#endif

      } // end need_tli

    // Process the performance information on the cli's 
    // command line and python initialization

#if BUILD_CLI_TIMING
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->processTimingEventEnd( SS_Timings::cliWindowInitStart,
                                                   SS_Timings::cliWindowInitCount,
                                                   SS_Timings::cliWindowInitMax,
                                                   SS_Timings::cliWindowInitMin,
                                                   SS_Timings::cliWindowInitTotal,
                                                   SS_Timings::cliWindowInitEnd);
    }
#endif

#if DEBUG_CLI
      std::cerr << "In cli_init, Calling Start_Command_Line, need_gui? " << need_gui <<  " \n" << std::endl;
      std::cerr << "In cli_init, Calling Start_Command_Line, gui_window= " << gui_window <<  " \n" << std::endl;
#endif

#if BUILD_CLI_TIMING
      // Gather performance information on the cli's gui loading
      if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
           cli_timing_handle->cli_perf_data[SS_Timings::cliGuiLoadStart] = Time::Now();
      }
#endif

      if (need_gui) {

        // The following is a timing hack -
        // if the TLI window hasn't been opened or
        // didn't specify async input, the input
        // routines may think they are at the end of
        // file before the GUI can open and define an
        // async input window. The hack is to define a
        // dummy async window before python starts. We
        // will need to sort this out at some point in
        // the future.

        argStruct->addArg("-gui");
        argStruct->addArg("-wid");

#if DEBUG_CLI
        std::cerr << "In cli_init, Calling loadTheGUI, gui_window? " << gui_window <<  " \n" << std::endl;
#endif

#if 1
        char buffer[10];
        sprintf(buffer, "%lld", gui_window);
        argStruct->addArg(buffer);
#else
        if (gui_window == 1) {
          argStruct->addArg("1");
        } else if (gui_window == 2) {
          argStruct->addArg("2");
        } else {
          argStruct->addArg("0");
        } 
#endif
       // The gui will be started in a pthread and do it's own initialization.
        extern void loadTheGUI(ArgStruct *);
//        loadTheGUI((ArgStruct *)NULL); // argStruct);
        loadTheGUI((ArgStruct *)argStruct); // NULL);
      }

      // Process the performance information on the cli's 
      // command line and python initialization

#if BUILD_CLI_TIMING
      if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
           cli_timing_handle->processTimingEventEnd( SS_Timings::cliGuiLoadStart,
                                                     SS_Timings::cliGuiLoadCount,
                                                     SS_Timings::cliGuiLoadMax,
                                                     SS_Timings::cliGuiLoadMin,
                                                     SS_Timings::cliGuiLoadTotal,
                                                     SS_Timings::cliGuiLoadEnd);
      }
#endif

     // Fire off Python.
      PyRun_SimpleString( "myparse.do_scripting_input ()\n");

     // When Python exits, terminate SpeedShop:
      Trying_to_terminate = true;
      try {
        cli_terminate ();
        Openss_Basic_Termination();
      }

      catch(const Exception& error) {
        std::cerr << "catch error during termination: " << error.getDescription() << std::endl;
      }

     // Release allocated space.
      delete argStruct;

     // exit from openss.
      exit(0);
    }

    catch (std::bad_alloc) {
      std::cerr << "ERROR: A Memory Allocation Error Has Occurred" << std::endl;
      abort();
    }

  }

/**
 * Method: lt_ptr()
 * 
 * When the cli requests to bring up a new GUI
 * thread. NOTE: One will be brought up for each
 * invocation. This routine is called to find the
 * entry point an load it. This way the cli, when
 * running alone, doesn't have to take the hit of
 * loading in all those nasty, ugly, big GUI
 * libraries. We load the gui as a dynamic library.
 * The GUI will then start it's own thread and fire
 * up a copy of the gui (that will talk with the
 * cli).
 *     
 * @param   dl_gui_kill_routine
 *
 * @return  void
 *
 * @todo    .
 *
 */
  static lt_ptr (*dl_gui_kill_routine)();
  void
  loadTheGUI(ArgStruct *argStruct)
  {
    char gui_plugin_file[2048];
    const char *gui_dl_name = getenv("OPENSS_GUI_RELOCATABLE_NAME");
    const char *gui_entry_point = getenv("OPENSS_GUI_ENTRY_POINT");
    const char *gui_exit_point = getenv("OPENSS_GUI_EXIT_POINT");
    // char *plugin_directory = getenv("OPENSS_PLUGIN_PATH");
    //
#if DEBUG_CLI
      std::cerr << "ENTER loadTheGUI, setting OPENSS_SAVE_VIEWS_FOR_REUSE to false for GUI only save/reuse" << std::endl;
#endif
    // Turn off saving and reusing view code in the CLI when the GUI is up.  The GUI has its own methods.
    OPENSS_SAVE_VIEWS_FOR_REUSE = false;


    // Insure the libltdl user-defined library search path has been set
    Assert(lt_dlgetsearchpath() != NULL);

    // Load GUI library
    if( !gui_dl_name ) gui_dl_name = "libopenss-gui";
    if( !gui_entry_point ) gui_entry_point = "gui_init";
    if( !gui_exit_point ) gui_exit_point = "gui_exit";

#if DEBUG_CLI
      std::cerr << "IN loadTheGUI, attempting to load the GUI, gui_dl_name=" << gui_dl_name << std::endl;
#endif
  
    lt_dlhandle dl_gui_object = lt_dlopenext((const char *)gui_dl_name);
    if( dl_gui_object == NULL ) {
      std::cerr << "ERROR: can not load GUI - " << lt_dlerror() << std::endl;
      exit(EXIT_FAILURE);
    }

#if DEBUG_CLI
      std::cerr << "IN loadTheGUI, attempting to init the GUI" << std::endl;
#endif

    lt_ptr (*dl_gui_init_routine)(void *, pthread_t *);
    dl_gui_init_routine = (lt_ptr (*)(void *, pthread_t *))lt_dlsym(dl_gui_object, gui_entry_point);
    if( dl_gui_init_routine == NULL )
    {
      std::cerr << "ERROR: can not initialize GUI - " << lt_dlerror() << std::endl;
      exit(EXIT_FAILURE);
    }

    dl_gui_kill_routine = (lt_ptr (*)())lt_dlsym(dl_gui_object, gui_exit_point);
    if( dl_gui_kill_routine == NULL )
    {
      std::cerr << "ERROR: can find GUI exit routine - " << lt_dlerror() << std::endl;
      exit(EXIT_FAILURE);
    }
  
#if DEBUG_CLI
      std::cerr << "IN loadTheGUI, before attempting to call the GUI phandle." << std::endl;
#endif
    //    pthread_t gui_phandle;
    (*dl_gui_init_routine)((void *)argStruct, &phandle[2]);
#if DEBUG_CLI
      std::cerr << "EXIT/IN loadTheGUI, after attempting to call the GUI phandle." << std::endl;
#endif
  }

/**
 * Method: killTheGUI()
 * 
 * 
 *     
 * @param   void
 *
 * @return  void
 *
 * @todo    .
 *
 */
  void
  killTheGUI()
  {
    (*dl_gui_kill_routine)();
  }

} // extern "C"

/**
 * Method: Openss_Basic_Initialization()
 * 
 * 
 *     
 * @param   argc
 * @param   argv
 *
 * @return  int
 *
 * @todo    .
 *
 */
  void
  Openss_Basic_Initialization ()
  {
   // Basic Initialization
    Trying_to_terminate = false;
    Watcher_Active = false;
    need_gui = false;
    need_tli = false;
    oss_start_mode = SM_Unknown;
    need_command_line = false;
    gui_window = 0;
    tli_window = 0;
    command_line_window = 0;

   // Read in user alterable configuration options.
    SS_Configure ();

   // Start up the Command line processor.
    Commander_Initialization ();

   // Define Built-In Views
    SS_Init_BuiltIn_Views();
    SS_Load_View_plugins();


   // Start a thread to look for special conditions.
    if (pthread_create(&phandle[0],
                        0,
                        (void *(*)(void *))SS_Watcher,
                        NULL) != 0) {
      Watcher_Active = true;
    }
  }

/**
 * Method: Openss_Basic_Termination()
 * 
 * 
 *     
 * @param   argc
 * @param   argv
 *
 * @return  int
 *
 * @todo    .
 *
 */
  void
  Openss_Basic_Termination()
  {
    Trying_to_terminate = true;
    if (Watcher_Active) {
     // Get rid of thread that looks for special conditions.
      Watcher_Active = false;
      pthread_kill (phandle[0], SIGUSR1);
    }

   // Delete any Custom View definitiions.
    SS_cvClear_All ();

   // Close any open experiments.
    Experiment_Termination ();

   // Unload plugin views.
    SS_Remove_View_plugins ();

   // Close down the CLI.
    Commander_Termination ();
  }
