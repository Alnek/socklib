#ifndef __CONNECTION_MANAGER_H__
#define __CONNECTION_MANAGER_H__

#include <stdint.h>

struct ConnectionInfo
{
    char addr[32];
    uint16_t port;

    ConnectionInfo()
    {
        addr[0] = 0;
        port = 0;
    }

    bool IsValid() const { return port != 0; }
};

#endif
