#ifndef __OPENSPEEDSHOP_CLI_OUTPUTOBJ_HXX
#define  __OPENSPEEDSHOP_CLI_OUTPUTOBJ_HXX

namespace OpenSpeedShop { namespace cli {
// ForwardIteratorOutputObject [GENERIC]
// One possible output object that takes two forward iterators
// as parameter
// Can be used that way:
//  ForwardIteratorOutputObject<std::vector<int> > sendvec;
//  std::vector<int> v;
//  sendvec(v.begin(), v.end())
// In tts current definition, it doesn't do anything, classes should inherit 
// from it.
template<typename ForwardIterator>
struct ForwardIteratorOutputObject
{
public:
    virtual void operator()( ForwardIterator const& f, ForwardIterator const& l ) const {}
};


// PrintTheData
// One possible output object that takes two forward iterators
// as parameter. Inherites from ForwardIteratorOutputObject.
// Can be used that way:
//  PrintTheData<std::vector<int> > sendvec;
//  std::vector<int> v;
//  sendvec(v.begin(), v.end())
// This one currently prints any data it receives
template <typename ForwardIterator>
struct PrintTheData : 
        public ForwardIteratorOutputObject< ForwardIterator >
{
    virtual void operator()( ForwardIterator const& first,
                             ForwardIterator const& last ) const
    {
        for( ForwardIterator it = first; it!= last; it++ )
        {
            std::cout << "Generic: " << *it << std::endl;
        }
    }
};


// SingleDataOutputObject [GENERIC]
// One possible output object that takes only one parameter
// Can be used that way:
//  SingleDataOutputObject<int> sendint;
//  sendint(3);
template <typename T>
struct SingleDataOutputObject
{
    virtual void operator()( T const& t ) const
    {
std::cout << "Generic SingleDataOutputObject::operator()" << std::endl;
    }
};
}}
#endif
