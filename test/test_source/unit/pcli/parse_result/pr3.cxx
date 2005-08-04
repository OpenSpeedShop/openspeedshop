#include <stdio.h>

#include <vector>
#include <iostream>

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;
/*
This is a negative test for ParseResult.setCommandType().
*/

int main()
{
	ParseResult p_parse_result;
	p_parse_result.setCommandType(CMD_EXP_CREATE);
	if (p_parse_result.getCommandType() == CMD_EXP_DETACH)
		cout << "FAIL" << endl;
	else
		cout << "PASS" << endl;
	return 0;
}
