#include "sock.h"

#include "connection.h"
#include "socket.h"
#include "socketcallback.h"
#include "socketobject.h"

#define SOCKLIB_IPV6 1
#define FD_SETSIZE  10000

#include <ws2tcpip.h>

//template<int s> struct CompilePrint;
//CompilePrint<sizeof(std::shared_ptr<SystemSocket>)> _;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SocketSystem* SocketSystem::sInstance = nullptr;

SocketSystem& SocketSystem::GetInstance()
{
    if (nullptr == sInstance)
    {
        sInstance = new SocketSystem();
        sInstance->Init();
    }

    return *sInstance;
}

SocketSystem::SocketSystem()
    : mSockets()
{
    mSockets.reserve(FD_SETSIZE);
}

SocketSystem::~SocketSystem()
{
    //
}

void SocketSystem::Init()
{
    WSAData wsaData;
    WORD wVersionRequested = MAKEWORD(2, 0);
    int r = WSAStartup(wVersionRequested, &wsaData);
    r = r;
}

void SocketSystem::ShutDown()
{
    WSACleanup();

    delete sInstance;
    sInstance = nullptr;
}
/*
std::shared_ptr<SystemSocket> SocketSystem::FindByFD(uintptr_t fd) const
{
    for (auto it = mSockets.begin(); it != mSockets.end(); ++it)
    {
        if ((*it)->fd == fd)
        {
            return (*it);
        }
    }

    return nullptr;
}

std::shared_ptr<SystemSocket> SocketSystem::FindByCallback(const SocketCallback* ptr) const
{
    for (auto it = mSockets.begin(); it != mSockets.end(); ++it)
    {
        if ((*it)->callback.get() == ptr)
        {
            return (*it);
        }
    }

    return nullptr;
}
*/
void SocketSystem::Select()
{
    if (0 == mSockets.size()) return;

    fd_set rset;
    fd_set wset;
    fd_set xset;
    rset.fd_count = wset.fd_count = xset.fd_count = 0;
    for (size_t i = 0; i != mSockets.size(); ++i)
    {
        SystemSocket& ss = *mSockets[i];
        if (Socket::READ & ss.state) rset.fd_array[rset.fd_count++] = ss.fd;
        if (Socket::WRITE & ss.state) wset.fd_array[wset.fd_count++] = ss.fd;
        xset.fd_array[xset.fd_count++] = ss.fd;
    }

    timeval t = { 0 };
    t.tv_sec = 0;
    t.tv_usec = 100;
    int r = select(0, &rset, &wset, &xset, &t);
    //int r = select(0, &rset, &wset, &xset, nullptr);
    if (r > 0 && xset.fd_count > 0)
    {
        for (u_int i = 0; i != xset.fd_count; ++i)
        {
            SOCKET fd = xset.fd_array[i];
            std::shared_ptr<SystemSocket> ss = FindByFD(fd);
            ss->callback->ExFunc(ss);
        }
    }
    else if (r > 0 && rset.fd_count > 0)
    {
        for (u_int i = 0; i != rset.fd_count; ++i)
        {
            SOCKET fd = rset.fd_array[i];
            std::shared_ptr<SystemSocket> ss = FindByFD(fd);
            ss->callback->CanRead(ss);
        }
    }
    else if (r > 0 && wset.fd_count > 0)
    {
        for (u_int i = 0; i != wset.fd_count; ++i)
        {
            SOCKET fd = wset.fd_array[i];
            std::shared_ptr<SystemSocket> ss = FindByFD(fd);
            ss->callback->CanWrite(ss);
        }
    }
    else if (SOCKET_ERROR == r)
    {
        r = r;
    }
}

void SocketSystem::Include(Socket& socket)
{
    mSockets.push_back(socket.mSystemSocket);
}

void SocketSystem::Exclude(Socket& socket)
{
    for (auto it = mSockets.begin(); it != mSockets.end(); ++it)
    {
        std::shared_ptr<SystemSocket>& cur = *it;
        if (cur->fd == socket.mSystemSocket->fd)
        {
            mSockets.erase(it);
            break;
        }
    }
}
