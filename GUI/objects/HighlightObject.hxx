#ifndef HIGHLIGHTOBJECT_H
#define HIGHLIGHTOBJECT_H

#include <stdlib.h>
#include <qvaluelist.h>
class HighlightObject;
typedef QValueList<HighlightObject *>HighlightList;


//! A simple object to describe which lines to highlight and in what color.
class HighlightObject
{
public:
    HighlightObject()
    {
      // Default constructor.  Not used.
      line = 0;
      description = "none";
    };

    HighlightObject(int l, char *c="red", char *d="N/A")
    {
      line = l;
      color = strdup(c);
      description = strdup(d);
    };

    ~HighlightObject()
    {
//      dprintf("delete HighlightObject %s\n", description);
      if( description )
      {
        free(description);
      }
      if( color )
      {
        free(color);
      }
    }

    void print()
    {
      printf("%d %s %s\n", line, color, description);
    }

    int line;
    char *color;
    char *description;
};
#endif // HIGHLIGHTOBJECT_H
