/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2012 Krell Institute  All Rights Reserved.
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

/** @file
 *
 * Definition of the information that the user has control over w.r.t. OpenSpeedShop's behavior.
 *
 */

#include <string>

enum OpenSpeedShop_Start_Modes {
   SM_Batch,        /**< -batch  */
   SM_Cli,          /**< -cli */
   SM_Gui,          /**< default and -gui */
   SM_Offline,      /**< -offline */
   SM_Online,       /**< -online */
   SM_Unknown       /**< UNKNOWN/ERROR */
};

// User control over some of OpenSpeedShop's behavior.

// Formatting output of `expView` commands.
extern int64_t OPENSS_VIEW_FIELD_SIZE;
extern int64_t OPENSS_VIEW_MAX_FIELD_SIZE;
extern int64_t OPENSS_VIEW_PRECISION;
extern int64_t OPENSS_VIEW_DATE_TIME_PRECISION;
extern bool    OPENSS_VIEW_FIELD_SIZE_IS_DYNAMIC;
extern bool    OPENSS_VIEW_FULLPATH;
extern bool    OPENSS_VIEW_ENTIRE_STRING;
extern bool    OPENSS_VIEW_DEFINING_LOCATION;
extern bool    OPENSS_VIEW_MANGLED_NAME;
extern bool    OPENSS_VIEW_SUPPRESS_UNUSED_ELEMENTS;
extern bool    OPENSS_VIEW_THREAD_ID_WITH_MAX_OR_MIN;
extern bool    OPENSS_VIEW_USE_BLANK_IN_PLACE_OF_ZERO;
extern bool    OPENSS_REDIRECT_USE_BLANK_IN_PLACE_OF_ZERO;
extern std::string OPENSS_VIEW_EOC;
extern std::string OPENSS_VIEW_EOL;
extern std::string OPENSS_VIEW_EOV;
// Saving and reusing output of `expView` commands.
extern bool    OPENSS_SAVE_VIEWS_FOR_REUSE;
extern int64_t OPENSS_SAVE_VIEWS_FILE_LIMIT;
extern int64_t OPENSS_SAVE_VIEWS_TIME;
// Modifications of various commands.
extern bool    OPENSS_SAVE_EXPERIMENT_DATABASE;
extern bool    OPENSS_ASK_ABOUT_SAVING_THE_DATABASE;
extern bool    OPENSS_ON_RERUN_SAVE_COPY_OF_EXPERIMENT_DATABASE;
extern bool    OPENSS_ASK_ABOUT_CHANGING_ARGS;
extern bool    OPENSS_ALLOW_PYTHON_COMMANDS;
extern bool    OPENSS_LESS_RESTRICTIVE_COMPARISONS;
extern bool    OPENSS_LOG_BY_DEFAULT;
extern bool    OPENSS_LIMIT_SIGNAL_CATCHING;
extern bool    OPENSS_INSTRUMENTOR_IS_OFFLINE;
extern bool    OPENSS_AUTO_CREATE_DERIVED_METRICS;
extern int64_t OPENSS_HISTORY_LIMIT;
extern int64_t OPENSS_HISTORY_DEFAULT;
extern int64_t OPENSS_MAX_ASYNC_COMMANDS;
extern int64_t OPENSS_HELP_LEVEL_DEFAULT;
extern OpenSpeedShop_Start_Modes actualCLIStartMode;


// Read in the environment variables that control OpenSpeedShop
void SS_Configure ();

// Look up valid names with the following utilities:
bool check_validConfigurationName(std::string s);
bool check_validFormatName(std::string s);
