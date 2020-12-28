#include <functional>

#include "example_proxy.h"

example_proxy::example_proxy()
{
    ipc_.connect(EXAMPLE_CONNECTION);
    ipc_.bind(EXAMPLE_OBJECT_PATH, EXAMPLE_INTERFACE, "on_info", 
        [this](const std::string &info)
        {
            for (auto i : callbacks_)
            {
                if (nullptr != i)
                {
                    i->on_info(info);
                }
                
            }
        });
    ipc_.start();
}

example_proxy::~example_proxy() {}

void example_proxy::func1(const int32_t _i32, const bool _b, const data_a &_a, std::vector<data_a> &_as)
{
    ipc_.invoke(EXAMPLE_OBJECT_PATH, EXAMPLE_INTERFACE, __FUNCTION__, _i32, _b, _a, _as);
}

void example_proxy::func2(const double _f64)
{
    ipc_.invoke(EXAMPLE_OBJECT_PATH, EXAMPLE_INTERFACE, __FUNCTION__, _f64);
}

void example_proxy::func3(uint8_t &_i8)
{
    ipc_.invoke(EXAMPLE_OBJECT_PATH, EXAMPLE_INTERFACE, __FUNCTION__, _i8);
}

void example_proxy::func4()
{
    ipc_.invoke(EXAMPLE_OBJECT_PATH, EXAMPLE_INTERFACE, __FUNCTION__);
}









