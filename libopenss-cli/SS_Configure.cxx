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

int64_t OPENSSS_VIEW_FIELD_SIZE = 20;
int64_t OPENSSS_VIEW_PRECISION = 4;
int64_t OPENSSS_HISTORY_LIMIT = 100;
int64_t OPENSSS_HISTORY_DEFAULT = 24;
int64_t OPENSSS_MAX_ASYNC_COMMANDS = 20;
int64_t OPENSSS_HELP_LEVEL_DEFAULT = 2;
bool    OPENSSS_SAVE_EXPERIMENT_DATABASE = false;
bool    OPENSSS_ALLOW_PYTHON_COMMANDS = true;
bool    OPENSSS_LOG_BY_DEFAULT = false;

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

  set_int64 (OPENSSS_VIEW_FIELD_SIZE, "OPENSSS_VIEW_FIELD_SIZE");
  set_int64 (OPENSSS_VIEW_PRECISION, "OPENSSS_VIEW_PRECISION");
  set_int64 (OPENSSS_HISTORY_LIMIT, "OPENSSS_HISTORY_LIMIT");
  set_int64 (OPENSSS_HISTORY_DEFAULT, "OPENSSS_HISTORY_DEFAULT");
  set_int64 (OPENSSS_MAX_ASYNC_COMMANDS, "OPENSSS_MAX_ASYNC_COMMANDS");
  set_int64 (OPENSSS_HELP_LEVEL_DEFAULT, "OPENSSS_HELP_LEVEL_DEFAULT");

  set_bool (OPENSSS_SAVE_EXPERIMENT_DATABASE, "OPENSSS_SAVE_EXPERIMENT_DATABASE");
  set_bool (OPENSSS_ALLOW_PYTHON_COMMANDS, "OPENSSS_ALLOW_PYTHON_COMMANDS");
  set_bool (OPENSSS_LOG_BY_DEFAULT, "OPENSSS_LOG_BY_DEFAULT");
}
