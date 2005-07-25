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

#include "SS_Input_Manager.hxx"
extern "C" void loadTheGUI(ArgStruct *);

char *Current_OpenSpeedShop_Prompt = "openss>>";
char *Alternate_Current_OpenSpeedShop_Prompt = "....ss>";

int64_t History_Limit = DEFAULT_HISTORY_BUFFER;
int64_t History_Count = 0;
std::list<std::string> History;

static FILE *ttyin = NULL;  // Read directly from this xterm window.
static FILE *ttyout = NULL; // Write directly to this xterm window.

static ss_ostream *ss_err = NULL;
static ss_ostream *ss_out = NULL;
static ss_ostream *ss_ttyout = NULL;

// Forward definitions of local functions
void Default_TLI_Command_Output (CommandObject *C);
bool All_Command_Objects_Are_Used (InputLineObject *clip);

inline std::string ptr2str (void *p) {
  char s[40];
  sprintf ( s, "%p", p);
  return std::string (s);
}

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
  } else if (!strcmp( filename.c_str(), "/dev/tty")) {
    return ttyout;
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
static bool Async_Inputs = false;
static bool Looking_for_Async_Inputs = false;
static CMDWID More_Input_Needed_From_Window = 0;
static pthread_mutex_t Async_Input_Lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  Async_Input_Available = PTHREAD_COND_INITIALIZER;

static CMDWID Default_WindowID = 0;
static CMDWID TLI_WindowID = 0;
static CMDWID GUI_WindowID = 0;
static CMDWID Embedded_WindowID = 0;


// Input_Source
#define DEFAULT_INPUT_BUFFER_SIZE 4096

class Input_Source
{
 protected:
  Input_Source *Next_Source;
  bool Predefined;
  std::string Name;
  FILE *Fp;
  int64_t Buffer_Size;
  int64_t Next_Line_At;
  int64_t Last_Valid_Data;
  char *Buffer;
  InputLineObject *Input_Object;
  bool Record_To_A_Predefined_File;
  std::string Record_Name;
  FILE *Record_F;

 // Used if files are to be read
 // These pointers are copied to the InputLineObject generated for each line from the file.
  void (*CallBackLine) (InputLineObject *b);  // Optional call back function to notify output routine.
  void (*CallBackCmd) (CommandObject *b);  // Optional call back function for Command Objects.


 public:
  // Constructor & Destructor
  Input_Source (std::string my_name) {
    Next_Source = NULL;
    Name = my_name;
    Fp = predefined_filename (my_name);
    if (Fp == NULL) {
      Predefined = false;
      Fp = fopen (my_name.c_str(), "r");
      if (Fp == NULL) {
       // Calling routines must use InFileError() to check for error.
      }
    } else {
      Predefined = true;
    }
    Next_Line_At = 0;
    Last_Valid_Data = 0;
    Buffer_Size = DEFAULT_INPUT_BUFFER_SIZE;
    Buffer = (char *)malloc(Buffer_Size);
    Input_Object = NULL;
    Record_To_A_Predefined_File = false;
    Record_Name = std::string("");
    Record_F = NULL;
    CallBackLine = NULL;
    CallBackCmd = NULL;
  }
  Input_Source (InputLineObject *clip) {
    Next_Source = NULL;
    Name = std::string("");
    Fp = NULL;
    Predefined = false;
    Next_Line_At = 0;
    Last_Valid_Data = 0;
    Buffer_Size = 0;
    Buffer = NULL;
    Input_Object = clip;
    Record_To_A_Predefined_File = false;
    Record_Name = std::string("");
    Record_F = NULL;
    CallBackLine = NULL;
    CallBackCmd = NULL;
  }
  Input_Source (int64_t buffsize, char *buffer) {
    Next_Source = NULL;
    Name = std::string("");
    Fp = NULL;
    Predefined = false;
    Next_Line_At = 0;
    Last_Valid_Data = buffsize-1;
    Buffer_Size = buffsize;
    Buffer = buffer;
    Input_Object = NULL;
    Record_To_A_Predefined_File = false;
    Record_Name = std::string("");
    Record_F = NULL;
    CallBackLine = NULL;
    CallBackCmd = NULL;
  }
  ~Input_Source () {
   /* Assume that this routine has ownership of any buffer it was given. */
    if (Buffer) {
      free (Buffer);
    }
   /* Close input files. */
    if (Fp) {
      fclose (Fp);
    }
   /* Close Record files. */
    if (Record_F && !Record_To_A_Predefined_File) {
      fclose (Record_F);
    }
  }

  void Set_CallBackL  (void (*cbf) (InputLineObject *b)) { CallBackLine = cbf; }
  void Set_CallBackC  (void (*cbf) (CommandObject *b)) { CallBackCmd = cbf; }
  void Copy_CallBack  (InputLineObject *clip) {
    if (CallBackLine != NULL) clip->Set_CallBackL (CallBackLine);
    if (CallBackCmd  != NULL) clip->Set_CallBackC (CallBackCmd); 
  }

  void Link (Input_Source *inp) { Next_Source = inp; }
  Input_Source *Next () { return Next_Source; }
  InputLineObject *InObj () { return Input_Object; }
  bool InFileError () { return (Fp == NULL); }

  char *Get_Next_Line () {
    if (Next_Line_At >= Last_Valid_Data) {
      if (Fp == NULL) {
        return NULL;
      }
      Buffer[0] = *("\0");
      Buffer[Buffer_Size-1] = (char)0;
      Next_Line_At = 0;
      Last_Valid_Data = 0;
      fgets (&Buffer[0], Buffer_Size, Fp);
      if (Buffer[Buffer_Size-1] != (char)0) {
        FILE *ef = ((ttyout != NULL)  && isatty(fileno(stderr))) ? ttyout : stderr;
        fprintf(ef,"ERROR: Input line from %s is too long for buffer.\n",Name.c_str());
        if (ttyout == ef) SS_Issue_Prompt (ttyout);
        return NULL;
      }
      Last_Valid_Data = strlen(&Buffer[0]);
    }
    char *next_line = &Buffer[Next_Line_At];
    int64_t line_len = strlen(next_line);
    int64_t i;
    for (i = 1; i < line_len; i++) {  
      if (next_line[i] == *("\0")) {
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

  bool Set_Record ( std::string tofname) {
    if (Record_F && !Record_To_A_Predefined_File) {
      fclose (Record_F);
    }
    FILE *tof = predefined_filename( tofname );
    bool is_predefined = (tof != NULL);
    if (tof == NULL) tof = fopen (tofname.c_str(), "a");
    Record_To_A_Predefined_File = is_predefined;
    Record_Name = tofname;
    Record_F = tof;
    return (tof != NULL);
  }
  void Remove_Record () {
    if (Record_F && !Record_To_A_Predefined_File) {
      fclose (Record_F);
    }
    Record_To_A_Predefined_File = false;
    Record_Name = std::string("");
    Record_F = NULL;
  }
  bool Record_File_Is_Predefined () { return Record_To_A_Predefined_File; }
  std::string Record_File_Name () { return Record_Name; }
  FILE *Record_File () { return Record_F; }

  // Debug aids
  void Dump (ostream &mystream) {
    bool nl_at_eol = false;
    mystream << "    Read from: " << ((Fp) ? Name : (Input_Object) ? "image " : "buffer ");
    if (Input_Object != NULL) {
      Input_Object->Print (mystream);
      nl_at_eol = true;
    } else if (!Fp) {
      mystream << "len=" << Buffer_Size << ", next=" << Next_Line_At;
      if (Buffer_Size > Next_Line_At) {
        int64_t nline = MIN (20,strlen (&(Buffer[Next_Line_At])));
        nl_at_eol = (Buffer[Next_Line_At+nline] == *("\n"));
        if (nline > 2) {
          mystream << ": " << std::setiosflags(std::ios::left) << std::setw(20) << Buffer;
          if (nline > 20) mystream << "...";
        }
      }
    }
    if (!nl_at_eol) {
      mystream << std::endl;
    }
    if (Record_F) {
      mystream << "     record to: "<< Record_Name << std::endl;
    }
  }
/*
  void Dump(FILE *TFile) {
      bool nl_at_eol = false;
      fprintf(TFile,"    Read from: %s",
                    (Fp) ? Name.c_str() : 
                    (Input_Object) ? "image " : "buffer ");
      if (Input_Object != NULL) {
        Input_Object->Print (TFile);
        nl_at_eol = true;
      } else if (!Fp) {
        fprintf(TFile,"len=%d, next=%d",Buffer_Size,Next_Line_At);
        if (Buffer_Size > Next_Line_At) {
          int64_t nline = strlen (&(Buffer[Next_Line_At]));
          nl_at_eol = (Buffer[Next_Line_At+nline] == *("\n"));
          if (nline > 2) {
            fprintf(TFile,": %.20s", &(Buffer[Next_Line_At]));
            if (nline > 20) fprintf(TFile,"...");
          }
        }
      }
      if (!nl_at_eol) {
        fprintf(TFile,"\n");
      }
      if (Record_F) {
        fprintf(TFile,"     record to: %s\n",Record_Name.c_str());
      }
  }
*/
};

// CommandWindowID
class CommandWindowID
{
 protected:
  CMDWID id;
  FILE *default_output;
  ss_ostream *default_outstream;
  ss_ostream *default_errstream;
  bool remote;
  std::string I_Call_Myself;
  std::string Host_ID;
  pid_t Process_ID;
  int64_t Panel_ID;
  int64_t *MsgWaitingFlag;
  int64_t Current_Input_Level;
  int64_t Cmd_Count_In_Log_File;
  std::string Log_File_Name;
  FILE *Log_F;
  bool Log_File_Is_A_Temporary_File;
  std::string Record_File_Name;
  FILE *Record_F;
  bool Record_File_Is_A_Temporary_File;
  bool Input_Is_Async;
  Input_Source *Input;
  pthread_mutex_t Input_List_Lock;
  EXPID FocusedExp;

  pthread_mutex_t Cmds_List_Lock;
  std::list<InputLineObject *>Complete_Cmds;

 public:
  // Virtual functions for creating InputLineObjects
  // This is the default for the command line window
  virtual InputLineObject *New_InputLineObject (CMDWID From, std::string Cmd)
    {
      return new InputLineObject (From, Cmd);
    }

  // Constructor & Destructor
 public:
  CommandWindowID () {
   // Disallow default constructor.
   // It can not be private because of inheritance.
   Assert(0);
  }

 public:
  CommandWindowID ( std::string IAM, std::string  Host, pid_t Process, int64_t Panel, bool async)
    {
      default_output = NULL;
      default_outstream = ss_out;
      default_errstream = ss_err;
      remote = false;
      I_Call_Myself = IAM;
      Host_ID = Host;
      Process_ID = Process;
      Panel_ID = Panel;
      Current_Input_Level = 0;
      Cmd_Count_In_Log_File = 0;
      Log_File_Name = "";
      Log_F = NULL;
      Log_File_Is_A_Temporary_File = false;
      Record_File_Name = "";
      Record_F = NULL;
      Record_File_Is_A_Temporary_File = false;
      Input = NULL;
      Input_Is_Async = async;
      if (Input_Is_Async) {
        Assert(pthread_mutex_init(&Input_List_Lock, NULL) == 0); // dynamic initialization
      }
      Assert(pthread_mutex_init(&Cmds_List_Lock, NULL) == 0); // dynamic initialization
      FocusedExp = -1;  // This is a "not yet intitialized" flag.  The user should never see it.

     // Generate a unique ID and remember it

     // Get exclusive access to the lock so that only one
     // add/remove/search of the list is done at a time.
      Assert(pthread_mutex_lock(&Window_List_Lock) == 0);

      id = ++Command_Window_ID;
      CommandWindowID_list.push_front(this);

     // Release the lock.
      Assert(pthread_mutex_unlock(&Window_List_Lock) == 0);

     // Allocate a log file for commands associated with this window
      char base[20];
      snprintf(base, 20, "sstr%lld.XXXXXX",id);
      Log_File_Name = std::string(tempnam ("./", &base[0] )) + ".openss";
      Log_F  = fopen (Log_File_Name.c_str(), "w");
      Log_File_Is_A_Temporary_File = true;
      Record_File_Name = "";
      Record_F = NULL;
      Record_File_Is_A_Temporary_File = false;
    }
  ~CommandWindowID()
    {
     // Clear the identification field in case someone trys to reference
     // this entry again.
      id = 0;
     // Remove the log files
      if ((Log_F != NULL) &&
          (predefined_filename (Log_File_Name) == NULL)) {
        fclose (Log_F);
        if (Log_File_Is_A_Temporary_File) {
          (void ) remove (Log_File_Name.c_str());
        }
      }
     // Remove the record files
      if ((Record_F != NULL) &&
          (predefined_filename (Record_File_Name) == NULL)) {
        fclose (Record_F);
        if (Record_File_Is_A_Temporary_File) {
          (void) remove (Record_File_Name.c_str());
        }
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
      if (Input_Is_Async) {
        pthread_mutex_destroy(&Input_List_Lock);
      }
      pthread_mutex_destroy(&Cmds_List_Lock);

     // Unlink from the chain of windows

     // Get exclusive access to the lock so that only one
     // add/remove/search of the list is done at a time.
      Assert(pthread_mutex_lock(&Window_List_Lock) == 0);

      if (*CommandWindowID_list.begin()) {
        CommandWindowID_list.remove(this);
      }

     // Release the lock.
      Assert(pthread_mutex_unlock(&Window_List_Lock) == 0);


      if (default_errstream &&
          (default_errstream != default_outstream) &&
          (default_errstream != ss_ttyout) &&
          (default_errstream != ss_out) &&
          (default_errstream != ss_err)) {
        delete default_errstream;
        default_errstream = NULL;
      }
      if (default_outstream &&
          (default_outstream != ss_ttyout) &&
          (default_errstream != ss_out) &&
          (default_errstream != ss_err)) {
        delete default_outstream;
        default_outstream = NULL;
      }
    }

  void Purge_All_Input () {
   // Get the lock to this window's inputs
    Assert(pthread_mutex_lock(&Input_List_Lock) == 0);

   // Go throught the list of remaining inputs and delete them.
    for (Input_Source *inp = Input; inp != NULL; ) {
      Input_Source *next = inp->Next();
      delete inp;
      inp = next;
    }
    Input = NULL;

   // Release the lock
    Assert(pthread_mutex_unlock(&Input_List_Lock) == 0);
  }

  void Abort_Executing_Input_Lines () {
   // Get the lock to this window's current in-process commands.
    Assert(pthread_mutex_lock(&Cmds_List_Lock) == 0);

   // Look through the list for unneeded Commands
    std::list<InputLineObject *>::iterator cmi;
    for (cmi = Complete_Cmds.begin(); cmi != Complete_Cmds.end(); cmi++) {
      InputLineObject *clip = *cmi;
      std::list<CommandObject *> cmd_object = clip->CmdObj_List();
      std::list<CommandObject *>::iterator coi;
      for (coi = cmd_object.begin(); coi != cmd_object.end(); coi++) {
        if (((*coi)->Status() == CMD_PARSED) ||
            ((*coi)->Status() == CMD_EXECUTING)) {
          (*coi)->set_Status (CMD_ABORTED);
        }
      }
     // Terminate the Clip after aborting the CommandObjects so that
     // the final status of commands is entered in the log file.
      clip->SetStatus (ILO_ERROR);
    }

   // Release the lock
    Assert(pthread_mutex_unlock(&Cmds_List_Lock) == 0);
  }

  void Append_Input_Source (Input_Source *inp) {
   // Get exclusive access to the lock so that only one
   // read, write, add or delete is done at a time.
    if (Input_Is_Async) {
      Assert(pthread_mutex_lock(&Input_List_Lock) == 0);
    }

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
    if (Input_Is_Async) {
      Assert(pthread_mutex_unlock(&Input_List_Lock) == 0);
    }

   // After a new comand is placed in the input window,
   // wake up a sleeping input reader.
    if (Looking_for_Async_Inputs) {
      Assert(pthread_mutex_lock(&Async_Input_Lock) == 0);
     // After we get the lock, be sure that the reader
     // is still waiting for input. No need to send a
     // signal if it grabbed the last line before we
     // got ahold of the lock.
      if (Looking_for_Async_Inputs) {
        Looking_for_Async_Inputs = false;
        Assert(pthread_cond_signal(&Async_Input_Available) == 0);
      }
      Assert(pthread_mutex_unlock(&Async_Input_Lock) == 0);
    }
  }
  void Push_Input_Source (Input_Source *inp) {
   // Get exclusive access to the lock so that only one
   // read, write, add or delete is done at a time.
    if (Input_Is_Async) {
      Assert(pthread_mutex_lock(&Input_List_Lock) == 0);
    }

    Input_Source *previous_inp = Input;
    inp->Link(previous_inp);
    Input = inp;

   // Release the lock.
    if (Input_Is_Async) {
      Assert(pthread_mutex_unlock(&Input_List_Lock) == 0);
    }
  }
  void TrackCmd (InputLineObject *Clip) {
   // Get the lock to this window's current in-process commands.
    Assert(pthread_mutex_lock(&Cmds_List_Lock) == 0);

   // Add a new in-process command to the list.
    Complete_Cmds.push_back (Clip);

   // Release the lock
    Assert(pthread_mutex_unlock(&Cmds_List_Lock) == 0);
  }
  void Remove_Completed_Input_Lines () {
   // Get the lock to this window's current in-process commands.
    Assert(pthread_mutex_lock(&Cmds_List_Lock) == 0);

   // Look through the list for unneeded lines
    std::list<InputLineObject *> cmd_object = Complete_Cmds;
    std::list<InputLineObject *>::iterator cmi;
    for (cmi = cmd_object.begin(); cmi != cmd_object.end(); ) {
      if (!(*cmi)->Results_Used ()) {
        if (All_Command_Objects_Are_Used( (*cmi) )) {
          (void)((*cmi)->CallBackL ());
        }
      }
      InputLineObject *L = (*cmi);
      ++cmi;
      if (L->Results_Used ()) {
        Complete_Cmds.remove(L);
      }
    }

   // Release the lock
    Assert(pthread_mutex_unlock(&Cmds_List_Lock) == 0);
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
  InputLineObject *Read_Command () {
   // Is there a good chance of finding something to read?
   // We aren't going to go to the trouble of getting the lock
   // unless there is something waiting to be processed.
    if (Input == NULL) {
     // We don't think there is anything to read.
     //
     // Yes, there is a race condiiton here, but the worst that
     // can happen is that we miss reading a line of input as
     // soon as we could.  Since this read is in a loop, we will
     // eventually get back here to read the new line.
      return NULL;
    }

   // Get exclusive access to the lock so that only one
   // read, write, add or delete is done at a time.
    if (Input_Is_Async) {
      Assert(pthread_mutex_lock(&Input_List_Lock) == 0);
    }

    InputLineObject *clip;
    char *next_line = NULL;

    while (Input != NULL) {
      clip = Input->InObj ();
      if (clip != NULL) {
       // Return this line and remove it from the input stack
        Pop_Input_Source();
        break;
      } else {
       // Read file or buffer to get input.
        next_line = Input->Get_Next_Line ();
        if (next_line != NULL) {
          break;
        }
        Pop_Input_Source();
      }
    }

   // Release the lock.
    if (Input_Is_Async) {
      Assert(pthread_mutex_unlock(&Input_List_Lock) == 0);
    }

    if (clip == NULL) {
      if (next_line == NULL) {
        return NULL;
      }
      clip = New_InputLineObject ( ID(), next_line);
      Input->Copy_CallBack (clip);
    }

    return clip;
  }

  bool Input_Available () {
    return  ((Input == NULL) ? Input_Is_Async : true);
  }

  // Field access
  void    set_output (FILE *output) { default_output = output; }
  void    set_outstream (ss_ostream *output) { default_outstream = output; }
  void    set_errstream (ss_ostream *errput) { default_errstream = errput; }
  FILE   *output () { return default_output; }
  bool has_outstream () { return (default_outstream != NULL); }
  ostream &outstream () { return default_outstream->mystream(); }
  ss_ostream *ss_outstream () { return default_outstream; }
  bool has_errstream () { return (default_errstream != NULL); }
  ostream &errstream () { return default_errstream->mystream(); }
  ss_ostream *ss_errstream () { return default_errstream; }
  std::string Log_Name() { return Log_File_Name.c_str(); }
  FILE       *Log_File() { return Log_F; }
  CMDWID  ID () { return id; }
  EXPID   Focus () { return FocusedExp; }
  void    Set_Focus (EXPID exp) { FocusedExp = exp; }
  bool    Async() {return Input_Is_Async;}
  int64_t Input_Level () { return Current_Input_Level; }
  void    Increment_Level () { Current_Input_Level++; }
  void    Decrement_Level () { Current_Input_Level--; }

 // The "Log" command will causes us to echo state changes that
 // are associate with a particular input stream to a user defined file.
  bool   Set_Log_File ( std::string tofname ) {
    FILE *tof = predefined_filename (tofname);
    if ((Log_F != NULL) &&
        (predefined_filename (Log_File_Name) == NULL)) {
     // Copy the old file to the new file.
      fclose (Log_F);
      if (tof == NULL) {
        int64_t len1 = Log_File_Name.length();
        int64_t len2 = tofname.length();
        char *scmd = (char *)malloc(6 + len1 + len2);
        sprintf(scmd,"mv %s %s\n\0",Log_File_Name.c_str(),tofname.c_str());
        int64_t ret = system(scmd);
        free (scmd);
        if (ret != 0) {
         // Some system error.  Keep the old log file around.
          return false;
        }
        Log_File_Is_A_Temporary_File = false;
      }
    }
    if (Log_File_Is_A_Temporary_File) {
      (void) remove (Log_File_Name.c_str());
    }
    if (tof == NULL) {
      tof = fopen (tofname.c_str(), "a");
    }
    Log_File_Name = tofname;
    Log_F  = tof;
    Log_File_Is_A_Temporary_File = false;
    return (Log_F != NULL);
  }
  void   Remove_Log_File () {
    if ((Log_F != NULL)  &&
        (predefined_filename (Log_File_Name) == NULL)) {
      fclose (Log_F);
    }
    Log_File_Name = std::string("");
    Log_F = NULL;
  }

 // The "Record" command will causes us to echo statements that
 // come from a particular input stream to a user defined file.
  bool Set_Record_File ( std::string tofname ) {
    if (Input) {
      return (Input->Set_Record ( tofname ));
    } else {
      if (Record_F && !Record_File_Is_A_Temporary_File) {
        fclose (Record_F);
      }
      FILE *tof = predefined_filename( tofname );
      bool is_predefined = (tof != NULL);
      if (tof == NULL) tof = fopen (tofname.c_str(), "a");
      Record_File_Is_A_Temporary_File = is_predefined;
      Record_File_Name = tofname;
      Record_F = tof; 
      return (tof != NULL);
    }
  }
  void   Remove_Record_File () {
    if (Input) {
      Input->Remove_Record ();
    } else {
      if (Record_F && !Record_File_Is_A_Temporary_File) {
        fclose (Record_F);
      }
      Record_File_Is_A_Temporary_File = false;
      Record_File_Name = std::string("");
      Record_F = NULL;
    }
  }
  void Record (InputLineObject *clip) {
    FILE *rf = NULL;
    bool is_predefined = false;
    if (Input) {
      rf = Input->Record_File();
      is_predefined = Input->Record_File_Is_Predefined();
    } else {
      rf = Record_F;
      is_predefined = Record_File_Is_A_Temporary_File;
    }
    if (rf) {
      fprintf(rf,"%s", clip->Command().c_str());  // Commands have a "\n" at the end.
      if (is_predefined) {
       // Log_File is something like stdout, so push the message out to user.
        fflush (rf);
      }
    }
  }

  // For error reporting
  void Print_Location(ostream &mystream) {
    mystream << Host_ID << " " << Process_ID;
  }
  // Debug aids
  void Print(ostream &mystream) {
    mystream << "W " << id << ":";
    mystream << (Input_Is_Async?" async":" sync") << (remote?" remote":" local");
    mystream << "IAM:" << I_Call_Myself << " " << Host_ID << " " << Process_ID;
    mystream << "focus at " << Focus() << " log->" << Log_File_Name << std::endl;

    mystream << "      outstreams=" << ptr2str(default_outstream);
    mystream << ", errstream=" << ptr2str (default_errstream) << std::endl;
    if (Input) {
      mystream << "  Active Input Source Stack:" << std::endl;
      for (Input_Source *inp = Input; inp != NULL; inp = inp->Next()) {
        inp->Dump(mystream);
      }
    }

   // Print the list of completed commands
   // Get the lock to this window's current in-process commands.
    Assert(pthread_mutex_lock(&Cmds_List_Lock) == 0);

    std::list<InputLineObject *> cmd_object = Complete_Cmds;
    std::list<InputLineObject *>::iterator cmi;
    for (cmi = cmd_object.begin(); cmi != cmd_object.end(); cmi++) {
      (*cmi)->Print (mystream);
    }

   // Release the lock
    Assert(pthread_mutex_unlock(&Cmds_List_Lock) == 0);
  }
  void Dump(ostream &mystream) {
    std::list<CommandWindowID *>::reverse_iterator cwi;
    for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++)
    {
      (*cwi)->Print(mystream);
    }
  }
};

// For the Text-Line-Interface
class TLI_CommandWindowID : public CommandWindowID
{
 public:
  // Virtual functions for creating InputLineObject and CommandObjects
  // These are the defaults for the command line window
  virtual InputLineObject *New_InputLineObject (CMDWID From, std::string Cmd)
    {
      return new TLI_InputLineObject (From, Cmd);
    }

// Constructors
 private:
  TLI_CommandWindowID () { }  // Hide default constructor to catch errors at compile time

 public:
  TLI_CommandWindowID (std::string IAM, std::string  Host, pid_t Process, int64_t Panel, bool async) :
    CommandWindowID (IAM, Host, Process, Panel, async)
    { }
};

// For the Graphacial-User-Interface
class GUI_CommandWindowID : public CommandWindowID
{
 public:
  // Virtual functions for creating InputLineObject and CommandObjects
  // These are the defaults for the command line window
  virtual InputLineObject *New_InputLineObject (CMDWID From, std::string Cmd)
    {
      return new GUI_InputLineObject (From, Cmd);
    }

  // Constructors
 private:
  GUI_CommandWindowID () { }  // Hide default constructor to catch errors at compile time

 public:
  GUI_CommandWindowID (std::string IAM, std::string  Host, pid_t Process, int64_t Panel, bool async) :
    CommandWindowID (IAM, Host, Process, Panel, async)
    { }
};

// For the Remote-Line-Interface
class RLI_CommandWindowID : public CommandWindowID
{
 public:
  // Virtual functions for creating InputLineObject and CommandObjects
  // These are the defaults for the command line window
  virtual InputLineObject *New_InputLineObject (CMDWID From, std::string Cmd)
    {
      return new RLI_InputLineObject (From, Cmd);
    }

  // Constructors
 private:
  RLI_CommandWindowID () { }  // Hide default constructor to catch errors at compile time

 public:
  RLI_CommandWindowID (std::string IAM, std::string  Host, pid_t Process, int64_t Panel, bool async) :
    CommandWindowID (IAM, Host, Process, Panel, async)
    { }
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

void Link_Cmd_Obj_to_Input (InputLineObject *I, CommandObject *C)
{
 // Get exclusive access to the lock so that only one
 // add/remove/search of the list is done at a time.
  CommandWindowID *cw = Find_Command_Window (I->Who());
  Assert(cw != NULL);
  Assert(pthread_mutex_lock(&Window_List_Lock) == 0);

  I->Push_Cmd_Obj(C);

  if (I->What() == ILO_ERROR) {
   // Something is wrong with the original commanad.
   // Abort processing of the CommandObject.
    C->set_Status (CMD_ABORTED);
  }

 // Release the lock.
  Assert(pthread_mutex_unlock(&Window_List_Lock) == 0);
}

bool All_Command_Objects_Are_Used (InputLineObject *clip) {
  std::list<CommandObject *> cmd_object = clip->CmdObj_List();
  std::list<CommandObject *>::iterator coi;
  for (coi = cmd_object.begin(); coi != cmd_object.end(); coi++) {
    if (!((*coi)->Results_Used())) {
      return false;
    }
  }
  return true;
}

bool All_Command_Objects_Have_Executed (InputLineObject *clip) {
  std::list<CommandObject *> cmd_object = clip->CmdObj_List();
  std::list<CommandObject *>::iterator coi;
  for (coi = cmd_object.begin(); coi != cmd_object.end(); coi++) {
    Command_Status S = (*coi)->Status();
    if ((S != CMD_COMPLETE) &&
        (S != CMD_ERROR) &&
        (S != CMD_ABORTED)) {
      return false;
    }
  }
  return true;
}

void Clip_Complete (InputLineObject *clip) {
  (void)(clip->CallBackL ());
}

void Cmd_Obj_Complete (CommandObject *C) {
  InputLineObject *clip = C->Clip();
  (void)(clip->CallBackC (C));
}

int64_t Find_Command_Level (CMDWID WindowID)
{
  CommandWindowID *cwi = Find_Command_Window (WindowID);
  return (cwi != NULL) ? cwi->Input_Level() : 0;
}

FILE *Log_File (CMDWID WindowID)
{
  CommandWindowID *cwi = Find_Command_Window (WindowID);
  return (cwi != NULL) ? cwi->Log_File() : NULL;
}

// Semantic Utilities
bool Window_Is_Async (CMDWID WindowID)
{
  CommandWindowID *cwi = Find_Command_Window (WindowID);
  return (cwi != NULL) ? cwi->Async() : false;
}

// Low Level Semantic Routines

// Remove all knowledge of the given EXPID.
void Experiment_Purge_Focus (EXPID ExperimentID)
{
  std::list<CommandWindowID *>::reverse_iterator cwi;
  for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++)
  {
    if (ExperimentID == (*cwi)->Focus ()) {
      (*cwi)->Set_Focus(0);
    }
  }
}

// What is the current focus associated with a CommandWindow?
EXPID Experiment_Focus (CMDWID WindowID)
{
  if (WindowID == 0) WindowID = Last_ReadWindow;
  CommandWindowID *my_window = Find_Command_Window (WindowID);
  EXPID f = my_window ? my_window->Focus() : 0;
  if (f < 0) {
   // This window has not been given a focus, use the last experiment.
    f = Experiment_Sequence_Number;

   // Find the last one that still exist.
    while ((f > 0) && (Find_Experiment_Object (f) == NULL)) {
      f--;
    }

    if (f > 0) {
     // This is the initial value we are going to use.
      my_window->Set_Focus (f);

      if (f > 1) {
        char a[100];
        char *m = "The Focus has been initialized to Experiment ";
        bcopy (m, a, strlen(m));
        sprintf (&a[strlen(m)], "%lld\n", f);
        ReDirect_User_Stderr (a, strlen(a), (void *)WindowID);
      }
    }
  }
  return f;
}

// Set the focus for a particular CommandWindow.
EXPID Experiment_Focus (CMDWID WindowID, EXPID ExperimentID)
{
  if (WindowID == 0) WindowID = Last_ReadWindow;
  CommandWindowID *my_window = Find_Command_Window (WindowID);
  if (my_window) {
    ExperimentObject *Experiment = (ExperimentID) ? Find_Experiment_Object (ExperimentID) : NULL;
    my_window->Set_Focus(ExperimentID);
    return ExperimentID;
  }
  return 0;
}

void List_CommandWindows (ostream &mystream)
{
  if (*CommandWindowID_list.begin()) {
    (*CommandWindowID_list.begin())->Dump(mystream);
  } else {
    mystream << std::endl << "(there are no defined windows)" << std::endl;
  }
}

static bool Read_Log_File_History (CommandObject *cmd, enum Log_Entry_Type log_type,
                                   std::string fname, FILE *TFile)
{
  FILE *cmdf = fopen (fname.c_str(), "r");
  struct stat stat_buf;
  if (!cmdf) {
    return false;
  }
  if (stat (fname.c_str(), &stat_buf) != 0) {
    return false;
  }
  char *s = (char *)malloc(stat_buf.st_size+1);
  for (int i=0; i < stat_buf.st_size; ) {
    fgets (s, (stat_buf.st_size), cmdf);
    int64_t len  = strlen(s);
    if (len > 2) {
      bool dump_this_record = false;
      char *t = s;
      switch (log_type)
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
            char *S = strstr(t,"PARSING:");
            if (S != NULL) {
              S += 9;  // One more to get rid of blank separator.
              if (strlen(t) > 2) {
                dump_this_record = true;
                t = S;
                break;
              }
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
        if (TFile == NULL) {
          *(s+len-1) = *("\0");
          cmd->Result_String (t);
        } else {
          *(s+len-1) = *("\n");
          fprintf(TFile,"%s",t);
        }
      }
    }
    i+=len;
  }
  free (s);
  fclose (cmdf);
  return true;
}

// Set up an alternative log file at user request.
bool Command_Log_ON (CMDWID WindowID, std::string tofname)
{
  CommandWindowID *cmdw = Find_Command_Window (WindowID);
  return (cmdw->Set_Log_File (tofname));
}
bool Command_Log_OFF (CMDWID WindowID)
{
  CommandWindowID *cmdw = Find_Command_Window (WindowID);
  cmdw->Remove_Log_File ();
  return true;
}

// Set up an alternative record file at user request.
bool Command_Record_ON (CMDWID WindowID, std::string tofname)
{
  CommandWindowID *cmdw = Find_Command_Window (WindowID);
  return (cmdw->Set_Record_File (tofname));
}

bool Command_Record_OFF (CMDWID WindowID)
{
  CommandWindowID *cmdw = Find_Command_Window (WindowID);
  cmdw->Remove_Record_File ();
  return true;
}

// This is the start of the Command Line Processing Routines.

void Commander_Initialization () {

 // Define default err and out streams.
  class err_ostream : public ss_ostream {
   private:
    virtual void output_string (std::string s) {
      fprintf(stderr,"%s",s.c_str());
    }
    virtual void flush_ostream () {
      fflush(stderr);
    }
  };
  class out_ostream : public ss_ostream {
   private:
    virtual void output_string (std::string s) {
      fprintf(stdout,"%s",s.c_str());
    }
    virtual void flush_ostream () {
      fflush(stdout);
    }
  };
  ss_err = new err_ostream ();
  ss_out = new out_ostream ();
  ss_ttyout = NULL;

 // Set up History Buffer.
  History_Limit = DEFAULT_HISTORY_BUFFER;
  History_Count = 0;
  History.empty();
}

CMDWID Default_Window (char *my_name, char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async)
{
  // Assert(Default_WindowID == 0);
 // Create a new Window
  CommandWindowID *cwid = new CommandWindowID(std::string(my_name ? my_name : ""),
                                              std::string(my_host ? my_host : ""),
                                              my_pid, my_panel, Input_is_Async);
  Async_Inputs |= Input_is_Async;
  Default_WindowID = cwid->ID();
  return Default_WindowID;
}

CMDWID TLI_Window (char *my_name, char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async)
{
  Assert(TLI_WindowID == 0);
 // Define the output control stream for the command terminal.
  class tli_ostream : public ss_ostream {
   private:
    virtual void output_string (std::string s) {
      fprintf(ttyout,"%s",s.c_str());
    }
    virtual void flush_stream () {
      fflush(ttyout);
    }
  };
  ss_ttyout = new tli_ostream ();
  ss_ttyout->Set_Issue_Prompt (true);

 // Create a new Window
  CommandWindowID *cwid = new TLI_CommandWindowID(std::string(my_name ? my_name : ""),
                                                 std::string(my_host ? my_host : ""),
                                                 my_pid, my_panel, Input_is_Async);

 // Setup output redirection.
 // The user can redirect stdout for the tool and we
 // will still send critical output to the terminal window.
  if (isatty(fileno(stdout))) {
   // Replace uses of stdout with ttyout.
    std::list<CommandWindowID *>::reverse_iterator cwi;
    for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++)
    {
      CommandWindowID *my_window = *cwi;
      if (my_window->ss_outstream() == ss_out) {
        my_window->set_outstream ( ss_ttyout );
      }
    }
  }
  if (isatty(fileno(stderr))) {
   // Replace uses of stderr with ttyout.
    std::list<CommandWindowID *>::reverse_iterator cwi;
    for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++)
    {
      CommandWindowID *my_window = *cwi;
      if (my_window->ss_errstream() == ss_err) {
        my_window->set_errstream ( ss_ttyout );
      }
    }
  }

  Async_Inputs |= Input_is_Async;
  TLI_WindowID = cwid->ID();
  return TLI_WindowID;
}

CMDWID GUI_Window (char *my_name, char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async)
{
 // Create a new Window
  Assert(GUI_WindowID == 0);
  CommandWindowID *cwid = new GUI_CommandWindowID(std::string(my_name ? my_name : ""),
                                                 std::string(my_host ? my_host : ""),
                                                 my_pid, my_panel, Input_is_Async);
  Async_Inputs |= Input_is_Async;
  GUI_WindowID = cwid->ID();
  return GUI_WindowID;
}

CMDWID RLI_Window (char *my_name, char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async)
{
 // Create a new Window
  CommandWindowID *cwid = new RLI_CommandWindowID(std::string(my_name ? my_name : ""),
                                                 std::string(my_host ? my_host : ""),
                                                 my_pid, my_panel, Input_is_Async);
  Async_Inputs |= Input_is_Async;
  return cwid->ID();
}

CMDWID EM_Window (char *my_name, char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async)
{
 // Check for any previous initialization.
  Assert(Default_WindowID == 0);
  Assert(TLI_WindowID == 0);
  Assert(GUI_WindowID == 0);
  Assert(Embedded_WindowID == 0);

 // Create a new Window
  Embedded_WindowID = Default_Window(my_name,my_host,my_pid,my_panel,Input_is_Async);
  return Embedded_WindowID;
}

void Window_Termination (CMDWID im)
{
  if (im) {
    CommandWindowID *my_window = Find_Command_Window (im);
    if (my_window) delete my_window;

    if (im == Default_WindowID) {
      Default_WindowID = 0;
    } else if (im == TLI_WindowID) {
      TLI_WindowID = 0;
    } else if (im == GUI_WindowID) {
      GUI_WindowID = 0;
    }
  }
  return;
}

void Commander_Termination () {

 // Remove all remaining input windows.
  std::list<CommandWindowID *>::reverse_iterator cwi;
  for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); )
  {
    CommandWindowID *my_window = *cwi;
    cwi++;
    if (my_window) {
      delete my_window;
    }
  }

 // Remove default ss_ostream definitions.
 // Copy and celar them first, so that catching a signal will not cause
 // a problem if we re-enter this same routine.
  ss_ostream *old_ss_ttyout = ss_ttyout;
  ss_ostream *old_ss_out = ss_out;
  ss_ostream *old_ss_err = ss_err;
  ss_ttyout = NULL;
  ss_out = NULL;
  ss_err = NULL;
  if (old_ss_ttyout && (old_ss_ttyout != old_ss_out)) delete old_ss_ttyout;
  if (old_ss_out) delete old_ss_out;
  if (old_ss_err) delete old_ss_err;

  return;
}

void Redirect_GUI_Output_To_TLI () {
  CommandWindowID *gui_window = Find_Command_Window (GUI_WindowID);
  if (TLI_WindowID == 0) {
    pid_t my_pid = getpid();
    char HostName[MAXHOSTNAMELEN+1];
    TLI_WindowID = TLI_Window ("TLI",&HostName[0],my_pid,0,true);
  }
  CommandWindowID *tli_window = Find_Command_Window (TLI_WindowID);
  if ((gui_window != NULL) && (tli_window != NULL)) {
    if (gui_window->has_outstream()) {
      gui_window->set_outstream (tli_window->ss_outstream());
    }
    if (gui_window->has_errstream()) {
      gui_window->set_errstream (tli_window->ss_errstream());
    }
  }
}

void Redirect_Window_Output (CMDWID for_window, ss_ostream *for_out, ss_ostream *for_err) {
  CommandWindowID *my_window = Find_Command_Window (for_window);
  if (for_out != NULL) {
    my_window->set_outstream (for_out);
  }
  if (for_err != NULL) {
    my_window->set_errstream (for_err);
  }
}

ss_ostream *Predefined_ostream (std::string oname)
{
  if (oname.length() == 0) {
    return NULL;
  } if (oname == "stdout") {
    return ss_out;
  } else if (oname == "stderr") {
    return ss_err;
  } else if (oname == "/dev/tty") {
    return ss_ttyout;
  } else {
    return NULL;
  }
}

ss_ostream *Window_outstream (CMDWID for_window) {
  CommandWindowID *cw = Find_Command_Window (for_window);
  return (cw != NULL) ? cw->ss_outstream() : NULL;
}

ss_ostream *Window_errstream (CMDWID for_window) {
  CommandWindowID *cw = Find_Command_Window (for_window);
  return (cw != NULL) ? cw->ss_errstream() : NULL;
}

static void User_Info_Dump (CMDWID issuedbywindow) {
    bool Fatal_Error_Encountered = false;
    CommandWindowID *cw = Find_Command_Window (issuedbywindow);
    if ((cw == NULL) || (cw->ID() == 0)) {
      fprintf(stderr,"    ERROR: the window(%lld) this command came from is illegal\n",issuedbywindow);
      return;
    }
    if (!(cw->has_outstream())) {
      fprintf(stderr,"    ERROR: window(%lld) has no defined output stream\n",issuedbywindow);
      return;
    }
    ss_ostream *this_ss_stream = Window_outstream (issuedbywindow);
    this_ss_stream->acquireLock();
    ostream &mystream = this_ss_stream->mystream();
    mystream << "SpeedShop Status:" << std::endl;
    mystream << "    " << (Looking_for_Async_Inputs?" ":"Not") << " Waiting for Async input" << std::endl;
    if (Looking_for_Async_Inputs) {
      if (More_Input_Needed_From_Window) {
        mystream << "    " << "Processing of a complex statement requires input from W "
                 << More_Input_Needed_From_Window  << std::endl;
        CommandWindowID *lw = Find_Command_Window (More_Input_Needed_From_Window);
        if ((lw == NULL) || (!lw->Input_Available())) {
          mystream << "    ERROR: this window can not provide more input!!" << std::endl;
          Fatal_Error_Encountered = true;
        }
      }
    }
    mystream << "      ss_ostreams: stdout=" << ptr2str(ss_out);
    mystream << ", stderr=" << ptr2str (ss_err);
    mystream << ", ttyout=" << ptr2str (ss_ttyout) << std::endl;
    List_CommandWindows(mystream);
    ExperimentObject::Dump(mystream);
    this_ss_stream->releaseLock();
    Assert(!Fatal_Error_Encountered);
}

static bool Isa_SS_Command (CMDWID issuedbywindow, const char *b_ptr) {
  int64_t fc;
  for (fc = 0; fc < strlen(b_ptr); fc++) {
    if (b_ptr[fc] != *(" ")) break;
  }
  if (b_ptr[fc] == *("\?")) {
    User_Info_Dump  (issuedbywindow);
    return false;
  } else if (b_ptr[fc] == *("!")) {
    (void) system(&b_ptr[fc+1]);
    return false;
  }
  return true;
}

static InputLineObject *Append_Input_String (CMDWID issuedbywindow, InputLineObject *clip) {
  if (clip != NULL) {
    if (Isa_SS_Command(issuedbywindow,clip->Command().c_str())) {
      CommandWindowID *cw = Find_Command_Window (issuedbywindow);
      Assert (cw);

      Input_Source *inp = new Input_Source (clip);
      clip->SetStatus (ILO_QUEUED_INPUT);
      cw->Append_Input_Source (inp);
      return clip;
    } else {
      delete clip;
    }
  }
  return NULL;
}

InputLineObject *Append_Input_String (CMDWID issuedbywindow, char *b_ptr,
                                      void *LocalCmdId,
                                      void (*CallBackLine) (InputLineObject *b),
                                      void (*CallBackCmd) (CommandObject *b)) {
  InputLineObject *clip = NULL;;
  if (Isa_SS_Command(issuedbywindow,b_ptr)) {
    CommandWindowID *cw = Find_Command_Window (issuedbywindow);
    Assert (cw);
    clip = cw->New_InputLineObject(issuedbywindow, std::string(b_ptr));
    Input_Source *inp = new Input_Source (clip);
    clip->Set_CallBackId (LocalCmdId);
    clip->Set_CallBackL (CallBackLine);
    clip->Set_CallBackC (CallBackCmd);
    clip->SetStatus (ILO_QUEUED_INPUT);
    cw->Append_Input_Source (inp);
  }
  return clip;
}

static bool Append_Input_Buffer (CMDWID issuedbywindow, int64_t b_size, char *b_ptr) {
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);

// DEBUG: hacks to let the gui pass information in without initializing a window.
if (!cw) {
  issuedbywindow = Command_Window_ID;  // default to the last allocated window
  Append_Input_String (issuedbywindow, b_ptr);
} else {

  Assert (cw);
  Input_Source *inp = new Input_Source (b_size, b_ptr);
  cw->Append_Input_Source (inp);
}
  return true;
}

bool Append_Input_File (CMDWID issuedbywindow, std::string fromfname,
                                      void (*CallBackLine) (InputLineObject *b),
                                      void (*CallBackCmd) (CommandObject *b)) {
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);
  Assert (cw);
  Input_Source *inp = new Input_Source (fromfname);
  if (inp->InFileError()) {
    return FALSE;
  }
  inp->Set_CallBackL (CallBackLine);
  inp->Set_CallBackC (CallBackCmd);
  cw->Append_Input_Source (inp);
  return true;
}

static bool Push_Input_Buffer (CMDWID issuedbywindow, int64_t b_size, char *b_ptr) {
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);
  Assert (cw);
  Input_Source *inp = new Input_Source (b_size, b_ptr);
  cw->Push_Input_Source (inp);
  return true;
}

bool Push_Input_File (CMDWID issuedbywindow, std::string fromfname,
                                      void (*CallBackLine) (InputLineObject *b),
                                      void (*CallBackCmd) (CommandObject *b)) {
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);
  Assert (cw);
  Input_Source *inp = new Input_Source (fromfname);
  if (inp->InFileError()) {
    return false;
  }
  inp->Set_CallBackL (CallBackLine);
  inp->Set_CallBackC (CallBackCmd);
  cw->Push_Input_Source (inp);
  return true;
}

void Default_TLI_Line_Output (InputLineObject *clip) {
 // Make sure all the output, associated with every CommandObject, has been printed.
  Assert(clip != NULL);
  if (!(clip->Results_Used())) {
    std::list<CommandObject *> cmd_list = clip->CmdObj_List();
    std::list<CommandObject *>::iterator cmi;
    for (cmi = cmd_list.begin(); cmi != cmd_list.end(); cmi++) {
      if (!(*cmi)->Results_Used ()) {
        Default_TLI_Command_Output (*cmi);
      }
    }
    clip->Set_Results_Used();
  }
}

void ReDirect_User_Stdout (const char *S, const int &len, void *tag) {
  CMDWID to_window = (CMDWID)tag;
  CommandWindowID *cw = (to_window) ? Find_Command_Window (to_window) : NULL;

  if ((cw != NULL) &&
      cw->has_outstream()) {
    ss_ostream *this_ss_stream = cw->ss_outstream();
    this_ss_stream->acquireLock();
    ostream &mystream = this_ss_stream->mystream();
    mystream.write( S, len);
    this_ss_stream->releaseLock();
  } else {
    FILE *outfile = (cw) ? cw->output() : NULL;
    if (outfile == NULL) {
      outfile = stdout;
      if ((ttyout != NULL)  &&
          isatty(fileno(outfile))) {
        outfile = ttyout;
      }
    }
    for (int i = 0; i < len; i++) {
      fputc ((S[i]), outfile);
    }
    fflush(outfile);
  }
}

void ReDirect_User_Stderr (const char *S, const int &len, void *tag) {
  CMDWID to_window = (CMDWID)tag;
  CommandWindowID *cw = (to_window) ? Find_Command_Window (to_window) : NULL;

  if ((cw != NULL) &&
      cw->has_errstream()) {
    ss_ostream *this_ss_stream = cw->ss_errstream();
    this_ss_stream->acquireLock();
    ostream &mystream = this_ss_stream->mystream();
    mystream.write( S, len);
    this_ss_stream->releaseLock();
  } else {
    FILE *errfile = (cw) ? cw->output() : NULL;
    if (errfile == NULL) {
      errfile = stderr;
      if ((ttyout != NULL)  &&
          isatty(fileno(errfile))) {
        errfile = ttyout;
      }
    }
    for (int i = 0; i < len; i++) {
      fputc (S[i], errfile);
    }
    fflush(errfile);
  }
}

void Default_TLI_Command_Output (CommandObject *C) {
  if (!(C->Results_Used()) &&
      !(C->Needed_By_Python())) {
    std::list<CommandResult *> cmd_result = C->Result_List();
    if ((cmd_result.begin() != cmd_result.end()) &&
        (C->Status() != CMD_ABORTED)) {
      InputLineObject *clip = C->Clip ();
      CMDWID w = (clip) ? clip->Who() : 0;
      CommandWindowID *cw = (w) ? Find_Command_Window (w) : NULL;

     // Print the ResultdObject list
      if (cw->has_outstream()) {
        ss_ostream *this_ss_stream = ((C->Status() == CMD_ERROR) && cw->has_errstream())
                                                   ? cw->ss_errstream() : cw->ss_outstream();
        this_ss_stream->acquireLock();
        if (C->Print_Results (this_ss_stream->mystream(), "\n", "\n") &&
            this_ss_stream->Issue_Prompt()) {
         // Re-issue the prompt
          this_ss_stream->mystream() << Current_OpenSpeedShop_Prompt;
        }
        this_ss_stream->releaseLock();
      } else {
        FILE *outfile = (cw) ? cw->output() : NULL; // (C->Status() == CMD_ERROR) ? stderr : stdout;
        if (outfile == NULL) {
          outfile = (C->Status() == CMD_ERROR) ? stderr : stdout;
          if ((ttyout != NULL)  &&
              isatty(fileno(outfile))) {
            outfile = ttyout;
          }
        }

        if (C->Print_Results (outfile, "\n", "\n") &&
              cw->Async()) {
            // isatty(fileno(outfile))) {
         // Re-issue the prompt
          SS_Issue_Prompt (outfile);
        }
      }
    }
    C->set_Results_Used (); // Everything is where it belongs.
  }
}

// Catch keyboard interrupt signals from TLI window.
#include "sys/signal.h"

void User_Interrupt (CMDWID issuedbywindow) {
  CommandWindowID *cw = Find_Command_Window (issuedbywindow);
  if (cw != NULL) {
   // Get rid of all queued commands from this window.
    cw->Purge_All_Input ();

   // Stop commands that are being processed.
    cw->Abort_Executing_Input_Lines ();
  }
}

static void
catch_TLI_signal (int sig, int error_num)
{
  if (sig == SIGINT) {
    User_Interrupt (TLI_WindowID);

    ss_ttyout->acquireLock();
    ss_ttyout->mystream() << std::endl << Current_OpenSpeedShop_Prompt;
    ss_ttyout->releaseLock();
  }

 // This isn't graceful, but it gets the job done.
  pthread_exit(0);
}
inline static void
setup_signal_handler (int s)
{
    if (signal (s, SIG_IGN) != SIG_IGN)
        signal (s,  reinterpret_cast <void (*)(int)> (catch_TLI_signal));
}

// This routine continuously reads from /dev/tty and appends the string to an input window.
// It is intended that this routine execute in it's own thread.
void SS_Direct_stdin_Input (void * attachtowindow) {
  Assert ((CMDWID)attachtowindow != 0);
  CommandWindowID *cw = Find_Command_Window ((CMDWID)attachtowindow);
  Assert (cw);
  int64_t Buffer_Size= DEFAULT_INPUT_BUFFER_SIZE;
  char Buffer[Buffer_Size];
  Buffer[Buffer_Size-1] = *"\0";
  ttyin = fopen ( "/dev/tty", "r" );  // Read directly from the xterm window
  ttyout = fopen ( "/dev/tty", "w" );  // Write prompt directly to the xterm window

 // Set up to catch keyboard control signals
  setup_signal_handler (SIGINT); // CNTRL-C
  setup_signal_handler (SIGUSR1);
  setup_signal_handler (SIGRTMIN+0);
  setup_signal_handler (SIGRTMIN+1);
  setup_signal_handler (SIGRTMIN+32);
  setup_signal_handler (SIGRTMAX);

 // Allow us to be terminated from the main thread.
  int previous_value;
  pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, &previous_value);
  pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, &previous_value);

 // If this is the only window, send a welcome message to the user.
  if ((Default_WindowID == 0) &&
      (GUI_WindowID == 0) &&
      (Embedded_WindowID == 0)) {
    char *Welcome_Message = "Welcome to Open|SpeedShop, version 0.1.\n"
                            "Type 'help' for more information.\n";
    ss_ttyout->acquireLock();
    ss_ttyout->mystream() << Welcome_Message;
    ss_ttyout->releaseLock();
  }

 // Infinite loop to read user input.
  for(;;) {
    // usleep (10000); /* DEBUG - give testing code time to react before splashing screen with prompt */
    ss_ttyout->acquireLock();
    ss_ttyout->mystream() << Current_OpenSpeedShop_Prompt;
    ss_ttyout->releaseLock();
    Buffer[0] == *("\0");
    pthread_testcancel();
    char *read_result = fgets (&Buffer[0], Buffer_Size, ttyin);
    pthread_testcancel();
    if (Buffer[Buffer_Size-1] != (char)0) {
      ss_ttyout->acquireLock();
      ss_ttyout->mystream() << "ERROR: Input line is too long for buffer." << std::endl;
      ss_ttyout->releaseLock();
      Buffer[Buffer_Size-1] = *"\0";
      continue; // Don't terminate the thread - allow user to retry.
    }
    if (read_result == NULL) {
     // This indicates an EOF or error.
     // This could be a CNTRL-D signal.
      break; // terminate the thread
    }
    if (Buffer[0] == *("\0")) {
     // This indicates an EOF.
      break; // terminate the thread
    }
    if (cw->ID() == 0) {
     // This indicates that someone freed the input window
      break; // terminate the thread
    }
    (void) Append_Input_String ((CMDWID)attachtowindow, &Buffer[0], 
                                 NULL, &Default_TLI_Line_Output, &Default_TLI_Command_Output);
  }

 // We have exited the loop and will no longer read input.
  TLI_WindowID = 0;
  if (GUI_WindowID != 0) {
   // Stop this thread but keep the GUI runnning.
   // The user can stop OpenSS through the GUI.
    /* pthread_exit (0); */
  } else if (Default_WindowID == 0) {
   // This is the only window open, terminate Openss with a normal exit
   // so that all previously read commands finish first.
    (void) Append_Input_String ((CMDWID)attachtowindow, "exit",
                                 NULL, &Default_TLI_Line_Output, &Default_TLI_Command_Output);
  } else {
   // Add an Exit command to the end of the command line input stream
   // so that the command line arguments and input files are processed first.
    (void) Append_Input_String (Default_WindowID, "exit",
                                NULL, &Default_TLI_Line_Output, &Default_TLI_Command_Output);
  }
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

    std::list<CommandWindowID *>::reverse_iterator cwi;
    for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++)
    {
      if (selectwindow == (*cwi)->ID ()) {
        std::list<CommandWindowID *>::reverse_iterator next_cwi = ++cwi;
        if (next_cwi == CommandWindowID_list.rend()) {
          next_cwi = CommandWindowID_list.rbegin();
        }
        selectwindow = (*next_cwi)->ID();
        goto window_found;
      }
    }

   // A fall through the loop indicates that we didn't find the Last_ReadWindow.
   // Do error recovery by choosing the first one on the list.
    cwi = CommandWindowID_list.rbegin();
    if ((*cwi)) selectwindow = (*cwi)->ID();
    //selectwindow = (*CommandWindowID_list.rbegin())->ID();

  }

window_found:

 // Release the lock.
  Assert(pthread_mutex_unlock(&Window_List_Lock) == 0);

  Assert (selectwindow);
  Last_ReadWindow = selectwindow;
  return selectwindow;
}

static void There_Must_Be_More_Input (CommandWindowID *cw) {
  if ((cw == NULL) || (!cw->Input_Available())) {
    FILE *ef = ((ttyout != NULL)  && isatty(fileno(stderr))) ? ttyout : stderr;
    fprintf(ef,"ERROR: The input source that started a complex statement"
                   " failed to complete the expression.\n");
    fflush (ef);
    Assert (cw->Input_Available());
  }
}

InputLineObject *SpeedShop_ReadLine (int is_more)
{
  char *save_prompt = Current_OpenSpeedShop_Prompt;
  InputLineObject *clip;

  CMDWID readfromwindow = select_input_window(is_more);
  CMDWID firstreadwindow = readfromwindow;
  bool I_HAVE_ASYNC_INPUT_LOCK = false;
  
  if (is_more) {
    Current_OpenSpeedShop_Prompt = Alternate_Current_OpenSpeedShop_Prompt;
  }

read_another_window:

  CommandWindowID *cw = Find_Command_Window (readfromwindow);
  Assert (cw);
  if (is_more == 0) {
   // What is waiting for completion?
    cw->Remove_Completed_Input_Lines ();
  }

  do {
    clip = cw->Read_Command ();
    if (clip == NULL) {
     // The read failed.  Why?  Can we find something else to read?

     // It might be possible to read from a different window.
     // Try all of them so we can pick up commands that are waiting.
      readfromwindow = select_input_window(is_more);
      if (readfromwindow != firstreadwindow) {
       // There is another window to read from.
        goto read_another_window;
      }

     // After checking all windows for waiting input,
     // we might look for input from the gui or a terminal.
      if (Async_Inputs) {
        if (I_HAVE_ASYNC_INPUT_LOCK) {
         // Force a wait until data is ready.
          More_Input_Needed_From_Window = (is_more) ? readfromwindow : 0;
          Assert(pthread_cond_wait(&Async_Input_Available,&Async_Input_Lock) == 0);
          I_HAVE_ASYNC_INPUT_LOCK = false;
          Assert(pthread_mutex_unlock(&Async_Input_Lock) == 0);
        } else {
  
          if (is_more) {
            There_Must_Be_More_Input (cw);
          }

         // Get the lock and try again to read from each of the input windows
         // because something might have arrived after our first check of the
         // window.
          Assert(pthread_mutex_lock(&Async_Input_Lock) == 0);
          Looking_for_Async_Inputs = true;
          I_HAVE_ASYNC_INPUT_LOCK = true;
        }
        goto read_another_window;
      }

     // The end of all window inputs has been reached.
      if (is_more) {
       // We MUST read from this window!
       // This is an error situation.  How can we recover?
        There_Must_Be_More_Input(NULL);
      }

     // Return an empty line to indicate an EOF.
      break;
    }
    const char *s = clip->Command().c_str();
    (void) strlen(s);
    if (!Isa_SS_Command(readfromwindow, s)) {
      clip = NULL;
      continue;
    }
  } while (clip == NULL);

  if (I_HAVE_ASYNC_INPUT_LOCK) {
    I_HAVE_ASYNC_INPUT_LOCK = false;
    Looking_for_Async_Inputs = false;
    Assert(pthread_mutex_unlock(&Async_Input_Lock) == 0);
  }

  Current_OpenSpeedShop_Prompt = save_prompt;
  if (clip == NULL) {
    return NULL;
  }

// Initialize fields in the Clip:
 // Assign a sequence number to the command.
  clip->SetSeq (++Command_Sequence_Number);
 // Reflect internal state in the command and log it to the log file.
  clip->SetStatus (ILO_IN_PARSER);
 // Mark nested commands
  if (is_more)   clip-> Set_Complex_Exp ();

 // Add command to the history file.
  History.push_back(clip->Command());
  History_Count++;
  if (History_Count > History_Limit) {
    (void) History.pop_front();
    History_Count--;
  }

 // Track it until completion
  cw->TrackCmd(clip);

 // Log the command to any user defined record file.
  cw->Record(clip);

  return clip;
}
