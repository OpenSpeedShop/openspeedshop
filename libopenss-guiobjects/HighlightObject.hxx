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


#ifndef HIGHLIGHTOBJECT_H
#define HIGHLIGHTOBJECT_H

#include <stdlib.h>
#include <qvaluelist.h>
class HighlightObject;
typedef QValueList<HighlightObject *>HighlightList;


//! A simple object to describe which lines to highlight and in what color.
/*! \class HighlightObject
    This class is used by SourcePanel to highlight particular lines with 
    a specific color.
 */
class HighlightObject
{
public:
    HighlightObject()
    {
      // Default constructor.  Not used.
      funcName = QString::null;
      fileName = QString::null;
      line = 0;
      value = QString::null;
      description = "none";
      value_description = QString::null;
    };

    HighlightObject(QString _funcName, QString _fileName, int l, QString c="red", QString v=QString::null, QString d="N/A", QString vd="Stats")
    {
      funcName = _funcName;
      fileName = _fileName;
      line = l;
      color = c;
      value = v;
      description = d;
      value_description = vd;
    };

    ~HighlightObject()
    {
//      dprintf("delete HighlightObject %s\n", description);
    }

    void print()
    {
      printf("funcName:  %s\n", funcName.ascii());
      printf("fileName:  %s\n", fileName.ascii());
      printf("line:  %d\n", line);
      printf("color:  %s\n", color.isEmpty() ? "" : color.ascii() );
      printf("value:  %s\n", value.isEmpty() ? "" : value.ascii() );
      printf("description: %s\n", description.isEmpty() ? "" : description.ascii() );
    }

    QString funcName;
    QString fileName;
    int line;
    QString color;
    QString value;
    QString value_description;
    QString description;
};
#endif // HIGHLIGHTOBJECT_H
