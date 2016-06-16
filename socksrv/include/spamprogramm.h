#ifndef __SPAM_PROGRAMM_H__
#define __SPAM_PROGRAMM_H__

#include "processmanager.h"
#include "socket.h"
#include "socketcallback.h"

#include <string>

class SpamProgramm
    : public SocketCallback
    , private Runnable
{
public:
    virtual void DoRecv() override;
    virtual void DoSend() override;
    virtual void HandleError() override;

    virtual void Run() override;

    SpamProgramm(Socket& socket);
    virtual ~SpamProgramm();

private:
    std::string mRecvBuffer;
    std::string mSendBuffer;

    Socket mSocket;
};

#endif
