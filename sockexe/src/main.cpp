#include "sock.h"

#include "acceptprogramm.h"
#include "console.h"
#include "processmanager.h"

#include <windows.h>

bool runFlag = true;

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
    DWORD threadId;
    HANDLE thread = CreateThread(nullptr, 0, run, nullptr, 0, &threadId);

    SocketSystem& ssys = SocketSystem::GetInstance();
    Socket s = ssys.CreateSocket();
    //s.Bind("::0", 7788);
    s.Bind("0.0.0.0", 7788);
    s.Listen();
    s.SetCallback(new AcceptProgramm);
    s.SetState(Socket::READ);
    ssys.Include(s);

    while (runFlag)
    {
        ProcessManager::GetInstance().Run();
        ssys.Select();
        Sleep(0);
    }

    ssys.Exclude(s);
    s.Close();
    ssys.ShutDown();
    WaitForSingleObject(thread, INFINITE);
    s.SetCallback(nullptr);
    Sleep(1000);
    return 0;
}
