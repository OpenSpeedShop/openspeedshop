#ifndef METRICINFO_H
#define METRICINFO_H

class MetricInfo
{
public:
    MetricInfo();

    MetricInfo(int i, float p, float es, char *_functionName, char *_fileName, int fl, int s, int e);

    ~MetricInfo();

    int index;
    float percent;
    float exclusive_seconds;
    char *functionName;
    char *fileName;
    int function_line_number;
    int start;
    int end;
};
#endif // METRICINFO_H
