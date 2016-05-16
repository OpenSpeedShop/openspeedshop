////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016 Argo Navis Technologies. All Rights Reserved.
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
 * Definition of the ClusteringCriterion class.
 *
 */

#include "Cluster.hxx"
#include "ClusteringCriterion.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get our name.
 *
 * Returns the name of this clustering criterion.
 *
 * @return    Name of this clustering criterion.
 */
std::string ClusteringCriterion::getName() const
{
    std::string name;
    
    // Find our name
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT name FROM ClusteringCriteria WHERE id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        name = dm_database->getResultAsString(1);
    END_TRANSACTION(dm_database);
    
    // Return the name to the caller
    return name;    
}



/**
 * Get our clusters.
 *
 * Returns the clusters for this clustering criterion.
 *
 * @return    Clusters for this clustering criterion.
 */
std::set<Cluster> ClusteringCriterion::getClusters() const
{
    std::set<Cluster> clusters;

    // Find our clusters
    BEGIN_TRANSACTION(dm_database);    
    dm_database->prepareStatement(
        "SELECT id FROM Clusters WHERE criterion = ?;"
        );
    while(dm_database->executeStatement())
        clusters.insert(
            Cluster(dm_database, dm_database->getResultAsInteger(1))
            );
    END_TRANSACTION(dm_database);
    
    // Return the clusters to the caller
    return clusters;
}



/**
 * Default constructor.
 *
 * Constructs a ClusteringCriterion that refers to a non-existent clustering
 * criterion. Any use of a member function on an object constructed in this
 * way will result in an assertion failure.
 */
ClusteringCriterion::ClusteringCriterion() :
    Entry()
{
}



/**
 * Constructor from a clustering criterion entry.
 *
 * Constructs a new ClusteringCriterion for the specified clustering criterion
 * entry.
 *
 * @param database    Database containing this clustering criterion.
 * @param entry       Identifier for this clustering criterion.
 */
ClusteringCriterion::ClusteringCriterion(const SmartPtr<Database>& database,
                                   const int& entry) :
    Entry(database, Entry::ClusteringCriteria, entry)
{
}
