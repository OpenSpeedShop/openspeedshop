/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007-2014 Krell Institute  All Rights Reserved.
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
 * Definition of the CLI command processing routines.
 *
 */

// Utility used to find command options after parsing.
bool Look_For_KeyWord (CommandObject *cmd, std::string Key);

// One to convert time interval specifications to unix time.
bool Parse_Interval_Specification (
        CommandObject *cmd,
        ExperimentObject *exp,
        std::vector<std::pair<Time,Time> >& intervals);

// Experiment level commands
bool SS_expAttach (CommandObject *cmd);
bool SS_expClone (CommandObject *cmd);
bool SS_expClose (CommandObject *cmd);
bool SS_expCreate (CommandObject *cmd);
bool SS_expCont (CommandObject *cmd);
bool SS_expDetach (CommandObject *cmd);
bool SS_expDisable (CommandObject *cmd);
bool SS_expEnable (CommandObject *cmd);
bool SS_expFocus  (CommandObject *cmd);
bool SS_expGo (CommandObject *cmd);
bool SS_expPause (CommandObject *cmd);
bool SS_expRestore (CommandObject *cmd);
bool SS_expSave (CommandObject *cmd);
bool SS_expSetArgs (CommandObject *cmd);
bool SS_expSetParam (CommandObject *cmd);

// Information level commands
bool SS_expCompare(CommandObject *cmd);
bool SS_expStatus(CommandObject *cmd);
bool SS_expView (CommandObject *cmd);
bool SS_View (CommandObject *cmd);
bool SS_Info (CommandObject *cmd);

//Custom View Commands
bool SS_cvSet (CommandObject *cmd);
void SS_cvClear_All ();
bool SS_cvClear (CommandObject *cmd);
bool SS_cvInfo (CommandObject *cmd);
bool SS_cView (CommandObject *cmd);
bool SS_cvClusters (CommandObject *cmd);

// Primitive Information level commands
bool SS_ListGeneric (CommandObject *cmd);
bool SS_ListCviews (CommandObject *cmd);

int SS_getNumThreads (CommandObject *cmd, ExperimentObject *exp);
int SS_getNumRanks (CommandObject *cmd, ExperimentObject *exp);

// Session level commands
bool SS_ClearBreaks (CommandObject *cmd);
bool SS_Echo (CommandObject *cmd);
bool SS_Exit (CommandObject *cmd);
bool SS_Help (CommandObject *cmd);
bool SS_History (CommandObject *cmd);
bool SS_Log (CommandObject *cmd);
bool SS_OpenGui (CommandObject *cmd);
bool SS_Playback (CommandObject *cmd);
bool SS_Record (CommandObject *cmd);
bool SS_SetBreak (CommandObject *cmd);
bool SS_Wait (CommandObject *cmd);

// Place holders for commands that are not yet implemented
bool SS_no_value    	(CommandObject *cmd);
bool SS_string_value    (CommandObject *cmd);
bool SS_int_value   	(CommandObject *cmd);
