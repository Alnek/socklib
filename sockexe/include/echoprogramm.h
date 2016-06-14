#ifndef __ECHO_PROGRAMM_H__
#define __ECHO_PROGRAMM_H__

#include "socketcallback.h"

#include <vector>

class EchoProgramm
    : public SocketCallback
{
public:
    virtual void CanWrite(Socket socket) override;
    virtual void CanRead(Socket socket) override;
    virtual void ExFunc(Socket socket) override;

    EchoProgramm();
    virtual ~EchoProgramm();

private:
    std::vector<char> mRecvBuffer;
    std::vector<char> mSendBuffer;
};

#endif
