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
 * Declaration of the Queries namespace.
 *
 */

#ifndef _OpenSpeedShop_Framework_Queries_
#define _OpenSpeedShop_Framework_Queries_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "SmartPtr.hxx"

#include <map>
#include <string>



namespace OpenSpeedShop { namespace Framework {

    class Collector;
    class Function;
    class Statement;
    class Thread;

    /**
     * High-level queries.
     *
     * Namespace containing procedural functions for performing high-level
     * queries on experiments. There is really nothing "magical" about these
     * functions. They use the same Tool API that is available directly to
     * every tool. The intention is simply to promote re-use by providing
     * some of the more commonly used queries.
     *
     * @todo    Currently these queries make the assumption that a given source
     *          entity (function, statement, etc.) does not change addresses
     *          during the lifetime of the thread. This is not, of course,
     *          necessarily true in the presence of dlopen() and dlclose().
     *          While the database, etc. is setup to handle the reality of
     *          moving shared libraries, the interface into this is awkward at
     *          present. In the future this assumption should be removed.
     *
     * @ingroup ToolAPI
     */
    namespace Queries
    {

	void GetMetricByFunctionInThread(
	    const Collector&, const std::string&,
	    const Thread&,
	    SmartPtr<std::map<Function, double> >&
	    );

	void GetMetricByStatementInFunction(
	    const Collector&, const std::string&,
	    const Function&,
	    SmartPtr<std::map<Statement, double> >&
	    );
	
    }
    
} }



#endif
