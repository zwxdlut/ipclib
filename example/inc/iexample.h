#ifndef __IEXAMPLE_H__
#define __IEXAMPLE_H__

#include <algorithm>
#include <vector>

#include "protocol.h"

class iexample
{
public:
    class icallback
    {
    public:
        virtual void on_info(const std::string &info) = 0;
    };

public:
    iexample() {}

    virtual ~iexample() {}

    virtual void func1(const int32_t _i32, const bool _b, const data_a &_a, std::vector<data_a> &_as) = 0;

    virtual void func2(const double _f64) = 0;

    virtual void func3(uint8_t &_i8) = 0;

    virtual void func4() = 0;

    virtual void add_callback(const icallback *_callback)
    {
        callbacks_.push_back(const_cast<icallback *>(_callback));
    }

    virtual void remove_callback(const icallback *_callback)
    {
        std::vector<icallback *>::iterator iter = std::find(callbacks_.begin(), callbacks_.end(), _callback);

        if (callbacks_.end() == iter)
        {
            return;
        }

         callbacks_.erase(iter);        
    }

protected:
    std::vector<icallback *> callbacks_;
};

#endif // __IEXAMPLE_H__
