#include <boost/python.hpp>
#include <iostream>

#include "pyinit.hxx"
#include "Parser.hxx"
#include "parseresult.hxx"
#include "commandobject.hxx"
#include "outputobj.hxx"
#include "pyAPI.hxx"

using namespace OpenSpeedShop::cli;

struct ShowExperiment :
        public SingleDataOutputObject< pExperiment >
{
    virtual void operator()( pExperiment const& exp ) const
    {
        std::cout << "Experiment received: " << exp.repr() << std::endl;
    }
};


struct ShowListOfProcesses :
        public ForwardIteratorOutputObject< std::vector<pProcess>::iterator >
{
    virtual void operator()( std::vector<pProcess>::iterator const& first,
                             std::vector<pProcess>::iterator const& last) const

    {
        std::cout << "I'm in ShowListOfProcesses" << std::endl;
        for( std::vector< pProcess >::iterator p = first;
             p != last;
             p++ )
        {
            std::cout << "ProcessList received: " << p->repr() << std::endl;
        }
    }
};


class MyParser: public OpenSpeedShop::cli::Parser
{
public:

    MyParser(): Parser( "SimuParser" )
    {
    }

protected:
    virtual _CommandObject createCommandObject(OpenSpeedShop::cli::ParseResult const& r)
    {
        if( r.has_key("expCreate") )
        {
           std::cout << "MyParser::createCommandObject for expCreate"
                     << std::endl;
           return _CommandObject(static_cast<void*>( new CommandObject<ShowExperiment>() ));
        }
        if( r.has_key("listPids") )
        {
           std::cout << "MyParser::createCommandObject for listPids"
                     << std::endl;
           return _CommandObject(static_cast<void*>( new CommandObject<ShowListOfProcesses>() ));
        }
        return NULL;
    }
};


using namespace boost::python;
using namespace OpenSpeedShop::cli;


main()
{
    try
    {
        MyParser parser;
	parser.execute( "expCreate -f /home/foo/bin/bar pcsamp", _CommandObject(NULL) );
        // parser.execute( "listPids -h localhost", _CommandObject(NULL) );
    }
    catch( error_already_set )
    {
        std::cerr << "Something's wrong: " << __FILE__ << ", " << __LINE__
                  << std::endl;
        PyErr_Print();
    }
}
