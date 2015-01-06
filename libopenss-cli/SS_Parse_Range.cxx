/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2015 Krell Institute  All Rights Reserved.
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

/** @file
 *
 * Storage for the parse results of a single OpenSpeedShop command.
 *
 */


// This must be first include due to warnings regarding redefinition
// of certain definitions (POSIX_*)
#include "SS_Input_Manager.hxx"

#include <vector>
#include <iostream>

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;
/**
 * Method: ParseRange::ParseRange(int64_t)
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
ParseRange::
ParseRange(int64_t num)
{
    dm_parse_type = PARSE_RANGE_VALUE;
    next = NULL;

    dm_range.start_range.tag = VAL_NUMBER;
    dm_range.start_range.num = num;

    dm_range.is_range = false;
}
 
/**
 * Method: ParseRange::ParseRange()
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */

ParseRange::
ParseRange(const char * name)
{
    dm_parse_type = PARSE_RANGE_VALUE;
    next = NULL;

    dm_range.start_range.tag = VAL_STRING;
    dm_range.start_range.name = name;
	    
    dm_range.is_range = false;
}
 
/**
 * Method: ParseRange::ParseRange()
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
ParseRange::
ParseRange(const char * name, int64_t num)
{
    dm_parse_type = PARSE_RANGE_VALUE;
    next = NULL;

    dm_range.start_range.tag = VAL_STRING;
    dm_range.start_range.name = name;
	    
    dm_range.end_range.tag = VAL_NUMBER;
    dm_range.end_range.num = num;
	    
    dm_range.is_range = true;
}
 
/**
 * Method: ParseRange::ParseRange()
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
ParseRange::
ParseRange(const char * name, double dval)
{
    dm_parse_type = PARSE_RANGE_VALUE;
    next = NULL;

    dm_range.start_range.tag = VAL_STRING;
    dm_range.start_range.name = name;
	    
    dm_range.end_range.tag = VAL_DOUBLE;
    dm_range.end_range.val = dval;
	    
    dm_range.is_range = true;
}
 
/**
 * Method: ParseRange::ParseRange()
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
ParseRange::
ParseRange(const char * name1, const char * name2)
{
    dm_parse_type = PARSE_RANGE_VALUE;
    next = NULL;

    dm_range.start_range.tag = VAL_STRING;
    dm_range.start_range.name = name1;
	    
    dm_range.end_range.tag = VAL_STRING;
    dm_range.end_range.name = name2;
	    
    dm_range.is_range = true;
}
 
/**
 * Method: ParseRange::ParseRange()
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
ParseRange::
ParseRange(int64_t num, const char * name)
{
    dm_parse_type = PARSE_RANGE_VALUE;
    next = NULL;

    dm_range.start_range.tag = VAL_NUMBER;
    dm_range.start_range.num = num;
	    
    dm_range.end_range.tag = VAL_STRING;
    dm_range.end_range.name = name;
	    
    dm_range.is_range = true;
}
 
/**
 * Method: ParseRange::ParseRange()
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
ParseRange::
ParseRange(int64_t num1, int64_t num2)
{
    dm_parse_type = PARSE_RANGE_VALUE;
    next = NULL;

    dm_range.start_range.tag = VAL_NUMBER;
    dm_range.start_range.num = num1;
	    
    dm_range.end_range.tag = VAL_NUMBER;
    dm_range.end_range.num = num2;
	    
    dm_range.is_range = true;
}

static void Dump_parse_val (parse_val_t v)
{
  if (v.tag == VAL_STRING) {
    printf("string: %s",v.name.c_str());
  } else if (v.tag == VAL_NUMBER) {
    printf("integer: %lld",v.num);
  } else if (v.tag == VAL_DOUBLE) {
    printf("double: %20.10f",v.val);
  } else {
    printf("?");
  }
}


void ParseRange::Dump()
{
    printf("ParseRange %p, type",this);
    if (dm_parse_type == PARSE_RANGE_VALUE) {
      printf(" PARSE_RANGE_VALUE ");
      if (dm_range.is_range) {
        Dump_parse_val(dm_range.start_range);
        printf (" through " );
        Dump_parse_val(dm_range.end_range);
      } else {
        Dump_parse_val(dm_range.start_range);
      }
    } else if (dm_parse_type == PARSE_EXPRESSION_VALUE) {
      printf(" PARSE_EXPRESSION_VALUE %s(",ExprOperator(dm_exp.exp_op).c_str());
      std::vector<ParseRange *> *exp = &(this->dm_exp.exp_operands);
      std::vector<ParseRange *>::iterator expi;
      for (expi = exp->begin(); expi != exp->end(); expi++) {
        ParseRange *pr = (*expi);
        printf("\toperand: %p\n",pr);
        pr->Dump();
      }
      printf(" ) ");
    } else {
      printf(" -- unknown --");
    }
    printf("\n");
}
 
/**
 * Method: ParseRange::ParseRange(expression_operation_t, char *)
 * 
 *   Build the operation expressions for parse opjects.
 *
 */
ParseRange::
ParseRange(expression_operation_t op, char *c)
{
    next = NULL;
    dm_exp.exp_op = op;
    dm_parse_type = PARSE_RANGE_VALUE;
    next = NULL;

    dm_range.start_range.tag = VAL_STRING;
    dm_range.start_range.name = std::string(c);

    dm_range.is_range = false;

#if DEBUG_CLI
    //std::vector<ParseRange *> *pv = &(this->dm_exp.exp_operands);
    //pv->push_back(operand0);
    //dm_exp.exp_operand.push_back(operand0);
    //dm_exp.exp_operand[0] = operand0;
    //printf("\tExit ParseRangeExp <char *>: %p ",pr);pr->Dump();
    printf("\tExit ParseRangeExp for char star\n"); 
#endif
}
 
/**
 * Method: ParseRange::ParseRange(expression_operation_t, int64_t)
 * 
 *   Build the operation expressions for parse opjects.
 *
 */
ParseRange::
ParseRange(expression_operation_t op, int64_t ival)
{
    next = NULL;
    dm_exp.exp_op = op;
    dm_parse_type = PARSE_RANGE_VALUE;
    next = NULL;

    dm_range.start_range.tag = VAL_NUMBER;
    dm_range.start_range.num = ival;

    dm_range.is_range = false;

#if DEBUG_CLI
    //std::vector<ParseRange *> *pv = &(this->dm_exp.exp_operands);
    //pv->push_back(operand0);
    //dm_exp.exp_operand.push_back(operand0);
    //dm_exp.exp_operand[0] = operand0;
    //printf("\tExit ParseRange <int64_t>: %p ",this);this->Dump();
    printf("\tExit ParseRangeExp for int64\n"); 
#endif
}
 
/**
 * Method: ParseRange::ParseRange(expression_operation_t, double)
 * 
 *   Build the operation expressions for parse opjects.
 *
 */
ParseRange::
ParseRange(expression_operation_t op, double dval)
{
    next = NULL;
    dm_exp.exp_op = op;
    dm_parse_type = PARSE_RANGE_VALUE;
    next = NULL;

    dm_range.start_range.tag = VAL_DOUBLE;
    dm_range.start_range.val = dval;

    dm_range.is_range = false;

#if DEBUG_CLI
    //std::vector<ParseRange *> *pv = &(this->dm_exp.exp_operands);
    //pv->push_back(operand0);
    //dm_exp.exp_operand.push_back(operand0);
    //dm_exp.exp_operand[0] = operand0;
    //printf("\tExit ParseRange <double>: %p ",this);this->Dump();
    printf("\tExit ParseRangeExp for double\n"); 
#endif
}
 
/**
 * Method: ParseRange::ParseRange(expression_operation_t, ...)
 * 
 *   Build the operation expressions for parse opjects.
 *   Support expressions with up to 3 operands.
 *
 */
ParseRange::
ParseRange(expression_operation_t op, ParseRange *operand0)
{
    dm_parse_type = PARSE_EXPRESSION_VALUE;
    next = NULL;
    dm_exp.exp_op = op;
    std::vector<ParseRange *> *pv = &(this->dm_exp.exp_operands);
    pv->push_back(operand0);
}
ParseRange::
ParseRange(expression_operation_t op, ParseRange *operand0, ParseRange *operand1)
{
    dm_parse_type = PARSE_EXPRESSION_VALUE;
    next = NULL;
    dm_exp.exp_op = op;
    std::vector<ParseRange *> *pv = &(this->dm_exp.exp_operands);
    pv->push_back(operand0);
    pv->push_back(operand1);
}
ParseRange::
ParseRange(expression_operation_t op, ParseRange *operand0, ParseRange *operand1, ParseRange *operand2)
{
    dm_parse_type = PARSE_EXPRESSION_VALUE;
    next = NULL;
    dm_exp.exp_op = op;
    std::vector<ParseRange *> *pv = &(this->dm_exp.exp_operands);
    pv->push_back(operand0);
    pv->push_back(operand1);
    pv->push_back(operand2);
}
