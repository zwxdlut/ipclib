#include "example_factory.h"
#include "example.h"
#include "example_proxy.h"

std::unique_ptr<iexample> example_factory::create(interface_type _type)
{
    if (LOCAL == _type)
    {
        return std::make_unique<example>();
    }
    else
    {
        return std::make_unique<example_proxy>();
    }
}