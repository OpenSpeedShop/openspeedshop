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
 * Declaration of the ClusteringMetric class.
 *
 */

#ifndef _OpenSpeedShop_Framework_ClusteringMetric_
#define _OpenSpeedShop_Framework_ClusteringMetric_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Entry.hxx"

#include <set>
#include <string>



namespace OpenSpeedShop { namespace Framework {

    class Cluster;
    class Database;
    class Experiment;
    template <typename> class SmartPtr;
    
    /**
     * Data clustering metric.
     *
     * Representation of a data clustering metric. Provides member functions for
     * requesting information about this metric and what clusters it contains.
     */
    class ClusteringMetric :
        public Entry
    {
        friend class Cluster;
        friend class Experiment;
        
    public:
        
        std::string getName() const;
        std::set<Cluster> getClusters() const;
        
    private:

        ClusteringMetric();
        ClusteringMetric(const SmartPtr<Database>&, const int&);
        
    };
    
} }



#endif
