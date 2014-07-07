////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2007-2014 Krell Institute  All Rights Reserved.
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

/* Take the define below out of comments for debug
   output for the GUI routines that are present this file.
#define DEBUG_GUI 1
*/


/*! \class CLIInterface

    This class is the interface class to the cli.  I contains convienience
    routines to make synchronous calls to the cli.  
  
    Each call has an optional timeout value that can be set.   This timeout
    is defaulted to a reasonable expected failure time.   If the command
    doesn't successfully complete in that time it is aborted and any
    subsequent return value will be ignored.     This behaviour can 
    be modified 2 ways.    There is a flag (warn_of_time) that can 
    be set.   When set to true, a warning will be issued each time the 
    timeout period has been hit.   The use is then prompted if they want
    to continue with another timeout slot.    Alternately, if the timeout
    is 0 (zero) the command will only return when it completes.
*/

#include "CLIInterface.hxx"
#include "debug.hxx"

#include "qcursor.h"
#include "qapplication.h"

#include <qinputdialog.h>
#include <qmessagebox.h>

namespace {

#define CLI_NANOSLEEP_MS 250
   /**
     * Suspend the calling thread.
     *
     * Suspends the calling thread for approximately CLI_NANOSLEEP_MS. Used to implement
     * busy loops that are waiting for state changes in threads.
     *
     * The suspend here replaced sleep(1) calls wherever seen in this file (11/18/13)
     */
    void suspend()
    {
        // Setup to wait
        struct timespec wait;
        wait.tv_sec = 0;
        wait.tv_nsec = CLI_NANOSLEEP_MS * 1000 * 1000;

        // Suspend ourselves temporarily
        // This while loop ensures that nanosleep will sleep at
        // least the amount of time even if a signal interupts nanosleep.
        while(nanosleep(&wait, &wait));
    }
}


bool CLIInterface::interrupt = false;
#define RETURN_FALSE QApplication::restoreOverrideCursor();return false;
#define RETURN_TRUE QApplication::restoreOverrideCursor();return true;

/*! Given a window id create an interface to the cli. */
CLIInterface::CLIInterface(int _wid) : QObject()
{
  wid = _wid;
  CLIInterface::interrupt = false;
  timer = NULL;
  maxTime = 99999;
  warn_of_time = FALSE;
}

CLIInterface::~CLIInterface()
{
}


/**
 * Call into the CLI and ultimately the framework to get the metrics associated with the view command
 *
 * Returns the clip with command objects that represent the view requested
 *
 * @wid           The window id of the requesting window (GUI in this situation)
 * @return        Clip (InputLineObject *) with included command objects
 */


InputLineObject *
CLIInterface::run_Append_Input_String( int wid, const char *command )
{
  maxTime = 99999;
  warn_of_time = FALSE;

#ifdef DEBUG_GUI
  printf("run_Append_Input_String(2) command = (%s)\n", command );
#endif

//  nprintf(DEBUG_COMMANDS) ("run_Append_Input_String(2) command = (%s)\n", command );

  InputLineObject *clip = Append_Input_String( wid, (char *)command);
  return clip;
}

/*! Run a synchronous command.   No values from the cli are return. 
    \param command   The command line interface (cli) command to execute.

    \param mt     The maximum time to wait before command is considered
                  failed.

    \param warn_of_time  If set the user will be prompted at the timeout
                         to see if the command should contunue.
*/
bool
CLIInterface::runSynchronousCLI(const char *command, int mt, bool wot )
{
  maxTime = mt;
  warn_of_time = wot;
  QApplication::setOverrideCursor(QCursor::WaitCursor);
nprintf(DEBUG_COMMANDS) ("runSynchronousCLI() command = (%s)\n", command );
  InputLineObject *clip = Append_Input_String( wid, (char *)command);
  if( clip == NULL )
  {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    RETURN_FALSE;
  }
  Input_Line_Status status = ILO_UNKNOWN;

  while( status != ILO_COMPLETE )
  {
    status = checkStatus(clip, command);
    if( !status || status == ILO_ERROR )
    { // An error occurred.... A message should have been posted.. return;
       RETURN_FALSE;
    }

    qApp->processEvents(8000);

    if( !shouldWeContinue() )
    {
//printf("RETURN FALSE!   COMMAND FAILED!\n");
      clip->Set_Results_Used();
      RETURN_FALSE;
    }

    suspend();
  }

  if( timer )
  {
    timer->stop();
    delete timer;
    timer = NULL;
  }

  if( clip ) 
  {
    clip->Set_Results_Used();
  }
  RETURN_TRUE;
}

/*! Run a synchronous command and return an integer value.
    \param command   The command line interface (cli) command to execute.

    \param val    The integer value returned form the cli.

    \param mark_for_delete   If true delete the values in the cli's objects.

    \param mt     The maximum time to wait before command is considered
                  failed.

    \param warn_of_time  If set the user will be prompted at the timeout
                         to see if the command should contunue.
*/
bool
CLIInterface::getIntValueFromCLI(const char *command, int64_t *val, bool mark_value_for_delete, int mt, bool wot )
{
#ifdef DEBUG_GUI
  printf("CLIInterface::getIntValueFromCLI, command=%s\n", command);
#endif

  maxTime = mt;
  warn_of_time = wot;

  QApplication::setOverrideCursor(QCursor::WaitCursor);

#ifdef DEBUG_GUI
  printf("CLIInterface::getIntValueFromCLI() command = (%s)\n", command );
#endif

  InputLineObject *clip = Append_Input_String( wid, (char *)command);

#ifdef DEBUG_GUI
  printf("CLIInterface::getIntValueFromCLI, after Append_Input_String, clip=0x%x\n", clip);
#endif

  if( clip == NULL ) {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    RETURN_FALSE;
  }
  Input_Line_Status status = ILO_UNKNOWN;

  int timeout_cnt = 0;

  while( status != ILO_COMPLETE ) {

#ifdef DEBUG_GUI
    printf("CLIInterface::getIntValueFromCLI, status != ILO_COMPLETE , calling to checkStatus, status=%d\n", status);
#endif

    status = checkStatus(clip, command);

#ifdef DEBUG_GUI
  printf("CLIInterface::getIntValueFromCLI, status=%d\n", status);
#endif

    if( !status || status == ILO_ERROR ) {   
       // An error occurred.... A message should have been posted.. return;
      fprintf(stderr, "an error occurred processing (%s)!\n", command);
      if( clip ) {
        clip->Set_Results_Used();
      }
      RETURN_FALSE;
    }

    if( status == ILO_COMPLETE ) {

#ifdef DEBUG_GUI
      printf("CLIInterface::getIntValueFromCLI, status = ILO_COMPLETE!, clip=0x%x\n", clip);
#endif

      std::list<CommandObject *>::iterator coi;
      if( clip->CmdObj_List().size() == 1 ) {
      // We should only have one in this case.\n");

#ifdef DEBUG_GUI
        printf("CLIInterface::getIntValueFromCLI, We have 1 command object, Get the data, clip=0x%x\n", clip);
#endif

        coi = clip->CmdObj_List().begin();
        CommandObject *co = (CommandObject *)(*coi);

      
        std::list<CommandResult *>::iterator crl;

#ifdef DEBUG_GUI
        printf("CLIInterface::getIntValueFromCLI,co=0x%x\n", co);
#endif

        crl = co->Result_List().begin();    
        CommandResult_Int *cr_int = (CommandResult_Int *)(*crl);

#ifdef DEBUG_GUI
        printf("CLIInterface::getIntValueFromCLI,cr_int=0x%x\n", cr_int);
#endif

        cr_int->Value(*val);

        if( mark_value_for_delete ) {
          //Allow the garbage collector to clean up the value...

#ifdef DEBUG_GUI
          printf("CLIInterface::getIntValueFromCLI, marking clip->Set_Results_Used, clip=0x%x, clip=0x%x\n", &clip, clip);
#endif

          clip->Set_Results_Used();
        }
      }
      break;
    }
    
    qApp->processEvents(8000);

    if( !shouldWeContinue() )
    {
#ifdef DEBUG_GUI
      printf("EXIT CLIInterface::getIntValueFromCLI, RETURN FALSE!   COMMAND FAILED!,clip=0x%x, clip=0x%x\n", &clip, clip);
#endif
      RETURN_FALSE;
    }

    suspend();
  } 

  if( timer )
  {
    timer->stop();
    delete timer;
    timer = NULL;
  }

#ifdef DEBUG_GUI
    printf("EXIT CLIInterface::getIntValueFromCLI, clip=0x%x, clip=0x%x\n", &clip, clip);
#endif
  RETURN_TRUE;
}


/*! Run a synchronous command and return a list of integer values.
    \param command   The command line interface (cli) command to execute.

    \param int_list    The integer values returned form the cli.

    \param mark_for_delete   If true delete the values in the cli's objects.

    \param mt     The maximum time to wait before command is considered
                  failed.

    \param warn_of_time  If set the user will be prompted at the timeout
                         to see if the command should contunue.
*/
bool
CLIInterface::getIntListValueFromCLI(const char *command, std::list<int64_t> *int_list, InputLineObject *clip, bool mark_value_for_delete, int mt, bool wot )
{
//  printf("getIntListValueFromCLI(%s)\n", command);
  maxTime = mt;
  warn_of_time = wot;

  QApplication::setOverrideCursor(QCursor::WaitCursor);
  nprintf(DEBUG_COMMANDS) ("getIntListValueFromCLI() command = (%s)\n", command );

#ifdef DEBUG_GUI
  printf("getIntListValueFromCLI() command = (%s)\n", command );
#endif

  clip = Append_Input_String( wid, (char *)command);

#ifdef DEBUG_GUI
  printf("clip = 0x%x\n", clip);
#endif

  if( clip == NULL )
  {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    RETURN_FALSE;
  }
  Input_Line_Status status = ILO_UNKNOWN;

  int timeout_cnt = 0;

  while( status != ILO_COMPLETE )
  {
#ifdef DEBUG_GUI
  printf("CLIInterface::getIntListValueFromCLI, status != ILO_COMPLETE , calling to checkStatus, status=%d\n", status);
#endif
    status = checkStatus(clip, command);

#ifdef DEBUG_GUI
    printf("status = %d\n", status);
#endif

    if( !status || status == ILO_ERROR )
    {   // An error occurred.... A message should have been posted.. return;
      fprintf(stderr, "an error occurred processing (%s)!\n", command);
      if( mark_value_for_delete && clip ) 
      {
        clip->Set_Results_Used();
      }
      RETURN_FALSE;
    }

    if( status == ILO_COMPLETE )
    {
#ifdef DEBUG_GUI
      printf("status = ILO_COMPLETE!\n");
#endif
      std::list<CommandObject *>::iterator coi;
#ifdef DEBUG_GUI
      printf("clip->CmdObj_List().size()=%d\n", clip->CmdObj_List().size() );
#endif

      if( clip->CmdObj_List().size() == 1 ) // We should only have one in this case.\n");
      {

#ifdef DEBUG_GUI
        printf("We have 1 command object.   Get the data.\n");
#endif

        coi = clip->CmdObj_List().begin();
        CommandObject *co = (CommandObject *)(*coi);

#ifdef DEBUG_GUI
        printf("co=0x%x\n", co);
#endif

        std::list<CommandResult *>::iterator cri;
#ifdef DEBUG_GUI
        printf("co->Result_List().size()=%d\n", co->Result_List().size() );
#endif

        std::list<CommandResult *> cmd_result = co->Result_List();

        for(cri = cmd_result.begin(); cri != cmd_result.end(); cri++ )
        {
          CommandResult_Int *cr_int = (CommandResult_Int *)(*cri);

#ifdef DEBUG_GUI
          printf("cr_int=0x%x\n", cr_int);   // This is null?
#endif

          int64_t val = 0;
          cr_int->Value(val);
          (*int_list).push_back(val);
        }

        //Allow the garbage collector to clean up the value...
        if( mark_value_for_delete && clip )
        {
          clip->Set_Results_Used();
        }
      }
      break;
    }
    
    qApp->processEvents(8000);

    if( !shouldWeContinue() )
    {

#ifdef DEBUG_GUI
      printf("RETURN FALSE!   COMMAND FAILED!\n");
#endif

      if( mark_value_for_delete && clip ) 
      {
        clip->Set_Results_Used();
      }
      RETURN_FALSE;
    }

    suspend();
  } 

  if( timer )
  {
    timer->stop();
    delete timer;
    timer = NULL;
  }

  if( mark_value_for_delete && clip ) 
  {
    clip->Set_Results_Used();
  }
  RETURN_TRUE;
}


/*! Run a synchronous command and return a string value.
    \param command   The command line interface (cli) command to execute.

    \param str_val    The string value returned form the cli.

    \param mark_for_delete   If true delete the values in the cli's objects.

    \param mt     The maximum time to wait before command is considered
                  failed.

    \param warn_of_time  If set the user will be prompted at the timeout
                         to see if the command should contunue.
*/
bool
CLIInterface::getStringValueFromCLI(const char *command, std::string *str_val, bool mark_value_for_delete, int mt, bool wot )
{

#ifdef DEBUG_GUI
  printf("getStringValueFromCLI(%s)\n", command);
#endif

  maxTime = mt;
  warn_of_time = wot;

  QApplication::setOverrideCursor(QCursor::WaitCursor);
  nprintf(DEBUG_COMMANDS) ("getStringValueFromCLI() command = (%s)\n", command );

#ifdef DEBUG_GUI
  printf("getStringValueFromCLI() command = (%s)\n", command );
#endif

  InputLineObject *clip = Append_Input_String( wid, (char *)command);
  if( clip == NULL )
  {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    RETURN_FALSE;
  }
  Input_Line_Status status = ILO_UNKNOWN;

  int timeout_cnt = 0;

  while( status != ILO_COMPLETE )
  {
#ifdef DEBUG_GUI
  printf("CLIInterface::getStringValueFromCLI, status != ILO_COMPLETE , calling to checkStatus, status=%d\n", status);
#endif
    status = checkStatus(clip, command);

#ifdef DEBUG_GUI
    printf("getStringValueFromCLI() status = %d\n", status);
#endif

    if( !status || status == ILO_ERROR )
    {   // An error occurred.... A message should have been posted.. return;
      fprintf(stderr, "an error occurred processing (%s)!\n", command);
      if( clip ) 
      {
        clip->Set_Results_Used();
      }
      RETURN_FALSE;
    }

    if( status == ILO_COMPLETE )
    {
//printf("status = ILO_COMPLETE!\n");
      std::list<CommandObject *>::iterator coi;
      if( clip->CmdObj_List().size() == 1 ) // We should only have one in this case.\n");
      {
//printf("We have 1 command object.   Get the data.\n");
        coi = clip->CmdObj_List().begin();
        CommandObject *co = (CommandObject *)(*coi);

      
        std::list<CommandResult *>::iterator crl;
        crl = co->Result_List().begin();    
        CommandResult_String *cr_str = (CommandResult_String *)(*crl);

        cr_str->Value(*str_val);

        if( mark_value_for_delete )
        {
          //Allow the garbage collector to clean up the value...
          clip->Set_Results_Used();
        }
      }
      break;
    }
    
    qApp->processEvents(8000);

    if( !shouldWeContinue() )
    {
//printf("RETURN FALSE!   COMMAND FAILED!\n");
      if( clip ) 
      {
        clip->Set_Results_Used();
      }
      RETURN_FALSE;
    }

    suspend();
  } 

  if( timer )
  {
    timer->stop();
    delete timer;
    timer = NULL;
  }

  if( clip ) 
  {
    clip->Set_Results_Used();
  }
  RETURN_TRUE;
}


/*! Run a synchronous command and return a list of string values.
    \param command   The command line interface (cli) command to execute.

    \param str_list    The string values returned form the cli.

    \param mark_for_delete   If true delete the values in the cli's objects.

    \param mt     The maximum time to wait before command is considered
                  failed.

    \param warn_of_time  If set the user will be prompted at the timeout
                         to see if the command should contunue.
*/
bool
CLIInterface::getStringListValueFromCLI(const char *command, std::list<std::string> *str_list, InputLineObject *clip, bool mark_value_for_delete, int mt, bool wot )
{

#if DEBUG_GUI
  printf("getStringListValueFromCLI(%s)\n", command);
#endif

  maxTime = mt;
  warn_of_time = wot;

  QApplication::setOverrideCursor(QCursor::WaitCursor);

nprintf(DEBUG_COMMANDS) ("getStringListValueFromCLI() command = (%s)\n", command );

  clip = Append_Input_String( wid, (char *)command);
  if( clip == NULL )
  {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    RETURN_FALSE;
  }
  Input_Line_Status status = ILO_UNKNOWN;

  int timeout_cnt = 0;

  while( status != ILO_COMPLETE )
  {
#ifdef DEBUG_GUI
  printf("CLIInterface::getStringListValueFromCLI, status != ILO_COMPLETE , calling to checkStatus, status=%d\n", status);
#endif
    status = checkStatus(clip, command);
    if( !status || status == ILO_ERROR )
    {   // An error occurred.... A message should have been posted.. return;
      fprintf(stderr, "an error occurred processing (%s)!\n", command);
      if( mark_value_for_delete && clip ) 
      {
        clip->Set_Results_Used();
      }
      RETURN_FALSE;
    }

    if( status == ILO_COMPLETE )
    {
      std::list<CommandObject *>::iterator coi;
      if( clip->CmdObj_List().size() == 1 ) // We should only have one in this case.\n");
      {
        coi = clip->CmdObj_List().begin();
        CommandObject *co = (CommandObject *)(*coi);

        std::list<CommandResult *>::iterator cri;
        std::list<CommandResult *> cmd_result = co->Result_List();
        for(cri = cmd_result.begin(); cri != cmd_result.end(); cri++ )
        {
          CommandResult_String *cr_str = (CommandResult_String *)(*cri);

#ifdef DEBUG_GUI
         printf("CLIInterface::getStringListValueFromCLI, calling Value\n");
#endif
          std::string str_val;
          cr_str->Value(str_val);
          (*str_list).push_back(str_val);
        }

        if( mark_value_for_delete && clip )
        {
          //Allow the garbage collector to clean up the value...
          clip->Set_Results_Used();
        }
      }
      break;
    }
    
    qApp->processEvents(8000);

    if( !shouldWeContinue() )
    {
//printf("RETURN FALSE!   COMMAND FAILED!\n");
      if( mark_value_for_delete && clip ) 
      {
        clip->Set_Results_Used();
      }
      RETURN_FALSE;
    }

    suspend();

  } 

  if( timer )
  {
    timer->stop();
    delete timer;
    timer = NULL;
  }

  if( mark_value_for_delete && clip ) 
  {
    clip->Set_Results_Used();
  }
  RETURN_TRUE;
}

/*! This routine simply checks the status of the command 
    in the cli's processing.
*/
Input_Line_Status
CLIInterface::checkStatus(InputLineObject *clip, const char *command)
{

#ifdef DEBUG_GUI
  printf("ENTER CLIInterface::checkStatus() command=(%s)\n", command);
#endif

  Input_Line_Status status = clip->What();

#ifdef DEBUG_GUI
  printf("CLIInterface::checkStatus() status=(%d)\n", status);
  printf("CLIInterface::checkStatus() ILO_QUEUED_INPUT=(%d)\n", ILO_QUEUED_INPUT);
  printf("CLIInterface::checkStatus() ILO_IN_PARSER=(%d)\n", ILO_IN_PARSER);
  printf("CLIInterface::checkStatus() ILO_EXECUTING=(%d)\n", ILO_EXECUTING);
  printf("CLIInterface::checkStatus() ILO_COMPLETE=(%d)\n", ILO_COMPLETE);
  printf("CLIInterface::checkStatus() ILO_ERROR=(%d)\n", ILO_ERROR);
#endif

  switch( status )
  {
    case ILO_QUEUED_INPUT:
#ifdef DEBUG_GUI
      printf("command queued for processing.\n");
#endif
      break;
    case ILO_IN_PARSER:
#ifdef DEBUG_GUI
      printf("command queued for parsing.\n");
#endif
      break;
    case ILO_EXECUTING:
#ifdef DEBUG_GUI
      printf("command is executing.\n");
#endif
      break;
    case ILO_COMPLETE:
#ifdef DEBUG_GUI
      printf("command has sucessfully completed.\n");
#endif
      break;
    case ILO_ERROR:
#ifdef DEBUG_GUI
      fprintf(stderr, "Unable to process the clip.   Error encountered.\n");
#endif

      // Now put out whatever error message there might be.
      {
        std::list<CommandObject *>::iterator coi;
        if( clip->CmdObj_List().size() == 1 )
        {
          coi = clip->CmdObj_List().begin();
          CommandObject *co = (CommandObject *)(*coi);

          std::list<CommandResult *>::iterator cri;
          std::list<CommandResult *> cmd_result = co->Result_List();
          QString info_str = QString::null;
          for(cri = cmd_result.begin(); cri != cmd_result.end(); cri++ ) {
            CommandResult_String *cr_str = (CommandResult_String *)(*cri);

            std::string str_val;
            cr_str->Value(str_val);
            info_str += str_val.c_str();
          }

          if( QString(command) == "" ) {
            QMessageBox::information( NULL, "Command Failure Information", info_str, QMessageBox::Ok );
          } else {
            QMessageBox::information( NULL, "Command Failure Information", QString("Command: %1\n").arg(command)+info_str, QMessageBox::Ok );
          }

          //Allow the garbage collector to clean up the value...
          clip->Set_Results_Used();
        }
      }
      // Remove the timer.  This return status should abort the command...
      if( timer ) {
        timer->stop();
        delete timer;
        timer = NULL;
      }
      break;
    default:
      fprintf(stderr, "Unknown status (%d) return in clip.\n", status);
      break;
  }

#ifdef DEBUG_GUI
  printf("Exit checkStatus, status=%d\n", status);
#endif
  return status;
}

/*! A simple routine to check the command status and the interrupt flag.
  
    If all is clear to continue it returns true.
*/
bool 
CLIInterface::shouldWeContinue()
{
  // Set an abort timer if the caller asked to set one up...
  if( maxTime > 0 && warn_of_time == TRUE )
  {
    if( timer == NULL )
    {
//printf("shouldWeContinue() set the timer!\n");
      timer = new QTimer(this, "runSynchronousCLITimer");
      connect( timer, SIGNAL(timeout()), this, SLOT(wakeupFromTimer()) );
      timer->start(maxTime, TRUE);
    }
  }
  if( CLIInterface::interrupt )
  {
    // First cancel the timer..
    if( timer )
    {
      timer->stop();
      delete timer;
      timer = NULL;
    }

    // Now cancel the command(s)

    // Also cancel the interrupt;
//    setInterrupt(false);
    CLIInterface::interrupt = false;
  
//printf("stop!!!\n");
    return false;
  }

//printf("continue!!!\n");
  return true;
}

/*! This routine is called when the timer is fired.

    If the user had set the warn_of_time setting they will be prompted
    before returning.   Otherwise the command is aborted.
*/
void
CLIInterface::wakeupFromTimer()
{
//printf("wakeupFromTimer() entered  \n");
  if( timer )
  {
    timer->stop();
    delete timer;
    timer = NULL;
  }

  if( warn_of_time )
  {
    bool ok;
    int res = QInputDialog::getInteger(
              "Unable to complete command before timeout?", "Enter milleseconds before next timeout: (Hit cancel to abort command.)", maxTime, 0, 100000, 1000, &ok);
  if ( ok ) 
    {
      // user entered something and pressed OK
      maxTime = res;
    } else
    {
      // user entered nothing or pressed Cancel
//      setInterrupt(true);
      CLIInterface::interrupt = true;
    }
  } else
  {
    // No warning required.
//    setInterrupt(true);
    CLIInterface::interrupt = true;
  }
}
