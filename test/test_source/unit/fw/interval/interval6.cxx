#include <iostream>
#include <list>
#include "inttypes.h"
#include "../../../../../libopenss-framework/AddressRange.hxx"

using namespace std;
using namespace OpenSpeedShop;
using namespace Framework;

/* This is a test for Interval::doesIntersect(interval &).  It verifies
that doesIntersect returns true for an interval that intersects another interval.
*/

int main(){
	long int w = 4;
	long int x = 5;
	long int y = 6;
	long int z = 7;
	uint64_t w_address = uint64_t(&w);
	uint64_t x_address = uint64_t(&x);
	uint64_t y_address = uint64_t(&y);
	uint64_t z_address = uint64_t(&z);
	Address testwAddress(w_address);
	Address testxAddress(x_address);
	Address testyAddress(y_address);
	Address testzAddress(z_address);
	AddressRange testwyRange(testwAddress, testyAddress);
	AddressRange testxzRange(testxAddress, testzAddress);
	if (testwyRange.doesIntersect(testxzRange)){
		cout << "PASS" << endl;
	}
	else
	{
		cout << "FAIL" << endl;
	}
	
 return 0;
}
