#ifndef PROCESSLISTOBJECT_H
#define PROCESSLISTOBJECT_H

#include <qvaluelist.h>

#include "ProcessEntryClass.hxx"

typedef QValueList<ProcessEntry *> ProcessEntryList;

class ProcessListObject
{
  public:
    ProcessListObject();
    ProcessListObject(char *host = NULL);
    ~ProcessListObject();

    ProcessEntryList processEntryList;

    void print();
  private:
    int do_ps_cmd(char *aout_name, char *arg0, int *child_pid);
    int do_ps_cmd(char *aout_name, char *arg0, char *arg1, char *arg2, int *child_pid);
    int extract_ps_list ( char *host, FILE *input, int *count, int new_pid);
    bool analyze_ps_header ( char *header, char *first_line );

    void createProcList(char *host);
};
#endif // PROCESSLISTOBJECT_H
