#include <functional>

#include "myservice_stub.h"
#include "myservice_factory.h"

myservice_stub::myservice_stub()
{
    service_ = myservice_factory::create(myservice_factory::interface_type::LOCAL);
    service_->add_callback(this);

    ipc_.connect(MYSERVICE_CONNECTION);
      
    ipc_.bind(MYSERVICE_OBJECT_PATH, MYSERVICE_INTERFACE, "func1", 
        [this](const int32_t &_i32, const bool &_b, const data_a &_a, std::vector<data_a> &_as)
        {
            service_->func1(_i32, _b, _a, _as);
        });
    ipc_.bind(MYSERVICE_OBJECT_PATH, MYSERVICE_INTERFACE, "func2", 
        [this](const double &_f64)
        {
            service_->func2(_f64);
        });
    ipc_.bind(MYSERVICE_OBJECT_PATH, MYSERVICE_INTERFACE, "func3", 
        [this](uint8_t &_i8)
        {
            service_->func3(_i8);
        });
    ipc_.bind(MYSERVICE_OBJECT_PATH, MYSERVICE_INTERFACE, "func4", 
        [this](void)
        {
            service_->func4();
        });

    ipc_.start();
}

myservice_stub::~myservice_stub()
{
    service_->remove_callback(this);
}

void myservice_stub::on_info(const std::string &_info)
{
    ipc_.invoke(MYSERVICE_OBJECT_PATH, MYSERVICE_INTERFACE, __FUNCTION__, _info);
}



