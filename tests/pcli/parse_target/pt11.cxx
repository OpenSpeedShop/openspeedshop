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
	char * word1 = "hello";
	char * word2 = "goodbye";
	string testword1;
	string testword2;
	vector<ParseRange> * pr_vector;
	p_parse_target.pushClusterRange(word1, word2);
	pr_vector = p_parse_target.getClusterList();
	ParseRange testRange = pr_vector->front();
	testword1 = testRange.getRange()->start_range.name;
	testword2 = testRange.getRange()->end_range.name;
	if (testword2== "goodbye" && testword1 == "hello")
		cout << "PASS" << endl;
	else
		cout << "FAIL" << endl;
	return 0;
}
