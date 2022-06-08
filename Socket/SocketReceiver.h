#ifndef SOCKET_RECEIVER_H
#define SOCKET_RECEIVER_H

#include "Socket.h"

class SocketReceiver : public SocketRecvListener
{
public:
    SocketReceiver();
    ~SocketReceiver();

    virtual void onDataReceived(int boardId, void* data, int len);
};

#endif