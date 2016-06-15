#include "acceptprogramm.h"

#include "instancemanager.h"
#include "console.h"
#include "connection.h"

AcceptProgramm::AcceptProgramm()
    //: mSocket(socket)
{
    Console::GetInstance().Print("AcceptProgramm");

    mSocket.Bind("0.0.0.0", 7788);
    mSocket.Listen();
    mSocket.SetCallback(this);
    mSocket.AsyncRead();
}

AcceptProgramm::~AcceptProgramm()
{
    Console::GetInstance().Print("~AcceptProgramm");
}

void AcceptProgramm::DoRecv()
{
    Socket s = mSocket.Accept();
    if (false == s.GetConnInfo().IsValid())
        return;

    InstanceManager::GetInstance().StartProgramm(s);

    //mSocket.SetCallback(nullptr);
    mSocket.AsyncRead();
}

void AcceptProgramm::HandleError()
{
    mSocket.SetCallback(nullptr);
}
