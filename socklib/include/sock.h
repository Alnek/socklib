#ifndef __SOCK_H__
#define __SOCK_H__

#include <memory>
#include <vector>

class Socket;
class SocketCallback;
class SystemSocket;

class SocketSystem
{
public:
    static SocketSystem& GetInstance();

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
    std::shared_ptr<SystemSocket> FindByCallback(const SocketCallback* ptr) const;

    static SocketSystem* sInstance;

    std::vector<std::shared_ptr<SystemSocket> > mSockets;
};

#endif
