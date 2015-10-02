////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
// Copyright (c) 2012-2015 The KrellInstitute. All Rights Reserved.
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
 * Definition of the Frontend namespace.
 *
 */

#include <string>
#include "Frontend.hxx"

using namespace OpenSpeedShop::Framework;



namespace {

#ifndef NDEBUG
    /** Flag indicating if debugging for the frontend is enabled. */
    bool is_frontend_debug_enabled = false;

    /** Flag indicating if debugging for performance data is enabled. */
    bool is_perfdata_debug_enabled = false;

    /** Flag indicating if standard I/O debugging is enabled. */
    bool is_stdio_debug_enabled = false;

    /** Flag indicating if symbols debugging is enabled. */
    bool is_symbols_debug_enabled = false;

    /** Flag indicating if debugging for the frontend is enabled. */
    bool is_timing_debug_enabled = false;

    /** Flag indicating if debugging for the frontend is enabled. */
    bool is_metric_debug_enabled = false;

    /** Flag indicating if debugging for the frontend is enabled. */
    bool is_show_loadbalance = false;
#endif
    
}



#ifndef NDEBUG
/**
 * Get frontend debugging flag.
 *
 * Returns a flag indicating if debugging for the frontend is enabled.
 *
 * @return    Boolean "true" if debugging for the frontend is enabled,
 *            "false" otherwise.
 */
bool Frontend::isTimingDebugEnabled()
{
    is_timing_debug_enabled = (getenv("OPENSS_DEBUG_CBTF_TIMING") != NULL) ;
    return is_timing_debug_enabled;
}

/**
 * Get frontend debugging flag.
 *
 * Returns a flag indicating if debugging for the frontend is enabled.
 *
 * @return    Boolean "true" if debugging for the frontend is enabled,
 *            "false" otherwise.
 */
bool Frontend::isDebugEnabled()
{
    is_frontend_debug_enabled =
        (getenv("OPENSS_DEBUG_CBTF_FRONTEND") != NULL);
    return is_frontend_debug_enabled;
}



/**
 * Get performance data debugging flag.
 *
 * Returns a flag indicating if performance data debugging is enabled.
 *
 * @return    Boolean "true" if debugging for performance data is enabled,
 *            "false" otherwise.
 */
bool Frontend::isPerfDataDebugEnabled()
{
    is_perfdata_debug_enabled = (getenv("OPENSS_DEBUG_CBTF_PERFDATA") != NULL) ;
    return is_perfdata_debug_enabled;
}



/**
 * Get standard I/O debugging flag.
 *
 * Returns a flag indicating if standard I/O debugging is enabled.
 *
 * @return    Boolean "true" if debugging for standard I/O is enabled,
 *            "false" otherwise.
 */
bool Frontend::isStdioDebugEnabled()
{
    return is_stdio_debug_enabled;
}



/**
 * Get symbols debugging flag.
 *
 * Returns a flag indicating if symbols debugging is enabled.
 *
 * @return    Boolean "true" if debugging for symbols is enabled,
 *            "false" otherwise.
 */
bool Frontend::isSymbolsDebugEnabled()
{
    is_symbols_debug_enabled = (getenv("OPENSS_DEBUG_CBTF_SYMBOLS") != NULL) ;
    return is_symbols_debug_enabled;
}

/**
 * Get frontend debugging flag.
 *
 * Returns a flag indicating if debugging for the frontend is enabled.
 *
 * @return    Boolean "true" if debugging for the frontend is enabled,
 *            "false" otherwise.
 */
bool Frontend::isMetricDebugEnabled()
{
    is_metric_debug_enabled = (getenv("OPENSS_DEBUG_CBTF_METRICS") != NULL) ;
    return is_metric_debug_enabled;
}

bool Frontend::isShowLoadbalanceEnabled()
{
    is_show_loadbalance = (getenv("OPENSS_SHOW_LOADBALANCE") != NULL) ;
    return is_show_loadbalance;
}
#endif    
