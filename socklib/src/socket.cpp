#include "socket.h"

#include "connection.h"
#include "connectionmanager.h"
#include "socketcallback.h"
#include "socketobject.h"

Socket::Socket()
    : mSystemSocket(new SystemSocket)
{
    mSystemSocket->Create();
    ConnectionManager::GetInstance().Register(mSystemSocket);
}

Socket::Socket(const Socket& socket) //default is ok
{
    mSystemSocket = socket.mSystemSocket;
    auto refCount = mSystemSocket.use_count();
    refCount = refCount;
}

Socket::Socket(uintptr_t s)
    : mSystemSocket(new SystemSocket)
{
    mSystemSocket->fd = s;
    ConnectionManager::GetInstance().Register(mSystemSocket);
}

Socket::~Socket()
{
    const auto refCount = mSystemSocket.use_count();
    if (refCount <= 2) // one for this and one for ConnectionManager;
    {
        ConnectionManager::GetInstance().Unregister(mSystemSocket);
    }
}

void Socket::AsyncRead()
{
    mSystemSocket->state = Socket::READ;
}

void Socket::AsyncWrite()
{
    mSystemSocket->state = Socket::WRITE;
}

void Socket::CancelAsync()
{
    mSystemSocket->state = 0;
}

void Socket::SetCallback(SocketCallback* callback)
{
    mSystemSocket->callback.reset(callback);
}

const ConnectionInfo& Socket::GetConnInfo() const
{
    return mSystemSocket->conInfo;
}

bool Socket::Bind(const char* addr, uint16_t port)
{
    return mSystemSocket->Bind(addr, port);
}

bool Socket::Listen()
{
    return mSystemSocket->Listen();
}

Socket Socket::Accept()
{
    return mSystemSocket->Accept();
}

bool Socket::Recv(std::vector<char>& buffer)
{
    return mSystemSocket->Recv(buffer);
}

bool Socket::Send(const std::vector<char>& buffer)
{
    return mSystemSocket->Send(buffer);
}

void Socket::Shutdown()
{
    mSystemSocket->Shutdown();
    AsyncRead();
}

void Socket::Close()
{
    mSystemSocket->Close();
}
