/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007-2011 Krell Institute  All Rights Reserved.
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
// Enable this define for limited debugging
//#define DEBUG_CLI 1

#include "Python.h"
#include "SS_Input_Manager.hxx"

//#define DEBUG_SYNC 1

extern void pcli_load_messages(void);
extern void pcli_load_scripting_messages(void);

extern FILE *yyin;
extern int yyparse (void);
ParseResult *p_parse_result;

/* Global Data for tracking the current command line. */
InputLineObject *Current_ILO	= NULL;
CommandObject   *Current_CO 	= NULL;
CommandObject   *p_cmdobj   	= NULL;

// The following mechanism is defined to allow python programs
// to capture the output of OSS commands and avoid sending it
// to the input window for printing.
//
//   (Embedded_WindowID != 0) is the default action
//   cmd_output_to_python defines the action for individual commands

static bool cmd_output_to_python = false;

// Exception we raise for various reasons
static PyObject * OpenssError = NULL;

/**
 * Method: openss_error()
 * 
 * Set string and activate exception.
 *     
 * @param   str - String to associated with error.
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
void
openss_error(char *str)
{
    if (OpenssError) {
    	PyErr_SetString(OpenssError, str);
    }

    return;	/* To xx, which will return a failure indicator */
}

/**
 * Method: SS_Set_Assign()
 * 
 * .
 *     
 * @param   self
 * @param   args
 *
 * @return  PyObject *
 *
 * @todo    Error handling.
 *
 */
static PyObject *
SS_Set_Assign (PyObject *self, PyObject *args) {

  cmd_output_to_python = true;

  return Py_BuildValue("");
}

/**
 * Method: Convert_CommandResult_To_Python()
 * 
 * Determine if raw data or formatted strings will
 * be returned in the form of a python object.
 *     
 * @param   cmd - command object for this command
 * @param   cr	- command result
 *
 * @return  PyObject *
 *
 * @todo    Error handling.
 *
 */
static PyObject *

Convert_CommandResult_To_Python (CommandObject *cmd, CommandResult *cr) {
    PyObject *p_object = NULL;
    bool KeyWord = Look_For_KeyWord (cmd, "data");;
  
    switch (cmd->Type()) {

    	case CMD_EXP_VIEW:
    	case CMD_EXP_COMPARE:
    	case CMD_C_VIEW:
    	    if (!KeyWord) {
	    	// Convert to Python Objects as strings
    	    	return Py_BuildValue("s",cr->Form().c_str());
    	    }
	    // Fall through

    	case CMD_EXP_DATA:
	default:
    	    switch (cr->Type()) {

    	    	case CMD_RESULT_UINT:
    	    	case CMD_RESULT_INT:
    	    	case CMD_RESULT_FLOAT:
    	    	case CMD_RESULT_STRING:
    	    	case CMD_RESULT_RAWSTRING:
    	    	case CMD_RESULT_COLUMN_VALUES:
    	    	case CMD_RESULT_FUNCTION:
    	    	    return cr->pyValue();   // Convert to Python Objects

    	    	case CMD_RESULT_TITLE:	    // Ignore for Python
    	    	case CMD_RESULT_COLUMN_HEADER:
    	    	case CMD_RESULT_COLUMN_ENDER:
    	    	default:
    	    	    return NULL;
    	}
    }
}

/**
 * Method: Convert_Cmd_To__Python()
 * 
 * .
 *     
 * @param   cmd command object pointer
 *
 * @return  PyObject *
 *
 * @todo    Error handling.
 *
 */
static PyObject *
Convert_Cmd_To__Python (CommandObject *cmd) {

  PyObject *p_object = NULL;
  PyObject *py_list = NULL;
  std::list<CommandResult *> cmd_result = cmd->Result_List();
  std::list<CommandResult *>::iterator cri;

  bool list_returned = cmd_desc[cmd->Type()].ret_list;
  if (!list_returned && (cmd_result.size() > 1)) {
    std::string s("Too many results were generated for the command.");
    Mark_Cmd_With_Soft_Error(cmd,s);
  }

  // Start building python list object
   if (list_returned) {
  	py_list = PyList_New(0);
  }

  for (cri = cmd_result.begin(); cri != cmd_result.end(); cri++) {
    if (*cri != NULL) {
      int ret = 0; // python conversion routine error flag

      p_object = Convert_CommandResult_To_Python (cmd, *cri);
      if (p_object == NULL) {
        continue;
      }

      if (list_returned) {
        ret = PyList_Append(py_list,p_object);
        if (ret != 0) {
    	    std::string s("PyList_Append() failed for int.");
    	    Mark_Cmd_With_Soft_Error(cmd,s);
        }
      }
    }
  }

 // The results have been copied for use within Python
 // so we are done working with the command.
  cmd->set_Results_Used ();
  Cmd_Obj_Complete (cmd);
  return list_returned ? py_list : ((p_object == NULL) ? Py_BuildValue("") : p_object);
}

/**
 * Method: SS_CallParser()
 * 
 * Parse and execute an individual command.
 *     
 * @param   self
 * @param   args
 *
 * @return  PyObject *
 *
 * @todo    Error handling.
 *
 */
static PyObject *
SS_CallParser (PyObject *self, PyObject *args) {

    char *input_line = NULL;
    PyObject *p_object = NULL;
    int ret;
    CommandObject *cmd = NULL;
    ParseResult *parse_result = new ParseResult();
    int i;
    bool list_returned = false;
    PyObject *py_list = NULL;
 
#if DEBUG_CLI
    printf("SS_CallParser, entered, dumping PyObject args\n");
    PyObject_Print(args, stdout, NULL);
    printf("\n");
#endif

    // Copy the desired action and reset the default action
    bool python_needs_result = cmd_output_to_python;
    cmd_output_to_python = (Embedded_WindowID != 0);
    
    // Give yacc access to ParseResult object.
    p_parse_result = parse_result;
    
    if (!PyArg_ParseTuple(args, "s", &input_line)) {
    	;
    }
    
    yyin = tmpfile();
    
    fprintf(yyin,"%s\n", input_line);
    rewind(yyin);

#if DEBUG_CLI
    printf("SS_CallParser, before calling yyparse with yyin==input_line=%s\n", input_line);
#endif

    ret = yyparse();
 
#if DEBUG_CLI
    printf("SS_CallParser, after calling yyparse with yyin==input_line=%s, ret=%d\n", input_line, ret);
#endif

    fclose(yyin); 
    
    // testing code
#if DEBUG_CLI
     printf("SS_CallParser, START DUMPING PARSE_RESULT\n");
     parse_result->dumpInfo();
     printf("SS_CallParser, END.. DUMPING PARSE_RESULT, python_needs_result=%d\n", python_needs_result);
#endif

    // Build a CommandObject so that the semantic routines 
    // can be called.
    cmd = new CommandObject (parse_result, python_needs_result);

    // See if the parse went alright.
    if ((p_parse_result->syntaxError()) ||
        (p_parse_result->getCommandType() == CMD_HEAD_ERROR)) {

#if DEBUG_CLI
    printf("SS_CallParser, Parsing failed, p_parse_result->syntaxError()=%d\n", p_parse_result->syntaxError());
    printf("SS_CallParser, Parsing failed, p_parse_result->getCommandType()=%d\n", p_parse_result->getCommandType());
#endif

      	std::string s("Parsing failed.");
      	Mark_Cmd_With_Soft_Error(cmd,s);
	parse_result->dumpError(cmd);
        Cmd_Obj_Complete (cmd);

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

   // Convert the results to proper Python form.
    return Convert_Cmd_To__Python (cmd);
}

/**
 * Method: SS_DelayILO()
 * 
 * Save a pointer to the most recent Input_Line_Object.
 *
 * Since Python likes character strings better than anything,
 * Convert the pointer to a string so that Python can manipulate
 * it without knowing what it is.  The only use of this string
 * will be by the routine SS_ReadILO.
 *     
 * @param   self
 * @param   args
 *
 * @return  PyObject *
 *
 * @todo    Error handling.
 *
 */
static PyObject *
SS_DelayILO (PyObject *self, PyObject *args) {

  char *seqnum = (char *)PyMem_Malloc(32);

  snprintf( seqnum, 32, "%p", Current_ILO);
  return Py_BuildValue("s", seqnum);
}

/**
 * Method: Prepare_Input_Line()
 * 
 * Extract the input line from an Input_Line_Object.
 *
 * Save this line in a Python object.
 * While at it, set "current" pointers.
 *     
 * @param   clip InputLineObject pointer
 *
 * @return  PyObject *
 *
 * @todo    Error handling.
 *
 */
static PyObject *
Prepare_Input_Line (InputLineObject *clip) {

  int64_t buffer_size = clip->Command().length()+1;
  bool need_newline = false;

 // Verify that the command is still valid.
  if ((clip->What() == ILO_ERROR)) {
    return NULL;
  }

 // Python requires that every input line end with 
 // an EOL character.
  if (clip->Command().c_str()[buffer_size-2] != *("\n")) {
    buffer_size++;
    need_newline = true;
  }

 // Use Python's memory allocator so that it will 
 // free the space when done.
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

/**
 * Method: SS_ReadILO()
 * 
 * Convert a Python character string to an ILO pointer.
 * Return the associated line of data to Python.
 *     
 * @param   self
 * @param   args
 *
 * @return  PyObject *
 *
 * @todo    Error handling.
 *
 */
static PyObject *
SS_ReadILO (PyObject *self, PyObject *args) {

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

/**
 * Method: SS_ReadLine()
 * 
 * Get an Input_Line_Object from the input manager.
 * Return the associated line of data to Python.
 *     
 * @param   self
 * @param   args
 *
 * @return  PyObject *
 *
 * @todo    Error handling.
 *
 */
static PyObject *
SS_ReadLine (PyObject *self, PyObject *args) {

    int more = 0;
  
    if (!PyArg_ParseTuple(args, "i", &more))
        return NULL;

#ifdef DEBUG_SYNC
    printf("[TID=%ld], SS_ReadLine, before calling SpeedShop_ReadLine, more=%ld\n", pthread_self(), more);
#endif

    InputLineObject *clip = SpeedShop_ReadLine(more);

    if (clip == NULL) {
    	PyErr_SetString(PyExc_EOFError, "End Of File");
    	return NULL;
    }

  return Prepare_Input_Line(clip);
}

/**
 * Method: SS_ParseError()
 * 
 * This will be used to determine how to react to
 * errors found during the python preparse part
 * of our CLI parsing. This is different from the
 * Scripting API that is done in python.
 *
 * This is not finished yet!
 *     
 * @param   self
 * @param   args
 *
 * @return  PyObject *
 *
 * @todo    Error handling.
 *
 */
static PyObject *
SS_ParseError (PyObject *self, PyObject *args) {

    CommandObject *cmd = NULL;
    PyObject *p_object = NULL;
    ParseResult *parse_result = new ParseResult();

    // Build a CommandObject so that the semantic 
    // routines can be called.
    cmd = new CommandObject (parse_result, false);

    std::string s("Preparse syntax error.");
    Mark_Cmd_With_Soft_Error(cmd,s);
    Cmd_Obj_Complete (cmd);
    
    //cmd->Clip()->Print(stdout);

    // I should be reporting exactly what went wrong here.
    p_object = Py_BuildValue("");
    return p_object;

}

/**
 * Method: SS_Set_Terminating()
 * 
 * Set the Shut_Down global variable after Python
 * detects a "Quit" statement.
 *     
 * @param   self
 * @param   args
 *
 * @return  PyObject *
 *
 */
static PyObject *
SS_Set_Terminating (PyObject *self, PyObject *args) {
  Shut_Down = true;
  return Py_BuildValue("");
}

/**
 * Method: SS_Terminating()
 * 
 * Convert the boolean variable Shut_Down to an integer 1 or 0
 * and build a Python object for the result.
 *     
 * @param   self
 * @param   args
 *
 * @return  PyObject *
 *
 */
static PyObject *
SS_Terminating (PyObject *self, PyObject *args) {
  return Py_BuildValue("l", (Shut_Down ? 1 : 0) );
}

/**
 * Method: SS_Do_Scripting()
 * 
 * Convert the boolean variable OPENSS_ALLOW_PYTHON_COMMANDS to
 * an integer 1 or 0 and build a Python object for the result.
 *     
 * @param   self
 * @param   args
 *
 * @return  PyObject *
 *
 */
static PyObject *
SS_Do_Scripting (PyObject *self, PyObject *args) {
  return Py_BuildValue("l", (OPENSS_ALLOW_PYTHON_COMMANDS ? 1 : 0) );
}

// Definitions for basic initialization / termination sequences.
void Openss_Basic_Initialization ();
void Openss_Basic_Termination();

 
/**
 * Method: SS_InitEmbeddedInterface()
 * 
 * Openss can be used as a utility from a Python routine.
 * To do so, this initialization routine must be the first
 * thing called within the Openss utility.
 *     
 * @param   self
 * @param   args
 *
 * @return  PyObject *
 *
 * @todo    Error handling.
 *
 */
static PyObject *
SS_InitEmbeddedInterface (PyObject *self, PyObject *args) {

//printf("Entering SS_InitEmbeddedInterface()\n");

 // Initialize the basic CLI mechanisms.
  Openss_Basic_Initialization ();

 // Always allow Python commands to be intermixed with Openss commands.
  OPENSS_ALLOW_PYTHON_COMMANDS = true;

 // Define a default input window as an anchor 
 // for tracking commands
  pid_t my_pid = getpid();
  char HostName[MAXHOSTNAMELEN+1];
  Embedded_WindowID = Embedded_Window ("EmbeddedInterface", 
    	    	    	    	    	&HostName[0],
					my_pid,
					0,
					false);

 // Load in pcli messages into message czar
 // This must follow setting Embedded_WindowID
  pcli_load_scripting_messages();

 // Direct output back to Python.
  cmd_output_to_python = true;

//printf("L SS_InitEmbeddedInterface()\n");

  return Py_BuildValue("");
}

/**
 * Method: SS_ExitEmbeddedInterface()
 * 
 * .
 *     
 * @param   self
 * @param   args
 *
 * @return  PyObject *
 *
 * @todo    Error handling.
 *
 */
static PyObject *
SS_ExitEmbeddedInterface (PyObject *self, PyObject *args) {

//printf("Entering SS_ExitEmbeddedInterface()\n");

  if (Embedded_WindowID != 0) {
   // Close down the CLI.
    Openss_Basic_Termination();
    Embedded_WindowID = 0;
  }

//printf("L SS_ExitEmbeddedInterface()\n");
  
  return Py_BuildValue("");
}

/**
 * Method: SS_EmbeddedParser()
 * 
 * Create a Clip for the command and then go parse 
 * and execute it.
 *     
 * @param   self
 * @param   args
 *
 * @return  PyObject *
 *
 * @todo    Error handling.
 *
 */
static PyObject *
SS_EmbeddedParser (PyObject *self, PyObject *args) {

static CMDID Scripting_Sequence_Number = 0;

  char *input_line = NULL;
  Assert (Embedded_WindowID != 0);  // Did we forget to initialize??
  
  try {
    if (!PyArg_ParseTuple(args, "s", &input_line)) {
  	  ;
    }
    Current_ILO = new InputLineObject (Embedded_WindowID, std::string(input_line));
    Current_ILO->SetSeq (++Scripting_Sequence_Number);
    Current_ILO->SetStatus (ILO_IN_PARSER);;
    Current_CO = NULL;

    PyObject *result = SS_CallParser (self, args);

    delete Current_ILO;

    if (Shut_Down) {
      // We have seen an Exit command - close down the CLI.
      (void)SS_ExitEmbeddedInterface (self, args);
    }

    return result;
  }
  catch (std::bad_alloc) {
    Shut_Down = true;

   // I should be reporting exactly what went wrong here.
    PyObject *p_object = Py_BuildValue("");
    return p_object;
  }
}

static CMDID Scripting_Sequence_Number = 0;


// Define a basic class with a constructor and destructor
// to catch dlopens and dlclose on use of the Embedded interface.
// The constructor could initialize OpenSpeedShop, but Python
// already has a mechanism that will call an init routine
// when the dso is loaded.
// The use of the destructor is a safety check to be sure
// that the exit routine is called in cases where no 'exit'
// command is issued by the user.
class Embedded_Entry_Control {
 public :
  Embedded_Entry_Control () {}
  ~Embedded_Entry_Control () {
    SS_ExitEmbeddedInterface (NULL, NULL);
  }
};
static Embedded_Entry_Control EEC;


/**
 * Table of OpenSS routines accessable from
 * python.
 *
 */
static PyMethodDef 
PYopenss_Methods[] = {

    {"CallParser",  SS_CallParser, METH_VARARGS,
     "Call the YACC'd parser."},

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

    {"Set_Terminate_PY",  SS_Set_Terminating, METH_VARARGS,
     "Set the Shut_Down global variable to TRUE."},

    {"Terminate_PY",  SS_Terminating, METH_VARARGS,
     "Return the value of the Shut_Down global variable."},

    {"Do_Scripting",  SS_Do_Scripting, METH_VARARGS,
     "Return the value of the OPENSS_ALLOW_PYTHON_COMMANDS varible."},

    {"OSS_Init",  SS_InitEmbeddedInterface, METH_VARARGS,
     "Initialize openss for use as an embedded utility."},

    {"OSS_Fini",  SS_ExitEmbeddedInterface, METH_VARARGS,
     "Terminate openss for use as an embedded utility."},

    {"EmbeddedParser",  SS_EmbeddedParser, METH_VARARGS,
     "Call the YACC'd parser for a scripting command."},

    {NULL, NULL, 0, NULL}        /* Sentinel */
};


/**
 * Method: initPY_Input()
 * 
 * Python initialization for CLI behavior.
 *     
 * @param   void
 *
 * @return  PyMODINIT_FUNC
 *
 * @todo    Error handling.
 *
 */
PyMODINIT_FUNC
initPY_Input (void) {
  
    // Register python exported methods
    (void) Py_InitModule("PY_Input", PYopenss_Methods);
}



/**
 * Method: initPYopenss()
 * 
 * Python initialization for scripting API.
 *     
 * @param   void
 *
 * @return  PyMODINIT_FUNC
 *
 * @todo    Error handling.
 *
 */
PyMODINIT_FUNC
initPYopenss (void) {

    PyObject *m, *d;

#if DEBUG_CLI
    fprintf(stderr, "actualCLIStartMode=%d, SM_Unknown=%d, SM_Batch=%d, SM_Offline=%d\n", 
            actualCLIStartMode, SM_Unknown, SM_Batch, SM_Unknown);
#endif

    //
    // per dew 2/22/10 since offline was added we now need to reset SM_Batch as the type 
    // of mode of execution during intitialization
    //
    if (actualCLIStartMode == SM_Unknown) {
      actualCLIStartMode = SM_Batch;
//      actualCLIStartMode = SM_Offline;
   } 
  
    // Register python exported methods
    m =  Py_InitModule("PYopenss", PYopenss_Methods);
  
    // Initialize exception handler
    d = PyModule_GetDict(m);
    char oss_err[] = "openss.error";
    OpenssError = PyErr_NewException(oss_err, NULL, NULL);

    if (OpenssError != NULL)
    	PyDict_SetItemString(d, "error", OpenssError);

    // Initialize openss for use.
    (void) SS_InitEmbeddedInterface (NULL,NULL);
}



