#ifndef EXPFANCY
#define EXPFANCY

#include "CmdClass.hxx"



class q : public CmdClass
{
public:
   q();
   ~q();

  void parseCmd(const char *);
  void doCmd(const char *);
};
#endif // EXPFANCY

