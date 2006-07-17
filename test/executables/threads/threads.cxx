
#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>

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

void* work0(void* arg)
{
    int i, j, t = 0, size = (int)arg;

    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            t += f3(i, j) - f2(i, j) - f1(i, j);
    return (void*)t;    
}

void* work1(void* arg)
{
    int i, j, t = 0, size = (int)arg;

    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            t += f3(i, j) + f2(i, j);
    return (void*)t;    
}

void* work2(void* arg)
{
    int i, j, t = 0, size = (int)arg;

    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            t += f2(i, j) - f1(i, j);
    return (void*)t;    
}

void* work3(void* arg)
{
    int i, j, t = 0, size = (int)arg;

    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            t += f1(i, j);
    return (void*)t;    
}

int main(int argc, char* argv[])
{
    const int SIZE = 500;
    void* (*funcs[4])(void*) = { work0, work1, work2, work3 };
    pthread_t tids[4];
    
    for(int n = 0; n < 4; ++n)
	if(pthread_create(&(tids[n]), NULL, funcs[n], (void*)SIZE) != 0) {
	    perror("pthread_create(): ");
	    fflush(stderr);
	}
	else {
	    printf("tids[%d] = %llu\n", n, (uint64_t)tids[n]);
	    fflush(stdout);
	}

    for(int n = 0; n < 4; ++n) {
	void* retval = NULL;
	pthread_join(tids[n], &retval);
	printf("work%d(%d) = %d\n", n, SIZE, (int)retval);
	fflush(stdout);
    }
}
