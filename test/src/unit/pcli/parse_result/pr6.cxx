#include <stdio.h>

#include <vector>
#include <iostream>

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;
/*
This is a negative test to see if the function ParseResult.isExpId() works propertly.
*/

int main()
{
	ParseResult p_parse_result;
	if (p_parse_result.isExpId())
		cout << "FAIL" << endl;
	else
		cout << "PASS" << endl;
	return 0;
}
