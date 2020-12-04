#include "server.h"

namespace ipc
{
    server::server()
    {
        TAG = "ipc::server";
    }

    server::~server(){}

    int server::connect(const std::string &_conn)
    {
        core::connect(_conn);
        LOGI(TAG, "conn = %s", _conn.c_str());

        return dbus_.connect(conn_);
    }

    int server::add_match(const std::string &_path, const std::string &_iface, const std::string &_member)
    {
        return dbus_.add_match(std::string("type='method_call',path='") + _path + "',interface='" + _iface + "',member='" + _member + "'");
    }

    bool server::is_match(dbus_message _msg, const std::string &_iface, const std::string &_member)
    {
        return dbus_.is_method_call(_msg, _iface, _member);
    }
} // namespace ipc

