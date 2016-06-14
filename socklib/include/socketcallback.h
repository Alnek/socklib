#ifndef __SOCKET_CALLBACK_H__
#define __SOCKET_CALLBACK_H__

#include "socket.h"

#include <memory>
#include <vector>

class SocketCallback
{
public:
    Socket GetSocket(const SocketCallback* programm) const;

    virtual void CanRead(Socket socket) {};
    virtual void CanWrite(Socket socket) {};
    virtual void ExFunc(Socket socket) {};

    virtual ~SocketCallback() {};
};

#endif
