/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2010 Krell Institute  All Rights Reserved.
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

/** @file
 *
 * Definition of the Command processing related information.
 *
 */


#ifndef COMMANDER_H
#define COMMANDER_H
typedef int64_t EXPID;
typedef int64_t CMDID;
typedef int64_t CMDWID;

// Forward class definitions:
class CommandObject; // defined in CommandObject.hxx
class InputLineObject; // defined in Clip.hxx
class CommandWindowID; // defined in Commander.cxx
class ss_ostream; // defined in SS_Output.hxx

// Basic Initialization and Termination calls.
void Commander_Initialization ();
void Commander_Termination ();

// Command Windows provide a way to get textual commands into the OpendSpeedShop tool.
CMDWID Default_Window (const char *my_name, const char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async);
CMDWID TLI_Window     (const char *my_name, const char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async);
CMDWID GUI_Window     (const char *my_name, const char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async);
CMDWID RLI_Window     (const char *my_name, const char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async);
CMDWID Embedded_Window(const char *my_name, char *my_host, pid_t my_pid, int64_t my_panel, bool Input_is_Async);
void   Window_Termination (CMDWID my_window);

// ss_ostreams provide a way to get textual output to the proper window.
void Send_Message_To_Window (CMDWID to_window, std::string S);
void Redirect_Window_Output (CMDWID for_window, ss_ostream *for_out, ss_ostream *for_err);
ss_ostream *Predefined_ostream (std::string oname);
ss_ostream *Window_outstream (CMDWID for_window);
ss_ostream *Window_errstream (CMDWID for_window);

void Default_TLI_Line_Output (InputLineObject *clip);
void Default_TLI_Command_Output (CommandObject *C);
void Default_Log_Output (InputLineObject *clip);
void Default_Log_Output (CommandObject *C);

extern CMDWID command_line_window;
extern CMDWID tli_window;
extern CMDWID gui_window;
extern CMDWID Embedded_WindowID;
extern const char *Current_OpenSpeedShop_Prompt;
extern const char *Alternate_Current_OpenSpeedShop_Prompt;

// History Buffers
extern int64_t History_Count;
extern std::list<std::string> History;

// Selection of items in the log file are controlled throught his enum.
// Except for raw data dumps, the record identifier is stripped from output.
enum Log_Entry_Type
  { CMDW_TRACE_ALL,                  // Dump raw data
    CMDW_TRACE_COMMANDS,             // Dump command line records with added information - "C "
    CMDW_TRACE_ORIGINAL_COMMANDS,    // Dump command line recoreds as entered by user - "C "
    CMDW_TRACE_RESULTS               // Dump result records  - "R "
  };

// Main readline interface
InputLineObject *SpeedShop_ReadLine (int is_more);

/* Global Data for tracking the current command line. */
extern InputLineObject *Current_ILO;
extern CommandObject   *Current_CO;

// Attach a new input source line that will be read AFTER all the previous ones
InputLineObject *Append_Input_String (CMDWID issuedbywindow, const char *b_ptr,
                                      void *LocalCmdId = NULL,
                                      void (*CallBackLine) (InputLineObject *b) = NULL,
                                      void (*CallBackCmd) (CommandObject *b) = NULL);

// Attach a new input source that will be read AFTER all the previous ones
bool Append_Input_File (CMDWID issuedbywindow, std::string fromfname,
                                      void (*CallBackLine) (InputLineObject *b) = NULL,
                                      void (*CallBackCmd) (CommandObject *b) = NULL);

// Attach a new input source that will be read BEFORE all the previous ones
bool Push_Input_File (CMDWID issuedbywindow, std::string fromfname,
                                      void (*CallBackLine) (InputLineObject *b) = NULL,
                                      void (*CallBackCmd) (CommandObject *b) = NULL);

// Manipulate tracing options
bool Command_Record_OFF (CMDWID WindowID);
bool Command_Record_ON (CMDWID WindowID, std::string tofname);
bool Command_Log_OFF (CMDWID WindowID);
bool Command_Log_ON (CMDWID WindowID, std::string tofname);

// Focus is a property of the Command Window that issued the command.
void Experiment_Purge_Focus (EXPID ExperimentID);                // Remove focus from windows
EXPID Experiment_Focus (CMDWID WindowID);                        // What is the focus?
EXPID Experiment_Focus (CMDWID WindowID, EXPID ExperimentID);    // Set the focus.

// Batch processing input is not Async.  Command execution should be serial.
bool Window_Is_Async (CMDWID WindowID);

// Communicate command information to the window manager
extern void Link_Cmd_Obj_to_Input (InputLineObject *I, CommandObject *);
extern void Clip_Complete (InputLineObject *clip);
extern void Cmd_Obj_Complete (CommandObject *C);
extern void ReDirect_User_Stdout (const char *S, const int &len, void *tag);
extern void ReDirect_User_Stderr (const char *S, const int &len, void *tag);

// The following routines are used to clean up after a CNTRL-C or "Exit" command.
// They will purge waiting commands from the input queues, abort any commands
// that have been aprsed but are not yet executing, and will stop the execution
// of all commands that are being executed.
extern void Purge_Input_Sources ();
extern void Purge_Dispatch_Queue ();
extern void Purge_Executing_Commands ();

// The following controls are used to properly order termination of threads.
extern pthread_mutex_t Async_Input_Lock;
extern int AsyncInputLockCount;
extern pthread_cond_t  Async_Input_Available;  // The input reader waits here.
extern bool            Shut_Down;              // Set when 'exit' command is processed

#endif // COMMANDER_H
