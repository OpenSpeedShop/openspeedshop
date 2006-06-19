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
  void Openss_Basic_Initialization ();
  void Openss_Basic_Termination();

#include "ToolAPI.hxx"
#include "SS_Input_Manager.hxx"

#include <ltdl.h>
#include <pthread.h>
#include <Python.h>

using namespace OpenSpeedShop::Framework;

// Global links to some windows.
CMDWID gui_window = 0;
CMDWID tli_window = 0;
CMDWID command_line_window = 0;
CMDWID Embedded_WindowID = 0;

#define PTMAX 10
pthread_t phandle[PTMAX];

static bool Trying_to_terminate;
static bool Watcher_Active;
static bool need_gui;
static bool need_tli;
static bool need_batch;
static bool need_command_line;

static bool executable_encountered;
static bool collector_encountered;
static bool read_stdin_file;

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
  bool found_tli = false;
  bool found_gui = false;
  bool found_batch = false;
  int i;

 /* Check the command line flags: */
  for ( i=1; i<argc; i++ ) {

    if (argv[i] == NULL) {
      continue;
    }

   // Look for an indication of which input control window to open.
   // (I really do intend to allow both gui and tli windows at the same time!)
    if (!strcasecmp( argv[i], "-cli")) {
      found_tli = true;
      need_tli = true;
      if (!found_gui) need_gui = false;
      continue;
    } else if (!strcasecmp( argv[i], "-gui")) {
      found_gui = true;
      need_gui = true;
      if (!found_tli) need_tli = false;
      continue;
    } else if (!strcasecmp( argv[i], "-batch")) {
      found_batch = true;
      need_batch = true;
      read_stdin_file = (stdin && !isatty(fileno(stdin)));
      continue;
    }

   /* Look for an executable description. */
    if (!strcasecmp( argv[i], "-c") ||
        !strcasecmp( argv[i], "-h") ||
        !strcasecmp( argv[i], "-f") ||
        !strcasecmp( argv[i], "-p") ||
        !strcasecmp( argv[i], "-t") ||
        !strcasecmp( argv[i], "-r")) {
      executable_encountered = true;
      need_command_line = true;
     // if the next argv is not another "-" option, skip it.
      if (((i+1)<argc) &&
          (argv[i+1] != NULL) &&
          (*(argv[i+1]) != '-' )) i++;
      continue;
    } else if ( argv[i] != NULL && *(argv[i]) != '-' ) {
      collector_encountered = true;
      need_command_line = true;
      continue;
    }
  }

  if (!found_batch && !found_gui && !found_tli) {
   // If not specified by the user, default to -gui only mode.
    need_gui = true;
    need_tli = false;
  }
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
    const char* preparser_filename = "SS_Preparse.py";

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
	Path candidate = directory + Path(preparser_filename);
	if(candidate.isFile()) {
	    
	    // Initialize Python
	    Py_Initialize();
	    
	    // Load the Py_Input module into Python
	    initPY_Input();

	    // Load the preparser into Python
	    FILE* fp = fopen(candidate.c_str(), "r");
	    Assert(fp != NULL);
	    PyRun_SimpleFile(fp, preparser_filename);
	    Assert(fclose(fp) == 0);

            // Initialize the port used by the DPCL daemons
            std::string dpcl_port = "DpcldListenerPort = '" + Experiment::getDpcldListenerPort() + "'";
            PyRun_SimpleString(dpcl_port.c_str());
	    
	    // Return successfully to the caller
	    return;
	    
	}	
    }

    // Failed to find the preparser if we get this far
    std::cerr << "Unable to locate \"" << preparser_filename
	      << "\" necessary to initialize python." << std::endl;
    exit(EXIT_FAILURE);
}

bool Start_COMMAND_LINE_Mode (CMDWID my_window, int argc, char ** argv, bool batch_mode);
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
    printf("      o hwc      - Counts at the source line, machine instruction, and\n");
    printf("                   function levels of various hardware events, including:\n");
    printf("                   clock cycles, graduated instructions, primary\n");
    printf("                   instruction cache misses, secondary instruction cache\n");
    printf("                   misses, primary data cache misses, secondary data\n");
    printf("                   cache misses, translation lookaside buffer (TLB)\n");
    printf("                   misses, and graduated floating-point instructions.\n");
    printf("                   See HW Counter Wizard.\n");
    printf("\n");
    printf("      o io       - Traces the following I/O system calls: read, readv,\n");
    printf("                   write, writev, open, close, dup, pipe, creat.\n");
    printf("                   See IO Wizard.\n");
    printf("\n");
    printf("      o mpi      - Traces calls to various MPI routines.\n");
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
    if (tli_window != 0)
    {
     // Before we do anything else -
     // Stop async read loop for xterm window
      //pthread_cancel (phandle[1]);
      pthread_kill (phandle[1], SIGUSR1);
      usleep (10000);
    }

   // Close Python
    Py_Finalize ();

   // Close allocated input windows.
    if (gui_window != 0)
    {
      CMDWID w = gui_window;
      extern void killTheGUI();
      killTheGUI();
      pthread_cancel (phandle[2]);
      gui_window = 0;
      Window_Termination(w);
    }
    if (tli_window != 0)
    {
     // Stop async read loop for xterm window
     //  pthread_cancel (phandle[1]);

      CMDWID w = tli_window;
      tli_window = 0;
      Window_Termination(w);
    }
    if (command_line_window != 0)
    {
      CMDWID w = command_line_window;
      command_line_window = 0;
      Window_Termination(w);
    }
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
        cerr << "Error during termination - " << sig << " " <<  error_num <<  std::endl;
        abort();
      }
      Trying_to_terminate = true;
     // the folowing lines are for debugging
    	cerr << "catch_signal " << sig << std::endl;
    	// Internal_Info_Dump(1);
    	// User_Info_Dump(1);

      try {
        cli_terminate ();
        Openss_Basic_Termination();
      }
      catch(const Exception& error) {
        cerr << "catch secondary exception: " << error.getDescription() << std::endl;
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
      ArgStruct *argStruct = new ArgStruct(argc, argv);
      pid_t my_pid = getpid();
      char HostName[MAXHOSTNAMELEN+1];
      if (gethostname ( &HostName[0], MAXHOSTNAMELEN)) {
        cerr << "ERROR: can not retrieve host name\n";
        abort ();
      }
      
      // Sanity check that the command enum is not broken.
      // See oss_cmd_enum definition in SS_Parse_Result.hxx.
      for (i=1;i<CMD_MAX;++i) {
      	if (i != OpenSpeedShop::cli::cmd_desc[i].ndx) {
        	    cerr << "ERROR: cmd_desc array out of synch with oss_cmd_enum \n";
        	    cerr << "       See oss_cmd_enum definition in SS_Parse_Result.hxx \n";
        	    abort ();
	}
      }

      read_stdin_file = (stdin && !isatty(fileno(stdin)));
      executable_encountered = false;
      collector_encountered = false;
      Process_Command_Line (argc, argv);

     // Load in pcli messages into message czar
      pcli_load_messages();

     // Open the Python interpreter.
      Initial_Python ();

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
      if (need_gui) {
        gui_window = GUI_Window ("GUI",&HostName[0],my_pid,0,true);
      }

     // Complete set up for each input window.
      if (command_line_window != 0) {
       // Move the command line options to an input control window.
        if ( !Start_COMMAND_LINE_Mode( command_line_window, 
        	    	    	    	     argc, 
				       argv, 
				       need_batch) ) {
          return -1;
        }
      } else if (need_batch && (argc <= 2) && !read_stdin_file) {
        cerr << "Missing command line arguments\n";
        return -1;
      }

      if (need_tli)
      {
       // Start up the Text Line Interface to read from the keyboard.
        int stat = pthread_create(&phandle[1], 
        	    	    	    	0, 
				(void   *(*)(void *))SS_Direct_stdin_Input,
				(void   *)tli_window);
      }

      if (need_gui)
      {

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
        char buffer[10];
        sprintf(buffer, "%d", gui_window);
        argStruct->addArg(buffer);
       // The gui will be started in a pthread and do it's own initialization.
        extern void loadTheGUI(ArgStruct *);
//        loadTheGUI((ArgStruct *)NULL); // argStruct);
        loadTheGUI((ArgStruct *)argStruct); // NULL);
      }

     // Fire off Python.
      // OPENSS_ALLOW_PYTHON_COMMANDS = 0;
      if (OPENSS_ALLOW_PYTHON_COMMANDS)
      	PyRun_SimpleString( "myparse.do_scripting_input ()\n");
      else
      	PyRun_SimpleString( "myparse.do_flat_input ()\n");

     // When Python exits, terminate SpeedShop:
      Trying_to_terminate = true;
      try {
        cli_terminate ();
        Openss_Basic_Termination();
      }
      catch(const Exception& error) {
        cerr << "catch error during termination: " << error.getDescription() << std::endl;
      }

     // Release allocated space.
      delete argStruct;

     // exit from openss.
      exit(0);
    }
    catch (std::bad_alloc) {
      cerr << "ERROR: A Memory Allocation Error Has Occurred" << std::endl;
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
    char *gui_dl_name = getenv("OPENSS_GUI_RELOCATABLE_NAME");
    char *gui_entry_point = getenv("OPENSS_GUI_ENTRY_POINT");
    char *gui_exit_point = getenv("OPENSS_GUI_EXIT_POINT");
    // char *plugin_directory = getenv("OPENSS_PLUGIN_PATH");

    // Insure the libltdl user-defined library search path has been set
    Assert(lt_dlgetsearchpath() != NULL);

    // Load GUI library
    if( !gui_dl_name ) gui_dl_name = "libopenss-gui";
    if( !gui_entry_point ) gui_entry_point = "gui_init";
    if( !gui_exit_point ) gui_exit_point = "gui_exit";
  
    lt_dlhandle dl_gui_object = lt_dlopenext((const char *)gui_dl_name);
    if( dl_gui_object == NULL ) {
      cerr << "ERROR: can not load GUI - " << lt_dlerror() << std::endl;
      exit(EXIT_FAILURE);
    }

    lt_ptr (*dl_gui_init_routine)(void *, pthread_t *);
    dl_gui_init_routine = (lt_ptr (*)(void *, pthread_t *))lt_dlsym(dl_gui_object, gui_entry_point);
    if( dl_gui_init_routine == NULL )
    {
      cerr << "ERROR: can not initialize GUI - " << lt_dlerror() << std::endl;
      exit(EXIT_FAILURE);
    }

    dl_gui_kill_routine = (lt_ptr (*)())lt_dlsym(dl_gui_object, gui_exit_point);
    if( dl_gui_kill_routine == NULL )
    {
      cerr << "ERROR: can find GUI exit routine - " << lt_dlerror() << std::endl;
      exit(EXIT_FAILURE);
    }
  
    //    pthread_t gui_phandle;
    (*dl_gui_init_routine)((void *)argStruct, &phandle[2]);
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
    need_batch = false;
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
