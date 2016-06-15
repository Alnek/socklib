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
    std::vector<Socket> copy(mSockets);
    mFDMap.clear();
    mSockets.clear();

    for (auto it = copy.begin(); it != copy.end(); ++it)
    {
        it->SetCallback(nullptr);
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
    Socket::Init();
    mSockets.reserve(Socket::MAX);
    mRSet.reserve(Socket::MAX);
    mWSet.reserve(Socket::MAX);
    mXSet.reserve(Socket::MAX);
}

ConnectionManager::~ConnectionManager()
{
    Socket::CleanUp();
}

void ConnectionManager::Register(Socket socket)
{
    mSockets.push_back(socket);

    const size_t lastIndex = mSockets.size() - 1;
    mFDMap[socket.GetFD()] = lastIndex;
}

void ConnectionManager::Unregister(Socket socket)
{
    auto it = std::find(mSockets.begin(), mSockets.end(), socket);
    if (it != mSockets.end()) mSockets.erase(it);

    mFDMap.clear();
    int index = 0;
    for (auto it = mSockets.begin(); it != mSockets.end(); ++it)
    {
        mFDMap[it->GetFD()] = index++;
    }
}

void ConnectionManager::Select(uint64_t nanoSec)
{
    if (0 == mSockets.size()) return;

    mRSet.clear();mWSet.clear();mXSet.clear();

    // fill
    for (auto it = mSockets.begin(); it != mSockets.end(); ++it)
    {
        Socket& s = *it;
        if (0 == s.GetFD() || Socket::INVALID == s.GetFD()) continue;

        if (Socket::READ & s.GetState()) mRSet.push_back(s.GetFD());
        if (Socket::WRITE & s.GetState()) mWSet.push_back(s.GetFD());
        mXSet.push_back(s.GetFD());
    }

    if (false == Socket::Select(nanoSec == FOREVER ? nullptr : &nanoSec, mRSet, mWSet, mXSet))
        return;

    // process
    for (auto it = mRSet.begin(); it != mRSet.end(); ++it)
    {
        Socket* socket = FindByFD(*it);
        if (socket) socket->CancelAsyncRead();
        if (socket && socket->GetCallback())
        {
            socket->GetCallback()->DoRecv();
        }
    }
    for (auto it = mWSet.begin(); it != mWSet.end(); ++it)
    {
        Socket* socket = FindByFD(*it);
        if (socket) socket->CancelAsyncWrite();
        if (socket && socket->GetCallback())
        {
            socket->GetCallback()->DoSend();
        }
    }
    for (auto it = mXSet.begin(); it != mXSet.end(); ++it)
    {
        Socket* socket = FindByFD(*it);
        if (socket) socket->CancelAsync();
        if (socket && socket->GetCallback())
        {
            socket->GetCallback()->HandleError();
        }
    }
}

Socket* ConnectionManager::FindByFD(uintptr_t fd)
{
    auto it = mFDMap.find(fd);
    if (it == mFDMap.end()) return nullptr;

    return &mSockets[it->second];
}

void ConnectionManager::List(std::vector<uint64_t>& ids) const
{
    for (auto it = mSockets.begin(); it != mSockets.end(); ++it)
    {
        //if (it->GetConnInfo().IsValid())
            ids.push_back(it->GetFD());
    }
}

Socket* ConnectionManager::FindById(uintptr_t id)
{
    return FindByFD(id);
}
