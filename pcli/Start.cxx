#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <list>
#include <inttypes.h>
#include <stdexcept>
#include <string>
#include <sys/stat.h>               /* for fstat() */
#include <sys/mman.h>               /* for mmap() */
#include <signal.h>

// for host name description
     #include <sys/socket.h>
     #include <netinet/in.h>
     #include <netdb.h>

#define PTMAX 10
pthread_t phandle[PTMAX];

#include "ToolAPI.hxx"
using namespace OpenSpeedShop::Framework;

#include "support.h"
#include "Commander.hxx"
#include "Clip.hxx"
#include "Experiment.hxx"

#include "ArgClass.hxx"

static bool need_gui;
static bool need_tli;
static bool need_batch;
static bool need_command_line;
static CMDWID gui_window;
static CMDWID tli_window;
static CMDWID command_line_window;

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
  char *plugin_directory = getenv("OPENSPEEDSHOP_PLUGIN_PATH");
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

static void
catch_signal (int sig, int error_num)
{
  // fprintf(stdout,"catch_signal %d\n",sig);
  pthread_cancel (phandle[0]);
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
    setup_signal_handler (SIGTRAP);
    // setup_signal_handler (SIGABRT);
    setup_signal_handler (SIGFPE);
    setup_signal_handler (SIGKILL);
    setup_signal_handler (SIGBUS);
    setup_signal_handler (SIGSEGV);
    setup_signal_handler (SIGSYS);
    setup_signal_handler (SIGPIPE);
    setup_signal_handler (SIGCLD);

    int i;
    ArgStruct *argStruct = new ArgStruct(argc, argv);
    struct hostent *my_host = gethostent();
    pid_t my_pid = getpid();

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
  
   // Open the Python interpreter.
    Initial_Python ();

    if (need_command_line || read_stdin_file)
    {
     // Move the command line options to an input control window.
      command_line_window = Commander_Initialization ("COMMAND_LINE",my_host->h_name,my_pid,0,false);
      Start_COMMAND_LINE_Mode( command_line_window, argc-1, &argv[1], initiate_command_at-1,
                               need_batch, read_stdin_file);
    } else if (need_batch && (argc <= 2) && !read_stdin_file) {
      fprintf(stderr,"Missing command line arguments\n");
      return -1;
    }

    if (need_tli)
    {
     // Start up the Text Line Interface to read from the keyboard.
      tli_window = Commander_Initialization ("TLI",my_host->h_name,my_pid,0,true);
      int stat = pthread_create(&phandle[0], 0, (void *(*)(void *))SS_Direct_stdin_Input,(void *)tli_window);
    }

    if (need_gui)
    {
     // The gui will be started in a pthread and do it's own itinialization.
      extern void loadTheGUI(ArgStruct *);
      loadTheGUI((ArgStruct *)NULL); // argStruct);

// The following is a timing hack -
// if the TLI window hasn't been opened or didn't specify async input,
// the input routines may think they are at the end of file before
// the GUI can open and define an async input window.
// The hack is to define a dummy async window before python starts.
// We will need to sort this out at some point in the future.
      gui_window = Commander_Initialization ("GUI",my_host->h_name,my_pid,0,true);
    }

   // Fire off Python.
    PyRun_SimpleString( "myparse.do_input ()\n");

   // When Python exits, terminate SpeedShop:

   // Close Python
    Py_Finalize ();

   // Close allocated input windows.
    if (gui_window != 0)
    {
      Commander_Termination(gui_window);
    }
    if (tli_window != 0)
    {
      Commander_Termination(tli_window);
    }
    if (command_line_window != 0)
    {
      Commander_Termination(command_line_window);
    }

    return 0;
  }

  // When the cli requests to bring up a new GUI thread.  NOTE: One will 
  // be brought up for each invocation.   This routine is called to find
  // the entry point an load it.   This way the cli, when running alone,
  // doesn't have to take the hit of loading in all those nasty, ugly,
  // big GUI libraries.   We load the gui as a dynamic library. 
  // The GUI will then start it's own thread and fire up a copy of the
  // gui (that will talk with the cli).
  #include <dlfcn.h>
  void
  loadTheGUI(ArgStruct *argStruct)
  {
    char gui_plugin_file[2048];
    char *gui_dl_name = getenv("OPENSPEEDSHOP_GUI_RELOCATABLE_NAME");
    char *gui_entry_point = getenv("OPENSPEEDSHOP_GUI_ENTRY_POINT");
    char *plugin_directory = getenv("OPENSPEEDSHOP_PLUGIN_PATH");
  
    if( !plugin_directory ) exit(EXIT_FAILURE);
  
    sprintf(gui_plugin_file, "%s/%s", plugin_directory, gui_dl_name);
    void *dl_gui_object = dlopen((const char *)gui_plugin_file, (int)RTLD_LAZY );
    if( !dl_gui_object ) {
      fprintf(stderr, "%s\n", dlerror() );
      exit(EXIT_FAILURE);
    }
  
    int (*dl_gui_init_routine)(void *);
    dl_gui_init_routine = (int (*)(void *))dlsym(dl_gui_object, gui_entry_point);
    if( dl_gui_init_routine == NULL )
    {
      fprintf(stderr, "%s\n", dlerror() );
      exit(EXIT_FAILURE);
    }
  
    (*dl_gui_init_routine)((void *)argStruct);
  }
  
}
