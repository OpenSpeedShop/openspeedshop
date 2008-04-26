////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
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
 * Declaration of the FileName class.
 *
 */

#ifndef _OpenSpeedShop_Framework_FileName_
#define _OpenSpeedShop_Framework_FileName_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Path.hxx"
#include "Protocol.h"
#include "TotallyOrdered.hxx"
#include "Utility.hxx"

#include <BPatch.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif



namespace OpenSpeedShop { namespace Framework {

    /**
     * File name.
     *
     * Names a single file by providing the full path name of that file. A
     * checksum is also provided, which allows the frontend to verify the
     * availability of the correct file.
     *
     * @ingroup Implementation
     */
    class FileName :
	public TotallyOrdered<FileName>
    {

    public:

	/** Default constructor. */
	FileName() :
	    dm_path(),
	    dm_checksum(0)
	{
	}

	/** Constructor from a path. */
	FileName(const Path& path) :
	    dm_path(path),
	    dm_checksum(0)
	{
	    dm_checksum = computeChecksum(dm_path);
	}

	/** Constructor from a BPatch_image object. */
	FileName(/* const */ BPatch_image& image) :
	    dm_path(),
	    dm_checksum(0)
	{
	    char buffer[PATH_MAX];
	    image.getProgramFileName(buffer, sizeof(buffer));
	    dm_path = buffer;
	    dm_checksum = computeChecksum(dm_path);
	}

	/** Constructor from a BPatch_module object. */
	FileName(/* const */ BPatch_module& module) :
	    dm_path(),
	    dm_checksum(0)
	{
	    char buffer[PATH_MAX];
	    module.getFullName(buffer, sizeof(buffer));
	    dm_path = buffer;
	    dm_checksum = computeChecksum(dm_path);
	}

	/** Constructor from a OpenSS_Protocol_FileName object. */
	FileName(const OpenSS_Protocol_FileName& object) :
	    dm_path(object.path),
	    dm_checksum(object.checksum)
	{
	}

	/** Type conversion to a OpenSS_Protocol_FileName object. */
	operator OpenSS_Protocol_FileName() const
	{
	    OpenSS_Protocol_FileName object;
	    convert(dm_path, object.path);
	    object.checksum = dm_checksum;
	    return object;
	}
	
	/** Operator "<" defined for two FileName objects. */
	bool operator<(const FileName& other) const
	{
	    // TODO: use checksums only once they are actually computed
	    // return dm_checksum < other.dm_checksum;
	    return dm_path < other.dm_path;
	}
	
	/** Read-only data member accessor function. */
	const Path& getPath() const
	{
	    return dm_path;
	}
	
	/** Read-only data member accessor function. */
	const uint64_t& getChecksum() const
	{
	    return dm_checksum;
	}
	
    private:
	
	/** Full path name of the file. */
	Path dm_path;
	
	/** Checksum calculated on this file. */
	uint64_t dm_checksum;
	
    };
    
} }



#endif
