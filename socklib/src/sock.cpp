#include "sock.h"

#include <winsock.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SystemSocket
{
    SOCKET fd;
    Socket* ptr;

    SystemSocket()
        : fd(0)
        , ptr(nullptr)
    {}
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
    : mSockets(FD_SETSIZE)
    , mRSet(nullptr)
    , mWSet(nullptr)
    , mXSet(nullptr)
{
    mRSet = new fd_set;
    mWSet = new fd_set;
    mXSet = new fd_set;
    memset(mRSet, 0, sizeof(fd_set));
    memset(mWSet, 0, sizeof(fd_set));
    memset(mXSet, 0, sizeof(fd_set));
}

SocketSystem::~SocketSystem()
{
    delete mRSet;
    delete mWSet;
    delete mXSet;
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

Socket SocketSystem::CreateSocket()
{
    return Socket();
}

void SocketSystem::Select()
{
    fd_set rset = *mRSet;
    fd_set wset = *mWSet;
    fd_set xset = *mXSet;

    timeval t = { 0 };
    int r = select(0, &rset, &wset, &xset, &t);
    if (r > 0 && (rset.fd_count > 0 || wset.fd_count > 0 || xset.fd_count > 0))
    {
        //Socket s = Accept();
    }
    else if (SOCKET_ERROR == r)
    {
        r = r;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Socket::Socket()
    : mSockInfo(new SystemSocket)
{
    mSockInfo->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

Socket::~Socket()
{
    //
}

Socket::Socket(uintptr_t s)
    : mSockInfo(new SystemSocket)
{
    mSockInfo->fd = s;
}

bool Socket::Bind()
{
    SOCKADDR_IN sockAddr;
    sockAddr.sin_port = htons(7788);
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int r = bind(mSockInfo->fd, (LPSOCKADDR)&sockAddr, sizeof(sockAddr));
    return 0 == r;
}

bool Socket::Listen()
{
    int r = listen(mSockInfo->fd, SOMAXCONN);
    return 0 == r;
}

Socket Socket::Accept()
{
    SOCKADDR_IN sockAddr;
    int len = sizeof(sockAddr);

    SOCKET fd = accept(mSockInfo->fd, (LPSOCKADDR)&sockAddr, &len);

    return Socket(fd);
}
