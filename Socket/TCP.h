#ifndef TCP_H
#define TCP_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <mutex>
#include "Socket.h"

#define CACHE_THREAD_MAX 10

typedef struct
{
    int clientId;
    int len;
    char* buf;
    SocketRecvThread* recvThread;
    bool connectShutdown;
}ConnectInfo;

class TCP : public Socket
{
public:
    TCP();
    TCP(char id);
    virtual ~TCP();

    virtual bool initSocket(int port, const char* ip = nullptr) = 0;
    virtual int sendData(const void* data, int len, int target = 0);
    virtual void stopSocket();

protected:
    void startReceiveData(int fd, int len, std::map<int, ConnectInfo>& connectFds, bool startThread = true); 

    virtual bool requestConnect() {};
    virtual int acceptConnect() {};
    virtual void identifyAccesser(int fd) {};
    virtual int receiveData(int targetFd = -1);
    virtual void shutdownConnect(int fd, std::map<int, ConnectInfo>& connectFds);
    
    virtual void dataReceived(int fd);

    virtual void run() = 0;

    // char mId;    
    int mThreadCacheNum;
    SocketRecvThread* mThreadCaches[CACHE_THREAD_MAX];
    // boardId->connId
    std::map<int, int> mConnectIds;
    // connId->ConnInfo
    std::map<int, ConnectInfo> mConnectFds;
    // std::mutex mMutex;
};

#endif