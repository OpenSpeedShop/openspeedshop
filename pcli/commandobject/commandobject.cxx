#include "cmdobj.h"
#include <iostream>

class MyCommandObject: public CommandObject<void>
{
public:
    MyCommandObject() : CommandObject<void>()
    {
    }
    
    
    virtual void dataIsReady()
    {
        std::cout << "MyCmmandObject::dataIsReady" << std::endl;
    }
};


main()
{
    MyCommandObject o;
    
    o.waitUntilDataReady();
    
    sleep( 10 );
    o.setReadyFlag();
    
    
    sleep( 5 );

}

