#include "Python.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "ToolAPI.hxx"
using namespace OpenSpeedShop::Framework;

#include "support.h"
#include "Commander.hxx"
#include "Clip.hxx"

extern FILE *yyin;
extern int yyparse (void);

static PyObject *SS_CallParser (PyObject *self, PyObject *args) {
    char *input_line = NULL;
    PyObject *p_object;
    int ret;

    if (!PyArg_ParseTuple(args, "s", &input_line)) {
    	;
    }

    yyin = fopen("/usr/tmp/jack.tmp","w+");
    memset(&command,0,sizeof(command_t));

    cmd_init();
 
    fprintf(yyin,"%s\n", input_line);
    rewind(yyin);

    ret = yyparse();

    fclose(yyin); 

    p_object = Py_BuildValue("");

    return p_object;
}

static PyObject *SS_ReadLine (PyObject *self, PyObject *args) {
  int more = 0;
  
  if (!PyArg_ParseTuple(args, "i", &more))
        return NULL;
  InputLineObject *clip = SpeedShop_ReadLine(more);
  if (clip == NULL) {
    PyErr_SetString(PyExc_EOFError, "End Of File");
    return NULL;
  }
  int64_t buffer_size = clip->Command().length()+1;
  bool need_newline = false;
  if (clip->Command().c_str()[buffer_size-2] != *("\n")) {
    buffer_size++;
    need_newline = true;
  }
  char *sbuf = (char *)PyMem_Malloc(buffer_size);
  strcpy (sbuf, clip->Command().c_str());
  if (need_newline) {
    sbuf[buffer_size-2] = *("\n");
    sbuf[buffer_size-1] = *("\0");
  }
  delete (clip);
  return Py_BuildValue("s", sbuf);
}

static PyObject *SS_expFocus (PyObject *self, PyObject *args) {
  EXPID exp = 0;
  char *arg0 = NULL;
  if ((args == NULL) || (!(PyArg_ParseTuple(args, "s", &arg0)) || (arg0 == NULL))) {
    exp = Experiment_Focus ( 0 );
  } else {
    if (!PyArg_ParseTuple(args, "L", &exp)) {
      exp = Experiment_Focus ( 0, exp);
    }
  }
  return Py_BuildValue("L", exp);
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

    {"CallParser",  SS_CallParser, METH_VARARGS,
     "Call the YACC'd parser."},
	
    {"ReadLine",  SS_ReadLine, METH_VARARGS,
     "Read a SpeedShop command."},

    {"expFocus",  SS_expFocus, METH_VARARGS,
     "Set the focused experiment."},

    {"record",  SS_Record, METH_VARARGS,
     "Control Command tracing."},

    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initPY_Input (void) {
  (void) Py_InitModule("PY_Input", PY_Input_Methods);
}
