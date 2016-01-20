/******************************************************************************e
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2016 Krell Institute  All Rights Reserved.
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

// Formating output of `expView` commands.
int64_t OPENSS_VIEW_FIELD_SIZE = 24;
int64_t OPENSS_VIEW_MAX_FIELD_SIZE = 4096;
int64_t OPENSS_VIEW_PRECISION = 6;
int64_t OPENSS_VIEW_DATE_TIME_PRECISION = 3;
bool    OPENSS_VIEW_FIELD_SIZE_IS_DYNAMIC = true;
bool    OPENSS_VIEW_FULLPATH = false;
bool    OPENSS_VIEW_ENTIRE_STRING = true;
bool    OPENSS_VIEW_DEFINING_LOCATION = true;
bool    OPENSS_VIEW_MANGLED_NAME = false;
bool    OPENSS_VIEW_SUPPRESS_UNUSED_ELEMENTS = true;
bool    OPENSS_VIEW_THREAD_ID_WITH_MAX_OR_MIN = true;
bool    OPENSS_VIEW_USE_BLANK_IN_PLACE_OF_ZERO = false;
bool    OPENSS_REDIRECT_USE_BLANK_IN_PLACE_OF_ZERO = false;
std::string OPENSS_VIEW_EOC = "  ";
std::string OPENSS_VIEW_EOL = "\n";
std::string OPENSS_VIEW_EOV = "\n";
// Saving and reusing ouput of `expView` commands.
bool    OPENSS_SAVE_VIEWS_FOR_REUSE = false;
int64_t OPENSS_SAVE_VIEWS_FILE_LIMIT = 200;
int64_t OPENSS_SAVE_VIEWS_TIME = 10;
// Modifications of various commands.
bool    OPENSS_SAVE_EXPERIMENT_DATABASE = true;
bool    OPENSS_ASK_ABOUT_SAVING_THE_DATABASE = true;
bool    OPENSS_ON_RERUN_SAVE_COPY_OF_EXPERIMENT_DATABASE = true;
bool    OPENSS_ASK_ABOUT_CHANGING_ARGS = true;
bool    OPENSS_ALLOW_PYTHON_COMMANDS = true;
bool    OPENSS_LESS_RESTRICTIVE_COMPARISONS = true;
bool    OPENSS_LOG_BY_DEFAULT = false;
bool    OPENSS_LIMIT_SIGNAL_CATCHING = false;
bool    OPENSS_INSTRUMENTOR_IS_OFFLINE = true;
int64_t OPENSS_HISTORY_LIMIT = 100;
int64_t OPENSS_HISTORY_DEFAULT = 24;
int64_t OPENSS_MAX_ASYNC_COMMANDS = 20;
int64_t OPENSS_HELP_LEVEL_DEFAULT = 1;
bool    OPENSS_AUTO_CREATE_DERIVED_METRICS = true;

// Remember valid user names for error cehcking of format specifiers on view commands.
// Valid names are captured when SS_Configure.cxx looks them up with the following utilities.
#include <string.h>
#include <vector>

enum config_type_enum {config_type_string, config_type_int64, config_type_bool};
struct config_union {
//  union {
    std::string *config_string;
    bool *config_bool;
    int64_t *config_int64;
//  };
};
struct config_options {
  std::string user_config_name;
  config_type_enum config_type;
  union {
    std::string *config_string;
    bool *config_bool;
    int64_t *config_int64;
  } internal;
  union {
    std::string *config_string;
    bool config_bool;
    int64_t config_int64;
  } defaults;
//  config_union internal;
//  config_union defaults;
};

static std::vector<config_options *> config_info;

static void
Record_Config_Info( std::string user_name, std::string *internal_value)
{
  config_options *tmp = new config_options();
  tmp->user_config_name = user_name;
  tmp->config_type = config_type_string;
  tmp->internal.config_string = internal_value;
  tmp->defaults.config_string = new std::string(internal_value->c_str());
  config_info.push_back(tmp); // Capture valid names for error checking.
}

static void
Record_Config_Info( std::string user_name, int64_t *internal_value)
{
  config_options *tmp = new config_options();
  tmp->user_config_name = user_name;
  tmp->config_type = config_type_int64;
  tmp->internal.config_int64 = internal_value;
  tmp->defaults.config_int64 = *internal_value;
  config_info.push_back(tmp); // Capture valid names for error checking.
}

static void
Record_Config_Info( std::string user_name, bool *internal_value)
{
  config_options *tmp = new config_options();
  tmp->user_config_name = user_name;
  tmp->config_type = config_type_bool;
  tmp->internal.config_bool = internal_value;
  tmp->defaults.config_bool = *internal_value;
  config_info.push_back(tmp); // Capture valid names for error checking.
}



bool check_validConfigurationName(std::string s) {
  for (int64_t i=0; i< config_info.size(); i++) {
    if ( !strcasecmp( config_info[i]->user_config_name.c_str(), s.c_str()) ) {
      return true;
    }
  }

  return false;
}

bool set_ConfigValue(std::string config_name, std::string value) {
  for (int64_t i=0; i< config_info.size(); i++) {
    if ( !strcasecmp(config_info[i]->user_config_name.c_str(), config_name.c_str()) ) {
      if (config_info[i]->config_type != config_type_string) {
        return false;
      }
      if ( *(config_info[i]->internal.config_string)  != value ) {
        *(config_info[i]->internal.config_string) = value;
// TODO - need to write changes to option file.
      }
      return true;
    }
  }

  return false;
}

bool set_ConfigValue(std::string config_name, int64_t value) {
  for (int64_t i=0; i<config_info.size(); i++) {
    if ( !strcasecmp(config_info[i]->user_config_name.c_str(), config_name.c_str()) ) {
      if (config_info[i]->config_type != config_type_int64) {
        return false;
      }
      if ( *(config_info[i]->internal.config_int64) != value ) {
        *(config_info[i]->internal.config_int64) = value;
// TODO - need to write changes to option file.
      }
      return true;
    }
  }

  return false;
}

bool set_ConfigValue(std::string config_name, bool value) {
  for (int64_t i=0; i<config_info.size(); i++) {
    if ( !strcasecmp(config_info[i]->user_config_name.c_str(), config_name.c_str()) ) {
      if (config_info[i]->config_type != config_type_bool) {
        return false;
      }
      if ( *(config_info[i]->internal.config_bool) != value ) {
        *(config_info[i]->internal.config_bool) = (value == true) ? true : false;
// TODO - need to write changes to option file.
      }
      return true;
    }
  }

  return false;
}

bool List_ConfigInfo(CommandObject *cmd) {
  for (int64_t i=0; i<config_info.size(); i++) {
    std::string config = config_info[i]->user_config_name + " = ";
    std::string iname;
    switch (config_info[i]->config_type) {
      case config_type_string:
        config += "\"";
        iname = *(config_info[i]->internal.config_string);
        for (int64_t j=0; j<iname.length(); j++) {
          char c = iname[j];
          if (c == *"\n") {
            config += "\\n";
          } else {
            config += c;
          }
        }
        config += "\"";
        break;
      case config_type_int64:
      {
        char S[40];
        sprintf (S, "%lld", *(config_info[i]->internal.config_int64));
        int64_t V64;
        config += S;
        break;
      }
      case config_type_bool:
        config += (*(config_info[i]->internal.config_bool) == true) ? "true" : "false";
        break;
    }
   // Add default value if it is different from the current value.
    switch (config_info[i]->config_type) {
      case config_type_string:
        iname = *(config_info[i]->defaults.config_string);
        if (iname != *(config_info[i]->internal.config_string)) {
          config += " (default was \"";
          for (int64_t j=0; j<iname.length(); j++) {
            char c = iname[j];
            if (c == *"\n") {
              config += "\\n";
            } else {
              config += c;
            }
          }
          config += "\")";
        }
        break;
      case config_type_int64:
      {
        if (*(config_info[i]->internal.config_int64) != config_info[i]->defaults.config_int64) {
          char S[40];
          config += " (default was ";
          sprintf (S, "%lld", config_info[i]->defaults.config_int64);
          config += S;
          config += ")";
        }
        break;
      }
      case config_type_bool:
        if (*(config_info[i]->internal.config_bool) != config_info[i]->defaults.config_bool) {
          config += " (default was ";
          config += (config_info[i]->defaults.config_bool == true) ? "true" : "false";
          config += ")";
        break;
      }
    }
    config += "\n";
    cmd->Result_String(config);
  }

  return true;
}

static std::vector<std::string> validFormatNames;
bool check_validFormatName(std::string s) {
  for (int64_t i=0; i<validFormatNames.size(); i++) {
    if (!strcasecmp(validFormatNames[i].c_str(), s.c_str())) {
      return true;
    }
  }

  return false;
}

// This function is intended for use by internal debug options that
// are not accessable by users.  These options can only be changed
// by altering this routine and recompiling OPENSS.
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
  std::string configName;
  bool ok;

 // Open the prefence data base
  SS_Settings *settings = new SS_Settings();
  validFormatNames.clear();
  config_info.clear();

 // Containers to hold results of preference reads
  int64_t Ivalue;
  double Fvalue;
  bool Bvalue;
  std::string Svalue;

  configName = "DpcldListenerPort";
  Add_Help (czar, "DpcldListenerPort", "an internal configuration",
            "When DPCL is initalized, it determines which port to listen on "
            "for manually started daemons.  The string is in the form "
            "'host_name:port_number' and will be returned when the"
            "name is entered.  It must be used if the daemons are started by "
            "the user.  In most environments, the daemons are installed in "
            "system libraries and do not need to be manually started.");

// Formating output of `expView` commands.
  configName = "viewFieldSize";
  validFormatNames.push_back(configName);
  Add_Help (czar, "viewFieldSize", "an integer, view format preference",
            "Define the width of each field when the result "
            "of an 'expView' command is printed.  The default is 24 columns.");
  Ivalue = settings->readNumEntry(std::string("viewFieldSize"), OPENSS_VIEW_FIELD_SIZE, &ok);
  if (ok && (Ivalue >= 0)) OPENSS_VIEW_FIELD_SIZE = Ivalue;
  Record_Config_Info(configName, &OPENSS_VIEW_FIELD_SIZE);

  configName = "viewMaxFieldSize";
  Add_Help (czar, "viewMaxFieldSize", "an integer, preference",
            "Define the maximum width of each field when the result "
            "of an 'expView' command is printed.  Field width is flexible, "
            "but a limit is needed for internal buffers. The default is 4096 characters.");
  Ivalue = settings->readNumEntry(std::string("viewMaxFieldSize"), OPENSS_VIEW_MAX_FIELD_SIZE, &ok);
  if (ok && (Ivalue >= 0)) OPENSS_VIEW_MAX_FIELD_SIZE = Ivalue;
  Record_Config_Info(configName, &OPENSS_VIEW_MAX_FIELD_SIZE);

  configName = "viewPrecision";
  validFormatNames.push_back(configName);
  Add_Help (czar, "viewPrecision", "an integer, view format preference",
            "Define the precision used to format a floating point number when "
            "the result of an 'expView' command is printed.  The default is 6.");
  Ivalue = settings->readNumEntry(std::string("viewPrecision"), OPENSS_VIEW_PRECISION, &ok);
  if (ok && (Ivalue >= 0)) OPENSS_VIEW_PRECISION = Ivalue;
  Record_Config_Info(configName, &OPENSS_VIEW_PRECISION);

  configName = "viewDateTimePrecision";
  validFormatNames.push_back(configName);
  Add_Help (czar, "viewDateTimePrecision", "an integer, view format preference",
            "Define the precision used to format a time value that is part of a Start or "
            "End time displayed when the result of an 'expView' command is printed. "
            "Valid values are \"0\" to \"9\" and control the number of decimal points of "
            "accuracy displayed for fractions of a second. Since each value requires a "
            "column in the output, the width of output fields, \"viewFieldSize\", will "
            "be automatically increased when it is too small.  "
            "The default is 0.");
  Ivalue = settings->readNumEntry(std::string("viewDateTimePrecision"), OPENSS_VIEW_DATE_TIME_PRECISION, &ok);
  if (ok && (Ivalue >= 0)) {
   if (Ivalue < 0) {
     Ivalue = 0;
   } else if (Ivalue > 9) {
     Ivalue = 9;
   }
   if (OPENSS_VIEW_FIELD_SIZE < (20 + Ivalue)) {
    // Need to make fixed sized fields larger to accommodate decimal point and fraction.
     OPENSS_VIEW_FIELD_SIZE = 20 + Ivalue;
   }
   OPENSS_VIEW_DATE_TIME_PRECISION = Ivalue;
  }
  Record_Config_Info(configName, &OPENSS_VIEW_DATE_TIME_PRECISION);

  configName = "viewFieldSizeIsDynamic";
  validFormatNames.push_back(configName);
  Add_Help (czar, "viewFieldSizeIsDynamic", "a boolean, view format preference",
            "Declare whether or not the width of each column of ouput is "
            "is dynamically determined by the minimum width actually required by the data, "
            "or is fixed and determined by \"viewFieldSize\". "
            "The default is false.");
  Bvalue = settings->readBoolEntry(std::string("viewFieldSizeIsDynamic"), OPENSS_VIEW_FIELD_SIZE_IS_DYNAMIC, &ok);
  if (ok) OPENSS_VIEW_FIELD_SIZE_IS_DYNAMIC = Bvalue;
  Record_Config_Info(configName, &OPENSS_VIEW_FIELD_SIZE_IS_DYNAMIC);

  configName = "viewFullPath";
  validFormatNames.push_back(configName);
  Add_Help (czar, "viewFullPath", "a boolean, view format preference",
            "Declare whether or not a full path is displayed in place of "
            "a file name when the function, linkedobject, or statement "
            "location is displayed as part of an 'expView' command.  The "
            "default is false, allowing only the containing file name to "
            "be displayed.");
  Bvalue = settings->readBoolEntry(std::string("viewFullPath"), OPENSS_VIEW_FULLPATH, &ok);
  if (ok) OPENSS_VIEW_FULLPATH = Bvalue;
  Record_Config_Info(configName, &OPENSS_VIEW_FULLPATH);

  configName = "viewEntireString";
  validFormatNames.push_back(configName);
  Add_Help (czar, "viewEntireString", "a boolean, view format preference",
            "Declare whether or not an entire string is displayed for "
            "a file name when the length of the string exceeds the "
            "size specified by viewFieldSize preference.  The "
            "default is false, requiring that the string be truncated "
            "on the left (indicated with '\?\?\?' characters) to fit "
            "within the allowed space.  If this preference is selected "
            "and the string is larger than the FieldSize, the FieldSize "
            "is ignored which will cause misalignment of other fields "
            "to the right of the one the string was to occupy.");

  Bvalue = settings->readBoolEntry(std::string("viewEntireString"), OPENSS_VIEW_ENTIRE_STRING, &ok);
  if (ok) OPENSS_VIEW_ENTIRE_STRING = Bvalue;
  Record_Config_Info(configName, &OPENSS_VIEW_ENTIRE_STRING);

  configName = "viewDefiningLocation";
  validFormatNames.push_back(configName);
  Add_Help (czar, "viewDefiningLocation", "a boolean, view format preference",
            "Declare whether or not the defining location is displayed "
            "when a function name is displayed as part of an 'expView' "
            "command.  The default is true");
  Bvalue = settings->readBoolEntry(std::string("viewDefiningLocation"), OPENSS_VIEW_DEFINING_LOCATION, &ok);
  if (ok) OPENSS_VIEW_DEFINING_LOCATION = Bvalue;
  Record_Config_Info(configName, &OPENSS_VIEW_DEFINING_LOCATION);

  configName = "viewMangledName";
  validFormatNames.push_back(configName);
  Add_Help (czar, "viewMangledName", "a boolean, view format preference",
            "Declare whether or not a mangled name is displayed "
            "when a function is displayed as part of an 'expView' command. "
            "The default is false, allowing the demangled names, that are "
            "used in the source code, to be displayed.");
  Bvalue = settings->readBoolEntry(std::string("viewMangledName"), OPENSS_VIEW_MANGLED_NAME, &ok);
  if (ok) OPENSS_VIEW_MANGLED_NAME = Bvalue;
  Record_Config_Info(configName, &OPENSS_VIEW_MANGLED_NAME);

  configName = "viewSuppressUnusedElements";
  Add_Help (czar, "viewSuppressUnusedElements", "a boolean, preference",
            "Declare whether or not a line of ouput is suppressed if "
            "there were no measurements recorded for the associated "
            "statement, function or linked object. "
            "The default is true, causing elements that are unsed "
            "to be deleted from the output.");
  Bvalue = settings->readBoolEntry(std::string("viewSuppressUnusedElements"), OPENSS_VIEW_SUPPRESS_UNUSED_ELEMENTS, &ok);
  if (ok) OPENSS_VIEW_SUPPRESS_UNUSED_ELEMENTS = Bvalue;
  Record_Config_Info(configName, &OPENSS_VIEW_SUPPRESS_UNUSED_ELEMENTS);

  configName = "viewThreadIdWithMaxMin";
  Add_Help (czar, "viewThreadIdWithMaxMin", "a boolean, preference",
            "Declare whether or not the thread ID, of the tread that "
            "contains the Max or Min value, is displayed "
            "when a user requests '-m ThreadMax' or -m ThreadMin` "
            "on an 'expView' command. "
            "The default is true, causing the thread Id of the Max and Min "
            "to be displayed next to the Max and Min values.");
  Bvalue = settings->readBoolEntry(std::string("viewThreadIdWithMaxMin"), OPENSS_VIEW_THREAD_ID_WITH_MAX_OR_MIN, &ok);
  if (ok) OPENSS_VIEW_THREAD_ID_WITH_MAX_OR_MIN = Bvalue;
  Record_Config_Info(configName, &OPENSS_VIEW_THREAD_ID_WITH_MAX_OR_MIN);

  configName = "viewBlankInPlaceOfZero";
  validFormatNames.push_back(configName);
  Add_Help (czar, "viewBlankInPlaceOfZero", "a boolean, view format preference",
            "Declare whether or not Blanks are displayed for Zero valued fields "
            "in the columns of output from an 'expView' command. "
            "See 'redirectBlankInPlaceOfZero' when output is redirected to a file. "
            "The default is false, causing zeros values to be displayed in the output.");
  Bvalue = settings->readBoolEntry(std::string("viewBlankInPlaceOfZero"), OPENSS_VIEW_USE_BLANK_IN_PLACE_OF_ZERO, &ok);
  if (ok) OPENSS_VIEW_USE_BLANK_IN_PLACE_OF_ZERO = Bvalue;
  Record_Config_Info(configName, &OPENSS_VIEW_USE_BLANK_IN_PLACE_OF_ZERO);

  configName = "redirectBlankInPlaceOfZero";
  Add_Help (czar, "redirectBlankInPlaceOfZero", "a boolean, preference",
            "Declare whether or not Blanks are displayed for Zero valued fields "
            "in the columns of output for an 'expView' command when "
            "output is redirected to a file. "
            "See 'viewBlankInPlaceOfZero' when output is not redirected. "
            "The default is false, causing zeros values to be displayed in the output.");
  Bvalue = settings->readBoolEntry(std::string("redirectBlankInPlaceOfZero"), OPENSS_REDIRECT_USE_BLANK_IN_PLACE_OF_ZERO, &ok);
  if (ok) OPENSS_REDIRECT_USE_BLANK_IN_PLACE_OF_ZERO = Bvalue;
  Record_Config_Info(configName, &OPENSS_REDIRECT_USE_BLANK_IN_PLACE_OF_ZERO);

  configName = "viewEoc";
  validFormatNames.push_back(configName);
  Add_Help (czar, "viewEoc", "a character string, view format preference",
            "Specify the character string that is appended to the end "
            "of each column of output generated by an 'expView'. "
            "The default is \"  \" (2 spaces).");
  Svalue = settings->readEntry(std::string("viewEoc"), OPENSS_VIEW_EOC, &ok);
  if (ok) OPENSS_VIEW_EOC = Svalue;
  Record_Config_Info(configName, &OPENSS_VIEW_EOC);

  configName = "viewEol";
  validFormatNames.push_back(configName);
  Add_Help (czar, "viewEol", "a character string, view format preference",
            "Specify the character string that is appended to the end "
            "of each row of output generated by an 'expView'. "
            "The default is \"\n\" (a line feed).");
  Svalue = settings->readEntry(std::string("viewEol"), OPENSS_VIEW_EOL, &ok);
  if (ok) OPENSS_VIEW_EOL = Svalue;
  Record_Config_Info(configName, &OPENSS_VIEW_EOL);

  configName = "viewEov";
  validFormatNames.push_back(configName);
  Add_Help (czar, "viewEov", "a character string, view format preference",
            "Specify the character string that is appended to the end "
            "of the output generated from an 'expView' command. "
            "It is intended to aid the readability of the output."
            "The default is \"\n\" (line feed).");
  Svalue = settings->readEntry(std::string("viewEov"), OPENSS_VIEW_EOV, &ok);
  if (ok) OPENSS_VIEW_EOV = Svalue;
  Record_Config_Info(configName, &OPENSS_VIEW_EOV);

// Formating output of `expView` commands using the `-F` option.
  configName = "viewLeftJustify";
  validFormatNames.push_back(configName);  // Capture valid names for error checking.
  Add_Help (czar, "viewLeftJustify", "a boolean, view format option",
            "Declare whether or not all output should be left justified "
            "in the columns of output for an 'expView'. "
            "The default is false, causing only the right most column to be left justified.");

  configName = "CSV";
  validFormatNames.push_back(configName);  // Capture valid names for error checking.
  Add_Help (czar, "CSV", "a view format option",
            "Specify that each column of output generated by an 'expView' "
            "command is separated with a comma (\",\"). "
            "Example: 'expview -F CSV'. "
            "The default is to place \"  \" (2 spaces) between columns.");

// Saving and reusing ouput of `expView` commands.
  configName = "saveViewsForReuse";
  Add_Help (czar, "saveViewsForReuse", "a boolean, preference",
            "Declare that the report created when an 'expView' "
            "command is issued will be saved and reused if the same "
            "view is requested again.  Reusing a previously created view saves "
            "the time required to regenerate the same information. "
            "The saved view will be in the user's "
            "current directory and will be of the form: "
            " 'database_name.iiiiii.view'  "
            "where the 'iiiiii' field is the first integer, starting with 0, "
            "that generates a unique file name. "
            "If set to 'false', previously saved views will not be resued and "
            "no new ones will be saved. "
            "If set to 'true', existing experiment views will be resued and newly created "
            "ones will be saved, according to the values set for the preferences "
            "'saveViewsFileLimit' and 'saveViewsTime'.  The default is 'true'.");
  Bvalue = settings->readBoolEntry(std::string("saveViewsForReuse"), OPENSS_SAVE_VIEWS_FOR_REUSE, &ok);
  if (ok) OPENSS_SAVE_VIEWS_FOR_REUSE = Bvalue;
  // std::cerr << "Configuring OPENSS_SAVE_VIEWS_FOR_REUSE=" << Bvalue << std::endl;
  Record_Config_Info(configName, &OPENSS_SAVE_VIEWS_FOR_REUSE);

  configName = "saveViewsFileLimit";
  Add_Help (czar, "saveViewsFileLimit", "an integer preference",
            "Declare the maximum  number of files that will be created to "
            "save the results of unique `expView` commands. "
            "The default is 200.");
  Ivalue = settings->readNumEntry(std::string("saveViewsFileLimit"), OPENSS_SAVE_VIEWS_FILE_LIMIT, &ok);
  if (ok) OPENSS_SAVE_VIEWS_FILE_LIMIT = Ivalue;
  Record_Config_Info(configName, &OPENSS_SAVE_VIEWS_FILE_LIMIT);

  configName = "saveViewsTime";
  Add_Help (czar, "saveViewsTime", "an integer preference",
            "if `saveViewsForReuse` is `true` and the number of saved files has not "
            "reached the limit imposed by `saveViewsFileLimit`, all unique views will "
            "be save for the current session and will be retrieved from the saved file "
            "if the same command is reissued during the current session. "
            "This preference controls what happens to newly created files, "
            "at the close of the current session: "
            "if the value is less than 0, no files will be saved; "
            "if it is equal to 0, all files will be saved; "
            "if greater than 0, a file will be saved only if the time taken to generate "
            "it exceeded the number of seconds specified by this preference. "
            "The default is 10.");
  Ivalue = settings->readNumEntry(std::string("saveViewsTime"), OPENSS_SAVE_VIEWS_TIME, &ok);
  if (ok) OPENSS_SAVE_VIEWS_TIME = Ivalue;
  Record_Config_Info(configName, &OPENSS_SAVE_VIEWS_TIME);

// Modifications of various commands.
  configName = "saveExperimentDatabase";
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
  Record_Config_Info(configName, &OPENSS_SAVE_EXPERIMENT_DATABASE);

  configName = "askAboutSavingTheDatabase";
  Add_Help (czar, "askAboutSavingTheDatabase", "a boolean, preference",
            "Declare that when rerunning an experiment create a dialog"
            " message that allows the experiment database file to be saved "
            "prior to reexecuting the experiment.");
  Bvalue = settings->readBoolEntry(std::string("askAboutSavingTheDatabase"), OPENSS_ASK_ABOUT_SAVING_THE_DATABASE, &ok);
  if (ok) OPENSS_ASK_ABOUT_SAVING_THE_DATABASE = Bvalue;
  Record_Config_Info(configName, &OPENSS_ASK_ABOUT_SAVING_THE_DATABASE);

  configName = "askAboutChangingArgs";
  Add_Help (czar, "askAboutChangingArgs", "a boolean, preference",
            "Declare that when rerunning an experiment create a dialog"
            " message that allows the application's arguments to be changed prior "
            "to reexecuting the experiment.");
  Bvalue = settings->readBoolEntry(std::string("askAboutChangingArgs"), OPENSS_ASK_ABOUT_CHANGING_ARGS, &ok);
  if (ok) OPENSS_ASK_ABOUT_CHANGING_ARGS = Bvalue;
  Record_Config_Info(configName, &OPENSS_ASK_ABOUT_CHANGING_ARGS);

  configName = "onRerunSaveCopyOfExperimentDatabase";
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
  Record_Config_Info(configName, &OPENSS_ON_RERUN_SAVE_COPY_OF_EXPERIMENT_DATABASE);

  configName = "allowPythonCommands";
  Add_Help (czar, "allowPythonCommands", "a boolean, preference",
            "Declare that Python commands may be intermixed with OpenSS "
            "commands.  The default is false.");
  Bvalue = settings->readBoolEntry(std::string("allowPythonCommands"), OPENSS_ALLOW_PYTHON_COMMANDS, &ok);
  if (ok) OPENSS_ALLOW_PYTHON_COMMANDS = Bvalue;
  Record_Config_Info(configName, &OPENSS_ALLOW_PYTHON_COMMANDS);

  configName = "lessRestrictiveComparisons";
  Add_Help (czar, "lessRestrictiveComparisons", "a boolean, preference",
            "Declare whether or not comparisons should consider the directory path and linked object "
            "when comparing performance data for a particular function.  If this preference is set, "
            "the directory path of the source file and the linked object will not be considered.  Use this if you are comparing "
            "the same program but have different source versions of the program in separate directories.");
  Bvalue = settings->readBoolEntry(std::string("lessRestrictiveComparisons"), OPENSS_LESS_RESTRICTIVE_COMPARISONS, &ok);
//  std::cerr << " SS_Configure.cxx, lessRestrictiveComparisons section, Bvalue=" << Bvalue << " ok=" << ok << std::endl;
  if (ok) OPENSS_LESS_RESTRICTIVE_COMPARISONS = Bvalue;
  Record_Config_Info(configName, &OPENSS_LESS_RESTRICTIVE_COMPARISONS);

  configName = "OPENSS_LOG_BY_DEFAULT";
  Add_Help (czar, "OPENSS_LOG_BY_DEFAULT", "a boolean environment",
            "Declare that a log file will be opened and each command "
            "will be tracked through the various internal processing "
            "steps of OpenSS.  This is intended to be an internal debug "
            "aid and is not generally useful.  The default is false.");
  set_bool (OPENSS_LOG_BY_DEFAULT, "OPENSS_LOG_BY_DEFAULT");

  configName = "OPENSS_LIMIT_SIGNAL_CATCHING";
  Add_Help (czar, "OPENSS_LIMIT_SIGNAL_CATCHING", "a boolean environment",
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

  configName = "instrumentorIsOffline";
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
  Record_Config_Info(configName, &OPENSS_INSTRUMENTOR_IS_OFFLINE);

  configName = "historyLimit";
  Add_Help (czar, "historyLimit", "an integer, preference",
            "Define the maximum number of commands that are remembered for the "
            "'history' command.  If the command is issued with a larger number, "
            "this limit is automatically increased.  The default is 100.");
  Ivalue = settings->readNumEntry(std::string("historyLimit"), OPENSS_HISTORY_LIMIT, &ok);
  if (ok && (Ivalue >= 0)) OPENSS_HISTORY_LIMIT = Ivalue;
  Record_Config_Info(configName, &OPENSS_HISTORY_LIMIT);

  configName = "historyDefault";
  Add_Help (czar, "historyDefault", "an integer, preference",
            "Define the number of previous commands that will be printed when "
            "the 'history' command is issued without a requesting length. The "
            "default is 24.");
  Ivalue = settings->readNumEntry(std::string("historyDefault"), OPENSS_HISTORY_DEFAULT, &ok);
  if (ok && (Ivalue >= 0)) OPENSS_HISTORY_DEFAULT = Ivalue;
  Record_Config_Info(configName, &OPENSS_HISTORY_DEFAULT);

  configName = "maxAsyncCommands";
  Add_Help (czar, "maxAsyncCommands", "an integer, preference",
            "Define the maximum number of commands that can be processed at "
            "the same time. This is a limit on the parallel execution of "
            "commands in OpenSS and controls the degree to which commands "
            "can be overlapped.  The default is 20.");
  Ivalue = settings->readNumEntry(std::string("maxAsyncCommands"), OPENSS_MAX_ASYNC_COMMANDS, &ok);
  if (ok && (Ivalue > 0)) OPENSS_MAX_ASYNC_COMMANDS = Ivalue;
  Record_Config_Info(configName, &OPENSS_MAX_ASYNC_COMMANDS);

  configName = "helpLevelDefault";
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
  Record_Config_Info(configName, &OPENSS_HELP_LEVEL_DEFAULT);

// Creating Derived Metric Output when the necessary hardware counters are available.
  configName = "autoCreateDerivedMetrics";
  Add_Help (czar, "autoCreateDerivedMetrics", "a boolean, preference",
            "Declare that OpenSpeedShop should match hardware counters to create"
	    " derived metric values for the default 'expView' "
            "command."
            "If set to 'false', no derived metric output will be created and "
            "displayed."
            "If set to 'true', derived metrics values will be created and displayed if "
            "the hardware counters were gathers and are programmed as key derived"
            "metrics of importance internal to OpenSpeedShop The default is 'true'.");
  Bvalue = settings->readBoolEntry(std::string("autoCreateDerivedMetrics"), OPENSS_AUTO_CREATE_DERIVED_METRICS, &ok);
  if (ok) OPENSS_AUTO_CREATE_DERIVED_METRICS = Bvalue;
  // std::cerr << "Configuring OPENSS_AUTO_CREATE_DERIVED_METRICS=" << Bvalue << std::endl;
  Record_Config_Info(configName, &OPENSS_AUTO_CREATE_DERIVED_METRICS);

}
