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
    SourceObject(char *_functionName, char *_fileName, int l, bool rF=1, HighlightList *hll=NULL);

    //! Destructor
    ~SourceObject();

    //! Prints debug information about this object.
    void print();

    //! The function name to highlight
    char *functionName;

    //! The file name to load.
    char *fileName;

    //! The line number to focus.
    int line_number;

    //! Flag to determine if the reciever should raise it's panel
    //! when handling this message.
    bool raiseFLAG;

    //! The list of lines to hightlight.
    HighlightList *highlightList;
};
#endif // SOURCEOBJECT_H
