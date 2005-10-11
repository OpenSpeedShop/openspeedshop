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


#ifndef PROCESSLISTOBJECT_H
#define PROCESSLISTOBJECT_H

#include <qvaluelist.h>

#include "ProcessEntryClass.hxx"

typedef QValueList<ProcessEntry *> ProcessEntryList;

class ProcessListObject
{
  public:
    ProcessListObject();
    ProcessListObject(char *host = NULL);
    ~ProcessListObject();

    ProcessEntryList processEntryList;

    void print();
  private:
    int do_ps_cmd(char *aout_name, char *arg0, int *child_pid);
    int do_ps_cmd(char *aout_name, char *arg0, char *arg1, char *arg2, int *child_pid);
    int extract_ps_list ( char *host, FILE *input, int *count, int new_pid);
    bool analyze_ps_header ( char *header, char *first_line );

    void createProcList(const char *command, char *host);
};
#endif // PROCESSLISTOBJECT_H
