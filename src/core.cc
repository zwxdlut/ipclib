#include "core.h"
#include "server.h"
#include "client.h"

namespace ipc
{
    core::core()
    {
        done_ = true;
        listen_thread_exit_ = true;
    }

    core::~core()
    {
        disconnect();
    }

    int core::connect(const std::string &_conn)
    {
        conn_ = _conn;
        return 0;
    }

    int core::disconnect()
    {
        stop();
        binds_.clear();
        conn_.clear();
        return dbus_.disconnect();
    }

    void core::unbind(const std::string &_path, const std::string &_iface, const std::string &_method)
    {
        std::lock_guard<std::mutex> lock(binds_mutex_);
        binds_.erase(key_t{_path, _iface, _method});
    }

    void core::start(const bool _sync)
    {
        done_ = false;

        if(_sync)
        {
            listen();
        }
        else
        {
            listen_thread_exit_ = false;
            listen_thread_ = new std::thread([this](){listen();});
        }
    }

    void core::stop()
    {
        done_ = true;

        {
            std::unique_lock<std::mutex> lock(listen_mutex_);
            if (!listen_thread_exit_)
            {
                listen_cond_.wait(lock);
            }
        }

        if (nullptr != listen_thread_)
        {
            listen_thread_->join();
            delete listen_thread_;
            listen_thread_ = nullptr;
        }
    }

    void core::listen()
    {
        LOGI(TAG, "+");

        while(!done_)
        {
            DBusMessage *msg = dbus_.pop();

            if(nullptr != msg)
            {           
                // Debug
                // const char *type = dbus_message_type_to_string(dbus_message_get_type(msg));
                // const char *sender = dbus_message_get_sender(msg);
                // const char *dest = dbus_message_get_destination(msg);
                // const char *path = dbus_message_get_path(msg);
                // const char *iface = dbus_message_get_interface(msg);
                // const char *member = dbus_message_get_member(msg);
                // LOGI(TAG, "msg = %p, type = %s, sender = %s -> dest = %s, path = %s, iface = %s, member = %s", msg, type, sender, dst, path, iface, member);

                key_t key { dbus_message_get_path(msg), dbus_message_get_interface(msg), dbus_message_get_member(msg) };

                LOGI(TAG, "path = %s, iface = %s, member = %s", key.path_.c_str(), key.iface_.c_str(), key.member_.c_str());

                {
                    std::lock_guard<std::mutex> lock(binds_mutex_);
                    auto find = binds_.find(key);

                    if (binds_.end() != find)
                    {
                        find->second(msg);
                    }
                }

                dbus_.free(msg);
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        std::lock_guard<std::mutex> lock(listen_mutex_);

        listen_cond_.notify_all();
        listen_thread_exit_ = true;

        LOGI(TAG, "-");
    }
    
    // TODO: CRTP
    // RTTI
    template <typename... ARGS>
    int core::invoke(const std::string &_path, const std::string &_iface, const std::string &_member, ARGS&... _args)
    {
        if (client *ipc = dynamic_cast<client*>(this))
        {
            return ipc->invoke(_path, _iface, _member, _args...);
        } 
        else if(server *ipc = dynamic_cast<server*>(this))
        {
            return ipc->invoke(_path, _iface, _member, _args...);
        }
        else
        {
            LOGE(TAG, "dynamic_cast ipc core failed!");
            return -1;
        }
    }
} // namespace ipc

