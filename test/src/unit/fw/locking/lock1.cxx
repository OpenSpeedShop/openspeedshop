#include <iostream>
#include "Database.hxx"
#include "Collector.hxx"

using namespace std;
using namespace OpenSpeedShop::Framework;

class countableLock : private Lockable
{
   public:
	countableLock();
	void incrementCounter();
   private:
	int counter;
};

countableLock::countableLock(){
	counter = 0;
}
 
void countableLock::incrementCounter(){
	counter++;
}

int main(){
	countableLock cl;
 return 0;
}
