#include "Python.h"
#include <stdio.h>

char *SpeedShop_ReadLine (int is_more);

static PyObject *SS_ReadLine (PyObject *self, PyObject *args) {
  int more = 0;
  if (!PyArg_ParseTuple(args, "i", &more))
        return NULL;
  char *news = SpeedShop_ReadLine(more);
  if (news == NULL) {
    PyErr_SetString(PyExc_EOFError, "End Of File");
    return NULL;
  }
  return Py_BuildValue("s", news);
}

static PyMethodDef PY_Input_Methods[] = {
    {"ReadLine",  SS_ReadLine, METH_VARARGS,
     "Read a SpeedShop command."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initPY_Input (void) {
  (void) Py_InitModule("PY_Input", PY_Input_Methods);
}
