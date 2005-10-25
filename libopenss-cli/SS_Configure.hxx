/*******************************************************************************
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

// User control over some of OpenSpeedShop's behavior.
extern int64_t OPENSSS_VIEW_FIELD_SIZE;
extern int64_t OPENSSS_VIEW_PRECISION;
extern int64_t OPENSSS_HISTORY_LIMIT;
extern int64_t OPENSSS_HISTORY_DEFAULT;
extern int64_t OPENSSS_MAX_ASYNC_COMMANDS;
extern int64_t OPENSSS_HELP_LEVEL_DEFAULT;
extern bool    OPENSSS_SAVE_EXPERIMENT_DATABASE;
extern bool    OPENSSS_ALLOW_PYTHON_COMMANDS;
extern bool    OPENSSS_LOG_BY_DEFAULT;

// Read in the environment variables that control OpenSpeedShop
void SS_Init_Environment ();
