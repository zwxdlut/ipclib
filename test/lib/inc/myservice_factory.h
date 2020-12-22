#ifndef __MYSERVICE_FACTORY__H__
#define __MYSERVICE_FACTORY__H__

#include <memory>

#include "imyservice.h"
 
class myservice_factory
{
public:
    enum interface_type
    {
        LOCAL,
        PROXY
    };

    static std::unique_ptr<imyservice> create(interface_type _type);
};

#endif // __MYSERVICE_FACTORY__H__
