#ifndef __ACCEPT_PROGRAMM_H__
#define __ACCEPT_PROGRAMM_H__

#include "socketcallback.h"

class AcceptProgramm : public SocketCallback
{
public:
    virtual void DoRecv() override;
    //virtual void DoSend() override;
    virtual void HandleError() override;

    AcceptProgramm();
    virtual ~AcceptProgramm();

private:
    Socket mSocket;
};

#endif
