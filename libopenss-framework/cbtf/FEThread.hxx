////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012 The Krell Institute. All Rights Reserved.
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
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/thread.hpp>

/*using namespace boost;*/

/**
 * main thread for the cbtf instrumentor.
 */


class FEThread
{
  public:

  FEThread();

  void start( const std::string& collector, const unsigned int& numBE,
	     bool& finished);
  void start( const std::string& topology, const std::string& connections,
	      const std::string& collector, const unsigned int& numBE,
	     bool& finished);

  void join();

  void run(const std::string& topology, const std::string& connections,
	   const std::string& collector, const unsigned int& numBE,
	   bool& finished);
  private:
	boost::thread dm_thread;
};
