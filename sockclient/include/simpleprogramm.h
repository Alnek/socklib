#ifndef __SIMPLE_PROGRAMM_H__
#define __SIMPLE_PROGRAMM_H__

#include "socket.h"
#include "socketcallback.h"

#include <string>

class SimpleProgramm
    : public SocketCallback
{
public:
    virtual void DoRecv() override;
    virtual void DoSend() override;
    virtual void HandleError() override;

    SimpleProgramm(Socket& socket);
    virtual ~SimpleProgramm();

    static void Report();
private:
    std::string mRecvBuffer;
    std::string mSendBuffer;

    Socket mSocket;
    static uint64_t sCounter;
    static uint64_t sPrevTick;
};

#endif
