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


#ifndef UPDATEOBJECT_H
#define UPDATEOBJECT_H

#include "MessageObject.hxx"

//! The message object for update data message of an experiment
class UpdateObject : public MessageObject
{
public:
    //! Constructor for creating source message.
    UpdateObject(void *expr, int  expID, QString experiment_name, bool raiseFLAG);

    //! Destructor
    ~UpdateObject();

    //! Prints debug information about this object.
    void print();

    //! pointer to the framework experiment
    void *fw_expr;

    //! The function name to highlight
    QString experiment_name;

    //! The experiment id of the update...
    int expID;

    //! Flag to determine if the reciever should raise it's panel
    //! when handling this message.
    bool raiseFLAG;
};
#endif // UPDATEOBJECT_H
