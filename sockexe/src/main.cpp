#include "sock.h"

#include <iostream>
#include <string>

#include "windows.h"

bool runFlag = true;

DWORD WINAPI run(LPVOID ptr)
{
    while (runFlag)
    {
        std::string command;
        std::getline(std::cin, command);

        if ("exit" == command) runFlag = false;
    }

    return 0;
}

int main()
{
    DWORD threadId;
    HANDLE h = CreateThread(nullptr, 0, run, nullptr, 0, &threadId);

    SocketSystem& ssys = SocketSystem::GetInstance();
    Socket s = ssys.CreateSocket();
    s.Bind();
    s.Listen();

    while (runFlag)
    {
        ssys.Select();
        Sleep(0);
    }

    ssys.ShutDown();

    return 0;
}
