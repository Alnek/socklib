#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <memory>
#include <map>
#include <vector>

class SocketCallback;
class SystemSocket;

class ConnectionManager
{
public:
    static const uint64_t FOREVER = ~0;
    static ConnectionManager& GetInstance();

    void Select(uint64_t nanoSec = FOREVER);
    void Shutdown();

    void List(std::vector<uint64_t>& ids) const;
private:
    friend class Socket;

    ConnectionManager();
    ~ConnectionManager();

    void Register(std::shared_ptr<SystemSocket> socket);
    void Unregister(std::shared_ptr<SystemSocket> socket);

    std::shared_ptr<SystemSocket> FindByFD(uintptr_t fd) const;

    std::vector<std::shared_ptr<SystemSocket> > mSockets;
    std::map<uintptr_t, size_t> mFDMap;

    std::vector<uintptr_t> mRSet;
    std::vector<uintptr_t> mWSet;
    std::vector<uintptr_t> mXSet;
};

#endif
