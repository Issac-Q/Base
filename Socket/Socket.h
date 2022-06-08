#ifndef SOCKET_H
#define SOCKET_H

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
#include "Thread.h"

#define CONNECT_NUM 10
#define TCP_BUF_LEN (2048)
#define UDP_BUF_LEN (65000)
//CMD format _ _ _ _ _ _ _ _
//-----------0 1 2 3 4 5 6 7
//0~3-0xCCDDEEFF
//4  -CMD_CLIENT_ID
//5  -CMD_CONTENT
//6~7-CMD_PACKET_LEN    //max length 2^16 = 65536
#define CMD_CODE          0xCCDDEEFF
#define CMD_CODE_LEN      4
#define CMD_DEAULT_LEN    8
#define BYTE_ID           CMD_CODE_LEN
#define BYTE_CMD          (BYTE_ID + 1)
#define BYTE_LEN          (BYTE_CMD + 1)
#define BYTE_CUSTOM_START CMD_DEAULT_LEN


#define CMD_IDENTIFY      0xFF

class SocketRecvThread;

class SocketRecvListener {
public:
    //for server: boardId is the client you communicate to
    //for client: boardId is the id itself
    virtual void onConnected(int boardId) {};
    virtual void onDataReceived(int boardId, const void* data, int len) = 0;
    virtual void onDisConnected(int board) {};
};

//functions that tcp and udp all need(different) are pure virtual
//functions that tcp and udp all need(same) are normal 
//functions only one need are viatual default implement
class Socket : public Thread
{
public:
    friend class SocketRecvThread;

    Socket();
    Socket(char id);
    virtual ~Socket();

    int getSendBufSize();
    int getRecvBufSize();
    bool setSendBufSize(int size);
    bool setRecvBufSize(int size);
    bool setSendTimeout(unsigned int ms);
    bool setRecvTimeout(unsigned int ms);

    void setRecvListener(int id, SocketRecvListener* recvListener);
    void setRecvListener(SocketRecvListener* recvListener);
    
    virtual bool initSocket(int port, const char* ip = nullptr) = 0;
    virtual void startSocket(const char *name = nullptr);    
    virtual int sendData(const void* data, int len, int target = 0) = 0;
    bool sendCommand(char cmd, char* data, int len, int target = 0);
    virtual void stopSocket();

protected:
    bool isCommand(char *buf, unsigned int cmdCode, int cmdLen);
    void packetCommand(char cmd, char id, char* data, int len, char* packet);

    virtual void identifyAccesser(int fd) {};
    virtual int receiveData(int targetFd = -1) {};
    virtual void dataReceived(int fd) {};

    virtual void run() = 0;

    char mId;   //for server is 0
    int mFd;
    struct sockaddr_in mAddr;
    // boardId->SocketListener
    std::map<int, SocketRecvListener*> mConnectListeners;
    std::mutex mMutex;
};

#endif