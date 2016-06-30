#include "connectionmanager.h"

#include "socketcallback.h"
#include "socketobject.h"

#include <iostream>
#include <set>

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
    auto copy = mSockets;
    mSockets.clear();

    for (auto it = copy.begin(); it != copy.end(); ++it)
    {
        it->second->SetCallback(nullptr);
    }
    copy.clear();

    if (nullptr != sInstance)
    {
        delete sInstance;
        sInstance = nullptr;
    }
}

ConnectionManager::ConnectionManager()
    : mStrategy(nullptr)
{
    Socket::Init();
    //mSockets.reserve(Socket::MAX_SOCKETS);

    mRSet.reserve(Socket::MAX_SOCKETS);
    mWSet.reserve(Socket::MAX_SOCKETS);
    mXSet.reserve(Socket::MAX_SOCKETS);
}

ConnectionManager::~ConnectionManager()
{
    Socket::CleanUp();
}

void ConnectionManager::Register(Socket socket, uint64_t threadId)
{
    if (threadId == GetTID())
    {
        mSockets[socket.GetFD()].reset(new Socket(socket));
        if (nullptr != mStrategy) mStrategy->onClientConnected(static_cast<int>(mSockets.size()));
    }
    else
    {
        mLock.Lock();
        mRegisterBuffer.push_back(socket);
        mLock.Unlock();
    }
}

void ConnectionManager::Unregister(Socket socket, uint64_t threadId)
{
    if (threadId == GetTID())
    {
        auto it = mSockets.find(socket.GetFD());
        if (it != mSockets.end())
        {
            mSockets.erase(it);
        }
        if (nullptr != mStrategy) mStrategy->onClientDisconnected(static_cast<int>(mSockets.size()));
    }
    else
    {
        mLock.Lock();
        mUnregisterBuffer.push_back(socket);
        mLock.Unlock();
    }
}

size_t ConnectionManager::Join()
{
    mLock.Lock();
    const auto tid = GetTID();
    for (auto it = mRegisterBuffer.begin(); it != mRegisterBuffer.end(); ++it)
    {
        Register(*it, tid);
    }
    for (auto it = mUnregisterBuffer.begin(); it != mUnregisterBuffer.end(); ++it)
    {
        Unregister(*it, tid);
    }
    const auto cnt = mRegisterBuffer.size() + mUnregisterBuffer.size();
    mRegisterBuffer.clear();
    mUnregisterBuffer.clear();
    mLock.Unlock();
    return cnt;
}

bool ConnectionManager::Select(uint64_t nanoSec)
{
    if (0 == mSockets.size()) return false;

    mRSet.clear();mWSet.clear();mXSet.clear();

    // fill
    for (auto it = mSockets.begin(); it != mSockets.end(); ++it)
    {
        Socket& s = *(it->second);
        if (0 == s.GetFD() || Socket::INVALID == s.GetFD()) continue;

        if (Socket::READ & s.GetState()) mRSet.push_back(s.GetFD());
        if (Socket::WRITE & s.GetState()) mWSet.push_back(s.GetFD());
        mXSet.push_back(s.GetFD());
    }

    if (false == Socket::Select(nanoSec == FOREVER ? nullptr : &nanoSec, mRSet, mWSet, mXSet))
        return false;

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

    return true;
}

Socket* ConnectionManager::FindByFD(uintptr_t fd)
{
    auto it = mSockets.find(fd);
    if (it == mSockets.end()) return nullptr;

    return it->second.get();
}

void ConnectionManager::List(std::vector<uint64_t>& ids) const
{
    for (auto it = mSockets.begin(); it != mSockets.end(); ++it)
    {
        if (it->second->GetConnInfo().IsValid())
            ids.push_back(it->second->GetFD());
    }
}

Socket* ConnectionManager::FindById(uintptr_t id)
{
    return FindByFD(id);
}

void ConnectionManager::InstallStrategy(ConnectionManagerStrategy* strategy)
{
    mStrategy = strategy;
    if (nullptr != mStrategy) mStrategy->onStrategyInstalled(static_cast<int>(mSockets.size()));
}

uint64_t ConnectionManager::GetTID()
{
    return SystemSocket::GetThreadId();
}
