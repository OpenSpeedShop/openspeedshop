#ifndef CMDCLASS
#define CMDCLASS

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <stdio.h>
#include <list>
#include <inttypes.h>
#include <stdexcept>
#include <string>

#include <vector>
#include <iostream>


class CmdClass
{
public:
   CmdClass();
   ~CmdClass();

  virtual void parseCmd(const char *);
  virtual void doCmd(const char *);

//  char *cmd_name;
  std::string cmd_name;
};

#endif // CMDCLASS

