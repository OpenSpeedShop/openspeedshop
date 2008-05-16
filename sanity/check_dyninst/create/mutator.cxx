#include <iostream>
#include <sstream>
#include "BPatch.h"
#include "BPatch_function.h"

BPatch bpatch;

static void
noFollowForkCB(BPatch_thread *parent, BPatch_thread *child)
{
std::cerr << "noFollowForkCB entered" << std::endl;
    if(child != NULL) {
       // detach here!
        BPatch_process *bp_fork_child_process = NULL;
        bp_fork_child_process = child->getProcess();
        bp_fork_child_process->detach(true);
    }
}

static void
dynLibrary(BPatch_thread* thread, BPatch_module* module, bool is_load)
{
char buffer[PATH_MAX];
BPatch_process* process = thread->getProcess();
std::cerr << "dynLibrary entered"
    << " with pid "  << process->getPid()
    << (is_load ? "  loaded" : "  unloaded")
    << " module " << module->getFullName(buffer, sizeof(buffer)) << std::endl;
}

static void
dyninstExec (BPatch_thread* thread)
{
BPatch_process* process = thread->getProcess();
std::cerr << "dyninstExec: PID "
    << process->getPid() << " has called exec()." << std::endl;
}

static void
dyninstExit(BPatch_thread* thread, BPatch_exitType exit_type)
{
BPatch_process* process = thread->getProcess();
        std::stringstream output;
        output << "dyninstExit: PID " << process->getPid() << " exited with \"";
        switch(exit_type) {
        case NoExit:
            output << "NoExit";
            break;
        case ExitedNormally:
            output << "ExitedNormally";
            break;
        case ExitedViaSignal:
            output << "ExitedViaSignal";
            break;
        default:
            output << "?";
            break;
        }
        output << "\"." << std::endl;
        std::cerr << output.str();
}

static void
dyninstThreadCreate(BPatch_process* process, BPatch_thread* thread)
{
        std::stringstream output;
        output << "dyninstthreadCreate(): TID "
               << static_cast<size_t>(thread->getTid()) << " of PID "
               << process->getPid() << " was created." << std::endl;
        std::cerr << output.str();
}

static void
dyninstThreadDestroy(BPatch_process* process, BPatch_thread* thread)
{
        std::stringstream output;
        output << "dyninstthreadDestroy(): TID "
               << static_cast<size_t>(thread->getTid()) << " of PID "
               << process->getPid() << " was destroyed." << std::endl;
        std::cerr << output.str();
}

int main(int argc, char* argv[])
{
    int count = 0;


    // Display usage information when necessary
    if(argc < 2) {
        printf("Usage: %s <a.out> <args>\n", argv[0]);
	exit(1);
    }

    //bpatch.registerPostForkCallback(noFollowForkCB);
    bpatch.registerExecCallback(dyninstExec);
    bpatch.registerExitCallback(dyninstExit);
    bpatch.registerDynLibraryCallback(dynLibrary);
    bpatch.registerThreadEventCallback(
			BPatch_threadCreateEvent,
			dyninstThreadCreate);
    bpatch.registerThreadEventCallback(
			BPatch_threadDestroyEvent,
			dyninstThreadDestroy);
   

    // Create the running program
    BPatch_thread* appThread = bpatch.createProcess(argv[1], (const char **)&(argv[1]));
    
    // Continue exection of mutatee and wait for it to terminate
    appThread->continueExecution();
    while(!appThread->isTerminated())
	bpatch.waitForStatusChange();
    
    printf("SUCCESS mutatee terminated!\n");
}
