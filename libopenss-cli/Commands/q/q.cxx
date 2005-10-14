#include "q.hxx"
#include "cli_plugin_entry_point.hxx"
#include "SS_Input_Manager.hxx"

q::q() : CmdClass()
{
  cmd_name = "q";
  printf("Just created the class q(%s)\n", cmd_name.c_str());
}

q::~q()
{
}

void
q::parseCmd(const char *command)
{
printf("  q::parseCmd(%s) entered\n", command);
}

#include "Commander.hxx"
void
q::doCmd(const char *_command)
{
printf("  q::doCmd(%s) entered\n", _command);

 char *command = "exit";
 InputLineObject *clip = Append_Input_String( 1, (char *)command);
}
