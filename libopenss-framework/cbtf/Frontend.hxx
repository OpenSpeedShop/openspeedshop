////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
// Copyright (c) 2012-2015 The Krell Institute. All Rights Reserved.
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
 * Declaration of the Frontend namespace.
 *
 */

#ifndef _OpenSpeedShop_Framework_Frontend_
#define _OpenSpeedShop_Framework_Frontend_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <cstdlib>



namespace OpenSpeedShop { namespace Framework {
    /**
     * CBTF frontend.
     *
     * @ingroup Implementation
     */
    namespace Frontend
    {
#ifndef NDEBUG
	bool isDebugEnabled();
	bool isTimingDebugEnabled();
	bool isMetricDebugEnabled();
	bool isPerfDataDebugEnabled();
	bool isStdioDebugEnabled();
	bool isSymbolsDebugEnabled();
	bool isShowLoadbalanceEnabled();
#endif
    }
    
} }



#endif
