
#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>

long f1(long x, long y)
{
    long i, t = 0;

    for(i = 0; i < 5000; i++)
        t += x * y;
    return t;
}

long f2(long x, long y)
{
    long i, t = 0;

    for(i = 0; i < 10000; i++)
        t += x * y;
    return t;
}

long f3(long x, long y)
{
    long i, t = 0;

    for(i = 0; i < 15000; i++)
        t += x * y;
    return t;
}

void* work0(void* arg)
{
    long i, j, t = 0, size = (long)arg;

    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            t += f3(i, j) - f2(i, j) - f1(i, j);
    return (void*)t;    
}

void* work1(void* arg)
{
    long i, j, t = 0, size = (long)arg;

    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            t += f3(i, j) + f2(i, j);
    return (void*)t;    
}

void* work2(void* arg)
{
    long i, j, t = 0, size = (long)arg;

    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            t += f2(i, j) - f1(i, j);
    return (void*)t;    
}

void* work3(void* arg)
{
    long i, j, t = 0, size = (long)arg;

    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            t += f1(i, j);
    return (void*)t;    
}

int main(long argc, char* argv[])
{
    const long SIZE = 500;
    void* (*funcs[4])(void*) = { work0, work1, work2, work3 };
    pthread_t tids[4];
    
    for(long n = 0; n < 4; ++n)
	if(pthread_create(&(tids[n]), NULL, funcs[n], (void*)SIZE) != 0) {
	    perror("pthread_create(): ");
	    fflush(stderr);
	}
	else {
	    printf("tids[%d] = %llu\n", n, (uint64_t)tids[n]);
	    fflush(stdout);
	}

    for(long n = 0; n < 4; ++n) {
	void* retval = NULL;
	pthread_join(tids[n], &retval);
	printf("work%d(%d) = %d\n", n, SIZE, (long)retval);
	fflush(stdout);
    }
}
