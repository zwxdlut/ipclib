#ifndef __EXAMPLE_PROXY_H__
#define __EXAMPLE_PROXY_H__

#include "iexample.h"
#include "client.h"

class example_proxy : public iexample
{
public:
    example_proxy();

    ~example_proxy();

    void func1(const int32_t _i32, const bool _b, const data_a &_a, std::vector<data_a> &_as);

    void func2(const double _f64);

    void func3(uint8_t &_i8);

    void func4();

private:
    ipc::client ipc_;
};

#endif // __EXAMPLE_PROXY_H__
