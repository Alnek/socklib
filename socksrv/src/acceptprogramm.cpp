#include "acceptprogramm.h"

#include "instancemanager.h"
#include "console.h"
#include "connection.h"

#include "stdallocator.h"

AcceptProgramm::AcceptProgramm(Socket& socket)
    : mSocket(socket)
{
    Console::GetInstance().Print("AcceptProgramm");

    //mSocket.Bind("0.0.0.0", 7788);
    mSocket.Bind("127.0.0.1", 7788);
    mSocket.Listen();
    //mSocket.AsyncRead(); do not use select

    std::vector<char, mmap_allocator<char> > test;

    mThread = CreateThread(nullptr, 0, run, this, 0, &mThreadId);
}

AcceptProgramm::~AcceptProgramm()
{
    Console::GetInstance().Print("~AcceptProgramm");
}

void AcceptProgramm::DoRecv()
{
    mSocket.AsyncRead();

    Socket s = mSocket.Accept();
    if (false == s.GetConnInfo().IsValid())
        return;

    InstanceManager::GetInstance().StartProgramm(s);
}

void AcceptProgramm::HandleError()
{
    mSocket.SetCallback(nullptr);
}

void AcceptProgramm::Run()
{
    while (true)
    {
        Socket s = mSocket.Accept();
        if (false == s.GetConnInfo().IsValid())
            return;

        InstanceManager::GetInstance().StartProgramm(s);
    }
}

void AcceptProgramm::Stop()
{
    mSocket.Close();
    
    //TerminateThread();
}

DWORD WINAPI AcceptProgramm::run(LPVOID ptr)
{
    AcceptProgramm* instance = static_cast<AcceptProgramm*>(ptr);
    instance->Run();
    return 0;
}
