#include "CmdClass.hxx"

CmdClass::CmdClass()
{
  cmd_name = "CmdClass";
}

CmdClass::~CmdClass()
{
}

void
CmdClass::parseCmd(const char *command)
{
printf("  CmdClass::parseCmd(%s) entered\n", command);
}

void
CmdClass::doCmd(const char *command)
{
printf("  CmdClass::doCmd(%s) entered\n", command);
}

