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
 * Definition of the Cluster class.
 *
 */

#include "Cluster.hxx"
#include "ClusteringCriterion.hxx"
#include "Thread.hxx"
#include "ThreadGroup.hxx"

using namespace OpenSpeedShop::Framework;



/**
 * Get our clustering criterion.
 *
 * Returns the clustering criterion containing this cluster.
 *
 * @return    Clustering criterion containing this cluster.
 */
ClusteringCriterion Cluster::getClusteringCriterion() const
{
    ClusteringCriterion clustering_criterion;

    // Find our clustering criterion
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT criterion FROM Clusters WHERE id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);     
    while(dm_database->executeStatement())
        clustering_criterion = ClusteringCriterion(
            dm_database, dm_database->getResultAsInteger(1)
            );
    END_TRANSACTION(dm_database);
    
    // Return the clustering criterion to the caller
    return clustering_criterion;
}



/**
 * Get our representative thread.
 *
 * Returns the representative thread for this cluster. This is the thread that
 * contains the performance data that should be considered representative for
 * ALL threads in the cluster. I.e. every thread return by getThreads() should
 * be considered to have exhibited roughly the same performance as that of this
 * thread for our clustering metric.
 *
 * @return    Representative thread for this cluster.
 */
Thread Cluster::getRepresentativeThread() const
{
    Thread representative_thread;

    // Find our representative thread
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT representative_thread FROM Clusters WHERE id = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        representative_thread = Thread(
            dm_database, dm_database->getResultAsInteger(1)
            );
    END_TRANSACTION(dm_database);
    
    // Return the representataive thread to the caller
    return representative_thread;
}



/**
 * Get our threads.
 *
 * Returns the threads in this cluster. An empty thread group is returned if
 * this cluster doesn't contain any threads.
 *
 * @return    Threads in this cluster.
 */
ThreadGroup Cluster::getThreads() const
{
    ThreadGroup threads;

    // Find the threads in this cluster
    BEGIN_TRANSACTION(dm_database);
    validate();
    dm_database->prepareStatement(
        "SELECT thread FROM ClusterMembership WHERE cluster = ?;"
        );
    dm_database->bindArgument(1, dm_entry);
    while(dm_database->executeStatement())
        threads.insert(Thread(dm_database, dm_database->getResultAsInteger(1)));
    END_TRANSACTION(dm_database);
    
    // Return the threads to the caller
    return threads;
}



/**
 * Default constructor.
 *
 * Constructs a Cluster that referes to a non-existent cluster. Any use of
 * a member function on an object constructed in this way will result in an
 * assertion failure.
 */
Cluster::Cluster() :
    Entry()
{
}



/**
 * Constructor from a cluster entry.
 *
 * Constructs a new Cluster for the specified cluster entry.
 *
 * @param database    Database containing this cluster.
 * @param entry       Identifier for this cluster.
 */
Cluster::Cluster(const SmartPtr<Database>& database, const int& entry) :
    Entry(database, Entry::Clusters, entry)
{
}
