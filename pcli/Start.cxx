#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdexcept>
#include <string>
#include <sys/stat.h>               /* for fstat() */

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

extern "C" void initSS_Util ();
#include "Python.h"
static void
Initial_Python ()
{
  FILE *fp;
  PyObject *my_dict, *my_module, cli;
  Py_Initialize ();
  initSS_Util();
  fp = fopen ("myparse.py", "r");
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
  Initial_Python ();

  switch (command_mode) {

  case INITIATE_MODE_BATCH:
   /* Start up batch mode operation. */
    Start_BATCH_Mode( 0, argc, argv, initiate_command_at, read_stdin_file);
    break;
  case INITIATE_MODE_GUI:
   /* Load the GUI dso and let it initialize SpeedShop. */
    Start_GUI_Mode( 0, argc, argv, initiate_command_at, read_stdin_file);
    break;
  case INITIATE_MODE_TLI:
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

int
main (int argc, char **argv)
{
  command_mode = INITIATE_MODE_NULL;
  initiate_command_at = -1;
  executable_encountered = false;
  collector_encountered = false;
  Process_Command_Line (argc-1, &argv[1]);
  Load_Initial_Modules (argc-1, &argv[1]);
}
