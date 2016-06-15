#include "socketobject.h"

#define SOCKLIB_IPV6 1
#define FD_SETSIZE  10000

#include <assert.h>
#include <ws2tcpip.h>

const uint32_t SystemSocket::MAX = FD_SETSIZE;
const uintptr_t SystemSocket::INVALID = INVALID_SOCKET;

void SystemSocket::Init()
{
    WSAData wsaData;
    WORD wVersionRequested = MAKEWORD(2, 0);
    int r = WSAStartup(wVersionRequested, &wsaData);
    assert(0 == r && "failed to Startup sockets");
}

void SystemSocket::CleanUp()
{
    WSACleanup();
}

bool SystemSocket::Select(uint64_t* nanoSec, std::vector<uintptr_t>& rSet, std::vector<uintptr_t>& wSet, std::vector<uintptr_t>& xSet)
{
    if (0 == rSet.size() && 0 == wSet.size() && 0 == xSet.size()) return false;

    fd_set rset;
    fd_set wset;
    fd_set xset;
    rset.fd_count = wset.fd_count = xset.fd_count = 0;

    for (auto it = rSet.begin(); it != rSet.end(); ++it)
    {
        rset.fd_array[rset.fd_count++] = *it;
    }
    for (auto it = wSet.begin(); it != wSet.end(); ++it)
    {
        wset.fd_array[wset.fd_count++] = *it;
    }
    for (auto it = xSet.begin(); it != xSet.end(); ++it)
    {
        xset.fd_array[xset.fd_count++] = *it;
    }

    rSet.clear(); wSet.clear(); xSet.clear();

    timeval t = { 0 };
    if (nullptr != nanoSec)
    {
        t.tv_sec = static_cast<uint32_t>(*nanoSec / 1000000);
        t.tv_usec = static_cast<uint32_t>(*nanoSec % 1000000);
    }
    int r = select(0
        , 0 == rset.fd_count ? nullptr : &rset
        , 0 == wset.fd_count ? nullptr : &wset
        , 0 == xset.fd_count ? nullptr : &xset
        , nullptr == nanoSec ? nullptr : &t);

    if (SOCKET_ERROR == r)
    {
        assert(0 && "unexpected SOCKET_ERROR");
        return false;
    }
    else if (r > 0 && rset.fd_count > 0)
    {
        for (u_int i = 0; i != rset.fd_count; ++i)
        {
            rSet.push_back(rset.fd_array[i]);
        }
    }
    else if (r > 0 && wset.fd_count > 0)
    {
        for (u_int i = 0; i != wset.fd_count; ++i)
        {
            wSet.push_back(wset.fd_array[i]);
        }
    }
    else if (r > 0 && xset.fd_count > 0)
    {
        for (u_int i = 0; i != xset.fd_count; ++i)
        {
            xSet.push_back(xset.fd_array[i]);
        }
    }
    return true;
}

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

    Socket result(acceptFd);
    result.mSystemSocket->conInfo = conInfo;
    return result;
}

bool SystemSocket::Recv(std::vector<char>& buffer)
{
    char buf[100*1024];
    int r = recv(fd, buf, sizeof(buf), 0);
    if (r <= 0)
    {
        return false;
    }
    else
    {
        //buffer.capacity()
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
    if (r < buffer.size())
    {
        assert(0 && "failed to send whole buffer");
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
