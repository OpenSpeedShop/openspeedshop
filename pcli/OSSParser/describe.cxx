#include "describe.hxx"

std::ostream& operator<< (std::ostream& out, PyObject* o )
{
    if( !o )
        out << "(nil)" << std::endl;
    else
    if(     PyObject_TypeCheck( o, &PyString_Type))
        out << "'" << PyString_AsString(o) << "'";
    else
    if(     PyObject_TypeCheck( o, &PyLong_Type))
        out << PyLong_AsLong(o) << "L";
    else
    if(     PyObject_TypeCheck( o, &PyInt_Type))
        out << PyInt_AsLong(o) << "i";
    else
    if(     PyObject_TypeCheck( o, &PyList_Type))
    {
        out << " list: " << (void*) o << " [cnt=" << o->ob_refcnt << "] =";
        out << "[";
        for( int i=0; i < PyList_Size( o ); i++ )
        {
            PyObject* item = PyList_GetItem( o, i );
            if( i ) 
                out << ", ";
            out << item;
        }
        out << "]";
    }
    else
    if(     PyObject_TypeCheck( o, &PyTuple_Type))
    {
        out << " tuple: " << (void*) o << " [cnt=" << o->ob_refcnt << "] =";
        out << "(";
        for( int i=0; i < PyTuple_Size( o ); i++ )
        {
            PyObject* item = PyTuple_GetItem( o, i );
            if( i ) 
                out << ", ";
            out << item;
        }
        out << ")";
    }
    else
    if(     PyObject_TypeCheck( o, &PyDict_Type))
    {
        out << " dict: " << (void*) o << " [cnt=" << o->ob_refcnt << "] =";
        out << "{";
        
        PyObject* key, *v;
        int i=0, pos=0;
        while( PyDict_Next( o, &pos, &key, &v ))
        {
            if( i++ > 0 )
                out << ", ";
            out << key << ": " << v;
        }

        out << "}";
    }
    else
    {
        out << "<" << (void*) o << ": Unknown type";
        PyObject_Print( PyObject_Type(o), stdout, Py_PRINT_RAW );
    }

    return out;
}





void describe( PyObject* ptr )
{
    std::cout << "describe( " << (void*)ptr << ")" << std::endl;
    std::cout << ptr << std::endl;
}


#ifdef PYTHONMODULE
#include <boost/python.hpp>
using namespace boost::python;
BOOST_PYTHON_MODULE(Describe)
{
    // debug function
    def("describe", describe );

}

#endif
