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
 * Definition of the Path class.
 *
 */

#include "Assert.hxx"
#include "Path.hxx"

#include <errno.h>
#include <pwd.h>
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#include <unistd.h>
#include <limits.h>

using namespace OpenSpeedShop::Framework;



/**
 * Default constructor.
 *
 * Constructs an empty Path ("").
 */
Path::Path() :
    std::string("")
{
}



/**
 * Constructor from a C++ string.
 *
 * Constructs a new Path from the specified standard C++ string.
 *
 * @param path    C++ string containing the path.
 */
Path::Path(const std::string& path) :
    std::string(path)
{
}



/**
 * Constructor from a C string.
 *
 * Constructs a new Path from the specified C string (i.e. a character array).
 *
 * @param path    C string containing the path.
 */
Path::Path(const char* path) :
    std::string(path)
{
}



/**
 * Addition-assignment operator.
 *
 * Operator "+=" defined for a Path object. Inserts a path separator ("/") in
 * between the two paths when necessary. 
 *
 * @param rhs    Path to be added to this path.
 * @return       Resulting, combined, path.
 */
Path& Path::operator+=(const Path& rhs)
{
    if((*(this->rbegin()) != '/') && (*(rhs.begin()) != '/'))
	this->append("/");
    this->append(rhs);
    return *this;
}



/**
 * Addition operator.
 *
 * Operator "+" defined for two Path objects. Implemented in terms of "+=".
 *
 * @param rhs    Path to be added to this path.
 * @return       Resulting, combined, path.
 */
Path Path::operator+(const Path& rhs)
{
    return Path(*this) += rhs;
}



/**
 * Test if we exist.
 *
 * Returns a boolean value indicating if this path exists. This is determined by
 * consulting the file system.
 *
 * @return    Boolean "true" if this path exists, "false" otherwise.
 */
bool Path::doesExist() const
{
    struct stat status;
    if(stat(c_str(), &status) == -1)
	return false;
    return true;
}



/**
 * Test if we are an absolute path.
 *
 * Returns a boolean value indicating if this path is an absolute path.
 *
 * @return    Boolean "true" if this path is an absolute path,
 *            "false" otherwise.
 */
bool Path::isAbsolute() const
{
    return find('/') == 0;
}



/**
 * Test if we are a directory.
 *
 * Returns a boolean value indicating if this path is a directory. This is
 * determined by consulting the file system.
 *
 * @return    Boolean "true" if this path is a directory, "false" otherwise.
 */
bool Path::isDirectory() const
{
    struct stat status;
    if(stat(c_str(), &status) == -1)
	return false;
    return S_ISDIR(status.st_mode);
}



/**
 * Test if we are a regular file.
 *
 * Returns a boolean value indicating if this path is a regular file. This is
 * determined by consulting the file system.
 *
 * @return    Boolean "true" if this path is a regular file, "false" otherwise.
 */
bool Path::isFile() const
{
    struct stat status;
    if(stat(c_str(), &status) == -1)
	return false;
    return S_ISREG(status.st_mode);
}



/**
 * Test if we are an executable file.
 *
 * Returns a boolean value indicating if this path is an executable file. This
 * is determined by consulting the file system.
 *
 * @return    Boolean "true" if this path is an executable file,
 *            "false" otherwise.
 */
bool Path::isExecutable() const
{
    struct stat status;
    if(stat(c_str(), &status) == -1)
	return false;
    return S_ISREG(status.st_mode) &&
	((status.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) != 0);
}



/**
 * Get directory name.
 *
 * Returns the directory name portion of this path, or "." if there is no
 * directory name portion.
 *
 * @return    Directory name portion of this path.
 */
Path Path::getDirName() const
{
    size_type pos = rfind('/');
    return (pos != npos) ?
	std::string(*this, 0, pos + 1) :
	std::string(".");
}



/**
 * Get base name.
 *
 * Returns the base name portion of this path.
 *
 * @return    Base name portion of this path.
 */
Path Path::getBaseName() const
{
    size_type pos = rfind('/');
    return (pos != npos) ?
	std::string(*this, pos + 1) :
	std::string(*this);
}



/**
 * Get normalized path.
 *
 * Returns a normalized version of this path. Normalization involves prepending
 * the current working directory to relative paths, expanding tilde expressions,
 * and resolving "." and ".." references.
 *
 * @return    Normalized version of this path.
 */
Path Path::getNormalized() const
{
    Path normalized;

    // Iterate over individual path components
    for(std::string::size_type 
	    i = find_first_not_of('/', 0), next = find('/', i), n = 1;
	i != std::string::npos;
	i = find_first_not_of('/', next), next = find('/', i), ++n) {
	
	// Extract this path component
	Path component =
	    substr(i, (next == std::string::npos) ? next : next - i);
	
	// Handle tilde expansion for first path component
	if((n == 1) && (component.find('~') == 0)) {

	    // Get the maximum buffer size for holding password entries
	    long buffer_size = sysconf(_SC_GETPW_R_SIZE_MAX);
	    Assert(buffer_size >= 0);
	    
	    // Allocate structures for holding the password entry
	    struct passwd pwent;
	    struct passwd* ret_pwent;
	    char buffer[buffer_size];
	    
	    // Is this expansion for the default user?
	    if(component == "~") {
		
		// Get the password file entry for our user id
		if(getpwuid_r(getuid(), &pwent, buffer,
			      buffer_size, &ret_pwent) != 0)
		    ret_pwent = NULL;
		
	    }
	    else {	    
		
		// Get the password file entry for the specified user
		if(getpwnam_r(component.substr(1, component.size() - 1).c_str(),
			      &pwent, buffer, buffer_size, &ret_pwent) != 0)
		    ret_pwent = NULL;
		
	    }

	    // Begin normalization with this user's home directory
	    normalized += (ret_pwent != NULL) ? pwent.pw_dir : component;
	    
	}
	
	// Handle relative path not beginning with tilde expansion
	else if((n == 1) && (i == 0)) {
	    
	    // Begin normalization with the current working directory
	    char buffer[PATH_MAX];
	    Assert(getcwd(buffer, sizeof(buffer)) != NULL);
	    normalized = buffer;

	}
	
	// Handle "."
	else if(component == ".")
	    continue;
	
	// Handle ".."
	else if(component == "..")
	    normalized = normalized.removeLast();
	
	// Handle everything else
	else
	    normalized += component;
	
    }
    
    // Return the normalized path to the caller
    return normalized;
}



/**
 * Get path with first component removed.
 *
 * Returns this path with its first path component removed, or "" if there is
 * only one path component remaining.
 *
 * @return    Path with first path component removed.
 */
Path Path::removeFirst() const
{
    size_type pos = find('/', 1);
    return (pos != npos) ?
	std::string(*this, pos + 1) :
	"";	
}



/**
 * Get path with last component removed.
 *
 * Returns this path with its last path component removed, or "" if there is
 * only one path component remaining.
 *
 * @return    Path with last path component removed.
 */
Path Path::removeLast() const
{
    size_type pos = rfind('/');
    return (pos != npos) ?
	std::string(*this, 0, pos) :
	"";
}
