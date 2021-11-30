#ifndef __IPC_CORE_SERVER_H__
#define __IPC_CORE_SERVER_H__

#include "core.h"

namespace ipc
{
    class server : public core 
    {
    public:
        server();

        ~server();

        int connect(const std::string &_conn);

        template <typename... ARGS>
        int invoke(const std::string &_path, const std::string &_iface, const std::string &_member, ARGS&... _args)
        {
            return send(_path, _iface, _member, _args...);
        }

    private:
        int add_match(const std::string &_path, const std::string &_iface, const std::string &_member);

        bool is_match(dbus_message _msg, const std::string &_iface, const std::string &_member);

        template <typename... ARGS>
        int send(const std::string &_path, const std::string &_iface, const std::string &_member, ARGS&... _args)
        {       
            int ret = 0;
            dbus_message msg = nullptr;
            std::lock_guard<std::mutex> lock(send_mutex_);
        
            if (0 == (msg = dbus_.create_signal(_path, _iface, _member))
                || 0 != dbus_.pack_args(msg, _args...) 
                || 0 != dbus_.send(msg))
            {
                ret = -1;
            }
        
            dbus_.free(msg);

            LOGI(TAG, "path = %s, iface = %s, method = %s %s", 
                _path.c_str(), _iface.c_str(), _member.c_str(), 0 == ret ? "successful!" : "failed!");

            return ret;
        }
    };
} // namespace ipc

#endif // __IPC_CORE_SERVER_H__
