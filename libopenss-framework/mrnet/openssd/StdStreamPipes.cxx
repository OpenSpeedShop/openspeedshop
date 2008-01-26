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
 * Definition of the StdStreamPipes class.
 *
 */

#include "Assert.hxx"
#include "StdStreamPipes.hxx"

#include <unistd.h>

using namespace OpenSpeedShop::Framework;



/**
 * Default constructor.
 *
 * ...
 */
StdStreamPipes::StdStreamPipes() :
    dm_stdin_pipe_fds(),
    dm_stdout_pipe_fds(),
    dm_stderr_pipe_fds()
{
    // Setup the pipe used to write the process' stdin stream
    Assert(pipe(dm_stdin_pipe_fds) == 0);
    
    // Setup the pipe used to read the process' stdout stream
    Assert(pipe(dm_stdout_pipe_fds) == 0);

    // Setup the pipe used to read the process' stderr stream
    Assert(pipe(dm_stderr_pipe_fds) == 0);

    // TODO: implement!
}



/**
 * Destructor.
 *
 * ...
 */
StdStreamPipes::~StdStreamPipes()
{
    // TODO: implement!
}
