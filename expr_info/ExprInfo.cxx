#include "ExprInfo.hxx"

#include "debug.hxx"

void
ExprInfo::init()
{
  targetProgram = NULL;
  experimentName = NULL;
  marchingOrders = NULL;
  experimentNotes = NULL;

// Debug only!!!!!
  fudgeValues();
// End debug only!!!!!
}

ExprInfo::ExprInfo()
{
  dprintf("ExprInfo::ExprInfo(entered.\n");
  init();
}

ExprInfo::ExprInfo(char *experiment_name)
{
  dprintf("ExprInfo::ExprInfo(entered.\n");
  init();
  experimentName = strdup(experiment_name);
}

void
ExprInfo::fudgeValues()
{
  dprintf("Debug only!!! fudging values...\n");
  if( targetProgram == NULL )
  {
    targetProgram = strdup("generic (ia64)");
  }
  if( experimentName == NULL )
  {
    experimentName = strdup("usertime");
  }
  if( marchingOrders == NULL )
  {
    marchingOrders = strdup("ut:cu:");
  }
  if( experimentNotes == NULL )
  {
    experimentNotes = strdup("From file generic.usertime.m16957:\nCaliper point 0 at target begin, PID 16957\n/speedtest/generic/generic\nCaliper point 1 at exit(0)");
  }

  int i = 0;
  FuncInfo *nfi;
  nfi = new FuncInfo(0, (float)40, "fred()", "Example.cpp", 80, 80, 82);
  funcInfoList.push_back(nfi);
  i++;

  nfi = new FuncInfo(1, (float)20, "barney()", "Example.cpp", 455, 455, 460);
  funcInfoList.push_back(nfi);
  i++;

  nfi = new FuncInfo(2, (float)15, "arney()", "Example.cpp", 786, 786, 787);
  funcInfoList.push_back(nfi);
  i++;

  nfi = new FuncInfo(3, (float)10, "foo()", "Example.cpp", 1110, 1110, 1115 );
  funcInfoList.push_back(nfi);
  i++;

  for( /* i */; i < 1024; i++ )
  {
    nfi = new FuncInfo(3, (float).001, "foo().001", "Example.cpp", 0, 0, 0 );
    funcInfoList.push_back(nfi);
  }
}
