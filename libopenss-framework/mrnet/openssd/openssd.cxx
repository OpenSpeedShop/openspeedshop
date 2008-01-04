////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include "Assert.hxx"
#include "Backend.hxx"
#include "Callbacks.hxx"
#include "DyninstCallbacks.hxx"
#include "Protocol.h"

#include <BPatch.h>

using namespace OpenSpeedShop::Framework;



/**
 * Main entry point.
 *
 * Main entry point for the daemon. ...
 *
 * @param argc    Number of command-line arguments.
 * @param argv    Array of command-line arguments.
 */
int main(int argc, char* argv[])
{
    // Register callbacks with Dyninst
    BPatch* bpatch = BPatch::getBPatch();
    Assert(bpatch != NULL);
    bpatch->registerDynLibraryCallback(DyninstCallbacks::dynLibrary);
    bpatch->registerErrorCallback(DyninstCallbacks::error);
    bpatch->registerExecCallback(DyninstCallbacks::exec);
    bpatch->registerExitCallback(DyninstCallbacks::exit);
    bpatch->registerPostForkCallback(DyninstCallbacks::postFork);
    bpatch->registerThreadEventCallback(BPatch_threadCreateEvent,
					DyninstCallbacks::threadCreate);
    bpatch->registerThreadEventCallback(BPatch_threadDestroyEvent,
					DyninstCallbacks::threadDestroy);

    // TODO: We need to register some sort of callback to pickup when a
    //       process under our control stops at a breakpoint. Per Matt
    //       Legendre on DEC-31-2007, Dyninst does not currently provide
    //       any sort of callback for this. The only way to do this is
    //       to track process state inside the daemon and have a thread
    //       do a Dyninst waitForStatusChange() and watch for which
    //       process has changed state, and if it has stopped.
    
    // Register callbacks with the backend
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_ATTACH_TO_THREADS,
			      Callbacks::attachToThreads);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_CHANGE_THREADS_STATE,
			      Callbacks::changeThreadsState);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_CREATE_PROCESS,
			      Callbacks::createProcess);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_DETACH_FROM_THREADS,
			      Callbacks::detachFromThreads);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_EXECUTE_NOW,
			      Callbacks::executeNow);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_EXECUTE_AT_ENTRY_OR_EXIT,
			      Callbacks::executeAtEntryOrExit);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_EXECUTE_IN_PLACE_OF,
			      Callbacks::executeInPlaceOf);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_GET_GLOBAL_INTEGER,
			      Callbacks::getGlobalInteger);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_GET_GLOBAL_STRING,
			      Callbacks::getGlobalString);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_GET_MPICH_PROC_TABLE,
			      Callbacks::getMPICHProcTable);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_SET_GLOBAL_INTEGER,
			      Callbacks::setGlobalInteger);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_STOP_AT_ENTRY_OR_EXIT,
			      Callbacks::stopAtEntryOrExit);
    Backend::registerCallback(OPENSS_PROTOCOL_TAG_UNINSTRUMENT,
			      Callbacks::uninstrument);
    
    // Start the backend's message pump
    Backend::startMessagePump(argc, argv);

    // TODO: how do we decide when to exit???
    
    // Indicate success to the shell
    return 0;
}
