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

#include <iostream>
#include <string>
#include <vector>

#include "rapidxml-1.13/rapidxml.hpp"

#define DEBUG

using namespace std;
using namespace rapidxml;

class FileSystem {
public:
  FileSystem() {}
  FileSystem(FileSystem &other) {}
  ~FileSystem() {}

  xml_node<> *dirStat(const string &path, memory_pool<> *memoryPool);
  xml_node<> *fileStat(const string &path, const string &file, memory_pool<> *memoryPool);
  xml_node<> *catFile(const string &path, memory_pool<> *memoryPool);
  xml_node<> *fileExists(const string &path, memory_pool<> *memoryPool);

protected:
  int dirList(const string &path, vector<string> &list);
};

