/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2010 Krell Institute  All Rights Reserved.
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
 * Definition of the Command control switch SS_Execute_Cmd and externs.
 *
 */


// The main switch that implements the commands,
// once we've figured out which one we were given.
void SS_Execute_Cmd (CommandObject *cmd);

// The number of Command Execution Threads that have been created.
extern int64_t EXT_Created;
extern bool isOfflineCmd;

// These routines are needed to control safe execution of commands.
extern void Wait_For_Previous_Cmds ();
extern void SafeToDoNextCmd ();
