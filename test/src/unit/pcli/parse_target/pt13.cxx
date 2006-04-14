#include <stdio.h>

#include <vector>
#include <iostream>

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;
/*
This test checks for the correctness of PushClusterRange().
*/
int main()
{
	ParseTarget p_parse_target;
	int number = 8;
	int number2 = 23;
	int testnumber;
	int testnumber2;
	vector<ParseRange> * pr_vector;
	p_parse_target.pushClusterRange(number, number2);
	pr_vector = p_parse_target.getClusterList();
	ParseRange testRange = pr_vector->front();
	testnumber = testRange.getRange()->start_range.num;
	testnumber2 = testRange.getRange()->end_range.num;
	if (testnumber2== 23 && testnumber == 8)
		cout << "PASS" << endl;
	else
		cout << "FAIL" << endl;
	return 0;
}
