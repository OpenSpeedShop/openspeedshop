#ifndef SAVEASOBJECT_H
#define SAVEASOBJECT_H

#include <qfile.h>
#include "MessageObject.hxx"

//! The message object for passing SaveAs file descriptor around:w

class SaveAsObject : public MessageObject
{
public:
    //! Constructor for attaching to executable
    SaveAsObject(QString fileName = NULL);

    //! Destructor
    ~SaveAsObject();

    //! Prints debug information about this object.
    void print();

    //! The file descriptor to write to.
    QFile *f;

    //! The filename 
    QString fileName;
};
#endif // SAVEASOBJECT_H
