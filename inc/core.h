#ifndef __IPC_CORE_H__
#define __IPC_CORE_H__

#include <mutex>

#if defined _IPC_DBUS_
    #include "dbus/core.h"
#elif defined _IPC_SOCKET_
#endif

#endif // __IPC_CORE_H__
