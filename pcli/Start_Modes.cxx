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

#include "ToolAPI.hxx"
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

   /* Put the "expCreate" command to the input stack */
    (void)Append_Input_String (my_window, cmdstr);
  }

}

int Start_TLI_Mode (CMDWID my_window)
{
  Assert (my_window);

 /* Define stdin as the main input file. */
  (void)Append_Input_File (my_window, std::string("stdin"));

  return 1;
}

int Start_COMMAND_LINE_Mode (CMDWID my_window, int argc, char ** argv, int butnotarg,
                             bool batch_mode, bool read_stdin_file)
{
  Assert (my_window);

 // Translate the command line arguments into an "expCreate command".
  Input_Command_Args ( my_window, argc, argv, butnotarg);

  if (read_stdin_file) {
   // Read a piped-in file.
    (void)Append_Input_File (my_window, std::string("stdin"));
  }

  if (batch_mode) {
   // If in "-batch" mode, end with an "expGo" command.
    char *cmdrun = "expGo\n";
    (void)Append_Input_String (my_window, cmdrun);
  }

  return 1;
}
