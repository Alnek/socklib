#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <memory>
#include <vector>

class SocketCallback;
class SystemSocket;
struct ConnectionInfo;

class Socket
{
public:
    Socket();
    //Socket(const Socket&); //default is ok?
    //Socket& operator=(const Socket&);
    ~Socket();

    bool Bind(const char* addr, uint16_t port);
    bool Listen();
    Socket Accept();

    bool Recv(std::vector<char>& buffer);
    bool Send(const std::vector<char>& buffer);
    void Shutdown();
    void Close();

    void AsyncRead();
    void AsyncWrite();
    void CancelAsync();
    void SetCallback(SocketCallback* callback);

    const ConnectionInfo& GetConnInfo() const;

    bool operator==(const Socket& rhs) const;

private:

    friend class ConnectionManager;
    friend class SystemSocket;

    static const uint32_t MAX;
    static const uintptr_t INVALID;
    static const int READ = 0x01;
    static const int WRITE = 0x02;

    static void Init();
    static void CleanUp();
    static bool Select(uint64_t* nanoSec, std::vector<uintptr_t>& rSet, std::vector<uintptr_t>& wSet, std::vector<uintptr_t>& xSet);

    Socket(uintptr_t s);

    uintptr_t GetFD() const;
    int GetState() const;
    SocketCallback* GetCallback();
    void CancelAsyncRead();
    void CancelAsyncWrite();

    std::shared_ptr<SystemSocket> mSystemSocket;
};

#endif
