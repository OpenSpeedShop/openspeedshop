
#include "CLIInterface.hxx"

CLIInterface::CLIInterface(int _wid)
{
  wid = _wid;
}

CLIInterface::~CLIInterface()
{
}

int
CLIInterface::getIntValueFromCLI(char *command, int64_t *val, bool mark_value_for_delete)
{
printf("NEW CLIInterface!\n");
  InputLineObject *clip = Append_Input_String( wid, command);
  if( clip == NULL )
  {
    fprintf(stderr, "FATAL ERROR: No clip returned from cli.\n");
    return false;
  }
  Input_Line_Status status = clip->What();

  while( true )
  {
    switch( status )
    {
      case ILO_QUEUED_INPUT:
        printf("command queued for processing.\n");
        break;
      case ILO_IN_PARSER:
        printf("command queued for parsing.\n");
        break;
      case ILO_EXECUTING:
        printf("command is executing.\n");
        break;
      case ILO_COMPLETE:
        printf("command has sucessfully completed.\n");
        break;
      case ILO_ERROR:
        fprintf(stderr, "Unable to process the clip.   Error encountered.\n");
        return false;
      default:
        fprintf(stderr, "Unknown status (%d) return in clip.\n", status);
        return false;
    }

    if( status == ILO_COMPLETE )
    {
      std::list<CommandObject *>::iterator coi;
      if( clip->CmdObj_List().size() == 1 ) // We should only have one in this case.\n");
      {
        printf("We have 1 command object.   Get the data.\n");
        coi = clip->CmdObj_List().begin();
        CommandObject *co = (CommandObject *)(*coi);

      
        std::list<CommandResult *>::iterator crl;
        crl = co->Result_List().begin();    
        CommandResult_Int *cri = (CommandResult_Int *)(*crl);

//        int64_t val = -1;
        cri->Value(val);

        if( mark_value_for_delete )
        {
          //Allow the garbage collector to clean up the value...
          clip->Set_Results_Used();
        }
      }
      break;
    }
    
    sleep(1);
    status = clip->What();
  }

  return(true);
}
