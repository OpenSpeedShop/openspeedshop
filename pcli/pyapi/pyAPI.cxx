#include <boost/python.hpp>
#include <boost/python/return_internal_reference.hpp>
#include "pyAPI.hxx"


// static pHost pLocalHost("localhost" );


#ifdef NOPYTHON

main()
{
    pHost localhost("localhost");
    pFile f( "a.out", localhost );
}

#else
using namespace boost::python;

// template <class T>
// struct describe
// {
//     static PyObject* convert(const T& x)
//     {
//         return boost::python::incref(x.describe().ptr());
//     }
// };

list listPidsOnHost( pHost host )
{
    return pFrameWork::listPidsOnHost( host );
}


BOOST_PYTHON_MODULE(pySS)
{
    def("listPidsOnHost", &pFrameWork::listPidsOnHost);
    
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

//    to_python_converter< pPosixThread, describe<pPosixThread> >();
//    to_python_converter< pProcess, describe<pProcess> > ();
}

/* Python code
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

#endif
