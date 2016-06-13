#ifndef __INSTANCE_MANAGER_H__
#define __INSTANCE_MANAGER_H__

#include "sock.h"

#include <vector>

class InstanceManager
{
public:
    static InstanceManager& GetInstance();

    void StartProgramm(Socket& socket);
    void StopProgramm(Socket& socket);
private:

};

#endif
