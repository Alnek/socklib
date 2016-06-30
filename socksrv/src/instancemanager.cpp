#include "instancemanager.h"

#include "connection.h"
#include "console.h"
#include "echoprogramm.h"
#include "spamprogramm.h"

InstanceManager& InstanceManager::GetInstance()
{
    static InstanceManager sInstance;
    return sInstance;
}

void InstanceManager::StartProgramm(Socket& socket)
{
    const ConnectionInfo& conInfo = socket.GetConnInfo();
    //LOG("Incoming connection: " << conInfo.addr << ":" << conInfo.port);

    //socket.SetCallback(new EchoProgramm(socket));
    socket.SetCallback(new SpamProgramm(socket));
}

void InstanceManager::StopProgramm(Socket socket)
{
    socket.SetCallback(nullptr);
    socket.Close();

    const ConnectionInfo& conInfo = socket.GetConnInfo();
    //LOG("Closed connection: " << conInfo.addr << ":" << conInfo.port);
}
