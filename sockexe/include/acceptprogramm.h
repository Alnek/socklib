#ifndef __ACCEPT_PROGRAMM_H__
#define __ACCEPT_PROGRAMM_H__

#include "socketcallback.h"

class AcceptProgramm : public SocketCallback
{
public:
    virtual void CanRead(Socket socket) override;
    virtual void ExFunc(Socket socket) override;

    AcceptProgramm();
    virtual ~AcceptProgramm();
};

#endif
