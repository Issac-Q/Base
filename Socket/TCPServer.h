#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "TCP.h"
#include "Thread.h"
#include "SocketRecvThread.h"
#include <map>
#include <vector>

class TCPServer : public TCP
{
public:
    TCPServer();
    virtual ~TCPServer();

    virtual bool initSocket(int port, const char* ip = nullptr);
    virtual void startSocket(const char *name = nullptr);
    virtual int sendData(const void* data, int len, int target = 0);

protected:
    virtual int acceptConnect();
    virtual void identifyAccesser(int fd);

    virtual void run();

private:
    bool mListened;
};

#endif