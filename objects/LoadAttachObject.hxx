#ifndef LOADATTACHOBJECT_H
#define LOADATTACHOBJECT_H

#include <qvaluelist.h>

#include "MessageObject.hxx"

//! The message object for passing file/line/highlight changes to the SourcePanel
class LoadAttachObject : public MessageObject
{
public:
    //! Unused constructor.
    LoadAttachObject();
    //! Constructor for attaching to executable
    LoadAttachObject(char *executable_name = NULL, char *pid_string = NULL);

    //! Destructor
    ~LoadAttachObject();

    //! Prints debug information about this object.
    void print();

    //! The executable to attach to...
    char *executableName;

    //! The pid to attach to..
    char *pidStr;
};
#endif // LOADATTACHOBJECT_H
