#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <memory>
#include <vector>

class SocketCallback;
class SystemSocket;
struct ConnectionInfo;

class Socket
{
public:
    static const int READ = 0x01;
    static const int WRITE = 0x02;

    Socket();
    //Socket(const Socket&); //default is ok
    ~Socket();

    bool Bind(const char* addr, uint16_t port);
    bool Listen();
    Socket Accept();

    bool Recv(std::vector<char>& buffer);
    bool Send(const std::vector<char>& buffer);
    void Shutdown();
    void Close();

    void SetState(int state);
    void SetCallback(SocketCallback* callback);

    const ConnectionInfo& GetConnInfo() const;

private:
    friend class SystemSocket;
    friend class SocketSystem;
    friend class SocketCallback;

    Socket(uintptr_t s);
    Socket(std::shared_ptr<SystemSocket>& systemSocket);

    std::shared_ptr<SystemSocket> mSystemSocket;
};

#endif
