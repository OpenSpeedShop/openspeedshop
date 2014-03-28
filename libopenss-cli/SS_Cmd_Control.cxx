/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2011 Krell Institute  All Rights Reserved.
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

//#define DEBUG_SYNC 1
//#define DEBUG_SYNC_SIGNAL 1

#include "SS_Input_Manager.hxx"
//#include <fstream.h>
#include "SS_Timings.hxx"

static void Cmd_Execute (CommandObject *cmd) {

#if DEBUG_SYNC
  printf("[TID=%ld], Cmd_Execute, entered\n", pthread_self() );
#endif

  std::string *redirect_name = cmd->P_Result()->getRedirectTarget();
  std::ofstream *redirect_streamP = NULL;
  std::ios_base::openmode open_mode = std::ios::out;
  if (redirect_name->length() == 0) {
    redirect_name = cmd->P_Result()->getAppendTarget();
    open_mode = std::ios::app;
  }
  if ((redirect_name->length() > 0) &&
      (Predefined_ostream (*redirect_name) == NULL)) {
   // If they give us a real file name, direct command output there.
    redirect_streamP = new std::ofstream (redirect_name->c_str(), open_mode);
    if (redirect_streamP == NULL) {
      std::string s("Could not open file " + *redirect_name + ".");
      Mark_Cmd_With_Soft_Error(cmd,s);
      Cmd_Obj_Complete (cmd);
      return;
    }
  }

// Bracket execution of commands with a "catch-all" exception handler.
// This is intended to catch failures in recovery code.
  bool cmd_successful = false;
try {

  InputLineObject *clip = cmd->Clip();

#if DEBUG_SYNC
  printf("[TID=%ld], Cmd_Execute, clip->Command().c_str()=%s\n", pthread_self(), clip->Command().c_str());
#endif

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

#if DEBUG_SYNC
  printf("[TID=%ld], Cmd_Execute, cmd->Type()=%d\n", pthread_self(), cmd->Type() );
  printf("[TID=%ld], Cmd_Execute, clip->Command().c_str()=%s\n", pthread_self(), clip->Command().c_str());
#endif

 // And now go and execute it!
  switch (cmd->Type()) {

// Experiment Building BLock Commands

  case CMD_EXP_ATTACH:

#if BUILD_CLI_TIMING
    // Gather performance information on the expAttach command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::expAttachStart] = Time::Now();
         cli_timing_handle->in_expAttach(true);
         cli_timing_handle->in_expCreate(false);
    }
#endif

    cmd_successful = SS_expAttach (cmd);

#if BUILD_CLI_TIMING
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->processTimingEventEnd( SS_Timings::expAttachStart,
                                                   SS_Timings::expAttachCount,
                                                   SS_Timings::expAttachMax,
                                                   SS_Timings::expAttachMin,
                                                   SS_Timings::expAttachTotal,
                                                   SS_Timings::expAttachEnd);
         cli_timing_handle->in_expAttach(false);
         cli_timing_handle->in_expCreate(false);
    }
#endif

    break;

  case CMD_EXP_CLONE:

    cmd_successful = SS_expClone(cmd);
    break;

  case CMD_EXP_CLOSE:

    cmd_successful = SS_expClose(cmd);
    break;

  case CMD_EXP_CONT:

    cmd_successful = SS_expCont (cmd);
    break;

  case CMD_EXP_CREATE:

#if BUILD_CLI_TIMING
    // Gather performance information on the expCreate command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::expCreateStart] = Time::Now();
         cli_timing_handle->in_expCreate(true);
         cli_timing_handle->in_expAttach(false);
    }
#endif

    cmd_successful = SS_expCreate (cmd);

#if BUILD_CLI_TIMING
    // Process the gathered performance information on the expCreate command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->processTimingEventEnd( SS_Timings::expCreateStart,
                                                   SS_Timings::expCreateCount,
                                                   SS_Timings::expCreateMax,
                                                   SS_Timings::expCreateMin,
                                                   SS_Timings::expCreateTotal,
                                                   SS_Timings::expCreateEnd);
         cli_timing_handle->in_expCreate(false);
         cli_timing_handle->in_expAttach(false);
    }
#endif

    break;
  case CMD_EXP_DETACH:

#if BUILD_CLI_TIMING
    // Gather performance information on the expDetach command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::expDetachStart] = Time::Now();
    }
#endif

    cmd_successful = SS_expDetach(cmd);

#if BUILD_CLI_TIMING
    // Process the gathered performance information on the expDetach command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->processTimingEventEnd( SS_Timings::expDetachStart,
                                                   SS_Timings::expDetachCount,
                                                   SS_Timings::expDetachMax,
                                                   SS_Timings::expDetachMin,
                                                   SS_Timings::expDetachTotal,
                                                   SS_Timings::expDetachEnd);
    }
#endif

    break;
  case CMD_EXP_DISABLE:

#if BUILD_CLI_TIMING
    // Gather performance information on the expDisable command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::expDisableStart] = Time::Now();
    }
#endif

    cmd_successful = SS_expDisable(cmd);

#if BUILD_CLI_TIMING
    // Process the gathered performance information on the expDisable command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->processTimingEventEnd( SS_Timings::expDisableStart,
                                                   SS_Timings::expDisableCount,
                                                   SS_Timings::expDisableMax,
                                                   SS_Timings::expDisableMin,
                                                   SS_Timings::expDisableTotal,
                                                   SS_Timings::expDisableEnd);
    }
#endif

    break;
  case CMD_EXP_ENABLE:

#if BUILD_CLI_TIMING
    // Gather performance information on the expEnable command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::expEnableStart] = Time::Now();
    }
#endif

    cmd_successful = SS_expEnable(cmd);

#if BUILD_CLI_TIMING
    // Process the gathered performance information on the expEnable command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->processTimingEventEnd( SS_Timings::expEnableStart,
                                                   SS_Timings::expEnableCount,
                                                   SS_Timings::expEnableMax,
                                                   SS_Timings::expEnableMin,
                                                   SS_Timings::expEnableTotal,
                                                   SS_Timings::expEnableEnd);
    }
#endif

    break;
  case CMD_EXP_FOCUS:

#if BUILD_CLI_TIMING
    // Gather performance information on the expFocus command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::expFocusStart] = Time::Now();
    }
#endif

    cmd_successful = SS_expFocus (cmd);

#if BUILD_CLI_TIMING
    // Process the gathered performance information on the expFocus command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->processTimingEventEnd( SS_Timings::expFocusStart,
                                                   SS_Timings::expFocusCount,
                                                   SS_Timings::expFocusMax,
                                                   SS_Timings::expFocusMin,
                                                   SS_Timings::expFocusTotal,
                                                   SS_Timings::expFocusEnd);
    }
#endif

    break;
  case CMD_EXP_PAUSE:

#if BUILD_CLI_TIMING
    // Gather performance information on the expPause command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::expPauseStart] = Time::Now();
    }
#endif

    cmd_successful = SS_expPause (cmd);

#if BUILD_CLI_TIMING
    // Process the gathered performance information on the expPause command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->processTimingEventEnd( SS_Timings::expPauseStart,
                                                   SS_Timings::expPauseCount,
                                                   SS_Timings::expPauseMax,
                                                   SS_Timings::expPauseMin,
                                                   SS_Timings::expPauseTotal,
                                                   SS_Timings::expPauseEnd);
    }
#endif

    break;
  case CMD_EXP_GO:

#if BUILD_CLI_TIMING
    // Gather performance information on the expGo command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::expGoStart] = Time::Now();
    }
#endif

    cmd_successful = SS_expGo (cmd);

#if BUILD_CLI_TIMING
    // Process the gathered performance information on the expGo command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->processTimingEventEnd( SS_Timings::expGoStart,
                                                   SS_Timings::expGoCount,
                                                   SS_Timings::expGoMax,
                                                   SS_Timings::expGoMin,
                                                   SS_Timings::expGoTotal,
                                                   SS_Timings::expGoEnd);
    }
#endif
    break;

  case CMD_EXP_RESTORE:

#if BUILD_CLI_TIMING
    // Gather performance information on the expRestore command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::expRestoreStart] = Time::Now();
    }
#endif

    cmd_successful = SS_expRestore (cmd);

#if BUILD_CLI_TIMING
    // Process the gathered performance information on the expRestore command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->processTimingEventEnd( SS_Timings::expRestoreStart,
                                                   SS_Timings::expRestoreCount,
                                                   SS_Timings::expRestoreMax,
                                                   SS_Timings::expRestoreMin,
                                                   SS_Timings::expRestoreTotal,
                                                   SS_Timings::expRestoreEnd);
    }
#endif

    break;
  case CMD_EXP_SAVE:

#if BUILD_CLI_TIMING
    // Gather performance information on the expSave command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::expSaveStart] = Time::Now();
    }
#endif
    cmd_successful = SS_expSave (cmd);

#if BUILD_CLI_TIMING
    // Process the gathered performance information on the expSave command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->processTimingEventEnd( SS_Timings::expSaveStart,
                                                   SS_Timings::expSaveCount,
                                                   SS_Timings::expSaveMax,
                                                   SS_Timings::expSaveMin,
                                                   SS_Timings::expSaveTotal,
                                                   SS_Timings::expSaveEnd);
    }
#endif

    break;
  case CMD_EXP_SETARGS:

    cmd_successful = SS_expSetArgs (cmd);

    break;

  case CMD_EXP_SETPARAM:

#if BUILD_CLI_TIMING
    // Gather performance information on the expSetParam command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::expSetParamStart] = Time::Now();
    }
#endif

    cmd_successful = SS_expSetParam (cmd);

#if BUILD_CLI_TIMING
    // Process the gathered performance information on the expSetParam command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->processTimingEventEnd( SS_Timings::expSetParamStart,
                                                   SS_Timings::expSetParamCount,
                                                   SS_Timings::expSetParamMax,
                                                   SS_Timings::expSetParamMin,
                                                   SS_Timings::expSetParamTotal,
                                                   SS_Timings::expSetParamEnd);
    }
#endif

    break;

// Information Commands
  case CMD_EXP_COMPARE:

#if BUILD_CLI_TIMING
    // Gather performance information on the expCompare command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::expCompareStart] = Time::Now();
    }
#endif

    cmd_successful = SS_expCompare (cmd);

#if BUILD_CLI_TIMING
    // Process the gathered performance information on the expCompare command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
       cli_timing_handle->processTimingEventEnd( SS_Timings::expCompareStart,
                                                 SS_Timings::expCompareCount,
                                                 SS_Timings::expCompareMax,
                                                 SS_Timings::expCompareMin,
                                                 SS_Timings::expCompareTotal,
                                                 SS_Timings::expCompareEnd);
    }
#endif

    break;
  case CMD_EXP_STATUS:
    cmd_successful = SS_expStatus (cmd);
    break;
  case CMD_EXP_DATA:
  case CMD_EXP_VIEW:
  case CMD_C_VIEW:
  case CMD_VIEW:

#if BUILD_CLI_TIMING
    // Gather performance information on the expCmd_C_View command
    if (cmd->Type() == CMD_C_VIEW && cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::expCmd_C_ViewStart] = Time::Now();
    }
#endif

#if BUILD_CLI_TIMING
    // Gather performance information on the expCmd_View command
    if (cmd->Type() == CMD_VIEW && cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::expCmd_ViewStart] = Time::Now();
    }
#endif

    cmd_successful = SS_View (cmd);

#if BUILD_CLI_TIMING
    // Process the gathered performance information on the expCmd_C_View command
    if (cmd->Type() == CMD_C_VIEW && cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
       cli_timing_handle->processTimingEventEnd( SS_Timings::expCmd_C_ViewStart,
                                                 SS_Timings::expCmd_C_ViewCount,
                                                 SS_Timings::expCmd_C_ViewMax,
                                                 SS_Timings::expCmd_C_ViewMin,
                                                 SS_Timings::expCmd_C_ViewTotal,
                                                 SS_Timings::expCmd_C_ViewEnd);
    }
#endif

#if BUILD_CLI_TIMING
    // Process the gathered performance information on the expCmd_View command
    if (cmd->Type() == CMD_VIEW && cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
       cli_timing_handle->processTimingEventEnd( SS_Timings::expCmd_ViewStart,
                                                 SS_Timings::expCmd_ViewCount,
                                                 SS_Timings::expCmd_ViewMax,
                                                 SS_Timings::expCmd_ViewMin,
                                                 SS_Timings::expCmd_ViewTotal,
                                                 SS_Timings::expCmd_ViewEnd);
    }
#endif

    break;
  case CMD_INFO:

#if BUILD_CLI_TIMING
    // Gather performance information on the expCmd_Info command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::expCmd_InfoStart] = Time::Now();
    }
#endif

    cmd_successful = SS_Info (cmd);

#if BUILD_CLI_TIMING
    // Process the gathered performance information on the expCmd_Info command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
       cli_timing_handle->processTimingEventEnd( SS_Timings::expCmd_InfoStart,
                                                 SS_Timings::expCmd_InfoCount,
                                                 SS_Timings::expCmd_InfoMax,
                                                 SS_Timings::expCmd_InfoMin,
                                                 SS_Timings::expCmd_InfoTotal,
                                                 SS_Timings::expCmd_InfoEnd);
    }
#endif
    break;

// Custom View Commands
  case  CMD_C_VIEW_CREATE:

#if BUILD_CLI_TIMING
    // Start timing - spent in the custom view creation processing
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::cvCreateStart] = Time::Now();
    }
#endif
    cmd_successful = SS_cvSet (cmd);

#if BUILD_CLI_TIMING
    // Process the time spent in the custom view creation processing
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
       cli_timing_handle->processTimingEventEnd( SS_Timings::cvCreateStart,
                                                 SS_Timings::cvCreateCount,
                                                 SS_Timings::cvCreateMax,
                                                 SS_Timings::cvCreateMin,
                                                 SS_Timings::cvCreateTotal,
                                                 SS_Timings::cvCreateEnd);
    }
#endif

    break;
  case  CMD_C_VIEW_DELETE:
    cmd_successful = SS_cvClear (cmd);
    break;
  case  CMD_C_VIEW_CLUSTERS:

#if BUILD_CLI_TIMING
    // Time the cluster view generation subroutine
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::cvClustersStart] = Time::Now();
    }
#endif

    cmd_successful = SS_cvClusters (cmd);

#if BUILD_CLI_TIMING
    // Process the time spent in the custom view cluster analysis processing
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
       cli_timing_handle->processTimingEventEnd( SS_Timings::cvClustersStart,
                                                 SS_Timings::cvClustersCount,
                                                 SS_Timings::cvClustersMax,
                                                 SS_Timings::cvClustersMin,
                                                 SS_Timings::cvClustersTotal,
                                                 SS_Timings::cvClustersEnd);
    }
#endif

    break;
  case  CMD_C_VIEW_INFO:

#if BUILD_CLI_TIMING
    // Time spent in the custom view information commands
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::cvInfoStart] = Time::Now();
    }
#endif

    cmd_successful = SS_cvInfo (cmd);

#if BUILD_CLI_TIMING
    // Process the time spent in the custom view information commands
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
       cli_timing_handle->processTimingEventEnd( SS_Timings::cvInfoStart,
                                                 SS_Timings::cvInfoCount,
                                                 SS_Timings::cvInfoMax,
                                                 SS_Timings::cvInfoMin,
                                                 SS_Timings::cvInfoTotal,
                                                 SS_Timings::cvInfoEnd);
    }
#endif

    break;


// Low Level Information Commands
  case CMD_LIST_GENERIC:

#if BUILD_CLI_TIMING
    // Time the generic list commands
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::listGenericStart] = Time::Now();
    }
#endif

    cmd_successful = SS_ListGeneric (cmd);

#if BUILD_CLI_TIMING
    // Process the time spent in the generic list commands
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
       cli_timing_handle->processTimingEventEnd( SS_Timings::listGenericStart,
                                                 SS_Timings::listGenericCount,
                                                 SS_Timings::listGenericMax,
                                                 SS_Timings::listGenericMin,
                                                 SS_Timings::listGenericTotal,
                                                 SS_Timings::listGenericEnd);
    }
#endif

    break;

// Session Commands
  case CMD_CLEAR_BREAK:
    cmd_successful = SS_ClearBreaks (cmd);
    break;
  case CMD_ECHO:
    cmd_successful = SS_Echo (cmd);
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

#if BUILD_CLI_TIMING
    // Time the wait command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::cliWaitStart] = Time::Now();
    }
#endif

    cmd_successful = SS_Wait (cmd);

#if BUILD_CLI_TIMING
    // Process the time spent in the wait command
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
       cli_timing_handle->processTimingEventEnd( SS_Timings::cliWaitStart,
                                                 SS_Timings::cliWaitCount,
                                                 SS_Timings::cliWaitMax,
                                                 SS_Timings::cliWaitMin,
                                                 SS_Timings::cliWaitTotal,
                                                 SS_Timings::cliWaitEnd);
    }
#endif

    break;
  default:
    std::string s("This command has not been implemented yet.");
    Mark_Cmd_With_Soft_Error(cmd,s);
    cmd_successful = false;
  }

}
catch (std::bad_alloc) {
  Mark_Cmd_With_Soft_Error(cmd,"The command could not be executed because a memory allocation error has occurred.");
  cmd_successful = false;
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
    cmd->Print_Results (*redirect_streamP, "\n", "\n" );
    cmd->set_Results_Used ();
    redirect_streamP->flush();
    delete redirect_streamP;
  } else if ( (cmd->Clip() != NULL) &&
              (cmd->Clip()->Who() != tli_window) &&
              OPENSS_SAVE_EXPERIMENT_DATABASE &&
              cmd->SaveResult() &&
              (cmd->SaveResultFile().length() > 0) ) {
    cmd->Print_Results (std::cerr, "\n", "\n" );
    cmd->set_Results_Used ();
  }

  Cmd_Obj_Complete (cmd);
}

int64_t EXT_Created = 0;
bool isOfflineCmd = false;
static int64_t EXT_Allocated = 0;
static int64_t EXT_Free      = 0;
static bool Ready_for_Next_Cmd = true;
static CommandObject *Last_Cmd = NULL;
static int64_t Cmds_Executed = 0;

static pthread_mutex_t Cmd_EXT_Lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  Cmd_EXT_Dispatch = PTHREAD_COND_INITIALIZER;
static std::list<CommandObject *> EXT_Dispatch;
int  CmdEXTDispatchCount = 1000;

static bool Cmd_Waiting = false;
static pthread_cond_t  Waiting_For_Cmds = PTHREAD_COND_INITIALIZER;
int  WaitingForCmdsCount = 1000;

static bool Main_Waiting = false;
static int64_t Main_Waiting_Count = 0;
static pthread_cond_t  Waiting_For_Main = PTHREAD_COND_INITIALIZER;
int  WaitingForMainCount = 1000;

static void Wait_For_Others_To_Terminate () {

#if DEBUG_SYNC
  printf("[TID=%ld], Wait_For_Others_To_Terminate, entered\n", pthread_self() );
#endif

  Assert(pthread_mutex_lock(&Cmd_EXT_Lock) == 0);

#if BUILD_CLI_TIMING
  // Start timing the processing time spent in the  Wait_For_Others_To_Terminate routine
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->cli_perf_data[SS_Timings::cliWaitForOthersTermStart] = Time::Now();
  }
#endif

  while (EXT_Allocated > 2) {   // There must be 2 processes: me and one to wake me up!
   // Wait for other pthreads to finish executing and exit.
    Cmd_Waiting = true;

    Assert(pthread_cond_wait(&Waiting_For_Cmds,&Cmd_EXT_Lock) == 0);

    Cmd_Waiting = false;
  }

#if BUILD_CLI_TIMING
  // Process the time spent in the Wait_For_Others_To_Terminate  routine
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->processTimingEventEnd( SS_Timings::cliWaitForOthersTermStart,
                                                SS_Timings::cliWaitForOthersTermCount,
                                                SS_Timings::cliWaitForOthersTermMax,
                                                SS_Timings::cliWaitForOthersTermMin,
                                                SS_Timings::cliWaitForOthersTermTotal,
                                                SS_Timings::cliWaitForOthersTermEnd);
  }
#endif

  Assert(pthread_mutex_unlock(&Cmd_EXT_Lock) == 0);

}

void Wait_For_Previous_Cmds () {

#if DEBUG_SYNC
  printf("[TID=%ld], Wait_For_Previous_Cmds, entered\n", pthread_self() );
#endif

  Assert(pthread_mutex_lock(&Cmd_EXT_Lock) == 0);

#if BUILD_CLI_TIMING
  // Start timing the processing time spent in the  Wait_For_Previous_Cmds routine
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->cli_perf_data[SS_Timings::cliWaitForPrevCmdsStart] = Time::Now();
  }
#endif

  while (EXT_Allocated > (EXT_Free + 1)) {
   // Wait for other pthreads to finish executing.
    Cmd_Waiting = true;

    Assert(pthread_cond_wait(&Waiting_For_Cmds,&Cmd_EXT_Lock) == 0);

    Cmd_Waiting = false;
  }

#if BUILD_CLI_TIMING
  // Process the time spent in the Wait_For_Previous_Cmds routine
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->processTimingEventEnd( SS_Timings::cliWaitForPrevCmdsStart,
                                                SS_Timings::cliWaitForPrevCmdsCount,
                                                SS_Timings::cliWaitForPrevCmdsMax,
                                                SS_Timings::cliWaitForPrevCmdsMin,
                                                SS_Timings::cliWaitForPrevCmdsTotal,
                                                SS_Timings::cliWaitForPrevCmdsEnd);
  }
#endif

  Assert(pthread_mutex_unlock(&Cmd_EXT_Lock) == 0);

}

void Purge_Dispatch_Queue () {

#if DEBUG_SYNC
  printf("[TID=%ld], Purge_Dispatch_Queue, entered\n", pthread_self() );
#endif

  Assert(pthread_mutex_lock(&Cmd_EXT_Lock) == 0);

#if BUILD_CLI_TIMING
  // Start timing the processing time spent in the  Purge_Dispatch_Queue routine
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->cli_perf_data[SS_Timings::cliPurgeDispatchQueueStart] = Time::Now();
  }
#endif

  while (EXT_Dispatch.begin() != EXT_Dispatch.end()) {
    CommandObject *cmd = *EXT_Dispatch.begin();
    EXT_Dispatch.pop_front();
    cmd->set_Status (CMD_ABORTED);
    Cmd_Obj_Complete (cmd);
  }

#if BUILD_CLI_TIMING
  // Process the time spent in the Purge_Dispatch_Queue routine
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->processTimingEventEnd( SS_Timings::cliPurgeDispatchQueueStart,
                                                SS_Timings::cliPurgeDispatchQueueCount,
                                                SS_Timings::cliPurgeDispatchQueueMax,
                                                SS_Timings::cliPurgeDispatchQueueMin,
                                                SS_Timings::cliPurgeDispatchQueueTotal,
                                                SS_Timings::cliPurgeDispatchQueueEnd);
  }
#endif

  Assert(pthread_mutex_unlock(&Cmd_EXT_Lock) == 0);
}

void SafeToDoNextCmd () {

#if DEBUG_SYNC
  printf("[TID=%ld], SafeToDoNextCmd, entered\n", pthread_self() );
#endif

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

#if DEBUG_SYNC
  printf("[TID=%ld], Cmd_EXT_Create, entered\n", pthread_self() );
#endif

  // When we get here Cmd_EXT_Lock is not set.
  Assert(pthread_mutex_lock(&Cmd_EXT_Lock) == 0);

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

        // Relocking on exit from the if block so that the unlock before pthread_exit doesn't cause a double unlock
        Assert(pthread_mutex_lock(&Cmd_EXT_Lock) == 0);

      } else if (Cmd_Waiting && (EXT_Allocated == 2)) {

         // I'm going to go away, so if there is just the waiting
         // process remaining, wake it up so it can terminate, too.
          Assert(pthread_cond_signal(&Waiting_For_Cmds) == 0);
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


  } // end for

  // Lock aquired around line 861 is only release if ShutDown is true.
  // Should release it here since ShutDown releases the lock end calls pthread_exit...
  Assert(pthread_mutex_unlock(&Cmd_EXT_Lock) == 0);
}


void SS_Execute_Cmd (CommandObject *cmd) {
  bool serialize_exexcution = cmd->Needed_By_Python();

#if DEBUG_SYNC
  printf("[TID=%ld], SS_Execute_Cmd, entered\n", pthread_self() );
#endif

#if BUILD_CLI_TIMING
    // Start timing the processing time spent in the SS_Execute_Cmd routine
    if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
         cli_timing_handle->cli_perf_data[SS_Timings::cliExecuteCmdStart] = Time::Now();
    }
#endif

 // Prevent unsafe changes
  Assert(pthread_mutex_lock(&Cmd_EXT_Lock) == 0);

  if (!Shut_Down) {

    if (cmd->Type() == CMD_EXP_GO) {
      if (isOfflineCmd) {
        serialize_exexcution = true;
      }
    } else {
	serialize_exexcution = cmd->Needed_By_Python();
    }

    if (cmd->Type() == CMD_RECORD) {
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
          EXT_Allocated++;
          int stat = 0;
          try {
            stat = pthread_create(&EXT_handle, 0, (void *(*)(void *))Cmd_EXT_Create,(void *)NULL);
            if (stat == 0) {
              EXT_Created++;
            }
          }
          catch (std::bad_alloc) {
            if (EXT_Allocated <= 1) {
             // Attempt to gracefully terminate.
              stat = -1;
            } else {
             // Silently recover and continue executing with the existing execution threads.
              EXT_Allocated--;
              OPENSS_MAX_ASYNC_COMMANDS = EXT_Allocated;
              stat == 0;
            }
          }
          if (stat != 0) {
           // Attempt error recovery and exit.
            EXT_Allocated--;
            cmd->Result_String ("ERROR: unable to execute any commands.");
            cmd->set_Status (CMD_ERROR);
            Cmd_Obj_Complete (cmd);
            Shut_Down = true;
          }
        }
      }

      if (serialize_exexcution) {
       // Don't read any more input until this command is
       // processed and the result is ready for use.
        Main_Waiting = true;
        Main_Waiting_Count = 0;

        Assert(pthread_cond_wait(&Waiting_For_Main,&Cmd_EXT_Lock) == 0);

      } else if (cmd->Type() == CMD_EXIT) {
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

#if BUILD_CLI_TIMING
  // Process the time spent in the SS_Execute_Cmd routine
  if (cli_timing_handle && cli_timing_handle->is_debug_perf_enabled() ) {
      cli_timing_handle->processTimingEventEnd( SS_Timings::cliExecuteCmdStart,
                                                SS_Timings::cliExecuteCmdCount,
                                                SS_Timings::cliExecuteCmdMax,
                                                SS_Timings::cliExecuteCmdMin,
                                                SS_Timings::cliExecuteCmdTotal,
                                                SS_Timings::cliExecuteCmdEnd);
   }
#endif
}
