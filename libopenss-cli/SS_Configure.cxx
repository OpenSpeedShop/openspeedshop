/******************************************************************************e
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007, 2008 Krell Institute  All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

#include "SS_Input_Manager.hxx"
#include "SS_Settings.hxx"
// where is this include????    #include <libxml/xmlreader.h>

int64_t OPENSS_VIEW_FIELD_SIZE = 20;
int64_t OPENSS_VIEW_PRECISION = 6;
int64_t OPENSS_HISTORY_LIMIT = 100;
int64_t OPENSS_HISTORY_DEFAULT = 24;
int64_t OPENSS_MAX_ASYNC_COMMANDS = 20;
int64_t OPENSS_HELP_LEVEL_DEFAULT = 1;
bool    OPENSS_VIEW_FULLPATH = false;
bool    OPENSS_VIEW_DEFINING_LOCATION = true;
bool    OPENSS_VIEW_MANGLED_NAME = false;
bool    OPENSS_SAVE_EXPERIMENT_DATABASE = false;
bool    OPENSS_ASK_ABOUT_CHANGING_ARGS = true;
bool    OPENSS_ASK_ABOUT_SAVING_THE_DATABASE = true;
bool    OPENSS_ON_RERUN_SAVE_COPY_OF_EXPERIMENT_DATABASE = true;
bool    OPENSS_ALLOW_PYTHON_COMMANDS = true;
bool    OPENSS_LOG_BY_DEFAULT = false;
bool    OPENSS_LIMIT_SIGNAL_CATCHING = false;
bool    OPENSS_INSTRUMENTOR_IS_OFFLINE = true;
bool    OPENSS_LESS_RESTRICTIVE_COMPARISONS = false;

static inline void set_int64 (int64_t &env, std::string envName) {
  char *S = getenv (envName.c_str());
  if (S != NULL) {
    int64_t V64;
    if ((sscanf ( S, "%lld", &V64 )) &&
        (V64 > 0)) {
      env = V64;
    }
  }
}

static inline void set_bool (bool &env, std::string envName) {
  char *S = getenv (envName.c_str());
  if (S != NULL) {
    if (strcasecmp (S, "true") == 0) {
      env = true;
    } else if (strcasecmp (S, "false") == 0) {
      env = false;
    }
  }
}

static void Add_Help (SS_Message_Czar& czar,
                      std::string name,
                      std::string type_info,
                      std::string description) {
  SS_Message_Element element;
  element.set_keyword (name, "preferences");
  std::string s = type_info + " variable.";
  element.set_brief (s);
  s = s + "\n\n" + description;
  element.add_normal (s);
  element.add_verbose (s);
  czar.Add_Help(element);
}

void SS_Configure () {
  SS_Message_Czar& czar = theMessageCzar();
  bool ok;

 // Open the prefence data base
  SS_Settings *settings = new SS_Settings();

 // Containers to hold results of preference reads
  int64_t Ivalue;
  double Fvalue;
  bool Bvalue;
  std::string Svalue;

  Add_Help (czar, "DpcldListenerPort", "an internal configuration",
            "When DPCL is initalized, it determines which port to listen on "
            "for manually started daemons.  The string is in the form "
            "'host_name:port_number' and will be returned when the variable "
            "name is entered.  It must be used if the daemons are started by "
            "the user.  In most environments, the daemons are installed in "
            "system libraries and do not need to be manually started.");

  Add_Help (czar, "viewFieldSize", "an integer, preference",
            "Define the width of each field when the result "
            "of an 'expView' command is printed.  The default is 20 columns.");
  Ivalue = settings->readNumEntry(std::string("viewFieldSize"), OPENSS_VIEW_FIELD_SIZE, &ok);
  if (ok && (Ivalue >= 0)) OPENSS_VIEW_FIELD_SIZE = Ivalue;

  Add_Help (czar, "viewPrecision", "an integer, preference",
            "Define the precision used to format a floating point number when "
            "the result of an 'expView' command is printed.  The default is 6.");
  Ivalue = settings->readNumEntry(std::string("viewPrecision"), OPENSS_VIEW_PRECISION, &ok);
  if (ok && (Ivalue >= 0)) OPENSS_VIEW_PRECISION = Ivalue;

  Add_Help (czar, "historyLimit", "an integer, preference",
            "Define the maximum number of commands that are remembered for the "
            "'history' command.  If the command is issued with a larger number, "
            "this limit is automatically increased.  The default is 100.");
  Ivalue = settings->readNumEntry(std::string("historyLimit"), OPENSS_HISTORY_LIMIT, &ok);
  if (ok && (Ivalue >= 0)) OPENSS_HISTORY_LIMIT = Ivalue;

  Add_Help (czar, "historyDefault", "an integer, preference",
            "Define the number of previous commands that will be printed when "
            "the 'history' command is issued without a requesting length. The "
            "default is 24.");
  Ivalue = settings->readNumEntry(std::string("historyDefault"), OPENSS_HISTORY_DEFAULT, &ok);
  if (ok && (Ivalue >= 0)) OPENSS_HISTORY_DEFAULT = Ivalue;

  Add_Help (czar, "maxAsyncCommands", "an integer, preference",
            "Define the maximum number of commands that can be processed at "
            "the same time. This is a limit on the parallel execution of "
            "commands in OpenSS and controls the degree to which commands "
            "can be overlapped.  The default is 20.");
  Ivalue = settings->readNumEntry(std::string("maxAsyncCommands"), OPENSS_MAX_ASYNC_COMMANDS, &ok);
  if (ok && (Ivalue > 0)) OPENSS_MAX_ASYNC_COMMANDS = Ivalue;

  Add_Help (czar, "helpLevelDefault", "an integer, preference",
            "Define the level of help information that is displayed when "
            "the 'help' command is issued without a <verbosity_list_spec>. "
            "The valid integer values correspond to the folowing:"
            "\n  '0' == '-v brief'"
            "\n  '1' == '-v normal'"
            "\n  '2' == '-v detailed'"
            "\nThe default is level 1, 'normal'.");
  Ivalue = settings->readNumEntry(std::string("helpLevelDefault"), OPENSS_HELP_LEVEL_DEFAULT, &ok);
  if (ok && (Ivalue >= 0)) OPENSS_HELP_LEVEL_DEFAULT = Ivalue;


  Add_Help (czar, "viewFullPath", "a boolean, preference",
            "Declare whether or not a full path is displayed in place of "
            "a file name when the function, linkedobject, or statement "
            "location is displayed as part of an 'expView' command.  The "
            "default is false, allowing only the containing file name to "
            "be displayed.");
  Bvalue = settings->readBoolEntry(std::string("viewFullPath"), OPENSS_VIEW_FULLPATH, &ok);
  if (ok) OPENSS_VIEW_FULLPATH = Bvalue;

  Add_Help (czar, "viewDefiningLocation", "a boolean, preference",
            "Declare whether or not the defining location is displayed "
            "when a function name is displayed as part of an 'expView' "
            "command.  The default is true");
  Bvalue = settings->readBoolEntry(std::string("viewDefiningLocation"), OPENSS_VIEW_DEFINING_LOCATION, &ok);
  if (ok) OPENSS_VIEW_DEFINING_LOCATION = Bvalue;

  Add_Help (czar, "viewMangledName", "a boolean, preference",
            "Declare whether or not a mangled name is displayed "
            "when a function is displayed as part of an 'expView' command. "
            "The default is false, allowing the demangled names, that are "
            "used in the source code, to be displayed.");
  Bvalue = settings->readBoolEntry(std::string("viewMangledName"), OPENSS_VIEW_MANGLED_NAME, &ok);
  if (ok) OPENSS_VIEW_MANGLED_NAME = Bvalue;

  Add_Help (czar, "saveExperimentDatabase", "a boolean, preference",
            "Declare that the database created when an 'expCreate' "
            "command is issued will be saved when the OpenSS session is "
            "terminated.  The saved database will be in the user's "
            "current directory and will be of the form: "
            " 'X<exp_id>_iiiiii.openss'  "
            "where the 'iiiiii' field is the first integer, starting with 0, "
            "that generates a unique file name.  The default is 'false' "
            "and experiment databases will be deleted when the OpenSS "
            "session terminates unless the user has issued an 'expSave' "
            "command.");
  Bvalue = settings->readBoolEntry(std::string("saveExperimentDatabase"), OPENSS_SAVE_EXPERIMENT_DATABASE, &ok);
  if (ok) OPENSS_SAVE_EXPERIMENT_DATABASE = Bvalue;

  Add_Help (czar, "askAboutSavingTheDatabase", "a boolean, preference",
            "Declare that when rerunning an experiment create a dialog"
            " message that allows the experiment database file to be saved "
            "prior to reexecuting the experiment.");
  Bvalue = settings->readBoolEntry(std::string("askAboutSavingTheDatabase"), OPENSS_ASK_ABOUT_SAVING_THE_DATABASE, &ok);
  if (ok) OPENSS_ASK_ABOUT_SAVING_THE_DATABASE = Bvalue;

  Add_Help (czar, "askAboutChangingArgs", "a boolean, preference",
            "Declare that when rerunning an experiment create a dialog"
            " message that allows the application's arguments to be changed prior "
            "to reexecuting the experiment.");
  Bvalue = settings->readBoolEntry(std::string("askAboutChangingArgs"), OPENSS_ASK_ABOUT_CHANGING_ARGS, &ok);
  if (ok) OPENSS_ASK_ABOUT_CHANGING_ARGS = Bvalue;

  Add_Help (czar, "onRerunSaveCopyOfExperimentDatabase", "a boolean, preference",
            "Declare that the database created when an 'expCreate' "
            "command is issued will be saved when the OpenSS session is "
            "terminated.  The saved database will be in the user's "
            "current directory and will be of the form: "
            " 'X<exp_id>_iiiiii.openss'  "
            "where the 'iiiiii' field is the first integer, starting with 0, "
            "that generates a unique file name.  The default is 'false' "
            "and experiment databases will be deleted when the OpenSS "
            "session terminates unless the user has issued an 'expSave' "
            "command.");
  Bvalue = settings->readBoolEntry(std::string("onRerunSaveCopyOfExperimentDatabase"), OPENSS_ON_RERUN_SAVE_COPY_OF_EXPERIMENT_DATABASE, &ok);
  if (ok) OPENSS_ON_RERUN_SAVE_COPY_OF_EXPERIMENT_DATABASE = Bvalue;

  Add_Help (czar, "allowPythonCommands", "a boolean, preference",
            "Declare that Python commands may be intermixed with OpenSS "
            "commands.  The default is false.");
  Bvalue = settings->readBoolEntry(std::string("allowPythonCommands"), OPENSS_ALLOW_PYTHON_COMMANDS, &ok);
  if (ok) OPENSS_ALLOW_PYTHON_COMMANDS = Bvalue;

  Add_Help (czar, "instrumentorIsOffline", "a boolean, preference",
            "Declare whether or not the underlying instrumention mechanism for Open|SpeedShop "
            "is offline, or if not set: dynamic.  What this means is "
	    "that under the hood the mechanism used to gather the performance "
            "data is based on LD_PRELOAD not dynamic instrumentation.  Attaching "
	    "to a running process or any process control of the running "
	    "performance experiment is not able to be done.  Use the dynamic "
	    "instrumentation mechanism for those features");
  Bvalue = settings->readBoolEntry(std::string("instrumentorIsOffline"), OPENSS_INSTRUMENTOR_IS_OFFLINE, &ok);
  if (ok) OPENSS_INSTRUMENTOR_IS_OFFLINE = Bvalue;

  Add_Help (czar, "lessRestrictiveComparisons", "a boolean, preference",
            "Declare whether or not comparisons should consider the directory path and linked object "
            "when comparing performance data for a particular function.  If this preference is set, "
            "the directory path of the source file and the linked object will not be considered.  Use this if you are comparing "
            "the same program but have different source versions of the program in separate directories.");
  Bvalue = settings->readBoolEntry(std::string("lessRestrictiveComparisons"), OPENSS_LESS_RESTRICTIVE_COMPARISONS, &ok);
//  std::cout << " SS_Configure.cxx, lessRestrictiveComparisons section, Bvalue=" << Bvalue << " ok=" << ok << std::endl;
  if (ok) OPENSS_LESS_RESTRICTIVE_COMPARISONS = Bvalue;

  Add_Help (czar, "OPENSS_LOG_BY_DEFAULT", "a boolean, environment",
            "Declare that a log file will be opened and each command "
            "will be tracked through the various internal processing "
            "steps of OpenSS.  This is intended to be an internal debug "
            "aid and is not generally useful.  The default is false.");
  set_bool (OPENSS_LOG_BY_DEFAULT, "OPENSS_LOG_BY_DEFAULT");

  Add_Help (czar, "OPENSS_LIMIT_SIGNAL_CATCHING", "a boolean, environment",
            "Declare that OpenSS should limit the types of signals it "
            "traps. When set to true, OpenSS will ignore the following "
            "faults: "
            "\n  SIGILL - illegal instructions "
            "\n  SIGFPE - floating point exceptions "
            "\n  SIGBUS - bus errors "
            "\n  SIGSEGV - illegal memory addresses "
            "\n  SIGSYS - system errors "
            "\nIgnoring the errors will allow a core file to be generated, "
            "so this is intended to be an internal debug aid.  The default "
            "value is false and OpenSS will attempt to clean up if an "
            "error is encountered.  Setting the value to true may result "
            "in a number of files being left around if OpenSS encounters "
            "a fault.");
  set_bool (OPENSS_LIMIT_SIGNAL_CATCHING, "OPENSS_LIMIT_SIGNAL_CATCHING");
}
