#ifndef PREFERENCESCHANGEDOBJECT_H
#define PREFERENCESCHANGEDOBJECT_H

#include "MessageObject.hxx"

//! The message object for updating preferences
class PreferencesChangedObject : public MessageObject
{
public:
    //! Constructor for creating source message.
    PreferencesChangedObject();

    //! Destructor
    ~PreferencesChangedObject();
};
#endif // PREFERENCESCHANGEDOBJECT_H
