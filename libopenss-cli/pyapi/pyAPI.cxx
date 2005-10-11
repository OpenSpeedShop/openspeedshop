#include "pyAPI.hxx"
#include "outputobj.hxx"
#include <vector>
#include <string>


using namespace OpenSpeedShop::cli;
using namespace boost::python;

list pProcess::listThreads() const
{
    return pFrameWork::listThreadsInProcess( *this );
}


pExperiment::pExperiment(std::string type)
    : _file(pFile("<None>", pHost("<None>"))), 
      _type( type ), _name("<None>")
{
    pFrameWork::createExperiment( *this );
}

pExperiment::pExperiment( pFile file, std::string type)
    : _file(file), _type( type ), _name( type )
{
    pFrameWork::createExperiment( *this );
}

void pExperiment::attachToProcess( const pProcess p )
{
    pFrameWork::attachExpToProcess( *this, p );
}

void pExperiment::attachToMPIRank( const pRank r )
{

    pFrameWork::attachExpToMPIRank( *this, r );
}

void pExperiment::attachToPosixThread( const pPosixThread t )
{
    pFrameWork::attachExpToPosixThread( *this, t );
}
    
void pExperiment::detachProcess( const pProcess p )
{}

void pExperiment::detachMPIRank( const pRank r )
{}

void pExperiment::detachPosixThread( const pPosixThread t )
{}

    
void pExperiment::detachAll()
{}

void pExperiment::close() 
{}

    
void pExperiment::restart()
{}

void pExperiment::suspend()
{}

list pExperiment::extractResults(pMetric metric)
{
    return pFrameWork::extractExpResults( *this, metric );
}



// Basically the implementation of the API.
// This is the bridge to the FrameWork and here we have the
// knowledge of the Command Object, but has a cloaked 
// pointer, because of Pyhthon's total ignorance of
// the notion of uninstanciated template.
// Most of those functions will not be exported to Python


// listExps command
list pFrameWork::listDefinedExperiments()
{
    std::cout << "Unimplemented call to "
            << "pFrameWork::listDefinedExperiments" 
            << std::endl;

    boost::python::list l;
    l.append( pExperiment( pFile("/home/nobody/whatever/bin/myApp", 
                                 pHost("localhost" )),
                           "pcsamp" ));
                               
    l.append( pExperiment( pFile("/home/nobody/whatever/bin/myApp", 
                                 pHost("localhost" )),
                           "usertime" ));
    return l;

}


// listPids command
list pFrameWork::listPidsOnHost( pHost host)
{
    std::cout << "Unimplemented call to "
            << "pFrameWork::listPidsOnHost" 
            << "(" << host.repr() << ")"
            << std::endl;

    boost::python::list l;
    l.append( pProcess( 1234, host ) );
    l.append( pProcess( 456, host ) );
    return l;

}

// listHosts command
list pFrameWork::listHostsInCluster()
{
    std::cout << "Unimplemented call to "
            << "pFrameWork::listHostsInCluster" 
            << std::endl;

    boost::python::list l;
    l.append( pHost( "baguette.engr.sgi.com" ) );
    l.append( pHost( "bastille2.engr.sgi.com" ) );
    l.append( pHost( "hope.americas.sgi.com" ) );
    l.append( pHost( "hope2.americas.sgi.com" ) );
    return l;
}


void pFrameWork::attachExpToProcess( pExperiment const& exp, 
                                     pProcess const& proc )
{
    std::cout << "Unimplemented call to "
            << "pFrameWork::attachExpToProcess" 
            << "(" << exp.repr() << "," << proc.repr() << ")"
            << std::endl;

}

void pFrameWork::attachExpToMPIRank( pExperiment const& exp, 
                                     pRank const& rank )
{
    std::cout << "Unimplemented call to "
            << "pFrameWork::attachExpToProcess" 
            << "(" << exp.repr() << "," << rank.repr() << ")"
            << std::endl;

}

void pFrameWork::attachExpToPosixThread( pExperiment const& exp, 
                                         pPosixThread const& pthread )
{
    std::cout << "Unimplemented call to "
            << "pFrameWork::attachExpToProcess" 
            << "(" << exp.repr() << "," << pthread.repr() << ")"
            << std::endl;
}


list pFrameWork::listThreadsInProcess( pProcess const& proc )
{
    std::cout << "Unimplemented call to "
            << "pFrameWork::listThreadsInProcess" 
            << "(" << proc.repr() << ")"
            << std::endl;

    list l;
    l.append( pPosixThread( proc, 1234 ));
    l.append( pPosixThread( proc, 2345 ));
    l.append( pPosixThread( proc, 4567 ));
    return l;

}

pProcess pFrameWork::createProcess( pFile const& file )
{
    std::cout << "Unimplemented call to "
            << "pFrameWork::createProcess" 
            << "(" << file.repr() << ")"
            << std::endl;
    return pProcess( 1234, file.getHost());
}

pExperiment pFrameWork::createExperiment( pExperiment const& exp )
{
    std::cout << "Unimplemented call to "
            << "pFrameWork::createExperiment" 
            << "(" << exp.repr() << ")"
            << std::endl;
    return exp;
}

list pFrameWork::extractExpResults( pExperiment const& exp, 
                                    pMetric const& metric )
{
    std::cout << "Unimplemented call to "
            << "pFrameWork::extractExpResults" 
            << "(" << exp.repr() << "," << metric.repr() << ")"
            << std::endl;
}



