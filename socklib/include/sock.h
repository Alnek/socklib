#ifndef __SOCK_H__
#define __SOCK_H__

#include <memory>
#include <vector>

class Socket;
struct SystemSocket;
struct fd_set;

class SocketSystem
{
public:
    static SocketSystem& GetInstance();

    Socket CreateSocket();
    void ShutDown();
    void Select();
private:
    SocketSystem();
    ~SocketSystem();

    void Init();

    static SocketSystem* sInstance;

    std::vector<std::shared_ptr<Socket> > mSockets;

    fd_set* mRSet;
    fd_set* mWSet;
    fd_set* mXSet;
};

class Socket
{
public:
    bool Bind();
    bool Listen();
    Socket Accept();

    ~Socket();

private:
    friend class SocketSystem;

    Socket();
    Socket(uintptr_t s);

    std::shared_ptr<SystemSocket> mSockInfo;
};

#endif
