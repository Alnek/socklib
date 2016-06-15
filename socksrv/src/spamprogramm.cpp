#include "spamprogramm.h"

#include "echoprogramm.h"
#include "instancemanager.h"
#include "console.h"

SpamProgramm::SpamProgramm(Socket& socket)
    : mSocket(socket)
{
    Console::GetInstance().Print("SpamProgramm");

    //mRecvBuffer.reserve(1024);
    //mSendBuffer.reserve(1024);

    ProcessManager::GetInstance().Register(this);

    mSocket.CancelAsync();
}

SpamProgramm::~SpamProgramm()
{
    ProcessManager::GetInstance().Unregister(this);

    Console::GetInstance().Print("~SpamProgramm");
}

void SpamProgramm::DoRecv()
{
    mSocket.Recv(mRecvBuffer);
    mRecvBuffer.clear();
    mSocket.AsyncRead();
}

void SpamProgramm::DoSend()
{
    if (true == mSocket.Send(mSendBuffer))
    {
        mSendBuffer.clear();
        mSocket.SetCallback(new EchoProgramm(mSocket));
    }
    else
    {
        InstanceManager::GetInstance().StopProgramm(mSocket);
    }
}

void SpamProgramm::HandleError()
{
    InstanceManager::GetInstance().StopProgramm(mSocket);
}

void SpamProgramm::Run()
{
    if (false == mSendBuffer.empty())
        return;

    char buf[4];
    for (auto i = 0; i != sizeof(buf); ++i)
    {
        buf[i] = 'a' + rand() % ('a' - 'z');
    }
    //buf[sizeof(buf) - 1] = 0;

    mSendBuffer.insert(mSendBuffer.end(), &buf[0], &buf[sizeof(buf)]);
    mSocket.AsyncWrite();
}
