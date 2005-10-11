#ifndef __OPENSPEEDSHOP_CLI_COMMANDOBJECT
#define __OPENSPEEDSHOP_CLI_COMMANDOBJECT

#include <pthread.h>
#include <unistd.h>
#include <iostream>

namespace OpenSpeedShop { namespace cli {
// Just a simple template to setup a lock when accessing a protected data
// There are probably a bunch of templates like this out there
// Could boost::share_ptr<> be the one?
template <typename T>
class protected_data
{
    pthread_mutex_t _mutex;
    T t;
    
    void lock()
    {
        pthread_mutex_lock( &_mutex );
    }
    
    void unlock()
    {
        pthread_mutex_unlock( &_mutex );
    }

public:
    protected_data()
    {
        pthread_mutex_init( &_mutex, NULL ); 
    }
    
    virtual ~protected_data()
    {
        unlock();
        pthread_mutex_destroy( &_mutex );
    }
    
    void set( T v )
    {
        lock();
        t = v;
        unlock();
    }
        
    T get()
    {
        T r;
        lock();
        r = t;
        unlock();
        return r;
    }

    operator T() const 
    {
        return t;
    }
};


// The CommandObject
// Takes a template parameter OutputObject so that we can define whatever
// type we want. In fact, all we want is a callback function. By using a 
// template we can define any kind of function by using function object whose
// type will be known only at the instanciation of the template
// The CommandObject is defined here to be inherited from.
//
template <class OutputObject>
class CommandObject
{

    typedef OutputObject OutputObj;
    protected_data<bool> dataReady;

    OutputObj _outputObj;

public:
    CommandObject()
    {
        dataReady.set(false);
    }
    
    void setReadyFlag()
    {
        dataReady.set(true);
    }
    
    // Can be used by any listner to get informed when data is ready
    void waitUntilDataReady()
    {
        runConcurrent( static_cast<void*>(this) );
    }

    // dataIsReady is called when the server has data ready to be sent
    // or already sent to the client.
    virtual void dataIsReady() const
    {
        // This one could be pure
        std::cout << "GENERIC: Data is Ready Here!" << std::endl;
    }


    // outputObj is called by the server when it needs to send data to 
    // its client. Thansk to the template definition, I can send whatever 
    // data using as many parameters as OutputObj can.
    virtual const OutputObject* outputObj() const
    {
        return &_outputObj;
    }
    
private:
    pthread_t thr;
    void runConcurrent( void* data )
    {
        pthread_create( &thr, NULL, __waitRoutine, data );
    }

    static void* __waitRoutine( void* data )
    {
        CommandObject* obj = static_cast< CommandObject* >(data);
        obj->waitForData();
    }

    void waitForData()
    {
        while( dataReady.get() == false )
        {
            std::cout << "waitForData: ready = " << dataReady.get() << std::endl;
            usleep( 500000 ); // 500 msec
        }
        dataIsReady();
    }
    
    
};

class _CommandObject
{
    const void* _cmdobj;
public:
    _CommandObject() : _cmdobj(NULL) {}
    _CommandObject( const void* p) : _cmdobj(p) {}
    _CommandObject( _CommandObject const& ref) : _cmdobj( ref._cmdobj ) {}
    _CommandObject& operator=(const _CommandObject& ref) { _cmdobj = ref._cmdobj; }

    const void* ptr() const {return _cmdobj;}
    void describe() { std::cout << "CommandObjectCloak: " << _cmdobj << std::endl; }
};


}}
#endif
