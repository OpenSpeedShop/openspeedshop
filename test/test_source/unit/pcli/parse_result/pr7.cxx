#include <stdio.h>

#include <vector>
#include <iostream>

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;
/*
This tests to make sure ParseResult::getAddressList() and pushAddressPoint(char *) works correctly.
*/

int main()
{
	ParseResult p_parse_result;
	char * tstname = "blue";
	p_parse_result.pushAddressPoint(tstname);
	vector<ParseRange> * AddList = p_parse_result.getAddressList();
 	ParseRange pr = AddList->front();	
	parse_range_t * prt = pr.getRange();	
	if (prt->start_range.name == "blue")
		cout << "PASS" << endl;
	else
		cout << "FAIL" << endl;
	return 0;
}
