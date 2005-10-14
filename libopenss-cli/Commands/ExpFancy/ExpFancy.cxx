#include "ExpFancy.hxx"
#include "cli_plugin_entry_point.hxx"

ExpFancy::ExpFancy() : CmdClass()
{
  cmd_name = "ExpFancy";
  printf("Just created the class ExpFancy(%s)\n", cmd_name.c_str());
}

ExpFancy::~ExpFancy()
{
}

void
ExpFancy::parseCmd(const char *command)
{
printf("  ExpFancy::parseCmd(%s) entered\n", command);
}

void
ExpFancy::doCmd(const char *command)
{
printf("  ExpFancy::doCmd(%s) entered\n", command);
}
