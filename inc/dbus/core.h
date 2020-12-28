#ifndef __IPC_DBUS_CORE_H__
#define __IPC_DBUS_CORE_H__

#include <string>
#include <vector>
#include <map>
#include <set>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <iostream>
#include <chrono>
#include <type_traits>

#include "dbus_helper.h"
#include "func_traits.h"

namespace ipc
{
    class core
    {
        struct key_t
        {
            std::string path_;
            std::string iface_;
            std::string member_;

            bool operator<(const key_t &_other) const
            {
                auto cmp_rst = path_.compare(_other.path_);
                if (0 != cmp_rst)
                {
                    return cmp_rst < 0;
                }

                cmp_rst = iface_.compare(_other.iface_);
                if (0 != cmp_rst)
                {
                    return cmp_rst < 0;
                }

                cmp_rst = member_.compare(_other.member_);
                if (0 != cmp_rst)
                {
                    return cmp_rst < 0;
                }

                return false;
            }
        };

    protected:
        using dbus_message = DBusMessage*;
        using bind_function = std::function<void(const dbus_message &)>;

    public:
        core();

        virtual ~core();

        virtual int connect(const std::string &_conn);

        int disconnect();

        /* If input argument of FUNC is 'const', it must be also decorated reference(&), 
           because substitution for FUNC argument will remove 'const'. */
        template <typename FUNC>
        void bind(const std::string &_path, const std::string &_iface, const std::string &_member, FUNC _func)
        {
            add_match(_path, _iface, _member);

            key_t key { _path, _iface, _member };
            std::lock_guard<std::mutex> lock(binds_mutex_);

            binds_.insert(std::make_pair(key, [this, _func, key](const dbus_message &_msg)
            { 
                receives(key, _func, _msg); 
            }));
        }

        void unbind(const std::string &_path, const std::string &_iface, const std::string &_member);

        void start(const bool _sync = false);

        void stop();

        template <typename... ARGS>
        int invoke(const std::string &_path, const std::string &_iface, const std::string &_member, ARGS&... _args);

    private:
        virtual int add_match(const std::string &_path, const std::string &_iface, const std::string &_member) = 0;

        virtual bool is_match(dbus_message _msg, const std::string &_iface, const std::string &_member) = 0;

        void listen();

        template <typename FUNC>
        int receives(const key_t &_key, FUNC _func, dbus_message _msg)
        {
            if (nullptr == _msg)
            {
                LOGE(TAG, "message is null!");
                return -1;
            }

            if (dbus_.has_path(_msg, _key.path_) && is_match(_msg, _key.iface_, _key.member_))
            {
                LOGI(TAG, "path = %s, iface = %s, member = %s", _key.path_.c_str(), _key.iface_.c_str(), _key.member_.c_str());

                using args_type = typename ipc::func_traits<FUNC>::args_type;
                args_type args;

                return receive(_key, _func, _msg, args, std::make_index_sequence<std::tuple_size<args_type>::value>{});

            }

            return 0;
        }

        template <typename FUNC, typename TUPLE_ARGS, size_t... N>
        int receive(const key_t &_key, const FUNC &_func, dbus_message _msg, TUPLE_ARGS &_args, std::index_sequence<N...>)
        {
            if (nullptr == _msg)
            {
                LOGE(TAG, "message is null!");
                return -1;
            }

            if (0 == dbus_.unpack_args(_msg, std::get<N>(_args)...))
            {
                _func(std::get<N>(_args)...);

                if (dbus_.is_method_call(_msg, _key.iface_, _key.member_))
                {
                    int ret = 0;

                    if (ipc::args_check<typename ipc::func_traits<FUNC>::args_oringinal_type>::has_out_)
                    {
                        dbus_message reply_msg = dbus_.create_reply(_msg);

                        if (0 == reply_msg || 0 != dbus_.pack_args(reply_msg, std::get<N>(_args)...) ||
                            0 != dbus_.send(reply_msg))
                        {
                            LOGE(TAG, "replay failed!");
                            ret = -1;
                        }

                        dbus_.free(reply_msg);
                    }

                    return ret;
                }

                return 0;
            }
            else
            {
                LOGE(TAG, "unpack arguments failed!");
                return -1;
            }
        }

    protected: 
        const char                     *TAG = "ipc::core";
        bool                           done_;
        bool                           listen_thread_exit_;
        std::string                    conn_;
        dbus_helper                    dbus_;
        std::map<key_t, bind_function> binds_;
        std::thread                    *listen_thread_;
        std::mutex                     listen_mutex_;
        std::condition_variable        listen_cond_;
        std::mutex                     send_mutex_;
        std::mutex                     binds_mutex_;
    };
} // namespace ipc

#endif // __IPC_DBUS_CORE_H__
