#include <iostream>
#include "Database.hxx"

using namespace std;
using namespace OpenSpeedShop;
using namespace Framework;

int main(){

	bool passed = false;
	try{
		string name = "test_db";
		Database::create(name);
		Database::create(name);
	}
	catch (runtime_error re){
		cout << "PASS" << endl;
		passed = true;
	}
	if (!passed){
		cout << "FAIL" << endl;
}
 return 0;
}
