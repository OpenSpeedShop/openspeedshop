#include <boost/python.hpp>

#include <iostream>
#include <string>
#include <vector>

struct boost_string
{
    
    boost::python::str operator()( std::string s )
    {
        return boost::python::str( s.c_str(), s.size() );
    }
};

struct boost_list
{
    template <typename ForwardIterator>
    boost::python::list operator()( const ForwardIterator& f, 
                                    const ForwardIterator& l)
    {
        boost::python::list pyList;
        for( ForwardIterator it = f; it != l; it++ )
        {
            pyList.append( *it );
        }
        return pyList;
    }
};


struct pHost
{
    // The host name
    std::string     _name;

    pHost()
        : _name( "<None>" )
    {
    }
    
    pHost( std::string hostname )
        : _name( hostname )
    {
    }
    
    const std::string hostname() const
    {
        return _name;
    }

    boost::python::str describe() const
    {
        return boost_string()(hostname());
    }
    
    std::string repr() const
    {
        std::ostringstream sbuf;
        sbuf << "hostname:" << _name ;
        return sbuf.str();
    }
};

struct pFile
{
    pHost _host;
    std::string _fname;
    pFile( std::string filename, const pHost& host )
        : _fname( filename ), _host( host._name )
    {
    }

    const pHost& getHost() const 
    { 
        return _host; 
    } 

    const std::string filename() const
    {
        return _fname;
    }

    const std::string hostname() const
    {
        return _host.hostname();
    }

    boost::python::tuple describe() const
    {
        return boost::python::make_tuple( filename(), _host.hostname());
    }

    std::string repr() const
    {
        std::ostringstream sbuf;
        sbuf << "(" << "filename:" << _fname << ", " << _host.repr() << ")" ;
        return sbuf.str();
    }
};




struct pThread
{
    pFile _file;
    unsigned long _pid;
    unsigned long _rank;
    unsigned long _tid;
    
    pThread( unsigned long pid, pHost host  )
        : _file("a.out", host), _pid(pid), _tid(0), _rank(0)
    {
        // Should we get the missing info?
    }

    pThread( unsigned long pid, unsigned long tid, pHost host )
        : _file("a.out", host), _pid(pid), _tid(tid), _rank(0)
    {
        // Should we get the rankid, if it exists ?
    }

    pThread( pFile file, unsigned long rank )
        : _file(file), _pid(0), _tid(0), _rank(rank)
    {
        // Should we get the pid, since it always exist
    }

    const pHost& getHost() const { return _file.getHost(); }
    const pFile& getFile() const { return _file; }
    unsigned long getPid()  { return _pid; }
    unsigned long getRank() { return _rank; }
    unsigned long getPosixThreadId() { return _tid; }
};


struct pRank : public pThread
{
    pRank( pFile file, unsigned long rank ) 
        : pThread( file, rank )
    {}
    
    boost::python::tuple describe() const 
    {
        return boost::python::make_tuple( _file.describe(), _rank );
    }
    
    std::string repr() const
    {
        std::ostringstream sbuf;
        sbuf << "(file:" << _file.repr() << ", rank:" << _rank << ")" ;
        return sbuf.str();
    }
};

struct pPosixThread : public pThread
{
    pPosixThread( unsigned long pid, unsigned long tid, pHost host )
        : pThread( pid, tid, host )
    {}

    boost::python::tuple describe() const
    {
        return boost::python::make_tuple( _file.describe(), 
                    boost::python::make_tuple( _pid, _tid) );
    }

    std::string repr() const
    {
        std::ostringstream sbuf;
        sbuf << "(" << _file.repr() << ", " 
             << "(pid:" << _pid << ", tid:" << _tid << ")" << ")";
        return sbuf.str();
    }
};



struct pProcess : public pThread
{
    pProcess( unsigned long pid, pHost host )
        : pThread( pid, host )
    {}
    
    boost::python::tuple describe() const
    {
        return boost::python::make_tuple( _file.describe(), _pid );
    }

    boost::python::list 
    listThreads() const
    {
        //Simulates FW call to get the Threads in one process
        // And creates the objects for each Posix thread
        std::vector<pPosixThread> v;
        v.push_back( pPosixThread( _pid, 1234, _file.getHost()) );
        v.push_back( pPosixThread( _pid, 2345, _file.getHost()) );
        v.push_back( pPosixThread( _pid, 4567, _file.getHost()) );
        
        
        return boost_list() ( v.begin(), v.end() );
    }
    
    std::string repr() const
    {
        std::ostringstream sbuf;
        sbuf << "(" << _file.repr() << ", pid:" << _pid << ")";
        return sbuf.str();
    }
};

class pExperiment
{
    pFile       _file;
    std::string _type;
    std::string _name;

public:
    pExperiment(std::string type)
        : _file(pFile("<None>", pHost("<None>"))), _type( type ),
        _name("<None>")
    {
    }

    pExperiment( pFile file, std::string type)
        : _file(file), _type( type ), _name( type ) 
    {
    }

    const pFile& getFile() const 
    { 
        return _file;
    }
    
    std::string rename(std::string name)
    {
        _name = name;
        return _name;
    }
    
    void attachToProcess( const pProcess p )
    {
        std::cout << "Unimplemented call to "
            << "FWapi::Experiment::attach<pProcess>" 
            << "(" << p.repr() << ")"
            << ": " << repr()
            << std::endl;
    }
    
    void attachToMPIRank( const pRank r )
    {
        std::cout << "Unimplemented call to "
            << "FWapi::Experiment::attach<pRank>"
            << "(" << r.repr() << ")"
            << ": " << repr()
            << std::endl;
    }
    
    void attachToPosixThread( const pPosixThread t )
    {
        std::cout << "Unimplemented call to "
            << "FWapi::Experiment::attach<pPosixThread>" 
            << "(" << t.repr() << ")"
            << ": " << repr()
            << std::endl;
    }
    
    void detachProcess( const pProcess p )
    {
        std::cout << "Unimplemented call to "
            << "FWapi::Experiment::detach<pProcess>"
            << "(" << p.repr() << ")"
            << ": " << repr()
            << std::endl;
    }
    
    void detachMPIRank( const pRank r )
    {
        std::cout << "Unimplemented call to "
            << "FWapi::Experiment::detach<pRank>"
            << "(" << r.repr() << ")"
            << ": " << repr()
            << std::endl;
    }
    
    void detachPosixThread( const pPosixThread t )
    {
        std::cout << "Unimplemented call to "
            << "FWapi::Experiment::detach<pPosixThread>"
            << "(" << t.repr() << ")"
            << ": " << repr()
            << std::endl;
    }
    
    void detachAll()
    {
        std::cout << "Unimplemented call to "
            << "FWapi::Experiment::detachAll"
            << ": " << repr()
            << std::endl;
    }
    
    void close() 
    {
        std::cout << "Unimplemented call to "
            << "FWapi::Experiment::close"
            << ": " << repr()
            << std::endl;
    }
    
    void restart()
    {
        std::cout << "Unimplemented call to "
            << "FWapi::Experiment::restart"
            << ": " << repr()
            << std::endl;
    }

    void suspend()
    {
        std::cout << "Unimplemented call to "
            << "FWapi::Experiment::suspend"
            << ": " << repr()
            << std::endl;
    }

    void saveToFile( std::string filename )
    {
        std::cout << "Unimplemented call to "
            << "FWapi::Experiment::saveToFile"
            << "(" << filename << ")"
            << ": " << repr() 
            << std::endl;
    }
    
    void setParam( std::string param, boost::python::object val )
    {
        try
        {
            std::cout << "Unimplemented call to "
                << "FWapi::Experiment::setParam"
                << "(" << param << "=" << boost::python::extract<char*>(val) << ")"
                << ": " << repr() 
                << std::endl;
        }
        
        catch(boost::python::error_already_set)
        {
            if( PyErr_ExceptionMatches( PyExc_TypeError ))
            {
                std::cout << "Bad type for parameter:" << param
                          << ". Expected a string." 
                          << std::endl;
                PyErr_Clear();
            }
            else
            {
                PyErr_Print();
                return;
            }
        }
    }
    
    boost::python::list extractResults(std::string metric)
    {
        std::cout << "Unimplemented call to "
            << "FWapi::Experiment::extractResult" 
            << "(" << metric << ")"
            << ": " << repr() 
            << std::endl;
        boost::python::list l;
        l.append( boost::python::make_tuple(
            // The host
            boost::python::str( _file.getHost().hostname().c_str() ),
            // The binary
            boost::python::str( "/home/nobody/whatever/bin/a.out" ),
            // The compiled source file 
            boost::python::str( "/home/nobody/whatever/src/main.cpp"),
            // The source file 
            boost::python::str( "/home/nobody/whatever/src/main.cpp"),
            //The function name
            boost::python::str( "::main"),
            // The line# and col#
            1506, 25,
            // The metric data
            boost::python::make_tuple(10, 410) ));
            
        l.append( boost::python::make_tuple(
            boost::python::str( _file.getHost().hostname().c_str() ),
            boost::python::str( "/home/nobody/whatever/lib/foo.so" ),
            boost::python::str( "/home/nobody/whatever/src/foo.cpp"),
            boost::python::str( "/home/nobody/whatever/include/foo.hpp"),
            boost::python::str( "someclass::function_foo(int,float)"),
            2207, 22,
            boost::python::make_tuple(220,12) ));
        
        return l;
    }

    boost::python::tuple describe() const
    {
        return boost::python::make_tuple( _file.describe(), 
                    boost::python::make_tuple
                        ( boost_string()( _name),
                          boost_string()( _type)) );
    }
    
    std::string repr() const
    {
        std::ostringstream sbuf;
        sbuf << "(" << _file.repr() << ", " 
                    << "(name:" << _name << ", " << "base:"<< _type << ")"
             << ")";
        return sbuf.str();
    }

private:
    
};



class pFrameWork
{
public:
    pFrameWork() {}
    // listExps command
    static boost::python::list listDefinedExperiments()
    {
        std::cout << "Unimplemented call to "
            << "FWapi::listDefinedExperiments" 
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
    static boost::python::list listPidsOnHost( pHost host)
    {
        boost::python::list l;
        l.append( boost::python::make_tuple(
               pProcess( 1234, host ),
               pFile( "/home/foo/bin/bar", host)));
        l.append( boost::python::make_tuple(
               pProcess( 456, host ),
               pFile( "/bin/whatever", host )));
        return l;
    }

    // listHosts command
    static boost::python::list listHostsInCluster()
    {
        boost::python::list l;
        l.append( pHost( "baguette.engr.sgi.com" ) );
        l.append( pHost( "bastille2.engr.sgi.com" ) );
        l.append( pHost( "hope.americas.sgi.com" ) );
        l.append( pHost( "hope2.americas.sgi.com" ) );
        return l;
    }
};
