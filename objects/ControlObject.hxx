#ifndef CONTROLOBJECT_H
#define CONTROLOBJECT_H

#include <qvaluelist.h>

#include "MessageObject.hxx"

enum ControlObjectType { NONE_T, ATTACH_PROCESS_T, DETACH_PROCESS_T, ATTACH_COLLECTOR_T, REMOVE_COLLECTOR_T, RUN_T, PAUSE_T, CONT_T, UPDATE_T, INTERRUPT_T, TERMINATE_T };

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
