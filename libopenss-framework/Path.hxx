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
 * Declaration of the Path class.
 *
 */

#ifndef _OpenSpeedShop_Framework_Path_
#define _OpenSpeedShop_Framework_Path_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>



namespace OpenSpeedShop { namespace Framework {

    /**
     * Directory or file path.
     *
     * Stores a file system directory or file path. Encapsulates many common
     * path operations that would otherwise be cumbersome using straight STL
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
     * @todo    Probably need some improvements in the portability of this
     *          class' use of the stat() function.
     */
    class Path :
	public std::string
    {
	
    public:

	Path();
	Path(const std::string&);
	Path(const char*);

	Path& operator+=(const Path&);
	Path operator+(const Path&);

	bool doesExist() const;

	bool isAbsolute() const;
	bool isDirectory() const;
	bool isFile() const;
	bool isExecutable() const;
	
	Path getDirName() const;
	Path getBaseName() const;
	Path getNormalized() const;

	Path removeFirst() const;
	Path removeLast() const;
	
    };
    
} }



#endif
