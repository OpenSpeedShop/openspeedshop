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
    targetProgram = strdup("fred_calls_ted (ia32)");
  }
  if( experimentName == NULL )
  {
    experimentName = strdup("pc Sampling");
  }
  if( marchingOrders == NULL )
  {
    marchingOrders = strdup("n/a");
  }
  if( experimentNotes == NULL )
  {
    experimentNotes = strdup("From file fred_calls_ted.pcsamp\n");
  }

  int i = 0;
  FuncInfo *nfi;
  nfi = new FuncInfo(i, (float)44.2, (float)0.190, "foo()", "foo.c", 11, 11, 14);
  funcInfoList.push_back(nfi);
  i++;

  nfi = new FuncInfo(i, (float)34.9, (float)0.150, "init()", "fred_calls_ted.c", 15, 15, 18 );
  funcInfoList.push_back(nfi);
  i++;

  nfi = new FuncInfo(i, (float)11.6, (float)0.050, "fred()", "fred.c", 20, 20, 23);
  funcInfoList.push_back(nfi);
  i++;

  nfi = new FuncInfo(i, (float)9.3, (float)0.040, "ted()", "ted.c", 10, 10, 13);
  funcInfoList.push_back(nfi);
  i++;

  for( /* i */; i < 1024; i++ )
  {
    nfi = new FuncInfo(i, (float).000, (float)0.000, "__libcall()", "_libcall.cpp", 0, 0, 0 );
    funcInfoList.push_back(nfi);
  }
}
