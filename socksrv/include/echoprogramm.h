#ifndef __ECHO_PROGRAMM_H__
#define __ECHO_PROGRAMM_H__

#include "socket.h"
#include "socketcallback.h"

#include <string>

class EchoProgramm
    : public SocketCallback
{
public:
    virtual void DoRecv() override;
    virtual void DoSend() override;
    virtual void HandleError() override;

    EchoProgramm(Socket& socket);
    virtual ~EchoProgramm();

private:
    std::string mRecvBuffer;
    std::string mSendBuffer;

    Socket mSocket;
};

#endif
