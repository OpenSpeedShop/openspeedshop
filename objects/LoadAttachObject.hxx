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
    LoadAttachObject(QString executable_name = NULL, QString pid_string = NULL, QString param_list = NULL);

    //! Destructor
    ~LoadAttachObject();

    //! Prints debug information about this object.
    void print();

    //! The executable to attach to...
    QString executableName;

    //! The pid to attach to..
    QString pidStr;

    //! The list of parameters
    QString paramList;
};
#endif // LOADATTACHOBJECT_H
