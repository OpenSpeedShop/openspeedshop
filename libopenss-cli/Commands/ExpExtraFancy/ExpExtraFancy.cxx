#include "ExpExtraFancy.hxx"
#include "cli_plugin_entry_point.hxx"

#include "SS_Input_Manager.hxx"

ExpExtraFancy::ExpExtraFancy() : CmdClass()
{
  cmd_name = "ExpExtraFancy";
//  printf("Just created the class ExpExtraFancy(%s)\n", cmd_name.c_str());
}

ExpExtraFancy::~ExpExtraFancy()
{
}

void
ExpExtraFancy::parseCmd(const char *command)
{
printf("  ExpExtraFancy::parseCmd(%s) entered\n", command);
}

void
ExpExtraFancy::doCmd(const char *command)
{
printf("  ExpExtraFancy::doCmd(%s) entered\n", command);

  int expID = 1;
  try
  {
    std::set<Statement>::const_iterator di = NULL;

    ExperimentObject *eo = Find_Experiment_Object((EXPID)expID);
 

    if( eo == NULL )
    {
      fprintf(stderr, "No experiment yet defined.\n");
      return;
    }
    Experiment *fw_experiment = eo->FW();
    ThreadGroup tgrp = fw_experiment->getThreads();
    ThreadGroup::iterator ti = tgrp.begin();
    printf("You have %d threads defined.\n", tgrp.size() );
    if( tgrp.size() == 0 )
    { // No threads to look up the data...
       return;
    }
    

  }
  catch(const std::exception& error)
  {
    std::cerr << std::endl << "Error: "
              << (((error.what() == NULL) || (strlen(error.what()) == 0)) ?
              "Unknown runtime error." : error.what()) << std::endl
              << std::endl;
    return;
  }


 
}
