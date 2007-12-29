////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
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
 * Definition of the SentFilesTable class.
 *
 */

#include "Guard.hxx"
#include "SentFilesTable.hxx"
#include "ExperimentGroup.hxx"

using namespace OpenSpeedShop::Framework;



/** Singleton thread table. */
SentFilesTable SentFilesTable::TheTable;



/**
 * Default constructor.
 *
 * Constructs an empty sent files table.
 */
SentFilesTable::SentFilesTable() :
    Lockable(),
    std::multimap<FileName, Experiment>()
{
}



/**
 * Add sent file.
 *
 * Indicate that the specified file has been sent for the specified experiments.
 *
 * @note    An assertion failure occurs if an attempt is made to add a
 *          file/experiment pairing more than once.
 *
 * @param file           File that has been sent.
 * @param experiments    Experiments for which the file has been sent.
 */
void SentFilesTable::addSent(const FileName& file,
			     const ExperimentGroup& experiments)
{
    Guard guard_myself(this);

    // Check assertions
    for(const_iterator i = lower_bound(file); i != upper_bound(file); ++i)
	Assert(experiments.find(i->second) == experiments.end());

    // Add the specified file/experiment pairings to the table
    for(ExperimentGroup::const_iterator 
	    i = experiments.begin(); i != experiments.end(); ++i)
	insert(std::make_pair(file, *i));
}



/**
 * Get unsent experiments for a file.
 *
 * Returns the subset of the specified experiments that are unsent for the
 * specified file. An empty experiment group is returned if the file has been
 * sent for all the specified experiments.
 *
 * @param file           File to be sent.
 * @param experiments    Experiments for which the file might be sent.
 * @return               Subset of experiments for which the file is unsent.
 */
ExperimentGroup
SentFilesTable::getUnsent(const FileName& file,
			  const ExperimentGroup& experiments) const
{
    Guard guard_myself(this);
    
    // Find the unsent subset of experiments for the specified file
    ExperimentGroup unsent(experiments);
    for(const_iterator i = lower_bound(file); i != upper_bound(file); ++i)
	unsent.erase(i->second);
    
    // Return the unsent experiments to the caller
    return unsent;
}
