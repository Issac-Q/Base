#include "TCPServer.h"
#include "SocketRecvThread.h"
#include "CaptureLog.h"

TCPServer::TCPServer()
{

}

TCPServer::~TCPServer()
{

}

bool TCPServer::initSocket(int port, const char* ip)
{
    //1. create a socket
    if ((mFd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        LOG_SOCKET_E("[server] create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return false;
    }

    //2. set address
    mAddr.sin_family = AF_INET;
    mAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    mAddr.sin_port = htons(port);

    int on = 1;
    if((setsockopt(mFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
		perror("setsockopt SO_REUSEADDR failed");
	}

    //3. bind socket with address
    if (bind(mFd, (struct sockaddr*)&mAddr, sizeof(mAddr)) < 0) {
        LOG_SOCKET_E("[server] bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        return false;
    }
    return true;
}

void TCPServer::startSocket(const char *name)
{
    //4. listen the willing accesser
    if (listen(mFd, CONNECT_NUM) < 0){
        LOG_SOCKET_E("[server] listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        return;
    }
    TCP::startSocket("TCPServer");
}

void TCPServer::run()
{
    int connectFd = acceptConnect();
    if (connectFd < 0) {
        sleep(1);
    }
    else {
        LOG_SOCKET_E("[Server] accept a new connection:%d, threadId:%ud\n", connectFd, pthread_self());
        startReceiveData(connectFd, TCP_BUF_LEN, mConnectFds);
        // Thread::stopThread();
    }
}

int TCPServer::acceptConnect()
{
    socklen_t addrLen = sizeof(mAddr);
    int connectFd = accept(mFd, (struct sockaddr*)&mAddr, &addrLen);
    if (connectFd < 0) {
        LOG_SOCKET_E("[Server] accept socket error: %s(errno: %d)\n",strerror(errno),errno);
    }
    return connectFd;
}

void TCPServer::identifyAccesser(int fd)
{
    int tryCnt = 10;
    bool identifyRst = false;
    LOG_SOCKET_D("identifyAccesser\n");
    while (!identifyRst) {
        int recvLen =  receiveData(fd);
        if (mConnectFds.find(fd) == mConnectFds.end()) {
            LOG_SOCKET_I("[Server] client shutdowned when identifyAccesser\n");
            return;
        }
        ConnectInfo& conInfo = mConnectFds[fd];
        // int a = (int)conInfo.buf[BYTE_LEN];
        // int b = (int)conInfo.buf[BYTE_CONTENT];
        // int c = (int)conInfo.buf[BYTE_ID];
        // LOG_SOCKET_D("[server] identify a:%d, b:%u, c:%d\n", a, b, c);

        if (isCommand(conInfo.buf, CMD_CODE, CMD_CODE_LEN)) {
            conInfo.clientId = conInfo.buf[BYTE_ID];
            LOG_SOCKET_D("%d, %u\n", conInfo.buf[BYTE_CMD] & CMD_IDENTIFY, *(unsigned short*)(conInfo.buf + BYTE_LEN) );
            if (conInfo.buf[BYTE_CMD] & CMD_IDENTIFY &&
                *(unsigned short*)(conInfo.buf + BYTE_LEN) == CMD_DEAULT_LEN) {
                identifyRst = true;
                LOG_SOCKET_I("[server] recv identify connectFD:%d, clientID:%d\n", fd, conInfo.clientId); 
                while (TCP::sendData(conInfo.buf, CMD_DEAULT_LEN, fd) < 0) {
                    sleep(1);
                }
                if (mConnectListeners.find(conInfo.clientId) != mConnectListeners.end()) {
                    mConnectListeners.at(conInfo.clientId)->onConnected(conInfo.clientId);
                }
                mConnectIds[conInfo.clientId] = fd;
                LOG_SOCKET_I("[server] identify success connectFD:%d, clientID:%d\n", fd, conInfo.clientId);                
            }
        }
        else {
            LOG_SOCKET_E("error code\n");
        }
        // if (recvLen == CMD_PACKET_LEN && 
        //     conInfo.buf[BYTE_LEN] == CMD_PACKET_LEN &&
        //     conInfo.buf[BYTE_CONTENT] & CMD_IDENTIFY) 
        // {
        //     identifyRst = true;
        //     conInfo.clientId = (int)conInfo.buf[BYTE_ID];
        //     while (TCP::sendData(conInfo.buf, CMD_PACKET_LEN, fd) < 0) {
        //         sleep(1);
        //     }
        //     if (mConnectListeners.find(conInfo.clientId) != mConnectListeners.end()) {
        //         mConnectListeners.at(conInfo.clientId)->onConnected();
        //     }
        //     LOG_SOCKET_E("[server] identify success connectFD:%d, clientID:%d\n", fd, conInfo.clientId);
        // }
    }
}

int TCPServer::sendData(const void* data, int len, int target)
{
    int targetFD;
    if (mConnectIds.find(target) == mConnectIds.end()) {
        LOG_SOCKET_E("[Server] sendData:board:%d is not connected\n", target);
        return -1;
    }
    else {
        targetFD = mConnectIds.at(target);
    }

    return TCP::sendData(data, len, targetFD);
}

// int TCPServer::receiveData(int targetFd)
// {
//     char* data = mConnectFds[targetFd].buf;
//     ssize_t recvLen = recv(targetFd, data, TCP_BUF_LEN, 0); //might block
//     if (recvLen < 0) {
//         LOG_SOCKET_E("[server] recv msg error: %s(errno: %d) threadId:%u\n", strerror(errno), errno, pthread_self());
//         sleep(1);
//     }
//     else if (recvLen == 0) {
//         if (mConnectFds[targetFd].connectShutdown) {
//             LOG_SOCKET_E("[server] client shutdown end\n");
//             shutdownConnect(targetFd, mConnectFds);
//             close(targetFd);
//         }
//         else {
//             LOG_SOCKET_E("[server] client shutdown start\n");
//             mMutex.lock();
//             mConnectFds[targetFd].connectShutdown = true;
//             mMutex.lock();
//         }
//     }
//     return recvLen;
// }


// void TCPServer::receiveData()
// {
//     char buf[TCP_BUF_LEN];
//     long recvLen;
//     int connectFd = -1;    
//     unsigned long long totalLen = 0;
//     bool next = true;
//     int pNum = 0;
//     socklen_t addrLen = sizeof(mAddr);
//     while (1) {
//         if (connectFd < 0) {
//             connectFd = accept(mServerFd, (struct sockaddr*)&mAddr, &addrLen);
//             if (connectFd < 0) {
//                 printf("[Server] accept socket error: %s(errno: %d)",strerror(errno),errno);
//                 sleep(1);
//                 continue;
//             }
//             printf("[Server] receive a connection\n");
//         }

//         struct timespec now, end;
//         if (next) {        
//             clock_gettime(CLOCK_MONOTONIC, &now); 
//         }   
//         recvLen = recv(connectFd, buf, sizeof(buf), 0);
//         next = false;

//         if (recvLen < 0) {
//             printf("[Server] recv msg error: %s(errno: %d)\n", strerror(errno), errno);
//             sleep(1);
//         }
//         else if (recvLen == 0) {
//             // printf("[Server] receive a disconnection\n\n");
//             // close(connectFd);
//             // connectFd = -1;            
//             // break;
//             printf("pNum:%d\n", pNum);
//             sleep(1);
//         }
//         else {
//             if (recvLen > sizeof(int)) {
//                 if (*((int*)buf) == 1234) {
//                     // printf("1234\n");
//                     // printf("+++++++++++++++++totalLen:%d\n", totalLen);
//                 //     totalLen = 0;
//                 //     next = true;
//                 //     clock_gettime(CLOCK_MONOTONIC, &end);        
//                 //     printf("[Server] ===============ms:%d\n", end.tv_sec * 1000 + end.tv_nsec / 1000000 - now.tv_sec * 1000 - now.tv_nsec / 1000000);                    
//                 }
//                 totalLen += recvLen;
//                 printf("totalLen:%ld, recvLen:%d\n", totalLen, recvLen);
//                 if (totalLen > (TCP_BUF_LEN + 4)) {
//                     int offset = totalLen % (TCP_BUF_LEN + 4);
//                     if (recvLen >= offset && offset != 0) {
//                         int cmd = *(int*)(buf + recvLen - offset);
//                         ++pNum;        
//                         printf("cmd***************:%d, pNum:%d\n", cmd, pNum);    
//                         // sleep(1);
                                    
//                     }                
//                 }
//                 else if (totalLen == recvLen) {
//                     int cmd = *((int*)buf);
//                     ++pNum;        
//                     printf("cmd***************:%d, pNum:%d\n", cmd, pNum);
//                     // sleep(1);
//                 }
//                 // printf("totalLen:%d\n", totalLen);
//                 // printf("[Server] recv msg success:cmd:%d, data:%s, lenght:%d\n", *((int*)buf), buf + sizeof(int), recvLen);
//                 if (*((int*)buf) == 1234) {
//                     // printf("1234\n");
//                 //     totalLen = 0;
//                 //     next = true;
//                 //     clock_gettime(CLOCK_MONOTONIC, &end);        
//                 //     printf("[Server] ===============ms:%d\n", end.tv_sec * 1000 + end.tv_nsec / 1000000 - now.tv_sec * 1000 - now.tv_nsec / 1000000);                    
//                 }
//                 // if (totalLen == (1280*962*2 + 4)) {
//                 //     totalLen = 0;
//                 //     next = true;
//                 //     clock_gettime(CLOCK_MONOTONIC, &end);        
//                 //     printf("[Server] ===============ms:%d\n", end.tv_sec * 1000 + end.tv_nsec / 1000000 - now.tv_sec * 1000 - now.tv_nsec / 1000000);
//                 // }                
//             }
//             else {
//                 printf("[Server] recv msg success:cmd:%d, lenght:%d\n", *((int*)buf), recvLen);
//             }
//             // if ((recvLen = send(connectFd, (char*)buf, recvLen, 0))  < 0) {
//             //     printf("[Server] reply msg error: %s(errno: %d)\n", strerror(errno), errno);
//             //     sleep(1);
//             // }
//             // else {
//             //     printf("[Server] reply msg success:%d, lenght:%d\n", *((int*)buf), recvLen);
//             //     // if (mLister) {
//             //     //     mLister->onStateChanged();
//             //     // }
//             // }
//         }
//     }
//     if (connectFd > -1) {
//         close(connectFd);
//     }    
//     close(mServerFd);
// }