#include "frap.hxx"
#include "cli_plugin_entry_point.hxx"

#include "SS_Input_Manager.hxx"

frap::frap() : CmdClass()
{
  cmd_name = "list";
//  printf("Just created the class frap(%s)\n", cmd_name.c_str());
}

frap::~frap()
{
}

void
frap::parseCmd(const char *command)
{
printf("  frap::parseCmd(%s) entered\n", command);
}

void Default_TLI_Command_Output(CommandObject *C)
{
  printf("CmdPanel::callback(well sort of) entered\n");
}

void Default_TLI_Line_Output( InputLineObject *clip)
{
  printf("Default_TLI_Line_Output() entered\n");
}


void
frap::doCmd(const char *_command)
{
printf("  frap::doCmd(%s) entered\n", _command);

 const char *argument = _command + cmd_name.size() + 1;

// char *command = "listPids";
 std::string command_argument(argument);


 char *command = NULL;
 
 if( command_argument == "pids" || command_argument == "p" )
 {
   command = "listPids";
 } else if( command_argument == "types" || command_argument == "v" )
 {
   command = "listView";
 } else if( command_argument == "experiments" || command_argument == "exp" ||
            command_argument == "expr" || command_argument == "e" )
 {
   command = "listExp";
 } else if( command_argument == "types" || command_argument == "t" )
 {
   command = "listTypes";
 } else if( command_argument == "status" || command_argument == "stats" ||
            command_argument == "s" )
 {
   command = "expStatus";
 } else
 {
   printf("Unknown argument : usage 'list list-type'\n");
   return;
 }
 
 InputLineObject *clip = Append_Input_String( 1, (char *)command, NULL, &Default_TLI_Line_Output, &Default_TLI_Command_Output);
// InputLineObject *clip = Append_Input_String( 2, (char *)command, NULL, &Default_TLI_Line_Output, &Default_TLI_Command_Output);


}
