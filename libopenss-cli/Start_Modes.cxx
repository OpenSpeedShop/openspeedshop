/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006 Krell Institute. All Rights Reserved.
** Copyright (c) 2007 Krell Institute. All Rights Reserved.
** Copyright (c) 2008 Krell Institute. All Rights Reserved.
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

#include "ToolAPI.hxx"
#include "SS_Input_Manager.hxx"

//#define DEBUG_CLI_OPTIONS 1

static void Input_Command_Args (CMDWID my_window, int argc, char ** argv, bool &areWeRestoring)
{
 /* What is the maximum length of the expCreate command? */  
  bool processing_offline_option = false;
  bool initial_set_of_command_done_yet = false;
  bool processing_batch_option = false;
  areWeRestoring = false;
  int cmdlen = 0;
  int i;

  // Do some preliminary setup work
  for ( i=1; i<argc; i++ ) {

    if (strstr( argv[i], ".openss")) {
      areWeRestoring = true;
#ifdef DEBUG_CLI_OPTIONS
      printf(" StartModes, WE ARE RESTORING, -f command argv[i]=%s, areWeRestoring=%d\n", argv[i], areWeRestoring);
#endif
    }

    if (strlen(argv[i]) > 0) {
      cmdlen += strlen(argv[i]) + 3;  // add 3 for space and possible quote characters
    }
  }

  if (cmdlen > 0) {
    char *cmdstr = (char *)malloc(30 + cmdlen + 1);
    int num_cmds = 0;
    for ( i=1; i<argc; i++ ) {
      if (strlen(argv[i]) > 0) {

#ifdef DEBUG_CLI_OPTIONS
        printf(" StartModes, cycling through the argument list, processing_batch_option=%d, processing_offline_option=%d, argv[i=%d]=%s\n", 
                processing_batch_option, processing_offline_option,i, argv[i]);
#endif
        if (strcasecmp( argv[i], "-batch") == 0) {
#ifdef DEBUG_CLI_OPTIONS
            printf(" StartModes, setting processing_batch_option to TRUE, cycling through the argument list, argv[i=%d]=%s\n", i, argv[i]);
#endif
            processing_batch_option = true;
        } 
        if (strcasecmp( argv[i], "-offline") == 0) {
#ifdef DEBUG_CLI_OPTIONS
            printf(" StartModes, setting processing_offline_option to TRUE, cycling through the argument list, argv[i=%d]=%s\n", i, argv[i]);
#endif
            processing_offline_option = true;
        }

       // Don't include any mode options.
        if (!strcasecmp( argv[i], "-cli")) continue;
        if (!strcasecmp( argv[i], "-gui")) continue;
        if (!strcasecmp( argv[i], "-batch")) continue;
        if (!strcasecmp( argv[i], "-offline")) continue;
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
      if (!initial_set_of_command_done_yet) {
        initial_set_of_command_done_yet = true;
        if (processing_offline_option) {
          bcopy("RunOfflineExp", cmdstr, 15);
        } else {
	  if ( areWeRestoring ) { 
            bcopy("expRestore", cmdstr, 11);
	  } else {
            bcopy("expCreate", cmdstr, 10);
	  }
        } 
      } 
     
      if (strlen(argv[i]) > 0) {

        num_cmds++;
        strcat(cmdstr," ");

#ifdef DEBUG_CLI_OPTIONS
        printf(" StartModes, before checks for args, processing_offline_option=%d, i=%d, argc=%d, cmdstr=%s\n", processing_offline_option, i, argc, cmdstr);
#endif

        if ( processing_offline_option && (strlen(argv[i]) == 2) && !strncasecmp( argv[i], "-f", 2) && ((i+1) < argc) && strncasecmp( argv[i+1], "-", 1)) {
           // if this is offline then replace the -f with program=
           strcat(cmdstr,"(program=");
        } else if (processing_offline_option && ((i+1) == argc) ) {
          // This is the last argument in the list, assumed to be the collector type
          strcat(cmdstr,",collector=");
          strcat(cmdstr," \"");
          strcat(cmdstr,argv[i]);
          strcat(cmdstr,"\"");
          strcat(cmdstr,")");
        } else {
           strcat(cmdstr,argv[i]);
        } 

        // Still do this for offline option also, no additional checks needed
        if ((strlen(argv[i]) == 2) &&
          !strncasecmp( argv[i], "-f", 2) &&
          ((i+1) < argc) &&
          strncasecmp( argv[i+1], "-", 1)) {

         // Put quotes around the "-f" option string.
          i++;
          strcat(cmdstr," \"");
          strcat(cmdstr,argv[i]);
#ifdef DEBUG_CLI_OPTIONS
            printf(" StartModes, -f command argv[i]=%s cmdstr=%s\n", argv[i], cmdstr);
#endif
          strcat(cmdstr,"\"");

#ifdef DEBUG_CLI_OPTIONS
          printf(" StartModes, after quoting around -f command for non offline and for program= for offline, cmdstr=%s\n", cmdstr);
#endif

        } // end inside strlen if



      } // end strlen(argv[i]) > 0

#ifdef DEBUG_CLI_OPTIONS
      printf(" StartModes, at the bottom of the for loop, processing_batch_option=%d, processing_offline_option=%d, i=%d, argc=%d, cmdstr=%s\n", processing_batch_option, processing_offline_option, i, argc, cmdstr);
#endif

    } // end for

    if (num_cmds > 0) {
      strcat(cmdstr,"\n\0");

     /* Put the "expCreate" command to the input stack */
      (void)Append_Input_String (my_window, cmdstr,
                                 NULL, &Default_TLI_Line_Output, &Default_TLI_Command_Output); }

   // Release allocated space.
    free(cmdstr);
  }

}

bool Start_COMMAND_LINE_Mode (CMDWID my_window, int argc, char ** argv, OpenSpeedShop_Start_Modes oss_start_mode)
{
  Assert (my_window);

  bool read_stdin_file = (stdin && !isatty(fileno(stdin)));
  bool weAreRestoring = false;

 // Translate the command line arguments into an "expCreate command".
  Input_Command_Args ( my_window, argc, argv, weAreRestoring);

#ifdef DEBUG_CLI_OPTIONS
      printf(" Start_COMMAND_LINE_Mode, after Input_Command_Args, weAreRestoring=%d\n", weAreRestoring);
#endif

 // After executing an expCreate command, read any piped-in file.
  if (read_stdin_file) {
   // Read a piped-in file.
    if ( !Append_Input_File (my_window, std::string("stdin"),
                           &Default_TLI_Line_Output, &Default_TLI_Command_Output) ) {
      cerr << "ERROR: Unable to read piped in stdin file" << std::endl;
      return false;
    }
  }

 // If there is no input file and the user specified "-offline" mode,
 // execute with an "RunOfflineExp" command and display results with "expview stats".
 // Otherwise, assume the input file will control execution.
  if (oss_start_mode == SM_Offline && !read_stdin_file) {
    if ((NULL == Append_Input_String (my_window, "expView\n", NULL,
                                      &Default_TLI_Line_Output, &Default_TLI_Command_Output))) {
      cerr << "ERROR: Unable to initiate execution of commands." << std::endl;
      return false;
    }
  }

 // If there is no input file and the user specified "-batch" mode,
 // execute with an "expGo" command and display results with "expview stats".
 // Otherwise, assume the input file will control execution.
 // If we are restoring then don't to an expGo as the experiment can't run but we could to
 // an expview if desired. 
  if (oss_start_mode == SM_Batch && !read_stdin_file && !weAreRestoring) {
    if ((NULL == Append_Input_String (my_window, "expGo\n", NULL,
                                      &Default_TLI_Line_Output, &Default_TLI_Command_Output)) ||
        (NULL == Append_Input_String (my_window, "expView\n", NULL,
                                      &Default_TLI_Line_Output, &Default_TLI_Command_Output))) {
      cerr << "ERROR: Unable to initiate execution of commands." << std::endl;
      return false;
    }
  }

  return true;
}
