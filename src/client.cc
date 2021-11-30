#include "client.h"

namespace ipc
{
    client::client() 
    { 
        TAG = "ipc::client"; 
    }

    client::~client()
    {

    }

    int client::connect(const std::string &_conn)
    {
        core::connect(_conn);
        LOGI(TAG, "conn = %s", _conn.c_str());
        
        return dbus_.connect("");
    }

    int client::add_match(const std::string &_path, const std::string &_iface, const std::string &_member)
    {
        return dbus_.add_match(std::string("type='signal',path='") + _path + "',interface='" + _iface + "',member='" + _member + "'");
    }

    bool client::is_match(dbus_message _msg, const std::string &_iface, const std::string &_member)
    {
        return dbus_.is_signal(_msg, _iface, _member);
    }
} // namespace ipc

