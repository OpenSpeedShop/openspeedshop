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
 * Definition of frontend/backend communication protocol utility functions.
 *
 */

#include "Address.hxx"
#include "AddressBitmap.hxx"
#include "AddressRange.hxx"
#include "Assert.hxx"
#include "Blob.hxx"
#include "Path.hxx"
#include "Time.hxx"
#include "Utility.hxx"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <algorithm>
#include <iomanip>
#include <limits>
#include <sstream>
#include <unistd.h>

using namespace OpenSpeedShop::Framework;



/**
 * Compute the checksum of a file.
 *
 * ...
 *
 * @param path    Full path name of the file for which to calculat the checksum.
 * @return        Checksum of that file.
 */
uint64_t OpenSpeedShop::Framework::computeChecksum(const Path& path)
{
    // TODO: implement!
    return 0;
}



//
// Maximum length of a host name. According to the Linux manual page for the
// gethostname() function, this should be available in a header somewhere. But
// it isn't found on all systems, so define it directly if necessary.
//
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX (256)
#endif

/**
 * Get the local host name.
 *
 * Returns the name of the host on which we are executing. This information is
 * obtained directly from the operating system.
 *
 * @note    This function has intentionally been given an identical name to
 *          the similar method in the framework Experiment class. In the near
 *          future, the experiment database functionality may be split into a
 *          separate library and utilized by the daemon. If this happens, this
 *          function will be removed in favor of the existing framework method.
 *
 * @return    Name of the local host.
 */
std::string OpenSpeedShop::Framework::getLocalHost()
{
    // Obtain the local host name from the operating system
    char buffer[HOST_NAME_MAX];
    Assert(gethostname(buffer, sizeof(buffer)) == 0);
    
    // Return the local host name to the caller
    return buffer;
}



/**
 * Get the canonical name of a host.
 *
 * Returns the canonical name of the specified host. This information is
 * obtained directly from the operating system.
 *
 * @note    This function has intentionally been given an identical name to
 *          the similar method in the framework Experiment class. In the near
 *          future, the experiment database functionality may be split into a
 *          separate library and utilized by the daemon. If this happens, this
 *          function will be removed in favor of the existing framework method.
 *
 * @param host    Name of host for which to get canonical name.
 * @return        Canonical name of that host.
 */
std::string OpenSpeedShop::Framework::getCanonicalName(const std::string& host)
{
    std::string canonical = host;

    // Interested in IPv4 protocol information only (including canonical name)
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = AF_INET;
    hints.ai_protocol = PF_INET;
    
    // Request address information for this host
    struct addrinfo* results = NULL;
    getaddrinfo(host.c_str(), NULL, &hints, &results);
    
    // Was the specified name for the loopback device?
    if((results != NULL) && 
       (ntohl(reinterpret_cast<struct sockaddr_in*>
              (results->ai_addr)->sin_addr.s_addr) == INADDR_LOOPBACK)) {

        // Free the address information
        freeaddrinfo(results);

        // Request address information for the local host name
        results = NULL;
        getaddrinfo(getLocalHost().c_str(), NULL, &hints, &results);
        
    }

    // Did we get address information?
    if(results != NULL) {

        // Use the canonical name if one was provided
        if(results->ai_canonname != NULL)
            canonical = results->ai_canonname;

        // Free the address information
        freeaddrinfo(results);

    }

    // Return the canonical name to the caller
    return canonical;
}



/**
 * Parse a library function name.
 *
 * Returns the separate library and function names parsed from the specified
 * library function name. If the passed name cannot be parsed, the returned
 * library and function names will be empty.
 *
 * @param function    Name of the library function to be parsed.
 * @return            Pair containing the parsed library and function names.
 */
std::pair<std::string, std::string> 
OpenSpeedShop::Framework::parseLibraryFunctionName(const std::string& function)
{
    // Find the ":" separator between the library and function names
    std::string::size_type separator = function.find_first_of(':');    
    if(separator == std::string::npos)
        return std::pair<std::string, std::string>();
    
    // Library name is everything up to the separator (trimming spaces)
    std::string library_name = 
	function.substr(0, separator);
    std::string::size_type trim_left = library_name.find_first_not_of(' ');
    std::string::size_type trim_right = library_name.find_last_not_of(' ');    
    if(trim_left < trim_right)
        library_name = 
            library_name.substr(trim_left, trim_right - trim_left + 1);
    
    // Function name is everything after the separator (trimming spaces)
    std::string function_name =
        function.substr(separator + 1, function.size() - separator - 1);
    trim_left = function_name.find_first_not_of(' ');
    trim_right = function_name.find_last_not_of(' ');    
    if(trim_left < trim_right)
        function_name = 
            function_name.substr(trim_left, trim_right - trim_left + 1);

    // Return the parsed library and function names to the caller
    return std::make_pair(library_name, function_name);
}



/**
 * Search for an executable.
 *
 * Returns the full, normalized, path name of the specified executable. The
 * is accomplished by using the passed search path to find the executable. If
 * the specified executable already has an absolute path, or if an executable
 * cannot be found, the originally specified path is returned unchanged.
 *
 * @param path          Search path with which to find the executable.
 * @param executable    Executable to be found.
 * @return              Full, normalized, path of the executable if found, or
 *                      the original executable name if not.
 */
Path OpenSpeedShop::Framework::searchForExecutable(const std::string& path,
						   const Path& executable)
{
    // Return path unchanged if it is an absolute path
    if(executable.isAbsolute())
        return executable;
    
    // Return normalized path if it is an executable
    if(executable.getNormalized().isExecutable())
        return executable.getNormalized();

    // Is the search path non-empty?
    if(!path.empty()) {
        
        // Iterate over individual directories in the search path
        for(std::string::size_type 
                i = path.find_first_not_of(':', 0), next = path.find(':', i);
            i != std::string::npos;
            i = path.find_first_not_of(':', next), next = path.find(':', i)) {
            
            // Extract this directory
            Path directory = 
                path.substr(i, (next == std::string::npos) ? next : next - i);
            
            // Assmeble the candidate and check if it is an executable
            Path candidate = (directory + executable).getNormalized();
            if(candidate.isExecutable())
                return candidate;
            
        }
        
    }
    
    // Otherwise return the path unchanged
    return executable;
}



/**
 * Search for a library.
 *
 * Returns the full, normalized, path name of the specified library. The path
 * in the environment variable OPENSS_PLUGIN_PATH is searched for the library.
 * If the specified library already is an absolute path, or if the library
 * cannot be found, the originally specified path is returned unchanged.
 *
 * @param library    Library to be found.
 * @return           Full, normalized, path of the library if found, or
 *                   the original library name if not.
 */
Path OpenSpeedShop::Framework::searchForLibrary(const Path& library)
{
    // Return path unchanged if it is an absolute path
    if(library.isAbsolute())
	return library;

    // Get the plugin search path
    if(getenv("OPENSS_PLUGIN_PATH") != NULL) {
	std::string path = getenv("OPENSS_PLUGIN_PATH");

	// Iterate over individual directories in the search path
        for(std::string::size_type 
                i = path.find_first_not_of(':', 0), next = path.find(':', i);
            i != std::string::npos;
            i = path.find_first_not_of(':', next), next = path.find(':', i)) {
            
            // Extract this directory
            Path directory = 
                path.substr(i, (next == std::string::npos) ? next : next - i);
            
            // Assmeble the candidate and check if it is an existant file
            Path candidate = (directory + library).getNormalized();
            if(candidate.isFile())
                return candidate;
            
        }
	
    }
    
    // Otherwise return the path unchanged
    return library;
}



/**
 * Convert string for protocol use.
 *
 * Converts the specified C++ string to a C character array as used in
 * protocol messages.
 *
 * @note    The caller assumes responsibility for releasing the C character
 *          array when it is no longer needed.
 *
 * @param in      C++ string to be converted.
 * @retval out    C character array to hold the results.
 */
void OpenSpeedShop::Framework::convert(const std::string& in, char*& out)
{
    out = reinterpret_cast<char*>(malloc((in.size() + 1) * sizeof(char)));
    strcpy(out, in.c_str());
}



/**
 * Convert blob for protocol use.
 *
 * Converts the specified framework blob object to the structure used in
 * protocol messages.
 *
 * @note    The caller assumes responsibility for releasing all allocated
 *          memory when it is no longer needed.
 *
 * @param in     Blob to be converted.
 * @param out    Structure to hold the results.
 */
void OpenSpeedShop::Framework::convert(const Blob& in,
				       OpenSS_Protocol_Blob& out)
{
    out.data.data_len = in.getSize();
    out.data.data_val = reinterpret_cast<uint8_t*>(malloc(
        std::max(static_cast<unsigned>(1), in.getSize()) * sizeof(char)
	));
    if(in.getSize() > 0)
	memcpy(out.data.data_val, in.getContents(), in.getSize());
}



/**
 * Conversion from OpenSS_Protocol_AddressRange to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_AddressRange into a std::string.
 *
 * @param range    Address range to be converted.
 * @return         String conversion of that range.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_AddressRange& range
    )
{
    std::stringstream output;
    output << AddressRange(Address(range.begin), Address(range.end));
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_AddressBitmap to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_AddressBitmap into
 * a std::string. Simply returns the string containing the textual
 * representation of the address range and bitmap contents 
 *
 * @param range    Address range to be converted.
 * @return         String conversion of that range.
 */

std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_AddressBitmap& bitmap
    )
{
    std::stringstream output;
    output << AddressBitmap(AddressRange(bitmap.range.begin,
					 bitmap.range.end),
			    Blob(bitmap.bitmap.data.data_len,
				 bitmap.bitmap.data.data_val));
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_Blob to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_Blob into a std::string.
 * Simply returns the string containing each data byte, sixteen per line,
 * in both hexadecimal and character representations.
 *
 * @param blob    Blob to be converted.
 * @return        String conversion of that blob.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_Blob& blob
    )
{
    static const unsigned BytesPerLine = 16;
    std::stringstream output;

    // Get the size and contents of the blob
    const unsigned size = blob.data.data_len;
    const char* ptr = (const char*)blob.data.data_val;

    // Iterate over each byte in the blob's contents
    for(unsigned i = 0; i < size; i += BytesPerLine) {

	// Indent each line by four spaces
	output << "    ";

	// Display the hexadecimal representation
	for(unsigned j = 0; j < BytesPerLine; j++) {
	    if((i + j) < size)
		output << std::hex << std::setfill('0') << std::setw(2)
		       << static_cast<unsigned>(
			      static_cast<uint8_t>(ptr[i + j])
			      ) << " ";
	    else
		output << "   ";
	}

	// Indent another two spaces before the character representation
	output << "  ";

	// Display the character representation
	for(unsigned j = 0; j < BytesPerLine; j++) {
	    if((i + j) < size) {
		if(isprint(ptr[i + j]))
		    output << ptr[i + j];
		else
		    output << ".";
	    }
	    else
		output << " ";
	}

	// Complete this line
	output << std::endl;
	
    }

    // Handle special case of an empty blob
    if(size == 0)
	output << "    <empty>" << std::endl;

    // Return the string to the caller
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_Collector to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_Collector into a std::string.
 * Simply returns the string containing the textual representation of the
 * experiment and collector's unique identifiers.
 *
 * @param collector    Collector to be converted.
 * @return             String conversion of that collector.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_Collector& collector
    )
{
    std::stringstream output;
    output << collector.experiment << ":" << collector.collector;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_Experiment to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_Experiment into a std::string.
 * Simply returns the string containing the textual representation of the
 * experiment's unique identifier.
 *
 * @param experiment    Experiment to be converted.
 * @return              String conversion of that experiment.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_Experiment& experiment
    )
{
    std::stringstream output;
    output << experiment.experiment;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_ExperimentGroup to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_ExperimentGroup into a std::
 * string. Simply returns the string containing the textual representation of
 * the individual experiments in the experiment group.
 *
 * @param experiments    Experiment group to be converted.
 * @return               String conversion of that experiment group.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_ExperimentGroup& experiments
    )
{
    std::stringstream output;
    output << "    { ";
    for(int i = 0; i < experiments.experiments.experiments_len; ++i) {
	if(i > 0)
	    output << ", ";
	output << toString(experiments.experiments.experiments_val[i]);
    }
    output << " }";
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_FileName to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_FileName into a std::string.
 * Simply returns the string containing the textual representation of the
 * file's checksum and its name.
 *
 * @param file    File name to be converted.
 * @return        String conversion of that file name.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_FileName& file
    )
{
    std::stringstream output;
    output << Address(file.checksum) << ":";
    if(file.path != NULL)
	output << file.path;
    else
	output << "<null>";
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_FunctionEntry to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_FunctionEntry into a
 * std::string. ...
 *
 * @param function    Function entry to be converted.
 * @return            String conversion of that function entry.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_FunctionEntry& function
    )
{
    std::stringstream output;
    output << "    ";
    if(function.name != NULL)
	output << function.name;
    else
	output << "<null>";
    output << std::endl;
    for(int i = 0; i < function.bitmaps.bitmaps_len; ++i)
	output << "        " << toString(function.bitmaps.bitmaps_val[i])
	       << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_JobEntry to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_JobEntry into a std::string. ...
 * Simply returns the string containing the textual representation of the host
 * name and process identifier.
 *
 * @param entry    Job entry to be converted.
 * @return         String conversion of that job entry.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_JobEntry& entry
    )
{
    std::stringstream output;
    output << entry.host << ":" << entry.pid;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_Job to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_Job into a std::string. Simply
 * returns the string containing the textual representation of the individual
 * entries in the job.
 *
 * @param job    Job to be converted.
 * @return       String conversion of that job.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_Job& job
    )
{
    std::stringstream output;
    output << "    { ";
    for(int i = 0; i < job.entries.entries_len; ++i) {
	output << toString(job.entries.entries_val[i]);
	if(i < (job.entries.entries_len - 1))
	    output << "," << std::endl << "      ";
    }
    output << " }";
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_StatementEntry to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_StatementEntry into a
 * std::string. ...
 *
 * @param statement    Statement entry to be converted.
 * @return             String conversion of that statement entry.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_StatementEntry& statement
    )
{
    std::stringstream output;
    output << "    " << toString(statement.path) << ", line " 
	   << statement.line << ", column " << statement.column << std::endl;
    for(int i = 0; i < statement.bitmaps.bitmaps_len; ++i)
	output << "        " << toString(statement.bitmaps.bitmaps_val[i])
	       << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_ThreadName to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_ThreadName into a std::string.
 * Simply returns the string containing the textual representation of the
 * experiment's unique identifier, host name, the process identifier, and (if
 * present) the posix thread identifier.
 *
 * @param thread    Thread name to be converted.
 * @return          String conversion of that thread name.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_ThreadName& thread
    )
{
    std::stringstream output;
    output << thread.experiment << ":" << thread.host << ":" << thread.pid;
    if(thread.has_posix_tid)
	output << ":" << thread.posix_tid;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_ThreadNameGroup to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_ThreadNameGroup into a std::
 * string. Simply returns the string containing the textual representation of
 * the individual thread names in the thread name group.
 *
 * @param threads    Thread name group to be converted.
 * @return           String conversion of that thread name group.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_ThreadNameGroup& threads
    )
{
    std::stringstream output;
    output << "    { ";
    for(int i = 0; i < threads.names.names_len; ++i) {
	output << toString(threads.names.names_val[i]);
	if(i < (threads.names.names_len - 1))
	    output << "," << std::endl << "      ";
    }
    output << " }";
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_ThreadState to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_ThreadState into a std::string.
 * Simply returns the string containing the textual representation of the passed
 * thread state enumeration value.
 *
 * @param state    Thread state to be converted.
 * @return         String conversion of that thread state.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_ThreadState& state
    )
{
    switch(state) {
    case Disconnected:
	return "Disconnected";
    case Connecting:
	return "Connecting";
    case Nonexistent:
	return "Nonexistent";
    case Running:
	return "Running";
    case Suspended:
	return "Suspended";
    case Terminated:
	return "Terminated";
    default:
	return "?";
    }
}



/**
 * Conversion from OpenSS_Protocol_AttachToThreads to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_AttachToThreads message
 * into a std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_AttachToThreads& message
    )
{
    std::stringstream output;
    output << "attachToThreads(" << std::endl 
	   << toString(message.threads) << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_AttachedToThreads to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_AttachedToThreads message
 * into a std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_AttachedToThreads& message
    )
{
    std::stringstream output;
    output << "attachedToThreads(" << std::endl 
	   << toString(message.threads) << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_ChangeThreadsState to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_ChangeThreadsState message
 * into a std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_ChangeThreadsState& message
    )
{
    std::stringstream output;
    output << "changeThreadsState(" << std::endl
	   << toString(message.threads) << "," << std::endl
	   << "    " << toString(message.state) << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_CreateProcess to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_CreateProcess message into a
 * std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_CreateProcess& message
    )
{
    std::stringstream output;
    output << "createProcess(" << std::endl
	   << "    " << toString(message.thread) << "," << std::endl
	   << "    \"" << message.command << "\"," << std::endl
	   << toString(message.environment)
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_CreatedProcess to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_CreatedProcess message into a
 * std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_CreatedProcess& message
    )
{
    std::stringstream output;
    output << "createdProcess(" << std::endl
	   << "    " << toString(message.original_thread) << "," << std::endl
	   << "    " << toString(message.created_thread) << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_DetachFromThreads to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_DetachFromThreads message
 * into a std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_DetachFromThreads& message
    )
{
    std::stringstream output;
    output << "detachFromThreads(" << std::endl 
	   << toString(message.threads) << std::endl 
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_ExecuteNow to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_ExecuteNow message into a
 * std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_ExecuteNow& message
    )
{
    std::stringstream output;
    output << "executeNow(" << std::endl
	   << toString(message.threads) << "," << std::endl
	   << "    " << toString(message.collector) << "," << std::endl
	   << "    " << (message.disable_save_fpr ? "true" : "false") << "," 
	   << std::endl
	   << "    \"" << message.callee << "\"," << std::endl
	   << toString(message.argument)
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_ExecuteAtEntryOrExit to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_ExecuteAtEntryOrExit message
 * into a std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_ExecuteAtEntryOrExit& message
    )
{
    std::stringstream output;
    output << "executeAtEntryOrExit(" << std::endl
	   << toString(message.threads) << "," << std::endl
	   << "    " << toString(message.collector) << "," << std::endl
	   << "    \"" << message.where << "\"," << std::endl
	   << "    " << (message.at_entry ? "true" : "false") << "," 
	   << std::endl
	   << "    \"" << message.callee << "\"," << std::endl
	   << toString(message.argument)
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_ExecuteInPlaceOf to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_ExecuteInPlaceOf message
 * into a std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_ExecuteInPlaceOf& message
    )
{
    std::stringstream output;
    output << "executeInPlaceOf(" << std::endl
	   << toString(message.threads) << "," << std::endl
	   << "    " << toString(message.collector) << "," << std::endl
	   << "    \"" << message.where << "\"," << std::endl
	   << "    \"" << message.callee << "\"" << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_GetGlobalInteger to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_GetGlobalInteger message
 * into a std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_GetGlobalInteger& message
    )
{
    std::stringstream output;
    output << "getGlobalInteger(" << std::endl
	   << "    " << toString(message.thread) << "," << std::endl
	   << "    \"" << message.global << "\"" << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_GetGlobalString to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_GetGlobalString message
 * into a std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_GetGlobalString& message
    )
{
    std::stringstream output;
    output << "getGlobalString(" << std::endl
	   << "    " << toString(message.thread) << "," << std::endl
	   << "    \"" << message.global << "\"" << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_GetMPICHProcTable to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_GetMPICHProcTable message
 * into a std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_GetMPICHProcTable& message
    )
{
    std::stringstream output;
    output << "getMPICHProcTable(" << std::endl
	   << "    " << toString(message.thread) << "," << std::endl
	   << "    \"" << message.global << "\"" << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_GlobalIntegerValue to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_GlobalIntegerValue message
 * into a std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_GlobalIntegerValue& message
    )
{
    std::stringstream output;
    output << "globalIntegerValue(" << std::endl
	   << "    " << toString(message.thread) << "," << std::endl
	   << "    \"" << message.global << "\"," << std::endl 
	   << "    " << (message.found ? "true" : "false") << "," << std::endl
	   << "    " << message.value << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_GlobalJobValue to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_GlobalJobValue message into a
 * std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_GlobalJobValue& message
    )
{
    std::stringstream output;
    output << "globalJobValue(" << std::endl
	   << "    " << toString(message.thread) << "," << std::endl
	   << "    \"" << message.global << "\"," << std::endl
	   << "    " << (message.found ? "true" : "false") << "," << std::endl
	   << toString(message.value) << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_GlobalStringValue to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_GlobalStringValue message
 * into a std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_GlobalStringValue& message
    )
{
    std::stringstream output;
    output << "globalStringValue(" << std::endl
	   << "    " << toString(message.thread) << "," << std::endl
	   << "    \"" << message.global << "\"," << std::endl 
	   << "    " << (message.found ? "true" : "false") << "," << std::endl
	   << "    \"" << message.value << "\"" << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_Instrumented to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_Instrumented message into a
 * std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_Instrumented& message
    )
{
    std::stringstream output;
    output << "instrumented(" << std::endl
	   << toString(message.threads) << "," << std::endl
	   << "    " << toString(message.collector) << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_LoadedLinkedObject to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_LoadedLinkedObject message
 * into a std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_LoadedLinkedObject& message
    )
{
    std::stringstream output;
    output << "loadedLinkedObject(" << std::endl
	   << toString(message.threads) << "," << std::endl
	   << "    " << Time(message.time) << "," << std::endl
	   << "    " << toString(message.range) << "," << std::endl
	   << "    " << toString(message.linked_object) << "," << std::endl
	   << "    " << (message.is_executable ? "true" : "false") << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_ReportError to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_ReportError message into a
 * std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_ReportError& message
    )
{
    std::stringstream output;
    output << "reportError(" << std::endl
	   << "    \"" << message.text << "\"" << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_SetGlobalInteger to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_SetGlobalInteger message
 * into a std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_SetGlobalInteger& message
    )
{
    std::stringstream output;
    output << "setGlobalInteger(" << std::endl
	   << "    " << toString(message.thread) << "," << std::endl
	   << "    \"" << message.global << "\"," << std::endl
	   << "    " << message.value << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_StdErr to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_StdErr message into a
 * std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_StdErr& message
    )
{
    std::stringstream output;
    output << "stdErr(" << std::endl
	   << "    " << toString(message.thread) << "," << std::endl
	   << toString(message.data)
	   << ")" << std::endl;
    return output.str();
} 



/**
 * Conversion from OpenSS_Protocol_StdIn to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_StdIn message into a
 * std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_StdIn& message
    )
{
    std::stringstream output;
    output << "stdIn(" << std::endl
	   << "    " << toString(message.thread) << "," << std::endl
	   << toString(message.data)
	   << ")" << std::endl;
    return output.str();
} 



/**
 * Conversion from OpenSS_Protocol_StdOut to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_StdOut message into a
 * std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_StdOut& message
    )
{
    std::stringstream output;
    output << "stdOut(" << std::endl
	   << "    " << toString(message.thread) << "," << std::endl
	   << toString(message.data)
	   << ")" << std::endl;
    return output.str();
} 



/**
 * Conversion from OpenSS_Protocol_StopAtEntryOrExit to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_StopAtEntryOrExit message
 * into a std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_StopAtEntryOrExit& message
    )
{
    std::stringstream output;
    output << "stopAtEntryOrExit(" << std::endl
	   << toString(message.threads) << "," << std::endl
	   << "    \"" << message.where << "\"," << std::endl
	   << "    " << (message.at_entry ? "true" : "false") << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_SymbolTable to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_SymbolTable message into a
 * std::string. A boolean flag specifies whether a verbose conversion, which
 * includes all function names, statements, and address ranges, should be
 * generated or whether a brief conversion should be provided instead.
 * 
 *
 * @param message    Message to be converted.
 * @param verbose    Boolean "true" if a verbose conversion of the symbol table
 *                   should be generated, or "false" if a brief conversion.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_SymbolTable& message,
    const bool& verbose
    )
{
    std::stringstream output;
    output << "symbolTable(" << std::endl
	   << toString(message.experiments) << "," << std::endl
	   << "    " << toString(message.linked_object) << "," << std::endl;
    if(!verbose)
	output << "    { ... }," << std::endl << "    { ... }" << std::endl;
    else {    
	output << "    {" << std::endl;
	for(int i = 0; i < message.functions.functions_len; ++i)
	    output << toString(message.functions.functions_val[i]);
	output << "    }," << std::endl << "    {" << std::endl;
	for(int i = 0; i < message.statements.statements_len; ++i)
	    output << toString(message.statements.statements_val[i]);
	output << "    }" << std::endl;
    }
    output << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_ThreadsStateChanged to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_ThreadsStateChanged message
 * into a std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_ThreadsStateChanged& message
    )
{
    std::stringstream output;
    output << "threadsStateChanged(" << std::endl
	   << toString(message.threads) << "," << std::endl
	   << "    " << toString(message.state) << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_Uninstrument to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_Uninstrument message into a
 * std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_Uninstrument& message
    )
{
    std::stringstream output;
    output << "uninstrument(" << std::endl
	   << toString(message.threads) << "," << std::endl
	   << "    " << toString(message.collector) << std::endl
	   << ")" << std::endl;
    return output.str();
}



/**
 * Conversion from OpenSS_Protocol_UnloadedLinkedObject to std::string.
 *
 * Returns the conversion of an OpenSS_Protocol_UnloadedLinkedObject message
 * into a std::string.
 *
 * @param message    Message to be converted.
 * @return           String conversion of that message.
 */
std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_UnloadedLinkedObject& message
    )
{
    std::stringstream output;
    output << "unloadedLinkedObject(" << std::endl
	   << toString(message.threads) << "," << std::endl
	   << "    " << Time(message.time) << "," << std::endl
	   << "    " << toString(message.linked_object) << "," << std::endl
	   << ")" << std::endl;
    return output.str();
}

std::string OpenSpeedShop::Framework::toString(
    const OpenSS_Protocol_MPIStartup& message
    )
{
    std::stringstream output;
    output << "MPIStartup(" << std::endl
	   << toString(message.threads) << "," << std::endl
	   << "    " << message.in_mpi_startup << "," << std::endl
	   << ")" << std::endl;
    return output.str();
}
