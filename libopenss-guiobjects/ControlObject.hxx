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


#ifndef CONTROLOBJECT_H
#define CONTROLOBJECT_H

#include <qvaluelist.h>

#include "MessageObject.hxx"

#ifdef CONTROL_BUTTON
enum ControlObjectType { NONE_T, ATTACH_PROCESS_T, DETACH_PROCESS_T, ATTACH_COLLECTOR_T, REMOVE_COLLECTOR_T, RUN_T, PAUSE_T, CONT_T, UPDATE_T, INTERRUPT_T, TERMINATE_T };
#else // CONTROL_BUTTON
enum ControlObjectType { NONE_T, ATTACH_PROCESS_T, DETACH_PROCESS_T, ATTACH_COLLECTOR_T, REMOVE_COLLECTOR_T, RUN_T, CONT_T, PAUSE_T, UPDATE_T, INTERRUPT_T, TERMINATE_T };
#endif // CONTROL_BUTTON

//! The message object for passing process control action selections.
class ControlObject : public MessageObject
{
public:
    //! Default contrustor.
    ControlObject();

    //! Create a ControlObject of the passed in type.
    ControlObject(ControlObjectType _cot);

    //! Destructor
    ~ControlObject();

    ControlObjectType cot;

    //! Prints debug information about this object.
    void print();
};
#endif // CONTROLOBJECT_H
