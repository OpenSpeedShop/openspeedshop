////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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

#include "qcursor.h"
#include "qapplication.h"

#include <qinputdialog.h>

bool CLIInterface::interrupt = false;
#define RETURN_FALSE QApplication::restoreOverrideCursor();return false;
#define RETURN_TRUE QApplication::restoreOverrideCursor();return true;

/*! Given a window id create an interface to the cli. */
CLIInterface::CLIInterface(int _wid) : QObject()
{
  wid = _wid;
  CLIInterface::interrupt = false;
  timer = NULL;
}

CLIInterface::~CLIInterface()
{
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
//  printf("runSynchronousCLI(%s)\n", command);
  QApplication::setOverrideCursor(QCursor::WaitCursor);
  InputLineObject *clip = Append_Input_String( wid, (char *)command);
  if( clip == NULL )
  {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    RETURN_FALSE;
  }
  Input_Line_Status status = ILO_UNKNOWN;

  while( status != ILO_COMPLETE )
  {
    status = checkStatus(clip);
    if( !status || status == ILO_ERROR )
    { // An error occurred.... A message should have been posted.. return;
       RETURN_FALSE;
    }

    qApp->processEvents(1000);

    if( !shouldWeContinue() )
    {
//printf("RETURN FALSE!   COMMAND FAILED!\n");
      RETURN_FALSE;
    }

    sleep(1);
  }

  if( timer )
  {
    timer->stop();
    delete timer;
    timer = NULL;
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
//  printf("getIntValueFromCLI(%s)\n", command);
  maxTime = mt;
  warn_of_time = wot;

  QApplication::setOverrideCursor(QCursor::WaitCursor);
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
    status = checkStatus(clip);
//printf("status = %d\n", status);
    if( !status || status == ILO_ERROR )
    {   // An error occurred.... A message should have been posted.. return;
      fprintf(stderr, "an error occurred processing (%s)!\n", command);
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
        CommandResult_Int *cr_int = (CommandResult_Int *)(*crl);

        cr_int->Value(val);

        if( mark_value_for_delete )
        {
          //Allow the garbage collector to clean up the value...
          clip->Set_Results_Used();
        }
      }
      break;
    }
    
    qApp->processEvents(1000);

    if( !shouldWeContinue() )
    {
      //printf("RETURN FALSE!   COMMAND FAILED!\n");
      RETURN_FALSE;
    }

    sleep(1);
  } 

  if( timer )
  {
    timer->stop();
    delete timer;
    timer = NULL;
  }

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
CLIInterface::getIntListValueFromCLI(const char *command, std::list<int64_t> *int_list, bool mark_value_for_delete, int mt, bool wot )
{
//  printf("getIntListValueFromCLI(%s)\n", command);
  maxTime = mt;
  warn_of_time = wot;

  QApplication::setOverrideCursor(QCursor::WaitCursor);
  InputLineObject *clip = Append_Input_String( wid, (char *)command);
//printf("clip = 0x%x\n", clip);
  if( clip == NULL )
  {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    RETURN_FALSE;
  }
  Input_Line_Status status = ILO_UNKNOWN;

  int timeout_cnt = 0;

  while( status != ILO_COMPLETE )
  {
    status = checkStatus(clip);
//printf("status = %d\n", status);
    if( !status || status == ILO_ERROR )
    {   // An error occurred.... A message should have been posted.. return;
      fprintf(stderr, "an error occurred processing (%s)!\n", command);
      RETURN_FALSE;
    }

    if( status == ILO_COMPLETE )
    {
//printf("status = ILO_COMPLETE!\n");
      std::list<CommandObject *>::iterator coi;
//printf("clip->CmdObj_List().size()=%d\n", clip->CmdObj_List().size() );
      if( clip->CmdObj_List().size() == 1 ) // We should only have one in this case.\n");
      {
//printf("We have 1 command object.   Get the data.\n");
        coi = clip->CmdObj_List().begin();
        CommandObject *co = (CommandObject *)(*coi);
//printf("co=0x%x\n", co);

        std::list<CommandResult *>::iterator cri;
//printf("co->Result_List().size()=%d\n", co->Result_List().size() );
std::list<CommandResult *> cmd_result = co->Result_List();
        for(cri = cmd_result.begin(); cri != cmd_result.end(); cri++ )
        {
          CommandResult_Int *cr_int = (CommandResult_Int *)(*cri);
//printf("cr_int=0x%x\n", cr_int);   // This is null?

          int64_t val = 0;
          cr_int->Value(&val);
          (*int_list).push_back(val);
        }

        if( mark_value_for_delete )
        {
          //Allow the garbage collector to clean up the value...
          clip->Set_Results_Used();
        }
      }
      break;
    }
    
    qApp->processEvents(1000);

    if( !shouldWeContinue() )
    {
//printf("RETURN FALSE!   COMMAND FAILED!\n");
      RETURN_FALSE;
    }

    sleep(1);
  } 

  if( timer )
  {
    timer->stop();
    delete timer;
    timer = NULL;
  }

  RETURN_TRUE;
}


/*! Run a synchronous command and return a string value.
    \param command   The command line interface (cli) command to execute.

    \param str_val    The string values returned form the cli.

    \param mark_for_delete   If true delete the values in the cli's objects.

    \param mt     The maximum time to wait before command is considered
                  failed.

    \param warn_of_time  If set the user will be prompted at the timeout
                         to see if the command should contunue.
*/
bool
CLIInterface::getStringValueFromCLI(const char *command, std::string *str_val, bool mark_value_for_delete, int mt, bool wot )
{
//  printf("getStringValueFromCLI(%s)\n", command);
  maxTime = mt;
  warn_of_time = wot;

  QApplication::setOverrideCursor(QCursor::WaitCursor);
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
    status = checkStatus(clip);
//printf("status = %d\n", status);
    if( !status || status == ILO_ERROR )
    {   // An error occurred.... A message should have been posted.. return;
      fprintf(stderr, "an error occurred processing (%s)!\n", command);
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

        cr_str->Value(str_val);

        if( mark_value_for_delete )
        {
          //Allow the garbage collector to clean up the value...
          clip->Set_Results_Used();
        }
      }
      break;
    }
    
    qApp->processEvents(1000);

    if( !shouldWeContinue() )
    {
//printf("RETURN FALSE!   COMMAND FAILED!\n");
      RETURN_FALSE;
    }

    sleep(1);
  } 

  if( timer )
  {
    timer->stop();
    delete timer;
    timer = NULL;
  }

  RETURN_TRUE;
}


/*! Run a synchronous command and return a list of string values.
    \param command   The command line interface (cli) command to execute.

    \param int_list    The integer values returned form the cli.

    \param mark_for_delete   If true delete the values in the cli's objects.

    \param mt     The maximum time to wait before command is considered
                  failed.

    \param warn_of_time  If set the user will be prompted at the timeout
                         to see if the command should contunue.
*/
bool
CLIInterface::getStringListValueFromCLI(const char *command, std::list<std::string> *str_list, bool mark_value_for_delete, int mt, bool wot )
{
//  printf("getStringListValueFromCLI(%s)\n", command);
  maxTime = mt;
  warn_of_time = wot;

  QApplication::setOverrideCursor(QCursor::WaitCursor);
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
    status = checkStatus(clip);
    if( !status || status == ILO_ERROR )
    {   // An error occurred.... A message should have been posted.. return;
      fprintf(stderr, "an error occurred processing (%s)!\n", command);
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

          std::string str_val;
          cr_str->Value(&str_val);
          (*str_list).push_back(str_val);
        }

        if( mark_value_for_delete )
        {
          //Allow the garbage collector to clean up the value...
          clip->Set_Results_Used();
        }
      }
      break;
    }
    
    qApp->processEvents(1000);

    if( !shouldWeContinue() )
    {
printf("RETURN FALSE!   COMMAND FAILED!\n");
      RETURN_FALSE;
    }

    sleep(1);
  } 

  if( timer )
  {
    timer->stop();
    delete timer;
    timer = NULL;
  }

  RETURN_TRUE;
}

/*! This routine simply checks the status of the command 
    in the cli's processing.
*/
Input_Line_Status
CLIInterface::checkStatus(InputLineObject *clip)
{
  Input_Line_Status status = clip->What();

  switch( status )
  {
    case ILO_QUEUED_INPUT:
//printf("command queued for processing.\n");
      break;
    case ILO_IN_PARSER:
//printf("command queued for parsing.\n");
      break;
    case ILO_EXECUTING:
//printf("command is executing.\n");
      break;
    case ILO_COMPLETE:
//printf("command has sucessfully completed.\n");
      break;
    case ILO_ERROR:
      fprintf(stderr, "Unable to process the clip.   Error encountered.\n");
      // Remove the timer.  This return status should abort the command...
      if( timer )
      {
        timer->stop();
        delete timer;
        timer = NULL;
      }
      break;
    default:
      fprintf(stderr, "Unknown status (%d) return in clip.\n", status);
      break;
  }

  return status;
}

/*! A simple routine to check the command status and the interrupt flag.
  
    If all is clear to continue it returns true.
*/
bool 
CLIInterface::shouldWeContinue()
{
  if( maxTime > 0 )
  {
    if( timer == NULL )
    {
//printf("shouldWeContinue() set the timer!\n");
      timer = new QTimer(this, "runSynchronousCLITimer");
      connect( timer, SIGNAL(timeout()), this, SLOT(wakeupFromTimer()) );
      timer->start(maxTime, TRUE);
    }
  }
//  if( getInterrupt() )
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
