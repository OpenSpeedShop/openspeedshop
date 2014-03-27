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

#include "FileSystem.hxx"

#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>

using namespace std;
using namespace rapidxml;

xml_node<> *FileSystem::fileStat(const string &path, const string &file, memory_pool<> *memoryPool)
{
  xml_node<> *fileNode = memoryPool->allocate_node(node_element, memoryPool->allocate_string("File"));
  string fileName = file;

  /* Build the file path */
  string filePath(path);
  if(filePath[filePath.size()-1] != '/') {
    filePath.append("/");
  }
  filePath.append(fileName);

  /* Attempt to stat the file */  
  struct stat statBuff;
  if(stat(filePath.c_str(), &statBuff) < 0) {
    cerr << __FILE__ << ":" << __LINE__ << "\tCouldn't stat file: " << filePath << endl;
  } else {
    /* If it's a directory, append a slash as an indicator */
    if((statBuff.st_mode & S_IFMT) == S_IFDIR) {
      fileName.append("/");
    }
  }

  char *attributeValue = memoryPool->allocate_string(fileName.c_str());
  fileNode->append_attribute(memoryPool->allocate_attribute("name", attributeValue));

  return fileNode;
}

int FileSystem::dirList(const string &dirPath, vector<string> &list)
{
  DIR *directory;
  struct dirent *direntBuffer;

  directory = opendir(dirPath.c_str());
  if(!directory) {
    cerr << __FILE__ << ":" << __LINE__ << "\tCouldn't open directory: " << dirPath << endl;
    return errno;
  }

  while( (direntBuffer = readdir(directory)) ) {
    list.push_back(string(direntBuffer->d_name));
  }

  closedir(directory);

  return 0;
}

xml_node<> *FileSystem::dirStat(const string &path, memory_pool<> *memoryPool)
{
  /* Check for empty path */
  if(path.empty()) {
    char *errorWhat = memoryPool->allocate_string("Cannot stat empty path");
    return memoryPool->allocate_node(node_element, "Exception", errorWhat);
  }

  /* Make sure the path ends with a slash */
  string dirPath(path);
  if(dirPath[dirPath.size() - 1] != '/') {
    dirPath.append("/");
  }

  /* Detect missing directory */
  struct stat statBuff;
  if(stat(dirPath.c_str(), &statBuff) < 0) {
    char *errorWhat = memoryPool->allocate_string("Path not found");
    return memoryPool->allocate_node(node_element, "Exception", errorWhat);
  }

  xml_node<> *dirNode = memoryPool->allocate_node(node_element, "Dir");
  dirNode->append_attribute(memoryPool->allocate_attribute("path", memoryPool->allocate_string(dirPath.c_str())));
  
  /* Get the listing for the directory */
  vector<string> list = vector<string>();
  if(dirList(dirPath, list) == 0) {
    for(unsigned int i=0; i < list.size(); i++) {
      xml_node<> *fileNode = fileStat(dirPath, list[i], memoryPool);
      dirNode->append_node(fileNode);
    }
  }

  return dirNode;
}

xml_node<> *FileSystem::catFile(const string &path, memory_pool<> *memoryPool)
{
  /* Detect and deal with non-existent, empty or very large files */
  struct stat statBuff;
  if(stat(path.c_str(), &statBuff) < 0) {
    char *errorWhat = memoryPool->allocate_string("File not found");
    return memoryPool->allocate_node(node_element, "Exception", errorWhat);
  } else {
    if(statBuff.st_size < 0) {
      char *errorWhat = memoryPool->allocate_string("File less than empty");
      return memoryPool->allocate_node(node_element, "Exception", errorWhat);
    } else if(statBuff.st_size > 1048576 /* 1MB */) {
      char *errorWhat = memoryPool->allocate_string("File too large");
      return memoryPool->allocate_node(node_element, "Exception", errorWhat);
    }
  }

  /* Open the file, and check if it's "good" */
  ifstream file;
  file.open(path.c_str(), ifstream::in);
  if(!file.good()) {
      char *errorWhat = memoryPool->allocate_string("File not good");
      return memoryPool->allocate_node(node_element, "Exception", errorWhat);
  }

  /* Iterate over the file gathering the contents */
  xml_node<> *fileContentNode = memoryPool->allocate_node(node_element, "FileContent");
  fileContentNode->append_attribute(memoryPool->allocate_attribute("path", memoryPool->allocate_string(path.c_str())));
  stringstream fileContent;
  for(;;) {
    char c = (char)file.get();
    if(!file.good()) break;
    fileContent << c;
  }
  file.close();
  fileContentNode->value(memoryPool->allocate_string(fileContent.str().c_str()));
  
  return fileContentNode;
}

xml_node<> *FileSystem::fileExists(const string &path, memory_pool<> *memoryPool)
{
  xml_node<> *fileExistsNode = memoryPool->allocate_node(node_element, "FileExists");
  fileExistsNode->append_attribute(memoryPool->allocate_attribute("path", memoryPool->allocate_string(path.c_str())));

  struct stat statBuff;
  if(stat(path.c_str(), &statBuff) < 0) {
    fileExistsNode->append_attribute(memoryPool->allocate_attribute("exists", memoryPool->allocate_string("false")));
  } else {
    fileExistsNode->append_attribute(memoryPool->allocate_attribute("exists", memoryPool->allocate_string("true")));
  }
  
  return fileExistsNode;
}
