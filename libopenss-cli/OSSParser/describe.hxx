#include <Python.h>
#include <iostream>

std::ostream& operator<< (std::ostream& out, PyObject* o );
void describe( PyObject* ptr );
