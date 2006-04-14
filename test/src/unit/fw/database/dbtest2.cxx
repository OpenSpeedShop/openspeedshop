#include <iostream>
#include "Database.hxx"

using namespace std;
using namespace OpenSpeedShop;
using namespace Framework;

int main(){
	bool failed = false;
	try{
		string name = "test_db";
		string othername = "no_db";
		bool access, notaccess;
		Database::create(name);
		access = Database::isAccessible(name);
		notaccess = Database::isAccessible(othername);
		if(( access) && !(notaccess)){
			cout << "PASS" << endl;
		}
		else{
			failed = true;
		}

	}
	catch (runtime_error re){
		failed = true;
	}
	if (failed){
		cout << "FAIL" << endl;
	}
 return 0;
}
