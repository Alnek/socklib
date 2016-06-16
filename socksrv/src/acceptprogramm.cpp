#include "acceptprogramm.h"

#include "instancemanager.h"
#include "console.h"
#include "connection.h"

#include "stdallocator.h"

AcceptProgramm::AcceptProgramm(Socket& socket)
    : mSocket(socket)
{
    Console::GetInstance().Print("AcceptProgramm");

    mSocket.Bind("0.0.0.0", 7788);
    mSocket.Listen();
    //mSocket.SetCallback(this);
    mSocket.AsyncRead();

    std::vector<char, mmap_allocator<char> > test;
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
