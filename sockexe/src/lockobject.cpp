#include "lockobject.h"

#include <assert.h>

LockObject::LockObject()
{
#ifdef _WIN32
    mMutex = CreateMutexA(nullptr, false, nullptr);
    assert(INVALID_HANDLE_VALUE != mMutex);
#endif
}

LockObject::~LockObject()
{
#ifdef _WIN32
    assert(TRUE == CloseHandle(mMutex));
#endif
}

void LockObject::Lock()
{
#ifdef _WIN32
    assert(WAIT_OBJECT_0 == WaitForSingleObject(mMutex, INFINITE));
#endif
}

void LockObject::Unlock()
{
#ifdef _WIN32
    assert(TRUE == ReleaseMutex(mMutex));
#endif
}
