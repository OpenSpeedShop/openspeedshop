/*******************************************************************************
** Copyright (c) 2012-2014 Argo Navis Technologies LLC.  All Rights Reserved.
** Copyright (c) 2014 Krell Institute  All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>

#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"

#include "OpenSpeedShopCLI.hxx"
#include "FileSystem.hxx"
#include "Socket.hxx"

class SocketServer {

public:
  SocketServer(int port = 2048, const char *address = NULL);
  ~SocketServer();

protected:
  OpenSpeedShopCLI _cli;
  FileSystem _fileSystem;
  Socket _socket;

};


#endif // SOCKETSERVER_H

