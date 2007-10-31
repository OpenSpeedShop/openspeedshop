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
 * Definition of the Senders namespace.
 *
 */

#include "Protocol.h"
#include "Senders.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Attached to a thread.
 *
 * ...
 */
void Senders::attachedToThread()
{
    // TODO: implement!
}



/**
 * Value of an integer global variable.
 *
 * Issue a message to the frontend to return the current value of a signed
 * integer global variable within a thread.
 */
void Senders::globalIntegerValue()
{
    // TODO: implement!
}



/**
 * Value of a job description variable.
 *
 * Issue a message to the frontend to return the current value of a job
 * description global variable within a thread.
 */
void Senders::globalJobValue()
{
    // TODO: implement!
}



/**
 * Value of a string global variable.
 *
 * Issue a message to the frontend to return the current value of a character
 * string global variable within a thread.
 */
void Senders::globalStringValue()
{
    // TODO: implement!
}



/**
 * Linked object has been loaded.
 *
 * Issue a message to the frontend to indicate that the specified linked object
 * has been loaded into the address space of the specified threads. Includes the
 * time at which the load occured as well as a description of what was loaded.
 */
void Senders::loadedLinkedObject()
{
    // TODO: implement!
}



/**
 * Report an error.
 *
 * ...
 */
void Senders::reportError()
{
    // TODO: implement!
}



/**
 * Symbol table.
 *
 * Issue a message to the frontend to provide the symbol table for a single
 * linked object.
 */
void Senders::symbolTable()
{
    // TODO: implement!
}



/**
 * Thread's state has changed.
 *
 * Issue a message to the frontend to indicate that the current state of
 * every thread in the specified group has changed to the specified value.
 */
void Senders::threadsStateChanged()
{
    // TODO: implement!
}



/**
 * Linked object has been unloaded.
 *
 * Issue a message to the frontend to indicate that the specified linked object
 * has been unloaded from the address space of the specified threads. Includes
 * the time at which the unload occured as well as a description of what was
 * unloaded.
 */
void Senders::unloadedLinkedObject()
{
    // TODO: implement!
}
