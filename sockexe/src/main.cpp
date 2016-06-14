#include "sock.h"

#include "acceptprogramm.h"
#include "console.h"
#include "processmanager.h"

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
    //ProcessManager::GetInstance().List(ids);

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

    SocketSystem& ssys = SocketSystem::GetInstance();
    Socket s;
    s.Bind("0.0.0.0", 7788);
    s.Listen();
    s.SetCallback(new AcceptProgramm);
    s.SetState(Socket::READ);
    ssys.Include(s);

    while (runFlag)
    {
        ProcessManager::GetInstance().Run(120);
        ssys.Select();
    }

    ssys.Exclude(s);
    s.Close();
    ssys.ShutDown();
    WaitForSingleObject(thread, INFINITE);
    s.SetCallback(nullptr);
    Sleep(1000);
    return 0;
}
