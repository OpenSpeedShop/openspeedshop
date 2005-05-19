#include <stdio.h>

#include <vector>
#include <iostream>

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;
/*
This is a positive test to verify that ParseResult.SetExpId and ParseResult.GetExpId work correctly.


*/
int main()
{
	ParseResult p_parse_result;
	p_parse_result.SetExpId(5);
	if (p_parse_result.GetExpId() == 5)
		cout << "PASS" << endl;
	else
		cout << "FAIL" << endl;
	return 0;
}
