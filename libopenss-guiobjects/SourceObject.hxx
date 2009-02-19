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


#ifndef SOURCEOBJECT_H
#define SOURCEOBJECT_H

#include <qvaluelist.h>

#include "MessageObject.hxx"
#include "HighlightObject.hxx"
class HighlightObject;
typedef QValueList<HighlightObject *>HighlightList;

//! The message object for passing file/line/highlight changes to the SourcePanel
class SourceObject : public MessageObject
{
public:
    //! Unused constructor.
    SourceObject();
    //! Constructor for creating source message.
    SourceObject(QString _functionName, QString _fileName, int l, int gid = -1, bool rF=1, HighlightList *hll=NULL, int cid = -1);

    //! Destructor
    ~SourceObject();

    //! Prints debug information about this object.
    void print();

    //! The function name to highlight
    QString functionName;

    //! The file name to load.
    QString fileName;

    //! The line number to focus.
    int line_number;

    //! Group id
    int group_id;

    //! Flag to determine if the reciever should raise it's panel
    //! when handling this message.
    bool raiseFLAG;

    //! The list of lines to hightlight.
    HighlightList *highlightList;

    //! compare id
    int compare_id;
};
#endif // SOURCEOBJECT_H
