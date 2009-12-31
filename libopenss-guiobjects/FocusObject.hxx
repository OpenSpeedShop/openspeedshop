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


#ifndef FOCUSOBJECT_H
#define FOCUSOBJECT_H

#include "MessageObject.hxx"
#include <map>
#include <vector>

#include "DescriptionClassObject.hxx"

//! The message object for focusing an experiment
class FocusObject : public MessageObject
{
public:
    //! Constructor for creating source message.
    FocusObject(int  expID, QString host_name, QString pidString, QString threadNameString, QString rankNameString, bool rf=FALSE, bool rankf=FALSE);

    //! Destructor
    ~FocusObject();

    //! Prints debug information about this object.
    void print();


    //! The experiment id
    int expID;

    //! The host name
    QString host_name;

    //! The pid name
    QString pidString;

    //! The thread name 
    QString threadNameString;

    //! The rank name 
    QString rankNameString;

    QValueList<DescriptionClassObject> descriptionClassList;

    //! The raise FLAG...
    bool raiseFLAG;

    //! The focus on rank only FLAG...
    bool focusOnRankOnlyFLAG;
};
#endif // FOCUSOBJECT_H
