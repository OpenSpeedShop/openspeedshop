#include <iostream>

using namespace std;

static int f1(int x, int y)
{
    int i, t = 0;

    for(i = 0; i < 5000; i++)
        t += x * y;
    return t;
}

static int f2(int x, int y)
{
    int i, t = 0;

    for(i = 0; i < 10000; i++)
        t += x * y;
    return t;
}

static int f3(int x, int y)
{
    int i, t = 0;

    for(i = 0; i < 15000; i++)
        t += x * y;
    return t;
}

int work(int size)
{
    int i, j, t = 0;

    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            t += f3(i, j) - f2(i, j) - f1(i, j);
    return t;
}
 
int main(int argc, char* argv[])
{
   int size = 600;

    work(size);
}
