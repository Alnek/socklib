#ifndef __ECHO_PROGRAMM_H__
#define __ECHO_PROGRAMM_H__

#include "sock.h"

#include "processmanager.h"

#include <vector>

class EchoProgramm
    : public SocketCallback
    , private Runnable
{
public:
    virtual void CanWrite(Socket socket) override;
    virtual void CanRead(Socket socket) override;
    virtual void ExFunc(Socket socket) override;

    virtual void Run() override;

    EchoProgramm();
    virtual ~EchoProgramm();

private:
    std::vector<char> mRecvBuffer;
    std::vector<char> mSendBuffer;
};

#endif
