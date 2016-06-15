#ifndef __INSTANCE_MANAGER_H__
#define __INSTANCE_MANAGER_H__

#include <vector>

class Socket;

class InstanceManager
{
public:
    static InstanceManager& GetInstance();

    void StartProgramm(Socket& socket);
    void StopProgramm(Socket socket);
private:

};

#endif
