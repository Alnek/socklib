#ifndef __SOCKET_CALLBACK_H__
#define __SOCKET_CALLBACK_H__

#include "socket.h"

#include <memory>
#include <vector>

class SocketCallback
{
public:
    virtual void DoRecv() {};
    virtual void DoSend() {};
    virtual void HandleError() {};

    virtual ~SocketCallback() {};
};

#endif
