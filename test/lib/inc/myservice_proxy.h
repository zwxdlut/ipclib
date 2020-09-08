#ifndef __MYSERVICE_PROXY_H__
#define __MYSERVICE_PROXY_H__

#include "i_myservice.h"
#include "client.h"

class myservice_proxy : public i_myservice
{
public:
    myservice_proxy();

    ~myservice_proxy();

    int func1(const int32_t _i32, const bool _b, const data_a &_a, std::vector<data_a> &_as);

    int func2(const double _f64);

    int func3(uint8_t &_i8);

    int func4();

private:
    ipc::client ipc_;
};

#endif // __MYSERVICE_PROXY_H__
