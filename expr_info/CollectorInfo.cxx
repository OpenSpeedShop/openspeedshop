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

  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Percent"), FLOAT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Rank"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Exclusive Time"), FLOAT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Function"), CHAR_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Filename"), CHAR_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Start Line #"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range"), INT_T));

/*
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range1"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range2"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range3"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range4"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range5"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range6"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range7"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range8"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range9"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range10"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range11"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range12"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range13"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range14"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range15"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range16"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range17"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range18"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range19"), INT_T));
  metricHeaderInfoList.push_back(new MetricHeaderInfo(QString("Range20"), INT_T));
*/

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
