#include "TCP.h"
#include "SocketRecvThread.h"
#include "CaptureLog.h"

TCP::TCP()
: Socket()
, mThreadCacheNum(0)
{

}

TCP::TCP(char id)
: Socket(id)
, mThreadCacheNum(0)
{

}

TCP::~TCP()
{
    stopSocket();
    for (int i = 0; i < mThreadCacheNum; i++) {
        delete mThreadCaches[i];
        mThreadCaches[i] = nullptr;
    }
}

void TCP::startReceiveData(int fd, int len, std::map<int, ConnectInfo>& connectFds, bool startThread)
{
    if (connectFds.find(fd) == connectFds.end()) {
        ConnectInfo conInfo;
        conInfo.clientId = mId;
        if (mThreadCacheNum > 0) {      
            conInfo.recvThread = mThreadCaches[--mThreadCacheNum];
            LOG_SOCKET_D("reuse mThreadCacheNum:%d, thread:%p\n", mThreadCacheNum + 1, conInfo.recvThread);
        }
        else {
            conInfo.recvThread = new SocketRecvThread(this);
            LOG_SOCKET_D("new mThreadCacheNum:%d, thread:%p\n", mThreadCacheNum, conInfo.recvThread);
        }        
        conInfo.buf = (char*)malloc(len);
        conInfo.connectShutdown = false;
        connectFds[fd] = conInfo;
        if (startThread) {
            conInfo.recvThread->startRecv(fd);
        }
    }
}

int TCP::sendData(const void* data, int len, int target)
{
    if (!data) {
        LOG_SOCKET_E("TCP::sendData: data is NULL\n");
        return -1;
    }

    mMutex.lock();
    ssize_t sendLen = -1;
    if (mConnectFds.find(target) != mConnectFds.end() && 
        !mConnectFds.at(target).connectShutdown) {
        sendLen = send(target, data, len, 0); //might block
        if (sendLen < 0) {
            LOG_SOCKET_E("TCP::sendData send msg error: %s(errno: %d)\n", strerror(errno), errno);
        }
    }
    else {
        LOG_SOCKET_E("TCP::sendData target:%d is not connected\n", target);
        sleep(1);
    }
    mMutex.unlock();
    return sendLen;
}

int TCP::receiveData(int targetFd)
{
    char* data = mConnectFds[targetFd].buf;
    ssize_t recvLen = recv(targetFd, data, TCP_BUF_LEN, 0); //might block
    mConnectFds[targetFd].len = recvLen;
    if (recvLen < 0) {
        LOG_SOCKET_E("TCP::receiveData recv msg error: %s(errno: %d) threadId:%u\n", strerror(errno), errno, pthread_self());
        if (errno != 104) {
            sleep(1);
        }
    }
    
    if (recvLen == 0 || errno == 104) {
        if (errno == 104) {
            mMutex.lock();
            mConnectFds[targetFd].connectShutdown = true;
            recvLen = -10;
            usleep(100 * 1000);
            mMutex.unlock();
        }
        if (mConnectFds[targetFd].connectShutdown) {
            LOG_SOCKET_W("TCP::receiveData client shutdown end\n");
            int clientId = mConnectFds[targetFd].clientId;
            shutdownConnect(targetFd, mConnectFds);
            close(targetFd);
            if (mConnectListeners.find(clientId) != mConnectListeners.end()) {
                mConnectListeners.at(clientId)->onDisConnected(clientId);
            }
        }
        else {
            LOG_SOCKET_W("TCP::receiveData client shutdown start\n");
            mMutex.lock();
            mConnectFds[targetFd].connectShutdown = true;
            mMutex.unlock();
        }
    }
    return recvLen;
}

void TCP::dataReceived(int fd)
{
    ConnectInfo& conInfo = mConnectFds[fd];
    if (mConnectListeners.find(conInfo.clientId) != mConnectListeners.end()) {
        mConnectListeners.at(conInfo.clientId)->onDataReceived(conInfo.clientId, conInfo.buf, conInfo.len);
    }
    else {
        LOG_SOCKET_D("TCP::dataReceived: mRecvListener board:%d is NULL\n", conInfo.clientId);
    }
}

void TCP::shutdownConnect(int fd, std::map<int, ConnectInfo>& connectFds)
{
    if (connectFds.find(fd) != connectFds.end()) {
        const ConnectInfo& conInfo = connectFds[fd];
        conInfo.recvThread->stopRecv();
        if (mThreadCacheNum < CACHE_THREAD_MAX) {
            mThreadCaches[mThreadCacheNum++] = conInfo.recvThread;
            LOG_SOCKET_D("cache mThreadCacheNum:%d, thread:%p\n", mThreadCacheNum, conInfo.recvThread);
        }
        else {
            delete conInfo.recvThread;
        }        
        free(conInfo.buf);
        connectFds.erase(fd);
        if (mConnectIds.find(conInfo.clientId) != mConnectIds.end()) {
            mConnectIds.erase(conInfo.clientId);
        }
    }
}

void TCP::stopSocket()
{
    if (mFd < 0 && mConnectFds.size() == 0) {
        return;
    }

    bool removeFd = (mFd >=0 && (mConnectFds.find(mFd) == mConnectFds.cend()));
    for (auto it = mConnectFds.cbegin(); it != mConnectFds.cend(); ++it) {
        shutdownConnect(it->first, mConnectFds);
        close(it->first);
    }
    
    if (removeFd) {
        close(mFd);
    }
    mFd = -1;
    Socket::stopThread();
}