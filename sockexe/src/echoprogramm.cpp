#include "echoprogramm.h"

#include "instancemanager.h"
#include "console.h"

EchoProgramm::EchoProgramm()
{
    Console::GetInstance().Print("EchoProgramm");

    mRecvBuffer.reserve(1024);
    mSendBuffer.reserve(1024);

    ProcessManager::GetInstance().Register(this);
}

EchoProgramm::~EchoProgramm()
{
    ProcessManager::GetInstance().Unregister(this);

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

void EchoProgramm::Run()
{
    if (false == mSendBuffer.empty())
        return;

    char buf[10*1024];
    for (auto i = 0; i != sizeof(buf); ++i)
    {
        buf[i] = 'a' + rand() % ('a' - 'z');
    }
    buf[sizeof(buf) - 1] = 0;

    mSendBuffer.insert(mSendBuffer.end(), &buf[0], &buf[sizeof(buf)]);
    GetSocket(this).SetState(Socket::READ | Socket::WRITE);
}
