/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2015 Krell Institute. All Rights Reserved.
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

// This must be first include due to warnings regarding redefinition
// of certain definitions (POSIX_*)
#include "SS_Input_Manager.hxx"
#include "ToolAPI.hxx"

//#define DEBUG_CLI_OPTIONS 1

static void Input_Command_Args (CMDWID my_window, int argc, char ** argv, bool &areWeRestoring)
{
 /* What is the maximum length of the expCreate command? */  
  bool processing_offline_option = true;
  bool processing_online_option = false;
  bool onlineArgsFound = false;
  bool initial_set_of_command_done_yet = false;
  bool processing_batch_option = false;
  bool guiArgSpecified = false;
  bool cliArgSpecified = false;
  areWeRestoring = false;

#ifdef DEBUG_CLI_OPTIONS
  printf(" StartModes in Input_Command_Args, INITIALIZE areWeRestoring=%d\n", areWeRestoring);
#endif

  int cmdlen = 0;
  int i;

  // Do some preliminary setup work
  // ------------------------- START PRELIMINARY EXAMINATION OF ARGS ------------------
  //
  for ( i=1; i<argc; i++ ) {

    if (strcasecmp( argv[i], "-gui") == 0) {
      guiArgSpecified = true;
    }

    if (strcasecmp( argv[i], "-cli") == 0) {
      cliArgSpecified = true;
    }

    if (strcasecmp( argv[i], "-p") == 0 ||
        strcasecmp( argv[i], "-t") == 0 ||
        strcasecmp( argv[i], "-h") == 0 ||
        strcasecmp( argv[i], "-r") == 0 ||
        strcasecmp( argv[i], "-c") == 0 ) {
      onlineArgsFound = true;
    }

    if (strstr( argv[i], ".openss")) {

      if (!strstr( argv[i], "-f ")) {
        areWeRestoring = true;
#ifdef DEBUG_CLI_OPTIONS
        printf(" StartModes in Input_Command_Args, WE ARE RESTORING, -f command argv[i=%d]=%s, areWeRestoring=%d\n", 
               i, argv[i], areWeRestoring);
#endif
      }
    }

    if (strlen(argv[i]) > 0) {
      cmdlen += strlen(argv[i]) + 3;  // add 3 for space and possible quote characters
    }
  }

  //
  // ------------------------- END PRELIMINARY EXAMINATION OF ARGS ------------------
  //

  if (onlineArgsFound) {
    processing_offline_option = false;
    processing_online_option = true;
  }

  if (cmdlen > 0) {
    char *cmdstr = (char *)malloc(90 + cmdlen + 1);

#ifdef DEBUG_CLI_OPTIONS
    printf(" StartModes, ALLOCATING cmdstr, 91+cmdlen=%d\n", cmdlen);
#endif

    int num_cmds = 0;
    for ( i=1; i<argc; i++ ) {

      if (strlen(argv[i]) > 0) {

#ifdef DEBUG_CLI_OPTIONS
        printf(" StartModes, cycling through the argument list, processing_online_option=%d, processing_offline_option=%d, argv[i=%d]=%s\n", 
                processing_online_option, processing_offline_option,i, argv[i]);
        printf(" StartModes, cycling through the argument list, processing_batch_option=%d, onlineArgsFound=%d\n", 
                processing_batch_option, onlineArgsFound);
#endif

        if (strcasecmp( argv[i], "-batch") == 0) {

#ifdef DEBUG_CLI_OPTIONS
            printf(" StartModes, setting processing_batch_option to TRUE, cycling through the argument list, argv[i=%d]=%s\n", i, argv[i]);
#endif
            processing_batch_option = true;
            processing_offline_option = false;
            processing_online_option = false;
        } 

        if (strcasecmp( argv[i], "-offline") == 0) {

#ifdef DEBUG_CLI_OPTIONS
            printf(" StartModes, setting processing_offline_option to TRUE, cycling through the argument list, argv[i=%d]=%s\n", i, argv[i]);
#endif

            processing_offline_option = true;
            processing_online_option = false;
            processing_batch_option = false;
            if (onlineArgsFound) {
               // Warn the user about incorrect command syntax
               std::cerr << "WARNING: Found dynamic/online arguments (-c,-h,-p,-r,or -t) present with -offline specified.  Switching to online mode." << std::endl;
               processing_offline_option = false;
               processing_online_option = true;
               processing_batch_option = false;
            }

        }

        if (strcasecmp( argv[i], "-online") == 0) {

#ifdef DEBUG_CLI_OPTIONS
            printf(" StartModes, setting processing_online_option to TRUE, cycling through the argument list, argv[i=%d]=%s\n", i, argv[i]);
#endif

            processing_online_option = true;
            processing_offline_option = false;
            processing_batch_option = false;
        }

       // Don't include any mode options.
        if (!strcasecmp( argv[i], "-cli")) continue;
        if (!strcasecmp( argv[i], "-gui")) continue;
        if (!strcasecmp( argv[i], "-batch")) continue;
        if (!strcasecmp( argv[i], "-offline")) continue;
        if (!strcasecmp( argv[i], "-online")) continue;
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
	  if ( areWeRestoring ) { 
            bcopy("expRestore", cmdstr, 11);
	  } else {
            if (guiArgSpecified || cliArgSpecified) {
              bcopy("expCreate -i offline", cmdstr, 22);
            } else {
              bcopy("RunOfflineExp", cmdstr, 15);
            }
          }
        } else {

#ifdef DEBUG_CLI_OPTIONS
          printf(" StartModes in Input_Command_Args, Making a decision to restore or create, areWeRestoring=%d\n", areWeRestoring);
#endif

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

        if ( processing_offline_option && 
	      !areWeRestoring &&
	      !guiArgSpecified &&
	      !cliArgSpecified &&
             (strlen(argv[i]) == 2) && 
              !strncasecmp( argv[i], "-f", 2) && 
              ((i+1) < argc) && 
              strncasecmp( argv[i+1], "-", 1)) {

           // if this is offline then replace the -f with program=
           strcat(cmdstr,"(program=");

#ifdef DEBUG_CLI_OPTIONS
           printf(" StartModes, offline option, replace -f option with program, cmdstr=%s\n", cmdstr);
#endif

        } else if (processing_offline_option && ((i+1) == argc)  &&
	           !guiArgSpecified &&
	           !cliArgSpecified &&
	           !areWeRestoring ) {

          // This is the last argument in the list, assumed to be the collector type
          strcat(cmdstr,",collector=");
          strcat(cmdstr," \"");
          strcat(cmdstr,argv[i]);
          strcat(cmdstr,"\"");
          strcat(cmdstr,")");

#ifdef DEBUG_CLI_OPTIONS
          printf(" StartModes, offline option, last argument is the collector?, cmdstr=%s\n", cmdstr);
#endif

        } else {

           strcat(cmdstr,argv[i]);

#ifdef DEBUG_CLI_OPTIONS
           printf(" StartModes, else clause, cmdstr=%s\n", cmdstr);
#endif

        } 

        // Still do this for offline option also, no additional checks needed
        if (!areWeRestoring &&
          (strlen(argv[i]) == 2) &&
          !strncasecmp( argv[i], "-f", 2) &&
          ((i+1) < argc) &&
          strncasecmp( argv[i+1], "-", 1)) {

         // Put quotes around the "-f" option string.
          i++;
          strcat(cmdstr," \"");
          strcat(cmdstr,argv[i]);

#ifdef DEBUG_CLI_OPTIONS
            printf(" StartModes, before quoting of the -f command argv[i]=%s cmdstr=%s\n", argv[i], cmdstr);
#endif

          strcat(cmdstr,"\"");

#ifdef DEBUG_CLI_OPTIONS
          printf(" StartModes, after quoting around -f command for non offline and for program= for offline, cmdstr=%s\n", cmdstr);
#endif

        } // end inside strlen if



      } // end strlen(argv[i]) > 0

#ifdef DEBUG_CLI_OPTIONS
      printf(" StartModes, at the bottom, for loop, processing_batch_option=%d, processing_offline_option=%d, argc=%d, cmdstr=%s\n",
              processing_batch_option, processing_offline_option, argc, cmdstr);
      printf(" StartModes, at the bottom, for loop, guiArgSpecified=%d, processing_online_option=%d, i=%d\n", 
             guiArgSpecified, processing_online_option, i);
#endif

    } // end for

#ifdef DEBUG_CLI_OPTIONS
    printf(" StartModes, checking about putting expCreate in the input stack, num_cmds=%d\n", num_cmds);
#endif
    if (num_cmds > 0) {
      strcat(cmdstr,"\n\0");

     /* Put the "expCreate" command to the input stack */
      (void)Append_Input_String (my_window, cmdstr,
                                 NULL, &Default_TLI_Line_Output, &Default_TLI_Command_Output);
#ifdef DEBUG_CLI_OPTIONS
       printf(" StartModes, put expCreate in the input stack, cmdstr=%s\n", cmdstr);
#endif
    }

   // Release allocated space.
    free(cmdstr);
  }

}

bool Start_COMMAND_LINE_Mode (CMDWID my_window, 
                              int argc, 
                              char ** argv, 
                              bool need_cli, 
                              bool need_gui, 
                              OpenSpeedShop_Start_Modes oss_start_mode)
{
  Assert (my_window);

  bool read_stdin_file = false;
  struct stat info;
  int  status = -1;

  // Protect against an null stdin
  if (stdin) status = fstat(fileno(stdin), &info);

  // If status is not good fstat failed or stdin was null, in either case this path should be ok
  if (status != 0) {
      perror("fstat() error");
      // Define the read_stdin_file flag with the information available, w/o fstat info
      read_stdin_file = (stdin && !isatty(fileno(stdin)));
  } else {
  // If status is good fstat succeeded and stdin was not null, 
  // then this path should be ok for looking at fstat output
#ifdef DEBUG_CLI_OPTIONS
      puts("fstat() returned:");
      printf("  inode:   %d\n",   (int) info.st_ino);
      printf(" dev id:   %d\n",   (int) info.st_dev);
      printf("   mode:   %08x\n",       info.st_mode);
      printf("  links:   %d\n",         info.st_nlink);
      printf("    uid:   %d\n",   (int) info.st_uid);
      printf("    gid:   %d\n",   (int) info.st_gid);
      if (S_ISREG (info.st_mode)) {
         printf ("stdin is a normal file\n"); 
      } else {
         printf ("stdin is NOT a normal file\n"); 
      } 
#endif 
      // Define the read_stdin_file flag with the information available, with fstat info since it is available
//      read_stdin_file = (stdin && !isatty(fileno(stdin)) && (info.st_gid != 0) && (info.st_uid != 0));
      read_stdin_file = (stdin && !isatty(fileno(stdin)) && S_ISREG (info.st_mode));
  }

  bool weAreRestoring = false;

#ifdef DEBUG_CLI_OPTIONS
  printf("ENTER Start_COMMAND_LINE_Mode, before calling Input_Command_Args, read_stdin_file=%d, stdin=%ld, weAreRestoring=%d, need_gui=%d, need_cli=%d\n",
         read_stdin_file, stdin, weAreRestoring, need_gui, need_cli);
#endif 

  // Translate the command line arguments into an "expCreate command".
  Input_Command_Args ( my_window, argc, argv, weAreRestoring);

#ifdef DEBUG_CLI_OPTIONS
  printf(" Start_COMMAND_LINE_Mode, after calling Input_Command_Args, weAreRestoring=%d\n", weAreRestoring);
#endif

 // After executing an expCreate command, read any piped-in file.
  if (read_stdin_file) {
   // Read a piped-in file.
    if ( !Append_Input_File (my_window, std::string("stdin"),
                           &Default_TLI_Line_Output, &Default_TLI_Command_Output) ) {
      std::cerr << "ERROR: Unable to read piped in stdin file" << std::endl;
      return false;
    }
  }

 // If there is no input file and the user specified "-offline" mode,
 // execute with an "RunOfflineExp" command and display results with "expview stats".
 // Otherwise, assume the input file will control execution.

  bool createDBonly = (getenv("OPENSS_DATABASE_ONLY") != NULL);
  bool doNotViewData = (getenv("OPENSS_RAWDATA_ONLY") != NULL);
  if (oss_start_mode == SM_Offline && !need_gui && !need_cli && !doNotViewData && !createDBonly ) {

#ifdef DEBUG_CLI_OPTIONS
    printf(" Start_COMMAND_LINE_Mode, adding expView to the input stack, oss_start_mode (offline)=%d\n", oss_start_mode);
#endif

    if ((NULL == Append_Input_String (my_window, "expview\n", NULL,
                                      &Default_TLI_Line_Output, &Default_TLI_Command_Output)) ) {
      std::cerr << "ERROR: Unable to initiate execution of commands." << std::endl;
      return false;
    }
  }

 // If there is no input file and the user specified "-batch" mode,
 // execute with an "expGo" command and display results with "expview stats".
 // Otherwise, assume the input file will control execution.
 //
  if (oss_start_mode == SM_Batch && !read_stdin_file && weAreRestoring) {

#ifdef DEBUG_CLI_OPTIONS
    printf(" Start_COMMAND_LINE_Mode, adding expGo, expView to the input stack, oss_start_mode (batch)=%d\n", oss_start_mode);
#endif
    if ((NULL == Append_Input_String (my_window, "list -v walltime\n", NULL,
                                      &Default_TLI_Line_Output, &Default_TLI_Command_Output)) ||
        (NULL == Append_Input_String (my_window, "expview\n", NULL,
                                      &Default_TLI_Line_Output, &Default_TLI_Command_Output)) ) {
      std::cerr << "ERROR: Unable to initiate execution of commands." << std::endl;
      return false;
    }

  } else if (oss_start_mode == SM_Batch && !read_stdin_file) {

#ifdef DEBUG_CLI_OPTIONS
    printf(" Start_COMMAND_LINE_Mode, adding expGo, expView to the input stack, oss_start_mode (batch)=%d\n", oss_start_mode);
#endif

    if ((NULL == Append_Input_String (my_window, "expGo\n", NULL,
                                      &Default_TLI_Line_Output, &Default_TLI_Command_Output)) ||
        (NULL == Append_Input_String (my_window, "expview\n", NULL,
                                      &Default_TLI_Line_Output, &Default_TLI_Command_Output)) ) {
      std::cerr << "ERROR: Unable to initiate execution of commands." << std::endl;
      return false;
    }
  }

 // The user specified "-online" mode with no gui.  THis is the immediate command.
 // execute with an "expGo" command and display results with "expview stats".
 // Otherwise, assume the input file will control execution.
 //
  if (oss_start_mode == SM_Online && !need_gui ) {

#ifdef DEBUG_CLI_OPTIONS
    printf(" Start_COMMAND_LINE_Mode, adding expGo, expView to the input stack, oss_start_mode (online)=%d\n", oss_start_mode);
#endif

    if ((NULL == Append_Input_String (my_window, "expGo\n", NULL,
                                      &Default_TLI_Line_Output, &Default_TLI_Command_Output)) ||
        (NULL == Append_Input_String (my_window, "expview\n", NULL,
                                      &Default_TLI_Line_Output, &Default_TLI_Command_Output)) ) {
      std::cerr << "ERROR: Unable to initiate execution of commands." << std::endl;
      return false;
    }
  }

#ifdef DEBUG_CLI_OPTIONS
    printf(" Start_COMMAND_LINE_Mode, FALL THROUGH oss_start_mode=%d, need_gui=%d, need_cli=%d, areWeRestoring=%d\n", 
            oss_start_mode, need_gui, need_cli, weAreRestoring);
#endif



  return true;
}
