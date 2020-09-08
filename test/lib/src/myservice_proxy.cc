#include <functional>

#include "myservice_proxy.h"

myservice_proxy::myservice_proxy()
{
    ipc_.connect(MYSERVICE_CONNECTION);

    ipc_.bind(MYSERVICE_OBJECT_PATH, MYSERVICE_INFACE, "on_info", 
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

myservice_proxy::~myservice_proxy()
{
}

int myservice_proxy::func1(const int32_t _i32, const bool _b, const data_a &_a, std::vector<data_a> &_as)
{
    ipc_.invoke(MYSERVICE_OBJECT_PATH, MYSERVICE_INFACE, __FUNCTION__, _i32, _b, _a, _as);
    return 0;
}

int myservice_proxy::func2(const double _f64)
{
    ipc_.invoke(MYSERVICE_OBJECT_PATH, MYSERVICE_INFACE, __FUNCTION__, _f64);
    return 0;
}

int myservice_proxy::func3(uint8_t &_i8)
{
    ipc_.invoke(MYSERVICE_OBJECT_PATH, MYSERVICE_INFACE, __FUNCTION__, _i8);
    return 0;
}

int myservice_proxy::func4()
{
    ipc_.invoke(MYSERVICE_OBJECT_PATH, MYSERVICE_INFACE, __FUNCTION__);
    return 0;
}









