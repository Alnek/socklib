#ifndef __ECHO_PROGRAMM_H__
#define __ECHO_PROGRAMM_H__

#include "socket.h"
#include "socketcallback.h"

#include <vector>

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
    std::vector<char> mRecvBuffer;
    std::vector<char> mSendBuffer;

    Socket mSocket;
};

#endif
