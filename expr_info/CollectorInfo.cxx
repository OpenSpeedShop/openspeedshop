#include "CollectorInfo.hxx"

#include "debug.hxx"

void
CollectorInfo::init()
{
  targetProgram = NULL;
  experimentName = NULL;
  marchingOrders = NULL;
  experimentNotes = NULL;

// Debug only!!!!!
  fudgeValues();
// End debug only!!!!!
}

CollectorInfo::CollectorInfo()
{
  dprintf("CollectorInfo::CollectorInfo(entered.\n");
  init();
}

CollectorInfo::CollectorInfo(char *experiment_name)
{
  dprintf("CollectorInfo::CollectorInfo(entered.\n");
  init();
  experimentName = strdup(experiment_name);
}

void
CollectorInfo::fudgeValues()
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
  MetricInfo *nfi;
  nfi = new MetricInfo(i, (float)44.2, (float)0.190, "foo()", "foo.c", 11, 11, 14);
  metricInfoList.push_back(nfi);
  i++;

  nfi = new MetricInfo(i, (float)34.9, (float)0.150, "init()", "fred_calls_ted.c", 15, 15, 18 );
  metricInfoList.push_back(nfi);
  i++;

  nfi = new MetricInfo(i, (float)11.6, (float)0.050, "fred()", "fred.c", 20, 20, 23);
  metricInfoList.push_back(nfi);
  i++;

  nfi = new MetricInfo(i, (float)9.3, (float)0.040, "ted()", "ted.c", 10, 10, 13);
  metricInfoList.push_back(nfi);
  i++;

  for( /* i */; i < 1024; i++ )
  {
    nfi = new MetricInfo(i, (float).000, (float)0.000, "__libcall()", "_libcall.cpp", 0, 0, 0 );
    metricInfoList.push_back(nfi);
  }
}
