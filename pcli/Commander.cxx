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
static FILE *ttyin = NULL;  // Read directly from this xterm window.
static FILE *ttyout = NULL; // Write directly to this xterm window.

// Forward definitions of local functions
void Default_TLI_Command_Output (CommandObject *C);
bool All_Command_Objects_Are_Used (InputLineObject *clip);

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
  bool Trace_To_A_Predefined_File;
  std::string Trace_Name;
  FILE *Trace_F;

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
        fprintf(stderr,"ERROR: Unable to open alternate command file %s\n",my_name.c_str());
      }
    } else {
      Predefined = true;
    }
    Next_Line_At = 0;
    Last_Valid_Data = 0;
    Buffer_Size = DEFAULT_INPUT_BUFFER_SIZE;
    Buffer = (char *)malloc(Buffer_Size);
    Input_Object = NULL;
    Trace_To_A_Predefined_File = false;
    Trace_Name = std::string("");
    Trace_F = NULL;
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
    Trace_To_A_Predefined_File = false;
    Trace_Name = std::string("");
    Trace_F = NULL;
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
    Trace_To_A_Predefined_File = false;
    Trace_Name = std::string("");
    Trace_F = NULL;
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
   /* Close trace files. */
    if (Trace_F && !Trace_To_A_Predefined_File) {
      fclose (Trace_F);
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
        fprintf(stderr,"ERROR: Input line from %s is too long for buffer.\n",Name.c_str());
        return NULL;
      }
      Last_Valid_Data = strlen(&Buffer[0]);
    }
    char *next_line = &Buffer[Next_Line_At];
    int64_t line_len = strlen(next_line);
    int i;
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
          int nline = strlen (&(Buffer[Next_Line_At]));
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
      if (Trace_F) {
        fprintf(TFile,"     trace to: %s\n",Trace_Name.c_str());
      }
  }
};

// CommandWindowID
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
  bool Trace_File_Is_A_Temporary_File;
  std::string Record_File_Name;
  FILE *Record_F;
  bool Record_File_Is_A_Temporary_File;
  bool Input_Is_Async;
  Input_Source *Input;
  pthread_mutex_t Input_List_Lock;
  EXPID FocusedExp;

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
   // Disallow default constructor
    fprintf(stderr,"ERROR: Illegal use of default CommandWindowID Constructor\n");
    Assert (0);
  }

 public:
  CommandWindowID ( std::string IAM, std::string  Host, pid_t Process, int64_t Panel, bool async)
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
      Trace_File_Is_A_Temporary_File = false;
      Record_File_Name = "";
      Record_F = NULL;
      Record_File_Is_A_Temporary_File = false;
      Input = NULL;
      Input_Is_Async = async;
      if (Input_Is_Async) {
        Assert(pthread_mutex_init(&Input_List_Lock, NULL) == 0); // dynamic initialization
      }
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
      snprintf(base, 20, "sstr%lld.XXXXXX",id);
      Trace_File_Name = std::string(tempnam ("./", &base[0] )) + ".openss";
      Trace_F  = fopen (Trace_File_Name.c_str(), "w");
      Trace_File_Is_A_Temporary_File = true;
      Record_File_Name = "";
      Record_F = NULL;
      Record_File_Is_A_Temporary_File = false;
    }
  ~CommandWindowID()
    {
     // Clear the identification field in case someone trys to reference
     // this entry again.
      id = 0;
     // Remove the trace files
      if ((Trace_F != NULL) &&
          (predefined_filename (Trace_File_Name) == NULL)) {
        fclose (Trace_F);
        if (Trace_File_Is_A_Temporary_File) {
          int err = remove (Trace_File_Name.c_str());
        }
      }
     // Remove the record files
      if ((Record_F != NULL) &&
          (predefined_filename (Record_File_Name) == NULL)) {
        fclose (Record_F);
        if (Record_File_Is_A_Temporary_File) {
          int err = remove (Record_File_Name.c_str());
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
  void TrackCmd (InputLineObject *Clip) { Complete_Cmds.push_back (Clip); }
  void Remove_Completed_Input_Lines () {
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

   // For internal debugging, the trace file is used to record activity of the command
    if (Trace_F != NULL) {
      clip->Set_Trace (Trace_F);
    }

    return clip;
  }

  bool Input_Available () {
    return  ((Input == NULL) ? Input_Is_Async : true);
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

 // The "Log" command will causes us to echo state changes that
 // are associate with a particular input stream to a user defined file.
  bool   Set_Log_File ( std::string tofname ) {
    FILE *tof = predefined_filename (tofname);
    if ((Trace_F != NULL) &&
        (predefined_filename (Trace_File_Name) == NULL) &&
        (tof != NULL)) {
     // Copy the old file to the new file.
      fclose (Trace_F);
      if (tof == NULL) {
        int len1 = Trace_File_Name.length();
        int len2 = tofname.length();
        char *scmd = (char *)malloc(6 + len1 + len2);
        sprintf(scmd,"mv %s %s\n\0",Trace_File_Name.c_str(),tofname.c_str());
        int ret = system(scmd);
        free (scmd);
        if (ret != 0) return false;
        Trace_File_Is_A_Temporary_File = false;
      }
    }
    if (Trace_File_Is_A_Temporary_File) {
      (void) remove (Trace_File_Name.c_str());
    }
    if (tof == NULL) {
      tof = fopen (tofname.c_str(), "a");
    }
    Trace_File_Name = tofname;
    Trace_F  = tof;
    Trace_File_Is_A_Temporary_File = false;
    return (Trace_F != NULL);
  }
  void   Remove_Log_File () {
    if (Trace_F != NULL) {
      fclose (Trace_F);
    }
    Trace_File_Name = std::string("");
    Trace_F = NULL;
  }

 // The "Record" command will causes us to echo statements that
 // come from a particular input stream to a user defined file.
  void   Set_Record_File ( std::string tofname ) {
    if (Input) {
      Input->Set_Trace ( tofname );
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
    }
  }
  void   Remove_Record_File () {
    if (Input) {
      Input->Remove_Trace ();
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
      rf = Input->Trace_File();
      is_predefined = Input->Trace_File_Is_Predefined();
    } else {
      rf = Record_F;
      is_predefined = Record_File_Is_A_Temporary_File;
    }
    if (rf) {
      fprintf(rf,"%s", clip->Command().c_str());  // Commands have a "\n" at the end.
      if (is_predefined) {
       // Trace_File is something like stdout, so push the message out to user.
        fflush (rf);
      }
    }
  }

  // For error reporting
  void Print_Location(FILE *TFile) {
    fprintf(TFile,"%s %lld",Host_ID.c_str(),(int64_t)Process_ID);
  }
  // Debug aids
  void Print(FILE *TFile) {
    fprintf(TFile,
       "W %lld: %s %s IAM:%s %s %lld %lld, focus at %lld, log->%s\n",
        id,Input_Is_Async?"async":" sync",remote?"remote":"local",
        I_Call_Myself.c_str(),Host_ID.c_str(),(int64_t)Process_ID,Panel_ID,
        Focus(),Trace_File_Name.c_str());
    if (Input) {
      fprintf(TFile,"  Active Input Source Stack:\n");
      for (Input_Source *inp = Input; inp != NULL; inp = inp->Next()) {
        inp->Dump(TFile );
      }
    }
   // Print the list of completed commands
    std::list<InputLineObject *> cmd_object = Complete_Cmds;
    std::list<InputLineObject *>::iterator cmi;
    for (cmi = cmd_object.begin(); cmi != cmd_object.end(); cmi++) {
      (*cmi)->Print (TFile);
    }
  }
  void Trace (InputLineObject *clip) {
    if (Trace_File()) {
      FILE *TFile = Trace_File();
      clip->Print (TFile);
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
  I->Push_Cmd_Obj(C);
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
  return (my_window && my_window->Focus()) ? my_window->Focus() : 0;
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

void List_CommandWindows ( FILE *TFile )
{
  if (*CommandWindowID_list.begin()) {
    (*CommandWindowID_list.begin())->Dump(TFile);
  } else {
    fprintf(TFile,"\n(there are no defined windows)\n");
  }
}

static void Trace_File_History (CommandObject *cmd, enum Trace_Entry_Type
                                trace_type, std::string fname, FILE *TFile)
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
        if (cmd != NULL) {
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
}

// Read the trace files and echo selected entries to another file.
bool Command_Trace (CommandObject *cmd, enum Trace_Entry_Type trace_type,
                    CMDWID cmdwinid, std::string tofname)
{
  FILE *tof = predefined_filename( tofname );
  bool tof_predefined = (tof != NULL);
  if (tof == NULL) {
    if (tofname.length() != 0) {
      tof = fopen (tofname.c_str(), "a");
    }
  }
  if (tof == NULL) {
    tof = stderr;
    tof_predefined = true;
  }

  std::list<CommandWindowID *>::reverse_iterator cwi;
  for (cwi = CommandWindowID_list.rbegin(); cwi != CommandWindowID_list.rend(); cwi++)
  {
    if ((cmdwinid == 0) ||
        (cmdwinid == (*cwi)->ID ())) {
      std::string cmwtrn = (*cwi)->Trace_Name();
      FILE *cmwtrf = (*cwi)->Trace_File();
      if (!cmwtrf) {
        char *S;
        sprintf(S,"ERROR: missing history file %s\n",cmwtrn.c_str());
        if (cmd != NULL) {
          cmd->Result_String (S);
        } else {
          fprintf(tof,"%s\n",S);
        }
        return false;
      }
      if (fflush (cmwtrf)) {
        char *S;
        sprintf(S,"ERROR: can not flush trace file %s\n",cmwtrn.c_str());
        if (cmd != NULL) {
          cmd->Result_String (S);
        } else {
          fprintf(tof,"%s\n",S);
        }
        return false;
      }
      Trace_File_History (cmd, trace_type, cmwtrn, tof);
    }
  }
  if (tof) {
    fflush(tof);
    if (!tof_predefined) {
      fclose (tof);
    }
  }
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

// Set up an alternative trace file at user request.
bool Command_Trace_ON (CMDWID WindowID, std::string tofname)
{
  CommandWindowID *cmdw = Find_Command_Window (WindowID);
  cmdw->Set_Record_File (tofname);
  return true;
}
bool Command_Trace_OFF (CMDWID WindowID)
{
  CommandWindowID *cmdw = Find_Command_Window (WindowID);
  cmdw->Remove_Record_File ();
  return true;
}

// This is the start of the Command Line Processing Routines.
// Only one thread can be executing one of these rotuines at a time,
// so the must be protected with the use of Window_List_Lock.

CMDWID Commander_Initialization (char *my_name, char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async)
{
 // Create a new Window
  CommandWindowID *cwid = new CommandWindowID(std::string(my_name ? my_name : ""),
                                              std::string(my_host ? my_host : ""),
                                              my_pid, my_panel, Input_is_Async);
  Async_Inputs |= Input_is_Async;
  return cwid->ID();
}

CMDWID Default_Window (char *my_name, char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async)
{
 // Create a new Window
  CommandWindowID *cwid = new CommandWindowID(std::string(my_name ? my_name : ""),
                                              std::string(my_host ? my_host : ""),
                                              my_pid, my_panel, Input_is_Async);
  Async_Inputs |= Input_is_Async;
  return cwid->ID();
}

CMDWID TLI_Window (char *my_name, char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async)
{
 // Create a new Window
  CommandWindowID *cwid = new TLI_CommandWindowID(std::string(my_name ? my_name : ""),
                                                 std::string(my_host ? my_host : ""),
                                                 my_pid, my_panel, Input_is_Async);
  Async_Inputs |= Input_is_Async;
  return cwid->ID();
}

CMDWID GUI_Window (char *my_name, char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async)
{
 // Create a new Window
  CommandWindowID *cwid = new GUI_CommandWindowID(std::string(my_name ? my_name : ""),
                                                 std::string(my_host ? my_host : ""),
                                                 my_pid, my_panel, Input_is_Async);
  Async_Inputs |= Input_is_Async;
  return cwid->ID();
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

void Commander_Termination (CMDWID im)
{
  if (im) {
    CommandWindowID *my_window = Find_Command_Window (im);
    if (my_window) delete my_window;
  }
  return;
}


static bool Isa_SS_Command (CMDWID issuedbywindow, const char *b_ptr) {
  int fc;
  for (fc = 0; fc < strlen(b_ptr); fc++) {
    if (b_ptr[fc] != *(" ")) break;
  }
  if (b_ptr[fc] == *("\?")) {
    bool Fatal_Error_Encountered = false;
    fprintf(stdout,"SpeedShop Status:\n");
    CommandWindowID *cw = Find_Command_Window (issuedbywindow);
    if ((cw == NULL) || (cw->ID() == 0)) {
      fprintf(stderr,"    ERROR: the window(%lld) this command came from is illegal\n",issuedbywindow);
      Fatal_Error_Encountered = true;
    }
    fprintf(stdout,"  %s Waiting for Async input\n",Looking_for_Async_Inputs?" ":"Not");
    if (Looking_for_Async_Inputs) {
      if (More_Input_Needed_From_Window) {
        fprintf(stdout,"    Processing of a complex statement requires input from W %d.\n",
                More_Input_Needed_From_Window);
        CommandWindowID *lw = Find_Command_Window (More_Input_Needed_From_Window);
        if ((lw == NULL) || (!lw->Input_Available())) {
          fprintf(stdout,"    ERROR: this window can not provide more input!!\n");
          Fatal_Error_Encountered = true;
        }
      }
    }
    List_CommandWindows(stdout);
    ExperimentObject *a; a->Dump(stdout);
    Assert(!Fatal_Error_Encountered);
    return false;
  } else if (b_ptr[fc] == *("!")) {
    int ret = system(&b_ptr[fc+1]);
    return false;
  }
  return true;
}

static InputLineObject *Append_Input_String (CMDWID issuedbywindow, InputLineObject *clip) {
  if (clip != NULL) {
    if (Isa_SS_Command(issuedbywindow,clip->Command().c_str())) {
      CommandWindowID *cw = Find_Command_Window (issuedbywindow);
      Assert (cw);

      clip->Set_Trace (cw->Trace_File());
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
    clip->Set_Trace (cw->Trace_File());
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
  inp->Set_CallBackL (CallBackLine);
  inp->Set_CallBackC (CallBackCmd);
  cw->Push_Input_Source (inp);
  return true;
}

void Default_TLI_Line_Output (InputLineObject *clip) {
  if (!(clip->Results_Used())) {
   // The individual CommandObject's have been marked as "Results_Used"
   // by the time this function is caled.  Since there is no additional
   // output to produce, this line is also complete.
    clip->Set_Results_Used();
  }
}

void Default_TLI_Command_Output (CommandObject *C) {
  if (!(C->Results_Used())) {
    FILE *outfile = (ttyout != NULL) ? ttyout : stdout;

   // Print the ResultdObject list
    C->Print_Results (outfile, "\n", "");
    C->set_Results_Used (); // Everything is where it belongs.
    if (ttyout != NULL) {
     // Re-issue the prompt
      fprintf(ttyout,"\n");
      SS_Issue_Prompt (ttyout);
    }
  }
}

// This routine continuously reads from /dev/tty and appends the string to an input window.
// It is intended that this routine execute in it's own thread.
void SS_Direct_stdin_Input (void * attachtowindow) {
  Assert ((CMDWID)attachtowindow != 0);
  CommandWindowID *cw = Find_Command_Window ((CMDWID)attachtowindow);
  Assert (cw);
  int Buffer_Size= DEFAULT_INPUT_BUFFER_SIZE;
  char Buffer[Buffer_Size];
  Buffer[Buffer_Size-1] = *"\0";
  ttyin = fopen ( "/dev/tty", "r" );  // Read directly from the xterm window
  ttyout = fopen ( "/dev/tty", "w" );  // Write prompt directly to the xterm window
  for(;;) {
    // usleep (10000); /* DEBUG - give testing code time to react before splashing screen with prompt */
    SS_Issue_Prompt (ttyout);
    Buffer[0] == *("\0");
    char *read_result = fgets (&Buffer[0], Buffer_Size, ttyin);
    if (Buffer[Buffer_Size-1] != (char)0) {
      fprintf(stderr,"ERROR: Input line from stdin is too long for buffer.\n");
      exit (0); // terminate the thread
    }
    if (read_result == NULL) {
     // This indicates an EOF or error
      exit (0); // terminate the thread
    }
    if (Buffer[0] == *("\0")) {
     // This indicates an EOF
      exit (0); // terminate the thread
    }
    if (cw->ID() == 0) {
     // This indicates that someone freed the input window
      exit (0); // terminate the thread
    }
    (void) Append_Input_String ((CMDWID)attachtowindow, &Buffer[0], 
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
    fprintf(stderr,"ERROR: The input source that started a complex statement"
                   " failed to complete the expression.\n");
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
    int len  = strlen(s);
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
 // Reflect internal state in the command and log it to the trace file
  clip->SetStatus (ILO_IN_PARSER);
 // Mark nested commands
  if (is_more)   clip-> Set_Complex_Exp ();

 // Track it until completion
  cw->TrackCmd(clip);

 // Log the command to any user defined trace file.
  cw->Record(clip);

  return clip;
}
