#include <stdio.h>

#include <vector>
#include <iostream>
#include "Python.h"

#include "SS_Input_Manager.hxx"
#include "SS_Cmd_Execution.hxx"

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;

extern FILE *yyin;
extern int yyparse (void);
ParseResult *p_parse_result;
static CMDWID Embedded_WindowID = 0;


/**
 * Method: s_dumpRange()
 * 
 * Dump range and point lists
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
static void 
s_dumpRange(vector<ParseRange> *p_list, char *label, bool is_hex, bool newline)
{
    vector<ParseRange>::iterator iter;
    
    if (is_hex){
    	cout.setf(ios_base::hex,ios_base::basefield);
    	cout.setf(ios_base::showbase);
    }
    else {
    	cout.setf(ios_base::dec,ios_base::basefield);
    	cout.unsetf(ios_base::showbase);
    }
    
    if (p_list->begin() != p_list->end())
    	    cout << "\t" << label << ": " ;
    if (newline)
    	cout << endl;

    for (iter=p_list->begin();iter != p_list->end(); iter++) {
    	
	if (newline)
	    cout << "\t\t";
    	parse_range_t *p_range = iter->getRange();
    	if (p_range->is_range) {
    	    parse_val_t *p_val1 = &p_range->start_range;
    	    parse_val_t *p_val2 = &p_range->end_range;
    	    if (p_val1->tag == VAL_STRING) {
    	    	cout << p_val1->name << ":";
    	    }
    	    else {
    	    	cout << p_val1->num << ":";
    	    }
    	    if (p_val2->tag == VAL_STRING) {
    	    	cout << p_val2->name << " ";
    	    }
    	    else {
    	    	cout << p_val2->num << " ";
    	    }
    	}
    	else {
    	    parse_val_t *p_val1 = &p_range->start_range;
    	    if (p_val1->tag == VAL_STRING) {
    	    	cout << p_val1->name << " ";
    	    }
    	    else {
    	    	cout << p_val1->num << " ";
    	    }
    	}
	
	if (newline)
    	    cout << endl ;

    }
    if (p_list->begin() != p_list->end())
    	    cout << endl ;

}

/**
 * Function: parse_line
 * 
 * .
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
static void
parse_line(char *input_line)
{
    int ret;

    cout << input_line << endl;

    yyin = fopen("/usr/tmp/jack.tmp","w+");
    fprintf(yyin,"%s\n", input_line);
    rewind(yyin);

    ret = yyparse();

#if 1
    	p_parse_result->dumpInfo();
#else
    // testing code
    if (!p_parse_result->syntaxError())
    	p_parse_result->dumpInfo();
    // Syntax error.
    else {
    	char *cmd_name = p_parse_result->getCommandname();
	
    	s_dumpRange(p_parse_result->getErrorList(), 
	    	    cmd_name, 
		    false /* is_hex */,
		    true /* newline */);
    }
#endif

    fclose(yyin); 

    cout << endl << "****************************" << endl;
}

/**
 * Function: SS_CallParseTest
 * 
 * .
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
// Parse and execute an individual command
static PyObject *SS_CallParseTest (PyObject *self, PyObject *args) {
    int i;
    char *input_line = NULL;
    bool list_returned = false;
    PyObject *p_object = NULL;
    PyObject *py_list = NULL;

    // Give yacc access to ParseResult object.
    ParseResult parse_result = ParseResult();
    p_parse_result = &parse_result;

    
    if (!PyArg_ParseTuple(args, "s", &input_line)) {
    	;
    }
    
    cout << "SS_CallParseTest: \'" << input_line << "\'" << endl;

    parse_line(input_line);


    if (p_object == NULL) {
      p_object = Py_BuildValue("");
    }

    if (list_returned)
    	return py_list;
    else
    	return p_object;
}

/**
 * Function: jack_methods
 * 
 * .
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
static PyMethodDef 
jack_methods[] = {
    {"CallParseTest",SS_CallParseTest,METH_VARARGS,
    "Call the YACC'd parser."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

/**
 * Function: inittest_script
 * 
 * .
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
PyMODINIT_FUNC
initscript_test()
{
    Py_InitModule("script_test",jack_methods);
}
