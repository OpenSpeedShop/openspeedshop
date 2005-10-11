#include <Python.h>
#include <iostream>

namespace OpenSpeedShop { namespace Python
{
// Initialization of Python API.
// Keep it as a static to be sure it gets executed at startup and
// finalized at exit time.

    static struct PyInitFini
    {
        PyInitFini()
        {
            if( !Py_IsInitialized() )
            {
                std::cout << "Initializing Python" << std::endl;
                Py_Initialize();
            }
        }
        ~PyInitFini()
        {
            if( Py_IsInitialized() )
            {
                PyErr_Clear();        
                std::cout << "Finalizing Python" << std::endl;
                Py_Finalize();
            }
        }
    } _py00;
}}
