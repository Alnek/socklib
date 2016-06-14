#ifndef __SPAM_PROGRAMM_H__
#define __SPAM_PROGRAMM_H__

#include "processmanager.h"
#include "socketcallback.h"

#include <vector>

class SpamProgramm
    : public SocketCallback
    , private Runnable
{
public:
    virtual void CanWrite(Socket socket) override;
    virtual void CanRead(Socket socket) override;
    virtual void ExFunc(Socket socket) override;

    virtual void Run() override;

    SpamProgramm();
    virtual ~SpamProgramm();

private:
    std::vector<char> mRecvBuffer;
    std::vector<char> mSendBuffer;
};

#endif
