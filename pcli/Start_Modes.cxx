#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/stat.h>               /* for fstat() */
#include <sys/mman.h>               /* for mmap() */
#include <time.h>
#include <stdio.h>
#include <list>
#include <inttypes.h>
#include <stdexcept>
#include <string>

// for host name description
     #include <sys/socket.h>
     #include <netinet/in.h>
     #include <netdb.h>

#ifndef PTHREAD_MUTEX_RECURSIVE_NP
#define PTHREAD_MUTEX_RECURSIVE_NP 0
#endif
#include "Lockable.hxx"
#include "Guard.hxx"
#include "Application.hxx"
#include "Collector.hxx"
#include "Metric.hxx"
#include "Parameter.hxx"
#include "StringTable.hxx"

using namespace OpenSpeedShop::Framework;

#include "Commander.hxx"
#include "Python.h"

static void Input_Command_Args (CMDWID my_window, int argc, char ** argv, int butnotarg)
{
 /* How long is the expCreate command? */  
  int cmdlen = 0;
  int i;
  for ( i=0; i<argc; i++ ) {
    if (butnotarg == i) {
      continue;
    }
    if (strlen(argv[i]) > 0) {
      cmdlen += strlen(argv[i]) + 1;
    }
  }

  if (cmdlen > 0) {
    char *cmdstr = (char *)malloc(10 + cmdlen + 1);
    bcopy("expCreate", cmdstr, 10);
    cmdlen = strlen(cmdstr)-1;
    for ( i=0; i<argc; i++ ) {
      if (butnotarg == i) {
        continue;
      }
      if (strlen(argv[i]) > 0) {
        strcat(cmdstr," ");
        strcat(cmdstr,argv[i]);
      }
    }
    strcat(cmdstr,"\n\0");

   /* Note: Push commands on the stack in reverse order of how they execute. */

   /* FIrst, stack an "expRun" command. */
    char *cmdrun = "expRun\n";
    (void)Attach_Input_Buffer (my_window, strlen(cmdrun), cmdrun);

   /* Second, push the "expCreate" command to the input stack */
    (void)Attach_Input_Buffer (my_window, strlen(cmdstr), cmdstr);
  }

}

int Start_BATCH_Mode (CMDWID my_window, int argc, char ** argv, int butnotarg, bool read_stdin_file)
{
  if (my_window <= 0) {
    struct hostent *my_host = gethostent();
    pid_t my_pid = getpid();
    my_window = Commander_Initialization ("BATCH_MODE",my_host->h_name,my_pid,0);
    if (my_window <= 0) {
      return -1;  /* We tried and failed to define a window. */
    }
  }

 /*
    Read from stdin if one was piped into the command, otherwise
    assume that only the command line needs to be precessed.
 */
  if (read_stdin_file) {
    (void)Attach_Input_File (my_window, std::string("stdin"));
  }

 /* Set up buffers to read command line arguments. */
  Input_Command_Args ( my_window, argc, argv, butnotarg);

 /* Fire off Python. */
  PyRun_SimpleString( "myparse.do_input ()\n");

/* >>>>> Start DEBUG <<<<<
  fprintf(stdout,"\nList of defined Window IDs:\n");
  List_CommandWindows (stdout);
  fprintf(stdout,"\n");
>>>>> Stop  DEBUG <<<<< */

  Commander_Termination(my_window);
  return 1;
}

int Start_GUI_Mode (CMDWID my_window, int argc, char ** argv, int butnotarg, bool read_stdin_file)
{
  if (my_window <= 0) {
    struct hostent *my_host = gethostent();
    pid_t my_pid = getpid();
    my_window = Commander_Initialization ("BATCH_MODE",my_host->h_name,my_pid,0);
    if (my_window <= 0) {
      return -1;  /* We tried and failed to define a window. */
    }
  }

 /* NOW????  WHAT DO WE DO TO GET THE GUI UP??? */

 /* Define stdin as the main input file. */
 // (void)Attach_Input_File (my_window, std::string("stdin"));

 /* Set up buffers to read command line arguments. */
 // Input_Command_Args ( my_window, argc, argv, butnotarg);

 /* Fire off Python. */
 // PyRun_SimpleString( "myparse.do_input ()\n");

  Commander_Termination(my_window);
  return 1;
}

int Start_TLI_Mode (CMDWID my_window, int argc, char ** argv, int butnotarg, bool read_stdin_file)
{
  if (my_window <= 0) {
    struct hostent *my_host = gethostent();
    pid_t my_pid = getpid();
    my_window = Commander_Initialization ("TLI_MODE",my_host->h_name,my_pid,0);
    if (my_window <= 0) {
      return -1;  /* We tried and failed to define a window. */
    }
  }

 /* Define stdin as the main input file. */
  (void)Attach_Input_File (my_window, std::string("stdin"));

 /* Set up buffers to read command line arguments. */
  Input_Command_Args ( my_window, argc, argv, butnotarg);

 /* Fire off Python. */
  PyRun_SimpleString( "myparse.do_input ()\n");

/* >>>>> Start DEBUG <<<<<
  fprintf(stdout,"\nList of defined Window IDs:\n");
  List_CommandWindows (stdout);
  fprintf(stdout,"\n");
>>>>> Stop  DEBUG <<<<< */

  Commander_Termination(my_window);
  return 1;
}
