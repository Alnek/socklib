#ifndef __CONNECTION_H__
#define __CONNECTION_H__

class ConnectionManager
{
public:
    static ConnectionManager& GetInstance();

private:
    ConnectionManager();
    ~ConnectionManager();
};

#endif
