#include "processmanager.h"

ProcessManager& ProcessManager::GetInstance()
{
    static ProcessManager sInstance;
    return sInstance;
}

ProcessManager::ProcessManager()
{
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

void ProcessManager::Run()
{
    for (auto it = mRunnables.begin(); it != mRunnables.end(); ++it)
    {
        (*it)->Run();
    }
}
