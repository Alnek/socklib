#include "connectionmanager.h"
#include "socket.h"

#include <iostream>
#include <windows.h>

int main()
{
    ConnectionManager& cm = ConnectionManager::GetInstance();

    Socket client;

    std::string buffer;
    buffer.reserve(100*1024);
    uint64_t counter = 0;

    client.Connect("127.0.0.1", 7788);
    DWORD prev = GetTickCount();
    while(true)
    {
        if (!client.Recv(buffer)) break;

        counter += buffer.size();
        buffer.clear();

        DWORD delta = GetTickCount() - prev;
        if (delta > 333)
        {
            std::cout << "BPS: " << (1000 * counter)/(delta * 1024) << " KB/sec" << std::endl;
            counter = 0;
            prev += delta;
        }
    }
    
    client.Close();
    cm.Shutdown();
    return 0;
}
