#ifndef __LOCKOBJECT_H__
#define __LOCKOBJECT_H__

#ifdef _WIN32
#include "windows.h"
#define OS_LOCK_OBJECT HANDLE
#else
//TODO
#error
#endif

class LockObject
{
public:
    LockObject();
    ~LockObject();

    void Lock();
    void Unlock();

private:
    OS_LOCK_OBJECT mMutex;
};

#endif
