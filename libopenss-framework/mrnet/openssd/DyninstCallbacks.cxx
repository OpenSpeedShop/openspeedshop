////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Definition of the DyninstCallbacks namespace.
 *
 */

#include "DyninstCallbacks.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * ...
 *
 * ...
 *
 * @param thread     ...
 * @param module     ...
 * @param is_load    ...
 */
void DyninstCallbacks::dynLibrary(BPatch_thread* thread,
				  BPatch_module* module,
				  bool is_load)
{
    // TODO: implement!
}



/**
 * ...
 *
 * ...
 *
 * @param severity      ...
 * @param number        ...
 * @param parameters    ...
 */
void DyninstCallbacks::error(BPatchErrorLevel severity, int number,
			     const char* const* parameters)
{
    // TODO: implement!
}



/**
 * ...
 *
 * ...
 *
 * @param thread    ...
 */
void DyninstCallbacks::exec(BPatch_thread* thread)
{
    // TODO: implement!
}



/**
 * ...
 *
 * ...
 *
 * @param thread       ...
 * @param exit_type    ...
 */
void DyninstCallbacks::exit(BPatch_thread* thread, BPatch_exitType exit_type)
{
    // TODO: implement!
}



/**
 * ...
 *
 * ...
 *
 * @param parent    ...
 * @param child     ...
 */
void DyninstCallbacks::postFork(BPatch_thread* parent, BPatch_thread* child)
{
    // TODO: implement!
}



/**
 * ...
 *
 * ...
 *
 * @param process    ...
 * @param thread     ...
 */
void DyninstCallbacks::threadCreate(BPatch_process* process,
				    BPatch_thread* thread)
{
    // TODO: implement!
}



/**
 * ...
 *
 * ...
 *
 * @param process    ...
 * @param thread     ...
 */
void DyninstCallbacks::threadDestroy(BPatch_process* process,
				     BPatch_thread* thread)
{
    // TODO: implement!
}
