////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2004 Silicon Graphics, Inc. All Rights Reserved.
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
 * Declaration and definition of the Path class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Path_
#define _OpenSpeedShop_Framework_Path_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Assert.hxx"

#include <errno.h>
#include <string>
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif



namespace OpenSpeedShop { namespace Framework {

    /**
     * Directory or file path.
     *
     * Stores a file system directory or file path. Encapsulates many common
     * path operations that would otherwise be cubersome using straight STL
     * strings.
     *
     * @note    This class inherits from std::string which is considered a
     *          concrete class (it has a non-virtual destructor). Normally
     *          this would be a bad thing to do. It is OK here because this
     *          class doesn't have any private data members of its own and
     *          thus doesn't need its own destructor.
     *
     * @ingroup Utility
     *
     * @todo    Probably need improvements here in the portability of using the
     *          stat() function.
     */
    class Path :
	public std::string
    {
	
    public:

	/** Default constructor. */
	Path() :
	    std::string("")
	{
	}

	/** Constructor from a std::string. */
	Path(const std::string& path) :
	    std::string(path)
	{
	}

	/** Constructor from a C character array. */
	Path(const char* path) :
	    std::string(path)
	{
	}

	/* Operator "+=" defined for two Path objects. */
	Path& operator+=(const Path& rhs)
	{
	    if((*(this->rbegin()) != '/') && (*(rhs.begin()) != '/'))
		this->append("/");
	    this->append(rhs);
	    return *this;
	}

	/** Operator "+" defined in terms of "+=". */
	Path operator+(const Path& rhs)
	{
	    return Path(*this) += rhs;
	}
	
	/** Test if path exists. */
	bool doesExist() const
	{
	    struct stat status;
	    if(stat(c_str(), &status) == -1) {
		Assert(errno == ENOENT);
		return false;
	    }
	    return true;
	}

	/** Test if this path is absolute. */
	bool isAbsolute() const
	{
	    return find('/') == 0;
	}

	/** Test if this path is a directory. */
	bool isDirectory() const
        {
	    struct stat status;
	    if(stat(c_str(), &status) == -1) {
		Assert(errno == ENOENT);
		return false;
	    }
	    return S_ISDIR(status.st_mode);
	}

	/** Test if this path is a regular file. */
	bool isFile() const
	{
	    struct stat status;
	    if(stat(c_str(), &status) == -1) {
		Assert(errno == ENOENT);
		return false;
	    }	    
	    return S_ISREG(status.st_mode);
	}

	/** Obtain directory name portion of this path. */
	Path dirName() const
	{
	    size_type pos = rfind('/');
	    return (pos != npos) ?
		std::string(*this, 0, pos + 1) :
		std::string(".");
	}

	/** Obtain base name portion of this path. */
	Path baseName() const
	{
	    size_type pos = rfind('/');
	    return (pos != npos) ?
		std::string(*this, pos + 1) :
		std::string(*this);
	}

	/** Obtain this path with the first path component removed. */
	Path removeFirst() const
	{
	    size_type pos = find('/', 1);
	    return (pos != npos) ?
		std::string(*this, pos + 1) :
		"";	
	}

	/** Obtain this path with last path component removed. */
	Path removeLast() const
	{
	    size_type pos = rfind('/');
	    return (pos != npos) ?
		std::string(*this, 0, pos) :
		"";
	}

    };

} }



#endif
