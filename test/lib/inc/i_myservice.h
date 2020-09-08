#ifndef __I_MYSERVICE_H__
#define __I_MYSERVICE_H__

#include <algorithm>
#include <vector>

#include "protocol.h"

class i_myservice
{
public:
    class i_callback
    {
    public:
        virtual void on_info(const std::string &info) = 0;
    };

public:
    i_myservice()
    {
    }

    virtual ~i_myservice()
    {
    }

    virtual int func1(const int32_t _i32, const bool _b, const data_a &_a, std::vector<data_a> &_as) = 0;

    virtual int func2(const double _f64) = 0;

    virtual int func3(uint8_t &_i8) = 0;

    virtual int func4() = 0;

    virtual void add_callback(const i_callback *_callback)
    {
        callbacks_.push_back(const_cast<i_callback *>(_callback));
    }

    virtual void remove_callback(const i_callback *_callback)
    {
        std::vector<i_callback *>::iterator iter = std::find(callbacks_.begin(), callbacks_.end(), _callback);

        if (callbacks_.end() == iter)
        {
            return;
        }

         callbacks_.erase(iter);        
    }

protected:
    std::vector<i_callback *> callbacks_;
};

#endif // __I_MYSERVICE_H__
