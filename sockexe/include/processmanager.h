#ifndef __PROCESS_MANAGER_H__
#define __PROCESS_MANAGER_H__

#include <vector>

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

    void Run();
private:
    ProcessManager();

    std::vector<Runnable*> mRunnables;
};

#endif
