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
    Socket();
    Socket(const Socket&); //default is ok?
    ~Socket();

    bool Bind(const char* addr, uint16_t port);
    bool Listen();
    Socket Accept();

    bool Recv(std::vector<char>& buffer);
    bool Send(const std::vector<char>& buffer);
    void Shutdown();
    void Close();

    void AsyncRead();
    void AsyncWrite();
    void CancelAsync();
    void SetCallback(SocketCallback* callback);

    const ConnectionInfo& GetConnInfo() const;

private:
    Socket& operator=(const Socket&);

    friend class ConnectionManager;
    friend class SystemSocket;

    static const int READ = 0x01;
    static const int WRITE = 0x02;

    Socket(uintptr_t s);

    std::shared_ptr<SystemSocket> mSystemSocket;
};

#endif
