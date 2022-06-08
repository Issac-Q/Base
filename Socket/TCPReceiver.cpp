#include "TCPReceiver.h"
#include "CaptureLog.h"

TCPReceiver::TCPReceiver()
: mMsgExtractor(TCP_BUF_LEN, this)
{

}

TCPReceiver::~TCPReceiver()
{

}

void TCPReceiver::onConnected(int boardId)
{
    mMsgExtractor.reset();
    onTCPConnected(boardId);
}

void TCPReceiver::onDataReceived(int boardId, const void* data, int len)
{
    LOG_SOCKET_D("TCPReceiver::onDataReceived:%d\n", boardId);
    mMsgExtractor.startExtract(data, (uint64_t)len);
}

void TCPReceiver::onDisConnected(int boardId)
{
    onTCPDisConnected(boardId);
}