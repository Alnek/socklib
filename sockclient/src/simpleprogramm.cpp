#include "simpleprogramm.h"

#include <iostream>
#include <windows.h>

uint64_t SimpleProgramm::sCounter = 0;
uint64_t SimpleProgramm::sPrevTick = GetTickCount();

SimpleProgramm::SimpleProgramm(Socket& socket)
    : mSocket(socket)
{
    //Console::GetInstance().Print("SimpleProgramm");

    mRecvBuffer.reserve(64*1024);
    //mSendBuffer.reserve(1024*1024);

    mSocket.AsyncRead();
}

SimpleProgramm::~SimpleProgramm()
{
    //std::cout << "~SimpleProgramm" << std::endl;
    //Console::GetInstance().Print("~SimpleProgramm");
}

void SimpleProgramm::DoRecv()
{
    if (false == mSocket.Recv(mRecvBuffer))
    {
        mSocket.SetCallback(nullptr);
    }
    else
    {
        sCounter += mRecvBuffer.size();
        mRecvBuffer.clear();
        mSocket.AsyncRead();
    }
}

void SimpleProgramm::DoSend()
{
    if (true == mSocket.Send(mSendBuffer))
    {
        mSendBuffer.clear();
    }
    else
    {
        mSocket.SetCallback(nullptr);
    }
}

void SimpleProgramm::HandleError()
{
    mSocket.SetCallback(nullptr);
}

void SimpleProgramm::Report()
{
    uint64_t delta = GetTickCount() - sPrevTick;
    if (delta > 333)
    {
        std::cout << "BPS: " << (1000 * sCounter) / (delta * 1024) << " KB/sec" << std::endl;
        sCounter = 0;
        sPrevTick += delta;
    }
}
