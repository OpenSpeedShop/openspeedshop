#ifndef SOURCEOBJECT_H
#define SOURCEOBJECT_H

#include <qvaluelist.h>

#include "HighlightObject.hxx"
class HighlightObject;
typedef QValueList<HighlightObject *>HighlightList;

class SourceObject
{
public:
    SourceObject();
    SourceObject(char *_functionName, char *_fileName, int l, bool rF=1, HighlightList *hll=NULL);
    ~SourceObject();

    void print();

    char *functionName;
    char *fileName;
    int line_number;
    bool raiseFLAG;
    HighlightList *highlightList;
};
#endif // SOURCEOBJECT_H
