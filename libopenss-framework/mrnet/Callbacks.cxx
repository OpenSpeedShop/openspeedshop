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
#include "Callbacks.hxx"
#include "Frontend.hxx"
#include "Protocol.h"
#include "Utility.hxx"

#include <iostream>
#include <sstream>

using namespace OpenSpeedShop::Framework;



/**
 * Attached to a thread.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::attachedToThread(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_AttachedToThread message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_AttachedToThread),
	&message
	);

#ifndef NDEBUG
    if(Frontend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Value of an integer global variable.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::globalIntegerValue(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_GlobalIntegerValue message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GlobalIntegerValue),
	&message
	);

#ifndef NDEBUG
    if(Frontend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Value of a job description global variable.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::globalJobValue(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_GlobalJobValue message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GlobalJobValue),
	&message
	);

#ifndef NDEBUG
    if(Frontend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Value of a string global variable.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::globalStringValue(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_GlobalStringValue message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GlobalStringValue),
	&message
	);

#ifndef NDEBUG
    if(Frontend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Linked object has been loaded.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::loadedLinkedObject(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_LoadedLinkedObject message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_LoadedLinkedObject),
	&message
	);

#ifndef NDEBUG
    if(Frontend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * ...
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::reportError(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_ReportError message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ReportError),
	&message
	);

#ifndef NDEBUG
    if(Frontend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Symbol table.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::symbolTable(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_SymbolTable message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_SymbolTable),
	&message
	);

#ifndef NDEBUG
    if(Frontend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Thread's state has changed.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::threadsStateChanged(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_ThreadsStateChanged message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ThreadsStateChanged),
	&message
	);

#ifndef NDEBUG
    if(Frontend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif

    // TODO: implement!
}



/**
 * Linked object has been unloaded.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::unloadedLinkedObject(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_UnloadedLinkedObject message;
    memset(&message, 0, sizeof(message));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_UnloadedLinkedObject),
	&message
	);

#ifndef NDEBUG
    if(Frontend::isDebugEnabled()) {
	std::stringstream output;
	output << "[TID " << pthread_self() << "] Callbacks::"
	       << toString(message);
	std::cerr << output.str();
    }
#endif
    
    // TODO: implement!
}
