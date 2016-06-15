#include "connectionmanager.h"

#include "socketcallback.h"
#include "socketobject.h"

namespace
{
    ConnectionManager* sInstance = nullptr;
}

ConnectionManager& ConnectionManager::GetInstance()
{
    if (nullptr == sInstance)
    {
        sInstance = new ConnectionManager;
    }
    return *sInstance;
}

void ConnectionManager::Shutdown()
{
    std::vector<std::shared_ptr<SystemSocket> > copy(mSockets);
    mFDMap.clear();
    mSockets.clear();

    for (auto it = copy.begin(); it != copy.end(); ++it)
    {
        (*it)->callback = nullptr;
    }
    copy.clear();

    if (nullptr != sInstance)
    {
        delete sInstance;
        sInstance = nullptr;
    }
}

ConnectionManager::ConnectionManager()
{
    SystemSocket::Init();
    mSockets.reserve(SystemSocket::MAX);
    mRSet.reserve(SystemSocket::MAX);
    mWSet.reserve(SystemSocket::MAX);
    mXSet.reserve(SystemSocket::MAX);
}

ConnectionManager::~ConnectionManager()
{
    SystemSocket::Cleanup();
}

void ConnectionManager::Register(std::shared_ptr<SystemSocket> socket)
{
    mSockets.push_back(socket);

    const size_t lastIndex = mSockets.size() - 1;
    mFDMap[socket->fd] = lastIndex;
}

void ConnectionManager::Unregister(std::shared_ptr<SystemSocket> socket)
{
    auto it = std::find(mSockets.begin(), mSockets.end(), socket);
    if (it != mSockets.end()) mSockets.erase(it);

    mFDMap.erase(socket->fd);
}

void ConnectionManager::Select(uint64_t nanoSec)
{
    if (0 == mSockets.size()) return;

    mRSet.clear();mWSet.clear();mXSet.clear();

    // fill
    for (auto it = mSockets.begin(); it != mSockets.end(); ++it)
    {
        SystemSocket& s = **it;
        if (0 == s.fd || SystemSocket::INVALID == s.fd) continue;

        if (Socket::READ & s.state) mRSet.push_back(s.fd);
        if (Socket::WRITE & s.state) mWSet.push_back(s.fd);
        mXSet.push_back(s.fd);
    }

    if (false == SystemSocket::Select(nanoSec == FOREVER ? nullptr : &nanoSec, mRSet, mWSet, mXSet))
        return;

    // process
    for (auto it = mRSet.begin(); it != mRSet.end(); ++it)
    {
        std::shared_ptr<SystemSocket> socket = FindByFD(*it);
        if (socket)
        {
            socket->state &= (~Socket::READ);
            socket->callback->DoRecv();
        }
    }
    for (auto it = mWSet.begin(); it != mWSet.end(); ++it)
    {
        std::shared_ptr<SystemSocket> socket = FindByFD(*it);
        if (socket)
        {
            socket->state &= (~Socket::WRITE);
            socket->callback->DoSend();
        }
    }
    for (auto it = mXSet.begin(); it != mXSet.end(); ++it)
    {
        std::shared_ptr<SystemSocket> socket = FindByFD(*it);
        if (socket)
        {
            socket->state = 0;
            socket->callback->HandleError();
        }
    }
}

std::shared_ptr<SystemSocket> ConnectionManager::FindByFD(uintptr_t fd) const
{
    auto it = mFDMap.find(fd);
    if (it == mFDMap.end()) return nullptr;

    return mSockets[it->second];
}

void ConnectionManager::List(std::vector<uint64_t>& ids) const
{
    for (auto it = mSockets.begin(); it != mSockets.end(); ++it)
    {
        if ((*it)->conInfo.IsValid())
            ids.push_back((*it)->fd);
    }
}
