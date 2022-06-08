#include "SocketRecvThread.h"

SocketRecvThread::SocketRecvThread(Socket* socket)
: mSocket(socket)
{

}

SocketRecvThread::~SocketRecvThread()
{

}

void SocketRecvThread::startRecv(int fd)
{
    mRecvFd = fd;
    Thread::startThread("SocketRecvThread");
}

void SocketRecvThread::stopRecv()
{
    // printf("stop2:%u\n", pthread_self());
    Thread::stopThread();
    // Thread::terminate();
}

void SocketRecvThread::preRun()
{
    mSocket->identifyAccesser(mRecvFd);
}

void SocketRecvThread::run()
{
    int recvLen = mSocket->receiveData(mRecvFd);
    if((recvLen) > 0) {
        mSocket->dataReceived(mRecvFd);
    }
}