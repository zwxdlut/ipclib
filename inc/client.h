#ifndef __IPC_CLIENT_H__
#define __IPC_CLIENT_H__

#include <mutex>

#if defined _IPC_DBUS_
    #include "dbus/client.h"
#elif defined _IPC_SOCKET_
#endif

#endif // __IPC_CLIENT_H__
