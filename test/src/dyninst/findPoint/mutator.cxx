
#include "BPatch.h"
#include "BPatch_function.h"

BPatch bpatch;

int main(int argc, char* argv[])
{
    int count = 0;

#ifdef TEST_CREATE_PROCESS

    // Display usage information when necessary
    if(argc < 2) {
        printf("Usage: %s <a.out> <args>\n", argv[0]);
	exit(1);
    }

    // Create the running program
    BPatch_process* appThread = bpatch.processCreate(argv[1], (const char **)&(argv[1]));

#else

    // Display usage information when necessary
    if(argc != 3) {
	printf("Usage: %s <a.out> <pid>\n", argv[0]);
	exit(1);
    }
    
    // Attach to the running program
    BPatch_thread* appThread = bpatch.attachProcess(argv[1], atoi(argv[2]));
    
#endif
    
    // Read the program's image and get the associated image object
    BPatch_image* appImage = appThread->getImage();

    //
    // Instrument mutatee to count number of calls to sleep()
    //
    
    // Find sleep() and its entry point
    BPatch_Vector<BPatch_function*> sleepFuncs;
    appImage->findFunction("sleep", sleepFuncs);
    if(sleepFuncs.size() != 1) {
	printf("Found %d occurences of sleep().\n", sleepFuncs.size());
	exit(1);
    }
    BPatch_Vector<BPatch_point*>* sleepPts =
	sleepFuncs[0]->findPoint(BPatch_entry);
    if(!sleepPts || (sleepPts->size() < 1)) {
	printf("Found 0 entry points to sleep().\n");
	exit(1);
    }
    
    // Allocate a variable to hold the count and initialize it to zero
    BPatch_variableExpr* cntVar =
	appThread->malloc(*(appImage->findType("int")));
    if(sizeof(int) != cntVar->getSize()) {
	printf("Sizes of native and Dyninst integers don't match.\n");
	exit(1);
    }
    cntVar->writeValue(&count);
    
    // Build the expression "cntVar = cntVar + 1"
    BPatch_arithExpr assignExp(BPatch_assign, *cntVar,
			       BPatch_arithExpr(BPatch_plus, *cntVar,
						BPatch_constExpr(1)));

    // Insert the code snippet into the thread
    appThread->insertSnippet(assignExp, *sleepPts);

    //
    // Instrument mutatee to stop once main() exits
    //
    
    // Find main() and its exit point
    BPatch_Vector<BPatch_function*> mainFuncs;
    appImage->findFunction("main", mainFuncs);
    if(mainFuncs.size() != 1) {
        printf("Found %d occurences of main().\n", mainFuncs.size());
        exit(1);
    }
    BPatch_Vector<BPatch_point*>* mainPts =
	mainFuncs[0]->findPoint(BPatch_exit);
    if(!mainPts || (mainPts->size() < 1)) {
        printf("Found 0 exit points to main().\n");
        exit(1);
    }
    
    // Insert a breakpoint snippet into the thread
    appThread->insertSnippet(BPatch_breakPointExpr(), *mainPts);

    //
    // Let the mutatee run until completion
    //

    // Continue execution of mutatee and wait for it to hit the breakpoint
    appThread->continueExecution();
    while(!appThread->isStopped())
	bpatch.waitForStatusChange();
    
    // Obtain the count value
    cntVar->readValue(&count);

    // Continue exection of mutatee and wait for it to terminate
    appThread->continueExecution();
    while(!appThread->isTerminated())
	bpatch.waitForStatusChange();
    
    // Success!
    printf("Counted %d call%s to sleep()!\n", count, (count != 1) ? "s" : "");
}
