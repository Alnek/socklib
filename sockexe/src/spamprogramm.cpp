#include "spamprogramm.h"

#include "instancemanager.h"
#include "console.h"

SpamProgramm::SpamProgramm()
{
    Console::GetInstance().Print("SpamProgramm");

    mRecvBuffer.reserve(1024);
    mSendBuffer.reserve(1024);

    ProcessManager::GetInstance().Register(this);
}

SpamProgramm::~SpamProgramm()
{
    ProcessManager::GetInstance().Unregister(this);

    Console::GetInstance().Print("~SpamProgramm");
}

void SpamProgramm::CanWrite(Socket socket)
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

void SpamProgramm::CanRead(Socket socket)
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

void SpamProgramm::ExFunc(Socket socket)
{
    InstanceManager::GetInstance().StopProgramm(socket);
}

void SpamProgramm::Run()
{
    if (false == mSendBuffer.empty())
        return;

    char buf[256];
    for (auto i = 0; i != sizeof(buf); ++i)
    {
        buf[i] = 'a' + rand() % ('a' - 'z');
    }
    buf[sizeof(buf) - 1] = 0;

    mSendBuffer.insert(mSendBuffer.end(), &buf[0], &buf[sizeof(buf)]);
    GetSocket(this).SetState(Socket::READ | Socket::WRITE);
}
