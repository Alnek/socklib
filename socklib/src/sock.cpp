#include "sock.h"

#define SOCKLIB_IPV6 1
#define FD_SETSIZE  10000

#include <assert.h>
#include <ws2tcpip.h>

//template<int s> struct CompilePrint;
//CompilePrint<sizeof(std::shared_ptr<SystemSocket>)> _;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SystemSocket
{
    SOCKET fd;
    std::shared_ptr<SocketCallback> callback;
    int state;
    ConnectionInfo conInfo;

    SystemSocket()
        : fd(0)
        , callback(nullptr)
        , state(0)
    {}

    ~SystemSocket()
    {
        if (0 != fd && INVALID_SOCKET != fd) closesocket(fd);
    }
};

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

std::shared_ptr<SystemSocket> SocketSystem::FindByCallback(SocketCallback* ptr) const
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

void SocketSystem::ShutDown()
{
    WSACleanup();

    delete sInstance;
    sInstance = nullptr;
}

Socket SocketSystem::CreateSocket()
{
#if SOCKLIB_IPV6
    SOCKET fd = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
    assert(fd != INVALID_SOCKET && "failed to create socket");
    //DWORD opt = FALSE; int len = sizeof(opt);
    //int r = getsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&opt, &len);
    DWORD opt = FALSE;
    int r = setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, (const char*)&opt, sizeof(opt));
    assert(0 == r && "failed to setsockopt");
#else
    SOCKET fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert(fd != INVALID_SOCKET && "failed to create socket");
#endif
    return Socket(fd);
}

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
    t.tv_usec = 1000;
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
    mSockets.push_back(socket.mSockInfo);
}

void SocketSystem::Exclude(Socket& socket)
{
    for (auto it = mSockets.begin(); it != mSockets.end(); ++it)
    {
        std::shared_ptr<SystemSocket>& cur = *it;
        if (cur->fd == socket.mSockInfo->fd)
        {
            mSockets.erase(it);
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Socket::Socket()
//    : mSockInfo(new SystemSocket)
//{
//    mSockInfo->ptr = this;
//}

Socket::Socket(uintptr_t s)
    : mSockInfo(new SystemSocket)
{
    mSockInfo->fd = s;
}

Socket::Socket(std::shared_ptr<SystemSocket>& systemSocket)
    : mSockInfo(systemSocket)
{
    //
}

Socket::~Socket()
{
    //
}

bool Socket::Bind(const char* addr, uint16_t port)
{
#if SOCKLIB_IPV6
    SOCKADDR_IN6 sockAddr = {0};
    sockAddr.sin6_port = htons(port);
    sockAddr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, addr, &(sockAddr.sin6_addr));
#else
    SOCKADDR_IN sockAddr;
    sockAddr.sin_port = htons(port);
    sockAddr.sin_family = AF_INET;
    inet_pton(AF_INET, addr, &(sockAddr.sin_addr));
#endif
    int r = bind(mSockInfo->fd, (LPSOCKADDR)&sockAddr, sizeof(sockAddr));
    assert(0 == r && "failed to bind socket");
    return 0 == r;
}

bool Socket::Listen()
{
    int r = listen(mSockInfo->fd, SOMAXCONN);
    return 0 == r;
}

Socket Socket::Accept()
{
#if SOCKLIB_IPV6
    SOCKADDR_IN6 sockAddr;
#else
    SOCKADDR_IN sockAddr;
#endif
    int len = sizeof(sockAddr);
    SOCKET fd = accept(mSockInfo->fd, (LPSOCKADDR)&sockAddr, &len);

    ConnectionInfo conInfo;
    if (INVALID_SOCKET != fd)
    {
#if SOCKLIB_IPV6
        inet_ntop(AF_INET6, &sockAddr.sin6_addr, conInfo.addr, sizeof(conInfo.addr));
        conInfo.port = ntohs(sockAddr.sin6_port);
#else
        inet_ntop(AF_INET, &sockAddr.sin_addr, conInfo.addr, sizeof(conInfo.addr));
        conInfo.port = ntohs(sockAddr.sin_port);
#endif
    }

    Socket s(fd);
    s.mSockInfo->conInfo = conInfo;
    return s;
}

void Socket::SetState(int state)
{
    mSockInfo->state = state;
}

void Socket::SetCallback(SocketCallback* callback)
{
    mSockInfo->callback.reset(callback);
}

bool Socket::Recv(std::vector<char>& buffer)
{
    char buf[1024];
    int r = recv(mSockInfo->fd, buf, sizeof(buf), 0);
    if (r <= 0)
    {
        return false;
    }
    else
    {
        buffer.insert(buffer.end(), &buf[0], &buf[r]);
    }
    return true;
}

bool Socket::Send(const std::vector<char>& buffer)
{
    int r = send(mSockInfo->fd, &buffer[0], static_cast<int>(buffer.size()), 0);
    if (r <= 0)
    {
        return false;
    }
    return true;
}

void Socket::Shutdown()
{
    shutdown(mSockInfo->fd, SD_BOTH);
}

void Socket::Close()
{
    closesocket(mSockInfo->fd);
    mSockInfo->fd = 0;
}

const ConnectionInfo& Socket::GetConnInfo() const
{
    return mSockInfo->conInfo;
}

Socket SocketCallback::GetSocket(SocketCallback* programm)
{
    std::shared_ptr<SystemSocket> socket = SocketSystem::GetInstance().FindByCallback(programm);
    return Socket(socket);
}
