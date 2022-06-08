#ifndef SOCKET_RECV_THREAD_H
#define SOCKET_RECV_THREAD_H

#include "Thread.h"
#include "Socket.h"

class SocketRecvThread : public Thread
{
public:
    SocketRecvThread(Socket* socket);
    virtual ~SocketRecvThread();

    void startRecv(int fd);
    void stopRecv();

protected:
    virtual void preRun();
    virtual void run();

private:
    Socket* mSocket;
    int mRecvFd;
};

#endif