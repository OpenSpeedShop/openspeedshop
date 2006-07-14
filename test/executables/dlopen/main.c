#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int size = 300;
    void* handle = NULL;
    int (*work)(int) = NULL;

    if(argc != 2) {
        printf("Usage: %s <size>\n", argv[0]);
        printf("No size argument given. Defaulting to %d.\n", size);
    } 
    else
	size = atoi(argv[1]);

    handle = dlopen("libwork.so", RTLD_LAZY);
    assert(handle != NULL);

    work = dlsym(handle, "work");
    assert(work != NULL);

    (*work)(size);
}

