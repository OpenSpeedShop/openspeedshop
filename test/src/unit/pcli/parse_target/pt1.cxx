#include <stdio.h>

#include <vector>
#include <iostream>

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;
/*
This test checks for the correctness of bool used().
*/
int main()
{
	ParseTarget p_parse_target;
	char * word = "hello";
	p_parse_target.pushFilePoint(word);
	if (p_parse_target.used())
		cout << "PASS" << endl;
	else
		cout << "FAIL" << endl;
	return 0;
}
