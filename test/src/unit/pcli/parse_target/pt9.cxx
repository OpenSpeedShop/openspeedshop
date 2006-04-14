#include <stdio.h>

#include <vector>
#include <iostream>

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;
/*
This test checks for the correctness of PushClusterPoint().
*/
int main()
{
	ParseTarget p_parse_target;
	int  number= 8;
	int testnumber;
	vector<ParseRange> * pr_vector;
	p_parse_target.pushClusterPoint(number);
	pr_vector = p_parse_target.getClusterList();
	ParseRange testRange = pr_vector->front();
	testnumber = testRange.getRange()->start_range.num;
	if (testnumber == 8)
		cout << "PASS" << endl;
	else
		cout << "FAIL" << endl;
	return 0;
}
