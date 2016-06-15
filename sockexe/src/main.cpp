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
    //Runnable* runnable = ProcessManager::GetInstance().FindById(id);
    //if (nullptr == runnable)
    {
        LOG("shutdown: failed to find programm with ID=" << id);
        return;
    }

    //runnable->Shutdown();
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
        Sleep(0);
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

    //Socket server;
    //server.SetCallback(new AcceptProgramm(server));
    new AcceptProgramm;

    while (runFlag)
    {
        ProcessManager::GetInstance().Run(120);
        cm.Select(100);
    }

    //server.SetCallback(nullptr);
    //server.Close();
    cm.Shutdown();

    WaitForSingleObject(thread, INFINITE);
    Sleep(1000);
    return 0;
}
