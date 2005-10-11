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
      line = 0;
      value = QString::null;
      description = "none";
    };

    HighlightObject(QString fn, int l, char *c="red", QString v=QString::null, char *d="N/A")
    {
      fileName = fn;
      line = l;
      color = strdup(c);
      value = v;
      description = strdup(d);
    };

    ~HighlightObject()
    {
//      dprintf("delete HighlightObject %s\n", description);
      if( description )
      {
        free(description);
      }
      if( color )
      {
        free(color);
      }
    }

    void print()
    {
      printf("%d %s %s %s\n", line, color, value.ascii(), description);
    }

    QString fileName;
    int line;
    char *color;
    QString value;
    char *description;
};
#endif // HIGHLIGHTOBJECT_H
