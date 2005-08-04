#include <stdio.h>

#include <vector>
#include <iostream>

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;


int main()
{
	ParseResult p_parse_result;
	p_parse_result.setCommandType(CMD_EXP_ATTACH);
	if (p_parse_result.getCommandType() == CMD_EXP_ATTACH)
		cout << "PASS" << endl;
	else
		cout << "FAIL" << endl;
	return 0;
}
