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

int main()
{
    ConnectionManager& cm = ConnectionManager::GetInstance();

    while (0 == succes)
    {
        create_connection();
    }
    std::cout << "success = " << succes << std::endl;

    while(true == cm.Select(0))
    {
        if (succes < n)
            create_connection();
        Sleep(0);
        SimpleProgramm::Report();
    }

    cm.Shutdown();
    return 0;
}
