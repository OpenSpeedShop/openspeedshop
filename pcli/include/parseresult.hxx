#ifndef __OPENSPEEDSHOP_CLI_PARSERESULT
#define __OPENSPEEDSHOP_CLI_PARSERESULT

#include <Python.h>
#include <boost/python.hpp>
#include <iostream>

namespace OpenSpeedShop { namespace cli {

  // The Parser result container
  // ParseResult is a container for recursive key search, or token,
  // ideal to hold parsing results.
  // ParseResult( "abcd", 1234 )
  // ParseResult( "foo", ParseResult( "bar", 9999))
  // The result is accessible using the operator[]( std::string ) or find
  // r["foo"] retuns ParseResult("bar", 9999)
  // ... almost. In fact it returns a boost::python::object type
  // and boos::python::object also has an operator[]( std::string ) defined
  // which gives the same flexibility. 
  // ParseResult is based on the Python dictionary type.
  // See Boost Python documentation for the C++ interface of boost::python::dict

    class ParseResult : public boost::python::dict
    {
        bool _isLeaf;

        public:
                
        ParseResult() : boost::python::dict()
        {
            _isLeaf = true;
        }

        ParseResult( const std::string& key, long v )
            : boost::python::dict()
        {
            (*this)[ key ] = v;
            _isLeaf = true;
        }

        ParseResult( const std::string& key, float f )
            : boost::python::dict()
        {
            (*this)[ key ] = f;
            _isLeaf = true;
        }

        ParseResult( const std::string& key, const std::string& s )
            : boost::python::dict()
        {
            (*this)[ key ] = s;
            _isLeaf = true;
        }

        // For all other types, this constructor will do 
        // as long as there is a way for boost::python to convert
        // the type T into a python base object 
        // aka boost::python::object.
        template <class T>
        ParseResult( const std::string& key, const T& v )
            : boost::python::dict()
        {
            (*this)[ key ] = v;
            _isLeaf = true;
        }

        // Just in case the default constructors don't work
        // this constructor can also be used.
        // ToPythonCvt as a function object is supposed to be 
        // able to convert the template type T into a boost::python::object
        template <class T, class ToPythonCvt>
        ParseResult( const std::string& key, const T& v )
            : boost::python::dict()
        {
            (*this)[ key ] = ToPythonCvt()(v);
            _isLeaf = true;
        }
        
        ParseResult( const std::string& key, const ParseResult& r )
            : boost::python::dict()
        {
            (*this)[ key ] = r;
            _isLeaf = false;
        }
        
        ParseResult( const std::string& key, const boost::python::dict& r )
            : boost::python::dict()
        {
            // Update the dictionary with the given dictionary
            // Since we're in a constructor, this dictionary is empty
            update( r );
            _isLeaf = false;
        }
        
        virtual ~ParseResult()
        {
        }
        

        // Adds up two parse results.
        // Actually it just appends the result of one to the other
        // If there is any duplicate, rhs wins
        ParseResult operator+( const ParseResult& rhs ) const
        {
            ParseResult r;
            r.update( *this );
            r.update( rhs );
            return r;
        }        
        
        // Adds up two parse results.
        // Actually it just appends the result of one to the other
        // If there is any duplicate, rhs wins
        ParseResult& operator+=( const ParseResult& rhs )
        {
            update( rhs );
            return *this;
        }        

        boost::python::object find( std::string const& key )
        {
            return (*this)[key];
        }

        boost::python::object getattr( char const* key )
        {
#ifdef OSS_DEBUG
            std::cout << "getattr(" << key << ")" 
                      << std::endl;
#endif
            return attr( key );
        }

    };
    
}}
#endif
