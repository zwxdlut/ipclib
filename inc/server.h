#ifndef __IPC_SERVER_H__
#define __IPC_SERVER_H__

#include <mutex>

#if defined _IPC_DBUS_
    #include "dbus/server.h"
#elif defined _IPC_SOCKET_
#endif

#endif // __IPC_SERVER_H__