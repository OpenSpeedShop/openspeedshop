#include <iostream>
#include "Database.hxx"

using namespace std;
using namespace OpenSpeedShop;
using namespace Framework;

int main(){
	bool in_catch = false;
	try{
		string name = "test_db";
		Database::create(name);
		Database db(name);
		Database bad("no_db");
	}
	catch (runtime_error re){
		cout << "PASS" << endl;
		in_catch = true;
	}
	if (!in_catch){
		cout << "FAIL" << endl;
	}
 return 0;
}
