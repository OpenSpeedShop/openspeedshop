#ifndef CLIINTERFACE_H
#define CLIINTERFACE_H

#include "SS_Input_Manager.hxx"

class CLIInterface
{
  public:
    CLIInterface(int _wid);

    ~CLIInterface();

    int wid;

    int getIntValueFromCLI(char *command, int64_t *val, bool mark_value_for_delete );

};

#endif // CLIINTERFACE_H
