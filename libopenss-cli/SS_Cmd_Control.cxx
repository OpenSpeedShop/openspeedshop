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
#include <fstream.h>

static void Cmd_Execute (CommandObject *cmd) {

  string *redirect_name = cmd->P_Result()->getRedirectTarget();
  ofstream *redirect_streamP = NULL;
  ios_base::openmode open_mode = ios::out;
  if (redirect_name->length() == 0) {
    redirect_name = cmd->P_Result()->getAppendTarget();
    open_mode = ios::app;
  }
  if ((redirect_name->length() > 0) &&
      (Predefined_ostream (*redirect_name) == NULL)) {
   // If they give us a real file name, direct command output there.
    redirect_streamP = new ofstream (redirect_name->c_str(), open_mode);
    if (redirect_streamP == NULL) {
#if 1
  std::string s("Could not open file " + *redirect_name + ".");
  Mark_Cmd_With_Soft_Error(cmd,s);
#else
      cmd->Result_String (std::string("Could not open file ") + *redirect_name);
      cmd->set_Status(CMD_ERROR);
#endif
      Cmd_Obj_Complete (cmd);
      return;
    }
  }

// Bracket execution of commands with a "catch-all" exception handler.
// This is intended to catch failures in recovery code.
  bool cmd_successful = false;
try {

  InputLineObject *clip = cmd->Clip();

 // Move this command to the EXECUTING state.
  cmd->set_Status (CMD_EXECUTING);

  switch (clip->What()) {
  case ILO_IN_PARSER:
   // First tiem we've see this command.
    clip->SetStatus (ILO_EXECUTING);
    break;
  case ILO_EXECUTING:
   // Great!  Keep going.
    break;
  case ILO_COMPLETE:
   // There may be new CommandObjects that are not complete.
    break;
  case ILO_ERROR:
   // Unwind processing because of a previous error.
    cmd->set_Status (CMD_ABORTED);
    Cmd_Obj_Complete (cmd);
    return;
  }

 // And now go and execute it!
  switch (cmd->Type()) {

// Experiment Building BLock Commands
  case CMD_EXP_CLOSE:
    cmd_successful = SS_expClose(cmd);
    break;
  case CMD_EXP_ATTACH:
    cmd_successful = SS_expAttach (cmd);
    break;
  case CMD_EXP_CREATE:
    cmd_successful = SS_expCreate (cmd);
    break;
  case CMD_EXP_DETACH:
    cmd_successful = SS_expDetach(cmd);
    break;
  case CMD_EXP_DISABLE:
    cmd_successful = SS_expDisable(cmd);
    break;
  case CMD_EXP_ENABLE:
    cmd_successful = SS_expEnable(cmd);
    break;
  case CMD_EXP_FOCUS:
    cmd_successful = SS_expFocus (cmd);
    break;
  case CMD_EXP_PAUSE:
    cmd_successful = SS_expPause (cmd);
    break;
  case CMD_EXP_GO:
    cmd_successful = SS_expGo (cmd);
    break;
  case CMD_EXP_RESTORE:
    cmd_successful = SS_expRestore (cmd);
    break;
  case CMD_EXP_SAVE:
    cmd_successful = SS_expSave (cmd);
    break;
  case CMD_EXP_SETPARAM:
    cmd_successful = SS_expSetParam (cmd);
    break;

// Information Commands
  case CMD_EXP_STATUS:
    cmd_successful = SS_expStatus (cmd);
    break;
  case CMD_EXP_VIEW:
    cmd_successful = SS_expView (cmd);
    break;

// Low Level Information Commands
  case CMD_LIST_GENERIC:
    cmd_successful = SS_ListGeneric (cmd);
    break;
  case CMD_LIST_BREAKS:
    cmd_successful = SS_ListBreaks (cmd);
    break;
  case CMD_LIST_EXP:
    cmd_successful = SS_ListExp (cmd);
    break;
  case CMD_LIST_HOSTS:
    cmd_successful = SS_ListHosts (cmd);
    break;
  case CMD_LIST_OBJ:
    cmd_successful = SS_ListObj (cmd);
    break;
  case CMD_LIST_PIDS:
    cmd_successful = SS_ListPids (cmd);
    break;
  case CMD_LIST_METRICS:
    cmd_successful = SS_ListMetrics (cmd);
    break;
  case CMD_LIST_PARAMS:
    cmd_successful = SS_ListParams (cmd);
    break;
  case CMD_LIST_SRC:
    cmd_successful = SS_ListSrc (cmd);
    break;
  case CMD_LIST_STATUS:
    cmd_successful = SS_ListStatus (cmd);
    break;
  case CMD_LIST_RANKS:
    cmd_successful = SS_ListRanks (cmd);
    break;
  case CMD_LIST_THREADS:
    cmd_successful = SS_ListThreads (cmd);
    break;
  case CMD_LIST_TYPES:
    cmd_successful = SS_ListTypes (cmd);
    break;
  case CMD_LIST_VIEWS:
    cmd_successful = SS_ListViews (cmd);
    break;

// Session Commands
  case CMD_CLEAR_BREAK:
    cmd_successful = SS_ClearBreaks (cmd);
    break;
  case CMD_EXIT:
    cmd_successful = SS_Exit (cmd);
    break;
  case CMD_HELP:
    cmd_successful = SS_Help (cmd);
    break;
  case CMD_HISTORY:
    cmd_successful = SS_History (cmd);
    break;
  case CMD_LOG:
    cmd_successful = SS_Log (cmd);
    break;
  case CMD_OPEN_GUI:
    cmd_successful = SS_OpenGui (cmd);
    break;
  case CMD_PLAYBACK:
    cmd_successful = SS_Playback (cmd);
    break;
  case CMD_RECORD:
    cmd_successful = SS_Record (cmd);
    break;
  case CMD_SETBREAK:
    cmd_successful = SS_SetBreak (cmd);
    break;
  case CMD_WAIT:
    cmd_successful = SS_Wait (cmd);
    break;
  default:
#if 1
    std::string s("This command has not been implimented yet.");
    Mark_Cmd_With_Soft_Error(cmd,s);
#else
    cmd->Result_String ("This command has not been implimented yet");
    cmd->set_Status(CMD_ERROR);
#endif
    cmd_successful = false;
  }

}
catch(const Exception& error) {
  cmd->Result_String ("An unrecoverable error was encountered while trying to execute this command.");
  Mark_Cmd_With_Std_Error (cmd, error);
  cmd->set_Status(CMD_ERROR);
  cmd_successful = false;
}

  if (cmd->Status() == CMD_EXECUTING) {
   // The semantic processor failed to flag the command as complete!
   // This is clearly NOT how we intend to do things!
   // But we can't get here unless it is complete, so mark it.
    cmd->set_Status (cmd_successful ? CMD_COMPLETE : CMD_ERROR);
  }

  if (redirect_streamP != NULL) {
   cmd->Print_Results (*redirect_streamP, "\n", "\n");
   cmd->set_Results_Used ();
   redirect_streamP->flush();
   delete redirect_streamP;
  }

  Cmd_Obj_Complete (cmd);
}

int64_t EXT_Created = 0;
static int64_t EXT_Allocated = 0;
static int64_t EXT_Free      = 0;
static bool Ready_for_Next_Cmd = true;
static CommandObject *Last_Cmd = NULL;
static int64_t Cmds_Executed = 0;

static pthread_mutex_t Cmd_EXT_Lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  Cmd_EXT_Dispatch = PTHREAD_COND_INITIALIZER;
static std::list<CommandObject *> EXT_Dispatch;

static bool Cmd_Waiting = false;
static pthread_cond_t  Waiting_For_Cmds = PTHREAD_COND_INITIALIZER;

static bool Main_Waiting = false;
static int64_t Main_Waiting_Count = 0;
static pthread_cond_t  Waiting_For_Main = PTHREAD_COND_INITIALIZER;

static void Wait_For_Others_To_Terminate () {
  Assert(pthread_mutex_lock(&Cmd_EXT_Lock) == 0);
  while (EXT_Allocated > 2) {   // There must be 2 processes: me and one to wake me up!
   // Wait for other pthreads to finish executing and exit.
    Cmd_Waiting = true;
    Assert(pthread_cond_wait(&Waiting_For_Cmds,&Cmd_EXT_Lock) == 0);
    Cmd_Waiting = false;
  }
  Assert(pthread_mutex_unlock(&Cmd_EXT_Lock) == 0);
}

void Wait_For_Previous_Cmds () {
  Assert(pthread_mutex_lock(&Cmd_EXT_Lock) == 0);
  while (EXT_Allocated > (EXT_Free + 1)) {
   // Wait for other pthreads to finish executing.
    Cmd_Waiting = true;
    Assert(pthread_cond_wait(&Waiting_For_Cmds,&Cmd_EXT_Lock) == 0);
    Cmd_Waiting = false;
  }
  Assert(pthread_mutex_unlock(&Cmd_EXT_Lock) == 0);
}

void Purge_Dispatch_Queue () {
  Assert(pthread_mutex_lock(&Cmd_EXT_Lock) == 0);
  while (EXT_Dispatch.begin() != EXT_Dispatch.end()) {
    CommandObject *cmd = *EXT_Dispatch.begin();
    EXT_Dispatch.pop_front();
    cmd->set_Status (CMD_ABORTED);
    Cmd_Obj_Complete (cmd);
  }
  Assert(pthread_mutex_unlock(&Cmd_EXT_Lock) == 0);
}

void SafeToDoNextCmd () {
 // Get exclusive access to dispatch queue.
  Assert(pthread_mutex_lock(&Cmd_EXT_Lock) == 0);
  Ready_for_Next_Cmd = true;
  if ((EXT_Dispatch.begin() != EXT_Dispatch.end()) &&
      (EXT_Free != 0)) {
   // Wake up someone to process it.
    Assert(pthread_cond_signal(&Cmd_EXT_Dispatch) == 0);
  }
 // Release the lock so someone can pick up the command
  Assert(pthread_mutex_unlock(&Cmd_EXT_Lock) == 0);
}

static void Cmd_EXT_Create () {
 // When we get here Cmd_EXT_Lock is not set.
  Assert(pthread_mutex_lock(&Cmd_EXT_Lock) == 0);
  EXT_Allocated++;
  EXT_Created++;

 // This is an infinite loop that waits for the dispatch signal.
  for(;;) {
    CommandObject *cmd = NULL;
    int64_t this_cmd_is = 0;
    while (!Ready_for_Next_Cmd ||
           (EXT_Dispatch.begin() == EXT_Dispatch.end())) {
      EXT_Free++;  // I am looking for work.
      if (Cmd_Waiting &&
          !Shut_Down &&
          (EXT_Allocated == (EXT_Free + 1))) {
       // Wakeup the waiting process for single thread execution
        Assert(pthread_cond_signal(&Waiting_For_Cmds) == 0);
      }
     // Release the lock and wait.
      Assert(pthread_cond_wait(&Cmd_EXT_Dispatch,&Cmd_EXT_Lock) == 0);
     // When we wake up, we have the lock again.
      EXT_Free--;  // I found work.
      if (Shut_Down) {
       // They woke me up to get rid of me!
        EXT_Allocated--;  // I'm going to go away.
        if (EXT_Allocated == 1) {
         // Wakeup the last process so it can terminate.
          Assert(pthread_cond_signal(&Waiting_For_Cmds) == 0);
        }
        Assert(pthread_mutex_unlock(&Cmd_EXT_Lock) == 0);    // release lock
        pthread_exit(0);                                     // vanish
      }
    }

    // CommandObject *cmd = NULL;
    if (EXT_Dispatch.begin() != EXT_Dispatch.end()) {
     // Pick up the first command on the list.
      cmd = *EXT_Dispatch.begin();
      Last_Cmd = cmd;
      this_cmd_is = ++Cmds_Executed;
      EXT_Dispatch.pop_front();
      Ready_for_Next_Cmd = false;
    }
    Assert(pthread_mutex_unlock(&Cmd_EXT_Lock) == 0);

   // Go process the command.
    if ((cmd != NULL) &&
        (cmd->Status() != CMD_ERROR) &&
        (cmd->Status() != CMD_ABORTED)) {
      Cmd_Execute (cmd);
    }

   // When we complete the command, look for more work to do.
    Assert(pthread_mutex_lock(&Cmd_EXT_Lock) == 0);

    if (Shut_Down) {
     // Somebody has processed an Exit command.
      if (EXT_Free != 0) {
       // Signal all the free processes to terminate.
        Assert(pthread_cond_broadcast(&Cmd_EXT_Dispatch) == 0);
      }
      if (this_cmd_is == Cmds_Executed) {
       // I am the thread processing an 'Exit' command.
       // Let other threads start up - free ones will terminate.
        Assert(pthread_mutex_unlock(&Cmd_EXT_Lock) == 0);

       // Wait for all executing threads to terminate.
        Wait_For_Others_To_Terminate ();

       // Tell the input routines to send EOF to Python.
        Assert(pthread_mutex_lock(&Async_Input_Lock) == 0);
        Assert(pthread_cond_signal(&Async_Input_Available) == 0);
        Assert(pthread_mutex_unlock(&Async_Input_Lock) == 0);
        if (Main_Waiting) {
          Main_Waiting = false;
          Assert(pthread_cond_signal(&Waiting_For_Main) == 0);
        }
      } else {
        if (Cmd_Waiting &&
            (EXT_Allocated == 2)) {
         // I'm going to go away, so if there is just the waiting
         // process remaining, wake it up so it can terminate, too.
          Assert(pthread_cond_signal(&Waiting_For_Cmds) == 0);
        }
      }
      EXT_Allocated--;  // I'm going away, now.
      Assert(pthread_mutex_unlock(&Cmd_EXT_Lock) == 0);
      pthread_exit(0);
    }

    if (Main_Waiting &&
        (EXT_Dispatch.size() <= Main_Waiting_Count)) {
      Main_Waiting = false;
      Assert(pthread_cond_signal(&Waiting_For_Main) == 0);
    }

    if (!Ready_for_Next_Cmd &&
        (this_cmd_is == Cmds_Executed)) {
     // Be sure that the next command can proceed.
      Ready_for_Next_Cmd = true;
    }
  }
}


void SS_Execute_Cmd (CommandObject *cmd) {
  bool serialize_exexcution = cmd->Needed_By_Python();

 // Prevent unsafe changes
  Assert(pthread_mutex_lock(&Cmd_EXT_Lock) == 0);

  if (!Shut_Down) {

    if (serialize_exexcution) {
      if (EXT_Allocated != EXT_Free) {
        Main_Waiting = true;
        Main_Waiting_Count = 0;
        Assert(pthread_cond_wait(&Waiting_For_Main,&Cmd_EXT_Lock) == 0);
      }
      if (!Shut_Down) {
        Assert(pthread_mutex_unlock(&Cmd_EXT_Lock) == 0);
        Cmd_Execute (cmd);
        return;
      }
    } else  if (cmd->Type() == CMD_RECORD) {
     // Execute this command with the main thread
     // and do it immediately so that any new comamnds
     // read from the same input file will be logged
     // in the record file.
      Assert(pthread_mutex_unlock(&Cmd_EXT_Lock) == 0);
      Cmd_Execute (cmd);
      return;
    } else {
     // Add the Command to the Queue
      EXT_Dispatch.push_back(cmd);

     // Wake up someone to process it.
      if (Ready_for_Next_Cmd) {
        if (EXT_Free != 0) {
          Assert(pthread_cond_signal(&Cmd_EXT_Dispatch) == 0);
        }  else if (EXT_Allocated < OPENSS_MAX_ASYNC_COMMANDS) {
         // Allocate a new process to execute comamnds.
          pthread_t EXT_handle;
          int stat = pthread_create(&EXT_handle, 0, (void *(*)(void *))Cmd_EXT_Create,(void *)NULL);
          if (stat != 0) {
           // Attempt error recovery and exit.
            cmd->Result_String ("ERROR: unable to execute any commands.");
            cmd->set_Status (CMD_ERROR);
            Cmd_Obj_Complete (cmd);
            Shut_Down = true;
          }
        }
      }

      if (cmd->Type() == CMD_EXIT) {
       // Don't read any more input until this command is
       // processed and shut down can be initiated.
        if (!Shut_Down) {
          Main_Waiting = true;
          Main_Waiting_Count = 0;
          Assert(pthread_cond_wait(&Waiting_For_Main,&Cmd_EXT_Lock) == 0);
        }
      } else if (cmd->Type() == CMD_PLAYBACK) {
       // Don't read any more input until this command is
       // processed and the input file can be used.
        Main_Waiting = true;
        Main_Waiting_Count = 0;
        Assert(pthread_cond_wait(&Waiting_For_Main,&Cmd_EXT_Lock) == 0);
      } else if (EXT_Dispatch.size() > (OPENSS_MAX_ASYNC_COMMANDS * 2)) {
       // Don't read any more input until we can process
       // some of the commands we already have.
        Main_Waiting = true;
        Main_Waiting_Count = OPENSS_MAX_ASYNC_COMMANDS;
        Assert(pthread_cond_wait(&Waiting_For_Main,&Cmd_EXT_Lock) == 0);
      }
    }
  }

 // Release the lock so someone can pick up the command
  Assert(pthread_mutex_unlock(&Cmd_EXT_Lock) == 0);
}
