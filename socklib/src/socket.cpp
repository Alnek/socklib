#include "socket.h"

#include "connection.h"
#include "socketcallback.h"
#include "socketobject.h"

Socket::Socket()
    : mSystemSocket(new SystemSocket)
{
    mSystemSocket->Create();
}

Socket::Socket(uintptr_t s)
    : mSystemSocket(new SystemSocket)
{
    mSystemSocket->fd = s;
}

Socket::Socket(std::shared_ptr<SystemSocket>& systemSocket)
    : mSystemSocket(systemSocket)
{
    //
}

Socket::~Socket()
{
    //
}

void Socket::SetState(int state)
{
    mSystemSocket->state = state;
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
    SetState(Socket::READ);
}

void Socket::Close()
{
    mSystemSocket->Close();
}
