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
#include "Clip.hxx"
#include "Experiment.hxx"

// Local Macros
static inline
FILE *predefined_filename (std::string filename)
{
  if (filename.length() == 0) {
    return NULL;
  } if (!strcmp( filename.c_str(), "stdout")) {
    return stdout;
  } else if (!strcmp( filename.c_str(), "stderr")) {
    return stderr;
  } else if (!strcmp( filename.c_str(), "stdin")) {
    return stdin;
  } else {
    return NULL;
  }
}

// Static Local Data

// Allow only one thread at a time through the Command processor.
// Doing this allows only one thread at a time to allocate sequence numbers.
static CMDID Command_Sequence_Number = 0;

// To the outside world, Window_IDs are just simple integers.
// Allow only one thread at a time to allocate a Window ID or to
// add, remove or search the list of defined windows.
static pthread_mutex_t Window_List_Lock = PTHREAD_MUTEX_INITIALIZER;
static std::list<CommandWindowID *> CommandWindowID_list;
static CMDWID Command_Window_ID = 0;
static CMDWID Last_ReadWindow = 0;


// Input_Source
#define DEFAULT_INPUT_BUFFER_SIZE 4096

static char *current_prompt = "openss";

class Input_Source
{
 protected:
  Input_Source *Next_Source;
  bool Predefined;
  bool Prompt_Required;
  std::string Name;
  FILE *Fp;
  int64_t Buffer_Size;
  int64_t Next_Line_At;
  int64_t Last_Valid_Data;
  char *Buffer;
  bool Trace_To_A_Predefined_File;
  std::string Trace_Name;
  FILE *Trace_F;

 public:
  // Constructor & Destructor
  Input_Source (std::string my_name) {
    Next_Source = NULL;
    Name = my_name;
    Fp = predefined_filename (my_name);
    Predefined = (Fp != NULL);
    if (Fp == NULL) {
      Predefined = false;
      Prompt_Required = false;
      Fp = fopen (my_name.c_str(), "r");
      if (Fp == NULL) {
        fprintf(stderr,"ERROR: Unable to open alternate command file %s\n",my_name.c_str());
      }
    } else {
      Predefined = true;
      Prompt_Required = isatty(fileno(Fp));
    }
    Next_Line_At = 0;
    Last_Valid_Data = 0;
    Buffer_Size = DEFAULT_INPUT_BUFFER_SIZE;
    Buffer = (char *)malloc(Buffer_Size);
    Trace_To_A_Predefined_File = false;
    Trace_Name = std::string("");
    Trace_F = NULL;
  }
  Input_Source (int64_t buffsize, char *buffer) {
    Next_Source = NULL;
    Name = std::string("");
    Fp = NULL;
    Predefined = false;
    Prompt_Required = false;
    Next_Line_At = 0;
    Last_Valid_Data = buffsize-1;
    Buffer_Size = buffsize;
    Buffer = buffer;
    Trace_To_A_Predefined_File = false;
    Trace_Name = std::string("");
    Trace_F = NULL;
  }
  ~Input_Source () {
   /* Assume that this routine has ownership of any buffer it was given. */
    free (Buffer);
   /* Close input files. */
    if (Fp && !Predefined) {
      fclose (Fp);
    }
   /* Close trace files. */
    if (Trace_F && !Trace_To_A_Predefined_File) {
      fclose (Trace_F);
    }
  }

  void Link (Input_Source *inp) { Next_Source = inp; }
  Input_Source *Next () { return Next_Source; }

  char *Get_Next_Line () {
    if (Next_Line_At >= Last_Valid_Data) {
      if (Fp == NULL) {
        return NULL;
      }
      Buffer[0] = *("\0");
      Buffer[Buffer_Size-1] = (char)0;
      Next_Line_At = 0;
      Last_Valid_Data = 0;
      if (Prompt_Required) {
        fprintf(stderr,"%s->",current_prompt);
      }
      fgets (&Buffer[0], Buffer_Size, Fp);
      if (Buffer[Buffer_Size-1] != (char)0) {
        fprintf(stderr,"ERROR: Input line from %s is too long for buffer.\n",Name.c_str());
        return NULL;
      }
      Last_Valid_Data = strlen(&Buffer[0]);
    }
    char *next_line = &Buffer[Next_Line_At];
    int64_t line_len = strlen(next_line);
    int i;
    for (i = 1; i < line_len; i++) {  
      if (next_line[i] == *("\n")) {
        line_len = i;
        break;
      }
    }
    if (next_line[0] == *("\0")) {
     /* This indicates an EOF */
      return NULL;
    }
    Next_Line_At += line_len;
    return next_line;
  }

  void Set_Trace ( std::string tofname) {
    if (Trace_F && !Trace_To_A_Predefined_File) {
      fclose (Trace_F);
    }
    FILE *tof = predefined_filename( tofname );
    bool is_predefined = (tof != NULL);
    if (tof == NULL) tof = fopen (tofname.c_str(), "a");
    Trace_To_A_Predefined_File = is_predefined;
    Trace_Name = tofname;
    Trace_F = tof;
  }
  void Remove_Trace () {
    if (Trace_F && !Trace_To_A_Predefined_File) {
      fclose (Trace_F);
    }
    Trace_To_A_Predefined_File = false;
    Trace_Name = std::string("");
    Trace_F = NULL;
  }
  bool Trace_File_Is_Predefined () { return Trace_To_A_Predefined_File; }
  std::string Trace_File_Name () { return Trace_Name; }
  FILE *Trace_File () { return Trace_F; }

  // Debug aids
  void Dump(FILE *TFile) {
    fprintf(TFile,"  Active Input Source Stack:\n");
    for (Input_Source *inp = this; inp != NULL; inp = inp->Next()) {
      fprintf(TFile,"    Read from: %s",(inp->Fp) ? inp->Name.c_str() : "buffer");
      if (!inp->Fp) fprintf(TFile," len=%d, next=%d",inp->Buffer_Size,inp->Next_Line_At);
      fprintf(TFile,"\n");
      if (inp->Trace_F) {
        fprintf(TFile,"     trace to: %s\n",inp->Trace_Name.c_str());
      }
    }
  }
};

// CommandWindowID
class CommandWindowID;

class CommandWindowID
{
 protected:
  CMDWID id;
  bool remote;
  std::string I_Call_Myself;
  std::string Host_ID;
  pid_t Process_ID;
  int64_t Panel_ID;
  int64_t *MsgWaitingFlag;
  int64_t Current_Input_Level;
  int64_t Cmd_Count_In_Trace_File;
  std::string Trace_File_Name;
  FILE *Trace_F;
  Input_Source *Input;
  pthread_mutex_t Input_List_Lock;
  EXPID FocusedExp;

 public:
  // Constructor & Destructor
  CommandWindowID ( std::string IAM, std::string  Host, pid_t Process, int64_t Panel)
    {
      remote = false;
      I_Call_Myself = IAM;
      Host_ID = Host;
      Process_ID = Process;
      Panel_ID = Panel;
      Current_Input_Level = 0;
      Cmd_Count_In_Trace_File = 0;
      Trace_File_Name = "";
      Trace_F = NULL;
      Input = NULL;
      Assert(pthread_mutex_init(&Input_List_Lock, NULL) == 0); // dynamic initialization
      FocusedExp = 0;

     // Generate a unique ID and remember it

     // Get exclusive access to the lock so that only one
     // add/remove/search of the list is done at a time.
      Assert(pthread_mutex_lock(&Window_List_Lock) == 0);

      id = ++Command_Window_ID;
      CommandWindowID_list.push_front(this);

     // Release the lock.
      Assert(pthread_mutex_unlock(&Window_List_Lock) == 0);

     // Allocate a trace file for commands associated with this window
      char base[20];
      snprintf(base, 20, "ss%lld.XXXXXX",id);
      Trace_File_Name = tempnam ("./", &base[0] );
      Trace_F  = fopen (Trace_File_Name.c_str(), "w");
    }
  ~CommandWindowID()
    {
     // Remove the trace files
      if (Trace_F) {
        fclose (Trace_F);
        remove (Trace_File_Name.c_str());
      }
     // Remove the input specifiers
      if (Input) {
        for (Input_Source *inp = Input; inp != NULL; ) {
          Input_Source *next = inp->Next();
          delete inp;
          inp = next;
        }
        Input = NULL;
      }
     // Remove the control structures associate with the lock
      pthread_mutex_destroy(&Input_List_Lock);

     // Unlink from the chain of windows

     // Get exclusive access to the lock so that only one
     // add/remove/search of the list is done at a time.
      Assert(pthread_mutex_lock(&Window_List_Lock) == 0);

      if (*CommandWindowID_list.begin()) {
        CommandWindowID_list.remove(this);
      }

     // Release the lock.
      Assert(pthread_mutex_unlock(&Window_List_Lock) == 0);
    }

  void Append_Input_Source (Input_Source *inp) {
   // Get exclusive access to the lock so that only one
   // read, write, add or delete is done at a time.
    Assert(pthread_mutex_lock(&Input_List_Lock) == 0);

    if (Input == NULL) {
      Input = inp;
    } else {
      Input_Source *previous_inp = Input;
      while (previous_inp->Next() != NULL) {
        previous_inp = previous_inp->Next();
      }
      previous_inp->Link(inp);
    }

   // Release the lock.
    Assert(pthread_mutex_unlock(&Input_List_Lock) == 0);
  }
  void Push_Input_Source (Input_Source *inp) {
   // Get exclusive access to the lock so that only one
   // read, write, add or delete is done at a time.
    Assert(pthread_mutex_lock(&Input_List_Lock) == 0);

    Input_Source *previous_inp = Input;
    inp->Link(previous_inp);
    Input = inp;

   // Release the lock.
    Assert(pthread_mutex_unlock(&Input_List_Lock) == 0);
  }
private:
  void Pop_Input_Source () {
   // We do not need to get exclusive access to Input_List_Lock
   // Because the only path to this routine is through Read_Command,
   // which already has exclusive use of the lock.

    Input_Source *old = Input;
    Input = Input->Next();
    delete old;
  }
public:
  char *Read_Command () {
   // Get exclusive access to the lock so that only one
   // read, write, add or delete is done at a time.
    Assert(pthread_mutex_lock(&Input_List_Lock) == 0);

    char *next_line = NULL;
    while (Input != NULL) {
      next_line = Input->Get_Next_Line ();
      if (next_line != NULL) {
        break;
      }
      Pop_Input_Source();
    }

   // Release the lock.
    Assert(pthread_mutex_unlock(&Input_List_Lock) == 0);

    return next_line;
  }

  bool Input_Available () {
    return  ((Input != NULL));
  }

  // Field access
  std::string Trace_Name() { return Trace_File_Name.c_str(); }
  FILE   *Trace_File() { return Trace_F; }
  CMDWID  ID () { return id; }
  EXPID   Focus () { return FocusedExp; }
  void    Set_Focus (EXPID exp) { FocusedExp = exp; }
  int64_t Input_Level () { return Current_Input_Level; }
  void    Increment_Level () { Current_Input_Level++; }
  void    Decrement_Level () { Current_Input_Level--; }

  void   Set_Alt_Trace_File ( std::string tofname ) {
    if (Input) {
      Input->Set_Trace ( tofname );
    }
  }
  void   Remove_Alt_Trace_File () {
    if (Input) {
      Input->Remove_Trace ();
    }
  }

  // For error reporting
  void Print_Location(FILE *TFile) {
    fprintf(TFile,"%s %lld",Host_ID.c_str(),(int64_t)Process_ID);
  }
  // Debug aids
  void Print(FILE *TFile) {
    fprintf(TFile,
       "W %lld: remote:%s IAM:%s %s %lld %lld, history->%s\n",
        id,remote?"T":"F",I_Call_Myself.c_str(),Host_ID.c_str(),(int64_t)Process_ID,Panel_ID,
        Trace_File_Name.c_str());
    if (Input) {
      Input->Dump(TFile );
    }
  }
  void Trace (CommandObject *clip) {
    if (Trace_File()) {
      if (clip->Action() == CMD_PARSE) {
        FILE *TFile = Trace_File();
        CMDID seq_num = clip->Seq();
        CMDWID who = clip->Who();
        time_t cmd_time = clip->When();
        std::string command = clip->Command();
        fprintf(TFile,"C %lld (W%lld.L%lld@%.24s): ",
                      seq_num,who,Input_Level(),ctime(&cmd_time));
        if (command.length() != 0) {
          fprintf(TFile,"%s\n", command.c_str());
        }  

      }
    }
    if (Input) {
      FILE *ct = Input->Trace_File();
      if (ct) {
        fprintf(ct,"%s\n", clip->Command().c_str());
        if (Input->Trace_File_Is_Predefined()) {
         // Trace_File is something like stdout, so push the message out to user.
          fflush (ct);
        }
      }
    }
  }
  void Trace (ResultObject *rslt) {
    if (Trace_F) {
      RStatus status = rslt->Status();
      std::string result_type = rslt->Type();
      std::string msg_string = rslt->ResultMsg();
      void *result = rslt->Result();
      fprintf(Trace_F,
       "R %s %s 0x%x %s\n", (status == SUCCESS) ? "SUCCESS"
                                : (status == FAILURE) ? "FAILURE"
                                : (status == EXIT) ? "EXIT"
                                : "PARTIAL_SUCCESS",
                          (result_type.length()) ? result_type.c_str() : "notype",
                          (result != NULL) ? result : 0,
                          msg_string.c_str());
    }
  }
  void Dump(FILE *TFile)
    {
      std::list<CommandWindowID *>::reverse_iterator cwi;
      for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++)
      {
        (*cwi)->Print(TFile);
      }
    }
};

// Local Utilities

CommandWindowID *Find_Command_Window (CMDWID WindowID)
{
  CommandWindowID *found_window = NULL;

 // Get exclusive access to the lock so that only one
 // add/remove/search of the list is done at a time.
  Assert(pthread_mutex_lock(&Window_List_Lock) == 0);

// Search for existing entry.
  if (WindowID > 0) {
    std::list<CommandWindowID *>::iterator cwi;
    for (cwi = CommandWindowID_list.begin(); cwi != CommandWindowID_list.end(); cwi++) {
      if (WindowID == (*cwi)->ID ()) {
        found_window = *cwi;
        break;
      }
    }
  }

 // Release the lock.
  Assert(pthread_mutex_unlock(&Window_List_Lock) == 0);

  return found_window;
}

int64_t Find_Command_Level (CMDWID WindowID)
{
  CommandWindowID *cwi = Find_Command_Window (WindowID);
  return (cwi != NULL) ? cwi->Input_Level() : 0;
}

// Low Level Semantic Routines

// What is the current focus associated with a CommandWindow?
ResultObject Experiment_Focus (CMDWID WindowID)
{
  CommandWindowID *my_window = Find_Command_Window (WindowID);
  if (my_window && my_window->Focus()) {
    return ResultObject ( SUCCESS, "ExperimentObject", (void *)my_window->Focus(), "");
  } else {
    return ResultObject ( SUCCESS, "", NULL, "There is no Focused Experiment");
  }
}
// Set the focus for a particular CommandWindow.
ResultObject Experiment_Focus (CMDWID WindowID, EXPID ExperimentID)
{
  CommandWindowID *my_window = Find_Command_Window (WindowID);
  if (my_window) {
    ExperimentObject *Experiment = (ExperimentID) ? Find_Experiment_Object (ExperimentID) : NULL;
    my_window->Set_Focus(ExperimentID);
    if (ExperimentID) {
      return (Experiment != NULL)
             ? ResultObject ( SUCCESS, "ExperimentObject", (void *)my_window, "") 
             : ResultObject ( FAILURE, "Experiment could not be found" );
    }
  }
  return ResultObject ( SUCCESS, "ExperimentObject", NULL, "");
}

void List_CommandWindows ( FILE *TFile )
{
  if (*CommandWindowID_list.begin()) {
    (*CommandWindowID_list.begin())->Dump(TFile);
  } else {
    fprintf(TFile,"\n(there are no defined windows)\n");
  }
}

void Trace_File_History (enum Trace_Entry_Type trace_type, std::string fname, FILE *TFile)
{
  FILE *cmdf = fopen (fname.c_str(), "r");
  struct stat stat_buf;
  if (!cmdf) {
    fprintf(stderr,"ERROR: Unable to open trace file %s\n",fname.c_str());
    return;
  }
  if (stat (fname.c_str(), &stat_buf) != 0) {
    fprintf(stderr,"ERROR: Unable to perform fstat command on %s\n",fname.c_str());
    return;
  }
  char *s = (char *)malloc(stat_buf.st_size+1);
  for (int i=0; i < stat_buf.st_size; ) {
    fgets (s, (stat_buf.st_size), cmdf);
    int len  = strlen(s);
    if (len > 2) {
      bool dump_this_record = false;
      char *t = s;
      switch (trace_type)
      {
        case CMDW_TRACE_ALL :
         // Dump raw data - i.e. all of every record.
          dump_this_record = true;
          break;
        case CMDW_TRACE_COMMANDS:
         // Dump records with leading "C ".
          if ((*s) ==  (*"C ")) {
            dump_this_record = true;
            t+=2;
          }
          break;
        case CMDW_TRACE_ORIGINAL_COMMANDS:
         // Dump records with leading "C " but strip off time stamp
          if ((*s) ==  (*"C ")) {
            for (int j = 0; j < len; t++, j++) {
              if (!strncmp(t,")",1)) break;
            }
            if (strlen(t) > 2) {
              dump_this_record = true;
              t+=3;
            }
          }
          break;
        case CMDW_TRACE_RESULTS:
         // Dump records with leading "R ".
          if ((*s) ==  (*"R ")) {
            dump_this_record = true;
            t+=2;
          }
          break;
      }
      if (dump_this_record) {
        *(s+len-1) = *("\n");
        fprintf(TFile,"%s",t);
      }
    }
    i+=len;
  }
  free (s);
  fclose (cmdf);
}

// Read the trace file and echo selected entries to another file.
void Command_Trace (enum Trace_Entry_Type trace_type, CMDWID cmdwinid, std::string tofname)
{
  FILE *tof = predefined_filename( tofname );
  bool tof_predefined = (tof != NULL);
  if (tof == NULL) tof = fopen (tofname.c_str(), "a");

  std::list<CommandWindowID *>::reverse_iterator cwi;
  for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++)
  {
    if ((cmdwinid == 0) ||
        (cmdwinid == (*cwi)->ID ())) {
      std::string cmwtrn = (*cwi)->Trace_Name();
      FILE *cmwtrf = (*cwi)->Trace_File();
      if (!cmwtrf) {
        fprintf(stderr,"ERROR: no trace file %s\n",cmwtrn.c_str());
        return;
      }
      if (fflush (cmwtrf)) {
        fprintf(stderr,"ERROR: can not flush trace file %s\n",cmwtrn.c_str());
        return; 
      }
      Trace_File_History (trace_type, cmwtrn, tof);
    }
  }
  fflush(tof);
  if (!tof_predefined) {
    fclose (tof);
  }
}

// Set up an alternative trace file at user request.
ResultObject Command_Trace_ON (CMDWID WindowID, std::string tofname)
{
  CommandWindowID *cmdw = Find_Command_Window (WindowID);
  cmdw->Set_Alt_Trace_File (tofname);
  return ResultObject(SUCCESS, "Command_Trace_ON", (void *)0, "");
}
ResultObject Command_Trace_OFF (CMDWID WindowID)
{
  CommandWindowID *cmdw = Find_Command_Window (WindowID);
  cmdw->Remove_Alt_Trace_File ();
  return ResultObject(SUCCESS, "Command_Trace_OFF", (void *)0, "");
}

// This is the start of the Command Line Processing Routines.
// Only one thread can be executing one of these rotuines at a time,
// so the must be protected with the use of Window_List_Lock.

CMDWID Commander_Initialization (char *my_name, char *my_host, pid_t my_pid, int64_t my_panel)
{
 // Create a new Window
  CommandWindowID *cwid = new CommandWindowID(std::string(my_name ? my_name : ""),
                                              std::string(my_host ? my_host : ""),
                                              my_pid, my_panel);
  return cwid->ID();
}

void Commander_Termination (CMDWID im)
{
  if (im) {
    CommandWindowID *my_window = Find_Command_Window (im);
    if (my_window) delete my_window;
  }
  return;
}


ResultObject Append_Input_Buffer (CMDWID issuedbywindow, int64_t b_size, char *b_ptr) {
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);
  Assert (cw);
  Input_Source *inp = new Input_Source (b_size, b_ptr);
  cw->Append_Input_Source (inp);
  return ResultObject(SUCCESS, "Command_File_T", NULL, "Command file read and processed");
}

ResultObject Append_Input_String (CMDWID issuedbywindow, int64_t b_size, char *b_ptr) {
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);
  Assert (cw);
  int64_t buffer_size = b_size+1;
  char *buffer = (char *)malloc(buffer_size);
  strncpy (buffer, b_ptr, buffer_size);
  Input_Source *inp = new Input_Source (buffer_size, buffer);
  cw->Append_Input_Source (inp);
  return ResultObject(SUCCESS, "Command_File_T", NULL, "Command file read and processed");
}

ResultObject Append_Input_File (CMDWID issuedbywindow, std::string fromfname) {
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);
  Assert (cw);
  Input_Source *inp = new Input_Source (fromfname);
  cw->Append_Input_Source (inp);
  return ResultObject(SUCCESS, "Command_File_T", NULL, "Command file read and processed");
}

ResultObject Push_Input_Buffer (CMDWID issuedbywindow, int64_t b_size, char *b_ptr) {
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);
  Assert (cw);
  Input_Source *inp = new Input_Source (b_size, b_ptr);
  cw->Push_Input_Source (inp);
  return ResultObject(SUCCESS, "Command_File_T", NULL, "Command file read and processed");
}

ResultObject Push_Input_File (CMDWID issuedbywindow, std::string fromfname) {
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);
  Assert (cw);
  Input_Source *inp = new Input_Source (fromfname);
  cw->Push_Input_Source (inp);
  return ResultObject(SUCCESS, "Command_File_T", NULL, "Command file read and processed");
}

// The algorithm is to do a round-robin search of the defined window list
// by starting with the last widow that was read from.  The usual choice
// to read from the next window in the list but, if the parser indicates
// that it is processing a complex statement (e.g. a loop) the next line
// read MUST come from the previous window.
static CMDWID select_input_window (int is_more) {
  CMDWID selectwindow = Last_ReadWindow;

 // Get exclusive access to the lock so that only one
 // add/remove/search of the list is done at a time.
  Assert(pthread_mutex_lock(&Window_List_Lock) == 0);

  if (is_more == 0) {

    std::list<CommandWindowID *>::iterator cwi;
    for (cwi = CommandWindowID_list.begin(); cwi != CommandWindowID_list.end(); cwi++) {
      if (selectwindow == (*cwi)->ID ()) {
        std::list<CommandWindowID *>::iterator next_cwi = ++cwi;
        if (next_cwi == CommandWindowID_list.end()) {
          next_cwi = CommandWindowID_list.begin();
        }
        selectwindow = (*next_cwi)->ID();
        goto window_found;
      }
    }

   // A fall through the loop indicates that we didn't find the Last_ReadWindow.
   // Do error recovery by choosing the first one on the list.
    selectwindow = (*CommandWindowID_list.begin())->ID();

  }

window_found:

 // Release the lock.
  Assert(pthread_mutex_unlock(&Window_List_Lock) == 0);

  Assert (selectwindow);
  Last_ReadWindow = selectwindow;
  return selectwindow;
}

#include "Python.h"

char *SpeedShop_ReadLine (int is_more)
{
  char *save_prompt = current_prompt;
  if (is_more) {
    current_prompt = "....ss";
  }

  CMDWID readfromwindow = select_input_window(is_more);
  CMDWID firstreadwindow = readfromwindow;

read_another_window:

  CommandWindowID *cw = Find_Command_Window (readfromwindow);
  Assert (cw);
  char *s;

  do {
    s = cw->Read_Command ();
    if (s == NULL) {
     // The read failed.  Why?  Can we find something else to read?
      if (is_more) {
       // We MUST read from this window!
       // This is an error situation.  How can we recover?
      }
     // It might be possible to read from a different window.
     // Try to find another one.
      readfromwindow = select_input_window(is_more);
      if (readfromwindow != firstreadwindow) {
       // There is another window to read from.
        goto read_another_window;
      }
     // Sould we attempt to read from stdin?
     // HOW DO WE FORCE A WAIT UNTIL DATA IS READY?

     // The end of all window inputs has been reached.
     // Return an empty line to indicate an EOF.
      break;
    }
    int len  = strlen(s);
    if (len > 1) {
      if (!strncasecmp ( s, "quit\n", 5)) {
fprintf(stdout,"quit instruction encountered\n");
        s = NULL;
        break;
      }
    }
  } while (s == NULL);

  current_prompt = save_prompt;
  if (s == NULL) {
    return NULL;
  }

  char *sbuf = (char *)PyMem_Malloc(strlen(s) + 1);
  strcpy (sbuf, s);
  CommandObject *clip = new CommandObject(readfromwindow,sbuf);

 // Assign a sequence number to the command.
  clip->SetSeq (++Command_Sequence_Number);

 // Log the command.
  cw->Trace(clip);

  return sbuf;
}
