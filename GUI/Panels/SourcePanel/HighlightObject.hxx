#ifndef HIGHLIGHTOBJECT_H
#define HIGHLIGHTOBJECT_H

class HighlightObject
{
public:
    HighlightObject()
    {
      // Default constructor.  Not used.
      line = 0;
      description = "none";
    };

    HighlightObject(int l, char *d)
    {
      line = l;
      description = strdup(d);
    };

    ~HighlightObject()
    {
//      dprintf("delete HighlightObject %s\n", description);
      if( description )
      {
        free(description);
      }
    }

    void print()
    {
      printf("%d %s\n", line, description);
    }

    int line;
    char *description;
};
#endif // HIGHLIGHTOBJECT_H
