#ifndef EXPEXTRAFANCY
#define EXPEXTRAFANCY

#include "CmdClass.hxx"



class ExpExtraFancy : public CmdClass
{
public:
   ExpExtraFancy();
   ~ExpExtraFancy();

  void parseCmd(const char *);
  void doCmd(const char *);
};
#endif // EXPEXTRAFANCY

