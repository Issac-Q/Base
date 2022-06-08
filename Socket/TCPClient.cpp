#include "TCPClient.h"
#include "SocketRecvThread.h"
#include <signal.h>
#include "CaptureLog.h"

TCPClient::TCPClient()
: mExiting(false)
, mIdentifierFailed(false)
, mIdentifierCnt(0)
{
    sem_init(&mSemph, 0, 0);
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    sigprocmask(SIG_BLOCK, &set, NULL);

    unsigned int cmdCode = CMD_CODE;
    memcpy((void*)mIdentifyBuf, &cmdCode, CMD_CODE_LEN);
    mIdentifyBuf[BYTE_ID]  = mId;
    mIdentifyBuf[BYTE_CMD] = (char)CMD_IDENTIFY;
    mIdentifyBuf[BYTE_LEN] = CMD_DEAULT_LEN;    
}

TCPClient::TCPClient(char id)
: TCP(id)
, mExiting(false)
, mIdentifierFailed(false)
, mIdentifierCnt(0)
{
    sem_init(&mSemph, 0, 0);
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    sigprocmask(SIG_BLOCK, &set, NULL);

    unsigned int cmdCode = CMD_CODE;
    memcpy((void*)mIdentifyBuf, &cmdCode, CMD_CODE_LEN);
    mIdentifyBuf[BYTE_ID]  = mId;
    mIdentifyBuf[BYTE_CMD] = (char)CMD_IDENTIFY;
    mIdentifyBuf[BYTE_LEN] = CMD_DEAULT_LEN; 
}

TCPClient::~TCPClient()
{
    stopSocket();
    sem_destroy(&mSemph);
}

bool TCPClient::createSocket(int* fd)
{
    if ((*fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LOG_SOCKET_E("[client] create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return false;
    }
    return true;
}

bool TCPClient::initSocket(int port, const char* ip)
{
    if (!createSocket(&mFd)) {
        LOG_SOCKET_E("[client] create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return false;
    }
    
    mAddr.sin_family = AF_INET;
    mAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &mAddr.sin_addr) <= 0){
        LOG_SOCKET_E("[client] inet_pton error for ip:%s, port:%d\n", ip, port);
        return false;
    }

    return true;
}


void TCPClient::startSocket(const char *name)
{    
    TCP::startSocket("TCPClient");
}

void TCPClient::run()
{
    if (mExiting) {
        return;
    }

    if (!requestConnect()) {
        sleep(1);
    }
    else {
        mIdentifierFailed = false;
        mIdentifierCnt = 0;
        startReceiveData(mFd, TCP_BUF_LEN, mConnectFds, false);
        while (sendIdentifier() < 0 && !mIdentifierFailed) {
            LOG_SOCKET_I("try sendIdentifier\n");
            sleep(1);
        }

        if (!mIdentifierFailed) {
            mConnectFds[mFd].recvThread->startRecv(mFd);
            //connected success
            if (mConnectListeners.find(mId) != mConnectListeners.end()) {
                mConnectListeners.at(mId)->onConnected(mId);
            }
            //connect thread block wait to wake by the receive thread
            int semVal;
            do {
                sem_getvalue(&mSemph, &semVal);
                sem_wait(&mSemph);
            }while(semVal > 0);
        }
        else if (mConnectFds.find(mFd) != mConnectFds.end()) {
            LOG_SOCKET_D("shutdown the connection to unknow server\n");
            shutdownConnect(mFd, mConnectFds);
        }
        
        if (mConnectFds.find(mFd) == mConnectFds.end()) {
            LOG_SOCKET_D("[client] recreate socket\n");
            createSocket(&mFd);
        }
    }
}

bool TCPClient::requestConnect()
{
    if (connect(mFd, (struct sockaddr*)&mAddr, sizeof(mAddr)) < 0) {
        LOG_SOCKET_E("[client] connect error: %s(errno: %d)\n", strerror(errno), errno);
        return false;
    }
    LOG_SOCKET_I("[client] connect success\n");    
    return true;
}

int TCPClient::sendIdentifier()
{
    if (sendData(mIdentifyBuf, CMD_DEAULT_LEN) < 0) {
        return -1;
    }
    int recvLen = receiveData(mFd);
    if (mIdentifierFailed) {
        return -1;
    }

    if (recvLen > 0) {
        char buf[CMD_DEAULT_LEN];
        ConnectInfo& conInfo = mConnectFds[mFd];
        for (int i = 0; i < CMD_DEAULT_LEN; i++) {
            if (conInfo.buf[i] != mIdentifyBuf[i]) {
                return recvLen;
            }
        }
        LOG_SOCKET_I("[client] sendIdentifier success Id:%d\n", mId);
    }
    else if (errno == EAGAIN || errno == EWOULDBLOCK) {
        //sem_post(&mSemph);
        mIdentifierFailed = true;  
    }
    //send success and not receive the correct replay
    if (mIdentifierCnt++ > 10) {
        //sem_post(&mSemph);
        mIdentifierFailed = true;
    }
    return recvLen;
}

int TCPClient::sendData(const void* data, int len, int target)
{
    return TCP::sendData(data, len, mFd);
}

int TCPClient::receiveData(int targetFd)
{
    ssize_t recvLen = TCP::receiveData(targetFd);
    if ((recvLen == 0 || recvLen == -10) && mConnectFds.find(targetFd) == mConnectFds.end()) {
        sem_post(&mSemph);
        mIdentifierFailed = true;
    }
    return recvLen;
}

void TCPClient::stopSocket()
{
    int semVal;
    sem_getvalue(&mSemph, &semVal);
    if (semVal < 1) {
        sem_post(&mSemph);
        mExiting = true;
    }
    TCP::stopSocket();
}
