#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <sstream>
#include <string>
#include <vector>

#include "core.h"

#define MYSERVICE_CONNECTION "com.myservice.conn"
#define MYSERVICE_OBJECT_PATH "/com/myservice/path"
#define MYSERVICE_INFACE "com.myservice.inface"

struct data_b 
{
    uint8_t i8_;
    int32_t i32_;
};

struct data_a
{
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
    static std::unique_ptr<std::string> format(const T &_t) {return nullptr;}

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
    for(auto &s : _t.strs_)
    {
        ss << s << " ";
    }
    ss << "], " << _t.b1_ << ", " << _t.b2_ << ", " << *format(_t.b_) << "}";

    return std::make_unique<std::string>(ss.str());
}

namespace ipc
{
#if defined _IPC_DBUS_
        template<>
        inline int dbus_helper::signature<data_b>(std::string &_sig)
        {
            _sig = "(yi)";
            return 0;
        }

        template<>
        inline int dbus_helper::pack(DBusMessageIter &_iter, const data_b &_t){
            arg_in(_iter, _t.i8_);
            arg_in(_iter, _t.i32_);
            return 0;
        }

        template<>
        inline int dbus_helper::unpack(DBusMessageIter &_iter, data_b &_t){
            arg_out(_iter, _t.i8_);
            arg_out(_iter, _t.i32_);
            return 0;
        }

        template<>
        inline int dbus_helper::signature<data_a>(std::string &_sig){
            _sig = "(iasbb(yi))";
            return 0;
        }

        template<>
        inline int dbus_helper::pack(DBusMessageIter &_iter, const data_a &_t){
            arg_in(_iter, _t.i32_);
            arg_in(_iter, _t.strs_);
            arg_in(_iter, _t.b1_);
            arg_in(_iter, _t.b2_);
            arg_in(_iter, _t.b_);
            return 0;
        }

        template<>
        inline int dbus_helper::unpack(DBusMessageIter &_iter, data_a &_t){
            arg_out(_iter, _t.i32_);
            arg_out(_iter, _t.strs_);
            arg_out(_iter, _t.b1_);
            arg_out(_iter, _t.b2_);
            arg_out(_iter, _t.b_);
            return 0;
        }
#elif defined _IPC_SOCKET_
#endif
} // namespace ipc

#endif // __PROTOCOL_H__
