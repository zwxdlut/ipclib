#include <sstream>

#include "example.h"

example::example()
{
}

example::~example()
{ 
}

void example::func1(const int32_t _i32, const bool _b, const data_a &_a, std::vector<data_a> &_as)
{ 
    LOGI(TAG, "%d, %d, %s, %s", _i32, _b, formater::format(_a)->c_str(), formater::format(_as)->c_str());

    _as = {{1000, {"111", "222"}, true, true, {98, 980}}, {2000, {"333", "444"}, false, false, {99, 990}}};
    
    for (auto i : callbacks_)
    {
        if (nullptr != i)
        {
            i->on_info(std::string("example::func1 returned"));
        }
    }
}

void example::func2(const double _f64)
{
    LOGI(TAG, "%lf", _f64);

    for (auto i : callbacks_)
    {
        if (nullptr != i)
        {
            i->on_info(std::string("example::func2 returned"));
        }
    }
}

void example::func3(uint8_t &_i8)
{
    LOGI(TAG, "%d", _i8);

    _i8 = 97;

    for (auto i : callbacks_)
    {
        if (nullptr != i)
        {
            i->on_info(std::string("example::func3 returned"));
        }
    }
}

void example::func4()
{
    LOGI(TAG, "");

    for (auto i : callbacks_)
    {
        if (nullptr != i)
        {
            i->on_info(std::string("example::func4 returned"));
        }
    }
}


