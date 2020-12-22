#ifndef __MYSERVICE__H__
#define __MYSERVICE__H__

#include "imyservice.h"

class myservice : public imyservice
{
public:
    myservice();

    ~myservice();

    void func1(const int32_t _i32, const bool _b, const data_a &_a, std::vector<data_a> &_as);

    void func2(const double _f64);

    void func3(uint8_t &_i8);

    void func4();

private:
    const char *TAG = "myservice";
};

#endif // __MYSERVICE__H__
