#ifndef FUNCINFO_H
#define FUNCINFO_H

#define COLUMNS_OF_INFO 7
static char *header_strings[] = { "Percent", "Rank", "Exclusive Time", "Function", "Filename", "Start Line #", "Range" };

class FuncInfo
{
public:
    FuncInfo();

    FuncInfo(int i, float p, float es, char *_functionName, char *_fileName, int fl, int s, int e);

    ~FuncInfo();

    int index;
    float percent;
    float exclusive_seconds;
    char *functionName;
    char *fileName;
    int function_line_number;
    int start;
    int end;
};
#endif // FUNCINFO_H
