#include <Python.h>
#include <boost/python.hpp>
#include <iostream>
#include <string>


#include "pyinit.hxx"
#include "parseresult.hxx"
#include "describe.hxx"

using namespace OpenSpeedShop::cli;

ParseResult
parseThisString( const std::string str )
{
    return ParseResult( "return", "aOK");
}

void
printParseResult( const ParseResult& p )
{
    std::cout << p.ptr() << std::endl;
}

#ifdef STANDALONE
    
main()
{

    ParseResult r = parseThisString("expAttach -p 1234 -t 4567 -h foobar");
    printParseResult( r );
    
}

#else

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

    class_<OpenSpeedShop::cli::ParseResult>("ParseResult")

        // The possible constructors
        .def( init< std::string, long>())
        .def( init< std::string, std::string>() )
        .def( init< std::string, const OpenSpeedShop::cli::ParseResult&>() )

        // export instance[key] to python
        .def( "__getitem__", &OpenSpeedShop::cli::ParseResult::find )

        // Th following exports all the boost::python::dict methods to python
        .def( "__getattribute__", &OpenSpeedShop::cli::ParseResult::getattr )
        ;


    def("parseThisString", &parseThisString );
}
#endif
