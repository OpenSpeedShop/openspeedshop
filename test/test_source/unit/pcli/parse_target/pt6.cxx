#include <stdio.h>

#include <vector>
#include <iostream>

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;
/*
This test checks for the correctness of PushFileRange().
*/
int main()
{
	ParseTarget p_parse_target;
	char * word1 = "hello";
	int number = 8;
	int testnumber;
	string testword2;
	vector<ParseRange> * pr_vector;
	p_parse_target.pushFileRange(number, word1);
	pr_vector = p_parse_target.getFileList();
	ParseRange testRange = pr_vector->front();
	testnumber = testRange.getRange()->start_range.num;
	testword2 = testRange.getRange()->end_range.name;
	if (testword2== "hello" && testnumber == 8)
		cout << "PASS" << endl;
	else
		cout << "FAIL" << endl;
	return 0;
}
