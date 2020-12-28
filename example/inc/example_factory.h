#ifndef __EXAMPLE_FACTORY__H__
#define __EXAMPLE_FACTORY__H__

#include <memory>

#include "iexample.h"
 
class example_factory
{
public:
    enum interface_type
    {
        LOCAL,
        PROXY
    };

    static std::unique_ptr<iexample> create(interface_type _type);
};

#endif // __EXAMPLE_FACTORY__H__
