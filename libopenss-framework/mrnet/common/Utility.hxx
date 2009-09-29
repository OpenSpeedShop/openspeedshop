////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
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

/** @file
 *
 * Declaration of frontend/backend communication protocol utility functions.
 *
 */

#ifndef _OpenSpeedShop_Framework_Utility_
#define _OpenSpeedShop_Framework_Utility_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Protocol.h"

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#include <string>
#include <utility>



namespace OpenSpeedShop { namespace Framework {

    class Blob;
    class Path;

    uint64_t computeChecksum(const Path&);

    std::string getLocalHost();
    std::string getCanonicalName(const std::string&);

    std::pair<std::string, std::string>
    parseLibraryFunctionName(const std::string&);

    Path searchForExecutable(const std::string&, const Path&);
    Path searchForLibrary(const Path&);
    
    void convert(const std::string&, char*&);
    void convert(const Blob&, OpenSS_Protocol_Blob&);

    std::string toString(const OpenSS_Protocol_AddressRange&);
    std::string toString(const OpenSS_Protocol_AddressBitmap&);
    std::string toString(const OpenSS_Protocol_Blob&);
    std::string toString(const OpenSS_Protocol_Collector&);
    std::string toString(const OpenSS_Protocol_Experiment&);
    std::string toString(const OpenSS_Protocol_ExperimentGroup&);
    std::string toString(const OpenSS_Protocol_FileName&);
    std::string toString(const OpenSS_Protocol_FunctionEntry&);
    std::string toString(const OpenSS_Protocol_JobEntry&);
    std::string toString(const OpenSS_Protocol_Job&);
    std::string toString(const OpenSS_Protocol_StatementEntry&);
    std::string toString(const OpenSS_Protocol_ThreadName&);
    std::string toString(const OpenSS_Protocol_ThreadNameGroup&);
    std::string toString(const OpenSS_Protocol_ThreadState&);

    std::string toString(const OpenSS_Protocol_AttachToThreads&);
    std::string toString(const OpenSS_Protocol_AttachedToThreads&);
    std::string toString(const OpenSS_Protocol_ChangeThreadsState&);
    std::string toString(const OpenSS_Protocol_CreateProcess&);
    std::string toString(const OpenSS_Protocol_CreatedProcess&);
    std::string toString(const OpenSS_Protocol_DetachFromThreads&);
    std::string toString(const OpenSS_Protocol_ExecuteNow&);
    std::string toString(const OpenSS_Protocol_ExecuteAtEntryOrExit&);
    std::string toString(const OpenSS_Protocol_ExecuteInPlaceOf&);
    std::string toString(const OpenSS_Protocol_GetGlobalInteger&);
    std::string toString(const OpenSS_Protocol_GetGlobalString&);
    std::string toString(const OpenSS_Protocol_GetMPICHProcTable&);
    std::string toString(const OpenSS_Protocol_GlobalIntegerValue&);
    std::string toString(const OpenSS_Protocol_GlobalJobValue&);
    std::string toString(const OpenSS_Protocol_GlobalStringValue&);
    std::string toString(const OpenSS_Protocol_Instrumented&);
    std::string toString(const OpenSS_Protocol_LoadedLinkedObject&);
    std::string toString(const OpenSS_Protocol_ReportError&);
    std::string toString(const OpenSS_Protocol_SetGlobalInteger&);
    std::string toString(const OpenSS_Protocol_StdIn&);
    std::string toString(const OpenSS_Protocol_StdErr&);
    std::string toString(const OpenSS_Protocol_StdOut&);
    std::string toString(const OpenSS_Protocol_StopAtEntryOrExit&);
    std::string toString(const OpenSS_Protocol_SymbolTable&,
			 const bool& = true);
    std::string toString(const OpenSS_Protocol_ThreadsStateChanged&);
    std::string toString(const OpenSS_Protocol_Uninstrument&);
    std::string toString(const OpenSS_Protocol_UnloadedLinkedObject&);
    std::string toString(const OpenSS_Protocol_MPIStartup&);
    
} }



#endif
