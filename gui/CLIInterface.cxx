#include "CLIInterface.hxx"

#include "qapplication.h"

#include <qinputdialog.h>

CLIInterface::CLIInterface(int _wid) : QObject()
{
  wid = _wid;
  setInterrupt(false);
  timer = NULL;
}

CLIInterface::~CLIInterface()
{
}

bool
CLIInterface::runSynchronousCLI(char *command, int mt, bool warn_of_time )
{
  maxTime = mt;
printf("runSynchronousCLI(%s)\n", command);
  InputLineObject *clip = Append_Input_String( wid, command);
  if( clip == NULL )
  {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    return false;
  }
  Input_Line_Status status = ILO_UNKNOWN;

  while( status != ILO_COMPLETE )
  {
    status = checkStatus(clip);
    if( !status )
    { // An error occurred.... A message should have been posted.. return;
       return false;
    }

    qApp->processEvents(1000);

    if( !shouldWeContinue() )
    {
// printf("RETURN FALSE!   COMMAND FAILED!\n");
      return false;
    }

    sleep(1);
  }

  if( timer )
  {
    timer->stop();
    delete timer;
    timer = NULL;
  }

  return(true);
}

bool
CLIInterface::getIntValueFromCLI(char *command, int64_t *val, bool mark_value_for_delete, int mt, bool warn_of_time )
{
printf("getIntValueFromCLI(%s)\n", command);
  maxTime = mt;

  InputLineObject *clip = Append_Input_String( wid, command);
  if( clip == NULL )
  {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    return false;
  }
  Input_Line_Status status = ILO_UNKNOWN;

  int timeout_cnt = 0;

  while( status != ILO_COMPLETE )
  {
    status = checkStatus(clip);
// printf("status = %d\n", status);
    if( !status )
    {   // An error occurred.... A message should have been posted.. return;
      fprintf(stderr, "an error occurred processing (%s)!\n", command);
      return false;
    }

    if( status == ILO_COMPLETE )
    {
// printf("status = ILO_COMPLETE!\n");
      std::list<CommandObject *>::iterator coi;
      if( clip->CmdObj_List().size() == 1 ) // We should only have one in this case.\n");
      {
// printf("We have 1 command object.   Get the data.\n");
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
// printf("RETURN FALSE!   COMMAND FAILED!\n");
      return false;
    }

    sleep(1);
  } 

  if( timer )
  {
    timer->stop();
    delete timer;
    timer = NULL;
  }

  return(true);
}


bool
CLIInterface::getIntListValueFromCLI(char *command, std::list<int64_t> *int_list, bool mark_value_for_delete, int mt, bool warn_of_time )
{
printf("getIntListValueFromCLI(%s)\n", command);
  maxTime = mt;

  InputLineObject *clip = Append_Input_String( wid, command);
// printf("clip = 0x%x\n", clip);
  if( clip == NULL )
  {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    return false;
  }
  Input_Line_Status status = ILO_UNKNOWN;

  int timeout_cnt = 0;

  while( status != ILO_COMPLETE )
  {
    status = checkStatus(clip);
// printf("status = %d\n", status);
    if( !status )
    {   // An error occurred.... A message should have been posted.. return;
      fprintf(stderr, "an error occurred processing (%s)!\n", command);
      return false;
    }

    if( status == ILO_COMPLETE )
    {
// printf("status = ILO_COMPLETE!\n");
      std::list<CommandObject *>::iterator coi;
// printf("clip->CmdObj_List().size()=%d\n", clip->CmdObj_List().size() );
      if( clip->CmdObj_List().size() == 1 ) // We should only have one in this case.\n");
      {
// printf("We have 1 command object.   Get the data.\n");
        coi = clip->CmdObj_List().begin();
        CommandObject *co = (CommandObject *)(*coi);
// printf("co=0x%x\n", co);

        std::list<CommandResult *>::iterator cri;
// printf("co->Result_List().size()=%d\n", co->Result_List().size() );
std::list<CommandResult *> cmd_result = co->Result_List();
        for(cri = cmd_result.begin(); cri != cmd_result.end(); cri++ )
        {
          CommandResult_Int *cr_int = (CommandResult_Int *)(*cri);
// printf("cr_int=0x%x\n", cr_int);   // This is null?

          int64_t val = 0;
          cr_int->Value(&val);
          (*int_list).push_back(val);
// printf("Down here in ditch digging land.... val=(%d)\n", val);
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
// printf("RETURN FALSE!   COMMAND FAILED!\n");
      return false;
    }

    sleep(1);
  } 

  if( timer )
  {
    timer->stop();
    delete timer;
    timer = NULL;
  }

  return(true);
}

Input_Line_Status
CLIInterface::checkStatus(InputLineObject *clip)
{
  Input_Line_Status status = clip->What();

  switch( status )
  {
    case ILO_QUEUED_INPUT:
//      printf("command queued for processing.\n");
      break;
    case ILO_IN_PARSER:
//      printf("command queued for parsing.\n");
      break;
    case ILO_EXECUTING:
//      printf("command is executing.\n");
      break;
    case ILO_COMPLETE:
//      printf("command has sucessfully completed.\n");
      break;
    case ILO_ERROR:
//      fprintf(stderr, "Unable to process the clip.   Error encountered.\n");
      break;
    default:
//      fprintf(stderr, "Unknown status (%d) return in clip.\n", status);
      break;
  }

  return status;
}

bool 
CLIInterface::shouldWeContinue()
{
  if( maxTime > 0 )
  {
    if( timer == NULL )
    {
// printf("shouldWeContinue() set the timer!\n");
      timer = new QTimer(this, "runSynchronousCLITimer");
      connect( timer, SIGNAL(timeout()), this, SLOT(wakeupFromTimer()) );
      timer->start(maxTime, TRUE);
    }
  }
  if( getInterrupt() )
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
    setInterrupt(false);
  
// printf("stop!!!\n");
    return false;
  }

// printf("continue!!!\n");
  return true;
}

void
CLIInterface::wakeupFromTimer()
{
// printf("wakeupFromTimer() entered  \n");
  if( timer )
  {
    timer->stop();
    delete timer;
    timer = NULL;
  }

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
    setInterrupt(true);
  }
}
