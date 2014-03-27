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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <cstring>
#include <cerrno>

#include "Socket.hxx"

Socket::Socket()
{
  _socketDescriptor = -1;
}

Socket::~Socket()
{
}

bool Socket::start(int port, const char *address)
{
  struct sockaddr_in serverAddress;

  std::cerr << __FILE__ << ":" << __LINE__ << "\t\tClearing sockaddr structure" << std::endl;
  std::memset(&serverAddress, 0, sizeof(serverAddress));

  std::cerr << __FILE__ << ":" << __LINE__ << "\t\tSetting socket address type" << std::endl;
  serverAddress.sin_family = AF_INET;

  std::cerr << __FILE__ << ":" << __LINE__ << "\t\tSetting socket address" << std::endl;
  unsigned long s_addr = 0;
  if(address != NULL) {
    s_addr = inet_addr(address);
  }
  if(s_addr == 0) {
    s_addr = INADDR_ANY;
  }
  serverAddress.sin_addr.s_addr = s_addr;

  std::cerr << __FILE__ << ":" << __LINE__ << "\t\tSetting socket port" << std::endl;
  serverAddress.sin_port = htons(port);

  /* Create the socket */ 
  std::cerr << __FILE__ << ":" << __LINE__ << "\t\tCreating socket" << std::endl;
  _socketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(_socketDescriptor < 0) {
    std::cerr << __FILE__ << ":" << __LINE__ << "\t\tError while opening socket: " << strerror(errno) << std::endl;
    return false;
  }

  int on = 1;
  if(setsockopt(_socketDescriptor, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on)) < 0) {
    std::cerr << __FILE__ << ":" << __LINE__ << "\t\tError setting socket options: " << strerror(errno) << std::endl;
    return false;
  }

#if 0
  struct timeval tv;
  tv.tv_usec = 0;
  tv.tv_sec = 30;
  if(setsockopt(_socketDescriptor, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval)) < 0) {
    std::cerr << __FILE__ << ":" << __LINE__ << "\t\tError setting socket timeout options: " << strerror(errno) << std::endl;
    return false;
  }
#endif

  std::cerr << __FILE__ << ":" << __LINE__ << "\t\tBinding socket" << std::endl;
  if(bind(_socketDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
    std::cerr << __FILE__ << ":" << __LINE__ << "\t\tError binding socket to hostname: " << strerror(errno) << std::endl;
    return false;
  }

  std::cerr << __FILE__ << ":" << __LINE__ << "\t\tBeginning socket listen" << std::endl;
  if(listen(_socketDescriptor, 5) < 0) {
    std::cerr << __FILE__ << ":" << __LINE__ << "\t\tError listening socket: " << strerror(errno) << std::endl;
    return false;
  }

  return true;
}

bool Socket::accept(Socket &clientConnection)
{
  struct sockaddr_in clientAddress;
  socklen_t clientAddressSize = sizeof(clientAddress);

  std::cerr << __FILE__ << ":" << __LINE__ << "\t\tBeginning connection accept" << std::endl;
  clientConnection._socketDescriptor = ::accept(_socketDescriptor, (struct sockaddr *) &clientAddress, &clientAddressSize);
  if(clientConnection._socketDescriptor < 0) {
    std::cerr << __FILE__ << ":" << __LINE__ << "\t\tError while accepting connection: " << strerror(errno) << std::endl; 
    return false;
  }
  
  return true;
}

bool Socket::close()
{
  if(::close(_socketDescriptor) < 0) {
    std::cerr << __FILE__ << ":" << __LINE__ << "\t\tError while accepting connection: " << strerror(errno) << std::endl;
    return false;
  }
  return true;
}

bool Socket::send(std::string str)
{
  std::cerr << __FILE__ << ":" << __LINE__ << "\t\tSending data" << std::endl;

  uint32_t header = htonl((uint32_t)str.size());
  unsigned char headerArray[sizeof(header)];
  std::memcpy(headerArray, &header, sizeof(header));

  if(::send(_socketDescriptor, headerArray, sizeof(headerArray), MSG_NOSIGNAL) < 0) {
    std::cerr << __FILE__ << ":" << __LINE__ << "\t\tError while sending header: " << strerror(errno) << std::endl;
    return false;
  } else {
    if(::send(_socketDescriptor, str.c_str(), str.size(), MSG_NOSIGNAL) < 0) {
      std::cerr << __FILE__ << ":" << __LINE__ << "\t\tError while sending data: " << strerror(errno) << std::endl;
      return false;
    }
  }

  return true;
}

int Socket::recv(std::string &str)
{
  int retval = 0;
  str = "";

  int bufferSize = 64*1024;
  char buffer[bufferSize + 1];
  std::memset(buffer, 0, bufferSize + 1);

  std::cerr << __FILE__ << ":" << __LINE__ << "\t\tReceiving data" << std::endl;
  retval = ::recv(_socketDescriptor, buffer, bufferSize, 0);
  if(retval < 0) {
    std::cerr << __FILE__ << ":" << __LINE__ << "\t\tError while recieving data: " << strerror(errno) << std::endl;
    return -1;
  }

  str = buffer;
  return retval;
}

