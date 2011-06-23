
#include "BPatch.h"
#include "BPatch_function.h"

BPatch bpatch;

int main(int argc, char* argv[])
{
    int count = 0;


    // Display usage information when necessary
    if(argc < 2) {
        printf("Usage: %s <a.out> <args>\n", argv[0]);
	exit(1);
    }

    // Create the running program
    BPatch_process* appThread = bpatch.processCreate(argv[1], (const char **)&(argv[1]));
    
    // Continue exection of mutatee and wait for it to terminate
    appThread->continueExecution();
    while(!appThread->isTerminated())
	bpatch.waitForStatusChange();
    
    printf("SUCCESS mutatee terminated!\n");
}
