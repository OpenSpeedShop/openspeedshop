#include <iostream>
#include "Database.hxx"

using namespace std;
using namespace OpenSpeedShop;
using namespace Framework;

int main(){
	bool in_catch = false;
	bool right_name = false;
	string db_name;
	try{
		string name = "test_db";
		Database::create(name);
		Database db(name);
		db_name = db.getName();
	}
	catch (runtime_error re){
		in_catch = true;
	}
	if (db_name == "test_db"){
		right_name = true;
	}
	if ((!in_catch) && (right_name)){
		cout << "PASS" << endl;
	}
	else{
		cout << "FAIL" << endl;
	}
 return 0;
}
