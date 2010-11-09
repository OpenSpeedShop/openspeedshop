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
 * Definition of the Watcher related events/externs.
 *
 */


// Panic call to get rid of waits.  Useful for CNTRL-C processing where the
// goal is to get things out of the way so that the user can get control back.
// Call this AFTER setting Input_Line_Status for all commands to ILO_ERROR
// and AFTER setting Command_Status for all commands to CMD_ABORTED.
extern void Purge_Watcher_Events ();
extern void Purge_Watcher_Waits ();

// To let provide feed back to interactive users
extern void Request_Async_Notice_Of_Termination (CommandObject *cmd, ExperimentObject *exp);
extern void Cancle_Async_Notice (ExperimentObject *exp);

// To force command processing to wait for an experiment to
// complete execution.
extern void Wait_For_Exp (CommandObject *cmd, ExperimentObject *exp);
extern void Cancle_Exp_Wait (ExperimentObject *exp);
