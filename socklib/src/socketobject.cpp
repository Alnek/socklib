#include "socketobject.h"

#include "socketcallback.h"

#define SOCKLIB_IPV6 1
#define FD_SETSIZE  10000

#include <assert.h>
#include <ws2tcpip.h>

SystemSocket::SystemSocket()
    : fd(0)
    , callback(nullptr)
    , state(0)
{}

SystemSocket::~SystemSocket()
{
    if (0 != fd && INVALID_SOCKET != fd) closesocket(fd);
}

void SystemSocket::Create()
{
#if SOCKLIB_IPV6
    fd = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
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
}

bool SystemSocket::Bind(const char* addr, uint16_t port)
{
#if SOCKLIB_IPV6
    SOCKADDR_IN6 sockAddr = { 0 };
    sockAddr.sin6_port = htons(port);
    sockAddr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, addr, &(sockAddr.sin6_addr));
#else
    SOCKADDR_IN sockAddr;
    sockAddr.sin_port = htons(port);
    sockAddr.sin_family = AF_INET;
    inet_pton(AF_INET, addr, &(sockAddr.sin_addr));
#endif
    int r = bind(fd, (LPSOCKADDR)&sockAddr, sizeof(sockAddr));
    assert(0 == r && "failed to bind socket");
    return 0 == r;
}

bool SystemSocket::Listen()
{
    int r = listen(fd, SOMAXCONN);
    return 0 == r;
}

Socket SystemSocket::Accept()
{
#if SOCKLIB_IPV6
    SOCKADDR_IN6 sockAddr;
#else
    SOCKADDR_IN sockAddr;
#endif
    int len = sizeof(sockAddr);
    SOCKET acceptFd = accept(fd, (LPSOCKADDR)&sockAddr, &len);

    ConnectionInfo conInfo;
    if (INVALID_SOCKET != acceptFd)
    {
#if SOCKLIB_IPV6
        inet_ntop(AF_INET6, &sockAddr.sin6_addr, conInfo.addr, sizeof(conInfo.addr));
        conInfo.port = ntohs(sockAddr.sin6_port);
#else
        inet_ntop(AF_INET, &sockAddr.sin_addr, conInfo.addr, sizeof(conInfo.addr));
        conInfo.port = ntohs(sockAddr.sin_port);
#endif
    }

    Socket s(acceptFd);
    s.mSystemSocket->conInfo = conInfo;
    return s;
}

bool SystemSocket::Recv(std::vector<char>& buffer)
{
    char buf[1024];
    int r = recv(fd, buf, sizeof(buf), 0);
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

bool SystemSocket::Send(const std::vector<char>& buffer)
{
    int r = send(fd, &buffer[0], static_cast<int>(buffer.size()), 0);
    if (r <= 0)
    {
        return false;
    }
    return true;
}

void SystemSocket::Shutdown()
{
    shutdown(fd, SD_BOTH);
}

void SystemSocket::Close()
{
    closesocket(fd);
    fd = 0;
}
