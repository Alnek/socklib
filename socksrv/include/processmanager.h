#ifndef __PROCESS_MANAGER_H__
#define __PROCESS_MANAGER_H__

#include "lockobject.h"
#include <vector>

class ProcessManager;

class Runnable
{
public:
    virtual void Run() = 0;

    virtual ~Runnable() {};
};

class ProcessManager
{
public:
    static ProcessManager& GetInstance();

    void Register(Runnable* runnable, uint64_t threadId);
    void Unregister(Runnable* runnable, uint64_t threadId);

    void Run(uint32_t fps);
    void Join();
    void UpdateJoinTID();
private:
    ProcessManager();

    std::vector<Runnable*> mRunnables;
    
    uint64_t mFrameCounter;
    uint64_t mSkipCounter;
    uint64_t mStartTick;
    uint64_t mFPS;

    LockObject mLock;
    std::vector<Runnable*> mAddRunnables;
    uint64_t mJoinTID;
};

#endif
