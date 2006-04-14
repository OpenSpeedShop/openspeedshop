#include <iostream>
#include <list>
#include "inttypes.h"
#include "../../../../../libopenss-framework/AddressRange.hxx"

using namespace std;
using namespace OpenSpeedShop;
using namespace Framework;


int main(){
	AddressRange testRange;	
	if (testRange.isEmpty()){
		cout << "PASS" << endl;
	}
	else
	{
		cout << "FAIL" << endl;
	}
 return 0;
}
