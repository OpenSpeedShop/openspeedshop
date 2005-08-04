#include <stdio.h>

#include <vector>
#include <iostream>

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;
/*
This is a test to see if the function ParseResult.isExpId() works propertly.
*/

int main()
{
	ParseResult p_parse_result;
	p_parse_result.setExpId(5);
	if (p_parse_result.isExpId())
		cout << "PASS" << endl;
	else
		cout << "FAIL" << endl;
	return 0;
}
