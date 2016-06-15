#include "socket.h"

#include "connection.h"
#include "connectionmanager.h"
#include "socketcallback.h"
#include "socketobject.h"

const uint32_t Socket::MAX = SystemSocket::MAX;
const uintptr_t Socket::INVALID = SystemSocket::INVALID;

void Socket::Init() { SystemSocket::Init(); }
void Socket::CleanUp() { SystemSocket::CleanUp(); };
bool Socket::Select(uint64_t* nanoSec, std::vector<uintptr_t>& rSet, std::vector<uintptr_t>& wSet, std::vector<uintptr_t>& xSet)
{
    return SystemSocket::Select(nanoSec, rSet, wSet, xSet);
}

Socket::Socket()
    : mSystemSocket(new SystemSocket)
{
    mSystemSocket->Create();
    ConnectionManager::GetInstance().Register(*this);
}

Socket::Socket(uintptr_t s)
    : mSystemSocket(new SystemSocket)
{
    mSystemSocket->fd = s;
    ConnectionManager::GetInstance().Register(*this);
}

Socket::~Socket()
{
    const auto refCount = mSystemSocket.use_count();
    if (refCount <= 2) // one for this and one for ConnectionManager;
    {
        ConnectionManager::GetInstance().Unregister(*this);
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

uintptr_t Socket::GetFD() const
{
    return mSystemSocket->GetFD();
}

int Socket::GetState() const
{
    return mSystemSocket->state;
}

SocketCallback* Socket::GetCallback()
{
    return mSystemSocket->callback.get();
}

void Socket::CancelAsyncRead()
{
    mSystemSocket->state &= (~Socket::READ);
}

void Socket::CancelAsyncWrite()
{
    mSystemSocket->state &= (~Socket::WRITE);
}

bool Socket::operator==(const Socket& rhs) const
{
    return this->mSystemSocket->fd == rhs.mSystemSocket->fd;
}
