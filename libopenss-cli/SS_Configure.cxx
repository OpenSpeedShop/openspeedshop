/******************************************************************************e
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
// where is this include????    #include <libxml/xmlreader.h>

int64_t OPENSS_VIEW_FIELD_SIZE = 20;
int64_t OPENSS_VIEW_PRECISION = 4;
int64_t OPENSS_HISTORY_LIMIT = 100;
int64_t OPENSS_HISTORY_DEFAULT = 24;
int64_t OPENSS_MAX_ASYNC_COMMANDS = 20;
int64_t OPENSS_HELP_LEVEL_DEFAULT = 2;
bool    OPENSS_VIEW_FULLPATH = false;
bool    OPENSS_SAVE_EXPERIMENT_DATABASE = false;
bool    OPENSS_ALLOW_PYTHON_COMMANDS = true;
bool    OPENSS_LOG_BY_DEFAULT = false;
bool    OPENSS_LIMIT_SIGNAL_CATCHING = false;

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

void SS_Configure () {

  set_int64 (OPENSS_VIEW_FIELD_SIZE, "OPENSS_VIEW_FIELD_SIZE");
  set_int64 (OPENSS_VIEW_PRECISION, "OPENSS_VIEW_PRECISION");
  set_int64 (OPENSS_HISTORY_LIMIT, "OPENSS_HISTORY_LIMIT");
  set_int64 (OPENSS_HISTORY_DEFAULT, "OPENSS_HISTORY_DEFAULT");
  set_int64 (OPENSS_MAX_ASYNC_COMMANDS, "OPENSS_MAX_ASYNC_COMMANDS");
  set_int64 (OPENSS_HELP_LEVEL_DEFAULT, "OPENSS_HELP_LEVEL_DEFAULT");

  set_bool (OPENSS_VIEW_FULLPATH, "OPENSS_VIEW_FULLPATH");
  set_bool (OPENSS_SAVE_EXPERIMENT_DATABASE, "OPENSS_SAVE_EXPERIMENT_DATABASE");
  set_bool (OPENSS_ALLOW_PYTHON_COMMANDS, "OPENSS_ALLOW_PYTHON_COMMANDS");
  set_bool (OPENSS_LOG_BY_DEFAULT, "OPENSS_LOG_BY_DEFAULT");
  set_bool (OPENSS_LIMIT_SIGNAL_CATCHING, "OPENSS_LIMIT_SIGNAL_CATCHING");
}

static void Search_rc_Files () {
  std::string baseName = ".openss/preferencesrc";
  int fd;

 // Look for installation definitions.
  char *S = getenv ("OPENSS_INSTALL_DIR");
  if (S != NULL) {
    std::string FilePath = std::string(S) + baseName;
    if ((fd = open(FilePath.c_str(), O_RDONLY)) != -1) {
     // File name exists!
      Assert(close(fd) == 0);
    }
  }

 // Look for global user definitions.
  S = getenv ("HOME");
  if (S != NULL) {
    std::string FilePath = std::string(S) + baseName;
    if ((fd = open(FilePath.c_str(), O_RDONLY)) != -1) {
     // File name exists!
      Assert(close(fd) == 0);
    }
  }

 // Look for local user definitions.
  std::string FilePath = std::string("./") + baseName;
  if ((fd = open(FilePath.c_str(), O_RDONLY)) != -1) {
   // File name exists!
    Assert(close(fd) == 0);
  }
}
