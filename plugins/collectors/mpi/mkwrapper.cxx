////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2011 The Krell Institute  All Rights Reserved.
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
 * MPI wrapper generation utility.
 *
 */

#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>



/**
 * Main entry point.
 *
 * Main entry point for the MPI wrapper generation utility. This command takes
 * three arguments:
 *
 *     mkwrapper <mpi-function-name> <mpi-header-file-name> <template-file-name>
 *
 * The utility reads the MPI header file named <mpi-header-file-name> and looks
 * for the <mpi-function-name> function. Upon locating this function, it parses
 * the function's declaration to determine its argument list. Then the template
 * file <template-file-name> is read and its contents are copied to the standard
 * out stream after performing the following substitutions first:
 * 
 *     @ARGS@
 *
 *         Replaced with the argument list for the MPI function. This is built
 *         from the arguments found in the MPI header file.
 *
 *         Example: "arg1, arg2, arg3, arg4, arg5, arg6"
 *
 *     @FUNCTION@
 *
 *         Replaced with the MPI function name. This is directly copied from the
 *         first argument (<mpi-function-name>) of the command.
 *
 *         Example: "MPI_Send"
 *
 *     @TYPED_ARGS@
 *
 *         Replaced with the typed argument list for the MPI function. This is
 *         built from the arguments found in the MPI header file.
 *
 *         Example: "void * arg1, int arg2, MPI_Datatype arg3,
 *                   int arg4, int arg5, MPI_Comm arg6"
 *
 * Any and all errors are reported by printing a message to the standard error
 * stream and exiting with an exit code of '1'. Otherwise an exit code of '0' is
 * returned.
 *
 * @param argc    Number of command-line arguments.
 * @param argv    Array of command-line arguments.
 */
int main(int argc, char* argv[])
{
    // Display usage information when necessary
    if(argc != 4) {
	assert(argc > 0);
	std::cerr << std::endl << "Usage: " << argv[0] << " <mpi-function-name>"
		  << " <mpi-header-file-name> <template-file-name>" << std::endl
		  << std::endl;
	return 1;
    }
    
    // Read the entire MPI header file into a string
    std::string mpi_header_file;
    {
	std::ifstream file(argv[2]);
	if(!file.is_open()) {
	    std::cerr << std::endl << argv[0] << ": "
		      << "unable to open \"" << argv[2] << "\"" << std::endl
		      << std::endl;
	    return 1;
	}
	std::string buffer;
	while(std::getline(file, buffer, '\n'))
	    mpi_header_file += buffer + "\n";
    }

    // Locate the specified MPI function name in the MPI header file
    std::string::size_type function = 
	mpi_header_file.find(argv[1] + std::string("("));
    if(function == std::string::npos) {
	std::cerr << std::endl << argv[0] << ": "
		  << "parse error finding \"" << argv[1] << "(\"" << std::endl
		  << std::endl;
	return 1;
    }

    // Locate the first '(' and ')' following the function name
    std::string::size_type open_paren = mpi_header_file.find('(', function);
    if(open_paren == std::string::npos) {
	std::cerr << std::endl << argv[0] << ": "
		  << "parse error finding '('" << std::endl
		  << std::endl;
	return 1;
    }
    std::string::size_type close_paren = mpi_header_file.find(')', function);
    if(open_paren == std::string::npos) {
	std::cerr << std::endl << argv[0] << ": "
		  << "parse error finding ')'" << std::endl
		  << std::endl;
	return 1;	
    }
    
    // Parse the list of arguments for this function
    std::vector<std::string> arguments;    

    for(std::string::size_type
	    i = open_paren + 1, next = mpi_header_file.find(',', i);
	i < close_paren;
	i = next + 1, next = mpi_header_file.find(',', i))
	
	arguments.push_back(
	    mpi_header_file.substr(i, std::min(next, close_paren) - i)
	    );
    
    // Trim any leading and/or trailing spaces from each argument
    for(std::vector<std::string>::iterator
	    i = arguments.begin(); i != arguments.end(); ++i) {
	std::string::size_type left = i->find_first_not_of(" \n\r\t");
	std::string::size_type right = i->find_last_not_of(" \n\r\t");
	if(left < right)
	    *i = i->substr(left, right - left + 1);
    }

    // Strip out any (optional) names from each argument
    for(std::vector<std::string>::iterator
	    i = arguments.begin(); i != arguments.end(); ++i) {
	
	// Start at the right-most character in the argument
	std::string::size_type right = i->size();

	// Exclude any trailing array notation
	std::string::size_type open_bracket = i->find("[");
	right = (open_bracket == std::string::npos) ? right : open_bracket - 1;

	// Exclude any trailing white space
	std::string::size_type ws1 = i->find_last_not_of(" \n\r\t", right);
	right = (ws1 == std::string::npos) ? right : ws1;
	
	// Exclude any trailing argument name
	std::string::size_type name = i->find_last_of(" \n\r\t*", right);
	right = (name == std::string::npos) ? right : name;

	// Exclude any trailing white space
	std::string::size_type ws2 = i->find_last_not_of(" \n\r\t", right);
	right = (ws2 == std::string::npos) ? right : ws2;
	
	// Reconstruct the argument without the argument name
	std::string reconstructed = i->substr(0, right + 1);
	if(open_bracket != std::string::npos) {
	    reconstructed += std::string(" ");
	    reconstructed += i->substr(open_bracket, i->size() - open_bracket);
	}

	// Replace the original argument with the restructed one
	*i = reconstructed;
	
    }

    // Handle special case of single, void, argument
    if((arguments.size() == 1) && (arguments[0] == "void"))
	arguments.clear();

    // Read the entire template file into a string
    std::string template_file;
    {
	std::ifstream file(argv[3]);
	if(!file.is_open()) {
	    std::cerr << std::endl << argv[0] << ": "
		      << "unable to open \"" << argv[3] << "\"" << std::endl
		      << std::endl;
	    return 1;
	}
	std::string buffer;
	while(std::getline(file, buffer, '\n'))
	    template_file += buffer + "\n";
    }

    // Search and replace @ARGS@ in the template file
    {
	std::string to_find = "@ARGS@";

	std::string to_replace;
	for(std::vector<std::string>::size_type 
		i = 0; i < arguments.size(); ++i) {
	    if(i > 0)
		to_replace += ", ";
	    std::stringstream stream;
	    stream << "arg" << i + 1;
	    to_replace += stream.str();	    
	}

	std::string::size_type i;
	while((i = template_file.find(to_find)) != std::string::npos)
	    template_file.replace(i, to_find.size(), to_replace);
    }
    
    // Search and replace @FUNCTION@ in the template file
    {
	std::string to_find = "@FUNCTION@";

	std::string to_replace = argv[1];

	std::string::size_type i;
	while((i = template_file.find(to_find)) != std::string::npos)
	    template_file.replace(i, to_find.size(), to_replace);
    }

    // Search and replace @TYPED_ARGS@ in the template file
    {
	std::string to_find = "@TYPED_ARGS@";

	std::string to_replace;
	for(std::vector<std::string>::size_type 
		i = 0; i < arguments.size(); ++i) {
	    if(i > 0)
		to_replace += ", ";	    
	    std::stringstream stream;
	    stream << "arg" << i + 1;
	    
	    std::string::size_type open_bracket = arguments[i].find("[");
	    if(open_bracket == std::string::npos)
		to_replace += arguments[i] + std::string(" ") + stream.str();
	    else
		to_replace += 
		    arguments[i].substr(0, open_bracket) +
		    std::string(" ") + stream.str() + std::string(" ") +
		    arguments[i].substr(open_bracket,
					arguments[i].size() - open_bracket);

	}
	
	std::string::size_type i;
	while((i = template_file.find(to_find)) != std::string::npos)
	    template_file.replace(i, to_find.size(), to_replace);
    }    

    // Write the resulting template file string to the stdout stream
    std::cout << template_file;
}
