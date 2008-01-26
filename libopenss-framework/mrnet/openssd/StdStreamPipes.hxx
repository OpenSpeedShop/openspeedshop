////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008 William Hachfeld. All Rights Reserved.
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
 * Declaration of the StdStreamPipes class.
 *
 */

#ifndef _OpenSpeedShop_Framework_StdStreamPipes_
#define _OpenSpeedShop_Framework_StdStreamPipes_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif



namespace OpenSpeedShop { namespace Framework {

    /**
     * Standard stream pipes.
     *
     * Encapsulates a set of pipes, and their associated file descriptors, used
     * to write/read data to/from a Dyninst-created process' stdin, stdout, and
     * stderr streams ...
     *
     * @ingroup Implementation
     */
    class StdStreamPipes
    {
	
    public:

	StdStreamPipes();
	~StdStreamPipes();

	/** Read-only data member acessor function. */
	int getStdinForCreatedProcess() const
	{
	    return dm_stdin_pipe_fds[0];
	}

	/** Read-only data member acessor function. */
	int getStdoutForCreatedProcess() const
	{
	    return dm_stdout_pipe_fds[1];
	}

	/** Read-only data member acessor function. */
	int getStderrForCreatedProcess() const
	{
	    return dm_stderr_pipe_fds[1];
	}

    private:

	/** Pipe used for writing a Dyninst-created process' stdin stream. */
	int dm_stdin_pipe_fds[2];
	
	/** Pipe used for reading a Dyninst-created process' stdout stream. */
	int dm_stdout_pipe_fds[2];
	
	/** Pipe used for reading a Dyninst-created process' stderr stream. */
	int dm_stderr_pipe_fds[2];

    };

} }



#endif
