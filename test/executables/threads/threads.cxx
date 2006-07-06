
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int f1(int x, int y)
{
    int i, t = 0;

    for(i = 0; i < 5000; i++)
        t += x * y;
    return t;
}

int f2(int x, int y)
{
    int i, t = 0;

    for(i = 0; i < 10000; i++)
        t += x * y;
    return t;
}

int f3(int x, int y)
{
    int i, t = 0;

    for(i = 0; i < 15000; i++)
        t += x * y;
    return t;
}

void* work1(void* arg)
{
    int i, j, t = 0, size = (int)arg;

    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            t += f3(i, j) - f2(i, j) - f1(i, j);
    return (void*)t;    
}

void* work2(void* arg)
{
    int i, j, t = 0, size = (int)arg;

    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            t += f3(i, j) + f2(i, j);
    return (void*)t;    
}

void* work3(void* arg)
{
    int i, j, t = 0, size = (int)arg;

    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            t += f2(i, j) - f1(i, j);
    return (void*)t;    
}

void* work4(void* arg)
{
    int i, j, t = 0, size = (int)arg;

    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            t += f1(i, j);
    return (void*)t;    
}

int main(int argc, char* argv[])
{
    int SIZE = 500;
    pthread_t tids[4];
    void* retval;

    pthread_create(&(tids[0]), NULL, work1, (void*)SIZE);
    pthread_create(&(tids[1]), NULL, work2, (void*)SIZE);
    pthread_create(&(tids[2]), NULL, work3, (void*)SIZE);
    pthread_create(&(tids[3]), NULL, work4, (void*)SIZE);

    pthread_join(tids[0], &retval);
    printf("work1(...) = %d\n", (int)retval);

    pthread_join(tids[1], &retval);
    printf("work2(...) = %d\n", (int)retval);

    pthread_join(tids[2], &retval);
    printf("work3(...) = %d\n", (int)retval);

    pthread_join(tids[3], &retval);
    printf("work4(...) = %d\n", (int)retval);
}
