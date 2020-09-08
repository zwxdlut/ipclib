#ifndef __MYSERVICE__H__
#define __MYSERVICE__H__

#include "i_myservice.h"

class myservice : public i_myservice
{
public:
    myservice();

    ~myservice();

    int func1(const int32_t _i32, const bool _b, const data_a &_a, std::vector<data_a> &_as);

    int func2(const double _f64);

    int func3(uint8_t &_i8);

    int func4();

private:
    const char *TAG = "myservice";
};

#endif // __MYSERVICE__H__
