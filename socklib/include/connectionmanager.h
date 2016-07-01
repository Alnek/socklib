#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "socket.h"

#include "lockobject.h"

#include <memory>
#include <map>
#include <vector>

class SocketCallback;

class ConnectionManagerStrategy
{
public:
    virtual ~ConnectionManagerStrategy() {}

    virtual void onStrategyInstalled(int totalClients) = 0;
    virtual void onClientConnected(int totalClients) = 0;
    virtual void onClientDisconnected(int totalClients) = 0;
    //
};

class ConnectionManager
{
public:
    static const uint64_t FOREVER = ~0;
    static ConnectionManager& GetInstance();

    bool Select(uint64_t nanoSec = FOREVER);
    void Shutdown();

    void List(std::vector<uint64_t>& ids) const;
    Socket* FindById(uintptr_t id);

    void InstallStrategy(ConnectionManagerStrategy* strategy);
    size_t Join();
    void UpdateJoinTID();

    static uint64_t GetTID();
private:
    friend class Socket;

    ConnectionManager();
    ~ConnectionManager();

    void Register(Socket socket, uint64_t threadId);
    void Unregister(Socket socket, uint64_t threadId);

    Socket* FindByFD(uintptr_t fd);

    std::map<uintptr_t, std::shared_ptr<Socket> > mSockets;

    std::vector<uintptr_t> mRSet;
    std::vector<uintptr_t> mWSet;
    std::vector<uintptr_t> mXSet;

    //connect/disconnect events
    ConnectionManagerStrategy* mStrategy;

    LockObject mLock;
    std::vector<Socket> mRegisterBuffer;
    std::vector<Socket> mUnregisterBuffer;
    uint64_t mJoinTID;
};

#endif
