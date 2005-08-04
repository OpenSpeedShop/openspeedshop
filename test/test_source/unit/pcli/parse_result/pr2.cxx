#include <stdio.h>

#include <vector>
#include <iostream>

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;
/*
This is a positive test to verify that ParseResult.setExpId and ParseResult.getExpId work correctly.


*/
int main()
{
	ParseResult p_parse_result;
	p_parse_result.setExpId(5);
	if (p_parse_result.getExpId() == 5)
		cout << "PASS" << endl;
	else
		cout << "FAIL" << endl;
	return 0;
}
