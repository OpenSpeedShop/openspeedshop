#ifndef SAVEASOBJECT_H
#define SAVEASOBJECT_H

#include <qfile.h>
#include <qtextstream.h>
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

    //! The text stream to write...
    QTextStream *ts;

    //! The filename 
    QString fileName;
};
#endif // SAVEASOBJECT_H
