#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdexcept>
#include <string>
#include <sys/stat.h>               /* for fstat() */

#define PTMAX 10
pthread_t phandle[PTMAX];

#include "ArgClass.hxx"

enum INITIATE_MODE {
  INITIATE_MODE_NULL,
  INITIATE_MODE_BATCH,
  INITIATE_MODE_GUI,
  INITIATE_MODE_TLI
};
static INITIATE_MODE command_mode;
static int initiate_command_at;
static bool executable_encountered;
static bool collector_encountered;


static void
Process_Command_Line (int argc, char **argv)
{
  int i;

 /* Check the command line flags: */
  for ( i=0; i<argc; i++ ) {
    if ( argv[i] != NULL && *(argv[i]) == '-' ) {
      char *cp = argv[i]+1;         /* Pointer to next flag character */

     /* Look for an indication of which input control window to open. */
      if (command_mode == INITIATE_MODE_NULL) {
        initiate_command_at = i;
        if (!strncasecmp( cp, "cli", 3)) {
          command_mode = INITIATE_MODE_TLI;
          continue;
        } else if (!strncasecmp( cp, "gui", 3)) {
          command_mode = INITIATE_MODE_GUI;
          continue;
        } else if (!strncasecmp( cp, "batch", 5)) {
          command_mode = INITIATE_MODE_BATCH;
          continue;
        } else {
         /* We haven't found the mode, yet. */
          initiate_command_at = -1;
        }
      }

     /* Look for an executable description. */
      switch ( *cp++ ) {

      case 'h':               /* host options */
      case 'f':               /* file options */
      case 'p':               /* pid  options */
      case 't':               /* thread options */
      case 'r':               /* rank options */
        executable_encountered = true;
        break;

      default:
       /* This is probably an error. */
        break;

      }
      if (((i+1)<argc) &&
          (argv[i+1] != NULL) &&
          (*(argv[i+1]) != '-' )) i++;
      continue;

    } else {
      collector_encountered = true;
      continue;
    }
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

#include "Commander.hxx"
int Start_BATCH_Mode (CMDWID my_window, int argc, char ** argv, int butnotarg, bool read_stdin_file);
int Start_GUI_Mode (CMDWID my_window, int argc, char ** argv, int butnotarg, bool read_stdin_file);
int Start_TLI_Mode (CMDWID my_window, int argc, char ** argv, int butnotarg, bool read_stdin_file);

static void
Load_Initial_Modules (int argc, char **argv)
{
  int charcnt = 0;
  int i;
  bool read_stdin_file = (stdin && !isatty(fileno(stdin)));

  if (initiate_command_at == -1) {
    if (read_stdin_file ||
        (executable_encountered && collector_encountered)) {
      command_mode = INITIATE_MODE_BATCH;
    } else {
      command_mode = INITIATE_MODE_TLI;
    }
  }

  for ( i=0; i<argc; i++ ) {
    if (initiate_command_at == i) {
      continue;
    }
    charcnt += strlen(argv[i]) + 1;
  }

 /* Open the Python interpreter */
printf("call Initial_Python()\n");
  Initial_Python ();

  switch (command_mode) {

  case INITIATE_MODE_BATCH:
printf("BATCH()\n");
   /* Start up batch mode operation. */
    Start_BATCH_Mode( 0, argc, argv, initiate_command_at, read_stdin_file);
    break;
  case INITIATE_MODE_GUI:
printf("GUI()\n");
   /* Load the GUI dso and let it initialize SpeedShop. */
    Start_GUI_Mode( 0, argc, argv, initiate_command_at, read_stdin_file);
    break;
  case INITIATE_MODE_TLI:
printf("TLI()\n");
   /* Start up the text line interface. */
    Start_TLI_Mode( 0, argc, argv, initiate_command_at, read_stdin_file);
    break;
  default:
    fprintf(stderr,"ERROR: A supported interactive command mode was not specified.\n");
    abort();
  }

/* >>>>> Start DEBUG <<<<<
  switch (command_mode) {
  case INITIATE_MODE_BATCH: fprintf(stdout,"openss -batch"); break;
  case INITIATE_MODE_GUI:   fprintf(stdout,"openss -gui"); break;
  case INITIATE_MODE_TLI:   fprintf(stdout,"openss -cli"); break;
  }
  for ( i=0; i<argc; i++ ) {
    if ((command_mode != INITIATE_MODE_NULL) &&
        (initiate_command_at == i)) {
      continue;
    }
    fprintf(stdout," %s",argv[i]);
  }
  fprintf(stdout,"\n");
>>>>> Stop  DEBUG <<<<< */

 /* Close Python */
  Py_Finalize ();
}

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

  int
  cli_init(int argc, char **argv)
  {
    bool splashFLAG=true;
    printf("A: pcli.so cli_init() entered\n");
    printf("argc=%d\n", argc);
    ArgStruct *argStruct = new ArgStruct(argc, argv);
    bool cliOnlyFLAG = false;
    for(int i=0;i<argc;i++)
    {
      printf("argv[%d]=%s\n", i, argv[i] );
      if(strcmp(argv[i], "-cli") == 0 )
      {
        printf("cliOnlyFLAG == true!\n");
        cliOnlyFLAG = true;
      }
    }

    if( cliOnlyFLAG == false )
    {
      printf("A:Then start the gui as well..\n");
      extern void loadTheGUI(ArgStruct *);
      loadTheGUI(argStruct);
      printf("called loadTheGUI()\n");
// I sleep for now, only to get the prompt to come out after the GUI is up.
// We won't do this in the real product....
sleep(7);
    }


    extern void loadTheCLI(ArgStruct *);
    loadTheCLI(argStruct);

    // I just spin here.   This really should be looping looking for some
    // global variable that tells the cli/gui to go away...
    while(1)
    {
      sleep(100);
    }

  }

  // This is the initialization routine that (in it's own thread) initializes
  // the cli/python pair.
  void
  clithreadinit(void *ptr)
  {
    ArgStruct *arg_struct = (ArgStruct *)ptr;
    int argc = arg_struct->argc;
    char **argv = arg_struct->argv;
  
    command_mode = INITIATE_MODE_NULL;
    initiate_command_at = -1;
    executable_encountered = false;
    collector_encountered = false;
    Process_Command_Line (argc-1, &argv[1]);
    Load_Initial_Modules (argc-1, &argv[1]);
  }

  // This routine is called from cli_init to start the cli (The parsing
  // stuff, in it's own thread.
  void
  loadTheCLI(ArgStruct *argStruct)
  {
    int stat = pthread_create(&phandle[0], 0, (void *(*)(void *))clithreadinit,argStruct);
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
