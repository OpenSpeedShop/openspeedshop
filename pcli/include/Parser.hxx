#include <boost/python.hpp>
#include "parseresult.hxx"
#include "commandobject.hxx"
#include <iostream>

namespace OpenSpeedShop { namespace cli {

class Parser
{
    boost::python::object _module;
    boost::python::object _parsefunc;
    boost::python::object _semantic;


public:

    Parser( std::string const& modulename ) :
          _module(( boost::python::handle<>( PyImport_ImportModule( 
             const_cast<char*>(modulename.c_str()) ))))
    {
        PyObject_Print( _module.ptr(), stdout, Py_PRINT_RAW );
        _parsefunc = _module.attr( "parse" );
        PyObject_Print( _parsefunc.ptr(), stdout, Py_PRINT_RAW );
        _semantic = _module.attr( "semantic" );
        PyObject_Print( _semantic.ptr(), stdout, Py_PRINT_RAW );
    }

    void execute( const char* str, _CommandObject cmdobj)
    {
        ParseResult r = parse( str );
        if( cmdobj.ptr() == NULL ) 
            cmdobj = createCommandObject( r );
        semantic( r, cmdobj ); 
    }

    template <class OutputObject>
    void execute( const char* str,
                  CommandObject<OutputObject> const& obj)
    {
        execute( str, _CommandObject( static_cast<const void*>(obj)) );
    }

protected:
    OpenSpeedShop::cli::ParseResult parse( std::string str )
    {
        ParseResult r = boost::python::call
                           <OpenSpeedShop::cli::ParseResult>( 
                                _parsefunc.ptr(), this, str );
        return r;
    }

    virtual _CommandObject createCommandObject(OpenSpeedShop::cli::ParseResult const& r)
    {
        std::cout << "Base Parser: Should create a command object here" << std::endl;
        return _CommandObject(NULL);
    }

    void semantic( OpenSpeedShop::cli::ParseResult const& r,
                   _CommandObject const& cmdobj )
    {
        boost::python::call<void> ( _semantic.ptr(), r, cmdobj );
    }
};

}}
