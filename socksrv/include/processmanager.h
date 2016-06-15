#ifndef __PROCESS_MANAGER_H__
#define __PROCESS_MANAGER_H__

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

    void Register(Runnable* runnable);
    void Unregister(Runnable* runnable);

    void Run(uint32_t fps);
private:
    ProcessManager();

    std::vector<Runnable*> mRunnables;
    
    uint64_t mFrameCounter;
    uint64_t mSkipCounter;
    uint64_t mStartTick;
    uint64_t mFPS;
};

#endif
