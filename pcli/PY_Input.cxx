#include "Python.h"
#include <stdio.h>
// #include <ctype.h>
// #include <string.h>

#include "SS_Input_Manager.hxx"
#include "SS_Cmd_Execution.hxx"

extern FILE *yyin;
extern int yyparse (void);
void reset_command();

/* Global Data for tracking the current command line. */
InputLineObject *Current_ILO = NULL;
CommandObject   *Current_CO  = NULL;

static PyObject *SS_CallParser (PyObject *self, PyObject *args) {
    char *input_line = NULL;
    PyObject *p_object = NULL;
    int ret;
    CommandObject *cmd = NULL;
    command_t *cmd_args = NULL;

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

   // Build a CommandObject so that the semantic routines can be called.
    cmd_args = (command_t *)malloc(sizeof(command_t));
    memcpy (cmd_args, &command, sizeof(command_t));
    cmd = new CommandObject (cmd_args);
    SS_Execute_Cmd (cmd);
    
   // Free up and cleanup any parser generated structures
   //  reset_command();

   // Build Python Objects for any return results.
    {
      std::list<CommandResult *> cmd_result = cmd->Result_List();
      std::list<CommandResult *>::iterator cri;
      int cnt = 0;
      for (cri = cmd_result.begin(); cri != cmd_result.end(); cri++) {
        if (cri != NULL) {
          switch ((*cri)->Type()) {
          case CMD_RESULT_INT:
          {
            int64_t I = 0;
            ((CommandResult_Int *)(*cri))->Value(&I);
            p_object = Py_BuildValue("l", I);
            break;
          }
          case CMD_RESULT_STRING:
          {
            std::string C;
            ((CommandResult_String *)(*cri))->Value(&C);
//            p_object = Py_BuildValue("S", C.c_str());
    	      p_object = Py_BuildValue("s",C.c_str());
            break;
          }
          default:
           break;
          }
        }
      }
    }

    if (p_object == NULL) {
      p_object = Py_BuildValue("");
    }

    return p_object;
}

// Save a pointer to the most recent Input_Line_Object.
//
// Since Python likes character strings better than anything,
// Convert the pointer to a string so that Python can manipulate
// it without knowing what it is.  The only use of this string
// will be by the routine SS_ReadILO.
//
static PyObject *SS_DelayILO (PyObject *self, PyObject *args) {
  char *seqnum = (char *)PyMem_Malloc(32);
  snprintf( seqnum, 32, "%p", Current_ILO);
  return Py_BuildValue("s", seqnum);
}

// Extract the input line from an Input_Line_Object.
//
// Save this line in a Python object.
// While at it, set "current" pointers.
//
static PyObject *Prepare_Input_Line (InputLineObject *clip) {
  int64_t buffer_size = clip->Command().length()+1;
  bool need_newline = false;

 // Python requires that every input line end with an EOL character.
  if (clip->Command().c_str()[buffer_size-2] != *("\n")) {
    buffer_size++;
    need_newline = true;
  }

 // Use Python's memory allocator so that it will free the space when done.
  char *sbuf = (char *)PyMem_Malloc(buffer_size);
  strcpy (sbuf, clip->Command().c_str());
  if (need_newline) {
    sbuf[buffer_size-2] = *("\n");
    sbuf[buffer_size-1] = *("\0");
  }

 // Set global variables to track the current line.
  Current_ILO = clip;
  Current_CO = NULL;

  return Py_BuildValue("s", sbuf);
}

// Convert a Python character string to an ILO pointer.
// Return the associated line of data to Python.
//
static PyObject *SS_ReadILO (PyObject *self, PyObject *args) {
  char *ILO_Addr = NULL;
  InputLineObject *clip = NULL;
  
  if (!PyArg_ParseTuple(args, "s", &ILO_Addr))
        return NULL;
  if (ILO_Addr == NULL) {
   // This should be a more severe error
    PyErr_SetString(PyExc_EOFError, "End Of File");
    return NULL;
  }
  int r = sscanf (ILO_Addr, "%p", &clip);
  return Prepare_Input_Line(clip);
}

// Get an Input_Line_Object from the input manager.
// Return the associated line of data to Python.
//
static PyObject *SS_ReadLine (PyObject *self, PyObject *args) {
  int more = 0;
  
  if (!PyArg_ParseTuple(args, "i", &more))
        return NULL;
  InputLineObject *clip = SpeedShop_ReadLine(more);
  if (clip == NULL) {
    PyErr_SetString(PyExc_EOFError, "End Of File");
    return NULL;
  }
  return Prepare_Input_Line(clip);
}

static PyMethodDef PY_Input_Methods[] = {

    {"CallParser",  SS_CallParser, METH_VARARGS,
     "Call the YACC'd parser."},

    {"Save_ILO",  SS_DelayILO, METH_VARARGS,
     "Save the Current_ILO."},

    {"ReadILO",  SS_ReadILO, METH_VARARGS,
     "Use a saved ILO to get a command."},

    {"ReadLine",  SS_ReadLine, METH_VARARGS,
     "Read a SpeedShop command."},

    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initPY_Input (void) {
  (void) Py_InitModule("PY_Input", PY_Input_Methods);
}
