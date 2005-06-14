#include <iostream>
#include "inttypes.h"
#include "Blob.hxx"

using namespace std;
using namespace OpenSpeedShop;
using namespace Framework;

int main(){
	Blob test_blob;
        if (test_blob.isEmpty()){
                cout << "PASS" << endl;
        }
        else
        {
                cout << "FAIL" << endl;
        }

 return 0;
}
