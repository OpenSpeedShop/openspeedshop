/** @file
 *
 * Storage for the parse results of a single OpenSpeedShop command.
 *
 */

#include <vector>
#include <iostream>

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;

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
ParseRange(int num)
{
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
ParseRange(char * name)
{
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
ParseRange(char * name, int num)
{
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
ParseRange(char * name1, char * name2)
{
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
ParseRange(int num, char * name)
{
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
ParseRange(int num1, int num2)
{
    dm_range.start_range.tag = VAL_NUMBER;
    dm_range.start_range.num = num1;
	    
    dm_range.end_range.tag = VAL_NUMBER;
    dm_range.end_range.num = num2;
	    
    dm_range.is_range = true;
}
 
