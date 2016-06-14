#include "socketcallback.h"

#include "sock.h"

Socket SocketCallback::GetSocket(const SocketCallback* programm) const
{
    std::shared_ptr<SystemSocket> socket = SocketSystem::GetInstance().FindByCallback(programm);
    return Socket(socket);
}
