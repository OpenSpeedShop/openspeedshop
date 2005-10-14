#ifndef EXPEXTRAFANCY
#define EXPEXTRAFANCY

#include "CmdClass.hxx"



class frap : public CmdClass
{
public:
   frap();
   ~frap();

  void parseCmd(const char *);
  void doCmd(const char *);
};
#endif // EXPEXTRAFANCY

