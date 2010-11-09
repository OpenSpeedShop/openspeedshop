////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2010 Krell Institute  All Rights Reserved.

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
 * Definition of the SS_Settings class which interfaces with the Open|SpeedShop preferences processing.
 *
 */



#ifndef SS_SETTINGS_H
#define SS_SETTINGS_H

#include <stdio.h>
#include <iostream>
#include <fstream>



class SS_Settings 
{
  public: 
    // The default constructor.  Calls initialize() with null string.
    SS_Settings( );
    // The default constructor.  Calls initialize() with filename.
    SS_Settings( char *filename );

    // The closes the file.
    ~SS_Settings();

    // The std stream for the preference file.
    std::ifstream *settings_file;

    // read a string entry from the preference file and return it's value
    std::string readEntry(std::string key, const std::string &def, bool *ok);

    // read an integer entry from the preference file and return it's value
    int64_t readNumEntry(std::string key, int64_t def, bool *ok);

    // read a double entry from the preference file and return it's value
    double readDoubleEntry(std::string key, double def, bool *ok);

    // read a bool entry from the preference file and return it's value
    bool readBoolEntry(std::string key, bool def, bool *ok);

  private:
    // Lookups up and opens the preference file.
    void initialize(char *filename);

    // given a key string, lookups up the entry in the preference file.
    std::string find(std::string key, bool *ok);

    // stores the start of the [general] section to start all preference
    // lookups.
    int64_t startOfSection;
};



#endif // SS_SETTINGS_H


