#ifndef PROCESSENTRYCLASS_H
#define PROCESSENTRYCLASS_H
class ProcessEntry
{
  public:
    ProcessEntry();
    ProcessEntry(char *h, int p, char *n)
    {
      host_name = strdup(h);
      pid = p;
      process_name = strdup(n);
    }
    ~ProcessEntry()
    {
      if( host_name ) free(host_name);
      if( process_name ) free(process_name);
    }
    int pid;
    char *host_name;
    char *process_name;
};
#endif // PROCESSENTRYCLASS_H
