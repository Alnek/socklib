#include "echoprogramm.h"

#include "instancemanager.h"
#include "console.h"
#include "spamprogramm.h"

EchoProgramm::EchoProgramm(Socket& socket)
    : mSocket(socket)
{
    LOG("EchoProgramm");

    mRecvBuffer.reserve(1024);
    mSendBuffer.reserve(1024);

    const char hello[] = "hello\n";
    mSendBuffer.insert(mSendBuffer.end(), &hello[0], &hello[sizeof(hello)]);
    mSocket.AsyncWrite();
}

EchoProgramm::~EchoProgramm()
{
    LOG("~EchoProgramm");
}

void EchoProgramm::DoRecv()
{
    if (true == mSocket.Recv(mRecvBuffer))
    {
        mSendBuffer.insert(mSendBuffer.end(), mRecvBuffer.begin(), mRecvBuffer.end());
        mRecvBuffer.clear();

        if (mSendBuffer.end() == std::find(mSendBuffer.begin(), mSendBuffer.end(), '\r'))
        {
            mSocket.AsyncRead();
        }
        else
        {
            mSocket.AsyncWrite();
        }
    }
    else
    {
        InstanceManager::GetInstance().StopProgramm(mSocket);
    }
}

void EchoProgramm::DoSend()
{
    if (true == mSocket.Send(mSendBuffer))
    {
        mSendBuffer.clear();
        mSocket.AsyncRead();
    }
    else
    {
        InstanceManager::GetInstance().StopProgramm(mSocket);
    }
}

void EchoProgramm::HandleError()
{
    InstanceManager::GetInstance().StopProgramm(mSocket);
}
