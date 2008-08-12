#include <iostream>
#include <sstream>
#include <limits.h>
#include "BPatch.h"
#include "BPatch_function.h"

BPatch bpatch;

static void
dyninstPostFork(BPatch_thread *parent, BPatch_thread *child)
{
#if 0
    if(child != NULL) {
		
std::cerr << "dyninstPostFork: Do NOT FOLLOW FORK. dyninst detach from child"
	<< std::endl;
       // detach here!
        BPatch_process *bp_fork_child_process = NULL;
        bp_fork_child_process = child->getProcess();
        bp_fork_child_process->detach(true);
    }
#else
	BPatch_process* parent_process = parent->getProcess();
	BPatch_process* child_process = child->getProcess();
        std::stringstream output;
        output << "dyninstpostFork(): PID " << parent_process->getPid()
               << " forked PID " << child_process->getPid()
               << "." << std::endl;
        std::cerr << output.str();
#endif

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

static void
dyninstError(BPatchErrorLevel severity, int number,
	     const char* const* parameters)
{
    std::string text;

    // Attach the error's severity to the error text
    switch(severity) {
    case BPatchFatal: text = "BPatchFatal"; break;
    case BPatchSerious: text = "BPatchSerious"; break;
    case BPatchWarning: text = "BPatchWarning"; break;
    case BPatchInfo: text = "BPatchInfo"; break;
    default: text = "?"; break;
    }
    text += ": ";

    // Attach the formatted error string to the error text
    char buffer[16384];
    BPatch::formatErrorString(buffer, sizeof(buffer),
                              BPatch::getEnglishErrorString(number),
                              parameters);
    text += buffer;
    // Display the error to the stdout stream
    std::cout << text << std::endl;
}

int main(int argc, char* argv[])
{
    int count = 0;


    // Display usage information when necessary
    if(argc < 2) {
        printf("Usage: %s <a.out> <args>\n", argv[0]);
	exit(1);
    }

    bpatch.registerErrorCallback(dyninstError);
    bpatch.registerPostForkCallback(dyninstPostFork);
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
