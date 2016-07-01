#include "connectionmanager.h"
#include "simpleprogramm.h"
#include "socket.h"

#include <iostream>
#include <windows.h>

const int n = 10000;
int succes = 0;

void create_connection()
{
    Socket client;
    if (true == client.Connect("127.0.0.1", 7788))
    {
        client.SetCallback(new SimpleProgramm(client));
        succes++;
        //std::cout << "success = " << succes << std::endl;
    }
    else
    {
        std::cout << "Failed" << std::endl;
    }
}

DWORD WINAPI run(LPVOID ptr)
{
    while (succes <= n)
    {
        create_connection();
    }
    return 0;
}

int main()
{
    ConnectionManager& cm = ConnectionManager::GetInstance();
    cm.UpdateJoinTID();

    while (0 == succes)
    {
        create_connection();
    }
    std::cout << "success = " << succes << std::endl;

    DWORD threadId;
    HANDLE thread = CreateThread(nullptr, 0, run, nullptr, 0, &threadId);

    while(true == cm.Select(0))
    {
        cm.Join();
        Sleep(1);
        SimpleProgramm::Report();
    }

    cm.Shutdown();
    return 0;
}
