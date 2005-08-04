#include <stdio.h>

#include <vector>
#include <iostream>

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;
/*
This tests to make sure ParseResult::getAddressList() and pushAddressPoint(int num) works correctly.
*/

int main()
{
	ParseResult p_parse_result;
	char * name = "hello";
	p_parse_result.pushModifiers(name);
}
