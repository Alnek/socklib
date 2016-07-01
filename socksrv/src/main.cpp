#include "acceptprogramm.h"
#include "connectionmanager.h"
#include "console.h"
#include "processmanager.h"
#include "socket.h"

#include <windows.h>
#include <sstream>

bool runFlag = true;

void Exit(const std::vector<std::string>&, void*)
{
    LOG("shutting down...");
    runFlag = false;
}

void ShutdownConnection(const std::vector<std::string>& tokens, void*)
{
    if (tokens.size() < 2)
    {
        LOG("shutdown: missing ID");
        return;
    }

    uint64_t id = _atoi64(tokens[1].c_str());
    Socket* socket = ConnectionManager::GetInstance().FindById(id);
    if (nullptr == socket)
    {
        LOG("shutdown: failed to find socket with ID=" << id);
        return;
    }

    //socket->SetCallback(nullptr);
    socket->Close();
}

void ListConnections(const std::vector<std::string>&, void*)
{
    std::vector<uint64_t> ids;
    ConnectionManager::GetInstance().List(ids);

    std::stringstream ss;
    for (auto it = ids.begin(); it != ids.end(); ++it)
    {
        ss << *it << " ";
    }
    std::string message = ss.str();
    LOG("list: " << message);
    LOG("list: size = " << ids.size());
}

DWORD WINAPI run(LPVOID ptr)
{
    while (runFlag)
    {
        Sleep(1);
        Console::GetInstance().Process();
    }
    return 0;
}

class AcceptStrategy : public ConnectionManagerStrategy
{
    const int MAX_CLIENTS = Socket::MAX_SOCKETS;
public:
    AcceptStrategy()
        : mServerSocket(nullptr)
    {}

    virtual void onStrategyInstalled(int totalClients)
    {
        StartServer();
    }

    virtual void onClientConnected(int totalClients)
    {
        if (totalClients >= MAX_CLIENTS)
            StopServer();
    }

    virtual void onClientDisconnected(int totalClients)
    {
        if (totalClients < MAX_CLIENTS)
            StartServer();
    }

private:
    void StartServer()
    {
        if (nullptr == mServerSocket)
        {
            mServerSocket = new Socket;
            mServerSocket->SetCallback(new AcceptProgramm(*mServerSocket));
        }
    }
    void StopServer()
    {
        if (nullptr != mServerSocket)
        {
            mServerSocket->SetCallback(nullptr);
            delete mServerSocket;
            mServerSocket = nullptr;
        }
    }

    Socket* mServerSocket;
};

int main()
{
    Console::GetInstance().BindAction("exit", Exit);
    Console::GetInstance().BindAction("shutdown", ShutdownConnection);
    Console::GetInstance().BindAction("list", ListConnections);

    DWORD threadId;
    HANDLE thread = CreateThread(nullptr, 0, run, nullptr, 0, &threadId);

    //AcceptStrategy as;
    ConnectionManager& cm = ConnectionManager::GetInstance();
    cm.UpdateJoinTID();
    ProcessManager& pm = ProcessManager::GetInstance();
    pm.UpdateJoinTID();
    //cm.InstallStrategy(&as);
    Socket s;
    AcceptProgramm ap(s);

    while (runFlag)
    {
        pm.Join();
        pm.Run(100);

        cm.Join();
        if (false == cm.Select(1000))
            Sleep(1);

        Console::GetInstance().ProcessQueue();
    }
    ap.Stop();
    cm.Shutdown();

    WaitForSingleObject(thread, INFINITE);
    Sleep(200);
    return 0;
}
