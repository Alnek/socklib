#include "echoprogramm.h"

#include "instancemanager.h"
#include "console.h"

EchoProgramm::EchoProgramm()
{
    Console::GetInstance().Print("EchoProgramm");

    mRecvBuffer.reserve(1024);
    mSendBuffer.reserve(1024);
}

EchoProgramm::~EchoProgramm()
{
    Console::GetInstance().Print("~EchoProgramm");
}

void EchoProgramm::CanWrite(Socket socket)
{
    if (false == socket.Send(mSendBuffer))
    {
        InstanceManager::GetInstance().StopProgramm(socket);
    }
    else
    {
        socket.SetState(Socket::READ);
        mSendBuffer.clear();
    }
}

void EchoProgramm::CanRead(Socket socket)
{
    if (false == socket.Recv(mRecvBuffer))
    {
        InstanceManager::GetInstance().StopProgramm(socket);
    }
    else
    {
        mSendBuffer.insert(mSendBuffer.end(), mRecvBuffer.begin(), mRecvBuffer.end());
        mRecvBuffer.clear();
        socket.SetState(Socket::READ | Socket::WRITE);
    }
}

void EchoProgramm::ExFunc(Socket socket)
{
    InstanceManager::GetInstance().StopProgramm(socket);
}
