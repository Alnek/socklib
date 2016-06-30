#ifndef __ACCEPT_PROGRAMM_H__
#define __ACCEPT_PROGRAMM_H__

#include "socketcallback.h"

#include "windows.h"

class AcceptProgramm : public SocketCallback
{
public:
    virtual void DoRecv() override;
    //virtual void DoSend() override;
    virtual void HandleError() override;

    AcceptProgramm(Socket& socket);
    virtual ~AcceptProgramm();

    void Run();
    void Stop();
private:
    static DWORD WINAPI run(LPVOID ptr);

    Socket mSocket;

    DWORD mThreadId;
    HANDLE mThread;
};

#endif
