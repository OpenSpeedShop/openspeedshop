#include "Python.h"
#include <stdio.h>

char *SpeedShop_ReadLine (int is_more);
void  SpeedShop_Trace_ON (char *tofile);
void  SpeedShop_Trace_OFF(void);

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

static PyObject *SS_Record (PyObject *self, PyObject *args) {
  char *tofile = NULL;
  if (!PyArg_ParseTuple(args, "s", &tofile)) {
    (void)SpeedShop_Trace_OFF ();
  } else {
    (void)SpeedShop_Trace_ON(tofile);
  }
  return Py_BuildValue("s", NULL);
}

static PyMethodDef PY_Input_Methods[] = {
    {"ReadLine",  SS_ReadLine, METH_VARARGS,
       "Read a SpeedShop command."},
    {"record",  SS_Record, METH_VARARGS,
       "Control Command tracing."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initPY_Input (void) {
  (void) Py_InitModule("PY_Input", PY_Input_Methods);
}
