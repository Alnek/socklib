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

int main()
{
    Console::GetInstance().BindAction("exit", Exit);
    Console::GetInstance().BindAction("shutdown", ShutdownConnection);
    Console::GetInstance().BindAction("list", ListConnections);

    DWORD threadId;
    HANDLE thread = CreateThread(nullptr, 0, run, nullptr, 0, &threadId);

    ConnectionManager& cm = ConnectionManager::GetInstance();

    new AcceptProgramm;

    while (runFlag)
    {
        Console::GetInstance().ProcessQueue();
        ProcessManager::GetInstance().Run(120);
        cm.Select(100);
    }

    cm.Shutdown();

    WaitForSingleObject(thread, INFINITE);
    Sleep(1000);
    return 0;
}
