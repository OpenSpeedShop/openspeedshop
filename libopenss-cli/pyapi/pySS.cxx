#include <boost/python.hpp>
#include <boost/python/return_internal_reference.hpp>
#include "pyAPI.hxx"
#include "outputobj.hxx"
#include "commandobject.hxx"


using namespace boost::python;
using namespace OpenSpeedShop::cli;

void sendOneExperiment( pExperiment const& exp, _CommandObject const& cmdobj )
{
    std::cout << "I'm in sendOneExperiment" << std::endl;
    pCommandObject::sendOne( exp, cmdobj );
}

void sendListOfProcesses( list const& l, _CommandObject const& cmdobj )
{
    std::cout << "I'm in sendListOfProcesses" << std::endl;
    pCommandObject::sendList<pProcess, std::vector<pProcess>::iterator>( l, cmdobj );
}


struct ShowListOfProcesses :
        public ForwardIteratorOutputObject< std::vector<pProcess>::iterator >
{
    virtual void operator()( std::vector<pProcess>::iterator const& first,
                             std::vector<pProcess>::iterator const& last) const

    {
        std::cout << "Processing a list of processes" << std::endl;
        for( std::vector< pProcess >::iterator p = first;
             p != last;
             p++ )
        {
            std::cout << "GENERIC ProcessList received: " << p->repr() << std::endl;
        }
    }
};

_CommandObject genericListOfProcesses()
{
return   _CommandObject( static_cast<void*>( new CommandObject<ShowListOfProcesses>()));
}


BOOST_PYTHON_MODULE(pySS)
{
    def("sendOneExperiment", sendOneExperiment );

    def("genericListOfProcesses", genericListOfProcesses );
    def("sendListOfProcesses", sendListOfProcesses );

    def("listPidsOnHost", &pFrameWork::listPidsOnHost);
    
    // Just so that Python knows about our cloaking
    class_<_CommandObject>("CommandObject", init<>())
        .def("describe", &_CommandObject::describe)
    ;

    class_<pHost>("Host", init<std::string>())
        .def("hostname", &pHost::hostname)
        .def("describe", &pHost::describe)
        .def("__repr__", &pHost::repr)
    ;
    
    class_<pFile>("File", init<std::string, pHost>())
        .def("filename", &pFile::filename)
        .def("hostname", &pFile::hostname)
        .def("host", &pFile::getHost, return_internal_reference<>())
        .def("describe", &pFile::describe)
        .def("__repr__", &pFile::repr)
    ;
    
    class_<pProcess>( "Process", init< unsigned long, pHost>() )
        .def("listThreads", &pProcess::listThreads)
        .def("host", &pThread::getHost,   return_internal_reference<>())
        .def("file", &pThread::getFile, return_internal_reference<>())
        .def("pid",  &pThread::getPid)
        
        // Description and overloaded python methods
        .def("describe", &pProcess::describe)
        .def("__repr__", &pProcess::repr)
    ;
    
    class_<pRank>( "Rank", init< pFile, unsigned long>() )
        .def("host", &pThread::getHost,   return_internal_reference<>())
        .def("file", &pThread::getFile, return_internal_reference<>())
        .def("rank",  &pThread::getRank)

        // Description and overloaded python methods
        .def("describe", &pRank::describe)
        .def("__repr__", &pRank::repr)
    ;
    
    class_<pPosixThread>( "PosixThread", 
        init< unsigned long, unsigned long, pHost>() )
        .def("host", &pThread::getHost,   return_internal_reference<>())
        .def("file", &pThread::getFile, return_internal_reference<>())
        .def("pid", &pThread::getPid)
        .def("tid", &pThread::getPosixThreadId)
        .def("describe", &pPosixThread::describe)
        .def("__repr__", &pPosixThread::repr)
    ;

    class_<pMetric>( "Metric",
        init< std::string >() )
        .def("__repr__", &pMetric::repr )
    ;
    
    class_<pExperiment>( "Experiment", init<pFile, std::string>())
        .def(init<std::string>())
        .def("file", &pExperiment::getFile, return_internal_reference<>())

        // expAtatch command
        .def("attachToProcess", &pExperiment::attachToProcess)
        .def("attachToPosixThread", &pExperiment::attachToPosixThread)
        .def("attachToMPIRank", &pExperiment::attachToMPIRank)

        // expDetach command
        .def("detachProcess", &pExperiment::detachProcess)
        .def("detachMPIRank", &pExperiment::detachMPIRank)
        .def("detachPosixThread", &pExperiment::detachPosixThread)

        .def("detachAll",  &pExperiment::detachPosixThread)

        // expClose command
        .def("close", &pExperiment::close )
        
        // expContinue command
        .def("continue", &pExperiment::restart)

        // expStop command
        .def("stop", &pExperiment::suspend)

        // expSaveFile command
        .def("saveFile", &pExperiment::saveToFile)
        
        // expSetParam command
        .def("setParam", &pExperiment::setParam)
        
        // expView
        .def("extractResults", &pExperiment::extractResults)

        // Description and overloaded python methods
        .def("describe", &pExperiment::describe)
        .def("__repr__", &pExperiment::repr)
    ;
    
    class_<pFrameWork>("FrameWork", init<>())

        .def("listDefinedExperiments", &pFrameWork::listDefinedExperiments)    
        .def("listHostsInCluster", &pFrameWork::listHostsInCluster)    
    ;

}

/* Python code example (or unit test):
import pySS
h=pySS.Host("localhost")
f=pySS.File("./a.out", h)
h.describe()
f.describe()
exp=pySS.Experiment(pySS.File("./a.out",pySS.Host("localhost")),"pcsamp")
exp.describe()
p=pySS.Process(1234, pySS.Host("localhost"))
rid=pySS.Rank( pySS.File("./a.out", pySS.Host("localhost")), 1)
exp.attachToMPIRank( rid )
l=p.listThreads()
exp.attachToPosixThread( l[0] )
*/

