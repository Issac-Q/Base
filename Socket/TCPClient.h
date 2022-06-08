#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <semaphore.h>
#include "TCP.h"
#include "mutex"

class TCPClient : public TCP
{
public:
    TCPClient();
    TCPClient(char id);
    virtual ~TCPClient();

    virtual bool initSocket(int port, const char* ip = nullptr);
    virtual void startSocket(const char* name = nullptr);
    virtual int sendData(const void* data, int len, int target = 0);

protected:
    virtual bool requestConnect();
    virtual int receiveData(int targetFd = -1);
    virtual void stopSocket();

    virtual void run();

private:
    bool createSocket(int* fd);
    int sendIdentifier();
    
    int mIdentifierCnt;
    bool mIdentifierFailed;
    sem_t mSemph;
    bool mExiting;
    char mIdentifyBuf[CMD_DEAULT_LEN];
};

#endif