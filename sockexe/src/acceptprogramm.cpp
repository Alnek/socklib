#include "acceptprogramm.h"

#include "instancemanager.h"
#include "console.h"

AcceptProgramm::AcceptProgramm()
{
    Console::GetInstance().Print("AcceptProgramm");
}

AcceptProgramm::~AcceptProgramm()
{
    Console::GetInstance().Print("~AcceptProgramm");
}

void AcceptProgramm::CanRead(Socket socket)
{
    Socket s = socket.Accept();
    if (false == s.GetConnInfo().IsValid())
        return;

    InstanceManager::GetInstance().StartProgramm(s);
}

void AcceptProgramm::ExFunc(Socket socket)
{
    //
}
