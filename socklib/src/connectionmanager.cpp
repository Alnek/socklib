#include "connectionmanager.h"

ConnectionManager& ConnectionManager::GetInstance()
{
    static ConnectionManager sInstance;
    return sInstance;
}

ConnectionManager::ConnectionManager()
{
    //
}

ConnectionManager::~ConnectionManager()
{
    //
}
