#include "MetricInfo.hxx"
#include <stdio.h>
#include <string.h>

#include "debug.hxx"

MetricInfo::MetricInfo()
{
  dprintf("MetricInfo::MetricInfo(entered.\n");
}

MetricInfo::MetricInfo(int i, float ep, float es, char *_functionName, char *_fileName, int l, int s, int e)
{
  dprintf("MetricInfo::MetricInfo(entered.\n");
  index = i;
  percent = ep;
  exclusive_seconds = es;
  functionName = strdup(_functionName);
  fileName = strdup(_fileName);
  function_line_number = l;
  start = s;
  end = e;
}
