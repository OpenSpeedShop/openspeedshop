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

/** @file
 *
 * Definition of the Callbacks namespace.
 *
 */

#include "Blob.hxx"
#include "Backend.hxx"
#include "Callbacks.hxx"
#include "Protocol.h"
#include "Utility.hxx"

#include <iostream>
#include <sstream>

using namespace OpenSpeedShop::Framework;



/**
 * Attach to threads.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::attachToThreads(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_AttachToThreads message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_AttachToThreads),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Change state of threads.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::changeThreadsState(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_ChangeThreadsState message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ChangeThreadsState),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Create a thread.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::createProcess(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_CreateProcess message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_CreateProcess),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Detach from threads.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::detachFromThreads(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_DetachFromThreads message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_DetachFromThreads),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Execute a library function now.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::executeNow(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_ExecuteNow message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ExecuteNow),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Execute a library function at another function's entry or exit.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::executeAtEntryOrExit(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_ExecuteAtEntryOrExit message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ExecuteAtEntryOrExit),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Execute a library function in place of another function.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::executeInPlaceOf(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_ExecuteInPlaceOf message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ExecuteInPlaceOf),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Get value of an integer global variable from a thread.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::getGlobalInteger(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_GetGlobalInteger message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GetGlobalInteger),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Get value of a string global variable from a thread.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::getGlobalString(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_GetGlobalString message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GetGlobalString),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Get value of the MPICH process table from a thread.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::getMPICHProcTable(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_GetMPICHProcTable message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GetMPICHProcTable),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Set value of an integer global variable in a thread.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::setGlobalInteger(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_SetGlobalInteger message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_SetGlobalInteger),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Stop at a function's entry or exit.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::stopAtEntryOrExit(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_StopAtEntryOrExit message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_StopAtEntryOrExit),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Remove instrumentation from threads.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::uninstrument(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_Uninstrument message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_Uninstrument),
	&message
	);

#ifndef NDEBUG
    if(Backend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}
