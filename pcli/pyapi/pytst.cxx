#include <boost/python.hpp>
#include <iostream>

#include "pyinit.hxx"
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
        public ForwardIteratorOutputObject< std::vector<pExperiment>::iterator >
{
    virtual void operator()( std::vector<pProcess>::iterator const& first,
                             std::vector<pProcess>::iterator const& last) const

    {
        for( std::vector< pProcess >::iterator p = first;
             p != last;
             p++ )
        {
            std::cout << "ProcessList received: " << p->repr() << std::endl;
        }
    }
};



int main()
{
    pHost localhost("localhost");
    pFile file( "a.out", localhost );
    pExperiment exp( file, "pcsamp" );


    pCommandObject::sendOne( exp, _CommandObject(static_cast<void*>(new CommandObject<ShowExperiment>()) ));
    
}
