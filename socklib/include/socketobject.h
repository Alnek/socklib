#ifndef __SOCKET_OBJECT_H__
#define __SOCKET_OBJECT_H__

#include "connection.h"
#include "socket.h"

#include <memory>
#include <string>
#include <vector>

class SocketCallback;

class SystemSocket
{
public:
    ~SystemSocket();

private:
    friend class Socket;            // only Socket can spawn SystemSocket

    static const uint32_t MAX;
    static const uintptr_t INVALID;

    static void Init();             // OS specific init
    static void CleanUp();          // OS specific cleanup
    static bool Select(uint64_t* nanoSec, std::vector<uintptr_t>& rSet, std::vector<uintptr_t>& wSet, std::vector<uintptr_t>& xSet);

    SystemSocket();
    SystemSocket(const SystemSocket&);
    SystemSocket& operator=(const SystemSocket&);

    void Create();
    bool Connect(const char* addr, uint16_t port);
    bool Bind(const char* addr, uint16_t port);
    bool Listen();
    Socket Accept();
    bool Recv(std::string& buffer);
    bool Send(const std::string& buffer);
    void Shutdown();
    void Close();

    uintptr_t GetFD() const { return fd; }

    uintptr_t fd;
    std::shared_ptr<SocketCallback> callback;
    int state;
    ConnectionInfo conInfo;
};

#endif
