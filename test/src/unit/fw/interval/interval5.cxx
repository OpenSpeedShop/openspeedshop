#include <iostream>
#include <list>
#include "inttypes.h"
#include "../../../../../libopenss-framework/AddressRange.hxx"

using namespace std;
using namespace OpenSpeedShop;
using namespace Framework;

/* This is a negative test for Interval::doesContain(interval &).  It verifies
that doesContain returns false for an interval that is not within the range.
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
	AddressRange testwxRange(testwAddress, testxAddress);
	AddressRange testyzRange(testyAddress, testzAddress);
	if (testwxRange.doesContain(testyzRange)){
		cout << "FAIL" << endl;
	}
	else
	{
		cout << "PASS" << endl;
	}
	
 return 0;
}
