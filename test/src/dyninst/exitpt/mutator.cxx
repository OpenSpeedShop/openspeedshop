
#include "BPatch.h"
#include "BPatch_function.h"

BPatch bpatch;

int main(int argc, char* argv[])
{
    int count = 0;

    // Display usage information when necessary
//    if(argc < 2) {
//        printf("Usage: %s <a.out> <args>\n", argv[0]);
//	exit(1);
//    }

    // Create the running program
    BPatch_process* appThread = bpatch.processCreate(argv[1], (const char **)&(argv[1]));


 // Read the program's image and get the associated image object
    BPatch_image* appImage = appThread->getImage();

// __libc_thread_freeres
// Find exit()
    BPatch_Vector<BPatch_function*> exitFuncs;
    appImage->findFunction("exit", exitFuncs);
    if(exitFuncs.size() != 1) {
        printf("Found %d occurences of exit().\n", exitFuncs.size());
        exit(1);
    } else {
        printf("Found %d occurences of exit().\n", exitFuncs.size());
    }

// Find __libc_thread_freeres()
    BPatch_Vector<BPatch_function*> freeresFuncs;
    appImage->findFunction("__libc_thread_freeres", freeresFuncs);
    if(freeresFuncs.size() != 1) {
        printf("Found %d occurences of __libc_thread_freeres().\n", freeresFuncs.size());
        exit(1);
    } else {
        printf("Found %d occurences of __libc_thread_freeres().\n", freeresFuncs.size());
    }
    
    
    // Continue exection of mutatee and wait for it to terminate
    appThread->continueExecution();
    while(!appThread->isTerminated())
	bpatch.waitForStatusChange();
    
    printf("SUCCESS mutatee terminated!\n");
}
