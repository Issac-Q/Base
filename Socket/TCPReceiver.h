#ifndef TCP_RECEIVER_H
#define TCP_RECEIVER_H

#include <inttypes.h>
#include "Socket.h"
#include "TCPMsgExtractor.h"

class TCPReceiver : public SocketRecvListener
{
    friend class TCPMsgExtractor;
public:
    TCPReceiver();
    virtual ~TCPReceiver();
    
    virtual void onConnected(int boardId);
    virtual void onDataReceived(int boardId, const void* data, int len) final;
    virtual void onDisConnected(int boardId);

protected:
    virtual void onTCPConnected(int boardId) {};
    virtual void onTCPMsgReceive(const void* data, uint64_t len) = 0;
    virtual void onTCPDisConnected(int boardId) {};

private:
    TCPMsgExtractor mMsgExtractor;
};

#endif