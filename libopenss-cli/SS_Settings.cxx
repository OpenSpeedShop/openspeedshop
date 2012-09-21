////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2012 Krell Institute  All Rights Reserved.
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


#include <stdint.h>

#include "SS_Settings.hxx"

#include <iostream>
#include <fstream>
#include <string>

// For getuid(...)
#include <unistd.h>
#include <sys/types.h>

// For getpwuid(...)
#include <pwd.h>

#include <stdlib.h>



// The default constructor.   Call's initialize();
SS_Settings::SS_Settings()
{
  initialize( (char *)NULL );
}

// The constructor passed a filename.   Call's initialize();
SS_Settings::SS_Settings(char *filename)
{
  initialize( filename );
}

// The actual initializing routine.
// This routines looks up and opens the filename passed in, 
// then attempts to find the [general] section.   From there
// it stores the seek point for subsequent lookups and returns.
void
SS_Settings::initialize(char *filename)
{
  settings_file = NULL;
  startOfSection = 0;

  if( filename == NULL )
  {
    struct passwd *pw_buf = getpwuid(getuid() );
    if( pw_buf == NULL )
    {
      std::cerr << "Unable to determine user home directory." << std::endl;
      return;
    }

    std::string udstr = pw_buf->pw_dir;
    udstr += "/.qt/openspeedshoprc";

    settings_file = new std::ifstream(udstr.c_str());
  }
  else
    settings_file = new std::ifstream(filename);

  if( settings_file == NULL ||
      (settings_file->rdstate() & std::ifstream::failbit ) != 0 )
  {
//    std::cerr << "Error opening 'test.txt'\n";
  } else
  {
    std::string line;
    while( !settings_file->eof() )
    {
// std::cout << line << std::endl;
      getline (*settings_file,line);

      int64_t index = line.find("[general]", 0);
      if( index == 0 )
      {
        startOfSection = settings_file->tellg();

        break;
      }
    }
  }

}

// The destructor.  
// Close the file.
SS_Settings::~SS_Settings()
{
  if( settings_file )
  {
    settings_file->close();
  }
}

// Read a string entry.  If found, return the string.
// If unable to find the entry, set to any defaults (def) passed in.
// Upon successful look up, assigns 'ok' == 1.
std::string 
SS_Settings::readEntry(std::string key, const std::string &def, bool *ok)
{
  std::string str = find(key,ok);


  if( *ok == 0 )
  {
    str = def;
  } else
  {
    int64_t value_index = str.find("=",0);
    if( value_index != -1 )
    {
       str = std::string(str, value_index+1);
    } else
    {
      str = def;
    }
  }

  return str;
}

// Read an integer entry.  If found, return the integer value.
// If unable to find the entry, set to any defaults (def) passed in.
// Upon successful look up, assigns 'ok' == 1.
int64_t
SS_Settings::readNumEntry(std::string key, int64_t def, bool *ok)
{
  std::string str = find(key,ok);

  int64_t ret_int = def;

  if( *ok == 1 )
  {
    int64_t value_index = str.find("=",0);
    if( value_index != -1 )
    {
      std::string value_str = std::string(str, value_index+1);
      ret_int = atoll(value_str.c_str());
    }
  }

  return ret_int;
}

// Read a double entry.  If found, return the double value.
// If unable to find the entry, set to any defaults (def) passed in.
// Upon successful look up, assigns 'ok' == 1.
double 
SS_Settings::readDoubleEntry(std::string key, double def, bool *ok)
{
  std::string str = find(key,ok);

  double ret_double = def;

  if( *ok == 1 )
  {
    int64_t value_index = str.find("=",0);
    if( value_index != -1 )
    {
       std::string value_str = std::string(str, value_index+1);
       char *pend = NULL;
       ret_double = strtod(value_str.c_str(), &pend);
    }
  }

  return ret_double;
}

// Read a bool entry.  If found, return the bool value.
// If unable to find the entry, set to any defaults (def) passed in.
// Upon successful look up, assigns 'ok' == 1.
bool 
SS_Settings::readBoolEntry(std::string key, bool def, bool *ok)
{
  std::string str = find(key,ok);

  bool ret_bool = def;

  if( *ok == 1 )
  {
    int64_t value_index = str.find("=",0);
    if( value_index != -1 )
    {
       std::string value_str = std::string(str, value_index+1);
       if( value_str.find("true",0) == 0 || value_str.find("TRUE",0) == 0 || 
           value_str.find("True",0) == 0 )
       {
         ret_bool = true;
       } else if( value_str.find("false",0) == 0 || value_str.find("FALSE",0) == 0 || 
           value_str.find("False",0) == 0 )
       {
         ret_bool = false;
       } else
       {
         ret_bool = (atoll(value_str.c_str()) != 0);
       }
    }
  }

  return ret_bool;
}

// This does the actual lookup.   This attempts to set the seek point
// to the beginning of the [general] section, then read lines, for the
// [general] section only, looking for a string match.  Upon matching
// return the line the match was found.   Also set's ok=1, if a 
// match was found.
// If no match was found, sets ok=0 and returns a null string.
std::string
SS_Settings::find(std::string key, bool *ok)
{
  if( settings_file == NULL ||
      (settings_file->rdstate() & std::ifstream::failbit ) != 0 )
  {
//    std::cerr << "Error opening 'test.txt'\n";
    *ok = 0;
    return std::string();
  }

  std::string line;
  settings_file->seekg (startOfSection, std::ios::beg);
  
  while( !settings_file->eof() )
  {
    getline (*settings_file,line);
// std::cout << "(" << line << ")" << std::endl;
    if( line.size() == 0 || line.find("[",0) == 0 )
    { // We're at a new section.   Break...
      *ok = 0;
// printf("Return NULL\n");
      return std::string();
    }
    if( line.find(key, 0) == 0 )
    {
      *ok = 1;
      return line;
    }
  }

  return line;
}
