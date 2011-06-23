
#include "BPatch.h"
#include "BPatch_function.h"

BPatch bpatch;

int main(int argc, char* argv[])
{

    // Display usage information when necessary
    if(argc != 2) {
	printf("Usage: %s <pid>\n", argv[0]);
	exit(1);
    }
    
    // Attach to the running program
    BPatch_process* appThread = bpatch.processAttach(NULL, atoi(argv[1]));
//    BPatch_thread* appThread = bpatch.attachProcess(NULL, atoi(argv[1]));
    
    
    // Read the program's image and get the associated image object
    BPatch_image* appImage = appThread->getImage();

    
    // Find fpe_enable() 
    BPatch_Vector<BPatch_function*> fpeFuncs;
    appImage->findFunction("fpe_enable", fpeFuncs);
    if(fpeFuncs.size() != 1) {
	printf("Found %d occurences of fpe_enable().\n", fpeFuncs.size());
	exit(1);
    }
    
    BPatch_Vector<BPatch_snippet*> args;
    BPatch_funcCallExpr callExp(*(fpeFuncs[0]),args );

    // 
    appThread->oneTimeCode(callExp);


    // Continue exection of mutatee and wait for it to terminate
    appThread->continueExecution();
    while(!appThread->isTerminated())
	bpatch.waitForStatusChange();
    
}
