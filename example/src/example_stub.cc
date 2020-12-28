#include <functional>

#include "example_stub.h"
#include "example_factory.h"

example_stub::example_stub()
{
    example_ = example_factory::create(example_factory::interface_type::LOCAL);
    example_->add_callback(this);

    ipc_.connect(EXAMPLE_CONNECTION);
    
    ipc_.bind(EXAMPLE_OBJECT_PATH, EXAMPLE_INTERFACE, "func1", 
        [this](const int32_t &_i32, const bool &_b, const data_a &_a, std::vector<data_a> &_as)
        {
            example_->func1(_i32, _b, _a, _as);
        });

    ipc_.bind(EXAMPLE_OBJECT_PATH, EXAMPLE_INTERFACE, "func2", 
        [this](const double &_f64)
        {
            example_->func2(_f64);
        });

    ipc_.bind(EXAMPLE_OBJECT_PATH, EXAMPLE_INTERFACE, "func3", 
        [this](uint8_t &_i8)
        {
            example_->func3(_i8);
        });

    ipc_.bind(EXAMPLE_OBJECT_PATH, EXAMPLE_INTERFACE, "func4", 
        [this](void)
        {
            example_->func4();
        });

    ipc_.start();
}

example_stub::~example_stub()
{
    example_->remove_callback(this);
}

void example_stub::on_info(const std::string &_info)
{
    ipc_.invoke(EXAMPLE_OBJECT_PATH, EXAMPLE_INTERFACE, __FUNCTION__, _info);
}



