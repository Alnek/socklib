#ifndef __SOCKET_OBJECT_H__
#define __SOCKET_OBJECT_H__

#include "connection.h"
#include "socket.h"

#include <memory>
#include <vector>

class SocketCallback;

class SystemSocket
{
public:
    ~SystemSocket();

private:
    friend class Socket;

    SystemSocket();
    SystemSocket(const SystemSocket&);
    SystemSocket& operator=(const SystemSocket&);

    void Create();
    bool Bind(const char* addr, uint16_t port);
    bool Listen();
    Socket Accept();
    bool Recv(std::vector<char>& buffer);
    bool Send(const std::vector<char>& buffer);
    void Shutdown();
    void Close();

    uintptr_t fd;
    std::shared_ptr<SocketCallback> callback;
    int state;
    ConnectionInfo conInfo;
};

#endif
