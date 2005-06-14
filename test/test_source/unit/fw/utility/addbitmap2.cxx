#include <iostream>
#include "inttypes.h"
#include "AddressRange.hxx"
#include "AddressBitmap.hxx"

using namespace std;
using namespace OpenSpeedShop;
using namespace Framework;

int main(){
	bool passed = false;
	long int x = 5;
        long int y = 6;
        long int z = 7;
        uint64_t x_address = uint64_t(&x);
        uint64_t y_address = uint64_t(&y);
        uint64_t z_address = uint64_t(&z);
        Address testxAddress(x_address);
        Address testyAddress(y_address);
        Address testzAddress(z_address);
        AddressRange testRange(testxAddress, testzAddress);
	AddressBitmap abm(testRange);
	abm.setValue(y_address, true);
        if (abm.getValue(y_address)){
                cout << "PASS" << endl;
        }
        else
        {
                cout << "FAIL" << endl;
        }

 return 0;
}
