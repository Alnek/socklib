#include "processmanager.h"

#include "console.h"

#include <windows.h>

ProcessManager& ProcessManager::GetInstance()
{
    static ProcessManager sInstance;
    return sInstance;
}

ProcessManager::ProcessManager()
{
    mFrameCounter = mSkipCounter = mFPS = 0;
    mStartTick = GetTickCount64();

    mRunnables.reserve(1024);
}

void ProcessManager::Register(Runnable* runnable)
{
    mRunnables.push_back(runnable);
}

void ProcessManager::Unregister(Runnable* runnable)
{
    auto it = std::find(mRunnables.begin(), mRunnables.end(), runnable);
    if (it != mRunnables.end()) mRunnables.erase(it);
}

void ProcessManager::Run(uint32_t fps)
{
    uint64_t curTick = GetTickCount64();
    uint64_t delta = curTick - mStartTick;

    mFPS = mFPS/2 + 500 * mFrameCounter / (delta + 1);
    if (mFPS >= fps)
    {
        Sleep(0);
        mSkipCounter++;
        return;
    }

    for (auto it = mRunnables.begin(); it != mRunnables.end(); ++it)
    {
        (*it)->Run();
    }

    if (delta >= 1000)
    {
        //LOG("FPS:" << mFPS << " Frames:" << mFrameCounter << " Skipped:" << mSkipCounter);
        mSkipCounter = mFrameCounter = 0;
        mStartTick = curTick;
    }
    else
    {
        mFrameCounter++;
    }
}
