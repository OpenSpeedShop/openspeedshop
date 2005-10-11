#include <Python.h>
#include <boost/python.hpp>
#include <iostream>
#include <string>


#include "pyinit.hxx"
#include "parseresult.hxx"
#include "Parser.hxx"
#include "describe.hxx"

using namespace OpenSpeedShop::cli;

// This is the root of the parser
// The parser takes a simple string as inoput and 
// returns a tree C++ and Python can handle
// There is no handling of any exception yet.

ParseResult
parseThisString( const std::string str )
{
    std::cout << "Parsing This string: " << str << std::endl;
    // We really don't care about what's in the string for now.
    if( str.find( "listPids" ))
        return ParseResult( "listPids", ParseResult( "host", "localhost" ));
    // Same Here, just create one
    if( str.find( "expCreate" ))
        return ParseResult( "expCreate", 
                             ParseResult("file", "/home/foo/bin/bar" )
                          +  ParseResult( "expType", "pcsamp" ));

    // Problem: We just return an error
    // XXXX: Exception here!
    return ParseResult("Syntax", "Error" );
}

void
printParseResult( const ParseResult& p )
{
    std::cout << p.ptr() << std::endl;
}


// This is the Boost::Python export phase.
// We just export the data type. 

using namespace boost::python;


BOOST_PYTHON_MODULE(OSSParser)
{
    // debug function
    def("printRes", printParseResult );

    // Export the ParseResult class to Python
    // Python can use it this way:
    //     o = OSSParser.ParseThisString( str )
    //     where:
    //         +  str is any string to parse
    //         +  o is a ParseResult object that describes the parsing result

    class_<ParseResult>("ParseResult")

        // The possible constructors
        .def( init< std::string, long>())
        .def( init< std::string, std::string>() )
        .def( init< std::string, const OpenSpeedShop::cli::ParseResult&>() )

        // export instance[key] to python
        .def( "__getitem__", &OpenSpeedShop::cli::ParseResult::find )

        // Th following exports all the boost::python::dict methods to python
        .def( "__getattribute__", &OpenSpeedShop::cli::ParseResult::getattr )
        ;

    def("parseThisString", parseThisString );

    class_<Parser>("Parser", init< std::string >())
        ;
}
