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
#include "Protocol.h"

using namespace OpenSpeedShop::Framework;



/**
 * Attach to threads.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::attachToThread(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_AttachToThread data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_AttachToThread),
	&data
	);

    // ...

}



/**
 * Change state of threads.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::changeThreadState(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_ChangeThreadState data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ChangeThreadState),
	&data
	);

    // ...

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
    OpenSS_Protocol_CreateProcess data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_CreateProcess),
	&data
	);

    // ...

}



/**
 * Detach from threads.
 *
 * ...
 *
 * @param blob    Blob containing the message.
 */
void Callbacks::detachFromThread(const Blob& blob)
{
    // Decode the message
    OpenSS_Protocol_DetachFromThread data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_DetachFromThread),
	&data
	);

    // ...

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
    OpenSS_Protocol_ExecuteNow data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ExecuteNow),
	&data
	);

    // ...

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
    OpenSS_Protocol_ExecuteAtEntryOrExit data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ExecuteAtEntryOrExit),
	&data
	);

    // ...

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
    OpenSS_Protocol_ExecuteInPlaceOf data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_ExecuteInPlaceOf),
	&data
	);

    // ...

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
    OpenSS_Protocol_GetGlobalInteger data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GetGlobalInteger),
	&data
	);

    // ...

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
    OpenSS_Protocol_GetGlobalString data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GetGlobalString),
	&data
	);

    // ...

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
    OpenSS_Protocol_GetMPICHProcTable data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_GetMPICHProcTable),
	&data
	);

    // ...

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
    OpenSS_Protocol_SetGlobalInteger data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_SetGlobalInteger),
	&data
	);

    // ...

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
    OpenSS_Protocol_StopAtEntryOrExit data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_StopAtEntryOrExit),
	&data
	);

    // ...

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
    OpenSS_Protocol_Uninstrument data;
    memset(&data, 0, sizeof(data));
    blob.getXDRDecoding(
	reinterpret_cast<xdrproc_t>(xdr_OpenSS_Protocol_Uninstrument),
	&data
	);

    // ...

}
