/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/


#include "Python.h"
#include "SS_Input_Manager.hxx"

extern void pcli_load_messages(void);

extern FILE *yyin;
extern int yyparse (void);
extern int yydebug;
extern int yynerrs;
extern char yychar;
// extern yylval;
ParseResult *p_parse_result;

/* Global Data for tracking the current command line. */
InputLineObject *Current_ILO = NULL;
CommandObject   *Current_CO  = NULL;
CommandObject   *p_cmdobj = NULL;

// The following mechanism is defined to allow python programs
// to capture the output of OSS commands and avoid sending it
// to the input window for printing.
//
//   (Embedded_WindowID != 0) is the default action
//   cmd_output_to_python defines the action for individual commands

static CMDWID Embedded_WindowID = 0;
static bool cmd_output_to_python = false;

static PyObject *SS_Set_Assign (PyObject *self, PyObject *args) {
  cmd_output_to_python = true;
  return Py_BuildValue("");
}

// Parse and execute an individual command
static PyObject *SS_CallParser (PyObject *self, PyObject *args) {
    char *input_line = NULL;
    PyObject *p_object = NULL;
    int ret;
    CommandObject *cmd = NULL;
    ParseResult parse_result = ParseResult();
    int i;
    bool list_returned = false;
    PyObject *py_list = NULL;

   // Copy the desired action and reset the default action
    bool python_needs_result = cmd_output_to_python;
    cmd_output_to_python = (Embedded_WindowID != 0);
    
    // Give yacc access to ParseResult object.
    p_parse_result = &parse_result;
    
    if (!PyArg_ParseTuple(args, "s", &input_line)) {
    	;
    }
    
    yyin = tmpfile();

    fprintf(yyin,"%s\n", input_line);
    rewind(yyin);

    //yydebug = 1;
    //yynerrs = 0; // If this is not set then error conditions are carried over.
    //yychar = '\0';
    //cout << "in SS_CallParser():" << endl;
    //cout << "\tyynerrs = " << yynerrs << endl;
    //cout << "\tyydebug = " << yydebug << endl;
    //cout << "\tyychar = " << yychar << endl;
    ret = yyparse();

    fclose(yyin); 
    
    // testing code
    //parse_result.dumpInfo();

    // Build a CommandObject so that the semantic routines can be called.
    cmd = new CommandObject (&parse_result, python_needs_result);

    // See if the parse went alright.
    if ((p_parse_result->syntaxError()) ||
        (p_parse_result->getCommandType() == CMD_HEAD_ERROR)) {
        cmd->Result_String ("Parsing failed");
        cmd->set_Status(CMD_ERROR);

    	// I should be reporting exactly what went wrong here.
    	p_object = Py_BuildValue("");
    	return p_object;
    }

    SS_Execute_Cmd (cmd);

    if (!python_needs_result ||
        (cmd->Status() == CMD_ERROR) ||
        (cmd->Status() == CMD_ABORTED) ||
        (cmd->Clip()->What() == ILO_ERROR)) {
     // If the result is not needed within Python
     // or if something went wrong, return a null value to Python.
      p_object = Py_BuildValue("");
      return p_object;
    }
    
   // Build Python Objects for any return results.
    {
      std::list<CommandResult *> cmd_result = cmd->Result_List();
      std::list<CommandResult *>::iterator cri;

      list_returned = cmd_desc[cmd->Type()].ret_list;
      if (!list_returned && (cmd_result.size() > 1)) {
        cmd->Result_String ("Too many results were generated for the command");
        cmd->set_Status(CMD_ERROR);
      }

    	// Start building python list object
      if (list_returned) {
      	py_list = PyList_New(0);
      }

      for (cri = cmd_result.begin(), i=0; cri != cmd_result.end(); cri++,++i) {
        if (cri != NULL) {
	  int ret = 0; // python conversion routine error flag

          switch ((*cri)->Type()) {
          case CMD_RESULT_INT:
          {
            int64_t I = 0;

            ((CommandResult_Int *)(*cri))->Value(I);
            p_object = Py_BuildValue("l", I);

	    if (list_returned) {
            	ret = PyList_Append(py_list,p_object);
		if (ret != 0) {
            	    cmd->Result_String ("PyList_Append() failed for int");
            	    cmd->set_Status(CMD_ERROR);
		}
	    }
	    
            break;
          }
          case CMD_RESULT_STRING:
          case CMD_RESULT_RAWSTRING:
          {
            std::string C;

            ((CommandResult_String *)(*cri))->Value(C);
    	      p_object = Py_BuildValue("s",C.c_str());

	    if (list_returned) {
            	ret = PyList_Append(py_list,p_object);
		if (ret != 0) {
            	    cmd->Result_String ("PyList_Append() failed for string");
            	    cmd->set_Status(CMD_ERROR);
		}
	    }

            break;
          }
          default:
           break;
          }
        }
      }
     // The results have been copied for use within Python
     // so we are done working wih the command.
      cmd->set_Results_Used ();
    }

    if (p_object == NULL) {
      p_object = Py_BuildValue("");
    }

    if (list_returned)
    	return py_list;
    else
    	return p_object;
}

// Create a Clip for the command and then go parse and execute it.
static PyObject *SS_EmbeddedParser (PyObject *self, PyObject *args) {
  InputLineObject *clip = NULL;
  char *input_line = NULL;
    
  if (!PyArg_ParseTuple(args, "s", &input_line)) {
  	;
  }
  Current_ILO = new InputLineObject (Embedded_WindowID, std::string(input_line));
  Current_CO = NULL;

  return SS_CallParser (self, args);
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

 // Verify that the command is still valid.
  if ((clip->What() == ILO_ERROR)) {
    return NULL;
  }

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

// This will be used to determine how to react to
// errors found during the python preparse part
// of our CLI parsing. This is different from the
// Scripting API that is done in python.
//
// This is not finished yet!
//
static PyObject *SS_ParseError (PyObject *self, PyObject *args) {
    CommandObject *cmd = NULL;
    PyObject *p_object = NULL;
    ParseResult parse_result = ParseResult();

    // Build a CommandObject so that the semantic routines can be called.
    cmd = new CommandObject (&parse_result, false);

    cmd->Result_String ("Preparse syntax error");
    cmd->set_Status(CMD_ERROR);
    
    //cmd->Clip()->Print(stdout);

    // I should be reporting exactly what went wrong here.
    p_object = Py_BuildValue("");
    return p_object;

}
 
// Openss can be used as a utility from a Python routine.
// To do so, this initialization routine must be the first thing called
// within the Openss utility.
static PyObject *SS_InitEmbeddedInterface (PyObject *self, PyObject *args) {
 // Setup the Command Line tracking mechanisms
  Commander_Initialization ();

 // Load in pcli messages into message czar
  pcli_load_messages();

 // Define Built-In Views
   SS_Init_BuiltIn_Views ();

 // Define a default input window as an anchor for tracking commands
  pid_t my_pid = getpid();
  char HostName[MAXHOSTNAMELEN+1];
  Embedded_WindowID = Embedded_Window ("EmbeddedInterface", &HostName[0],my_pid,0,true);

 // Direct output back to Python.
  cmd_output_to_python = true;

  return Py_BuildValue("");
}

static PyMethodDef PY_Input_Methods[] = {

    {"CallParser",  SS_CallParser, METH_VARARGS,
     "Call the YACC'd parser."},

    {"EmbeddedParser",  SS_EmbeddedParser, METH_VARARGS,
     "Call the YACC'd parser for a scripting command."},

    {"SetAssign",  SS_Set_Assign, METH_VARARGS,
     "Set to 1 if the result is used in a python."},

    {"Save_ILO",  SS_DelayILO, METH_VARARGS,
     "Save the Current_ILO."},

    {"ReadILO",  SS_ReadILO, METH_VARARGS,
     "Use a saved ILO to get a command."},

    {"ReadLine",  SS_ReadLine, METH_VARARGS,
     "Read a SpeedShop command."},

    {"ParseError",  SS_ParseError, METH_VARARGS,
     "Python or Yacc parser error marking."},

    {"OSS_Init",  SS_InitEmbeddedInterface, METH_VARARGS,
     "Initialize openss for use as an embedded utility."},

    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initPY_Input (void) {
  (void) Py_InitModule("PY_Input", PY_Input_Methods);
}
