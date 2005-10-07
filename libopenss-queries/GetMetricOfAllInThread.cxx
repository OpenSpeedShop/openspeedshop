////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Definition of support code for the GetMetricOfAllInThread template function.
 *
 */

#include "Queries.hxx"
#include "ToolAPI.hxx"

using namespace OpenSpeedShop;



/**
 * Get a thread's linked objects.
 * 
 * Returns the linked objects contained within the specified thread. An empty
 * set is returned if no linked objects are found.
 *
 * @param thread     Thread for which to get linked objects.
 * @param objects    Linked objects contained within this thread.
 */
void Queries::getSourceObjects(const Framework::Thread& thread,
			       std::set<Framework::LinkedObject>& objects)
{
    objects = thread.getLinkedObjects();
}



/**
 * Get a thread's functions.
 * 
 * Returns the functions contained within the specified thread. An empty set is
 * returned if no functions are found.
 *
 * @param thread     Thread for which to get functions.
 * @param objects    Functions contained within this thread.
 */
void Queries::getSourceObjects(const Framework::Thread& thread,
			       std::set<Framework::Function>& objects)
{
    objects = thread.getFunctions();
}



/**
 * Get a thread's statements.
 * 
 * Returns the statements contained within the specified thread. An empty set is
 * returned if no statements are found.
 *
 * @param thread     Thread for which to get statements.
 * @param objects    Statements contained within this thread.
 */
void Queries::getSourceObjects(const Framework::Thread& thread,
			       std::set<Framework::Statement>& objects)
{
    objects = thread.getStatements();
}
