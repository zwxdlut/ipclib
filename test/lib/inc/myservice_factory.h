#ifndef __MYSERVICE_FACTORY__H__
#define __MYSERVICE_FACTORY__H__

#include <memory>

#include "i_myservice.h"
 
class myservice_factory
{
public:
    enum interface_type
    {
        LOCAL,
        PROXY
    };

    static std::unique_ptr<i_myservice> create(interface_type _type);
};

#endif // __MYSERVICE_FACTORY__H__
