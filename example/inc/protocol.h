#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <sstream>
#include <string>
#include <vector>

#include "core.h"

#define EXAMPLE_CONNECTION  "com.example.conn"
#define EXAMPLE_OBJECT_PATH "/com/example/path"
#define EXAMPLE_INTERFACE   "com.example.interface"

struct data_b : public ipc::dbus_helper::serializable
{
    data_b()
    {

    }

    data_b(const uint8_t _i8, const uint32_t _i32): i8_(_i8), i32_(_i32)
    {

    }

    void signature(std::string &_sig)
    {
        _sig = "(yi)";
    }

    int pack(ipc::dbus_helper *_dbus, DBusMessageIter &_iter) const
    {
        _dbus->arg_in(_iter, i8_);
        _dbus->arg_in(_iter, i32_);

        return 0;
    }

    int unpack(ipc::dbus_helper *_dbus, DBusMessageIter &_iter)
    {
        _dbus->arg_out(_iter, i8_);
        _dbus->arg_out(_iter, i32_);
        
        return 0;
    }

    uint8_t i8_;
    int32_t i32_;
};


struct data_a : public ipc::dbus_helper::serializable
{
    data_a()
    {

    }

    data_a(const int32_t _i32, const std::vector<std::string> &_strs, const bool _b1, const bool _b2, const data_b &_b): 
        i32_(_i32), strs_(_strs), b1_(_b1), b2_(_b2), b_(_b)
    {

    }

    void signature(std::string &_sig)
    {
        _sig = "(iasbb(yi))";
    }

    int pack(ipc::dbus_helper *_dbus, DBusMessageIter &_iter) const
    {
        _dbus->arg_in(_iter, i32_);
        _dbus->arg_in(_iter, strs_);
        _dbus->arg_in(_iter, b1_);
        _dbus->arg_in(_iter, b2_);
        _dbus->arg_in(_iter, b_);

        return 0;
    }

    int unpack(ipc::dbus_helper *_dbus, DBusMessageIter &_iter)
    {
        _dbus->arg_out(_iter, i32_);
        _dbus->arg_out(_iter, strs_);
        _dbus->arg_out(_iter, b1_);
        _dbus->arg_out(_iter, b2_);
        _dbus->arg_out(_iter, b_);

        return 0;
    }

    int32_t i32_;
    std::vector<std::string> strs_;
    bool b1_;
    bool b2_;
    data_b b_;
};

class formater
{
public:
    template <typename T>
    static std::unique_ptr<std::string> format(const T &_t)
    { 
        return nullptr;
    }

    template <typename T>
    static std::unique_ptr<std::string> format(const std::vector<T> &_vt)
    {
        std::stringstream ss;
        
        ss << "[";
        for (auto &t : _vt)
        {
            ss << *format(t) << " ";
        }
        ss << "]";

        return std::make_unique<std::string>(ss.str());
    }
};

template <>
inline std::unique_ptr<std::string> formater::format(const data_b &_t)
{
    std::stringstream ss;
    ss << "{" << _t.i8_ << ", " << _t.i32_ << "}";

    return std::make_unique<std::string>(ss.str());
}

template <>
inline std::unique_ptr<std::string> formater::format(const data_a &_t)
{
    std::stringstream ss;

    ss << "{" << _t.i32_ << ", [";
    for (auto &s : _t.strs_)
    {
        ss << s << " ";
    }
    ss << "], " << _t.b1_ << ", " << _t.b2_ << ", " << *format(_t.b_) << "}";

    return std::make_unique<std::string>(ss.str());
}

#endif // __PROTOCOL_H__
