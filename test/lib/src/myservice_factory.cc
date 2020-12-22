#include "myservice_factory.h"
#include "myservice.h"
#include "myservice_proxy.h"

std::unique_ptr<imyservice> myservice_factory::create(interface_type _type)
{
    if (LOCAL == _type)
    {
        return std::make_unique<myservice>();
    }
    else
    {
        return std::make_unique<myservice_proxy>();
    }
}