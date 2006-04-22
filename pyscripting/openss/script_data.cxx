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

struct pcsamp_data {
    double  inclusive;
    double  exclusive;
    double  total_percentage;
    char *  func_name;
};

#define SAMP_DATA_SZ 7
static
struct pcsamp_data data_1[SAMP_DATA_SZ] = {
    17.0571, 0.0000, 100.0000,	"__libc_start_main(libc.so.6.1)",
    17.0571, 0.0000, 100.0000,	"_start(fred)",
    17.0571, 0.0000, 100.0000,	"work(fred: work.c,2)",
    17.0571, 0.0000, 100.0000,	"main(fred: fred.c,5)",
     7.4571, 7.4571,  43.7186,	"f3(fred: f3.c,5)",
     6.1714, 6.1714,  36.1809,	"f2(fred: f2.c,5)",
     3.4286, 3.4286,  20.1005,	"f1(fred: f1.c,5)"
};

#define GEN_STRLIST 6
static
char * generic_str_list[] = {
    "this","is","a","list","of","strings"
};

#define GEN_INTLIST 5
static
int64_t generic_int_list[] = {
    4,5,6,7,8
};

static
int64_t generic_int = 98;

static 
char *generic_str = "this is a string";

struct exp_state {
    int64_t exp_id;
    char *  filename;
};

static 
int64_t cur_exp_id = -1;

static 
struct exp_state *exp_state_tbl = NULL;
    
/**
 * Function: xxx
 * 
 * .
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */

/**
 * Function: build_intlist
 * 
 * .
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
PyObject *
build_intlist(ParseResult *p_parse_result)
{
    PyObject *py_list = PyList_New(0);
    int i;

    for (i=0;i<GEN_INTLIST;++i) {
    	int64_t t_int = generic_int_list[i];
	int ret;

    	PyObject *p_object = Py_BuildValue("l", t_int);
    	ret = PyList_Append(py_list,p_object);
    	if (ret != 0) {
    	    printf ("PyList_Append() failed for long integer\n");
	    exit(1);
    	}
    }
    
    return py_list;
}

/**
 * Function: build_strlist
 * 
 * .
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
PyObject *
build_strlist(ParseResult *p_parse_result)
{
    PyObject *py_list = PyList_New(0);
    int i;

    for (i=0;i<GEN_STRLIST;++i) {
    	char *t_str = generic_str_list[i];
	int ret;

    	PyObject *p_object = Py_BuildValue("s", t_str);
    	ret = PyList_Append(py_list,p_object);
    	if (ret != 0) {
    	    printf ("PyList_Append() failed for string\n");
	    exit(1);
    	}
    }
    
    return py_list;
}

/**
 * Function: build_view
 * 
 * .
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
PyObject *
build_view(ParseResult *p_parse_result)
{
    PyObject *py_list_main = PyList_New(0);
    int i;

    for (i=0;i<SAMP_DATA_SZ;++i) {
    	struct pcsamp_data *p_data = &data_1[i];
	PyObject *py_list_sub = PyList_New(0);
	PyObject *p_object = NULL;
	int ret;

    	// double  inclusive;
    	p_object = Py_BuildValue("d", p_data->inclusive);
    	ret = PyList_Append(py_list_sub,p_object);
    	if (ret != 0) {
    	    printf ("PyList_Append() failed for double\n");
	    exit(1);
    	}

    	// double  exclusive;
    	p_object = Py_BuildValue("d", p_data->exclusive);
    	ret = PyList_Append(py_list_sub,p_object);
    	if (ret != 0) {
    	    printf ("PyList_Append() failed for double\n");
	    exit(1);
    	}

    	// double  total_percentage;
    	p_object = Py_BuildValue("d", p_data->total_percentage);
    	ret = PyList_Append(py_list_sub,p_object);
    	if (ret != 0) {
    	    printf ("PyList_Append() failed for double\n");
	    exit(1);
    	}

    	// char *  func_name;
    	p_object = Py_BuildValue("s", p_data->func_name);
    	ret = PyList_Append(py_list_sub,p_object);
    	if (ret != 0) {
    	    printf ("PyList_Append() failed for char *\n");
	    exit(1);
    	}
	
	PyList_Append(py_list_main,py_list_sub);

    }
    
    return py_list_main;
}

/**
 * Function: xxx
 * 
 * .
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
PyObject *
build_return(ParseResult *p_parse_result)
{
    PyObject *p_object = NULL;

    switch(p_parse_result->getCommandType()){
    	case CMD_EXP_CREATE:
            p_object = Py_BuildValue("l", ++cur_exp_id);
	    break;
    	case CMD_EXP_VIEW:
    	    p_object = build_view(p_parse_result);
	    break;

    	case CMD_HISTORY:
    	case CMD_HELP:
    	    p_object = Py_BuildValue("s",generic_str);
	    break;

    	case CMD_EXP_RESTORE:
    	case CMD_EXP_FOCUS:
    	case CMD_SETBREAK:
    	    p_object = Py_BuildValue("l",generic_int);
	    break;

    	default :
    	    p_object = Py_BuildValue("");
	    break;
    }
    
    return p_object;
}


















