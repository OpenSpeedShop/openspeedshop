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

#include "SS_Input_Manager.hxx"
#include <signal.h>

// Global links to some windows.
CMDWID gui_window = 0;
CMDWID tli_window = 0;
CMDWID command_line_window = 0;

#define PTMAX 10
pthread_t phandle[PTMAX];

static bool need_gui;
static bool need_tli;
static bool need_batch;
static bool need_command_line;

static int initiate_command_at;
static bool executable_encountered;
static bool collector_encountered;
static bool read_stdin_file;


static void
Process_Command_Line (int argc, char **argv)
{
  int i;

 /* Check the command line flags: */
  for ( i=1; i<argc; i++ ) {

    if (argv[i] == NULL) {
      continue;
    }

   // Look for an indication of which input control window to open.
   // Go with the first indication we encoutner.
    if (initiate_command_at == -1) {
        initiate_command_at = i;
      if (!strcasecmp( argv[i], "-cli")) {
        need_tli = true;
        continue;
      } else if (!strcasecmp( argv[i], "-gui")) {
        need_gui = true;
        need_tli = true;
        continue;
      } else if (!strcasecmp( argv[i], "-batch")) {
        need_batch = true;
        read_stdin_file = (stdin && !isatty(fileno(stdin)));
        continue;
      } else {
       // We haven't found the mode, yet.
        initiate_command_at = -1;
      }
    }

   /* Look for an executable description. */
    if (!strcasecmp( argv[i], "-h") ||
        !strcasecmp( argv[i], "-f") ||
        !strcasecmp( argv[i], "-p") ||
        !strcasecmp( argv[i], "-t") ||
        !strcasecmp( argv[i], "-r") ||
        !strcasecmp( argv[i], "-a")) {
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

  if (initiate_command_at == -1) {
   // If not specified by the user, default to -gui and -tli modes.
    need_gui = true;
    need_tli = true;
  }
}

extern "C" void initPY_Input ();
#include "Python.h"
static void
Initial_Python ()
{
  FILE *fp;
  PyObject *my_dict, *my_module, cli;
  Py_Initialize ();
  initPY_Input();
  char pyfile[1024];
  // There are multiple directories this can be locate in...
  // First look in the user defined one, then the installed,
  // and finally the package directory.
  char *plugin_directory = getenv("OPENSS_PLUGIN_PATH");
  if( plugin_directory == NULL )
  {
    plugin_directory = getenv("OPENSS_INSTALL_DIR");
  }
  if( plugin_directory == NULL )
  {
    plugin_directory = PLUGIN_PATH;
  }
  if( plugin_directory == NULL )
  {
    fprintf(stderr, "Unable to locate myparse.py to initialize python.\n");
    exit(EXIT_FAILURE);
  }
  sprintf(pyfile, "%s/%s", plugin_directory, "myparse.py");
  fp = fopen (pyfile, "r");
  PyRun_SimpleFile ( fp, "myparse.py" );
  fclose (fp);
}

int Start_TLI_Mode (CMDWID my_window);
int Start_COMMAND_LINE_Mode (CMDWID my_window, int argc, char ** argv, int butnotarg,
                             bool batch_mode,  bool read_stdin_file);
void SS_Direct_stdin_Input (void *attachtowindow);

extern "C"
{
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
    printf("  To start Open/Speedshop simply type:\n");
    printf("  $ openss\n");
    printf("\n\n");
    printf("  To load an executable into Open/Speedshop\n");
    printf("  for eventual capturing of data:\n");
    printf("  $ openss -f a.out\n");
    printf("\n\n");
    printf("  To attach to a running executable:\n");
    printf("  $ openss -p 123456\n");
    printf("\n\n");
    printf("  To load an executable to run a simple timing (pcSampling) experiment:\n");
    printf("  $ openss -f a.out -x pcsamp\n");
  }

  void
  cli_terminate ()
  {
   // Close Python
    Py_Finalize ();

   // Close any open experiments
    Experiment_Termination ();

   // Close allocated input windows.
    if (gui_window != 0)
    {
      Commander_Termination(gui_window);
      gui_window = 0;
    }
    if (tli_window != 0)
    {
     // Stop async read loop for xterm window
      pthread_cancel (phandle[0]);

      Commander_Termination(tli_window);
      tli_window = 0;
    }
    if (command_line_window != 0)
    {
      Commander_Termination(command_line_window);
      command_line_window = 0;
    }
  }

static void
catch_signal (int sig, int error_num)
{
  static bool processing_signal = false;
  if (processing_signal) {
    fprintf(stderr,"Multiple errors - %d %d\n",sig, error_num);
    abort();
  }
  processing_signal = true;
fprintf(stderr,"catch_signal %d\n",sig);
  cli_terminate ();
  exit (1);
}
inline static void
setup_signal_handler (int s)
{
    if (signal (s, SIG_IGN) != SIG_IGN)
        signal (s,  reinterpret_cast <void (*)(int)> (catch_signal));
}

  int
  cli_init(int argc, char **argv)
  {
   // Set up to catch bad errors
    setup_signal_handler (SIGILL);
    // setup_signal_handler (SIGTRAP);
    // setup_signal_handler (SIGABRT);
    setup_signal_handler (SIGFPE);
    // setup_signal_handler (SIGKILL);
    setup_signal_handler (SIGBUS);
    setup_signal_handler (SIGSEGV);
    setup_signal_handler (SIGSYS);
    // setup_signal_handler (SIGPIPE);
    // setup_signal_handler (SIGCLD);

    int i;
    ArgStruct *argStruct = new ArgStruct(argc, argv);
    pid_t my_pid = getpid();
    char HostName[MAXHOSTNAMELEN+1];
    if (gethostname ( &HostName[0], MAXHOSTNAMELEN)) {
      fprintf(stderr,"ERROR: can not retreive host name\n");
      abort ();
    }

    need_gui = false;
    need_tli = false;
    need_batch = false;
    need_command_line = false;
    gui_window = 0;
    tli_window = 0;
    command_line_window = 0;

    read_stdin_file = (stdin && !isatty(fileno(stdin)));
    initiate_command_at = -1;
    executable_encountered = false;
    collector_encountered = false;
    Process_Command_Line (argc, argv);

   // Define Built-In Views
    SS_Init_BuiltIn_Views ();

   // Open the Python interpreter.
    Initial_Python ();

    if (need_command_line || read_stdin_file)
    {
     // Move the command line options to an input control window.
      command_line_window = Default_Window ("COMMAND_LINE",&HostName[0],my_pid,0,false);
      Start_COMMAND_LINE_Mode( command_line_window, argc-1, &argv[1], initiate_command_at-1,
                               need_batch, read_stdin_file);
    } else if (need_batch && (argc <= 2) && !read_stdin_file) {
      fprintf(stderr,"Missing command line arguments\n");
      return -1;
    }

    if (need_tli)
    {
     // Start up the Text Line Interface to read from the keyboard.
      tli_window = TLI_Window ("TLI",&HostName[0],my_pid,0,true);
      int stat = pthread_create(&phandle[0], 0, (void *(*)(void *))SS_Direct_stdin_Input,(void *)tli_window);
    }

    if (need_gui)
    {

// The following is a timing hack -
// if the TLI window hasn't been opened or didn't specify async input,
// the input routines may think they are at the end of file before
// the GUI can open and define an async input window.
// The hack is to define a dummy async window before python starts.
// We will need to sort this out at some point in the future.
      gui_window = GUI_Window ("GUI",&HostName[0],my_pid,0,true);
      argStruct->addArg("-wid");
      char buffer[10];
      sprintf(buffer, "%d", gui_window);
      argStruct->addArg(buffer);
     // The gui will be started in a pthread and do it's own initialization.
      extern void loadTheGUI(ArgStruct *);
//      loadTheGUI((ArgStruct *)NULL); // argStruct);
      loadTheGUI((ArgStruct *)argStruct); // NULL);
    }

   // Fire off Python.
    PyRun_SimpleString( "myparse.do_input ()\n");

   // When Python exits, terminate SpeedShop:
    cli_terminate ();

    return 0;
  }

  // When the cli requests to bring up a new GUI thread.  NOTE: One will 
  // be brought up for each invocation.   This routine is called to find
  // the entry point an load it.   This way the cli, when running alone,
  // doesn't have to take the hit of loading in all those nasty, ugly,
  // big GUI libraries.   We load the gui as a dynamic library. 
  // The GUI will then start it's own thread and fire up a copy of the
  // gui (that will talk with the cli).
//  #include <dlfcn.h>
  #include <ltdl.h>
  #include <assert.h>
  void
  loadTheGUI(ArgStruct *argStruct)
  {
    char gui_plugin_file[2048];
    char *gui_dl_name = getenv("OPENSS_GUI_RELOCATABLE_NAME");
    char *gui_entry_point = getenv("OPENSS_GUI_ENTRY_POINT");
    char *plugin_directory = getenv("OPENSS_PLUGIN_PATH");

    assert(lt_dlinit() == 0);
    // Start with an empty user-defined search path
    assert(lt_dlsetsearchpath("") == 0);
    // Add the user-specified plugin path
    if(getenv("OPENSS_PLUGIN_PATH") != NULL)
    {
      char *user_specified_path = getenv("OPENSS_PLUGIN_PATH");
      const char *currrent_search_path = lt_dlgetsearchpath();
      assert(lt_dladdsearchdir(user_specified_path) == 0);
    }
    // Add the install plugin path
    char *openss_install_dir = getenv("OPENSS_INSTALL_DIR");
    if( openss_install_dir != NULL)
    {
      char *install_path = (char *)calloc(strlen(openss_install_dir)+
                                          strlen("/lib/openspeedshop")+1,
                                          sizeof(char *) );
      strcpy(install_path, openss_install_dir);
      strcat(install_path, "/lib/openspeedshop");
      const char *currrent_search_path = lt_dlgetsearchpath();
      assert(lt_dladdsearchdir(install_path) == 0);
    }
    // Add the compile-time plugin path
    assert(lt_dladdsearchdir(PLUGIN_PATH) == 0);
    // Now search for collector plugins in all these paths
//    printf("lt_dlgetsearchpath()=(%s)\n", lt_dlgetsearchpath() );

    if( !gui_dl_name ) gui_dl_name = "libopenss-GUI";
    if( !gui_entry_point ) gui_entry_point = "gui_init";
  
    lt_dlhandle dl_gui_object = lt_dlopenext((const char *)gui_dl_name);
    if( dl_gui_object == NULL ) {
      fprintf(stderr, "%s\n", lt_dlerror() );
      exit(EXIT_FAILURE);
    }
  
    lt_ptr (*dl_gui_init_routine)(void *);
    dl_gui_init_routine = (lt_ptr (*)(void *))lt_dlsym(dl_gui_object, gui_entry_point);
    if( dl_gui_init_routine == NULL )
    {
      fprintf(stderr, "%s\n", lt_dlerror() );
      exit(EXIT_FAILURE);
    }
  
    (*dl_gui_init_routine)((void *)argStruct);
  }

}
