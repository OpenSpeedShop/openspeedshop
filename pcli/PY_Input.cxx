#include "Python.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "support.h"

extern FILE *yyin;
extern int yyparse (void);

char *SpeedShop_ReadLine (int is_more);
void  SpeedShop_Trace_ON (char *tofile);
void  SpeedShop_Trace_OFF(void);

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

    {"CallParser",  SS_CallParser, METH_VARARGS,
     "Call the YACC'd parser."},
	
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
