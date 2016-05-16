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
 * Declaration of the Cluster class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Cluster_
#define _OpenSpeedShop_Framework_Cluster_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Entry.hxx"



namespace OpenSpeedShop { namespace Framework {

    class ClusteringCriterion;
    class Database;
    template <typename> class SmartPtr;
    class Thread;
    class ThreadGroup;
    
    /**
     * Data cluster.
     *
     * Representation of a data cluster. Provides member functions for
     * requesting information about this cluster and what it contains.
     */
    class Cluster :
        public Entry
    {
        friend class ClusteringCriterion;
        
    public:
        
        ClusteringCriterion getClusteringCriterion() const;
        
        Thread getRepresentativeThread() const;
        ThreadGroup getThreads() const;
        
    private:

        Cluster();
        Cluster(const SmartPtr<Database>&, const int&);
        
    };
    
} }



#endif
