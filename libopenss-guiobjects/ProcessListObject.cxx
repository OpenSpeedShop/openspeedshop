////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2012 Krell Institute All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////


#include <cstddef>
#include "ProcessListObject.hxx"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.hxx"

class ProcessEntry;
#include "ProcessEntryClass.hxx"

#include <sys/types.h>
//
// Note: This dummy comment is used to eliminate a false-positive found by
// the SGI source scanner when comparing against AT&T Unix SVR4.1ES source.
//

#include <sys/wait.h>
#include <unistd.h>

#include <qvaluelist.h>

#include "openspeedshop.hxx"
extern OpenSpeedshop *w;

ProcessListObject::ProcessListObject(char *host)
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("SourceObject::ProcesslistObject(host=%s)\n", host);

  const char *command = w->preferencesDialog->globalRemoteShell.ascii();
// printf("command=%s\n", command );

  createProcList(command, host);
}

ProcessListObject::~ProcessListObject()
{
  nprintf( DEBUG_CONST_DESTRUCT ) ("delete all the processEntries!\n");

  ProcessEntry *pe = NULL;
  ProcessEntryList::Iterator it;
  for( it = processEntryList.begin();
       it != processEntryList.end();
       ++it )
  {
    pe = (ProcessEntry *)(*it);
    delete pe;
  }

  processEntryList.clear();
}

#define parent_field_name   "PPID"
#define pid_field_name      "PID"
#define command_field_name  "CMD"
#define STATUS_FIELD_NAME   "STAT"
static int parent_field_name_column = 0;
static int pid_field_name_column = 0;
static int command_field_name_column = 0;
static int STATUS_FIELD_NAME_column = 0;

static int parent_field_start = 0;
static int parent_field_length = 0;
static int pid_field_start = 0;
static int pid_field_length = 0;
static int command_field_start = 0;


bool
ProcessListObject::analyze_ps_header ( char *header, char *first_line )
{
  nprintf( DEBUG_PANELS ) ("header=(%s)\n", header);

  int header_column_count = 0;  // 0 based
  char *header_tracker = header;
  char *line_tracker = first_line;
    char prev_field_name [256];
  int current_field_start;
  char one_tag [256];
  bool found_hdr = FALSE;

  parent_field_start = 0;
  parent_field_length = 0;
  pid_field_start = 0;
  pid_field_length = 0;
  command_field_start = 0;

  parent_field_name_column = 0;
  pid_field_name_column = 0;
  command_field_name_column = 0;
  STATUS_FIELD_NAME_column = 0;

  prev_field_name [0] = 0;

  // Skip any leading blanks to get to the first column header.
  if( *header_tracker && *header_tracker == ' ' )
  {
    while ( *header_tracker && *header_tracker != ' ')
    {
      header_tracker++;
    }
  }
  while ( *header_tracker )
  {
    current_field_start = header_tracker - header;

    if ( *header_tracker )
    {
      found_hdr = TRUE;

      sscanf (header_tracker, "%s", one_tag);

      if (strcmp(one_tag, parent_field_name) == 0 )
      {
        parent_field_name_column = header_column_count;
      } else if(strcmp(one_tag, pid_field_name) == 0)
      {
        pid_field_name_column = header_column_count;
      } else if (strcmp (one_tag, command_field_name) == 0 )
      {
        command_field_name_column = header_column_count;
      }
    }
    while ( *header_tracker && *header_tracker != ' ')	// scan to blank
    {
      header_tracker++;
    }

    while ( *header_tracker && *header_tracker == ' ')
    {
      header_tracker++;
    }
    header_column_count++;
  }

nprintf( DEBUG_PANELS ) ("pid_field_name_column = %d command_field_name_column = %d\n", pid_field_name_column, command_field_name_column );

  // You've looked up the header, now set up the started of the actual fields.

  // Skip any leading blanks.
  if( *line_tracker && *line_tracker == ' ' )
  {
    while ( *line_tracker && *line_tracker != ' ')
    {
      line_tracker++;
    }
  }

  int line_column_count = 0;
  while ( *line_tracker )
  {
    current_field_start = line_tracker - first_line;
    if( parent_field_name_column == line_column_count )
    {
      parent_field_start = current_field_start;
      char *ptr = first_line+current_field_start;
      while ( *ptr && *ptr != ' ')
      {
        ptr++;
      }
      parent_field_length = ptr - (first_line-parent_field_start);
    } else if( pid_field_name_column == line_column_count )
    {
      pid_field_start = current_field_start;
pid_field_start--;
      char *ptr = first_line+current_field_start;
      while ( *ptr && *ptr != ' ')
      {
        ptr++;
      }
      pid_field_length = ptr - (first_line-pid_field_start);
// printf("pid_field_start=%d pid_field_length=%d\n", pid_field_start, pid_field_length);
    } else if( command_field_name_column == line_column_count )
    {
      command_field_start = current_field_start-1;
    }
    while ( *line_tracker && *line_tracker != ' ')	// scan to blank
    {
      line_tracker++;
    }
    while ( *line_tracker && *line_tracker == ' ')
    {
      line_tracker++;
    }
    line_column_count++;
  }

  return (found_hdr);
}


//	---------------------------------------------------------------------------
// Issue a ps command thru vfork/exec.  Return the input file descriptor
// to the parent.
//	---------------------------------------------------------------------------

int 
ProcessListObject::do_ps_cmd ( char *aout_name, char *arg0, int *child_pid)
{
  int input[2];

  (void) pipe(input);

  int pid = fork();
  if (pid < 0)
  {
    perror("Can't fork ps command");
    return (-1);
  }

  if (pid == 0)     // child
  {
    (void) close(1);
    (void) dup2(input[1], 1);
    (void) close(input[1]);
    (void) execl(aout_name, aout_name, arg0, 0);
    perror("Can't exec ps command");
    _exit(0xf0);			// exec failed
  }

  // Don't need pipe output fd
  close (input[1]);

  return (input[0]);
}

// Same as above, just more arguments.
int 
ProcessListObject::do_ps_cmd ( char *aout_name, char *arg0, char *arg1, char *arg2, int *child_pid)
{
  int input[2];

  (void) pipe(input);

  int pid = fork();
  if (pid < 0)
  {
    perror("Can't fork ps command");
    return (-1);
  }

  if (pid == 0)     // child
  {
    (void) close(1);
    (void) dup2(input[1], 1);
    (void) close(input[1]);
    (void) execl(aout_name, aout_name, arg0, arg1, arg2, 0);
    perror("Can't exec ps command");
    _exit(0xf0);			// exec failed
  }

  // Don't need pipe output fd
  close (input[1]);

  return (input[0]);
}


// ---------------------------------------------------------------------------
// Return a substring of a string.  The return string is a temporary.
// ---------------------------------------------------------------------------
char *substr (
    char *old_string,
    int start,
    int length)
{
    if (!old_string)
        return (0);
    char *new_string = (char *)calloc(length, sizeof(char));
    return (strncpy(new_string, old_string + start, length));
}

int
ProcessListObject::extract_ps_list ( char *host, FILE *input, int *count, int /* new_pid */)
{
  char header [256];
  char line [256];
  int pid;
  int parent_pid;
  char status [255];
  char command_string [1024];
  char *base_command_string = NULL;

  // fetch the header line
  if ( !fgets (header, 255, input) )
  {
// printf("header=%s", header);
    return (0);
  }

  // Set up the field start/end pairs.
  if( fgets (line, 255, input) )
  {
// printf("line=%s", line);
    (void)analyze_ps_header (header, line);			
  }

  // Now, starting with the first line (previously used to find 
  // field start/end pairs.   Process the ps list.
  do
  {
// fprintf(stderr, "line=(%s) pid_field_start=%d pid_field_length=%d\n", line, pid_field_start, pid_field_length );

    // Throw out any known zombies.
// fprintf(stderr, "line=(%s)\n", line );
    if( strstr(line, "<defunct>") != NULL )
    {
// printf("Ignore this zombie!!\n");
      continue;
    }

    char *sub_str = substr( line, pid_field_start, pid_field_length );
    pid = (int) atol ( sub_str );
    free(sub_str);
    sub_str = substr ( line, parent_field_start, parent_field_length );
    parent_pid = (int) atol ( sub_str );
    free(sub_str);
    sscanf ( line + command_field_start, "%s", command_string);


    char *ptr = strchr(command_string, ' ');
    if( ptr != NULL )
    {
      *ptr = '\0';
    }       

    char *start_ptr = strrchr(command_string, '/');
    if( start_ptr == NULL )
    {
      base_command_string = strdup(command_string);
    } else
    {
      start_ptr++;
      base_command_string = strdup(start_ptr);
    }
    if( command_string[0]  == '-' ||
        strcmp (base_command_string, "sh") == 0 ||
        strcmp (base_command_string, "csh") == 0 ||
        strcmp (base_command_string, "tcsh") == 0 ||
        strcmp (base_command_string, "ksh") == 0 ||
        strcmp (base_command_string, "ps") == 0 ||
        strcmp (base_command_string, "rshd") == 0 ||
        strcmp (base_command_string, "sshd") == 0 ||
        strcmp (base_command_string, "rsh") == 0 ||
        strcmp (base_command_string, "ssh") == 0 ||
        strcmp(base_command_string, "openspeedshop") == 0 ||
        strcmp(base_command_string, "openss") == 0 ||
        strcmp(base_command_string, "dpcld") == 0 ||
        strcmp(base_command_string, "vi") == 0 ||
        strcmp(base_command_string, "emacs") == 0 ||
        strcmp(base_command_string, "xterm") == 0 )
    {
      continue;
    }

    if( ptr != NULL )
    {
      *ptr = ' ';
    }
  
    if (getpid() == pid)			// skip ourself
    {
      continue;
    }

    if (count)
    {
      (*count) ++;
    }

//    printf("  host=%s pid=%d parent_pid=%d base_command_string=(%s) command_string=(%s)\n", host, pid, parent_pid, base_command_string, command_string);

    ProcessEntry *pe = new ProcessEntry(host, pid, base_command_string);
    processEntryList.push_back(pe);

    free(base_command_string);
  } while ( fgets (line, 255, input) );


  return(0);
}

void
ProcessListObject::createProcList(const char *command, char *host)
{
  int new_pid = -1;
  int child_pid;

  char arg0[255];
  char **ps_args_buf = NULL;
  int _proc_count = 0;
  char *login = getlogin();
  
  if( login == NULL )
  {
    login = getenv("LOGNAME");
  }

  char *ps_command = (char *) "/bin/ps";
  char local_command[255];
  int in_fd = 0;

  if( strcmp(host, "localhost") == 0 )
  {
// printf("login=%s\n", login);
    strcpy(local_command, ps_command);
    sprintf(arg0, "-lu%s", login );
    in_fd = do_ps_cmd ( local_command, arg0, &child_pid );
  } else
  {
    char arg1[255];
    char arg2[255];
    strcpy(local_command, command);
    sprintf(arg0, "%s", host);
    sprintf(arg1, "%s", ps_command);
    sprintf(arg2, "-lu%s", login );
    in_fd = do_ps_cmd ( local_command, arg0, arg1, arg2, &child_pid );
  }
nprintf( DEBUG_PANELS ) ("local_command=(%s)\n", local_command);


  if (in_fd < 0)
  {
    return;
  }

  FILE *input_stuff = fdopen(in_fd, "r");
  extract_ps_list ( host, input_stuff, &_proc_count, new_pid );

  fclose (input_stuff);
  close (in_fd);

  return;
}



void
ProcessListObject::print()
{
  printf("ProcessListObject::print()\n");

  ProcessEntry *pe = NULL;

  ProcessEntryList::Iterator it;
  printf("%-20s %-10s %-20s\n", "Host Name:", "PID",  "Process Name:" );
  for( it = processEntryList.begin();
       it != processEntryList.end();
       ++it )
  {
    pe = (ProcessEntry *)*it;
    printf("%-20s %-10d %-20s\n", pe->host_name, pe->pid, pe->process_name);
  }
}
