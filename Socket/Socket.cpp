#include "Socket.h"
#include "SocketRecvThread.h"
#include "CaptureLog.h"


Socket::Socket()
: mId(0)
, mFd(-1)
, mConnectListeners()
{
    memset(&mAddr, 0, sizeof(mAddr));
}

Socket::Socket(char id)
: mId(id)
, mFd(-1)
, mConnectListeners()
{
    memset(&mAddr, 0, sizeof(mAddr));
}


Socket::~Socket()
{

}

int Socket::getSendBufSize()
{
    int opt_val;
    socklen_t opt_len = sizeof(opt_val);
    if (getsockopt(mFd, SOL_SOCKET, SO_SNDBUF, &opt_val, &opt_len) < 0) {
        LOG_SOCKET_E("fail to getsockopt SO_SNDBUF");
    }
    LOG_SOCKET_D("SO_SNDBUF:%dM\n", opt_val / 1000);
    return opt_val;
}

int Socket::getRecvBufSize()
{
    int opt_val;
    socklen_t opt_len = sizeof(opt_val);
    if (getsockopt(mFd, SOL_SOCKET, SO_RCVBUF, &opt_val, &opt_len) < 0) {
        LOG_SOCKET_E("fail to getsockopt SO_RCVBUF");
    }
    LOG_SOCKET_D("SO_RCVBUF:%dM\n", opt_val / 1000);
    return opt_val;
}

bool Socket::setSendBufSize(int size)
{
    int realSize = size * 1000;
    if(setsockopt(mFd, SOL_SOCKET, SO_SNDBUF, &realSize, sizeof(int)) < 0) {
        LOG_SOCKET_E("fail to setsockopt SO_SNDBUF size:%dM\n", realSize);
    }
}

bool Socket::setRecvBufSize(int size)
{
    int realSize = size * 1000;
    if(setsockopt(mFd, SOL_SOCKET, SO_RCVBUF, &realSize, sizeof(int)) < 0) {
        LOG_SOCKET_E("fail to setsockopt SO_RCVBUF size:%dM\n", realSize);
    }    
}

bool Socket::setSendTimeout(unsigned int ms)
{
    struct timeval tv;
    int ret;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    if (setsockopt(mFd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv) )< 0){
        LOG_SOCKET_E("socket option  SO_SNDTIMEO not support\n");
        return false;
    }
    return true;
}

bool Socket::setRecvTimeout(unsigned int ms)
{
    struct timeval tv;
    int ret;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    if( setsockopt(mFd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv))<0){
        LOG_SOCKET_E("socket option  SO_RCVTIMEO not support\n");
        return false;
    }
    return true;
}

void Socket::setRecvListener(int id, SocketRecvListener* recvListener)
{
    mConnectListeners[id] = recvListener;
}

void Socket::setRecvListener(SocketRecvListener* recvListener)
{
    setRecvListener(mId, recvListener);
}

void Socket::startSocket(const char *name)
{    
    Thread::startThread("Socket");
}

void Socket::packetCommand(char cmd, char id, char* data, int len, char* packet)
{
    unsigned int cmdCode = CMD_CODE;
    memcpy((void*)packet, &cmdCode, CMD_CODE_LEN);
    packet[BYTE_ID]  = id;
    packet[BYTE_CMD] = cmd;
    packet[BYTE_LEN] = CMD_DEAULT_LEN + len;
    if (data && len > 0) {
        memcpy((void*)(packet + BYTE_CUSTOM_START), data, len);
    }    
}

bool Socket::sendCommand(char cmd, char* data, int len, int target)
{
    int packetLen = CMD_DEAULT_LEN + len;
    char buf[packetLen];
    packetCommand(cmd, target, data, len, buf);
    int tryTimes = 10;
    while (tryTimes-- > 0 && sendData(buf, packetLen, target) < 1) {
        LOG_SOCKET_D("Socket::sendCommand: boardId:%d, cmd:%d failed!!\n", target, cmd);
        sleep(1);
    }
    if (tryTimes < 0) {
        LOG_SOCKET_D("Socket::sendCommand: failed after try 10 times, please check the network\n", target, cmd);
    }
    else {
        LOG_SOCKET_D("Socket::sendCommand: boardId:%d, cmd:%d\n", target, cmd);
    }
}

bool Socket::isCommand(char *buf, unsigned int cmdCode, int cmdLen)
{
    for (int i = 0; i < cmdLen; i++) {
        if (!(buf[i] & (cmdCode >> i))) {
            return false;
        }
    }
    return true;
}

void Socket::stopSocket()
{
    Thread::terminate();
}