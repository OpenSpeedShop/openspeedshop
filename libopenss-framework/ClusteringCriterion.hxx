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
 * Declaration of the ClusteringCriterion class.
 *
 */

#ifndef _OpenSpeedShop_Framework_ClusteringCriterion_
#define _OpenSpeedShop_Framework_ClusteringCriterion_

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
     * Data clustering criterion.
     *
     * Representation of a data clustering criterion. Provides member functions
     * for requesting information regarding this clustering criterion and what
     * clusters it contains.
     */
    class ClusteringCriterion :
        public Entry
    {
        friend class Cluster;
        friend class Experiment;
        
    public:
        
        std::string getName() const;
        std::set<Cluster> getClusters() const;
        
    private:

        ClusteringCriterion();
        ClusteringCriterion(const SmartPtr<Database>&, const int&);
        
    };
    
} }



#endif
