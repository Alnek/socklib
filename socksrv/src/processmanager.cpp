#include "processmanager.h"

#include "connectionmanager.h"
#include "console.h"

#include <cassert>
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
    mAddRunnables.reserve(128);
}

void ProcessManager::Register(Runnable* runnable, uint64_t threadId)
{
    if (threadId == mJoinTID)
    {
        mRunnables.push_back(runnable);
    }
    else
    {
        mLock.Lock();
        mAddRunnables.push_back(runnable);
        mLock.Unlock();
    }
}

void ProcessManager::Unregister(Runnable* runnable, uint64_t threadId)
{
    if (threadId == mJoinTID)
    {
        auto it = std::find(mRunnables.begin(), mRunnables.end(), runnable);
        if (it != mRunnables.end()) mRunnables.erase(it);
    }
    else
    {
        assert(false && "TODO: how to terminate runnable from separate thread??");
    }
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

void ProcessManager::Join()
{
    mLock.Lock();
    const auto cnt = mAddRunnables.size();
    if (0 == cnt)
    {
        mLock.Unlock();
        return;
    }

    std::vector<Runnable*> addBuffer;
    addBuffer.swap(mAddRunnables);
    mLock.Unlock();

    for (auto it = addBuffer.begin(); it != addBuffer.end(); ++it)
    {
        Register(*it, mJoinTID);
    }
    return;
}

void ProcessManager::UpdateJoinTID()
{
    mJoinTID = ConnectionManager::GetTID();
}
