#ifndef __EXAMPLE__H__
#define __EXAMPLE__H__

#include "iexample.h"

class example : public iexample
{
public:
    example();

    ~example();

    void func1(const int32_t _i32, const bool _b, const data_a &_a, std::vector<data_a> &_as);

    void func2(const double _f64);

    void func3(uint8_t &_i8);

    void func4();

private:
    const char *TAG = "example";
};

#endif // __EXAMPLE__H__
