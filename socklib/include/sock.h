#ifndef __SOCK_H__
#define __SOCK_H__

#include <memory>
#include <vector>

class Socket;
struct SystemSocket;
class SocketCallback;

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

class SocketSystem
{
public:
    static SocketSystem& GetInstance();

    Socket CreateSocket();
    void ShutDown();
    void Select();

    void Include(Socket& socket);
    void Exclude(Socket& socket);
private:
    friend class SocketCallback;

    SocketSystem();
    ~SocketSystem();

    void Init();

    std::shared_ptr<SystemSocket> FindByFD(uintptr_t fd) const;
    std::shared_ptr<SystemSocket> FindByCallback(SocketCallback* ptr) const;

    static SocketSystem* sInstance;

    std::vector<std::shared_ptr<SystemSocket> > mSockets;
};

class Socket
{
public:
    static const int READ = 0x01;
    static const int WRITE = 0x02;

    bool Bind(const char* addr, uint16_t port);
    bool Listen();
    Socket Accept();

    void SetState(int state);
    void SetCallback(SocketCallback* callback);

    bool Recv(std::vector<char>& buffer);
    bool Send(const std::vector<char>& buffer);
    void Shutdown();
    void Close();

    const ConnectionInfo& GetConnInfo() const;

    ~Socket();

private:
    friend class SocketSystem;
    friend class SocketCallback;

    //Socket();
    Socket(uintptr_t s);
    Socket(std::shared_ptr<SystemSocket>& systemSocket);

    std::shared_ptr<SystemSocket> mSockInfo;
};

class SocketCallback
{
public:
    Socket GetSocket(SocketCallback* programm);

    virtual void CanRead(Socket socket) {};
    virtual void CanWrite(Socket socket) {};
    virtual void ExFunc(Socket socket) {};

    virtual ~SocketCallback() {};
};

#endif
