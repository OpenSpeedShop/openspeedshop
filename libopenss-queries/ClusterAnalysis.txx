////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
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
 * Definition of the ClusterAnalysis namespace.
 *
 */

#ifndef _OpenSpeedShop_Queries_ClusterAnalysis_
#define _OpenSpeedShop_Queries_ClusterAnalysis_

#include "Queries.hxx"
#include "ToolAPI.hxx"



//
// To avoid requiring fully qualified names be used everywhere, something like:
//
//     using namespace OpenSpeedShop;
//     using namespace OpenSpeedShop::Framework;
//
// would normally be placed near the top of this source file. Here, however,
// that particular formulation doesn't work well. This file contains template
// definitions that are usually included directly into other source files. By
// using the above formulation, those source files would be required to have
// the same "using" clauses. And doing so would pretty much negate the whole
// reason for using namespaces in the first place. The solution is to place
// the definitions directly inside the OpenSpeedShop namespace and only
// qualify names with Framework:: where necessary.
//
namespace OpenSpeedShop {



/**
 * Average inter-cluster distance.
 *
 * Compute the average inter-cluster distance for the specified distance matrix
 * and cluster.
 *
 * @param distances    Distance matrix.
 * @param cluster      Cluster for which to compute the distance.
 * @return             Average inter-cluster distance.
 */
namespace Queries { namespace ClusterAnalysis {
template <typename TS, typename TM>
TM AverageInterClusterDistance(
    const DistanceMatrix<TS, TM >& distances,
    const Framework::ThreadGroup& cluster)
{
    Assert(!cluster.empty());
    unsigned n = 0;
    TM value = TM();
    for(Framework::ThreadGroup::const_iterator
	    i = cluster.begin(); i != cluster.end(); ++i) {
	Framework::ThreadGroup::const_iterator j = i; 
	++j;
	while(j != cluster.end()) {
	    n++;
	    value += distances.getDistance(*i, *j);
	    ++j;
	}
    }
    return value / n;
}
} }



/**
 * Manhattan individual distance measure.
 *
 * Compute the manhattan distance (sum of absolute distances for each variable)
 * for the specified individual metric results between two specified threads.
 * Source objects are the variable over which the sum is computed.
 *
 * @sa    http://en.wikipedia.org/wiki/Cluster_analysis#Distance_measure
 *
 * @param individual    Individual results for which to compute the distance.
 * @param first         First thread between which to compute the distance.
 * @param second        Second thread between which to compute the distance.
 * @return              Distance between these two threads.
 */
template <typename TS, typename TM>
TM Queries::ClusterAnalysis::ManhattanDistance(
    const std::map<TS, std::map<Framework::Thread, TM > >& individual,
    const Framework::Thread& first, const Framework::Thread& second)
{
    TM value = TM();

    // Iterate over each source object in the individual results
    for(typename std::map<TS, std::map<Framework::Thread, TM > >::const_iterator
	    i = individual.begin(); i != individual.end(); ++i) {

	// Get the metric value for the first thread
	typename std::map<Framework::Thread, TM >::const_iterator
	    jf = i->second.find(first);
	TM value_first = (jf != i->second.end()) ? jf->second : TM();

	// Get the metric value for the second thread
	typename std::map<Framework::Thread, TM >::const_iterator
	    js = i->second.find(second);
	TM value_second = (js != i->second.end()) ? js->second : TM();

	// Add the absolute value of the difference
	value += (value_first < value_second) ?
	    (value_second - value_first) : (value_first - value_second);

    }
    return value;    
}



/**
 * Matrix of individual distances.
 *
 * Template for a container holding the matrix of distances between threads.
 * Distances are computed upon object creation using the specified set of
 * individual metric results and distance measure. The list of unique threads
 * and actual distances and then be queried via member functions.
 *
 * @note    Internally the distances aren't stored in an actual matrix but
 *          rather in a mapping of the thread pair to their distance. Doing
 *          this facilitates computing and storing the distances for the
 *          unique thread pairings only.
 *
 * @sa    http://en.wikipedia.org/wiki/Distance_matrix
 */
template <typename TS, typename TM>
class Queries::ClusterAnalysis::DistanceMatrix
{

public:

    /** Constructor from indiviudal metric results. */
    DistanceMatrix(
	const std::map<TS, std::map<Framework::Thread, TM > >& individual,
	TM (*distance)(const std::map<TS, std::map<Framework::Thread, TM > >&,
		       const Framework::Thread&, const Framework::Thread&)) :
	dm_threads(),
	dm_table()
    {
	// Build a thread group containing each thread
	for(typename std::map<TS, std::map<Framework::Thread, TM > >::
		const_iterator 
		i = individual.begin(); i != individual.end(); ++i)
	    for(typename std::map<Framework::Thread, TM >::const_iterator
		    j = i->second.begin(); j != i->second.end(); ++j)
		dm_threads.insert(j->first);
	
	// Compute distances between each unique pairing of threads
	for(Framework::ThreadGroup::const_iterator
		i = dm_threads.begin(); i != dm_threads.end(); ++i) {
	    Framework::ThreadGroup::const_iterator j = i; 
	    ++j;
	    while(j != dm_threads.end()) {
		dm_table.insert(
		    std::make_pair((*i < *j) ? 
				   std::make_pair(*i, *j) : 
				   std::make_pair(*j, *i),
				   (*distance)(individual, *i, *j)
			)
		    );
		++j;
	    }
	}
    }

    /** Read-only data member accessor function. */
    const Framework::ThreadGroup& getThreads() const
    {
	return dm_threads;
    }
    
    /** Get the distance between two threads. */
    TM getDistance(const Framework::Thread& first, 
		   const Framework::Thread& second) const
    {
	if(first == second)
	    return TM();
	typename std::map<std::pair<Framework::Thread, Framework::Thread>, TM >
	    ::const_iterator i = dm_table.find((first < second) ? 
					       std::make_pair(first, second) :
					       std::make_pair(second, first));
	Assert(i != dm_table.end());
	return i->second;
    }

private:

    /** Thread group containing each thread in the original data set. */
    Framework::ThreadGroup dm_threads;
    
    /** Table of distances between each unique pairing of threads. */
    std::map<std::pair<Framework::Thread, Framework::Thread>, TM > dm_table;
    
};



/**
 * Complete linkage cluster distance measure.
 *
 * Compute the complete linkage distance (maximum distance between any two
 * elements) for the specified distance matrix between two specified clusters.
 *
 * @sa    http://en.wikipedia.org/wiki/Cluster_analysis#
 *        Agglomerative_hierarchical_clustering
 *
 * @param distances    Distance matrix.
 * @param first        First cluster between which to compute the distance.
 * @param second       Second cluster between which to compute the distance.
 * @return             Distance between these two clusters.
 */
template <typename TS, typename TM>
TM Queries::ClusterAnalysis::CompleteLinkageDistance(
    const DistanceMatrix<TS, TM >& distances,
    const Framework::ThreadGroup& first, const Framework::ThreadGroup& second)
{
    Assert(!first.empty() && !second.empty());
    bool is_initialized = false;
    TM value = TM();
    for(Framework::ThreadGroup::const_iterator
	    i = first.begin(); i != first.end(); ++i)
	for(Framework::ThreadGroup::const_iterator
		j = second.begin(); j != second.end(); ++j) {
	    TM d = distances.getDistance(*i, *j);
	    if(!is_initialized || (d > value)) {
		is_initialized = true;
		value = d;
	    }
	}
    return value;
}



/**
 * Single linkage cluster distance measure.
 *
 * Compute the single linkage distance (minimum distance between any two
 * elements) for the specified distance matrix between two specified clusters.
 *
 * @sa    http://en.wikipedia.org/wiki/Cluster_analysis#
 *        Agglomerative_hierarchical_clustering
 *
 * @param distances    Distance matrix.
 * @param first        First cluster between which to compute the distance.
 * @param second       Second cluster between which to compute the distance.
 * @return             Distance between these two clusters.
 */
template <typename TS, typename TM>
TM Queries::ClusterAnalysis::SingleLinkageDistance(
    const DistanceMatrix<TS, TM >& distances,
    const Framework::ThreadGroup& first, const Framework::ThreadGroup& second)
{
    Assert(!first.empty() && !second.empty());
    bool is_initialized = false;
    TM value = TM();
    for(Framework::ThreadGroup::const_iterator
	    i = first.begin(); i != first.end(); ++i)
	for(Framework::ThreadGroup::const_iterator
		j = second.begin(); j != second.end(); ++j) {
	    TM d = distances.getDistance(*i, *j);
	    if(!is_initialized || (d < value)) {
		is_initialized = true;
		value = d;
	    }
	}
    return value;
}



/**
 * Average linkage cluster distance measure.
 *
 * Compute the average linkage distance (average distance between any two
 * elements) for the specified distance matrix between two specified clusters.
 *
 * @sa    http://en.wikipedia.org/wiki/Cluster_analysis#
 *        Agglomerative_hierarchical_clustering
 *
 * @param distances    Distance matrix.
 * @param first        First cluster between which to compute the distance.
 * @param second       Second cluster between which to compute the distance.
 * @return             Distance between these two clusters.
 */
template <typename TS, typename TM>
TM Queries::ClusterAnalysis::AverageLinkageDistance(
    const DistanceMatrix<TS, TM >& distances,
    const Framework::ThreadGroup& first, const Framework::ThreadGroup& second)
{
    Assert(!first.empty() && !second.empty());
    TM value = TM();
    for(Framework::ThreadGroup::const_iterator
	    i = first.begin(); i != first.end(); ++i)
	for(Framework::ThreadGroup::const_iterator
		j = second.begin(); j != second.end(); ++j)
	    value += distances.getDistance(*i, *j);
    return value / (first.size() * second.size());
}



/**
 * Clustering termination predicate evalutor (numerical #1).
 *
 * Implements a cluster termination predicate that terminates the formation of
 * clusters when the number of clusters reaches a constant value (specified at
 * object construction).
 *
 * @param distances    Distance matrix.
 * @param clusters     All clusters of threads.
 * @param distance     Distance between the minimium distance pair.
 * @param first        First cluster in the minimum distance pair.
 * @param second       Second cluster in the minimum distance pair.
 * @return             Boolean "true" if cluster formation should be terminated,
 *                     or "false" otherwise.
 */
template <typename TS, typename TM>
bool Queries::ClusterAnalysis::NumberCriterion1<TS, TM >::operator()(
    const DistanceMatrix<TS, TM >& distances,
    const std::set<Framework::ThreadGroup>& clusters,
    const TM& distance,
    const Framework::ThreadGroup& first,
    const Framework::ThreadGroup& second) const
{
    return dm_count >= clusters.size();
}



/**
 * Clustering termination predicate (distance #1).
 *
 * Implements a cluster termination predicate that terminates the formation
 * of clusters when the ratio of the minimum distance between any two clusters
 * and the maximum of those cluster's average inter-cluster distance exceeds a 
 * constant value (specified at object construction).
 *
 * @param distances    Distance matrix.
 * @param clusters     All clusters of threads.
 * @param distance     Distance between the minimium distance pair.
 * @param first        First cluster in the minimum distance pair.
 * @param second       Second cluster in the minimum distance pair.
 * @return             Boolean "true" if cluster formation should be terminated,
 *                     or "false" otherwise.
 */
template <typename TS, typename TM>
bool Queries::ClusterAnalysis::DistanceCriterion1<TS, TM >::operator()(
    const DistanceMatrix<TS, TM >& distances,
    const std::set<Framework::ThreadGroup>& clusters,
    const TM& distance,
    const Framework::ThreadGroup& first,
    const Framework::ThreadGroup& second) const
{
    // Handle special case where both clusters contain a single thread
    if((first.size() < 2) && (second.size() < 2))
	return false;

    // Handle special case where only one cluster contains a single thread
    if((first.size() < 2) && (second.size() >= 2))
	return dm_ratio <= 
	    (static_cast<double>(distance) / static_cast<double>(
		AverageInterClusterDistance(distances, second)));
    if((first.size() >= 2) && (second.size() < 2))
	return dm_ratio <= 
	    (static_cast<double>(distance) / static_cast<double>( 
		AverageInterClusterDistance(distances, first)));

    // Handle default case where both clusters have more than one thread
    return dm_ratio <= 
	(static_cast<double>(distance) / static_cast<double>(
	    std::max(AverageInterClusterDistance(distances, first),
		     AverageInterClusterDistance(distances, second))));
}



/**
 * Apply cluster analysis.
 *
 * Apply agglomerative hierarchical clustering analysis to the specified
 * individual metric results using the specified individual and cluster
 * distance measures. Clustering is terminated when either there is only
 * one cluster left, or the specified termination criterion is met. Results
 * are returned as a set of thread groups.
 *
 * @pre    The specified individual metric results smart pointer must be valid.
 *         An assertion failure occurs if this pointer is null.
 *
 * @pre    The individual distance measure pointer must be valid. An assertion
 *         failure occurs if this pointer is null.
 *
 * @pre    The cluster distance measure pointer must be valid. An assertion
 *         failure occurs if this pointer is null.
 *
 * @sa    http://en.wikipedia.org/wiki/Cluster_analysis
 * 
 * @param individual    Smart pointer to the individual metric results.
 * @param idistance     Pointer to the individual distance measure to be used.
 * @param cdistance     Pointer to the cluster distance measure to be used.
 * @param criterion     Termination criteria functor.
 * @return              Clusters of threads.
 */
template <typename TS, typename TM, typename TC>
std::set<Framework::ThreadGroup> Queries::ClusterAnalysis::Apply(
    const Framework::SmartPtr<
        std::map<TS, std::map<Framework::Thread, TM > > >& individual,
    TM (*idistance)(const std::map<TS, std::map<Framework::Thread, TM > >&,
		    const Framework::Thread&, const Framework::Thread&),
    TM (*cdistance)(const DistanceMatrix<TS, TM >&, 
		    const Framework::ThreadGroup&,
		    const Framework::ThreadGroup&),
    const TC& criterion)
{
    std::set<Framework::ThreadGroup> clusters;

    // Check preconditions
    Assert(!individual.isNull());
    Assert(idistance != NULL);
    Assert(cdistance != NULL);

    // Compute the matrix of individual distances
    DistanceMatrix<TS, TM > distances(*individual, idistance);

    // Construct a cluster for each thread in the individual results
    Framework::ThreadGroup threads = distances.getThreads();
    for(Framework::ThreadGroup::const_iterator
	    i = threads.begin(); i != threads.end(); ++i)
	clusters.insert(MakeThreadGroup(*i));

    // Iterate while there is still more than one cluster
    while(clusters.size() > 1) {

	// Variables holding the minimum distance between any two clusters
	bool is_initialized = false;
	TM distance = TM();
	std::set<Framework::ThreadGroup>::iterator first = clusters.end();
	std::set<Framework::ThreadGroup>::iterator second = clusters.end();
	
	// Iterate over each unique pairing of clusters
	for(std::set<Framework::ThreadGroup>::const_iterator
		i = clusters.begin(); i != clusters.end(); ++i) {
	    std::set<Framework::ThreadGroup>::const_iterator j = i;
	    ++j;
	    while(j != clusters.end()) {

		// Compute the distance between these two clusters
		TM d = (*cdistance)(distances, *i, *j);

		// Replace the existing minimum if this distance is less
		if(!is_initialized || (d < distance)) {
		    is_initialized = true;
		    distance = d;
		    first = i;
		    second = j;
		}
		
		++j;
	    }	   
	}

	// Check assertions
	Assert((first != clusters.end()) && (second != clusters.end()));

	// Has the termination criterion been met?
	if(criterion(distances, clusters, distance, *first, *second)) {
	    // Have we reduced the number of clusters to a reasonbly small number yet?
	    // This needs to be something the user can change.  In some cases
	    // we hit the criterion very early and are left with many clusters.
	    // I suspect that the last cluster found in the early termination case
	    // may be an outlier but our current view code will print all the clusters.
	    // For a large scale job, that could mean printing 1000's of cluster data.
	    // The line below just says continue cluster reduction down to at most 4.
	    if (clusters.size() < 5) {
	        break;
	    }
	}
	
	// Replace the two original clusters with their union
	Framework::ThreadGroup temp;
	temp.insert(first->begin(), first->end());
	temp.insert(second->begin(), second->end());
	clusters.erase(first);
	clusters.erase(second);
	clusters.insert(temp);
	
    }

    // Return the clusters to the caller
    return clusters;
}



/**
 * Apply simple cluster analysis.
 *
 * Apply agglomerative hierarchical clustering analysis to the specified
 * individual metric results using the manhattan individual distance metric
 * and the average linkage cluster distance measure. Terminate the formation
 * of clusters when the ratio of the minimum distance between any two clusters
 * and the maximum of those cluster's average inter-cluster distance exceeds
 * two. Results are returned as a set of thread groups.
 * 
 * @param individual    Smart pointer to the individual metric results.
 * @return              Clusters of threads.
 */
template <typename TS, typename TM>
std::set<Framework::ThreadGroup> Queries::ClusterAnalysis::ApplySimple(
    const Framework::SmartPtr<
        std::map<TS, std::map<Framework::Thread, TM > > >& individual)
{
    DistanceCriterion1<TS, TM> criterion(2.0);
    return Apply(individual, ManhattanDistance, 
		 AverageLinkageDistance, criterion);
}



}  // namespace OpenSpeedShop



#endif
