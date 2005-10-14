#ifndef EXPFANCY
#define EXPFANCY

#include "CmdClass.hxx"



class ExpFancy : public CmdClass
{
public:
   ExpFancy();
   ~ExpFancy();

  void parseCmd(const char *);
  void doCmd(const char *);
};
#endif // EXPFANCY

