#include <boost/python.hpp>

#include <iostream>
#include <string>
#include <vector>

#include "commandobject.hxx"
#include "outputobj.hxx"

namespace OpenSpeedShop { namespace cli {

struct boost_string
{
    
    boost::python::str operator() ( const std::string s ) const
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

    pHost( pHost const& ref )
        : _name( ref._name )
    {}

    virtual ~pHost() {}
    
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


struct pMetric
{
    // The host name
    std::string     _name;

    pMetric()
        : _name( "<None>" )
    {
    }
    
    pMetric( std::string name )
        : _name( name )
    {
    }

    pMetric( pMetric const& ref )
        : _name( ref._name )
    {}

    virtual ~pMetric() {}
    
    const std::string metricname() const
    {
        return _name;
    }

    boost::python::str describe() const
    {
        return boost_string()(metricname());
    }
    
    std::string repr() const
    {
        std::ostringstream sbuf;
        sbuf << "metricname:" << _name ;
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

    pFile( pFile const& ref )
        : _host( ref._host ), _fname( ref._fname )
    {}

    virtual ~pFile() {}
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

    pThread( pThread const& ref )
        : _file(ref._file), _pid(ref._pid), _tid(ref._pid), _rank(ref._rank)
    {}

    virtual ~pThread() {}

    const pHost& getHost() const           { return _file.getHost(); }
    const pFile& getFile() const           { return _file; }
    unsigned long getPid() const           { return _pid; }
    unsigned long getRank() const          { return _rank; }
    unsigned long getPosixThreadId() const { return _tid; }
};


struct pRank : public pThread
{
    pRank( pFile file, unsigned long rank ) 
        : pThread( file, rank )
    {}
    
    virtual ~pRank() {}

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

struct pProcess : public pThread
{
    pProcess( unsigned long pid, pHost host )
        : pThread( pid, host )
    {}
    
    virtual ~pProcess() {}
    boost::python::tuple describe() const
    {
        return boost::python::make_tuple( _file.describe(), _pid );
    }

    boost::python::list listThreads() const;
    
    std::string repr() const
    {
        std::ostringstream sbuf;
        sbuf << "(" << _file.repr() << ", pid:" << _pid << ")";
        return sbuf.str();
    }
};

struct pPosixThread : public pThread
{
    pPosixThread( unsigned long pid, unsigned long tid, pHost host )
        : pThread( pid, tid, host )
    {}

    pPosixThread( pProcess const& proc, unsigned long tid )
        : pThread( proc.getPid(), tid, proc.getHost() )
    {}

    virtual ~pPosixThread() {}
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



class pExperiment
{
    pFile       _file;
    std::string _type;
    std::string _name;

public:
    pExperiment(std::string type);
    pExperiment( pFile file, std::string type);

    pExperiment( pExperiment const& ref)
        : _file( ref._file ), _type( ref._type ), _name( ref._name )
    {}

    virtual ~pExperiment()
    {}

    const pFile& getFile() const 
    { 
        return _file;
    }
    
    std::string rename(std::string name)
    {
        _name = name;
        return _name;
    }
    
    void attachToProcess( const pProcess p );
    void attachToMPIRank( const pRank r );
    void attachToPosixThread( const pPosixThread t );
    
    void detachProcess( const pProcess p );
    void detachMPIRank( const pRank r );
    void detachPosixThread( const pPosixThread t );
    
    void detachAll();
    
    void close(); 
    void restart();

    void suspend();

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
    
    boost::python::list extractResults(pMetric metric);

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



// Basically the implementation of the API.
// This is the bridge to the FrameWork and here we have the
// knowledge of the Command Object, but has a cloaked 
// pointer, because of Pyhthon's total ignorance of
// the notion of uninstanciated template.
// Most of those functions will not be exported to Python


class pFrameWork
{
public:
    pFrameWork() {}

    virtual ~pFrameWork() {}

    // listExps command
    static boost::python::list listDefinedExperiments();
   
    // listPids command
    static boost::python::list listPidsOnHost( pHost host);

    // listHosts command
    static boost::python::list listHostsInCluster();


    static void attachExpToProcess( pExperiment const& exp, 
                                    pProcess const& proc );

    static void attachExpToMPIRank( pExperiment const& exp, 
                                    pRank const& rank );

    static void attachExpToPosixThread( pExperiment const& exp, 
                                        pPosixThread const& pthread );

    static boost::python::list listThreadsInProcess( pProcess const& proc );

    static pProcess createProcess( pFile const& file );

    static pExperiment createExperiment( pExperiment const& exp );

    static boost::python::list extractExpResults( pExperiment const& exp, 
                                                  pMetric const& metric );

};


class pCommandObject
{
public:
    template <class T>
    static void sendOne( T const& obj, _CommandObject const& cmdobj )
    {
        if( cmdobj.ptr() )
        {
            typedef SingleDataOutputObject< T > OutputObject;
            typedef CommandObject< OutputObject > CmdObject;
    
            const CmdObject* o = static_cast<const CmdObject*>( cmdobj.ptr() );
            const OutputObject* out = o->outputObj();
            if( out )
            {
               (*out)( obj );
            }
            else
               std::cout << "pCommandObject::sendOne: No output here !!" << std::endl;
            o->dataIsReady();
        } 
        else
           std::cout << "pCommandObject::sendOne: No command object here !!" << std::endl;
    }

    template <class T, typename ForwardIterator>
    static void sendList( boost::python::list const& l, _CommandObject const& cmdobj )
    {
        if( cmdobj.ptr() )
        {
            typedef ForwardIteratorOutputObject< ForwardIterator > OutputObject;
            typedef CommandObject< OutputObject > CmdObject;
    
            const CmdObject* o = static_cast<const CmdObject*>( cmdobj.ptr() );
            const OutputObject* out = o->outputObj();
 
            if( out )
            {
               std::vector<T> v;
               std::cout << "Generating std::vector" << std::endl;
               std::cout << "I have " << boost::python::extract<long>( l.attr("__len__")())
                         << "items" << std::endl;
               for( long i=0; i < boost::python::extract<long>( l.attr("__len__")()); i++ )
               {
                    try
                    {
                        T t= boost::python::extract<T>(l[i]);
                        v.push_back( t );
                    }
                    
                    catch( boost::python::error_already_set )
                    {
                        std::cerr << "Something's wrong: " << __FILE__ << ", " << __LINE__
                                  << std::endl;
                        PyErr_Print();
                    }
               }
               std::cout << "Sending std::vector over" << std::endl;
               
               (*out)( v.begin(), v.end() );
            }
            else
               std::cout << "pCommandObject::sendList: No output here !!" << std::endl;
            o->dataIsReady();
        } 
        else
           std::cout << "pCommandObject::sendList: No command object here !!" << std::endl;
    }

};

}}
