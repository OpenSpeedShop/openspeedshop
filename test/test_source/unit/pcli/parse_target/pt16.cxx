#include <stdio.h>

#include <vector>
#include <iostream>

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;
/*
This test checks for the correctness of PushRankRange().
*/
int main()
{
	ParseTarget p_parse_target;
	char * word = "hello";
	int  number= 8;
	int testnumber;
	string testword;
	vector<ParseRange> * pr_vector;
	p_parse_target.pushRankRange(word, number);
	pr_vector = p_parse_target.getRankList();
	ParseRange testRange = pr_vector->front();
	testnumber = testRange.getRange()->end_range.num;
	testword = testRange.getRange()->start_range.name;
	if (testnumber == 8 && testword == "hello")
		cout << "PASS" << endl;
	else
		cout << "FAIL" << endl;
	return 0;
}
