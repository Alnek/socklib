#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "socket.h"

#include <memory>
#include <map>
#include <vector>

class SocketCallback;

class ConnectionManager
{
public:
    static const uint64_t FOREVER = ~0;
    static ConnectionManager& GetInstance();

    void Select(uint64_t nanoSec = FOREVER);
    void Shutdown();

    void List(std::vector<uint64_t>& ids) const;
    Socket* FindById(uintptr_t id);
private:
    friend class Socket;

    ConnectionManager();
    ~ConnectionManager();

    void Register(Socket socket);
    void Unregister(Socket socket);

    Socket* FindByFD(uintptr_t fd);

    std::vector<Socket> mSockets;
    std::map<uintptr_t, size_t> mFDMap;

    std::vector<uintptr_t> mRSet;
    std::vector<uintptr_t> mWSet;
    std::vector<uintptr_t> mXSet;
};

#endif
