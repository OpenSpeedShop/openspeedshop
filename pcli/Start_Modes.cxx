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

static void Input_Command_Args (CMDWID my_window, int argc, char ** argv)
{
 /* What is the maximum length of the expCreate command? */  
  int cmdlen = 0;
  int i;
  for ( i=1; i<argc; i++ ) {
    if (strlen(argv[i]) > 0) {
      cmdlen += strlen(argv[i]) + 3;  // add 3 for space and possible quote characters
    }
  }

  if (cmdlen > 0) {
    char *cmdstr = (char *)malloc(10 + cmdlen + 1);
    bcopy("expCreate", cmdstr, 10);
    int num_cmds = 0;
    for ( i=1; i<argc; i++ ) {
      if (strlen(argv[i]) > 0) {
       // Don't include any mode options.
        if (!strcasecmp( argv[i], "-cli")) continue;
        if (!strcasecmp( argv[i], "-gui")) continue;
        if (!strcasecmp( argv[i], "-batch")) continue;
      }
      if ((strlen(argv[i]) > 0) &&
        !strncasecmp( argv[i], "--", 2)) {
       // This argument represents an Xwindow control option.
        if ((strlen(argv[i+1]) > 0) &&
            strncasecmp( argv[i+1], "-", 1)) {
          i++;
          continue;
        }
      }
      if (strlen(argv[i]) > 0) {
        num_cmds++;
        strcat(cmdstr," ");
        strcat(cmdstr,argv[i]);

        if ((strlen(argv[i]) == 2) &&
          !strncasecmp( argv[i], "-f", 2) &&
          ((i+1) < argc) &&
          strncasecmp( argv[i+1], "-", 1)) {
         // Put quotes around the "-f" option string.
          i++;
          strcat(cmdstr," \"");
          strcat(cmdstr,argv[i]);
          strcat(cmdstr,"\"");
        }
      }
    }

    if (num_cmds > 0) {
      strcat(cmdstr,"\n\0");

     /* Put the "expCreate" command to the input stack */
      (void)Append_Input_String (my_window, cmdstr,
                                 NULL, &Default_TLI_Line_Output, &Default_TLI_Command_Output);
    }

   // Release allocated space.
    free(cmdstr);
  }

}

bool Start_COMMAND_LINE_Mode (CMDWID my_window, int argc, char ** argv, bool batch_mode)
{
  Assert (my_window);

  bool read_stdin_file = (stdin && !isatty(fileno(stdin)));

 // Translate the command line arguments into an "expCreate command".
  Input_Command_Args ( my_window, argc, argv);

 // After executing an expCreate command, read any piped-in file.
  if (read_stdin_file) {
   // Read a piped-in file.
    if ( !Append_Input_File (my_window, std::string("stdin"),
                           &Default_TLI_Line_Output, &Default_TLI_Command_Output) ) {
      fprintf(stderr,"ERROR: Unable to read piped in stdin file\n");
      return false;
    }
  }

 // If there is no input file and in "-batch" mode, end with an "expGo" command.
 // Otherwise, assume the input file will control execution.
  if (batch_mode && !read_stdin_file) {
    if (NULL == Append_Input_String (my_window, "expGo\n", NULL,
                                     &Default_TLI_Line_Output, &Default_TLI_Command_Output) ) {
      fprintf(stderr,"ERROR: Unable to initiate execution of commands.\n");
      return false;
    }
  }

  return true;
}
